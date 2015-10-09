
#ifndef _MADARA_TIMED_EVENT_THREAD_H_
#define _MADARA_TIMED_EVENT_THREAD_H_

/**
 * @file MulticastTransportReadThread.h
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains the MulticastTransportReadThread class, which provides a
 * multicast transport for reading knowledge updates in KaRL
 **/

#include <string>

#include "madara/utility/ScopedArray.h"
#include "madara/knowledge/ThreadSafeContext.h"
#include "madara/expression/ExpressionTree.h"
#include "madara/knowledge/TimedExecutor.h"

#include "ace/Task.h"
#include "ace/Atomic_Op_T.h"
#include "ace/Thread_Mutex.h"

#include "ace/Synch.h"
 
namespace madara
{
  namespace knowledge
  {
    /**
     * @class TimedEventThread
     * @brief Thread executing timed events
     **/
    class TimedEventThread : public ACE_Task<ACE_NULL_SYNCH>
    {
    public:
      /**
       * Constructor
       **/
      TimedEventThread (TimedEventThreadInfo & info);
      
      /**
      * Destructor
      **/
      ~TimedEventThread ();
      
      /**
      * Service loop for the thread
      **/
      int svc (void);
      
    private:
      /**
       * info for service loop
       **/
      TimedEventThreadInfo info_;
      
#ifndef _MADARA_NO_KARL_
      /**
       * expression for checking if events are queued or terminated
       **/
      CompiledExpression queued_or_terminated_;
      
      /**
       * expression for letting control plane know the thread is closed
       **/
      CompiledExpression thread_closed_;
#endif // _MADARA_NO_KARL_

    };
  }
}

#endif // _MADARA_TIMED_EVENT_THREAD_H_
