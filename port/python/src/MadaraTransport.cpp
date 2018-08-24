#ifndef _MADARA_PYTHON_PORT_MADARA_TRANSPORT_H_
#define _MADARA_PYTHON_PORT_MADARA_TRANSPORT_H_

#include <boost/python/detail/wrap_python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <boost/python/suite/indexing/map_indexing_suite.hpp>
#include <boost/python/dict.hpp>
#include <boost/python/import.hpp>
#include <boost/python/enum.hpp>

#include "madara/knowledge/KnowledgeBase.h"
#include "madara/filters/GenericFilters.h"
#include "FunctionDefaults.h"
#include "MadaraFilters.h"
#include "MadaraTransport.h"

/**
 * @file MadaraTransport.cpp
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains Boost.Python mappings for madara.transport module
 **/

using namespace boost::python;

class Transport_NS {};

void define_transport (void)
{
  object transport = object (handle<> (
    PyModule_New ("pymadara.transport")));

   transport.attr("__file__")="<synthetic>";
   scope().attr ("transport") = transport;
   transport.attr ("__doc__") = "Provides access to the transport layer";

   // this was the missing piece: sys.modules['modA.modB']=modB
   extract <dict> (getattr (
     import ("sys"), "modules"))()["pymadara.transport"] = transport;

  scope Transport = transport;

  // the types of packet drop policies for QoSTransportSettings class
  enum_<madara::transport::PacketDropType>("DropTypes")
    .value("PACKET_DROP_DETERMINISTIC",
      madara::transport::PACKET_DROP_DETERMINISTIC)
    .value("PACKET_DROP_PROBABLISTIC",
      madara::transport::PACKET_DROP_PROBABLISTIC)
  ;
    
  // the types of packet drop policies for QoSTransportSettings class
  enum_<madara::transport::Types>("TransportTypes")
    .value("NO_TRANSPORT",
      madara::transport::NO_TRANSPORT)
    .value("SPLICE",
      madara::transport::SPLICE)
    .value("NDDS",
      madara::transport::NDDS)
    .value("TCP",
      madara::transport::TCP)
    .value("UDP",
      madara::transport::UDP)
    .value("MULTICAST",
      madara::transport::MULTICAST)
    .value("BROADCAST",
      madara::transport::BROADCAST)
    .value("REGISTRY_SERVER",
      madara::transport::REGISTRY_SERVER)
    .value("REGISTRY_CLIENT",
      madara::transport::REGISTRY_CLIENT)
    .value("ZMQ",
      madara::transport::ZMQ)
  ;
    
  {
    /********************************************************
      * Transport Context definitions
      ********************************************************/
    
    scope TransportContext = 
      class_<madara::transport::TransportContext> ("TransportContext",
      "Container for transport-related state information", init <> ())

      // copy constructor
      .def (init<const madara::transport::TransportContext> ())

      // clear records
      .def ("clear_records",
        &madara::transport::TransportContext::clear_records,
        "Clears any uncommitted records associated with the context")
        
      // Retrieves the operation type
      .def ("get_operation",
        &madara::transport::TransportContext::get_operation,
        "Retrieves the operation type")
        
      // Retrieves receive bandwidth usage
      .def ("get_receive_bandwidth",
        &madara::transport::TransportContext::get_receive_bandwidth,
        "Returns received bandwidth usage in B/s over past 10s")
        
      // Retrieves send bandwidth usage
      .def ("get_send_bandwidth",
        &madara::transport::TransportContext::get_send_bandwidth,
        "Returns send bandwidth usage in B/s over past 10s")
        
      // Retrieves time that message was generated
      .def ("get_message_time",
        &madara::transport::TransportContext::get_message_time,
        "Retrieves time that message was generated")
        
      // Retrieves time that message is being processed
      .def ("get_current_time",
        &madara::transport::TransportContext::get_current_time,
        "Retrieves time that message is being processed")
          
      // Adds a record to the context for rebroadcasting or applying
      .def ("add_record",
        &madara::transport::TransportContext::add_record,
        "Adds a record to the context for rebroadcasting or applying")
          
      // Retrieves the knowledge domain (similar to topics in pub/sub)
      .def ("get_domain",
        &madara::transport::TransportContext::get_domain, 
        return_value_policy<reference_existing_object>(),
        "Retrieves the knowledge domain (similar to topics in pub/sub)")
          
      // Retrieves the message originator
      .def ("get_originator",
        &madara::transport::TransportContext::get_originator, 
        return_value_policy<reference_existing_object>(),
        "Retrieves the message originator")
          
      // Retrieves the current uncommitted record list"
      .def ("get_records",
        &madara::transport::TransportContext::get_records, 
        return_value_policy<reference_existing_object>(),
        "Retrieves the current uncommitted record list")

    ;

    // the types of packet drop policies for QoSTransportSettings class
    enum_<madara::transport::TransportContext::Operations> ("Operations")
      .value("IDLE_OPERATION",
        madara::transport::TransportContext::IDLE_OPERATION)
      .value("SENDING_OPERATION",
        madara::transport::TransportContext::SENDING_OPERATION)
      .value("RECEIVING_OPERATION",
        madara::transport::TransportContext::RECEIVING_OPERATION)
      .value("REBROADCASTING_OPERATION",
        madara::transport::TransportContext::REBROADCASTING_OPERATION)
      ;
  }
      
  /********************************************************
    * Transport Settings definitions
    ********************************************************/

  class_<madara::transport::TransportSettings> ("TransportSettings",
    "The main transport settings class", init <> ())
      
    .def (init<const madara::transport::TransportSettings &> ())

    .def ("add_read_domain",
    &madara::transport::TransportSettings::add_read_domain,
    "Adds a read domain to subscribe to")

    // define readwrite variables within the class
    .def_readwrite ("queue_length",
      &madara::transport::TransportSettings::queue_length,
      "Informs the transport of the requested queue_length in bytes")

    .def_readwrite ("type",
      &madara::transport::TransportSettings::type,
      "Indicates the type of transport (see TransportTypes)")

    .def_readwrite ("reliability",
      &madara::transport::TransportSettings::reliability,
      "Informs the transport of the requested reliability (0==BEST_EFFORT)")

    .def_readwrite ("id",
      &madara::transport::TransportSettings::id,
      "Informs the transport of the process id")

    .def_readwrite ("processes",
      &madara::transport::TransportSettings::processes,
      "Informs the transport of the number of expected processes")

    .def_readwrite ("on_data_received_logic",
      &madara::transport::TransportSettings::on_data_received_logic,
      "Provides a KaRL expression to evaluate when data is received")

    .def_readwrite ("delay_launch",
      &madara::transport::TransportSettings::delay_launch,
      "Indicates that the transport should delay launch until activated")

    .def_readwrite ("never_exit",
      &madara::transport::TransportSettings::never_exit,
      "Indicates that the transport should never exit, even in bad states")

    .def_readwrite ("send_reduced_message_header",
      &madara::transport::TransportSettings::send_reduced_message_header,
      "Indicates that a reduced message header should be used for messages")

    .def_readwrite ("hosts",
      &madara::transport::TransportSettings::hosts,
      "List of hosts for the transport layer")

    .def_readwrite ("write_domain",
      &madara::transport::TransportSettings::write_domain,
      "Indicates the domain to write to")

  ;
    
  /********************************************************
    * QoSTransportSettings definitions
    ********************************************************/

  class_<madara::transport::QoSTransportSettings,
          bases<madara::transport::TransportSettings> > (
            "QoSTransportSettings",
            "Transport settings with quality-of-service focus", init<> ())
      
    .def (init <const madara::transport::QoSTransportSettings &> ())
      
    .def (init <const madara::transport::TransportSettings &> ())
      
    // adds a python callback to the typed send filter chains
    .def( "add_rebroadcast_filter",
      static_cast<
        void (madara::transport::QoSTransportSettings::*)(
              uint32_t types, boost::python::object & object)
      > (&madara::transport::QoSTransportSettings::add_rebroadcast_filter),
      "Adds a python callback to the typed rebroadcst filter chains")
        
    // adds a python callback to the aggregate send filter chains
    .def( "add_rebroadcast_filter",
      static_cast<
        void (madara::transport::QoSTransportSettings::*)(
              boost::python::object & object)
      > (&madara::transport::QoSTransportSettings::add_rebroadcast_filter),
      "Adds a python callback to the aggregate rebroadcast filter chains")
        
    // adds a python callback to the typed receive filter chains
    .def( "add_receive_filter",
      static_cast<
        void (madara::transport::QoSTransportSettings::*)(
              uint32_t types, boost::python::object & object)
      > (&madara::transport::QoSTransportSettings::add_receive_filter),
      "Adds a python callback to the typed receive filter chains")
        
    // adds a python callback to the aggregate receive filter chains
    .def( "add_receive_filter",
      static_cast<
        void (madara::transport::QoSTransportSettings::*)(
              boost::python::object & object)
      > (&madara::transport::QoSTransportSettings::add_receive_filter),
      "Adds a python callback to the aggregate receive filter chains")
        
    // adds a python callback to the typed send filter chains
    .def( "add_send_filter",
      static_cast<
        void (madara::transport::QoSTransportSettings::*)(
              uint32_t types, boost::python::object & object)
      > (&madara::transport::QoSTransportSettings::add_send_filter),
      "Adds a python callback to the typed send filter chains")
        
    // adds a python callback to the aggregate send filter chains
    .def( "add_send_filter",
      static_cast<
        void (madara::transport::QoSTransportSettings::*)(
              boost::python::object & object)
      > (&madara::transport::QoSTransportSettings::add_send_filter),
      "Adds a python callback to the aggregate send filter chains")

    // Clears the rebroadcast filters for a specified type
    // .def ("add_filter",
    //   &madara::transport::QoSTransportSettings::add_filter,
    //   "Adds a BufferFilter to the transport")
        
    // adds a python callback to the typed send filter chains
    .def( "add_filter",
      static_cast<
        void (madara::transport::QoSTransportSettings::*)(
              madara::filters::BufferFilter *)
      > (&madara::transport::QoSTransportSettings::add_filter),
      "Adds a python callback to the typed send filter chains")
        
    // Clears the rebroadcast filters for a specified type
    .def ("clear_rebroadcast_filters",
      &madara::transport::QoSTransportSettings::clear_rebroadcast_filters,
      "Clears the rebroadcast filters for a specified type")
        
    // Clears the receive filters for a specified type
    .def ("clear_receive_filters",
      &madara::transport::QoSTransportSettings::clear_receive_filters,
      "Clears the receive filters for a specified type")
        
    // Clears the send filters for a specified type
    .def ("clear_send_filters",
      &madara::transport::QoSTransportSettings::clear_send_filters,
      "Clears the send filters for a specified type")
        
    // Clears the rebroadcast filters for a specified type
    .def ("clear_rebroadcast_aggregate_filters",
      &madara::transport::QoSTransportSettings::clear_rebroadcast_aggregate_filters,
      "Clears the rebroadcast aggregate filters")
        
    // Clears the receive filters for a specified type
    .def ("clear_receive_aggregate_filters",
      &madara::transport::QoSTransportSettings::clear_receive_aggregate_filters,
      "Clears the receive aggregate filters")
        
    // Clears the send filters for a specified type
    .def ("clear_send_aggregate_filters",
      &madara::transport::QoSTransportSettings::clear_send_aggregate_filters,
      "Clears the send aggregate filters")
        
    // Clears the send filters for a specified type
    .def ("clear_buffer_filters",
      &madara::transport::QoSTransportSettings::clear_buffer_filters,
      "Clears the buffer filters")
        

    // get the drop burst rate
    .def ("get_drop_burst",
      &madara::transport::QoSTransportSettings::get_drop_burst,
      "Retrieves the drop bursts")
        
    // update the drop rate
    .def ("get_drop_rate",
      &madara::transport::QoSTransportSettings::get_drop_rate,
      "Retrieves the rate of drops (0.8 is 80%)")
        
    // get the drop type
    .def ("get_drop_type",
      &madara::transport::QoSTransportSettings::get_drop_type,
      "Retrieves the type of drops")
        
    // Prints the number of rebroadcast filters for a specified type
    .def ("print_num_filters_rebroadcast",
      &madara::transport::QoSTransportSettings::print_num_filters_rebroadcast,
      "Prints the number of rebroadcast filters for a specified type")
        
    // Prints the number of receive filters for a specified type
    .def ("print_num_filters_receive",
      &madara::transport::QoSTransportSettings::print_num_filters_receive,
      "Prints the number of receive filters for a specified type")
        
    // Prints the number of send filters for a specified type
    .def ("print_num_filters_send",
      &madara::transport::QoSTransportSettings::print_num_filters_send,
      "Prints the number of send filters for a specified type")
        
    // Gets the number of rebroadcast aggregate filters
    .def ("get_number_of_rebroadcast_aggregate_filters",
      &madara::transport::QoSTransportSettings::get_number_of_rebroadcast_aggregate_filters,
      "Gets the number of receive aggregate filters")
        
    // Gets the number of receive filters for a specified type
    .def ("get_number_of_receive_aggregate_filters",
      &madara::transport::QoSTransportSettings::get_number_of_receive_aggregate_filters,
      "Gets the number of receive aggregate filters")
        
    // Gets the number of send aggregate filters
    .def ("get_number_of_send_aggregate_filters",
      &madara::transport::QoSTransportSettings::get_number_of_send_aggregate_filters,
      "Gets the number of send aggregate filters")
        
    // Gets the number of rebroadcast filters for a specified type
    .def ("get_number_of_rebroadcast_filtered_types",
      &madara::transport::QoSTransportSettings::get_number_of_rebroadcast_filtered_types,
      "Gets the number of rebroadcast filters for a specified type")
        
    // Gets the number of receive filters for a specified type
    .def ("get_number_of_receive_filtered_types",
      &madara::transport::QoSTransportSettings::get_number_of_receive_filtered_types,
      "Gets the number of receive filters for a specified type")
        
    // Gets the number of send filters for a specified type
    .def ("get_number_of_send_filtered_types",
      &madara::transport::QoSTransportSettings::get_number_of_send_filtered_types,
      "Gets the number of send filters for a specified type")

    // update the drop rate
    .def ("update_drop_rate",
      &madara::transport::QoSTransportSettings::update_drop_rate,
      "Informs the transport of the drop rate, type, and bursts")
        
    // the rebroadcast time-to-live for the transport
    .add_property("participant_ttl",
      &madara::transport::QoSTransportSettings::get_participant_ttl,
      &madara::transport::QoSTransportSettings::enable_participant_ttl,
      "Informs the transport that it should participate in rebroadcasting")

    // the rebroadcast time-to-live for the transport
    .add_property("rebroadcast_ttl",
      &madara::transport::QoSTransportSettings::get_rebroadcast_ttl,
      &madara::transport::QoSTransportSettings::set_rebroadcast_ttl,
      "Informs the transport of the time-to-live attached to all updates")

    // the send bandwidth for the transport
    .add_property("send_bandwidth",
    &madara::transport::QoSTransportSettings::get_send_bandwidth_limit,
      &madara::transport::QoSTransportSettings::set_send_bandwidth_limit,
      "Informs the transport of the send bandwidth limit in B/s that"
      " throttles the sending of new updates")

    // the total bandwidth for the transport
    .add_property("total_bandwidth",
    &madara::transport::QoSTransportSettings::get_total_bandwidth_limit,
      &madara::transport::QoSTransportSettings::set_total_bandwidth_limit,
      "Informs the transport of the total bandwidth limit in B/s that"
      " throttles the sending of new updates")

    // the deadline for messages
    .add_property("deadline",
      &madara::transport::QoSTransportSettings::get_deadline,
      &madara::transport::QoSTransportSettings::set_deadline,
      "Informs the transport of the deadline, in seconds, for useful info")
  ;
}

#endif // _MADARA_PYTHON_PORT_MADARA_TRANSPORT_H_

