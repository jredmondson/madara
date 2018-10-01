#include "TransportContext.h"
#include "madara/utility/Utility.h"

madara::transport::TransportContext::TransportContext(int64_t operation,
    uint64_t receive_bandwidth, uint64_t send_bandwidth, uint64_t message_time,
    uint64_t current_time, const std::string& domain,
    const std::string& originator, const std::string& endpoint)
  : operation_(operation),
    receive_bandwidth_(receive_bandwidth),
    send_bandwidth_(send_bandwidth),
    message_time_(message_time),
    current_time_(current_time),
    domain_(domain),
    originator_(originator),
    endpoint_(endpoint)
{
}

madara::transport::TransportContext::TransportContext(
    const TransportContext& rhs)
  : operation_(rhs.operation_),
    receive_bandwidth_(rhs.receive_bandwidth_),
    send_bandwidth_(rhs.send_bandwidth_),
    message_time_(rhs.message_time_),
    current_time_(rhs.current_time_),
    domain_(rhs.domain_),
    originator_(rhs.originator_),
    endpoint_(rhs.endpoint_),
    records_(rhs.records_)
{
}

madara::transport::TransportContext::~TransportContext() {}

void madara::transport::TransportContext::operator=(const TransportContext& rhs)
{
  if (this != &rhs)
  {
    operation_ = rhs.operation_;
    send_bandwidth_ = rhs.send_bandwidth_;
    receive_bandwidth_ = rhs.receive_bandwidth_;
    message_time_ = rhs.message_time_;
    current_time_ = rhs.current_time_;
    domain_ = rhs.domain_;
    originator_ = rhs.originator_;
    endpoint_ = rhs.endpoint_;
    records_ = rhs.records_;
  }
}
