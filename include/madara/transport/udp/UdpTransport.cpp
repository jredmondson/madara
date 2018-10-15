#include "madara/transport/udp/UdpTransport.h"
#include "madara/transport/udp/UdpTransportReadThread.h"
#include "madara/transport/TransportContext.h"

#include "madara/knowledge/KnowledgeRecord.h"
#include "madara/transport/ReducedMessageHeader.h"
#include "madara/utility/Utility.h"

#include <iostream>

namespace madara
{
namespace transport
{
UdpTransport::UdpTransport(const std::string& id,
    knowledge::ThreadSafeContext& context, TransportSettings& config,
    bool launch_transport)
  : BasicASIOTransport(id, context, config),
    enforcer_(1 / config.max_send_hertz)
{
  if(launch_transport)
    setup();

  if(config.debug_to_kb_prefix != "")
  {
    knowledge::KnowledgeBase kb;
    kb.use(context);

    sent_packets.set_name(config.debug_to_kb_prefix + ".sent_packets", kb);
    failed_sends.set_name(config.debug_to_kb_prefix + ".failed_sends", kb);
    sent_data_max.set_name(config.debug_to_kb_prefix + ".sent_data_max", kb);
    sent_data_min.set_name(config.debug_to_kb_prefix + ".sent_data_min", kb);
    sent_data.set_name(config.debug_to_kb_prefix + ".sent_data", kb);
  }
}

int UdpTransport::reliability(void) const
{
  return BEST_EFFORT;
}

int UdpTransport::reliability(const int&)
{
  return BEST_EFFORT;
}

int UdpTransport::setup_read_thread(double hertz, const std::string& name)
{
  madara_logger_log(context_.get_logger(), logger::LOG_MAJOR,
      "UdpTransport::setup_read_thread:"
      " Starting UdpTransport read thread: %s\n",
      name.c_str());
  read_threads_.run(hertz, name, new UdpTransportReadThread(*this));

  return 0;
}

int UdpTransport::setup_read_socket(void)
{
  if(BasicASIOTransport::setup_read_socket() < 0)
  {
    return -1;
  }

  try
  {
    socket_.non_blocking(true);

    socket_.bind(udp::endpoint(ip::address_v4::any(), addresses_[0].port()));

    madara_logger_log(context_.get_logger(), logger::LOG_MAJOR,
        "UdpTransport::setup_read_socket:"
        " Bound to port: %d\n",
        (int)addresses_[0].port());
  }
  catch (const boost::system::system_error& e)
  {
    madara_logger_log(context_.get_logger(), logger::LOG_MAJOR,
        "UdpTransport::setup:"
        " Error setting up read socket: %s\n",
        e.what());

    this->invalidate_transport();
    return -1;
  }

  return 0;
}

int UdpTransport::setup_write_socket(void)
{
  if(BasicASIOTransport::setup_write_socket() < 0)
  {
    return -1;
  }

  return 0;
}

long UdpTransport::send_buffer(
    const udp::endpoint& target, const char* buf, size_t size)
{
  uint64_t bytes_sent = 0;

  int send_attempts = -1;
  ssize_t actual_sent = -1;

  while (actual_sent < 0 && (settings_.resend_attempts < 0 ||
                                send_attempts < settings_.resend_attempts))
  {
    if(settings_.max_send_hertz > 0)
    {
      enforcer_.sleep_until_next();
    }

    // send the fragment
    try
    {
      actual_sent = socket_.send_to(asio::buffer(buf, size), target);
    }
    catch (const boost::system::system_error& e)
    {
      madara_logger_log(context_.get_logger(), logger::LOG_MAJOR,
          "UdpTransport::send_buffer:"
          " Error sending packet to %s:%d: %s\n",
          target.address().to_string().c_str(), (int)target.port(), e.what());

      // ensure erroneous data is not being used
      actual_sent = -1;
    }

    ++send_attempts;
    if(settings_.debug_to_kb_prefix != "")
    {
      ++sent_packets;
    }

    if(actual_sent > 0)
    {
      madara_logger_log(context_.get_logger(), logger::LOG_MAJOR,
          "UdpTransport::send_buffer: Sent %d byte packet to %s:%d\n",
          (int)actual_sent, target.address().to_string().c_str(),
          (int)target.port());

      bytes_sent += actual_sent;

      if(settings_.debug_to_kb_prefix != "")
      {
        sent_data += actual_sent;
        if(sent_data_max < actual_sent)
        {
          sent_data_max = actual_sent;
        }
        if(sent_data_min > actual_sent || sent_data_min == 0)
        {
          sent_data_min = actual_sent;
        }
      }
    }
    else
    {
      if(settings_.debug_to_kb_prefix != "")
      {
        ++failed_sends;
      }
    }
  }

  return (long)bytes_sent;
}

long UdpTransport::send_message(const char* buf, size_t packet_size,
  uint64_t clock)
{
  static const char print_prefix[] = "UdpTransport::send_message";

  uint64_t bytes_sent = 0;

  if(packet_size > settings_.max_fragment_size)
  {
    FragmentMap map;

    madara_logger_log(context_.get_logger(), logger::LOG_MAJOR,
        "%s:"
        " fragmenting %" PRIu64 " byte packet (%" PRIu32
        " bytes is max fragment size)\n",
        print_prefix, packet_size, settings_.max_fragment_size);

    // fragment the message
    frag(buf, packet_size, id_.c_str (), settings_.write_domain.c_str(),
      clock, utility::get_time(), 0, 0,
      settings_.max_fragment_size, map);

    int j(0);
    for(FragmentMap::iterator i = map.begin(); i != map.end(); ++i, ++j)
    {
      madara_logger_log(context_.get_logger(), logger::LOG_MAJOR,
          "%s:"
          " Sending fragment %d\n",
          print_prefix, j);

      for(const auto& address : addresses_)
      {
        if(pre_send_buffer(&address - &*addresses_.begin()))
        {
          bytes_sent += send_buffer(
              address, i->second.get(),
              (size_t)MessageHeader::get_size(i->second.get()));
        }
      }

      // sleep between fragments, if such a slack time is specified
      if(settings_.slack_time > 0)
        utility::sleep(settings_.slack_time);
    }

    madara_logger_log(context_.get_logger(), logger::LOG_MAJOR,
        "%s:"
        " Sent fragments totalling %" PRIu64 " bytes\n",
        print_prefix, bytes_sent);

    delete_fragments(map);
  }
  else
  {
    madara_logger_log(context_.get_logger(), logger::LOG_MAJOR,
        "%s:"
        " Sending packet of size %ld\n",
        print_prefix, packet_size);

    for(const auto& address : addresses_)
    {
      size_t addr_index = &address - &*addresses_.begin();
      bool should_send = pre_send_buffer(addr_index);

      madara_logger_log(context_.get_logger(), logger::LOG_MINOR,
          "%s:"
          " Deciding to send to %s:%d (index %d): %d\n",
          print_prefix, address.address().to_string().c_str(),
          (int)address.port(), addr_index, should_send);

      if(should_send)
      {
        bytes_sent += send_buffer(address, buf, (size_t)packet_size);
      }
    }
  }

  if(bytes_sent > 0)
  {
    send_monitor_.add((uint32_t)bytes_sent);
  }

  madara_logger_log(context_.get_logger(), logger::LOG_MAJOR,
      "%s:"
      " Send bandwidth = %" PRIu64 " B/s\n",
      print_prefix, send_monitor_.get_bytes_per_second());

  return (long)bytes_sent;
}

long UdpTransport::send_data(
    const knowledge::KnowledgeMap& orig_updates)
{
  long result(0);
  const char* print_prefix = "UdpTransport::send_data";

  if(!settings_.no_sending && orig_updates.size() != 0)
  {
    result = prep_send(orig_updates, print_prefix);

    if(addresses_.size() > 0 && result > 0)
    {
      result = send_message(
          buffer_.get_ptr(), result, orig_updates.begin()->second.clock);
    }
  }

  return result;
}
}
}
