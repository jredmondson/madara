#ifndef _MADARA_MULTICAST_TRANSPORT_READ_THREAD_H_
#define _MADARA_MULTICAST_TRANSPORT_READ_THREAD_H_

/**
 * @file MulticastTransportReadThread.h
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains the MulticastTransportReadThread class, which provides a
 * multicast transport for reading knowledge updates in KaRL
 **/

#include <string>

#include "madara/utility/ScopedArray.h"
#include "madara/knowledge/ThreadSafeContext.h"
#include "madara/transport/BandwidthMonitor.h"
#include "madara/transport/QoSTransportSettings.h"
#include "madara/expression/ExpressionTree.h"
#include "madara/transport/Transport.h"
#include "madara/transport/MessageHeader.h"
#include "madara/transport/udp/UdpTransport.h"
#include "madara/threads/BaseThread.h"
#include "madara/Boost.h"

namespace madara
{
namespace transport
{
namespace asio = boost::asio;
namespace ip = boost::asio::ip;
using udp = boost::asio::ip::udp;

/**
 * @class MulticastTransportReadThread
 * @brief Thread for reading knowledge updates through a Multicast
 *        datagram socket
 **/
class MulticastTransportReadThread : public UdpTransportReadThread
{
public:
  using UdpTransportReadThread::UdpTransportReadThread;

  /**
   * Cleanup function called by thread manager
   **/
  void cleanup(void) override;
};
}
}

#endif  // _MADARA_MULTICAST_TRANSPORT_READ_THREAD_H_
