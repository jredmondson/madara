#ifndef _MADARA_QOS_TRANSPORT_SETTINGS_H_
#define _MADARA_QOS_TRANSPORT_SETTINGS_H_

/**
 * @file QoSTransportSettings.h
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains the QoSTransportSettings class, which provides a
 * settings container for quality of service parameters for transport classes
 **/

#include <string>

#include "madara/transport/TransportSettings.h"
#include "madara/transport/TransportContext.h"
#include "madara/knowledge/ThreadSafeContext.h"
#include "madara/utility/StdInt.h"
#include "madara/MadaraExport.h"
#include "madara/filters/AggregateFilter.h"
#include "madara/filters/RecordFilter.h"
#include "madara/filters/BufferFilter.h"
#include "madara/knowledge/KnowledgeRecordFilters.h"

#ifdef _MADARA_JAVA_
  #include <jni.h>
#endif

#ifdef _MADARA_PYTHON_CALLBACKS_
  #include <boost/python.hpp>
#endif


namespace madara
{
  namespace transport
  {
    /**
     * Enumeration for packet drop policy types
     **/
    enum PacketDropType {
      PACKET_DROP_DETERMINISTIC,
      PACKET_DROP_PROBABLISTIC
    };

    /**
     * @class QoSTransportSettings
     * @brief Container for quality-of-service settings
     **/
    class MADARA_EXPORT QoSTransportSettings : public TransportSettings
    {
    public:
      /**
       * Default constructor
       **/
      QoSTransportSettings ();

      /**
       * Copy constructor
       **/
      QoSTransportSettings (const QoSTransportSettings & settings);
      
      /**
       * Copy constructor
       **/
      QoSTransportSettings (const TransportSettings & settings);

      /**
       * Destructor
       **/
      virtual ~QoSTransportSettings ();
       
      /**
       * Assignment operator
       **/
      void operator= (const QoSTransportSettings & settings);
      
      /**
       * Assignment operator
       **/
      void operator= (const TransportSettings & settings);

      /**
       * Adds a filter that will be applied to certain types before sending
       * @param   types      the types to add the filter to
       * @param   function   the function that will take the knowledge record
       *                     in FunctionArguments.
       **/
      void add_send_filter (uint32_t types,
        knowledge::KnowledgeRecord (*function) (knowledge::FunctionArguments &,
                                      knowledge::Variables &));
      
      /**
       * Adds a filter that will be applied to certain types before sending
       * @param   types      the types to add the filter to
       * @param   filter     an instance of an individual record filter that
       *                     will be managed by the underlying infrastructure
       **/
      void add_send_filter (uint32_t types,
        filters::RecordFilter * filter);
      
      /**
       * Adds an aggregate update filter that will be applied before sending,
       * after individual record filters.
       * @param   function  an aggregate update filter
       **/
      void add_send_filter (void (*function) (
        knowledge::KnowledgeMap &, const TransportContext &,
        knowledge::Variables &));
      
      /**
       * Adds an aggregate update filter that will be applied before sending,
       * after individual record filters.
       * @param   filter     an instance of an aggregate record filter that
       *                     will be managed by the underlying infrastructure
       **/
      void add_send_filter (filters::AggregateFilter * filter);

      /**
      * Adds a buffer filter to the chain
      * @param   filter     an instance of a buffer filter
      **/
      void add_filter (filters::BufferFilter * filter);

      /**
       * Adds a filter that will be applied to certain types after receiving
       * and before applying to the local knowledge base
       * @param   types      the types to add the filter to
       * @param   function   the function that will take the knowledge record
       *                     in FunctionArguments.
       **/
      void add_receive_filter (uint32_t types,
        knowledge::KnowledgeRecord (*function) (knowledge::FunctionArguments &,
                                      knowledge::Variables &));
      
      /**
       * Adds a filter that will be applied to certain types after receiving
       * @param   types      the types to add the filter to
       * @param   filter     an instance of an individual record filter that
       *                     will be managed by the underlying infrastructure
       **/
      void add_receive_filter (uint32_t types,
        filters::RecordFilter * filter);
      
      /**
       * Adds an aggregate update filter that will be applied after receiving,
       * after individual record filters.
       * @param   function  an aggregate update filter
       **/
      void add_receive_filter (void (*function) (
        knowledge::KnowledgeMap &, const TransportContext &,
        knowledge::Variables &));
 
      /**
       * Adds an aggregate update filter that will be applied after receiving,
       * after individual record filters.
       * @param   filter     an instance of an aggregate record filter that
       *                     will be managed by the underlying infrastructure
       **/
      void add_receive_filter (filters::AggregateFilter * filter);
      
