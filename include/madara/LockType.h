#ifndef _MADARA_LOCK_TYPE_
#define _MADARA_LOCK_TYPE_

#include "ace/Recursive_Thread_Mutex.h"
#include "ace/Null_Mutex.h"

  #ifndef MADARA_LOCK_TYPE

    #ifdef _MADARA_NULL_LOCK_
      #define  MADARA_LOCK_TYPE  ACE_Null_Mutex
    #else
      #define  MADARA_LOCK_TYPE  ACE_Recursive_Thread_Mutex
    #endif // !_MADARA_NULL_LOCK_

  #endif // !MADARA_LOCK_TYPE

#endif // _MADARA_LOCK_TYPE_
