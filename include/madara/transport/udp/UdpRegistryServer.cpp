#include "madara/transport/udp/UdpRegistryServer.h"
#include "madara/transport/TransportContext.h"

#include "madara/knowledge/KnowledgeRecord.h"
#include "madara/transport/ReducedMessageHeader.h"
#include "madara/utility/Utility.h"

#include <iostream>

madara::transport::UdpRegistryServer::UdpRegistryServer(const std::string& id,
    madara::knowledge::ThreadSafeContext& context, TransportSettings& config,
    bool launch_transport)
  : UdpTransport(id, context, config, false)
{
  endpoints_.set_name(
      "domain." + config.write_domain + ".endpoints", knowledge_);

  if (launch_transport)
    setup();
}

int madara::transport::UdpRegistryServer::setup(void)
{
  // call base setup method to initialize certain common variables
  int ret = UdpTransport::setup();

  server_count_ = addresses_.size();

  return ret;
}

long madara::transport::UdpRegistryServer::send_data(
    const madara::knowledge::VariableReferenceMap& orig_updates)
{
  if (!settings_.no_sending) {
    this->endpoints_.sync_keys();

    std::vector<std::string> hosts;
    this->addresses_.resize(server_count_);
    this->endpoints_.keys(hosts);

    for (auto& host : hosts) {
      auto addr_parts = utility::parse_address(std::move(host));
      auto addr = ip::address::from_string(addr_parts.first);
      addresses_.emplace_back(addr, addr_parts.second);

      madara_logger_log(context_.get_logger(), logger::LOG_MINOR,
          "UdpRegistryServer::send_data:"
          " adding %s:%d\n",
          addresses_.back().address().to_string().c_str(),
          addresses_.back().port());
    }
  }
  return UdpTransport::send_data(orig_updates);
}