      /**
       * Adds a filter that will be applied to certain types after receiving
       * and before rebroadcasting (if TTL > 0)
       * @param   types      the types to add the filter to
       * @param   function   the function that will take the knowledge record
       *                     in FunctionArguments.
       **/
      void add_rebroadcast_filter (uint32_t types,
        knowledge::KnowledgeRecord (*function) (knowledge::FunctionArguments &,
                                      knowledge::Variables &));
      
      /**
       * Adds a filter that will be applied to certain types after receiving
       * and before rebroadcasting (if TTL > 0)
       * @param   types      the types to add the filter to
       * @param   filter     an instance of an individual record filter that
       *                     will be managed by the underlying infrastructure
       **/
      void add_rebroadcast_filter (uint32_t types,
        filters::RecordFilter * filter);
      
      /**
       * Adds an aggregate update filter that will be applied before
       * rebroadcasting, after individual record filters.
       * @param   function  an aggregate update filter
       **/
      void add_rebroadcast_filter (void (*function) (
        knowledge::KnowledgeMap &, const TransportContext &,
        knowledge::Variables &));
 
      /**
       * Adds an aggregate update filter that will be applied before
       * rebroadcasting, after individual record filters.
       * @param   filter     an instance of an aggregate record filter that
       *                     will be managed by the underlying infrastructure
       **/
      void add_rebroadcast_filter (filters::AggregateFilter * filter);
      
#ifdef _MADARA_JAVA_
      
      /**
       * Adds a filter that will be applied to certain types after receiving
       * and before applying updates to the KnowledgeBase
       * @param   types      the types to add the filter to
       * @param   object     a java callback that takes in a list of args
       *                     and the Variables interface.
       **/
      void add_receive_filter (
                               uint32_t types, jobject & object);
      
      /**
       * Adds a filter that will be applied to certain types before sending
       * @param   types      the types to add the filter to
       * @param   object     a java callback that takes in a list of args
       *                     and the Variables interface.
       **/
      void add_send_filter (
                            uint32_t types, jobject & object);
      
      /**
       * Adds a filter that will be applied to certain types after receiving
       * and before rebroadcasting (if TTL > 0)
       * @param   types      the types to add the filter to
       * @param   object     a java callback that takes in a list of args
       *                     and the Variables interface.
       **/
      void add_rebroadcast_filter (
                                   uint32_t types, jobject & object);
      
      /**
       * Adds an aggregate filter for a map of variables to values
       * before applying updates to the KnowledgeBase
       * @param   object     a java callback that takes in a list of args
       *                     and the Variables interface.
       **/
      void add_receive_filter (jobject & object);
      
      /**
       * Adds an aggregate filter for a map of variables to values
       * before sending
       * @param   object     a java callback that takes in a list of args
       *                     and the Variables interface.
       **/
      void add_send_filter (jobject & object);
      
      /**
       * Adds an aggregate filter for a map of variables to values
       * after receiving and before rebroadcasting (if TTL > 0)
       * @param   object     a java callback that takes in a list of args
       *                     and the Variables interface.
       **/
      void add_rebroadcast_filter (jobject & object);
      
#endif
      
#ifdef _MADARA_PYTHON_CALLBACKS_
      
      /**
       * Adds a filter that will be applied to certain types after receiving
       * and before applying updates to the KnowledgeBase
       * @param   types      the types to add the filter to
       * @param   object     a python callback that takes in a list of args
       *                     and the Variables interface.
       **/
      void add_receive_filter (
        uint32_t types, boost::python::object & object);
      
      /**
       * Adds a filter that will be applied to certain types before sending
       * @param   types      the types to add the filter to
       * @param   object     a python callback that takes in a list of args
       *                     and the Variables interface.
       **/
      void add_send_filter (
        uint32_t types, boost::python::object & object);
      
      /**
       * Adds a filter that will be applied to certain types after receiving
       * and before rebroadcasting (if TTL > 0)
       * @param   types      the types to add the filter to
       * @param   object     a python callback that takes in a list of args
       *                     and the Variables interface.
       **/
      void add_rebroadcast_filter (
        uint32_t types, boost::python::object & object);
      
      /**
       * Adds an aggregate filter for a map of variables to values
       * before applying updates to the KnowledgeBase
       * @param   object     a python callback that takes in a list of args
       *                     and the Variables interface.
       **/
      void add_receive_filter (boost::python::object & object);
      
