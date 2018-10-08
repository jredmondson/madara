#ifndef _TRANSPORT_CONTEXT_INL_
#define _TRANSPORT_CONTEXT_INL_

#include "TransportContext.h"

inline void madara::transport::TransportContext::clear_records(void)
{
  records_.clear();
}

inline int64_t madara::transport::TransportContext::get_operation(void) const
{
  return operation_;
}

inline uint64_t madara::transport::TransportContext::get_receive_bandwidth(
    void) const
{
  return receive_bandwidth_;
}

inline uint64_t madara::transport::TransportContext::get_send_bandwidth(
    void) const
{
  return send_bandwidth_;
}

inline uint64_t madara::transport::TransportContext::get_current_time(
    void) const
{
  return current_time_;
}

inline uint64_t madara::transport::TransportContext::get_message_time(
    void) const
{
  return message_time_;
}

inline void madara::transport::TransportContext::set_operation(
    int64_t operation)
{
  operation_ = operation;
}

inline void madara::transport::TransportContext::set_receive_bandwidth(
    uint64_t bandwidth)
{
  receive_bandwidth_ = bandwidth;
}

inline void madara::transport::TransportContext::set_send_bandwidth(
    uint64_t bandwidth)
{
  send_bandwidth_ = bandwidth;
}

inline void madara::transport::TransportContext::set_current_time(
    uint64_t current_time)
{
  current_time_ = current_time;
}

inline void madara::transport::TransportContext::set_message_time(
    uint64_t message_time)
{
  message_time_ = message_time;
}

inline void madara::transport::TransportContext::add_record(
    const std::string& key, const madara::knowledge::KnowledgeRecord& record)
{
  records_[key] = record;
}

inline const madara::knowledge::KnowledgeMap&
madara::transport::TransportContext::get_records(void) const
{
  return records_;
}

inline void madara::transport::TransportContext::set_domain(
    const std::string& domain)
{
  domain_ = domain;
}

inline const std::string& madara::transport::TransportContext::get_domain(
    void) const
{
  return domain_;
}

inline void madara::transport::TransportContext::set_originator(
    const std::string& originator)
{
  originator_ = originator;
}

inline const std::string& madara::transport::TransportContext::get_originator(
    void) const
{
  return originator_;
}

inline void madara::transport::TransportContext::set_endpoint(
    const std::string& endpoint)
{
  endpoint_ = endpoint;
}

inline const std::string& madara::transport::TransportContext::get_endpoint(
    void) const
{
  return endpoint_;
}

inline void madara::transport::TransportContext::set_records(
    const knowledge::KnowledgeMap& source)
{
  records_ = source;
}

#endif  // _TRANSPORT_CONTEXT_INL_