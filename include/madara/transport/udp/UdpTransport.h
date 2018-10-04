#ifndef _MADARA_UDP_TRANSPORT_H_
#define _MADARA_UDP_TRANSPORT_H_

#include <string>

#include "madara/MadaraExport.h"
#include "madara/transport/BasicASIOTransport.h"
#include "madara/transport/Transport.h"
#include "madara/threads/Threader.h"
#include "madara/utility/EpochEnforcer.h"
#include "madara/knowledge/containers/Integer.h"

#include <string>
#include <map>

#include "madara/Boost.h"

namespace madara
{
namespace transport
{
namespace asio = boost::asio;
namespace ip = boost::asio::ip;
using udp = boost::asio::ip::udp;

/**
 * @class UdpTransport
 * @brief UDP-based transport for knowledge. This transport currently
 * supports the following transport settings:<br />
 *        1) multiple host:port pairing with self first in vector<br />
 *        2) the reduced message header<br />
 *        3) the normal message header<br />
 *        4) domain differentiation<br />
 *        5) on data received logic<br />
 *        6) multi-assignment of records<br />
 *        7) rebroadcasting<br />
 **/
class MADARA_EXPORT UdpTransport : public BasicASIOTransport
{
public:
  UdpTransport(const std::string& id,
      madara::knowledge::ThreadSafeContext& context, TransportSettings& config,
      bool launch_transport);

  /**
   * Accesses reliability setting
   * @return  whether we are using reliable dissemination or not
   **/
  int reliability(void) const;

  /**
   * Sets the reliability setting
   * @return  the changed setting
   **/
  int reliability(const int& setting);

  /**
   * Sends a list of knowledge updates to listeners
   * @param   updates listing of all updates that must be sent
   * @return  result of write operation or -1 if we are shutting down
   **/
  long send_data(
      const madara::knowledge::VariableReferenceMap& updates) override;

  /// sent packets
  knowledge::containers::Integer sent_packets;

  /// failed sends
  knowledge::containers::Integer failed_sends;

  /// sent data
  knowledge::containers::Integer sent_data;

  /// max data sent
  knowledge::containers::Integer sent_data_max;

  /// min data sent
  knowledge::containers::Integer sent_data_min;

protected:
  int setup_read_socket() override;
  int setup_write_socket() override;
  int setup_read_thread(double hertz, const std::string& name) override;

  long send_message(const char* buf, size_t size);
  long send_buffer(const udp::endpoint& target, const char* buf, size_t size);
  virtual bool pre_send_buffer(size_t addr_index)
  {
    return addr_index != 0;
  }

  /// enforces epochs when user specifies a max_send_hertz
  utility::EpochEnforcer<utility::Clock> enforcer_;

  friend class UdpTransportReadThread;
};
}
}

#include "madara/transport/udp/UdpTransportReadThread.h"

#endif  // _MADARA_UDP_TRANSPORT_H_
