#ifndef _MADARA_TRANSPORT_H_
#define _MADARA_TRANSPORT_H_

/**
 * @file Transport.h
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains the transport::Base class, which provides an
 * extensible transport layer for sending knowledge updates in KaRL.
 * To support knowledge updates, only the send_multiassignment method
 * is currently required to be extended as the set, evaluate, and wait
 * methods all call send_multiassignment. For example transport, @see
 * madara::transport::MulticastTransport.
 **/

#include <string>
#include <sstream>
#include <vector>
#include <ostream>
#include "ace/Thread_Mutex.h"
#include "ace/Recursive_Thread_Mutex.h"
#include "ace/Condition_T.h"
#include "ace/Guard_T.h"

#include "madara/utility/ThreadSafeVector.h"
#include "madara/MADARA_export.h"
#include "ace/High_Res_Timer.h"
#include "madara/transport/QoSTransportSettings.h"

#include "ReducedMessageHeader.h"
#include "madara/transport/BandwidthMonitor.h"
#include "madara/transport/PacketScheduler.h"

#ifdef _USE_CID_

#include "madara/cid/CIDSettings.h"
#include "madara/cid/CIDConvenience.h"
#include "madara/cid/CIDHeuristic.h"
#include "madara/cid/CIDGenetic.h"
#include "madara/utility/Utility.h"

#endif // _USE_CID_

#include "madara/knowledge/KnowledgeRecord.h"
#include "madara/knowledge/ThreadSafeContext.h"
#include "madara/expression/ExpressionTree.h"
#include "madara/expression/Interpreter.h"

namespace madara
{
  namespace transport
  {
    typedef    ACE_Condition <ACE_Thread_Mutex>    Condition;

    /**
     * Base class from which all transports must be derived.
     * To support knowledge updates, only the send_multiassignment method
     * is currently required to be extended as the set, evaluate, and wait
     * methods all call send_multiassignment. For example transport, @see
     * madara::transport::MulticastTransport.
     **/
    class MADARA_Export Base
    {
    public:
      /**
       * Used to define a vector of hosts to contact
       **/
      typedef    std::vector <std::string>           HostsVector;

      /**
       * Constructor
       * @param   id                unique identifier (generally host:port)
       * @param   new_settings      settings to apply to the transport
       * @param   context           the knowledge record context
       **/
      Base (const std::string & id, Settings & new_settings,
        knowledge::ThreadSafeContext & context);

      /**
       * Destructor
       **/
      virtual ~Base ();

      /**
       * Validates a transport to indicate it is not shutting down
       **/
      int validate_transport (void);

      /**
       * all subclasses should call this method at the end of its setup
       **/
      virtual int setup (void);

      /**
       * all subclasses should call this method at the beginning of send_data
       **/
      int check_transport (void);

      /**
       * Getter for the transport settings
       * @return   the current transport settings
       **/
      Settings & settings (void);

#ifdef _USE_CID_

      /**
       * Start latency
       * @return  result of operation or -1 if we are shutting down
       **/
      virtual long start_latency (void)
      {
        if (!settings_.latency_enabled)
        {
          madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_ALWAYS, 
              DLINFO "SpliceDDSTransport::start_latency:" \
              " Latency enabled is not set in your"
              " madara::transport::Settings instance. Update your"
              " program's code and set settings.latency_enabled = true.\n"));

          return -2;
        }
        return check_transport ();
      }

      /**
       * Vote for an algorithm result
       * @return  result of operation or -1 if we are shutting down
       **/
      virtual long vote (void)
      {
        if (!settings_.latency_enabled)
        {
          madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_ALWAYS, 
              DLINFO "SpliceDDSTransport::vote:" \
              " Latency enabled is not set in your"
              " madara::transport::Settings instance. Update your"
              " program's code and set settings.latency_enabled = true.\n"));

          return -2;
        }
        return check_transport ();
      }

