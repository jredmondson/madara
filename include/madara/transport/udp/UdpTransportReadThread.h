#ifndef _MADARA_UDP_TRANSPORT_READ_THREAD_H_
#define _MADARA_UDP_TRANSPORT_READ_THREAD_H_

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
#include "madara/boost.h"

namespace madara
{
  namespace transport
  {
    namespace asio = boost::asio;
    namespace ip = boost::asio::ip;
    using udp = boost::asio::ip::udp;

    /**
     * @class UdpTransportReadThread
     * @brief Thread for reading knowledge updates through a UDP socket
     **/
    class UdpTransportReadThread : public threads::BaseThread
    {
    public:
      UdpTransportReadThread (UdpTransport &transport);

      /**
       * Initializes MADARA context-related items
       * @param   knowledge   context for querying current program state
       **/
      void init (knowledge::KnowledgeBase & knowledge) override;

      /**
       * Cleanup function called by thread manager
       **/
      void cleanup (void) override;

      /**
       * The main loop internals for the read thread
       **/
      void run (void) override;

      /**
       * Sends a rebroadcast packet.
       * @param  print_prefix     prefix to include before every log message,
       *                          e.g., "MyTransport::svc"
       * @param   header   header for the rebroadcasted packet
       * @param   records  records to rebroadcast (already filtered for
       *                   rebroadcast)
       **/
      void rebroadcast (
        const char * print_prefix,
        MessageHeader * header,
        const knowledge::KnowledgeMap & records);

    protected:
      UdpTransport &transport_;

      knowledge::ThreadSafeContext * context_ = nullptr;

#ifndef _MADARA_NO_KARL_
      /// data received rules, defined in Transport settings
      madara::knowledge::CompiledExpression on_data_received_;
#endif // _MADARA_NO_KARL_

      /// buffer for receiving
      madara::utility::ScopedArray<char> buffer_;
    };
  }
}

#endif // _MADARA_UDP_TRANSPORT_READ_THREAD_H_