      /**
       * Adds an aggregate filter for a map of variables to values
       * before sending
       * @param   object     a python callback that takes in a list of args
       *                     and the Variables interface.
       **/
      void add_send_filter (boost::python::object & object);
      
      /**
       * Adds an aggregate filter for a map of variables to values 
       * after receiving and before rebroadcasting (if TTL > 0)
       * @param   object     a python callback that takes in a list of args
       *                     and the Variables interface.
       **/
      void add_rebroadcast_filter (boost::python::object & object);
      
#endif

      /**
       * Attaches a context to the various filtering systems. If the context
       * ever goes out of scope, a 0 should be passed into this function to
       * the context.
       * @param   context     context to be used for Variable lookups
       **/
      void attach (knowledge::ThreadSafeContext * context);
      
      /**
       * Clears the list of filters for the specified types
       * @param   types   the types to clear the filters of
       **/
      void clear_send_filters (uint32_t types);

      /**
       * Clears the list of send time aggregate filters
       **/
      void clear_send_aggregate_filters (void);

      /**
       * Clears the list of filters for the specified types
       * @param   types   the types to clear the filters of
       **/
      void clear_receive_filters (uint32_t types);

      /**
      * Clears the list of buffer filters
      **/
      void clear_buffer_filters (void);

      /**
       * Clears the list of receive time aggregate filters
       **/
      void clear_receive_aggregate_filters (void);

      /**
       * Clears the list of filters for the specified types
       * @param   types   the types to clear the filters of
       **/
      void clear_rebroadcast_filters (uint32_t types);
      
      /**
       * Clears the list of rebroadcast time aggregate filters
       **/
      void clear_rebroadcast_aggregate_filters (void);
      
      /**
       * Filters an input according to send's filter chain
       * @param   name    variable name of input ("" for unnamed)
       * @param   input   the argument to the filter chain
       * @param   context the context of the transport
       * @return  the result of filtering the input
       **/
      knowledge::KnowledgeRecord filter_send (
        const madara::knowledge::KnowledgeRecord & input,
        const std::string & name,
        transport::TransportContext & context) const;
        
      /**
       * Filters aggregate records according to the send filter chain
       * @param  records             the aggregate record map
       * @param  transport_context   the context of the transport
       **/
      void filter_send (knowledge::KnowledgeMap & records,
        const transport::TransportContext & transport_context) const;
          
      /**
       * Filters an input according to the receive filter chain
       * @param   name    variable name of input ("" for unnamed)
       * @param   input   the argument to the filter chain
       * @param   context the context of the transport
       * @return  the result of filtering the input
       **/
      knowledge::KnowledgeRecord filter_receive (
        const madara::knowledge::KnowledgeRecord & input,
        const std::string & name,
        transport::TransportContext & context) const;
      
      /**
       * Filters aggregate records according to the receive filter chain
       * @param  records             the aggregate record map
       * @param  transport_context   the context of the transport
       **/
      void filter_receive (knowledge::KnowledgeMap & records,
        const transport::TransportContext & transport_context) const;
          
      /**
       * Filters an input according to the rebroadcast filter chain
       * @param   name    variable name of input ("" for unnamed)
       * @param   input   the argument to the filter chain
       * @param   context the context of the transport
       * @return  the result of filtering the input
       **/
      knowledge::KnowledgeRecord filter_rebroadcast (
        const madara::knowledge::KnowledgeRecord & input,
        const std::string & name,
        transport::TransportContext & context) const;

      /**
       * Filters aggregate records according to the rebroadcast filter chain
       * @param  records             the aggregate record map
       * @param  transport_context   the context of the transport
       **/
      void filter_rebroadcast (knowledge::KnowledgeMap & records,
        const transport::TransportContext & transport_context) const;

      /**
      * Calls encode on the the buffer filter chain
      * @param   source           the source and destination buffer
      * @param   size             the amount of data in the buffer in bytes
      * @param   max_size         the amount of bytes the buffer can hold
      * @return  the new size after encoding
      * @throw exceptions::MemoryException  if max_size is not big enough
      **/
      int filter_encode (char * source, int size, int max_size) const;

      /**
      * Calls decode on the the buffer filter chain
      * @param   source           the source and destination buffer
      * @param   size             the amount of data in the buffer in bytes
      * @param   max_size         the amount of bytes the buffer can hold
      * @return  the new size after encoding
      **/
      int filter_decode (char * source, int size, int max_size) const;

      /**
       * Prints the number of filters chained for each type to the
       * send filter
       **/
      void print_num_filters_send (void) const;

      /**
       * Prints the number of filters chained for each type to the
       * receive filter
       **/
      void print_num_filters_receive (void) const;

