#include "BandwidthMonitor.h"
#include "madara/utility/Utility.h"
#include "madara/logger/GlobalLogger.h"
#include "madara/utility/IntTypes.h"

madara::transport::BandwidthMonitor::BandwidthMonitor(time_t window_in_secs)
  : utilization_(0), window_(window_in_secs)
{
}

madara::transport::BandwidthMonitor::BandwidthMonitor(
    const BandwidthMonitor& rhs)
  : messages_(rhs.messages_),
    utilization_(rhs.utilization_),
    window_(rhs.window_)
{
}

madara::transport::BandwidthMonitor::~BandwidthMonitor() {}

void madara::transport::BandwidthMonitor::operator=(const BandwidthMonitor& rhs)
{
  MADARA_GUARD_TYPE guard(mutex_);
  if(this != &rhs)
  {
    messages_ = rhs.messages_;
    utilization_ = rhs.utilization_;
    window_ = rhs.window_;
  }
}

void madara::transport::BandwidthMonitor::set_window(time_t window_in_secs)
{
  MADARA_GUARD_TYPE guard(mutex_);

  window_ = window_in_secs;
}

void madara::transport::BandwidthMonitor::add(uint64_t size)
{
  MADARA_GUARD_TYPE guard(mutex_);

  utilization_ += size;

  time_t cur_time = update_utilization();

  BandwidthRecord entry(cur_time, size);

  messages_.push_back(entry);
}

void madara::transport::BandwidthMonitor::add(time_t timestamp, uint64_t size)
{
  MADARA_GUARD_TYPE guard(mutex_);

  utilization_ += size;

  BandwidthRecord entry(timestamp, size);

  update_utilization();

  messages_.push_back(entry);
}

bool madara::transport::BandwidthMonitor::is_bandwidth_violated(int64_t limit)
{
  bool result = false;

  if(limit >= 0 && uint64_t(limit) > get_utilization())
    result = true;

  return result;
}

uint64_t madara::transport::BandwidthMonitor::get_utilization(void)
{
  MADARA_GUARD_TYPE guard(mutex_);

  update_utilization();

  return utilization_;
}

uint64_t madara::transport::BandwidthMonitor::get_bytes_per_second(void)
{
  MADARA_GUARD_TYPE guard(mutex_);

  update_utilization();

  return utilization_ / (uint64_t)window_;
}

void madara::transport::BandwidthMonitor::clear(void)
{
  MADARA_GUARD_TYPE guard(mutex_);
  messages_.clear();
  utilization_ = 0;
}

void madara::transport::BandwidthMonitor::print_utilization(void)
{
  MADARA_GUARD_TYPE guard(mutex_);

  update_utilization();

  madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_ALWAYS,
      "Bandwidth: %d messages "
      "for %" PRIu64 " bytes over %lld window (%" PRIu64 " B/s)\n",
      messages_.size(), utilization_, (long long)window_,
      get_bytes_per_second());
}

size_t madara::transport::BandwidthMonitor::get_number_of_messages(void)
{
  MADARA_GUARD_TYPE guard(mutex_);

  update_utilization();

  return messages_.size();
}
