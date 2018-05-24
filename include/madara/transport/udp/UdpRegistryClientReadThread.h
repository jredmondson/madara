#if 0 //ndef _MADARA_UDP_REGISTRY_CLIENT_READ_THREAD_H_
#define _MADARA_UDP_REGISTRY_CLIENT_READ_THREAD_H_

#include <string>

#include "madara/utility/ScopedArray.h"
#include "madara/knowledge/ThreadSafeContext.h"
#include "madara/transport/BandwidthMonitor.h"
#include "madara/transport/QoSTransportSettings.h"
#include "madara/expression/ExpressionTree.h"
#include "madara/transport/Transport.h"
#include "madara/transport/MessageHeader.h"
#include "madara/threads/BaseThread.h"

namespace madara
{
  namespace transport
  {
    /**
     * @class UdpRegistryClientReadThread
     * @brief Thread for reading knowledge and registry updates through a
     *        UDP socket
     **/
    class UdpRegistryClientReadThread : public threads::BaseThread
    {
    public:
      /**
       * Constructor
       * @param    settings   Transport settings
       * @param    id      host:port identifier of this process, to allow for 
       *                   rejection of duplicates
       * @param    addresses    the socket addresses to communicate with 
       * @param    socket       socket for sending and receiving
       * @param    send_monitor    bandwidth monitor for enforcing send limits
       * @param    receive_monitor    bandwidth monitor for enforcing
       *                              receive limits
       * @param    packet_scheduler scheduler for mimicking network conditions
       **/
      UdpRegistryClientReadThread (
        const TransportSettings & settings,
        const std::string & id,
        std::map <std::string, ACE_INET_Addr> & addresses,
        ACE_SOCK_Dgram & socket,
        BandwidthMonitor & send_monitor,
        BandwidthMonitor & receive_monitor,
        PacketScheduler & packet_scheduler);
      
      /**
       * Initializes MADARA context-related items
       * @param   knowledge   context for querying current program state
       **/
      void init (knowledge::KnowledgeBase & knowledge);

      /**
       * Cleanup function called by thread manager
       **/
      void cleanup (void);

      /**
       * The main loop internals for the read thread
       **/
      void run (void);

      /**
       * Sends a rebroadcast packet to all peers.
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

    private:
      /// Transport settings
      const QoSTransportSettings settings_;

      /// host:port identifier of this process
      const std::string                                 id_;
      
      /// knowledge context
      knowledge::ThreadSafeContext * context_;
      
      /// internet addresses of our peers
      std::map <std::string, ACE_INET_Addr> addresses_;
      
      /// The socket we are writing to and reading from
      ACE_SOCK_Dgram       &             socket_;
      
#ifndef _MADARA_NO_KARL_
      /// data received rules, defined in Transport settings
      madara::knowledge::CompiledExpression  on_data_received_;
#endif // _MADARA_NO_KARL_
      
      /// buffer for sending
      madara::utility::ScopedArray <char>      buffer_;

      /// monitor for sending bandwidth usage
      BandwidthMonitor   &   send_monitor_;
      
      /// monitor for receiving bandwidth usage
      BandwidthMonitor   &   receive_monitor_;

      /// scheduler for mimicking target network conditions
      PacketScheduler    &   packet_scheduler_;
    };
  }
}

#endif // _MADARA_UDP_REGISTRY_CLIENT_READ_THREAD_H_
