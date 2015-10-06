#ifndef _MADARA_NDDS_LISTENER_H_
#define _MADARA_NDDS_LISTENER_H_


#include <string>

#include "madara/knowledge/Thread_Safe_Context.h"
#include "madara/transport/Transport.h"

#include <ndds/ndds_cpp.h>

#include "madara/transport/ndds/Ndds_Knowledge_Update.h"
#include "madara/transport/ndds/Ndds_Knowledge_UpdateSupport.h"

namespace madara
{
  namespace transport
  {
    /**
     * @class NDDS_Listener
     * @brief Container for NDDS callbacks
     **/
    class NDDS_Listener: public DDSDataReaderListener
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
      NDDS_Listener(
        const Settings & settings, const std::string & id,
        knowledge::Thread_Safe_Context & context,
        Bandwidth_Monitor & send_monitor,
        Bandwidth_Monitor & receive_monitor,
        Packet_Scheduler & packet_scheduler);

      /**
       * Copy constructor
       **/
      NDDS_Listener(const NDDS_Listener &ref);

      /**
       * Destructor
       **/
      ~NDDS_Listener();

      /**
       * Handles the case where a subscription has been matched
       * @param reader the DDS data reader to read from
       * @param status the status of the matched subscription
       **/
      void on_subscription_matched (DDSDataReader* reader,
                        const DDS_SubscriptionMatchedStatus& status);
      
      /**
       * Sends a rebroadcast packet.
       * @param  print_prefix     prefix to include before every log message,
       *                          e.g., "My_Transport::svc"
       * @param   header   header for the rebroadcasted packet
       * @param   records  records to rebroadcast (already filtered for
       *                   rebroadcast)
       **/
      void rebroadcast (
        const char * print_prefix,
        Message_Header * header,
        const Knowledge_Map & records);

      /**
       * Handles the case that data has become available
       * @param reader the DDS data reader to read from
       **/
      void on_data_available(DDSDataReader* reader);
    private:
      
      /// Transport settings
      const Settings & settings_;

      // NDDS variables
      const std::string id_;
      knowledge::Thread_Safe_Context & context_;
      
      /// data received rules, defined in Transport settings
      knowledge::Compiled_Expression  on_data_received_;
      
      /// buffer for receiving
      utility::Scoped_Array <char>      buffer_;

      /// pointer to qos_settings (if applicable)
      const QoS_Transport_Settings *      qos_settings_;
      
      /// monitor for sending bandwidth usage
      Bandwidth_Monitor   &   send_monitor_;
      
      /// monitor for receiving bandwidth usage
      Bandwidth_Monitor   &   receive_monitor_;

      /// scheduler for mimicking target network conditions
      Packet_Scheduler    &   packet_scheduler_;
    };  // End of class NDDS_Listener
  }
}

#endif // _MADARA_NDDS_LISTENER_H_