      /**
       * Prints the number of filters chained for each type to the
       * rebroadcast filter
       **/
      void print_num_filters_rebroadcast (void) const;

      /**
       * Sets the time to live for our packets. This number is decremented
       * by one by each receiver. If time to live is greater than 0, the
       * message will be rebroadcasted. Default is 0.
       * @param  ttl   time-to-live for message sent over this transport
       **/
      void set_rebroadcast_ttl (unsigned char ttl);

      /**
       * Gets the time to live for rebroadcasting (number of rebroadcasts
       * per message).
       **/
      unsigned char get_rebroadcast_ttl (void) const;

      /**
       * Returns the number of types that are filtered before send
       * @return  the number of types that have filters
       **/
      size_t get_number_of_send_filtered_types (void) const;
      
      /**
       * Returns the number of aggregate filters applied before sending
       * @ return the number of aggregate filters
       **/
      size_t get_number_of_send_aggregate_filters (void) const;
       
      /**
       * Returns the number of aggregate filters applied before
       * rebroadcasting
       * @ return the number of aggregate filters
       **/
      size_t get_number_of_rebroadcast_aggregate_filters (void) const;

      /**
       * Returns the number of types that are filtered before rebroadcast
       * @return  the number of types that have filters
       **/
      size_t get_number_of_rebroadcast_filtered_types (void) const;
       
      /**
       * Returns the number of aggregate filters applied after receiving
       * @ return the number of aggregate filters
       **/
      size_t get_number_of_receive_aggregate_filters (void) const;

      /**
       * Returns the number of types that are filtered after received
       * @return  the number of types that have filters
       **/
      size_t get_number_of_receive_filtered_types (void) const;

      /**
      * Returns the number of buffer filters
      * @return  the number of buffer filters
      **/
      size_t get_number_of_buffer_filters (void) const;

      /**
       * Adds a trusted peer. By default, all peers are trusted unless
       * a trusted peer or banned peer is added to the settings.
       * @param   peer    peer to add (generally ip:port)
       **/
      void add_trusted_peer (const std::string & peer);

      /**
       * Adds a banned peer. By default, all peers are trusted unless
       * a trusted peer or banned peer is added to the settings.
       * @param   peer    peer to add (generally ip:port)
       **/
      void add_banned_peer (const std::string & peer);
      
      /**
       * Removes a trusted peer, if it exists in the list.
       * @param   peer    peer to remove (generally ip:port)
       * @return  true if peer existed, false if peer was not in list
       **/
      bool remove_trusted_peer (const std::string & peer);

      /**
       * Removes a trusted peer, if it exists in the list.
       * @param   peer    peer to remove (generally ip:port)
       * @return  true if peer existed, false if peer was not in list
       **/
      bool remove_banned_peer (const std::string & peer);

      /**
       * Checks if a peer is trusted.
       * @param   peer    peer to check (generally ip:port)
       * @return true if peer is trusted, false otherwise
       **/
      bool is_trusted (const std::string & peer) const;

      /**
       * Enables rebroadcast support up to a certain time to live for other
       * agent's messages. Default is 0 (no participation in rebroadcasts)
       * @param   maximum_ttl   Maximum time to live to support (0 for not
       *                        participating in rebroadcasts). A transport
       *                        cannot participate in more than 255 resends.
       **/
      void enable_participant_ttl (unsigned char maximum_ttl = 255);

      /**
       * Returns the maximum time to live participation of this transport in
       * rebroadcasting of other agent's messages.
       * @return                the maximum number of ttls this transport will
       *                        participate in (per message)
       **/
      unsigned char get_participant_ttl (void) const;

      /**
       * Updates a packet drop rate, type, and burst
       * @param   drop_rate    percent drop rate for sending packets
       * @param   drop_type    type of drop rate policy to use
       *                       @see PacketDropTypes
       * @param   drop_burst   number of packets to drop consecutively
       **/
      void update_drop_rate (double drop_rate,
        int drop_type = PACKET_DROP_DETERMINISTIC,
        uint64_t drop_burst = 1);

      /**
       * Returns the percentage of dropped packets to enforce on sends
       * @return the percentage of dropped packets to enforce
       **/
      double get_drop_rate (void) const;
      
      /**
       * Returns the policy type for packet drops
       * @return  the policy type for packet drops
       **/
      int get_drop_type (void) const;

      /**
       * Returns the bursts of packet drops
       * @return  the consecutive bursts of dropped packets
       **/
      uint64_t get_drop_burst (void) const;

