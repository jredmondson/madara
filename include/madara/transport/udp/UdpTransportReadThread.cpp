#include "madara/transport/udp/UdpTransportReadThread.h"

#include "madara/utility/Utility.h"
#include "madara/transport/ReducedMessageHeader.h"

#include <iostream>

namespace madara
{
namespace transport
{
UdpTransportReadThread::UdpTransportReadThread(UdpTransport& transport)
  : transport_(transport)
{
}

void UdpTransportReadThread::init(knowledge::KnowledgeBase& knowledge)
{
  const QoSTransportSettings& settings_ = transport_.settings_;

  context_ = &(knowledge.get_context());

  // setup the receive buffer
  if(settings_.queue_length > 0)
    buffer_ = new char[settings_.queue_length];

  madara_logger_log(this->context_->get_logger(), logger::LOG_MAJOR,
      "UdpTransportReadThread::init:"
      " UdpTransportReadThread started with queue length %d\n",
      settings_.queue_length);

  if(context_)
  {
    // check for an on_data_received ruleset
    if(settings_.on_data_received_logic.length() != 0)
    {
      madara_logger_log(this->context_->get_logger(), logger::LOG_MAJOR,
          "UdpTransportReadThread::init:"
          " setting rules to %s\n",
          settings_.on_data_received_logic.c_str());

#ifndef _MADARA_NO_KARL_
      expression::Interpreter interpreter;
      on_data_received_ = context_->compile(settings_.on_data_received_logic);
#endif  // _MADARA_NO_KARL_
    }
    else
    {
      madara_logger_log(this->context_->get_logger(), logger::LOG_MAJOR,
          "UdpTransportReadThread::init:"
          " no permanent rules were set\n");
    }

    if(settings_.debug_to_kb_prefix != "")
    {
      knowledge::KnowledgeBase kb;
      kb.use(*context_);
      received_packets_.set_name(
          settings_.debug_to_kb_prefix + ".received_packets", kb);
      failed_receives_.set_name(
          settings_.debug_to_kb_prefix + ".failed_receives", kb);
      received_data_max_.set_name(
          settings_.debug_to_kb_prefix + ".received_data_max", kb);
      received_data_min_.set_name(
          settings_.debug_to_kb_prefix + ".received_data_min", kb);
      received_data_.set_name(
          settings_.debug_to_kb_prefix + ".received_data", kb);
    }
  }
}

void UdpTransportReadThread::cleanup(void) {}

void UdpTransportReadThread::rebroadcast(const char* print_prefix,
    MessageHeader* header, const knowledge::KnowledgeMap& records)
{
  const QoSTransportSettings& settings_ = transport_.settings_;

  int64_t buffer_remaining = (int64_t)settings_.queue_length;
  char* buffer = buffer_.get_ptr();
  int result(0);

  if(!settings_.no_sending && records.size () > 0)
  {
    result = prep_rebroadcast(*context_, buffer, buffer_remaining, settings_,
        print_prefix, header, records, transport_.packet_scheduler_);

    if(result > 0)
    {
      uint64_t clock = records.begin()->second.clock;
      result = transport_.send_message(buffer, result, clock);

      if(result > 0)
      {
        transport_.send_monitor_.add(result);
        if(settings_.debug_to_kb_prefix != "")
        {
          transport_.sent_data += result;
          ++transport_.sent_packets;
          if(transport_.sent_data_max < result)
          {
            transport_.sent_data_max = result;
          }
          if(transport_.sent_data_min > result ||
              transport_.sent_data_min == 0)
          {
            transport_.sent_data_min = result;
          }
        }
      }
      else
      {
        if(settings_.debug_to_kb_prefix != "")
        {
          ++transport_.failed_sends;
        }
      }

      madara_logger_log(this->context_->get_logger(), logger::LOG_MAJOR,
          "%s:"
          " Send bandwidth = %" PRIu64 " B/s\n",
          print_prefix, transport_.send_monitor_.get_bytes_per_second());
    }
  }
}

void UdpTransportReadThread::run(void)
{
  const QoSTransportSettings& settings_ = transport_.settings_;

  if(settings_.no_receiving)
  {
    return;
  }

  // allocate a buffer to send
  char* buffer = buffer_.get_ptr();
  static const char print_prefix[] = "UdpTransportReadThread::run";

  madara_logger_log(this->context_->get_logger(), logger::LOG_MINOR,
      "%s:"
      " entering main service loop.\n",
      print_prefix);

  if(buffer == 0)
  {
    madara_logger_log(this->context_->get_logger(), logger::LOG_EMERGENCY,
        "%s:"
        " Unable to allocate buffer of size " PRIu32 ". Exiting thread.\n",
        print_prefix, settings_.queue_length);

    return;
  }

  madara_logger_log(this->context_->get_logger(), logger::LOG_MINOR,
      "%s: entering a recv on the socket.\n", print_prefix);

  udp::endpoint remote;
  boost::system::error_code err;
  size_t bytes_read = transport_.socket_.receive_from(
      asio::buffer((void*)buffer, settings_.queue_length), remote,
      udp::socket::message_flags{}, err);

  if(err == asio::error::would_block || bytes_read == 0)
  {
    madara_logger_log(this->context_->get_logger(), logger::LOG_MINOR,
        "%s: no bytes to read. Proceeding to next wait\n", print_prefix);

    if(settings_.debug_to_kb_prefix != "")
    {
      ++failed_receives_;
    }

    return;
  }
  else if(err)
  {
    madara_logger_log(this->context_->get_logger(), logger::LOG_MINOR,
        "%s: unexpected error: %s. Proceeding to next wait\n", print_prefix,
        err.message().c_str());

    if(settings_.debug_to_kb_prefix != "")
    {
      ++failed_receives_;
    }

    return;
  }

  if(settings_.debug_to_kb_prefix != "")
  {
    received_data_ += bytes_read;
    ++received_packets_;

    if(received_data_max_ < bytes_read)
    {
      received_data_max_ = bytes_read;
    }
    if(received_data_min_ > bytes_read || received_data_min_ == 0)
    {
      received_data_min_ = bytes_read;
    }
  }

  if(remote.address().to_string() != "")
  {
    madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_MAJOR,
        "%s:"
        " received a message header of %lld bytes from %s:%d\n",
        print_prefix, (long long)bytes_read,
        remote.address().to_string().c_str(), (int)remote.port());
  }
  else
  {
    madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_WARNING,
        "%s:"
        " received %lld bytes from unknown host\n",
        print_prefix, (long long)bytes_read);
  }

  MessageHeader* header = 0;

  std::stringstream remote_host;
  remote_host << remote.address().to_string();
  remote_host << ":";
  remote_host << remote.port();

  knowledge::KnowledgeMap rebroadcast_records;

  process_received_update(buffer, (uint32_t)bytes_read, transport_.id_,
      *context_, settings_, transport_.send_monitor_,
      transport_.receive_monitor_, rebroadcast_records,
#ifndef _MADARA_NO_KARL_
      on_data_received_,
#endif  // _MADARA_NO_KARL_
      print_prefix, remote_host.str().c_str(), header);

  if(header)
  {
    if(header->ttl > 0 && rebroadcast_records.size() > 0 &&
        settings_.get_participant_ttl() > 0)
    {
      --header->ttl;
      header->ttl = std::min(settings_.get_participant_ttl(), header->ttl);

      rebroadcast(print_prefix, header, rebroadcast_records);
    }

    // delete header
    delete header;
  }

  madara_logger_log(this->context_->get_logger(), logger::LOG_MAJOR,
      "%s:"
      " finished iteration.\n",
      print_prefix);
}
}
}
