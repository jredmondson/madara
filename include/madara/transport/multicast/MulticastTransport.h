#ifndef _MADARA_MULTICAST_TRANSPORT_H_
#define _MADARA_MULTICAST_TRANSPORT_H_

/**
 * @file MulticastTransport.h
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains the MulticastTransport class, which provides a
 * multicast transport for sending knowledge updates in KaRL
 **/

#include <string>

#include "madara/MADARA_export.h"
#include "madara/utility/ScopedArray.h"
#include "madara/transport/QoSTransportSettings.h"
#include "madara/transport/Transport.h"
#include "madara/knowledge/KnowledgeBase.h"
#include "madara/knowledge/KnowledgeRecord.h"
#include "madara/expression/ExpressionTree.h"
#include "madara/transport/BandwidthMonitor.h"
#include "madara/threads/Threader.h"
#include "madara/Boost.h"
#include "madara/transport/BasicASIOTransport.h"
#include "madara/transport/udp/UdpTransport.h"

namespace madara
{
  namespace transport
  {
    namespace asio = boost::asio;
    namespace ip = boost::asio::ip;
    using udp = boost::asio::ip::udp;

    /**
     * @class MulticastTransport
     * @brief Multicast-based transport for knowledge. This transport currently
     * supports the following transport settings:<br />
     *        1) a single host:port pairing<br />
     *        2) the reduced message header<br />
     *        3) the normal message header<br />
     *        4) domain differentiation<br />
     *        5) on data received logic<br />
     *        6) multi-assignment of records<br />
     *        7) rebroadcasting<br />
     **/
    class MADARA_Export MulticastTransport : public UdpTransport
    {
    public:
      /**
       * Constructor
       * @param   id   unique identifer - usually a combination of host:port
       * @param   context  knowledge context
       * @param   config   transport configuration settings
       * @param   launch_transport  whether or not to launch this transport
       **/
      MulticastTransport (const std::string & id,
        madara::knowledge::ThreadSafeContext & context,
        TransportSettings & config, bool launch_transport);

    protected:
      int setup_read_socket () override;
      int setup_write_socket () override;
      int setup_read_thread (double hertz, const std::string &name) override;

      bool pre_send_buffer (size_t addr_index) override { return addr_index == 0; }
    };
  }
}

#endif // _MADARA_MULTICAST_TRANSPORT_H_
