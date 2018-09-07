#ifndef _MADARA_THREADS_WORKER_THREAD_INL_
#define _MADARA_THREADS_WORKER_THREAD_INL_

/**
 * @file WorkerThread.inl
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains inlined methods of the WorkerThread class
 **/

#include "WorkerThread.h"

namespace madara { namespace threads {

inline void
WorkerThread::change_frequency (
  double hertz,
  utility::TimeValue & current, utility::Duration & frequency,
  utility::TimeValue & next_epoch,
  bool & one_shot, bool & blaster)
{
  hertz_ = hertz;
#ifndef MADARA_NO_THREAD_LOCAL
  madara::logger::Logger::set_thread_hertz(hertz);
#endif
  if (hertz_ > 0.0)
  {
    madara_logger_ptr_log (logger::global_logger.get (), logger::LOG_MAJOR,
      "WorkerThread(%s)::svc:" \
      " thread repeating at %f hz\n", name_.c_str (), hertz_);

    one_shot = false;
    
    frequency = utility::seconds_to_duration (1.0/hertz_);

    next_epoch = current;
    next_epoch += frequency;
  }
  else if (hertz_ == 0.0)
  {
    // infinite hertz until terminate

    madara_logger_ptr_log (logger::global_logger.get (), logger::LOG_MAJOR,
      "WorkerThread(%s)::svc:" \
      " thread blasting at infinite hz\n", name_.c_str ());

    one_shot = false;
    blaster = true;
  }
  else
  {
    madara_logger_ptr_log (logger::global_logger.get (), logger::LOG_MAJOR,
      "WorkerThread(%s)::svc:" \
      " thread running once\n", name_.c_str ());
  }
}

} }

#endif // _MADARA_THREADS_WORKER_THREAD_H_
