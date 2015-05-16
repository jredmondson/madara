#ifndef   _TRANSPORT_CONTEXT_H_
#define   _TRANSPORT_CONTEXT_H_

#include "madara/MADARA_export.h"
#include "madara/utility/stdint.h"
#include <time.h>
#include <string>
#include "madara/knowledge_engine/Knowledge_Record.h"

namespace Madara
{
  namespace Transport
  {
    /**
     * @class Transport_Context
     * @brief Provides context about the transport
     **/
    
    class MADARA_Export Transport_Context
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
      Transport_Context (int64_t operation = IDLE_OPERATION,
        uint64_t receive_bandwidth = 0, uint64_t send_bandwidth = 0,
        uint64_t message_time = 0, uint64_t current_time = time (NULL),
        const std::string & domain = "",
        const std::string & originator = "");

      /**
       * Copy constructor
       * @param  rhs   the value to be copied into this class
       **/
      Transport_Context (const Transport_Context & rhs);

      /**
       * Destructor
       **/
      ~Transport_Context ();
      
      /**
       * Assignment operator
       * @param  rhs   the value to be copied into this class
       **/
      void operator= (const Transport_Context & rhs);
      
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
       * @return   current time in seconds
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
       * Sets the current time. This should be time (NULL).
       * @param current_time the current time in seconds
       **/
      void set_current_time (uint64_t current_time);

      /**
       * Adds a record to the list that should be appended to send
       * or rebroadcast
       * @param   key      key of the record to add
       * @param   record   record to add to the append list
       **/
      void add_record (const std::string & key,
        const Madara::Knowledge_Record & record);

      /**
       * Returns the additional records stored in the context
       * @return records to add to context
       **/
      const Knowledge_Map & get_records (void) const;

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
       * Sets the current message originator
       * @param  source  the source record map to set
       **/
      void set_records (const Knowledge_Map & source);

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
       * Context specific records
       **/
      Knowledge_Map records_;
    };
  }
}

#include "Transport_Context.inl"

#endif