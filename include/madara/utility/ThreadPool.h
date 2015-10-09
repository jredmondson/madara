#ifndef _MADARA_THREAD_POOL_H_
#define _MADARA_THREAD_POOL_H_

#include "ace/Mutex.h"
#include "ace/Task.h"
#include "ace/Barrier.h"

class ACE_Event_Handler;

namespace madara
{
  
   /**
   * @class MessageGuard
   * @brief This class is used in the svc methods to
   *        ensure that all message blocks are released
   *        even in the event of a thread exit
   */
    class ThreadPool : public ACE_Task<ACE_MT_SYNCH>
    {
    public:

      enum size_t
        {
          DEFAULT_POOL_SIZE = 4
        };

      /// Constructor
      ThreadPool();
      ~ThreadPool();

      /// Open the thread pool with the indicated number of threads
      int open(int pool_size = DEFAULT_POOL_SIZE);

      /// Close the thread pool
      int close(void);

      /// Enqueue a unit of work
      int enqueue(ACE_Message_Block * message);

      /// typdef for a threadsafe counter
      typedef ACE_Atomic_Op<ACE_Mutex,int> Counter;
    protected:

      /**
       * @class CounterGuard
       * @brief This class is used in the svc methods to
       *        ensure that the active thread count is
       *        kept up to date, even if we do an thread exit
       */
      class CounterGuard
        {
        public:
          CounterGuard (Counter & counter, ACE_Barrier & barrier)
            : counter_ (counter), barrier_ (barrier)
            {
              ++counter_;
            }

          ~CounterGuard(void)
            {
              --counter_;
              
              ACE_DEBUG ((LM_DEBUG, "(%P|%t) entering barrier.\n"));
 
              barrier_.wait ();
            }

        protected:
          Counter & counter_;
          ACE_Barrier & barrier_;
        };

       /**
       * @class MessageGuard
       * @brief This class is used in the svc methods to
       *        ensure that all message blocks are released
       *        even in the event of a thread exit
       */
      class MessageBlockGuard
        {
        public:
          MessageBlockGuard (ACE_Message_Block * & mb)
            : mb_ (mb)
            {
            }

          ~MessageBlockGuard( void )
            {
              mb_->release();
            }

        protected:
          ACE_Message_Block * & mb_;
        };

      /// service entry point for all threads
      int svc (void);

      /// service exit point for thread
      int enter_barrier (void);

    private:
      // number of threads
      Counter threads_;
      ACE_Barrier * barrier_;
    };
}

#endif // _MADARA_THREAD_POOL_H_
