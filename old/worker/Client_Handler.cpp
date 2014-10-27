/*
Since this is the third time we've seen most of this, I'm going to strip out almost
all of the comments that you've already seen.  That way, you can concentrate on the
new items.
*/

#include <iostream>

#include "ace/High_Res_Timer.h"

#include "Client_Acceptor.h"
#include "Client_Handler.h"
#include "Agent_Messages.h"
#include "Madara_Common.h"

/*
We're going to be registering and unregistering a couple of times.  To make sure that
we use the same flags every time, I've created these handy macros.
*/
#define REGISTER_MASK    ACE_Event_Handler::READ_MASK | ACE_Event_Handler::TIMER_MASK
#define REMOVE_MASK             (ACE_Event_Handler::READ_MASK | ACE_Event_Handler::DONT_CALL)

/*
Our constructor still doesn't really do anything.  We simply initialize the acceptor
pointer to "null" and get our current thread id.  The static self() method of ACE_Thread
will return you a thread id native to your platform.
*/
Client_Handler::Client_Handler (void)
: client_acceptor_(0), creator_(ACE_Thread::self()),
  context_ (0)
  {
  }

Client_Handler::~Client_Handler (void)
  {
  }

/*
Query our acceptor for the concurrency strategy.  Notice that we don't bother
to check that our acceptor pointer is valid.  That is proably a bad idea...
*/
int Client_Handler::concurrency(void)
  {
  return this->client_acceptor()->concurrency();
  }

/*
And here we ask the acceptor about the thread pool.
*/
Thread_Pool * Client_Handler::thread_pool(void)
  {
  return this->client_acceptor()->thread_pool();
  }

/*
The destroy() method hasn't changed since we wrote it back in Tutorial 5.
*/
void Client_Handler::destroy (void)
  {
  this->peer ().close ();

  this->reactor ()->remove_handler (this, REMOVE_MASK );

  delete this;
  }

/*
Back to our open() method.  This is straight out of Tutorial 6.  There's
nothing additional here for the thread-pool implementation.
*/
int Client_Handler::open (void *_acceptor)
  {
  client_acceptor( (Client_Acceptor *) _acceptor );
  context_ = &(client_acceptor()->context ());

  if( concurrency() == Client_Acceptor::thread_per_connection_ )
    {
    return this->activate();
    }

  this->reactor (client_acceptor()->reactor ());

  ACE_INET_Addr addr;

  if (this->peer ().get_remote_addr (addr) == -1)
    {
    return -1;
    }

  if (this->reactor ()->register_handler (this, REGISTER_MASK) == -1)
    {
    ACE_ERROR_RETURN ((LM_ERROR, "(%P|%t) can't register with reactor\n"), -1);
    }

  ACE_DEBUG ((LM_DEBUG, "(%P|%t) connected with %s\n", addr.get_host_name ()));

  return 0;
  }

/*
As mentioned in the header, the typical way to close an object in a threaded
context is to invoke it's close() method.  Since we already have a handle_close()
method built to cleanup after us, we'll just forward the request on to that
object.
*/
int Client_Handler::close(u_long flags)
  {
  this->handle_close(ACE_INVALID_HANDLE,0);

  /*
  After we've taken care of ourselves, call the baseclass method
  to do any other necessary cleanup.
  */
  return inherited::close();
  }

/*
In the open() method, we registered with the reactor and requested to be
notified when there is data to be read.  When the reactor sees that activity
it will invoke this handle_input() method on us.  As I mentioned, the _handle
parameter isn't useful to us but it narrows the list of methods the reactor
has to worry about and the list of possible virtual functions we would have
to override.

You've read that much before...  Now we have to do some extra stuff in case
we're using the thread-pool implementation.  If we're called by our creator
thread then we must be in the reactor.  In that case, we arrange to be put
into the thread pool.  If we're not in the creator thread then we must be
in the thread pool and we can do some work.
*/
int Client_Handler::handle_input (ACE_HANDLE _handle)
  {
  ACE_UNUSED_ARG (_handle);

  /*
  Check our strategy.  If we're using the thread pool and we're in the creation
  thread then we know we were called by the reactor.
  */
  if( concurrency() == Client_Acceptor::thread_pool_ )
    {
    if( ACE_Thread::self() == creator_ )
      {
      /*
      Remove ourselves from the reactor and ask to be put into the thread pool's
      queue of work.  (You should be able to use suspend_handler() but I've had
      problems with that.)
      */
      this->reactor()->remove_handler( this, REMOVE_MASK );
      return this->thread_pool()->enqueue(this);
      }
    }

  /*
  Any strategy other than thread-per-connection will eventually get here.  If we're in the
  single-threaded implementation or the thread-pool, we still have to pass this way.
  */

  char buf[128];
  memset (buf, 0, sizeof (buf));

  /*
  Invoke the process() method to do the work but save it's return value instead
  of returning it immediately.
  */

  int rval = this->process(buf,sizeof(buf));

  /*
  Now, we look again to see if we're in the thread-pool implementation.  If so then we
  need to re-register ourselves with the reactor so that we can get more work when it
  is available.  (If suspend_handler() worked then we would use resume_handler() here.)
  */
  if( concurrency() == Client_Acceptor::thread_pool_ )
    {
    if( rval != -1 )
      {
      this->reactor()->register_handler( this, REGISTER_MASK );
      }
    }

  /*
  Return the result of process()
  */
  return(rval);
  }

int Client_Handler::handle_close (ACE_HANDLE _handle, ACE_Reactor_Mask _mask)
{
  ACE_UNUSED_ARG (_handle);
  ACE_UNUSED_ARG (_mask);

  this->destroy ();
  return 0;
}

