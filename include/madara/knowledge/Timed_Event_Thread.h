
#ifndef _MADARA_TIMED_EVENT_THREAD_H_
#define _MADARA_TIMED_EVENT_THREAD_H_

/**
 * @file Multicast_Transport_Read_Thread.h
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains the Multicast_Transport_Read_Thread class, which provides a
 * multicast transport for reading knowledge updates in KaRL
 **/

#include <string>

#include "madara/utility/Scoped_Array.h"
#include "madara/knowledge/Thread_Safe_Context.h"
#include "madara/expression/Expression_Tree.h"
#include "madara/knowledge/Timed_Executor.h"

#include "ace/Task.h"
#include "ace/Atomic_Op_T.h"
#include "ace/Thread_Mutex.h"

#include "ace/Synch.h"
 
namespace madara
{
  namespace knowledge
  {
    /**
     * @class Timed_Event_Thread
     * @brief Thread executing timed events
     **/
    class Timed_Event_Thread : public ACE_Task<ACE_NULL_SYNCH>
    {
    public:
      /**
       * Constructor
       **/
      Timed_Event_Thread (Timed_Event_Thread_Info & info);
      
      /**
      * Destructor
      **/
      ~Timed_Event_Thread ();
      
      /**
      * Service loop for the thread
      **/
      int svc (void);
      
    private:
      /**
       * info for service loop
       **/
      Timed_Event_Thread_Info info_;
      
#ifndef _MADARA_NO_KARL_
      /**
       * expression for checking if events are queued or terminated
       **/
      Compiled_Expression queued_or_terminated_;
      
      /**
       * expression for letting control plane know the thread is closed
       **/
      Compiled_Expression thread_closed_;
#endif // _MADARA_NO_KARL_

    };
  }
}

#endif // _MADARA_TIMED_EVENT_THREAD_H_
