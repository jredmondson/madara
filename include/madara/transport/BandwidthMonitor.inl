/* -*- C++ -*- */
#ifndef _BANDWIDTH_MONITOR_INL_
#define _BANDWIDTH_MONITOR_INL_

#include "BandwidthMonitor.h"
#include "madara/utility/Utility.h"

inline time_t
madara::transport::BandwidthMonitor::update_utilization (void)
{
  MADARA_GUARD_TYPE guard (mutex_);
  
  time_t cur_time = time (NULL);
  time_t earliest_time = cur_time - window_;
  
  BandwidthMessages::iterator begin = messages_.begin ();
  BandwidthMessages::iterator end = begin;

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
