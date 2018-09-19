#include "WorkerThread.h"
#include "madara/logger/GlobalLogger.h"
#include "madara/utility/Utility.h"

#ifdef _MADARA_JAVA_
#include <jni.h>
#include "madara_jni.h"
#include "madara/utility/java/Acquire_VM.h"
#endif


#include <iostream>
#include <algorithm>

namespace madara { namespace threads {

WorkerThread::WorkerThread (
  const std::string & name,
  BaseThread * thread,
  knowledge::KnowledgeBase control,
  knowledge::KnowledgeBase data,
  double hertz)
  : name_ (name), thread_ (thread), control_ (control), data_ (data),
    hertz_ (hertz)
{
  if (thread)
  {
    std::stringstream base_string;
    base_string << name;

    thread->name = name;
    thread->init_control_vars (control);

    finished_.set_name (
      base_string.str () + ".finished", control);
    started_.set_name (
      base_string.str () + ".started", control);
    new_hertz_.set_name (
      base_string.str () + ".hertz", control);

    executions_.set_name (
      base_string.str () + ".executions", control);
    start_time_.set_name (
      base_string.str () + ".start_time", control);
    last_start_time_.set_name (
      base_string.str () + ".last_start_time", control);
    end_time_.set_name (
      base_string.str () + ".end_time", control);

    finished_ = 0;
    started_ = 0;
    new_hertz_ = hertz_;
  }
}

WorkerThread::~WorkerThread () noexcept
{
  try {
    if (me_.joinable()) {
      madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_MINOR,
        "WorkerThread::~WorkerThread(%s):" \
        " thread wasn't joined before destruction\n", name_.c_str());
      me_.detach();
    }
  }
  catch (const std::system_error &e) {
    madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_MINOR,
      "WorkerThread::~WorkerThread(%s):" \
      " error trying to detach: %s\n", name_.c_str(), e.what());
  }
}

/*
void
WorkerThread::operator= (const WorkerThread & input)
{
  if (this != &input)
  {
    this->name_ = input.name_;
    this->thread_ = input.thread_;
    this->control_ = input.control_;
    this->data_ = input.data_;
    this->finished_ = input.finished_;
    this->started_ = input.started_;
    this->new_hertz_ = input.new_hertz_;
    this->hertz_ = input.hertz_;
  }
}*/

void
WorkerThread::run (void)
{
#ifndef _WIN32
#else
  //result = 0;
  //_beginthreadex(NULL, 0, worker_thread_windows_glue, (void*)this, 0, 0);

#endif
  try {
    me_ = std::thread(&WorkerThread::svc, this);

    std::ostringstream os;
    os << std::this_thread::get_id() << " spawned " << me_.get_id() << std::endl;

    madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_MAJOR,
      "WorkerThread::WorkerThread(%s):" \
      " thread started %s\n", name_.c_str (), os.str().c_str());
  }
  catch (const std::exception &e)
  {
    madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_MAJOR,
      "WorkerThread::WorkerThread(%s):" \
      " failed to create thread: %s\n", name_.c_str (), e.what ());
    throw;
  }
}

int
WorkerThread::svc (void)
{
  madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_MAJOR,
    "WorkerThread(%s)::svc:" \
    " checking thread existence\n", name_.c_str ());

  if (thread_)
  {
    started_ = 1;

#ifdef _MADARA_JAVA_
    // try detaching one more time, just to make sure.
    utility::java::Acquire_VM jvm (false);
#endif

    thread_->init (data_);

    {
      utility::TimeValue current = utility::get_time_value ();
      utility::TimeValue next_epoch;
      utility::Duration frequency;

      bool one_shot = true;
      bool blaster = false;

      knowledge::VariableReference terminated;
      knowledge::VariableReference paused;

      terminated = control_.get_ref (name_ + ".terminated");
      paused = control_.get_ref (name_ + ".paused");
#ifndef MADARA_NO_THREAD_LOCAL
      madara::logger::Logger::set_thread_name(name_);
#endif

      // change thread frequency
      change_frequency (hertz_, current, frequency, next_epoch,
        one_shot, blaster);
#ifndef MADARA_NO_THREAD_LOCAL
      madara::logger::Logger::set_thread_hertz(hertz_);
#endif

      start_time_ = utility::get_time ();

      while (control_.get (terminated).is_false ())
      {
        madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_MAJOR,
          "WorkerThread(%s)::svc:" \
          " thread checking for pause\n", name_.c_str ());

        if (control_.get (paused).is_false ())
        {
          madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_MAJOR,
            "WorkerThread(%s)::svc:" \
            " thread calling run function\n", name_.c_str ());

          try
          {
            last_start_time_ = utility::get_time ();
            ++executions_;

            thread_->run ();

            end_time_ = utility::get_time ();
          }
          catch (const std::exception &e)
          {
            madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_EMERGENCY,
              "WorkerThread(%s)::svc:" \
              " exception thrown: %s\n", name_.c_str (), e.what());
          }
        }

        if (one_shot)
          break;

        // check for a change in frequency/hertz
        if (new_hertz_ != hertz_)
        {
          change_frequency (*new_hertz_,
            current, frequency, next_epoch, one_shot, blaster);
        }

        if (!blaster)
        {
          current = utility::get_time_value ();

          madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_MAJOR,
            "WorkerThread(%s)::svc:" \
            " thread checking for next hertz epoch\n", name_.c_str ());

          if (current < next_epoch)
            utility::sleep (next_epoch - current);

          madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_MAJOR,
            "WorkerThread(%s)::svc:" \
            " thread past epoch\n", name_.c_str ());

          next_epoch += frequency;
        }
      } // end while !terminated

      madara_logger_ptr_log (logger::global_logger.get (), logger::LOG_MAJOR,
        "WorkerThread(%s)::svc:" \
        " thread has been terminated\n", name_.c_str ());

    }

    madara_logger_ptr_log (logger::global_logger.get (), logger::LOG_MAJOR,
      "WorkerThread(%s)::svc:" \
      " calling thread cleanup method\n", name_.c_str ());

    thread_->cleanup ();

    madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_MAJOR,
      "WorkerThread(%s)::svc:" \
      " deleting thread\n", name_.c_str ());

    madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_MAJOR,
      "WorkerThread(%s)::svc:" \
      " setting finished to 1\n", finished_.get_name ().c_str ());

    finished_ = 1;
  }
  else
  {
    madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_MAJOR,
      "WorkerThread(%s)::svc:" \
      " thread creation failed\n", name_.c_str ());
  }

  return 0;
}

} }