      /**
       * Sets a bandwidth limit for sending on this transport in bytes per sec
       * @param   bandwidth  send bandwidth in bytes per second
       **/
      void set_send_bandwidth_limit (int64_t bandwidth);
      
      /**
       * Returns the limit for sending on this transport in bytes per second
       * @return the send bandwidth limiting in bytes per second
       **/
      int64_t  get_send_bandwidth_limit (void) const;
       
      /**
       * Sets a bandwidth limit for receiving and sending over the transport.
       * -1 means no limit.
       * @param   bandwidth   total bandwidth limit in bytes per second
       **/
      void set_total_bandwidth_limit (int64_t bandwidth);
       
      /**
       * Returns the total limit for this transport in bytes per second.
       * -1 means no limit.
       * @return the total bandwidth limit in bytes per second
       **/
      int64_t get_total_bandwidth_limit (void) const;

      /**
       * Sets the packet deadline in seconds. Note that most transports only
       * enforce deadline in seconds. However, future transports may allow
       * for microsecond or millisecond deadlines, so we use a double here
       * for expansion purposes. -1 means no deadline.
       * @param   deadline   deadline in seconds
       **/
       void set_deadline (double deadline);

      /**
       * Returns the latency deadline in seconds. -1 means no deadline.
       * @return  deadline in seconds
       **/
       double get_deadline (void) const;

       /**
       * Loads the settings from a binary file
       * @param  filename    the file to load from
       * @param  prefix      prefix for all transports settings
       **/
       virtual void load (const std::string & filename,
         const std::string & prefix = "transport");

       /**
       * Loads the settings from a text file
       * @param  filename    the file to load from
       * @param  prefix      prefix for all transports settings
       **/
       virtual void load_text (const std::string & filename,
         const std::string & prefix = "transport");

       /**
       * Saves the settings from a binary file
       * @param  filename    the file to load from
       * @param  prefix      prefix for all transports settings
       **/
       virtual void save (const std::string & filename,
         const std::string & prefix = "transport") const;

       /**
       * Saves the settings from a text file
       * @param  filename    the file to load from
       * @param  prefix      prefix for all transports settings
       **/
       virtual void save_text (const std::string & filename,
         const std::string & prefix = "transport") const;

    private:
       
      /**
       * number of rebroadcasts for receivers to ultimately do. This field
       * is experimental and is meant to allow for rebroadcasting messages
       * across a number of loosely connected agents or amongst a routing
       * network. For instance, if A is connected to B and B is connected
       * to C, and A would like to have its update known to C, then a
       * rebroadcast_ttl of 1 would instruct B to rebroadcast updates
       * with a ttl of (rebroadcast_ttl - 1), resulting in 0 ttl when
       * received by C (it will not rebroadcast).
       **/
      unsigned char rebroadcast_ttl_;

      /**
       * This field is meant to limit the number of rebroadcasts that this
       * transport will participate in. If a ttl > 0 is received on a
       * message header, the new ttl for rebroadcasts will be set to the
       * minimum of this field and the decrement of the ttl of the message.
       * A zero here means the transport will not participate in rebroadcasts
       **/
      unsigned char participant_rebroadcast_ttl_;

      /**
       * A container of all trusted peers
       **/
      std::map <std::string, int> trusted_peers_;
      
      /**
       * A container of all banned peers
       **/
      std::map <std::string, int> banned_peers_;

      /**
       * A container for rebroadcast filters
       **/
      knowledge::KnowledgeRecordFilters  rebroadcast_filters_;
         
      /**
       * A container for receive filters
       **/
      knowledge::KnowledgeRecordFilters  receive_filters_;

      /**
       * A container for filters applied before sending from this host
       **/
      knowledge::KnowledgeRecordFilters  send_filters_;

      /**
       * buffer filters have an encode and decode method
       **/
      filters::BufferFilters   buffer_filters_;

      /**
       * Rate for dropping packets
       **/
      double packet_drop_rate_;

      /**
       * Drop rate type
       **/
      int packet_drop_type_;

      /**
       * Burst of packet drops
       **/
      uint64_t packet_drop_burst_;

      /**
       * Maximum send bandwidth usage per second before packets drop
       **/
      int64_t max_send_bandwidth_;

      /**
       * Maximum bandwidth usage for the transport (receive/send) before drop
       **/
      int64_t max_total_bandwidth_;

      /**
       * Deadline for packets at which packets drop
       **/
      double deadline_;
    };
  } // end Transport namespace
} // end Madara namespace

#endif // _MADARA_QOS_TRANSPORT_SETTINGS_H_