/*
Remember that when we leave our svc() method, the framework will take care
of calling our close() method so that we can cleanup after ourselves.
*/
int Client_Handler::svc(void)
{
  char buf[128];
  memset (buf, 0, sizeof (buf));

  while( !Madara::finished_ )
    {
      if( this->process(buf,sizeof(buf)) == -1 )
        {
          return(-1);
        }
    }

  return(0);
}

int Client_Handler::process_ping (Madara::Agent_Ping& from)
{
  // if we get a ping, reply to them, and then wait for them
  // to reply back. This results in someone doing the following,
  // where we are B
  //
  // B <- A
  // B -> A     : Help A get a roundtrip
  // B <- A     : A helps us get a roundtrip

ACE_DEBUG ((LM_DEBUG, "(%P|%t) client %s:%d wants to play\n", 
    from.host, from.port));
  
  std::string key;

  // compose a reply
  Madara::Agent_Ping reply;
  reply.size = 28;
  reply.type = 1;
  strcpy(reply.host, this->context_->getHost ().c_str ()); 
  reply.port = atoi (this->context_->getPort ().c_str ());

  // more than enough room
  char port[16];

  Madara::merge_key (key, from.host, itoa (from.port, port, 10));

  ACE_DEBUG ((LM_DEBUG, "(%P|%t) Sending reply to client %s@%d\n", 
    from.host, from.port));

  // keep track of time
  ACE_hrtime_t start = ACE_OS::gethrtime ();

  // send the reply
  this->peer ().send ((void *)&reply, sizeof(reply));

  // wait for our echo
  ACE_Time_Value timeout (0,5000);
  this->peer ().recv ( &from, sizeof (Madara::Agent_Ping), &timeout);

  ACE_hrtime_t end = ACE_OS::gethrtime ();

  // roundtrip latency
  
  ACE_hrtime_t total = end - start;

  unsigned int milliseconds = total / 1000;

  this->context_->addLatency (key, milliseconds);

  ACE_DEBUG ((LM_DEBUG, "(%P|%t) Latency round complete (%d) with client %s:%d\n", 
    milliseconds, from.host, from.port));

  return 0;
}

int Client_Handler::process_dump (Madara::Agent_Ping& from)
{
  // just dump the context to STDERR
  this->context_->write (std::cerr);

  return 0;
}

int Client_Handler::process_latency_query (Madara::Agent_Ping& from)
{
  Madara::Agent_Ping reply;
  std::string key;
  std::string host;
  std::string port;

  // fill in our host information
  strcpy(reply.host, context_->getHost ().c_str ()); 
  reply.port = atoi (context_->getPort ().c_str ());
  reply.size = context_->getNumKeys ();

  ACE_DEBUG ((LM_DEBUG, "(%P|%t) Received broker latency query\n"));
  
  // construct a latency array for our average latencies
  Madara::Agent_Latency * latencies = (Madara::Agent_Latency *)
    malloc (sizeof (Madara::Agent_Latency) * reply.size);

  ACE_DEBUG ((LM_DEBUG, "(%P|%t) Composing reply with %d latencies\n", 
    reply.size));
  
  
  reply.type = Madara::AGENT_LATENCY_QUERY_RESPONSE;

  // fill in average latency information
  for (unsigned int i = 0; i < reply.size; ++i)
    {
      key = context_->getKey (i);

      // fill in host / port
      Madara::split_key (key, host, port);

      strcpy (latencies[i].host, host.c_str ());
      latencies[i].port = atoi (port.c_str ());

      // fill in latency
      latencies[i].latency = context_->getAverageLatency (key);

      
  ACE_DEBUG ((LM_DEBUG, "(%P|%t) Latency[%d] = %d\n", 
    i, latencies[i].latency));
  
    }

  ACE_Time_Value timeout (4);

  // send the ping with the number of latencies
  this->peer ().send_n (&reply, sizeof(Madara::Agent_Ping), &timeout);
   
  ACE_DEBUG ((LM_DEBUG, "(%P|%t) LATENCIES REPLY\n"));
  
  ::size_t bytes_transferred = 0;

  // send the full average latency list
  this->peer ().send_n (latencies, 
    sizeof (Madara::Agent_Latency) * reply.size, 
    &timeout, &bytes_transferred);

  ACE_DEBUG ((LM_DEBUG, "(%P|%t) LATENCIES REPLIED %d bytes\n", bytes_transferred));
  
  return 0;
}

/*
Once again, we see that the application-level logic has not been at all affected
by our choice of threading models.  Of course, I'm not sharing data between threads
or anything.  We'll leave locking issues for a later tutorial.
*/
int Client_Handler::process (char *_rdbuf, int _rdbuf_len)
  {
  switch (this->peer ().recv (_rdbuf, _rdbuf_len))
    {
    case -1:
      ACE_ERROR_RETURN ((LM_ERROR, "(%P|%t) %p bad read\n", "client"), -1);
    case 0:
      ACE_ERROR_RETURN ((LM_ERROR, "(%P|%t) closing daemon (fd = %d)\n", this->get_handle ()), -1);
    default:
      Madara::Agent_Ping * from = (Madara::Agent_Ping *)_rdbuf;

      if (from->type == Madara::AGENT_PING)
        this->process_ping (*from);
      else if (from->type == Madara::AGENT_DUMP_CONTEXT)
        this->process_dump (*from);
      else if (from->type == Madara::BROKER_LATENCY_QUERY)
        this->process_latency_query (*from);
 
    }

  return 0;
  } 