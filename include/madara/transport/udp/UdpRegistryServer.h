#ifndef _MADARA_UDP_REGISTRY_SERVER_H_
#define _MADARA_UDP_REGISTRY_SERVER_H_

#include <string>

#include "madara/MadaraExport.h"
#include "madara/transport/udp/UdpTransport.h"
#include "madara/transport/Transport.h"
#include "madara/threads/Threader.h"
#include "madara/knowledge/containers/Map.h"

#include <string>
#include <map>

namespace madara
{
namespace transport
{
/**
 * @class UdpRegistryServer
 * @brief UDP-based server that handles a registry of UDP endpoints,
 *        which makes it ideal for any NAT-protected agents
 **/
class MADARA_EXPORT UdpRegistryServer : public UdpTransport
{
public:
  enum
  {
    ERROR_UDP_NOT_STARTED = -1,
  };

  static const int PROFILES = 1;

  /**
   * Constructor
   * @param   id   unique identifer - usually a combination of host:port
   * @param   context  knowledge context
   * @param   config   transport configuration settings
   * @param   launch_transport  whether or not to launch this transport
   **/
  UdpRegistryServer(const std::string& id,
      madara::knowledge::ThreadSafeContext& context, TransportSettings& config,
      bool launch_transport);

  /**
   * Sends a list of knowledge updates to listeners
   * @param   updates listing of all updates that must be sent
   * @return  result of write operation or -1 if we are shutting down
   **/
  long send_data(
      const madara::knowledge::KnowledgeMap& updates) override;

  int setup(void) override;

protected:
  size_t server_count_;

  knowledge::containers::Map endpoints_;
};
}
}

#endif  // _MADARA_UDP_REGISTRY_SERVER_H_
