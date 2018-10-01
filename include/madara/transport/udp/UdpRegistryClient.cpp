#include "madara/transport/udp/UdpRegistryClient.h"
#include "madara/transport/TransportContext.h"

#include "madara/knowledge/KnowledgeRecord.h"
#include "madara/transport/ReducedMessageHeader.h"
#include "madara/utility/Utility.h"

#include <iostream>

namespace madara
{
namespace transport
{
UdpRegistryClient::UdpRegistryClient(const std::string& id,
    knowledge::ThreadSafeContext& context, TransportSettings& config,
    bool launch_transport)
  : UdpTransport(id, context, config, false)
{
  endpoints_.set_name(
      "domain." + config.write_domain + ".endpoints", knowledge_);

  if (launch_transport)
    setup();
}

int UdpRegistryClient::setup(void)
{
  // call base setup method to initialize certain common variables
  int ret = UdpTransport::setup();
  if (ret < 0) {
    return ret;
  }

  if (addresses_.size() > 0) {
    servers_.clear();

    // UdpTransport::setup puts hosts into addresses_; move all but first to
    // servers_ instead
    for (size_t i = 1; i < addresses_.size(); ++i) {
      madara_logger_log(context_.get_logger(), logger::LOG_MAJOR,
          "UdpRegistryClient::setup:"
          " adding server %s:%d to registry lookup list\n",
          addresses_[i].address().to_string().c_str(),
          (int)addresses_[i].port());

      servers_.emplace_back(std::move(addresses_[i]));
    }

    addresses_.resize(1);
  }

  return this->validate_transport();
}

void UdpRegistryClient::send_register(void)
{
  const char* print_prefix = "UdpRegistryClient::register";

  long result(0);
  uint64_t bytes_sent = 0;

  if (servers_.size() > 0) {
    // Register messages always use the message header to include domain
    MessageHeader header;
    header.type = transport::REGISTER;
    strncpy(
        header.originator, this->id_.c_str(), sizeof(header.originator) - 1);
    strncpy(header.domain, this->settings_.write_domain.c_str(),
        sizeof(header.domain) - 1);
    header.updates = 0;
    header.clock = context_.get_clock();
    // compute size of this header
    header.size = header.encoded_size();

    int64_t buffer_remaining = settings_.queue_length;

    char* update = header.write(buffer_.get_ptr(), buffer_remaining);

    result = (long)(update - buffer_.get_ptr());

    for (const auto& server : servers_) {
      madara_logger_log(context_.get_logger(), logger::LOG_MAJOR,
          "%s:"
          " Sending register of size %d to %s:%d\n",
          print_prefix, (int)result, server.address().to_string().c_str(),
          server.port());

      ssize_t actual_sent = send_buffer(server, buffer_.get_ptr(), result);

      if (actual_sent > 0) {
        bytes_sent += actual_sent;

        madara_logger_log(context_.get_logger(), logger::LOG_MAJOR,
            "%s:"
            " Sent register of size %d to %s\n",
            print_prefix, (int)actual_sent,
            server.address().to_string().c_str(), server.port());

        send_monitor_.add((uint32_t)actual_sent);
      }
    }
  } else {
    madara_logger_log(context_.get_logger(), logger::LOG_MAJOR,
        "%s:"
        " ERROR: no servers available for sending. Failed to register.\n");
  }
}

long UdpRegistryClient::send_data(
    const knowledge::VariableReferenceMap& orig_updates)
{
  if (!settings_.no_sending) {
    this->endpoints_.sync_keys();

    std::vector<std::string> hosts;
    this->addresses_.resize(1);
    this->endpoints_.keys(hosts);

    for (auto& host : hosts) {
      auto addr_parts = utility::parse_address(std::move(host));
      auto addr = ip::address::from_string(addr_parts.first);
      addresses_.emplace_back(addr, addr_parts.second);

      madara_logger_log(context_.get_logger(), logger::LOG_MINOR,
          "UdpRegistryClient::send_data:"
          " adding %s:%d\n",
          addresses_.back().address().to_string().c_str(),
          addresses_.back().port());
    }

    send_register();
  }
  return UdpTransport::send_data(orig_updates);
}
}
}
