#ifndef _MADARA_BASIC_ASIO_TRANSPORT_H_
#define _MADARA_BASIC_ASIO_TRANSPORT_H_

#include <string>

#include "madara/MadaraExport.h"
#include "madara/utility/ScopedArray.h"
#include "madara/transport/QoSTransportSettings.h"
#include "madara/transport/Transport.h"
#include "madara/knowledge/KnowledgeBase.h"
#include "madara/knowledge/KnowledgeRecord.h"
#include "madara/expression/ExpressionTree.h"
#include "madara/transport/BandwidthMonitor.h"
#include "madara/threads/Threader.h"
#include "madara/Boost.h"

namespace madara
{
namespace transport
{
namespace asio = boost::asio;
namespace ip = boost::asio::ip;
using udp = boost::asio::ip::udp;

class MADARA_EXPORT BasicASIOTransport : public Base
{
public:
  BasicASIOTransport(const std::string& id,
      madara::knowledge::ThreadSafeContext& context, TransportSettings& config);

  ~BasicASIOTransport() = 0;

  void close() override;

  int setup() override;

  static const double default_read_hertz;

protected:
  virtual int setup_socket(udp::socket& socket);
  virtual int setup_read_socket();
  virtual int setup_write_socket();
  virtual int setup_sockets();

  virtual int setup_read_threads();
  virtual int setup_read_thread(double hertz, const std::string& name) = 0;

  /// knowledge base for threads to use
  knowledge::KnowledgeBase knowledge_;

  /// Boost::ASIO IO context
  asio::io_service io_service_;

  /// threads for reading knowledge updates
  threads::Threader read_threads_;

  /// holds all multicast addresses we are sending to
  std::vector<udp::endpoint> addresses_;

  /// underlying socket
  udp::socket socket_{io_service_};

  friend class UdpTransportReadThread;
  friend class MulticastTransportReadThread;
};
}
}

#endif
