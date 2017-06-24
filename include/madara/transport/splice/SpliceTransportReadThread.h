#ifndef SPLICE_TRANSPORT_READ_THREAD_H
#define SPLICE_TRANSPORT_READ_THREAD_H

/**
 * @file SpliceTransportReadThread.h
 * @author James Edmondson <james.r.edmondson@vanderbilt.edu>
 *
 * This file contains the implementation of the container for reading
 * DDS updates via Splice waitsets
 */

#include <string>

#include "madara/knowledge/ThreadSafeContext.h"
#include "madara/transport/Transport.h"
#include "madara/transport/QoSTransportSettings.h"
#include "madara/expression/ExpressionTree.h"
#include "madara/transport/Transport.h"
#include "madara/transport/MessageHeader.h"
#include "madara/threads/BaseThread.h"

#include "ccpp_dds_dcps.h"
#include "madara/transport/splice/ccpp_SpliceKnowledgeUpdate.h"

#include "ace/Task.h"
#include "ace/Mutex.h"
#include "ace/Barrier.h"
#include "ace/Atomic_Op_T.h"
#include "ace/Thread_Mutex.h"

#include "ace/Synch.h"

namespace madara
{
  namespace transport
  {
    /**
     * @class SpliceReadThread
     * @brief Thread for reading knowledge updates via waitsets
     **/
    class SpliceReadThread : public threads::BaseThread
    {
    public:
      /**
       * Constructor
       * @param    id unique identifier of this participant (e.g., host:port)
       * @param    context where to store and access knowledge
       * @param    update_reader reader created in @see SpliceDDSTransport
       * @param    update_writer writer created in @see SpliceDDSTransport
       * @param    settings   Transport settings
       * @param    send_monitor    bandwidth monitor for enforcing send limits
       * @param    receive_monitor    bandwidth monitor for enforcing
       *                              receive limits
       * @param    packet_scheduler scheduler for mimicking network conditions
       **/
      SpliceReadThread (const std::string & id,
        const TransportSettings & settings,
        knowledge::ThreadSafeContext & context,
        Knowledge::UpdateDataReader_ptr & update_reader,
        Knowledge::UpdateDataWriter_ptr & update_writer,
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
        const knowledge::KnowledgeMap & records);

      /**
       * We currently allow multiassignments completely through the key. This
       * is probably the least efficient way to handle this and should be
       * changed.
       **/
      ::std::vector< ::std::string>                        assignment_symbols_;

      /**
       * Unique identifier for this entity (e.g., host:port)
       **/
      const std::string                                    id_;
      
      /**
       * Transport settings
       **/
      const QoSTransportSettings    settings_;

      /**
       * The knowledge context that we will be updating
       **/
      knowledge::ThreadSafeContext * context_;

      /**
       * The DDS data reader that we will take from
       **/
      ::Knowledge::UpdateDataReader_var                    update_reader_;

      /**
       * The DDS data writer that we can write to
       **/
      ::Knowledge::UpdateDataWriter_var                    update_writer_;

      /// data received rules, defined in Transport settings
      knowledge::CompiledExpression  on_data_received_;
      
      /// buffer for receiving
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

#endif
