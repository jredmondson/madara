/* -*- C++ -*- */
#ifndef _BANDWIDTH_MONITOR_INL_
#define _BANDWIDTH_MONITOR_INL_

#include "Bandwidth_Monitor.h"

inline time_t
madara::transport::Bandwidth_Monitor::update_utilization (void)
{
  Bandwidth_Guard guard (mutex_);
  
  time_t cur_time = time (NULL);
  time_t earliest_time = cur_time - window_;
  
  Bandwidth_Messages::iterator begin = messages_.begin ();
  Bandwidth_Messages::iterator end = begin;

  for (end = begin; end != messages_.end (); ++end)
  {
    /**
     * if we have an old message, remove it from utilization and schedule
     * a deletion
     **/
    if (end->first < earliest_time)
    {
      utilization_ -= end->second;
    }
    else
      break;
  }

  if (end != begin)
    messages_.erase (begin, end);

  return cur_time;
}

#endif // _BANDWIDTH_MONITOR_INL_