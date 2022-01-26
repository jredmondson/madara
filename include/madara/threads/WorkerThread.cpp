#include "WorkerThread.h"
#include "madara/logger/GlobalLogger.h"
#include "madara/utility/Utility.h"

#ifdef _MADARA_JAVA_
#include <jni.h>
#include "madara_jni.h"
#include "madara/utility/java/Acquire_VM.h"
#endif

#ifndef _WIN32
#include <pthread.h>
#endif

#include <iostream>
#include <algorithm>

namespace madara
{
namespace threads
{
WorkerThread::WorkerThread(const std::string& name, BaseThread* thread,
    knowledge::KnowledgeBase control, knowledge::KnowledgeBase data,
    double hertz)
  : name_(name), thread_(thread), control_(control), data_(data), hertz_(hertz)
{
  if (thread)
  {
    std::stringstream base_string;

    knowledge::KnowledgeBase* kb = &control;
    knowledge::KnowledgeRecord debug_to_kb = control_.get(".debug_to_kb");
    if (debug_to_kb.exists())
    {
      base_string << debug_to_kb.to_string() << ".";
      kb = &data;
      data.set(debug_to_kb.to_string() + ".hertz", hertz,
          knowledge::EvalSettings::DELAY_NO_EXPAND);
    }
    base_string << name;

    thread->name = name;
    thread->init_control_vars(control);

    control_.get(".debug_to_kb").to_string();

    finished_.set_name(base_string.str() + ".finished", control);
    started_.set_name(base_string.str() + ".started", control);
    new_hertz_.set_name(base_string.str() + ".hertz", control);

    executions_.set_name(base_string.str() + ".executions", *kb);
    start_time_.set_name(base_string.str() + ".start_time", *kb);
    last_start_time_.set_name(base_string.str() + ".last_start_time", *kb);
    end_time_.set_name(base_string.str() + ".end_time", *kb);

    last_duration_.set_name(base_string.str() + ".last_duration", *kb);
    min_duration_.set_name(base_string.str() + ".min_duration", *kb);
    max_duration_.set_name(base_string.str() + ".max_duration", *kb);

    debug_.set_name(base_string.str() + ".debug", control);
    
    paused_.set_name(base_string.str() + ".paused", *kb);
    running_.set_name(base_string.str() + ".running", *kb);
    
    finished_ = 0;
    started_ = 0;
    new_hertz_ = hertz_;
  }
}

WorkerThread::~WorkerThread() noexcept
{
  try
  {
    if (me_.joinable())
    {
      madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_MINOR,
          "WorkerThread::~WorkerThread(%s):"
          " thread wasn't joined before destruction\n",
          name_.c_str());
      me_.detach();
    }
  }
  catch (const std::system_error& e)
  {
    madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_MINOR,
        "WorkerThread::~WorkerThread(%s):"
        " error trying to detach: %s\n",
        name_.c_str(), e.what());
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

#ifndef _WIN32
// Call pthread_setname_np if it exists ...
template<typename... Args>
auto try_pthread_setname_np(Args&&... args)
    -> decltype(pthread_setname_np(std::forward<Args>(args)...))
{
  return pthread_setname_np(std::forward<Args>(args)...);
}

// Otherwise, do nothing
void try_pthread_setname_np(...) {}
#endif

void WorkerThread::run(void)
{
  try
  {
    me_ = std::thread(&WorkerThread::svc, this);

#ifndef _WIN32
    try_pthread_setname_np(me_.native_handle(), name_.substr(0, 15).c_str());
#endif

    std::ostringstream os;
    os << std::this_thread::get_id() << " spawned " << me_.get_id()
       << std::endl;

    madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_MAJOR,
        "WorkerThread::WorkerThread(%s):"
        " thread started %s\n",
        name_.c_str(), os.str().c_str());
  }
  catch (const std::exception& e)
  {
    madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_MAJOR,
        "WorkerThread::WorkerThread(%s):"
        " failed to create thread: %s\n",
        name_.c_str(), e.what());
    throw;
  }
}

