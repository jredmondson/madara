#include "TimedExecutor.h"
#include "TimedEventThread.h"
#include "ace/Thread.h"
#include "madara/logger/GlobalLogger.h"

bool operator< (
  const madara::knowledge::TimedEvent & lhs,
  const madara::knowledge::TimedEvent & rhs)
{
  return lhs.first > rhs.first;
}


madara::knowledge::TimedExecutor::TimedExecutor ()
  : thread_info_ (0), threads_ (0), num_threads_ (0)
{
  threads_ref_ = control_plane_.get_ref ("threads");
  queue_size_ = control_plane_.get_ref ("queued");
  terminated_ = control_plane_.get_ref ("terminated");
}
      

madara::knowledge::TimedExecutor::~TimedExecutor ()
{
  shutdown ();
  clear_queue ();
}

void
madara::knowledge::TimedExecutor::add (const TimedEvent & new_event)
{
  Guard guard (mutex_);

  events_.push (new_event);
  
  // inform sleeping threads of new queued events
  control_plane_.set (
    queue_size_, knowledge::KnowledgeRecord::Integer (events_.size ()));
}

void
madara::knowledge::TimedExecutor::add (const Event & new_event)
{
  Guard guard (mutex_);

  TimedEvent timed_event;

  // setup times
  ACE_Time_Value cur_time = ACE_High_Res_Timer::gettimeofday ();

  // create timed_event
  timed_event.first = cur_time + new_event.delay;
  timed_event.second = new Event (new_event);

  // add the timed event to the event queue
  events_.push (timed_event);
  
  // inform sleeping threads of new queued events
  control_plane_.set (
    queue_size_, knowledge::KnowledgeRecord::Integer (events_.size ()));
}

ACE_Time_Value
madara::knowledge::TimedExecutor::remove (TimedEvent & cur_event)
{
  // obtain current time
  ACE_Time_Value cur_time = ACE_High_Res_Timer::gettimeofday ();
  
  mutex_.acquire ();

  // obtain next event in queue
  if (events_.size () > 0)
  {
    madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_MINOR, "TimedExecutor::remove: " \
      "events queue size is greater than zero\n");

    cur_event = events_.top ();

    // if we've hit the timeout, pop
    if (cur_time >= cur_event.first)
    {
      madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_MAJOR, "TimedExecutor::remove: "
        "removing top element from events queue\n");

      events_.pop ();
    }
    // otherwise, zero out the cur_event event pointer
    else
    {
      madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_MAJOR, "TimedExecutor::remove: "
        "earliest event is not ready yet\n");

      cur_event.second = 0;
      cur_time = cur_event.first - cur_time;
    }
    
    // inform sleeping threads of new queued events
    control_plane_.set (
      queue_size_, knowledge::KnowledgeRecord::Integer (events_.size ()));

    mutex_.release ();
  }
  // there are no events in queue
  else
  {
    mutex_.release ();

    madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_MAJOR, "TimedExecutor::remove: "
      "Nothing to do. Thread going to sleep\n");
    
    WaitSettings wait_settings;
    wait_settings.poll_frequency = -1;
    wait_settings.max_wait_time = 5.0;
    
#ifndef _MADARA_NO_KARL_
    // inform sleeping threads of new queued events
    control_plane_.wait ("queued > 0 || terminated", wait_settings);
#endif // _MADARA_NO_KARL_

    madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_MAJOR, "TimedExecutor::remove: "
      "Condition has changed. Thread waking up\n");

    cur_event.second = 0;
    cur_time.set (0.0);
  }

  return cur_time;
}

void
madara::knowledge::TimedExecutor::shutdown (void)
{
  if (thread_info_)
  {
    control_plane_.set (terminated_, knowledge::KnowledgeRecord::Integer (1));

    // wait for all threads to complete
    enter_barrier ();

    madara::utility::sleep (1.0);

    delete [] thread_info_;
  
    for (unsigned int i = 0; i < num_threads_; ++i)
    {
      delete threads_[i];
    }

    delete [] threads_;

    thread_info_ = 0;
    threads_ = 0;
    num_threads_ = 0;

    control_plane_.clear ();
  }
}

bool
madara::knowledge::TimedExecutor::is_shutdown (void)
{
  return control_plane_.get (terminated_).to_integer () != 0;
}

void
madara::knowledge::TimedExecutor::launch_threads (
  unsigned int num_threads)
{
  control_plane_.set (terminated_, knowledge::KnowledgeRecord::Integer (0));

  // shutdown any existing threads
  if (thread_info_)
  {
    shutdown ();
  }

  thread_info_ = new TimedEventThreadInfo [num_threads];
  threads_ = new TimedEventThread * [num_threads];
  num_threads_ = num_threads;
  control_plane_.set (threads_ref_, num_threads_);

  for (unsigned int i = 0; i < num_threads; ++i)
  {
    thread_info_[i].id = i;
    thread_info_[i].executor = this;
    thread_info_[i].control_plane = &control_plane_;
    threads_[i] = new TimedEventThread (thread_info_[i]);
  }
}

ACE_Time_Value
madara::knowledge::TimedExecutor::time_until_next (void)
{
  // get the current time
  ACE_Time_Value cur_time = ACE_High_Res_Timer::gettimeofday ();

  // calculate the time left before next event is due
  cur_time = events_.top ().first - cur_time;

  // return the time
  return cur_time;
}

void 
madara::knowledge::TimedExecutor::lock (void)
{
  mutex_.acquire ();
}

void 
madara::knowledge::TimedExecutor::unlock (void)
{
  mutex_.release ();
}

madara::knowledge::Event
madara::knowledge::fill_event (
  KnowledgeBase & knowledge, const std::string & expression,
      double delay, double period, int executions)
{
  Event new_event;
  new_event.executions = 0;
  new_event.intended_executions = executions;
  new_event.knowledge = &knowledge;
  new_event.period.set (period);
  new_event.delay.set (delay);

#ifndef _MADARA_NO_KARL_

  new_event.root = knowledge.compile (expression).get_root ();
  
#endif // _MADARA_NO_KARL_

  return new_event;
}

void 
madara::knowledge::TimedExecutor::enter_barrier (void)
{
  WaitSettings settings;
  settings.poll_frequency = .5;
  settings.max_wait_time = -1;

  madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_MAJOR, "TimedExecutor::enter_barrier: "
    "Entering barrier\n");
  
#ifndef _MADARA_NO_KARL_

  control_plane_.wait (
    "closed = 0 ;>\n"
    ".i [0->threads) (thread.{.i}.closed => ++closed) ;>\n"
    "closed == threads", settings);
  
#endif // _MADARA_NO_KARL_

  madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_MAJOR,
    "TimedExecutor::enter_barrier: Leaving barrier\n");
}

void 
madara::knowledge::TimedExecutor::clear_queue (void)
{
  Guard guard (mutex_);

  while (events_.size () > 0)
  {
    TimedEvent cur_event (events_.top ());
    delete cur_event.second;
    events_.pop ();
  }
}

madara::knowledge::KnowledgeRecord::Integer
madara::knowledge::TimedExecutor::num_threads (void)
{
  Guard guard (mutex_);

  return num_threads_;
}
