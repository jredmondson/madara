/*
Since this is the third time we've seen most of this, I'm going to strip out almost
all of the comments that you've already seen.  That way, you can concentrate on the
new items.
*/

#include <iostream>

#include "ace/High_Res_Timer.h"

#include "ClientAcceptor.h"
#include "ClientHandler.h"
#include "AgentMessages.h"
#include "MadaraCommon.h"
#include "BrokerContext.h"
#include "Deployment.h"

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
ClientHandler::ClientHandler (void)
: client_acceptor_(0), creator_(ACE_Thread::self()),
  context_ (0)
  {
  }

ClientHandler::~ClientHandler (void)
  {
  }

/*
Query our acceptor for the concurrency strategy.  Notice that we don't bother
to check that our acceptor pointer is valid.  That is proably a bad idea...
*/
int ClientHandler::concurrency(void)
  {
  return this->client_acceptor()->concurrency();
  }

/*
And here we ask the acceptor about the thread pool.
*/
ThreadPool * ClientHandler::thread_pool(void)
  {
  return this->client_acceptor()->thread_pool();
  }

/*
The destroy() method hasn't changed since we wrote it back in Tutorial 5.
*/
void ClientHandler::destroy (void)
  {
  this->peer ().close ();

  this->reactor ()->remove_handler (this, REMOVE_MASK );

  delete this;
  }

/*
Back to our open() method.  This is straight out of Tutorial 6.  There's
nothing additional here for the thread-pool implementation.
*/
int ClientHandler::open (void *_acceptor)
  {
  client_acceptor( (ClientAcceptor *) _acceptor );
  context_ = &(client_acceptor()->context ());

  if( concurrency() == ClientAcceptor::thread_per_connection_ )
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
int ClientHandler::close(u_long flags)
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
int ClientHandler::handle_input (ACE_HANDLE _handle)
  {
  ACE_UNUSED_ARG (_handle);

  /*
  Check our strategy.  If we're using the thread pool and we're in the creation
  thread then we know we were called by the reactor.
  */
  if( concurrency() == ClientAcceptor::thread_pool_ )
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
  if( concurrency() == ClientAcceptor::thread_pool_ )
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

int ClientHandler::handle_close (ACE_HANDLE _handle, ACE_Reactor_Mask _mask)
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
int ClientHandler::svc(void)
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

int ClientHandler::process_dump (Madara::AgentPing& from)
{
  // just dump the context to STDERR
  this->context_->write (std::cerr);

  return 0;
}

int ClientHandler::process_deployment_offer (Madara::AgentPing& from)
{
  // just dump the context to STDERR
  //this->context_->write (std::cerr);

  ACE_Time_Value timeout (4);
  Madara::AgentFile deployment_file;

  // send the ping with the number of latencies
  this->peer ().recv_n (&deployment_file, sizeof(deployment_file), &timeout);

  context_->read (deployment_file.name);

  context_->writeRequirementsLegible (std::cerr);

  return 0;
}

int ClientHandler::process_deployment_print (Madara::AgentPing& from)
{
  // just dump the context to STDERR
  //this->context_->write (std::cerr);

  // printout the deployment requirements
//  context_->writeRequirementsLegible (std::cerr);

  context_->writeRequirementsLegible (std::cerr);

  // generate current best candidate
  Madara::Deployment::Candidate best = context_->learnDeployment ();

  context_->writeDeploymentCandidate (std::cerr, best);

  return 0;
}

/*
Once again, we see that the application-level logic has not been at all affected
by our choice of threading models.  Of course, I'm not sharing data between threads
or anything.  We'll leave locking issues for a later tutorial.
*/
int ClientHandler::process (char *_rdbuf, int _rdbuf_len)
  {
  switch (this->peer ().recv (_rdbuf, _rdbuf_len))
    {
    case -1:
      ACE_ERROR_RETURN ((LM_ERROR, "(%P|%t) %p bad read\n", "client"), -1);
    case 0:
      ACE_ERROR_RETURN ((LM_ERROR, "(%P|%t) closing daemon (fd = %d)\n", this->get_handle ()), -1);
    default:
      Madara::AgentPing * from = (Madara::AgentPing *)_rdbuf;

      if (from->type == Madara::AGENT_DUMP_CONTEXT)
        this->process_dump (*from);
      else if (from->type == Madara::BROKER_DEPLOYMENT_OFFER)
        this->process_deployment_offer (*from);
      else if (from->type == Madara::BROKER_DEPLOYMENT_PRINT)
        this->process_deployment_print (*from);
    }

  return 0;
  } 