int WorkerThread::svc(void)
{
  madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_MAJOR,
      "WorkerThread(%s)::svc:"
      " checking thread existence\n",
      name_.c_str());

  if (thread_)
  {
    started_ = 1;

#ifdef _MADARA_JAVA_
    // try detaching one more time, just to make sure.
    utility::java::Acquire_VM jvm(false);
#endif
    
#if 0
    madara::logger::Logger::set_thread_name(name_);
#endif

    thread_->init(data_);

    {
      utility::TimeValue current = utility::get_time_value();
      utility::TimeValue next_epoch;
      utility::Duration frequency;

      // only allow one-way communication of durations. We never read control
      int64_t min_duration = -1;
      int64_t max_duration = 0;
      int64_t last_duration = 0;
      bool max_duration_changed = true;
      bool min_duration_changed = true;

      bool one_shot = true;
      bool blaster = false;

      bool debug = debug_.is_true();

      knowledge::VariableReference terminated;

      terminated = control_.get_ref(name_ + ".terminated");

      // change thread frequency
      change_frequency(
          hertz_, current, frequency, next_epoch, one_shot, blaster);
#if 0
      madara::logger::Logger::set_thread_hertz(hertz_);
#endif

      if (debug)
      {
        start_time_ = utility::get_time();
      }

      while (control_.get(terminated).is_false())
      {
        madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_MAJOR,
            "WorkerThread(%s)::svc:"
            " thread checking for pause\n",
            name_.c_str());

        if (paused_.is_false())
        {
          running_ = 1;

          madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_MAJOR,
              "WorkerThread(%s)::svc:"
              " thread calling run function\n",
              name_.c_str());

          try
          {
            int64_t start_time = 0, end_time = 0;
            debug = debug_.is_true();

            if (debug)
            {
              start_time = utility::get_time();
              ++executions_;
            } // debug

            thread_->run();

            if (debug)
            {
              end_time = utility::get_time();

              // update duration information
              last_duration = end_time - start_time;
              if (min_duration == -1 || last_duration < min_duration)
              {
                min_duration = last_duration;
                min_duration_changed = true;
              }
              if (last_duration > max_duration)
              {
                max_duration = last_duration;
                max_duration_changed = true;
              }

              // lock control plane and update
              {
                // write updates to control
                knowledge::ContextGuard guard(control_);
                last_start_time_ = start_time;
                end_time_ = end_time;

                last_duration_ = last_duration;
                if (max_duration_changed)
                {
                  max_duration_ = max_duration;
                }
                if (min_duration_changed)
                {
                  min_duration_ = min_duration;
                }
              }  // end lock of control plane
            }    // end if debug
          }      // end try of the run
          catch (const std::exception& e)
          {
            madara_logger_ptr_log(logger::global_logger.get(),
                logger::LOG_EMERGENCY,
                "WorkerThread(%s)::svc:"
                " exception thrown: %s\n",
                name_.c_str(), e.what());
          }

          running_ = 0;
        }

        if (one_shot)
          break;

        // check for a change in frequency/hertz
        if (new_hertz_ != hertz_)
        {
          change_frequency(
              *new_hertz_, current, frequency, next_epoch, one_shot, blaster);
        }

        if (!blaster)
        {
          current = utility::get_time_value();

          madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_MAJOR,
              "WorkerThread(%s)::svc:"
              " thread checking for next hertz epoch\n",
              name_.c_str());

          if (current < next_epoch)
            utility::sleep(next_epoch - current);

          madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_MAJOR,
              "WorkerThread(%s)::svc:"
              " thread past epoch\n",
              name_.c_str());

          next_epoch += frequency;
        }
      }  // end while !terminated

      madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_MAJOR,
          "WorkerThread(%s)::svc:"
          " thread has been terminated\n",
          name_.c_str());
    }

    madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_MAJOR,
        "WorkerThread(%s)::svc:"
        " calling thread cleanup method\n",
        name_.c_str());

    thread_->cleanup();

    madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_MAJOR,
        "WorkerThread(%s)::svc:"
        " deleting thread\n",
        name_.c_str());

    madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_MAJOR,
        "WorkerThread(%s)::svc:"
        " setting finished to 1\n",
        finished_.get_name().c_str());

    finished_ = 1;
  }
  else
  {
    madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_MAJOR,
        "WorkerThread(%s)::svc:"
        " thread creation failed\n",
        name_.c_str());
  }

  return 0;
}
}
}
