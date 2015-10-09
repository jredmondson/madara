
/*
We need this header so that we can invoke handle_input() on the objects we dequeue.
*/
#include "ace/Event_Handler.h"
#include "ace/OS.h"

#include "ace/Log_Msg.h"

#include "thread_pool.h"
#include "MadaraCommon.h"

/*
All we do here is initialize our active thread counter.
*/
ThreadPool::ThreadPool(void)
: active_threads_(0)
{
}

/*
Our open() method is a thin disguise around the ACE_Task<> activate() method.  By
hiding activate() in this way, the users of ThreadPool don't have to worry about
the thread configuration flags.
*/
int ThreadPool::open( int _pool_size )
{
  return this->activate(THR_NEW_LWP|THR_DETACHED,_pool_size);
}

/*
Closing the thread pool can be a tricky exercise.  I've decided to take an easy approach
and simply enqueue a secret message for each thread we have active.
*/
int ThreadPool::close(void)
  {
  int counter = active_threads_.value();

  
  ACE_DEBUG ((LM_DEBUG, "(%P|%t) closing threads.\n"));
 

  while( counter-- )
    {
    this->enqueue( 0 );
    }

  /* I'll probably replace this eventually with a barrier */
  while( active_threads_.value() )
    {
      ACE_OS::sleep( ACE_Time_Value(0.25) );
    }

  return(0);
  }

/*
When an object wants to do work in the pool, it should call the enqueue() method.
We introduce the ACE_Message_Block here but, unfortunately, we seriously missuse it.
*/
int ThreadPool::enqueue( ACE_Event_Handler * _handler )
  {

  void * v_data = (void*)_handler;
  char * c_data = (char*)v_data;

  ACE_Message_Block * mb = new ACE_Message_Block( c_data );

  /*
  Our putq() method is a wrapper around one of the enqueue methods of the ACE_Message_Queue
  that we own.  Like all good methods, it returns -1 if it fails for some reason.
  */
  if( this->putq(mb) == -1 )
    {
      mb->release();
      return(-1);
    }

  return(0);
  }


class CounterGuard
  {
  public:
    CounterGuard( ThreadPool::counter_t & _counter )
      : counter_(_counter)
      {
      ++counter_;
      }

    ~CounterGuard(void)
      {
      --counter_;
      }

  protected:
    ThreadPool::counter_t & counter_;
  };

/*
My MessageBlockGuard is also a little non-traditional.  It doesn't do anything in the
constructor but it's destructor ensures that the message block's release() method is called.
This is a cheap way to prevent a memory leak if I need an additional exit point in svc().
*/
class MessageBlockGuard
  {
  public:
    MessageBlockGuard( ACE_Message_Block * & _mb )
      : mb_(_mb)
      {
      }

    ~MessageBlockGuard( void )
      {
      mb_->release();
      }

  protected:
    ACE_Message_Block * & mb_;
  };

/*
Now we come to the svc() method.  As I said, this is being executed in each thread of the
ThreadPool.  Here, we pull messages off of our built-in ACE_Message_Queue and cause them
to do work.
*/
int ThreadPool::svc(void)
{
  ACE_Message_Block * mb;

  /*
  Create the guard for our active thread counter object.  No matter where we choose to
  return() from svc(), we no know that the counter will be decremented.
  */
  CounterGuard counter_guard(active_threads_);

  /* Process the queue */
  while( !Madara::finished_ && this->getq(mb) != -1 )
    {
      MessageBlockGuard message_block_guard(mb);

      char * c_data = mb->base();

        if( c_data )
          {
          void * v_data = (void*)c_data;

          ACE_Event_Handler * handler = (ACE_Event_Handler*)v_data;

          if( handler->handle_input(ACE_INVALID_HANDLE) == -1 )
            {
              handler->handle_close(handler->get_handle(),0);
            }
          }
        else
          {
            return(0);              // Ok, shutdown request
          }

        // message_block_guard goes out of scope here
        // and releases the message_block instance.
    }

  return(0);
} 