#endif //_USE_CID_
      
    /**
     * Preps a message for sending
     * @param  orig_updates     updates before send filtering is applied
     * @param  print_prefix     prefix to include before every log message,
     *                          e.g., "MyTransport::svc"
     * @return       -1   Transport is shutting down<br />
     *               -2   Transport is invalid<br />
     *               -3   Unable to allocate send buffer<br />
     *                0   No message to send
     *               > 0  size of buffered message
     **/
      long prep_send (const madara::knowledge::KnowledgeRecords & orig_updates,
        const char * print_prefix);

      /**
       * Sends a list of updates to the domain. This function must be
       * implemented by your transport
       * @return  result of operation or -1 if we are shutting down
       **/
      virtual long send_data (const madara::knowledge::KnowledgeRecords &) = 0;

      /**
       * Invalidates a transport to indicate it is shutting down
       **/
      void invalidate_transport (void);

      /**
       * Closes this transport
       **/
      virtual void close (void);
      
    protected:
      volatile bool is_valid_;
      volatile bool shutting_down_;
      HostsVector hosts;
      ACE_Thread_Mutex mutex_;
      Condition valid_setup_;

      /// host:port identifier of this process
      const std::string                               id_;

      QoSTransportSettings settings_;

      // context for knowledge base
      madara::knowledge::ThreadSafeContext & context_;
      
#ifndef _MADARA_NO_KARL_
      /// data received rules, defined in Transport settings
      madara::expression::ExpressionTree  on_data_received_;
      
#endif // _MADARA_NO_KARL_

      /// monitor for sending bandwidth usage
      BandwidthMonitor       send_monitor_;
      
      /// monitor for receiving bandwidth usage
      BandwidthMonitor       receive_monitor_;

      /// scheduler for dropping packets to simulate network issues
      PacketScheduler        packet_scheduler_;

      /// buffer for sending
      madara::utility::ScopedArray <char>      buffer_;
    };

    /**
     * Processes a received update, updates monitors, fills
     * rebroadcast records according to settings filters, and
     * performs the bulk of the logic required for a receiver thread.
     *
     * @param  buffer           buffer containing all serialized updates
     * @param   bytes_read       bytes in the buffer
     * @param  id               unique identifier for originator strings
     * @param  context          variable context of the knowledge base
     * @param  settings         transport settings
     * @param  send_monitor     monitor of send traffic
     * @param  receive_monitor  monitor of receive traffice
     * @param  rebroadcast_records  map of variables to records to be
     *                              rebroadcasted (will be filled in by this
     *                              method)
     * @param  remote_host      ip:port who actually sent this message
     * @param  on_data_received compiled expression tree of the
     *                          settings.on_data_received_logic (you have to
     *                          provide the compiled tree)
     * @param  print_prefix     prefix to include before every log message,
     *                          e.g., "MyTransport::svc"
     * @param  header           will contain the message header object from the
     *                          message received (you have to clean this up
     *                          delete--e.g., "delete header").
     * @return       -1   Rejected: Non-MADARA Message<br />
     *               -2   Rejected: Message from Self<br />
     *               -3   Rejected: Untrusted Peer<br />
     *               -4   Rejected: Untrusted Originator<br />
     *               -5   Rejected: Wrong domain<br />
     *               >=   Number of accepted updates
     **/
    int MADARA_Export process_received_update (
        const char * buffer,
        uint32_t bytes_read,
        const std::string & id,
        knowledge::ThreadSafeContext & context,
        const QoSTransportSettings & settings,
        BandwidthMonitor & send_monitor,
        BandwidthMonitor & receive_monitor,
       knowledge::KnowledgeMap& rebroadcast_records,
#ifndef _MADARA_NO_KARL_

        knowledge::CompiledExpression & on_data_received,
#endif // _MADARA_NO_KARL_

        const char * print_prefix,
        const char * remote_host,
        MessageHeader *& header
        );

    /**
     * Preps a buffer for rebroadcasting records to other agents
     * on the network.
     * @param  context  the key/value storage for variables
     * @param  buffer   the buffer to fill with header and records
     * @param  buffer_remaining  will contain the buffer remaining,
     *                           in case your transport must send other info
     * @param  settings         transport settings
     * @param  print_prefix     prefix to include before every log message,
     *                          e.g., "MyTransport::svc"
     * @param  header           message header from the received message
     * @param  records          a map of variables to records to send
     * @param  packet_scheduler scheduler for mimicking network conditions
     **/
    int MADARA_Export prep_rebroadcast (
      knowledge::ThreadSafeContext & context,
      char * buffer,
      int64_t & buffer_remaining,
      const QoSTransportSettings & settings,
      const char * print_prefix,
      MessageHeader * header,
      const knowledge::KnowledgeMap& records,
      PacketScheduler & packet_scheduler);

    typedef   utility::ThreadSafeVector <Base *>   Transports;
  }
}

#include "Transport.inl"

#endif  // _MADARA_TRANSPORT_H_
