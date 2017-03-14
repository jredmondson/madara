#include "WorkerThread.h"
#include "ace/High_Res_Timer.h"
#include "ace/OS_NS_sys_time.h"
#include "madara/logger/GlobalLogger.h"

#ifdef _MADARA_JAVA_
#include <jni.h>
#include "madara_jni.h"
#include "madara/utility/java/Acquire_VM.h"
#endif

#ifdef WIN32

#include <process.h>

unsigned __stdcall worker_thread_windows_glue (void * param)
{
  madara::threads::WorkerThread * caller = 
    static_cast < madara::threads::WorkerThread *> (
      param);
  if (caller)
  {
    return (unsigned) caller->svc ();
  }
  else
  {
    return 0;
  }
}

#endif


#include <iostream>
#include <algorithm>

madara::threads::WorkerThread::WorkerThread ()
  : thread_ (0), control_ (0), data_ (0), hertz_ (-1.0)
{
}


madara::threads::WorkerThread::WorkerThread (
  const std::string & name,
  BaseThread * thread,
  knowledge::KnowledgeBase * control,
  knowledge::KnowledgeBase * data,
  double hertz)
  : name_ (name), thread_ (thread), control_ (control), data_ (data),
    hertz_ (hertz)
{
  if (thread && control)
  {
    std::stringstream base_string;
    base_string << name;
    
    thread->name = name;
    thread->init_control_vars (*control);

    finished_.set_name (
      base_string.str () + ".finished", *control);
    started_.set_name (
      base_string.str () + ".started", *control);
    new_hertz_.set_name (
      base_string.str () + ".hertz", *control);

    finished_ = 0;
    started_ = 0;
    new_hertz_ = hertz_;
  }
}

madara::threads::WorkerThread::WorkerThread (const WorkerThread & input)
  : name_ (input.name_), thread_ (input.thread_),
    control_ (input.control_), data_ (input.data_),
    finished_ (input.finished_), started_ (input.started_),
    new_hertz_ (input.new_hertz_),
    hertz_ (input.hertz_)
{
}

madara::threads::WorkerThread::~WorkerThread ()
{
}

void
madara::threads::WorkerThread::operator= (const WorkerThread & input)
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
}

void
madara::threads::WorkerThread::run (void)
{
  int result;

#ifndef WIN32
  result = this->activate ();
#else
  result = 0;
  _beginthreadex(NULL, 0, worker_thread_windows_glue, (void*)this, 0, 0);
    
#endif

  if (result != -1)
  {
    madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_MAJOR,
      "WorkerThread::WorkerThread(%s):" \
      " thread started (result = %d)\n", name_.c_str (), result);
  }
  else
  {
    madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_MAJOR,
      "WorkerThread::WorkerThread(%s):" \
      " failed to create thread\n", name_.c_str ());
  }
}

int
madara::threads::WorkerThread::svc (void)
{
  madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_MAJOR,
    "WorkerThread(%s)::svc:" \
    " checking thread existence\n", name_.c_str ());

  if (thread_)
  {
    started_ = 1;
    
#ifdef _MADARA_JAVA_
    // try detaching one more time, just to make sure.
    madara::utility::java::Acquire_VM jvm (false);
#endif

    thread_->init (*data_);

    {
      ACE_Time_Value current = ACE_High_Res_Timer::gettimeofday ();
      ACE_Time_Value next_epoch, frequency;
      
      bool one_shot = true;
      bool blaster = false;

      knowledge::VariableReference terminated;
      knowledge::VariableReference paused;

      terminated = control_->get_ref (name_ + ".terminated");
      paused = control_->get_ref (name_ + ".paused");

      // change thread frequency
      change_frequency (hertz_, current, frequency, next_epoch,
        one_shot, blaster);

      while (control_->get (terminated).is_false ())
      {
        madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_MAJOR,
          "WorkerThread(%s)::svc:" \
          " thread checking for pause\n", name_.c_str ());

        if (control_->get (paused).is_false ())
        {
          madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_MAJOR,
            "WorkerThread(%s)::svc:" \
            " thread calling run function\n", name_.c_str ());

          thread_->run ();
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
          current = ACE_High_Res_Timer::gettimeofday ();

          madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_MAJOR,
            "WorkerThread(%s)::svc:" \
            " thread checking for next hertz epoch\n", name_.c_str ());

          if (current < next_epoch)
            madara::utility::sleep (next_epoch - current);  

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

    delete thread_;

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
