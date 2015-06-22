#include "Transport_Context.h"
#include "madara/utility/Utility.h"


Madara::Transport::Transport_Context::Transport_Context (int64_t operation,
  uint64_t receive_bandwidth, uint64_t send_bandwidth,
  uint64_t message_time, uint64_t current_time,
  const std::string & domain,
  const std::string & originator)
  : operation_ (operation),
    receive_bandwidth_ (receive_bandwidth),
    send_bandwidth_ (send_bandwidth),
    message_time_ (message_time),
    current_time_ (current_time),
    domain_ (domain),
    originator_ (originator)
{
}

Madara::Transport::Transport_Context::Transport_Context (
  const Transport_Context & rhs)
  : operation_ (rhs.operation_),
  receive_bandwidth_ (rhs.receive_bandwidth_),
  send_bandwidth_ (rhs.send_bandwidth_),
  message_time_ (rhs.message_time_),
  current_time_ (rhs.current_time_),
  domain_ (rhs.domain_),
  originator_ (rhs.originator_),
  records_ (rhs.records_)
{
}

Madara::Transport::Transport_Context::~Transport_Context ()
{
}

void
Madara::Transport::Transport_Context::operator= (
  const Transport_Context & rhs)
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
    records_ = rhs.records_;
  }
}
