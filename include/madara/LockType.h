#ifndef _MADARA_LOCK_TYPE_
#define _MADARA_LOCK_TYPE_

#include "ace/Recursive_Thread_Mutex.h"
#include "ace/Null_Mutex.h"

  #ifndef MADARA_LOCK_TYPE

    #ifdef _MADARA_STL_LOCK_
      // include headers for STL recursive mutexes
      #include <mutex>
      #include <condition_variable>

      #define  MADARA_LOCK_TYPE              std::recursive_mutex
      #define  MADARA_LOCK_LOCK              lock
      #define  MADARA_LOCK_UNLOCK            unlock
      #define  MADARA_GUARD_TYPE             std::lock_guard<MADARA_LOCK_TYPE>
      #define  MADARA_CONDITION_TYPE         std::condition_variable_any
      #define  MADARA_CONDITION_NOTIFY_ONE   notify_one
      #define  MADARA_CONDITION_NOTIFY_ALL   notify_all

    #elif defined _MADARA_NULL_LOCK_
      // include headers for ACE_Null_Mutex
      #include "ace/Guard_T.h"
      #include "ace/Recursive_Thread_Mutex.h"
      #include "ace/Condition_Recursive_Thread_Mutex.h"
      #include "ace/Synch.h"

      #define  MADARA_LOCK_TYPE              ACE_Null_Mutex
      #define  MADARA_LOCK_LOCK              acquire
      #define  MADARA_LOCK_UNLOCK            release
      #define  MADARA_GUARD_TYPE             ACE_Guard<MADARA_LOCK_TYPE>
      #define  MADARA_CONDITION_TYPE         ACE_Condition<MADARA_LOCK_TYPE>
      #define  MADARA_CONDITION_NOTIFY_ONE   signal
      #define  MADARA_CONDITION_NOTIFY_ALL   broadcast
      #define  MADARA_CONDITION_MUTEX_CONSTRUCTOR
    #else
      // include headers for ACE Recursive Thread Mutexes
      #include "ace/Guard_T.h"
      #include "ace/Recursive_Thread_Mutex.h"
      #include "ace/Condition_Recursive_Thread_Mutex.h"
      #include "ace/Synch.h"

      #define  MADARA_LOCK_TYPE              ACE_Recursive_Thread_Mutex
      #define  MADARA_LOCK_LOCK              acquire
      #define  MADARA_LOCK_UNLOCK            release
      #define  MADARA_GUARD_TYPE             ACE_Guard<MADARA_LOCK_TYPE>
      #define  MADARA_CONDITION_TYPE         ACE_Condition<MADARA_LOCK_TYPE>
      #define  MADARA_CONDITION_NOTIFY_ONE   signal
      #define  MADARA_CONDITION_NOTIFY_ALL   broadcast
      #define  MADARA_CONDITION_MUTEX_CONSTRUCTOR
    #endif // !_MADARA_NULL_LOCK_

  #endif // !MADARA_LOCK_TYPE

#endif // _MADARA_LOCK_TYPE_
