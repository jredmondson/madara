#ifndef _MADARA_LOCK_TYPE_
#define _MADARA_LOCK_TYPE_

#include <mutex>
#include <condition_variable>
#include "madara/MadaraExport.h"

#ifndef MADARA_LOCK_TYPE

  #if defined _MADARA_NULL_LOCK_
    namespace madara {
      struct null_mutex {
        void lock() {}
        void unlock() {}
        bool try_lock() { return true; }
      };
    }

    #define  MADARA_LOCK_TYPE              madara::null_mutex
    #define  MADARA_LOCK_LOCK              lock
    #define  MADARA_LOCK_UNLOCK            unlock
  #else
    #define  MADARA_LOCK_TYPE              std::recursive_mutex
    #define  MADARA_LOCK_LOCK              lock
    #define  MADARA_LOCK_UNLOCK            unlock
  #endif // !_MADARA_NULL_LOCK_

  #define  MADARA_GUARD_TYPE             std::lock_guard<MADARA_LOCK_TYPE>
  #define  MADARA_CONDITION_TYPE         std::condition_variable_any
  #define  MADARA_CONDITION_NOTIFY_ONE   notify_one
  #define  MADARA_CONDITION_NOTIFY_ALL   notify_all

#endif // !MADARA_LOCK_TYPE

#endif // _MADARA_LOCK_TYPE_
