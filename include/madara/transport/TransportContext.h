#ifndef   _MADARA_TRANSPORT_CONTEXT_H_
#define   _MADARA_TRANSPORT_CONTEXT_H_

#include "madara/MADARA_export.h"
#include "madara/utility/stdint.h"
#include <time.h>
#include <string>
#include "madara/knowledge/KnowledgeRecord.h"

namespace madara
{
  namespace transport
  {
    /**
     * @class TransportContext
     * @brief Provides context about the transport
     **/
    
    class MADARA_Export TransportContext
    {
    public:

      enum Operations {
        IDLE_OPERATION = 0,
        SENDING_OPERATION = 1,
        RECEIVING_OPERATION = 2,
        REBROADCASTING_OPERATION = 3
      };

      /**
       * Constructor
       **/
      TransportContext (int64_t operation = IDLE_OPERATION,
        uint64_t receive_bandwidth = 0, uint64_t send_bandwidth = 0,
        uint64_t message_time = 0, uint64_t current_time = utility::get_time (),
        const std::string & domain = "",
        const std::string & originator = "",
        const std::string & endpoint = "");

      /**
       * Copy constructor
       * @param  rhs   the value to be copied into this class
       **/
      TransportContext (const TransportContext & rhs);

      /**
       * Destructor
       **/
      ~TransportContext ();
      
      /**
       * Assignment operator
       * @param  rhs   the value to be copied into this class
       **/
      void operator= (const TransportContext & rhs);
      
      /**
       * Clears records added through filtering operations
       **/
       void clear_records (void);

      /**
       * Get operation that the context is performing
       * @return     operation being performed. See enums
       *             in this class.
       **/
      int64_t get_operation (void) const;

      /**
       * Sets the operation that the context is/should be performing
       * @param  operation   operation the context is undertaking
       **/
      void set_operation (int64_t operation);

      /**
       * Gets the receive bandwidth in bytes per second
       * @return     the bandwidth taken up in receive operations
       **/
      uint64_t  get_receive_bandwidth (void) const;
      
      /**
       * Gets the send/rebroadcast bandwidth in bytes per second
       * @return     the bandwidth taken up in send/rebroadcast operations
       **/
      uint64_t  get_send_bandwidth (void) const;

      
      /**
       * Sets the bandwidth used for receives in bytes per second
       * @param  bandwidth   bandwidth being used by the transport
       **/
      void set_receive_bandwidth (uint64_t bandwidth);
      
      /**
       * Sets the bandwidth used for sends/rebroadcast in bytes per second
       * @param  bandwidth   bandwidth being used by the transport
       **/
      void set_send_bandwidth (uint64_t bandwidth);

      /**
       * Gets the message timestamp. For receive and rebroadcast operations,
       * this timestamp will be the time that the message was originally
       * sent. For send operations, this timestamp is typically the same
       * as the current timestamp.
       * @return    timestamp of message
       **/
      uint64_t  get_message_time (void) const;

      /**
       * Gets the current timestamp.
       * @return   current time in nanoseconds
       **/
      uint64_t  get_current_time (void) const;

      /**
       * Sets the message time. For receive and rebroadcast operations,
       * this timestamp will be the time that the message was originally
       * sent. For send operations, this timestamp is typically the same
       * as the current timestamp.
       * @param message_time the message time in seconds
       **/
      void set_message_time (uint64_t message_time);

      /**
       * Sets the current time. This should be utility::get_time ().
       * @param current_time the current time in nanoseconds
       **/
      void set_current_time (uint64_t current_time);

      /**
       * Adds a record to the list that should be appended to send
       * or rebroadcast
       * @param   key      key of the record to add
       * @param   record   record to add to the append list
       **/
      void add_record (const std::string & key,
        const madara::knowledge::KnowledgeRecord & record);

      /**
       * Returns the additional records stored in the context
       * @return records to add to context
       **/
      const knowledge::KnowledgeMap & get_records (void) const;

      /**
       * Returns the network domain
       * @return the current knowledge domain
       **/
      const std::string & get_domain (void) const;

      /**
       * Sets the network domain
       * @param domain the knowledge domain
       **/
      void set_domain (const std::string & domain);
      
      /**
       * Returns the current message originator
       * @return the originator of current message
       **/
      const std::string & get_originator (void) const;

      /**
       * Sets the current message originator
       * @param  originator  the originator of the current message
       **/
      void set_originator (const std::string & originator);

      /**
      * Returns the current message endpoint. Endpoint is the public
      * host:port information about how to actually contact the
      * sender, whereas originator is the declared identifier of
      * the sender.
      * @return  endpoint  the public host:port information for the sender
      **/
      const std::string & get_endpoint (void) const;

      /**
      * Sets the current message endpoint. Endpoint is the public
      * host:port information about how to actually contact the
      * sender, whereas originator is the declared identifier of
      * the sender.
      * @param  endpoint  the public host:port information for the sender
      **/
      void set_endpoint (const std::string & endpoint);

      /**
       * Sets the current message originator
       * @param  source  the source record map to set
       **/
      void set_records (const knowledge::KnowledgeMap& source);

    private:

      /**
       * Operation being performed
       **/
      int64_t operation_;

      /**
       * Bandwidth being utilized for receive operations
       **/
      uint64_t receive_bandwidth_;

      /**
       * Bandwidth being utilized for send operations
       **/
      uint64_t send_bandwidth_;

      /**
       * Message timestamp
       **/
      uint64_t message_time_;

      /**
       * Current timestamp
       **/
      uint64_t current_time_;

      /**
       * Networking domain
       **/
      std::string domain_;

      /**
       * Originator of the current message
       **/
      std::string originator_;

      /**
      * Public endpoint of the current message. This may
      * be different from originator as originator is the
      * declared id of the agent sending. Endpoint is the
      * public ip:port information that the underlying
      * socket is claiming the message comes from. This is
      * an important distinction in NAT-based networking.
      **/
      std::string endpoint_;

      /**
       * Context specific records
       **/
     knowledge::KnowledgeMap records_;
    };
  }
}

#include "TransportContext.inl"

#endif // _MADARA_TRANSPORT_CONTEXT_H_
