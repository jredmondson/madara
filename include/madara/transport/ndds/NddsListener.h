#ifndef _MADARA_NDDS_LISTENER_H_
#define _MADARA_NDDS_LISTENER_H_


#include <string>

#include "madara/knowledge/ThreadSafeContext.h"
#include "madara/transport/Transport.h"

#include <ndds/ndds_cpp.h>

#include "madara/transport/ndds/Ndds_Knowledge_Update.h"
#include "madara/transport/ndds/Ndds_Knowledge_UpdateSupport.h"

namespace madara
{
  namespace transport
  {
    /**
     * @class NDDSListener
     * @brief Container for NDDS callbacks
     **/
    class NDDSListener: public DDSDataReaderListener
    {
    public:
      /**
       * Constructor
       * @param    settings   Transport settings
       * @param id  unique identifier of this entity (e.g., host:port)
       * @param context the knowledge context to access and mutate
       * @param    send_monitor    bandwidth monitor for enforcing send limits
       * @param    receive_monitor    bandwidth monitor for enforcing
       *                              receive limits
       * @param    packet_scheduler scheduler for mimicking network conditions
       **/
      NDDSListener(
        const TransportSettings & settings, const std::string & id,
        knowledge::ThreadSafeContext & context,
        BandwidthMonitor & send_monitor,
        BandwidthMonitor & receive_monitor,
        PacketScheduler & packet_scheduler);

      /**
       * Copy constructor
       **/
      NDDSListener(const NDDSListener &ref);

      /**
       * Destructor
       **/
      ~NDDSListener();

      /**
       * Handles the case where a subscription has been matched
       * @param reader the DDS data reader to read from
       * @param status the status of the matched subscription
       **/
      void on_subscription_matched (DDSDataReader * reader,
                        const DDS_SubscriptionMatchedStatus & status);
      
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
       * Handles the case that data has become available
       * @param reader the DDS data reader to read from
       **/
      void on_data_available(DDSDataReader* reader);
    private:
      
      /// Transport settings
      const QoSTransportSettings settings_;

      // NDDS variables
      const std::string id_;

      // context for updating variables from network
      knowledge::ThreadSafeContext * context_;
      
      /// data received rules, defined in Transport settings
      knowledge::CompiledExpression  on_data_received_;
      
      /// buffer for receiving
      utility::ScopedArray <char>      buffer_;

      /// monitor for sending bandwidth usage
      BandwidthMonitor   &   send_monitor_;
      
      /// monitor for receiving bandwidth usage
      BandwidthMonitor   &   receive_monitor_;

      /// scheduler for mimicking target network conditions
      PacketScheduler    &   packet_scheduler_;
    };  // End of class NDDSListener
  }
}

#endif // _MADARA_NDDS_LISTENER_H_
