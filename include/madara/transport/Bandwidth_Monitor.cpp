#include "Bandwidth_Monitor.h"
#include "madara/utility/Utility.h"
#include "madara/logger/Global_Logger.h"
#include "madara/utility/inttypes.h"


madara::transport::Bandwidth_Monitor::Bandwidth_Monitor (
  time_t window_in_secs)
  : utilization_ (0), window_ (window_in_secs)
{
}

madara::transport::Bandwidth_Monitor::Bandwidth_Monitor (
  const Bandwidth_Monitor & rhs)
  : messages_ (rhs.messages_), utilization_ (rhs.utilization_),
    window_ (rhs.window_)
{
}

madara::transport::Bandwidth_Monitor::~Bandwidth_Monitor ()
{
}

void
madara::transport::Bandwidth_Monitor::operator= (
  const Bandwidth_Monitor & rhs)
{
  Bandwidth_Guard guard (mutex_);
  if (this != &rhs)
  {
    messages_ = rhs.messages_;
    utilization_ = rhs.utilization_;
    window_ = rhs.window_;
  }
}

void
madara::transport::Bandwidth_Monitor::set_window (
  time_t window_in_secs)
{
  Bandwidth_Guard guard (mutex_);
  
  window_ = window_in_secs;
}

void
madara::transport::Bandwidth_Monitor::add (uint64_t size)
{
  Bandwidth_Guard guard (mutex_);
  
  utilization_ += size;

  time_t cur_time = update_utilization ();
  
  Bandwidth_Record entry (cur_time, size);
  
  messages_.push_back (entry);
}

void
madara::transport::Bandwidth_Monitor::add (
  time_t timestamp, uint64_t size)
{
  Bandwidth_Guard guard (mutex_);
  
  utilization_ += size;

  Bandwidth_Record entry (timestamp, size);

  update_utilization ();

  messages_.push_back (entry);
}

bool
madara::transport::Bandwidth_Monitor::is_bandwidth_violated (
  int64_t limit)
{
  bool result = false;

  if (limit >= 0 && uint64_t (limit) > get_utilization ())
    result = true;

  return result;
}

uint64_t
madara::transport::Bandwidth_Monitor::get_utilization (void)
{
  Bandwidth_Guard guard (mutex_);

  update_utilization ();

  return utilization_;
}

uint64_t
madara::transport::Bandwidth_Monitor::get_bytes_per_second (void)
{
  Bandwidth_Guard guard (mutex_);
  
  update_utilization ();

  return utilization_ / (uint64_t)window_;
}

void
madara::transport::Bandwidth_Monitor::clear (void)
{
  Bandwidth_Guard guard (mutex_);
  messages_.clear ();
  utilization_ = 0;
}

void
madara::transport::Bandwidth_Monitor::print_utilization (
  void)
{
  Bandwidth_Guard guard (mutex_);

  update_utilization ();

  madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_ALWAYS, "Bandwidth: %d messages "
    "for %" PRIu64 " bytes over %lld window (%" PRIu64 " B/s)\n", messages_.size (),
    utilization_, (long long)window_, get_bytes_per_second ());
}


size_t
madara::transport::Bandwidth_Monitor::get_number_of_messages (
  void)
{
  Bandwidth_Guard guard (mutex_);
  
  update_utilization ();

  return messages_.size ();
}
