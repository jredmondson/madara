#include "Timed_Executor.h"
#include "Timed_Event_Thread.h"
#include "ace/Thread.h"

bool operator< (
  const Madara::Knowledge_Engine::Timed_Event & lhs,
  const Madara::Knowledge_Engine::Timed_Event & rhs)
{
  return lhs.first > rhs.first;
}


Madara::Knowledge_Engine::Timed_Executor::Timed_Executor ()
  : thread_info_ (0), threads_ (0), num_threads_ (0)
{
  threads_ref_ = control_plane_.get_ref ("threads");
  queue_size_ = control_plane_.get_ref ("queued");
  terminated_ = control_plane_.get_ref ("terminated");
}
      

Madara::Knowledge_Engine::Timed_Executor::~Timed_Executor ()
{
  shutdown ();
  clear_queue ();
}

void
Madara::Knowledge_Engine::Timed_Executor::add (const Timed_Event & new_event)
{
  Guard guard (mutex_);

  events_.push (new_event);
  
  // inform sleeping threads of new queued events
  control_plane_.set (
    queue_size_, Knowledge_Record::Integer (events_.size ()));
}

void
Madara::Knowledge_Engine::Timed_Executor::add (const Event & new_event)
{
  Guard guard (mutex_);

  Timed_Event timed_event;

  // setup times
  ACE_Time_Value cur_time = ACE_High_Res_Timer::gettimeofday ();

  // create timed_event
  timed_event.first = cur_time + new_event.delay;
  timed_event.second = new Event (new_event);

  // add the timed event to the event queue
  events_.push (timed_event);
  
  // inform sleeping threads of new queued events
  control_plane_.set (
    queue_size_, Knowledge_Record::Integer (events_.size ()));
}

ACE_Time_Value
Madara::Knowledge_Engine::Timed_Executor::remove (Timed_Event & cur_event)
{
  // obtain current time
  ACE_Time_Value cur_time = ACE_High_Res_Timer::gettimeofday ();
  
  mutex_.acquire ();

  // obtain next event in queue
  if (events_.size () > 0)
  {
    MADARA_DEBUG (MADARA_LOG_MINOR_EVENT, (LM_DEBUG, 
      DLINFO "Timed_Executor::remove: " \
      "events queue size is greater than zero.\n"));

    cur_event = events_.top ();

    // if we've hit the timeout, pop
    if (cur_time >= cur_event.first)
    {
      MADARA_DEBUG (MADARA_LOG_MAJOR_EVENT, (LM_DEBUG, 
        DLINFO "Timed_Executor::remove: " \
        "removing top element from events queue.\n"));

      events_.pop ();
    }
    // otherwise, zero out the cur_event event pointer
    else
    {
      MADARA_DEBUG (MADARA_LOG_MAJOR_EVENT, (LM_DEBUG, 
        DLINFO "Timed_Executor::remove: " \
        "earliest event is not ready yet.\n"));

      cur_event.second = 0;
      cur_time = cur_event.first - cur_time;
    }
    
    // inform sleeping threads of new queued events
    control_plane_.set (
      queue_size_, Knowledge_Record::Integer (events_.size ()));

    mutex_.release ();
  }
  // there are no events in queue
  else
  {
    mutex_.release ();

    MADARA_DEBUG (MADARA_LOG_MAJOR_EVENT, (LM_DEBUG, 
      DLINFO "Timed_Executor::remove: " \
      "Nothing to do. Thread going to sleep.\n"));
    
    Wait_Settings wait_settings;
    wait_settings.poll_frequency = -1;
    wait_settings.max_wait_time = 5.0;
    
#ifndef _MADARA_NO_KARL_
    // inform sleeping threads of new queued events
    control_plane_.wait ("queued > 0 || terminated", wait_settings);
#endif // _MADARA_NO_KARL_

    MADARA_DEBUG (MADARA_LOG_MAJOR_EVENT, (LM_DEBUG, 
      DLINFO "Timed_Executor::remove: " \
      "Condition has changed. Thread waking up.\n"));

    cur_event.second = 0;
    cur_time.set (0.0);
  }

  return cur_time;
}

void
Madara::Knowledge_Engine::Timed_Executor::shutdown (void)
{
  if (thread_info_)
  {
    control_plane_.set (terminated_, Knowledge_Record::Integer (1));

    // wait for all threads to complete
    enter_barrier ();

    Madara::Utility::sleep (1.0);

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
Madara::Knowledge_Engine::Timed_Executor::is_shutdown (void)
{
  return control_plane_.get (terminated_).to_integer () != 0;
}

void
Madara::Knowledge_Engine::Timed_Executor::launch_threads (
  unsigned int num_threads)
{
  control_plane_.set (terminated_, Knowledge_Record::Integer (0));

  // shutdown any existing threads
  if (thread_info_)
  {
    shutdown ();
  }

  thread_info_ = new Timed_Event_Thread_Info [num_threads];
  threads_ = new Timed_Event_Thread * [num_threads];
  num_threads_ = num_threads;
  control_plane_.set (threads_ref_, num_threads_);

  for (unsigned int i = 0; i < num_threads; ++i)
  {
    thread_info_[i].id = i;
    thread_info_[i].executor = this;
    thread_info_[i].control_plane = &control_plane_;
    threads_[i] = new Timed_Event_Thread (thread_info_[i]);
  }
}

ACE_Time_Value
Madara::Knowledge_Engine::Timed_Executor::time_until_next (void)
{
  // get the current time
  ACE_Time_Value cur_time = ACE_High_Res_Timer::gettimeofday ();

  // calculate the time left before next event is due
  cur_time = events_.top ().first - cur_time;

  // return the time
  return cur_time;
}

void 
Madara::Knowledge_Engine::Timed_Executor::lock (void)
{
  mutex_.acquire ();
}

void 
Madara::Knowledge_Engine::Timed_Executor::unlock (void)
{
  mutex_.release ();
}

Madara::Knowledge_Engine::Event
Madara::Knowledge_Engine::fill_event (
  Knowledge_Base & knowledge, const std::string & expression,
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
Madara::Knowledge_Engine::Timed_Executor::enter_barrier (void)
{
  Wait_Settings settings;
  settings.poll_frequency = .5;
  settings.max_wait_time = -1;

  MADARA_DEBUG (MADARA_LOG_MAJOR_EVENT, (LM_DEBUG, 
    DLINFO "Timed_Executor::enter_barrier: " \
    "Entering barrier.\n"));
  
#ifndef _MADARA_NO_KARL_

  control_plane_.wait (
    "closed = 0 ;>\n"
    ".i [0->threads) (thread.{.i}.closed => ++closed) ;>\n"
    "closed == threads", settings);
  
#endif // _MADARA_NO_KARL_

  MADARA_DEBUG (MADARA_LOG_MAJOR_EVENT, (LM_DEBUG, 
    DLINFO "Timed_Executor::enter_barrier: " \
    "Leaving barrier.\n"));
}

void 
Madara::Knowledge_Engine::Timed_Executor::clear_queue (void)
{
  Guard guard (mutex_);

  while (events_.size () > 0)
  {
    Timed_Event cur_event (events_.top ());
    delete cur_event.second;
    events_.pop ();
  }
}

Madara::Knowledge_Record::Integer
Madara::Knowledge_Engine::Timed_Executor::num_threads (void)
{
  Guard guard (mutex_);

  return num_threads_;
}
