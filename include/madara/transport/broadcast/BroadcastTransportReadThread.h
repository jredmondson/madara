#ifndef _BROADCAST_TRANSPORT_READ_THREAD_H_
#define _BROADCAST_TRANSPORT_READ_THREAD_H_

/**
 * @file BroadcastTransportReadThread.h
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains the BroadcastTransportReadThread class, which provides a
 * broadcast transport for reading knowledge updates in KaRL
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

#include "ace/Task.h"
#include "ace/Mutex.h"
#include "ace/Barrier.h"
#include "ace/Atomic_Op_T.h"
#include "ace/Thread_Mutex.h"
#include "ace/SOCK_Dgram.h"
#include "ace/SOCK_Dgram_Bcast.h"

#include "ace/Synch.h"

namespace madara
{
  namespace transport
  {
    /**
     * @class BroadcastTransportReadThread
     * @brief Thread for reading knowledge updates through a Multicast
     * datagram socket
     **/
    class BroadcastTransportReadThread : public threads::BaseThread
    {
    public:
      /**
       * Constructor
       * @param    settings   Transport settings
       * @param    id      host:port identifier of this process, to allow for 
       *                   rejection of duplicates
       * @param    address    the ACE socket address to read from 
       * @param    socket     socket for sending
       * @param    send_monitor    bandwidth monitor for enforcing send limits
       * @param    receive_monitor    bandwidth monitor for enforcing
       *                              receive limits
       * @param    packet_scheduler scheduler for mimicking network conditions
       *
       **/
      BroadcastTransportReadThread (
        const Settings & settings,
        const std::string & id,
        const ACE_INET_Addr & address,
        ACE_SOCK_Dgram_Bcast & socket,
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
        const KnowledgeMap & records);

    private:
      /// Transport settings
      const Settings & settings_;

      /// host:port identifier of this process
      const std::string                                 id_;
      
      /// knowledge context
      knowledge::ThreadSafeContext * context_;
      
      /// The broadcast address we are subscribing to
      ACE_INET_Addr                      address_;
      
      /// The broadcast socket we are reading from
      ACE_SOCK_Dgram                     read_socket_;
      
      /// underlying socket for rebroadcasting
      ACE_SOCK_Dgram_Bcast               write_socket_;
      
#ifndef _MADARA_NO_KARL_
      /// data received rules, defined in Transport settings
      madara::knowledge::CompiledExpression  on_data_received_;
#endif // _MADARA_NO_KARL_
      
      /// buffer for sending
      madara::utility::ScopedArray <char>      buffer_;

      /// pointer to qos_settings (if applicable)
      const QoSTransportSettings *      qos_settings_;
      
      /// monitor for sending bandwidth usage
      BandwidthMonitor   &   send_monitor_;
      
      /// monitor for receiving bandwidth usage
      BandwidthMonitor   &   receive_monitor_;

      /// scheduler for mimicking target network conditions
      PacketScheduler    &   packet_scheduler_;
    };
  }
}

#endif // _BROADCAST_TRANSPORT_READ_THREAD_H_
