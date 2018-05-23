#ifndef _MADARA_BROADCAST_TRANSPORT_H_
#define _MADARA_BROADCAST_TRANSPORT_H_

/**
 * @file BroadcastTransport.h
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains the BroadcastTransport class, which provides a
 * broadcast transport for sending knowledge updates in KaRL
 **/

#include <string>

#include "madara/MADARA_export.h"
#include "madara/utility/ScopedArray.h"
#include "madara/transport/udp/UdpTransport.h"
#include "madara/knowledge/ThreadSafeContext.h"
#include "madara/transport/BandwidthMonitor.h"
#include "ace/SOCK_Dgram_Bcast.h"
#include "madara/utility/stdint.h"
#include "madara/expression/ExpressionTree.h"
#include "madara/threads/Threader.h"


namespace madara
{
  namespace transport
  {
    /**
     * @class BroadcastTransport
     * @brief Broadcast-based transport for knowledge. This transport currently
     * supports the following transport settings:<br />
     *        1) a single host:port pairing<br />
     *        2) the reduced message header<br />
     *        3) the normal message header<br />
     *        4) domain differentiation<br />
     *        5) on data received logic<br />
     *        6) multi-assignment of records<br />
     *        7) rebroadcasting<br />
     **/
    class MADARA_Export BroadcastTransport : public UdpTransport
    {
    public:
      using UdpTransport::UdpTransport;

    protected:
      bool pre_send_buffer (size_t addr_index) override { return addr_index == 0; }
    };
  }
}

#endif // _MADARA_BROADCAST_TRANSPORT_H_
