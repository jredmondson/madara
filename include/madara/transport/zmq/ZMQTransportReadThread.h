#ifndef _MADARA_ZEROMQ_TRANSPORT_READ_THREAD_H_
#define _MADARA_ZEROMQ_TRANSPORT_READ_THREAD_H_

/**
 * @file ZMQTransportReadThread.h
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains the ZMQTransportReadThread class, which provides a
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
#include "madara/threads/BaseThread.h"

#include <zmq.h>

namespace madara
{
  namespace transport
  {
    /**
     * @class ZMQTransportReadThread
     * @brief Thread for reading knowledge updates through a ZMQ
     *        datagram socket
     **/
    class ZMQTransportReadThread : public threads::BaseThread
    {
    public:
      /**
       * Constructor
       * @param    settings   Transport settings
       * @param    id      host:port identifier of this process, to allow for 
       *                   rejection of duplicates
       * @param    write_socket    socket for sending
       * @param    send_monitor    bandwidth monitor for enforcing send limits
       * @param    receive_monitor    bandwidth monitor for enforcing
       *                              receive limits
       * @param    packet_scheduler scheduler for mimicking network conditions
       **/
      ZMQTransportReadThread (
        const TransportSettings & settings,
        const std::string & id,
        void * write_socket,
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
        const knowledge::KnowledgeMap& records);

    private:
      /// quality-of-service transport settings
      const QoSTransportSettings  settings_;

      /// host:port identifier of this process
      const std::string          id_;

      /// knowledge context
      knowledge::ThreadSafeContext * context_;

      /// underlying socket for sending
      void * write_socket_;

      /// The multicast socket we are reading from
      void * read_socket_;
      
#ifndef _MADARA_NO_KARL_
      /// data received rules, defined in Transport settings
      madara::knowledge::CompiledExpression  on_data_received_;
#endif // _MADARA_NO_KARL_

      /// buffer for receiving
      madara::utility::ScopedArray <char>      buffer_;
      
      /// monitor for sending bandwidth usage
      BandwidthMonitor   &   send_monitor_;
      
      /// monitor for receiving bandwidth usage
      BandwidthMonitor   &   receive_monitor_;

      /// scheduler for mimicking target network conditions
      PacketScheduler    &   packet_scheduler_;
      
      /// received packets
      knowledge::containers::Integer received_packets_;

      /// bad receives
      knowledge::containers::Integer failed_receives_;

      /// received data
      knowledge::containers::Integer received_data_;

      /// max data received
      knowledge::containers::Integer received_data_max_;

      /// min data received
      knowledge::containers::Integer received_data_min_;
    };
  }
}

#endif // _MADARA_ZEROMQ_TRANSPORT_READ_THREAD_H_
