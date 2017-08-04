#include "madara/knowledge/KnowledgeBase.h"
#include "madara/filters/GenericFilters.h"
#include "FunctionDefaults.h"

#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <boost/python/suite/indexing/map_indexing_suite.hpp>

/**
 * @file MadaraModule.cpp
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains Boost.Python mappings for the C++ MADARA library
 * to a Python module called Madara.
 **/

using namespace boost::python;

class Filters_NS {};
class knowledge_NS {};
class Transport_NS {};

void define_knowledge_record (void)
{
}

/********************************************************
  * Filters namespace definitions
  ********************************************************/
void define_filters (void)
{
  scope filters = class_ <Filters_NS, boost::noncopyable> (
    "filters", no_init)
        
    // update the drop rate
    .def ("discard",
      &madara::filters::discard,
      "Drops the current record from the filtering process")
        
    // update the drop rate
    .def ("discard_nonprimitives",
      &madara::filters::discard_nonprimitives,
      "Drops nonprimitive records from the filtering process")
        
    // update the drop rate
    .def ("discard_nonfiles",
      &madara::filters::discard_nonfiles,
      "Drops nonfile records from the filtering process")
        
    // update the drop rate
    .def ("log_args",
      &madara::filters::log_args,
      "Logs all arguments passed via the filtering process")
        
    // update the drop rate
    .def ("log_aggregate",
      &madara::filters::log_aggregate,
      "Logs aggregate updates passed via the filtering process")
  ;
  
  filters.attr ("__file__") = "<synthetic>";
  scope().attr ("filters") = filters;
  filters.attr ("__doc__") = "Provides general purpose filters";

  extract <dict> (getattr (
    import ("sys"), "modules"))()["madara.filters"] = filters;
}

void define_transport (void)
{
  object transport = object (handle<> (
    PyModule_New ("madara.transport")));

   transport.attr("__file__")="<synthetic>";
   scope().attr ("transport") = transport;
   transport.attr ("__doc__") = "Provides access to the transport layer";

   // this was the missing piece: sys.modules['modA.modB']=modB
   extract <dict> (getattr (
     import ("sys"), "modules"))()["madara.transport"] = transport;

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
  
void define_knowledge_engine (void)
{
  object ke = object (handle<> (
    PyModule_New ("madara.knowledge")));

   ke.attr("__file__")="<synthetic>";
   scope().attr ("knowledge_engine") = ke;
   ke.attr ("__doc__") = "Provides access to the knowledge engine";

   // this was the missing piece: sys.modules['modA.modB']=modB
   extract <dict> (getattr (
     import ("sys"), "modules"))()["madara.knowledge"] = ke;

   scope knowledge = ke;

/*
   scope knowledge
    = class_ <knowledge_NS, boost::noncopyable> (
      "knowledge_engine", no_init);
   */



   class_<madara::knowledge::KnowledgeRecord> (
     "KnowledgeRecord",
     "Basic unit of knowledge", init <> ())

     // integer constructor
     .def (init <int64_t> ())

     .def (init <double> ())

     .def (init <std::vector <int64_t> > ())

     .def (init <std::vector <double> > ())

     .def (init <std::string> ())

     .def (init <const madara::knowledge::KnowledgeRecord &> ())

     // clears the value to a 0 integer
     .def ("clear_value", &madara::knowledge::KnowledgeRecord::clear_value,
     "Sets the value to 0 and type to integer")

     // sets the contents of the record to a jpeg
     .def ("read_file", &madara::knowledge::KnowledgeRecord::read_file,
     "Reads the contents of a file into the record")

     // gets the double precision
     .def ("get_precision", &madara::knowledge::KnowledgeRecord::get_precision,
     "Gets the double precision used in to_string")

     // decrements an index of an array
     .def ("dec_index", &madara::knowledge::KnowledgeRecord::inc_index,
     "Decrements an array element at a particular index")

     // increments an index of an array
     .def ("inc_index", &madara::knowledge::KnowledgeRecord::inc_index,
     "Increments an array element at a particular index")

     // retrieves an index of an array
     .def ("retrieve_index", &madara::knowledge::KnowledgeRecord::retrieve_index,
     "Retrieves an array element at a particular index")

     // sets the double precision
     .def ("set_precision", &madara::knowledge::KnowledgeRecord::set_precision,
     "Sets the double precision, generally for to_string")

     // reset the record to UNCREATED
     .def ("reset_value", &madara::knowledge::KnowledgeRecord::reset_value,
     "Resets the record to an UNCREATED status (faster than clear_value)")

     // sets the contents of the record to a jpeg
     .def ("set_jpeg", &madara::knowledge::KnowledgeRecord::set_jpeg,
     "Sets the value to a jpeg")

     // sets the contents of the record to a file
     .def ("set_file", &madara::knowledge::KnowledgeRecord::set_file,
     "Sets the value to a file's contents")

     // sets a knowledge record to a double
     .def ("set",
     static_cast<
     void (madara::knowledge::KnowledgeRecord::*)(
     const double &
     )
     > (&madara::knowledge::KnowledgeRecord::set_value),
     "Sets the value to a double")

     // sets a knowledge record to an array of doubles
     .def ("set",
     static_cast<
     void (madara::knowledge::KnowledgeRecord::*)(
     const std::vector <double> &
     )
     > (&madara::knowledge::KnowledgeRecord::set_value),
     "Sets the value to an array of doubles")

     // sets a knowledge record to an integer
     .def ("set",
     static_cast<
     void (madara::knowledge::KnowledgeRecord::*)(
     const madara::knowledge::KnowledgeRecord::Integer &
     )
     > (&madara::knowledge::KnowledgeRecord::set_value),
     "Sets the value to an integer")

     // sets a knowledge record to an array of integer
     .def ("set",
     static_cast<
     void (madara::knowledge::KnowledgeRecord::*)(
     const std::vector <madara::knowledge::KnowledgeRecord::Integer> &
     )
     > (&madara::knowledge::KnowledgeRecord::set_value),
     "Sets the value to an array of integers")

     // sets a knowledge record to a string
     .def ("set",
     static_cast<
     void (madara::knowledge::KnowledgeRecord::*)(
     const std::string &
     )
     > (&madara::knowledge::KnowledgeRecord::set_value),
     "Sets the value to a string")

     // sets an array index to an integer
     .def ("set_index",
     static_cast<
     void (madara::knowledge::KnowledgeRecord::*)(
     size_t,
     madara::knowledge::KnowledgeRecord::Integer)
     > (&madara::knowledge::KnowledgeRecord::set_index),
     "Sets an array index to an integer")

     // sets an array index to a double
     .def ("set_index",
     static_cast<
     void (madara::knowledge::KnowledgeRecord::*)(
     size_t,
     double)
     > (&madara::knowledge::KnowledgeRecord::set_index),
     "Sets an array index to a double (and converts the array to double)")

     // sets the contents of the record to a jpeg
     .def ("size", &madara::knowledge::KnowledgeRecord::size,
     "Returns the size of the value")

     // convert to a string
     .def ("status", &madara::knowledge::KnowledgeRecord::status,
     "Returns the status of the record")

     // convert to a string
     .def ("to_string", &madara::knowledge::KnowledgeRecord::to_string,
     m_to_string_0_of_1 (
     args ("delimiter"),
     "Converts the record to a string"))

     // convert to an integer
     .def ("to_integer", &madara::knowledge::KnowledgeRecord::to_integer,
     "Converts the record to an integer")

     // convert to a std::vector of integers
     .def ("to_integers", &madara::knowledge::KnowledgeRecord::to_integers,
     "Converts the record to an array of integers")

     // convert to a double
     .def ("to_double", &madara::knowledge::KnowledgeRecord::to_double,
     "Converts the record to a double")

     // convert to a std::vector of doubles
     .def ("to_doubles", &madara::knowledge::KnowledgeRecord::to_doubles,
     "Converts the record to an array of doubles")

     // save value to a file
  .def ("to_file", &madara::knowledge::KnowledgeRecord::to_file,
  "Saves the value of the record to a file")

  // gets the type of the record
  .def ("type", &madara::knowledge::KnowledgeRecord::type,
  "Returns the value type")

  // fragments the record
  .def ("fragment", &madara::knowledge::KnowledgeRecord::fragment,
  "Fragments the record into components")

  // checks if the record is false
  .def ("is_false", &madara::knowledge::KnowledgeRecord::is_false,
  "Checks if the record is false")

  // checks if the record is true
  .def ("is_true", &madara::knowledge::KnowledgeRecord::is_true,
  "Checks if the record is true")

  // checks if record is a binary file type
  .def ("is_binary_file_type",
  static_cast<bool (madara::knowledge::KnowledgeRecord::*)(void) const> (
  &madara::knowledge::KnowledgeRecord::is_binary_file_type),
  "Checks if the record is a binary file type")

  // checks if record is a double type
  .def ("is_double_type",
  static_cast<bool (madara::knowledge::KnowledgeRecord::*)(void) const> (
  &madara::knowledge::KnowledgeRecord::is_double_type),
  "Checks if the record is a double type")

  // checks if record is a file type
  .def ("is_file_type",
  static_cast<bool (madara::knowledge::KnowledgeRecord::*)(void) const> (
  &madara::knowledge::KnowledgeRecord::is_file_type),
  "Checks if the record is a file type")

  // checks if the record is an image type
  .def ("is_image_type",
  static_cast<bool (madara::knowledge::KnowledgeRecord::*)(void) const> (
  &madara::knowledge::KnowledgeRecord::is_image_type),
  "Checks if the record is an image type")

  // checks if the record is an integer type
  .def ("is_integer_type",
  static_cast<bool (madara::knowledge::KnowledgeRecord::*)(void) const> (
  &madara::knowledge::KnowledgeRecord::is_integer_type),
  "Checks if the record is an integer type")

  // checks if the record is a string type
  .def ("is_string_type",
  static_cast<bool (madara::knowledge::KnowledgeRecord::*)(void) const> (
  &madara::knowledge::KnowledgeRecord::is_string_type),
  "Checks if the record is a string type")

  // overloaded operators
  .def (self < self)
  .def (self <= self)
  .def (self == double ())
  .def (self == std::string ())
  .def (self == self)
  .def (self != self)
  .def (self > self)
  .def (self >= self)
  .def (!self)
  .def (-self)
  .def (self += self)
  .def (self -= self)
  .def (self *= self)
  .def (self /= self)
  .def (self %= self)
  .def (self * self)
  .def (self / self)
  .def (self % self)
  .def (self + self)
  .def (self - self)

  .def ("operator=", &madara::knowledge::KnowledgeRecord::operator=,
  "Assigns the value of one record to another",
  return_value_policy<reference_existing_object> ())

  .def ("operator==",
  static_cast<bool (
  madara::knowledge::KnowledgeRecord::*)(
  const madara::knowledge::KnowledgeRecord &) const> (
  &madara::knowledge::KnowledgeRecord::operator==),
  "Compares two records for equality")

  .def ("operator==",
  static_cast<bool (madara::knowledge::KnowledgeRecord::*)(
  madara::knowledge::KnowledgeRecord::Integer) const> (
  &madara::knowledge::KnowledgeRecord::operator==),
  "Compares two records for equality")

  .def ("operator++",
  &madara::knowledge::KnowledgeRecord::operator++,
  "Adds one to the record",
  return_value_policy<reference_existing_object> ())

  .def ("operator--",
  &madara::knowledge::KnowledgeRecord::operator--,
  "Subtracts one from the record",
  return_value_policy<reference_existing_object> ())
  ; // end of KnowledgeRecord


     // the types of values supported in KnowledgeRecord
     enum_<madara::knowledge::KnowledgeRecord::ValueTypes> ("ValueTypes")
     .value ("UNINITIALIZED",
     madara::knowledge::KnowledgeRecord::UNINITIALIZED)
     .value ("INTEGER",
     madara::knowledge::KnowledgeRecord::INTEGER)
     .value ("STRING",
     madara::knowledge::KnowledgeRecord::STRING)
     .value ("DOUBLE",
     madara::knowledge::KnowledgeRecord::DOUBLE)
     .value ("FLOAT",
     madara::knowledge::KnowledgeRecord::FLOAT)
     .value ("UNKNOWN_FILE_TYPE",
     madara::knowledge::KnowledgeRecord::UNKNOWN_FILE_TYPE)
     .value ("XML",
     madara::knowledge::KnowledgeRecord::XML)
     .value ("TEXT_FILE",
     madara::knowledge::KnowledgeRecord::TEXT_FILE)
     .value ("DOUBLE_ARRAY",
     madara::knowledge::KnowledgeRecord::DOUBLE_ARRAY)
     .value ("INTEGER_ARRAY",
     madara::knowledge::KnowledgeRecord::INTEGER_ARRAY)
     .value ("IMAGE_JPEG",
     madara::knowledge::KnowledgeRecord::IMAGE_JPEG)
     .value ("ALL_ARRAYS",
     madara::knowledge::KnowledgeRecord::ALL_ARRAYS)
     .value ("ALL_INTEGERS",
     madara::knowledge::KnowledgeRecord::ALL_INTEGERS)
     .value ("ALL_DOUBLES",
     madara::knowledge::KnowledgeRecord::ALL_DOUBLES)
     .value ("ALL_PRIMITIVE_TYPES",
     madara::knowledge::KnowledgeRecord::ALL_PRIMITIVE_TYPES)
     .value ("ALL_FILE_TYPES",
     madara::knowledge::KnowledgeRecord::ALL_FILE_TYPES)
     .value ("ALL_IMAGES",
     madara::knowledge::KnowledgeRecord::ALL_IMAGES)
     .value ("ALL_TEXT_FORMATS",
     madara::knowledge::KnowledgeRecord::ALL_TEXT_FORMATS)
     .value ("ALL_TYPES",
     madara::knowledge::KnowledgeRecord::ALL_TYPES)
   ; // end of ValueTypes

   // Boost.python does not appear to support this type of function
   //.def ("to_unmanaged_buffer", &madara::knowledge::KnowledgeRecord::to_unmanaged_buffer)





  /********************************************************
    * KnowledgeReferenceSettings definitions
    ********************************************************/
 
  class_<madara::knowledge::KnowledgeReferenceSettings> (
      "KnowledgeReferenceSettings",
      "Settings for referencing knowledge records", init<> ())
        
    .def (init <bool> ())
    .def (init <
      const madara::knowledge::KnowledgeReferenceSettings &> ())

    // define readwrite variables within the class
    .def_readwrite ("expand_variables",
      &madara::knowledge::KnowledgeReferenceSettings::expand_variables,
      "Always attempt to expand variable names")
    .def_readwrite ("never_exit",
      &madara::knowledge::KnowledgeReferenceSettings::never_exit,
      "Do not exit, even if in invalid states")

  ; // end class KnowledgeReferenceSettings
          
  /********************************************************
    * KnowledgeUpdateSettings definitions
    ********************************************************/
 
  class_<madara::knowledge::KnowledgeUpdateSettings,
          bases<madara::knowledge::KnowledgeReferenceSettings> > (
            "KnowledgeUpdateSettings",
            "Settings for updating knowledge records", init <> ())
    
    // other constructors here
    .def (init <
      const madara::knowledge::KnowledgeUpdateSettings &> ())

    // define readwrite variables within the class
    .def_readwrite ("treat_globals_as_locals",
      	&madara::knowledge::KnowledgeUpdateSettings::treat_globals_as_locals,
        "Treat global variable changes as if they were local variables")
    .def_readwrite ("signal_changes",
      	&madara::knowledge::KnowledgeUpdateSettings::signal_changes,
        "Always signal on mutation (useful for multi-threaded applications)")
    .def_readwrite ("always_overwrite",
      	&madara::knowledge::KnowledgeUpdateSettings::always_overwrite,
        "Always overwrite records, regardless of quality or consistency checks")
    .def_readwrite ("track_local_changes",
      	&madara::knowledge::KnowledgeUpdateSettings::track_local_changes,
        "Track local changes for checkpointing purposes")
    .def_readwrite ("clock_increment",
      	&madara::knowledge::KnowledgeUpdateSettings::clock_increment,
        "Integer increment for updates to Lamport clocks")
  ; // end class KnowledgeUpdateSettings
          
  /********************************************************
    * EvalSettings definitions
    ********************************************************/
 
  class_<madara::knowledge::EvalSettings,
          bases<madara::knowledge::KnowledgeUpdateSettings> > (
            "EvalSettings",
            "Settings for evaluate calls", init <> ())
    
    // other constructors here
    .def (init <
      const madara::knowledge::EvalSettings &> ())
    
    // define readwrite variables within the class
    .def_readwrite ("delay_sending_modifieds",
      	&madara::knowledge::EvalSettings::delay_sending_modifieds,
        "Do not send modifications after an evaluation")
    .def_readwrite ("pre_print_statement",
      	&madara::knowledge::EvalSettings::pre_print_statement,
        "Statement to atomically expand and print before an evaluate")
    .def_readwrite ("post_print_statement",
      	&madara::knowledge::EvalSettings::post_print_statement,
        "Statement to atomically expand and print after an evaluate")
    .def_readwrite ("send_list",
      	&madara::knowledge::EvalSettings::send_list,
        "List of variables that are allowed to be sent, if changed")

  ; // end class EvalSettings
          
  /********************************************************
    * WaitSettings definitions
    ********************************************************/
 
  class_<madara::knowledge::WaitSettings,
          bases<madara::knowledge::EvalSettings> > ("WaitSettings",
          "Settings for wait calls", init <> ())
    
    // other constructors here
    .def (init <
      const madara::knowledge::WaitSettings &> ())
        
    // define readwrite variables within the class
    .def_readwrite ("poll_frequency",
      	&madara::knowledge::WaitSettings::poll_frequency,
        "Frequency, in seconds, to evaluate the expression")
    .def_readwrite ("max_wait_time",
      	&madara::knowledge::WaitSettings::max_wait_time,
        "Maximum amount of time to wait for truth")
  ; // end class WaitSettings

  /********************************************************
    * Variables definitions
    ********************************************************/
    
  class_<madara::knowledge::Variables> ("Variables", init <> ())
      
    // prints all knowledge variables
    .def ("expand_statement",
      &madara::knowledge::Variables::expand_statement,
      "Expand a statement")
        
    // evaluate an expression
    .def( "print",
      static_cast<
        void (madara::knowledge::Variables::*)(
          unsigned int) const
      > (&madara::knowledge::Variables::print),
      "Prints all knowledge in the context") 
        
    // evaluate an expression
    .def( "print",
      static_cast<
        void (madara::knowledge::Variables::*)(
          const std::string &, unsigned int) const
      > (&madara::knowledge::Variables::print),
      "Prints a statement") 

    // evaluate an expression
    .def( "evaluate",
      static_cast<
        madara::knowledge::KnowledgeRecord (madara::knowledge::Variables::*)(
          const std::string &,
          const madara::knowledge::KnowledgeUpdateSettings &)
      > (&madara::knowledge::Variables::evaluate),
        m_eval_1_of_2 (
        args("expression", "settings"),
        "Evaluates an expression"))

    // get a knowledge record
    .def( "get",
      static_cast<
        madara::knowledge::KnowledgeRecord (madara::knowledge::Variables::*)(
          const std::string &,
          const madara::knowledge::KnowledgeReferenceSettings &)
      > (&madara::knowledge::Variables::get),
      m_get_1_of_2 (
        args("key", "settings"),
        "Retrieves a knowledge record"))
          
    // get a knowledge record at an index
    .def( "retrieve_index",
      static_cast<
        madara::knowledge::KnowledgeRecord (madara::knowledge::Variables::*)(
          const std::string &,
            size_t,
            const madara::knowledge::KnowledgeReferenceSettings &)
      > (&madara::knowledge::Variables::retrieve_index),
      m_retrieve_index_2_of_3 (
        args("key", "index", "settings"),
        "Retrieves a knowledge record from an index"))
          
    // sets a knowledge record to a double
    .def( "set",
      static_cast<
        int (madara::knowledge::Variables::*)(
          const std::string &,
          double,
          const madara::knowledge::KnowledgeUpdateSettings &)
      > (&madara::knowledge::Variables::set),
      "sets a knowledge record to a double") 
        
    // sets a knowledge record to an array of doubles
    .def( "set",
      static_cast<
        int (madara::knowledge::Variables::*)(
          const std::string &,
          const std::vector <double> &,
          const madara::knowledge::KnowledgeUpdateSettings &)
      > (&madara::knowledge::Variables::set),
      "sets a knowledge record to an array of doubles") 
        
    // sets a knowledge record to an integer
    .def( "set",
      static_cast<
        int (madara::knowledge::Variables::*)(
          const std::string &,
          madara::knowledge::KnowledgeRecord::Integer,
          const madara::knowledge::KnowledgeUpdateSettings &)
      > (&madara::knowledge::Variables::set),
      "sets a knowledge record to a integer") 
        
    // sets a knowledge record to an array of integer
    .def( "set",
      static_cast<
        int (madara::knowledge::Variables::*)(
          const std::string &,
          const std::vector <madara::knowledge::KnowledgeRecord::Integer> &,
          const madara::knowledge::KnowledgeUpdateSettings &)
      > (&madara::knowledge::Variables::set),
      "sets a knowledge record to an array of integers") 
        
    // sets a knowledge record to a string
    .def( "set",
      static_cast<
        int (madara::knowledge::Variables::*)(
          const std::string &,
          const std::string &,
          const madara::knowledge::KnowledgeUpdateSettings &)
      > (&madara::knowledge::Variables::set),
      "sets a knowledge record to a string") 
  ;
    
  /********************************************************
    * FunctionArguments definitions
    ********************************************************/
 
  class_ <std::vector <madara::knowledge::FunctionArguments> > (
        "FunctionArguments", "List of arguments to a function")
  ;
    
  /********************************************************
    * KnowledgeBase definitions
    ********************************************************/
 
  class_<madara::knowledge::KnowledgeBase> ("KnowledgeBase",
    "Network-enabled, thread-safe knowledge context", init <> ())
      
    // define constructors
    .def (init <const std::string &,
          const madara::transport::TransportSettings &> ())
    
    // define constructors
    .def (init <const madara::knowledge::KnowledgeBase &> ())

    // defines a python function
    .def( "define_function",
      static_cast<
        void (madara::knowledge::KnowledgeBase::*)(
          const std::string &,
          object)
      > (&madara::knowledge::KnowledgeBase::define_function),
      "defines a named function that can be called within evaluates")
        
    // evaluate an expression
    .def( "evaluate",
      static_cast<
        madara::knowledge::KnowledgeRecord (madara::knowledge::KnowledgeBase::*)(
          const std::string &, const madara::knowledge::EvalSettings &)
      > (&madara::knowledge::KnowledgeBase::evaluate),
        m_eval_1_of_2 (
        args("expression", "settings"),
        "Evaluates an expression"))
      
    // expands a statement with variable expansion
    .def ("expand_statement",
      &madara::knowledge::KnowledgeBase::expand_statement,
      "Expand a statement")
            
    // get a knowledge record
    .def( "get",
      static_cast<
        madara::knowledge::KnowledgeRecord (madara::knowledge::KnowledgeBase::*)(
          const std::string &,
          const madara::knowledge::KnowledgeReferenceSettings &)
      > (&madara::knowledge::KnowledgeBase::get),
      m_get_1_of_2 (
        args("key", "settings"),
        "Retrieves a knowledge record"))
        
    // expands and prints a statement
    .def ("load_context",
      static_cast<int64_t (madara::knowledge::KnowledgeBase::*)(
        const std::string &,
        bool,
        const madara::knowledge::KnowledgeUpdateSettings &)
      > (&madara::knowledge::KnowledgeBase::load_context),
      m_load_context_1_of_3 (
        args("filename", "use_id", "settings"),
        "Loads a variable context from a file"))
          
    // locks the knowledge base from updates from other threads
    .def ("lock",
      &madara::knowledge::KnowledgeBase::lock,
      "Locks the knowledge base from updates from other threads")
        
    // evaluate an expression
    .def( "print",
      static_cast<
        void (madara::knowledge::KnowledgeBase::*)(
          unsigned int) const
      > (&madara::knowledge::KnowledgeBase::print),
      m_print_0_of_1 (
        args("level"), "Prints all variables in the knowledge base"))
        
    // evaluate an expression
    .def( "print",
      static_cast<
        void (madara::knowledge::KnowledgeBase::*)(
          const std::string &, unsigned int) const
      > (&madara::knowledge::KnowledgeBase::print),
      m_print_1_of_2 (
        args("statement", "level"), "Expands and prints a statement"))

    // prints all knowledge variables
    .def ("print_knowledge",
      &madara::knowledge::KnowledgeBase::print_knowledge,
      m_print_knowledge_0_of_1 (
        args("level"),
        "Alias of print(level). Prints all variables in the knowledge base"))

    // get a knowledge record at an index
    .def( "retrieve_index",
      static_cast<
        madara::knowledge::KnowledgeRecord (madara::knowledge::KnowledgeBase::*)(
          const std::string &,
            size_t,
            const madara::knowledge::KnowledgeReferenceSettings &)
      > (&madara::knowledge::KnowledgeBase::retrieve_index),
      m_retrieve_index_2_of_3 (
        args("key", "index", "settings"),
        "Retrieves a knowledge record from an index"))
          
    // expands and prints a statement
    .def ("save_context",
      &madara::knowledge::KnowledgeBase::save_context,
        "Saves the context to a file")
          
    // Sends all modified variables through the attached transports
    .def( "send_modifieds",
      static_cast<
        int (madara::knowledge::KnowledgeBase::*)(
          const std::string &,
          const madara::knowledge::EvalSettings &)
      > (&madara::knowledge::KnowledgeBase::send_modifieds),
      m_send_modifieds_0_of_2 (
        args("prefix", "settings"),
        "Sends all modified variables through the attached transports."))

    // sets a knowledge record to a double
    .def( "set",
      static_cast<
        int (madara::knowledge::KnowledgeBase::*)(
          const std::string &,
          double,
          const madara::knowledge::EvalSettings &)
      > (&madara::knowledge::KnowledgeBase::set),
      m_set_2_of_3 (
        args("key", "value", "settings"),
        "Sets a knowledge record to a double"))
        
    // sets a knowledge record to an array of doubles
    .def( "set",
      static_cast<
        int (madara::knowledge::KnowledgeBase::*)(
          const std::string &,
          const std::vector <double> &,
          const madara::knowledge::EvalSettings &)
      > (&madara::knowledge::KnowledgeBase::set),
      m_set_2_of_3 (
        args("key", "value", "settings"),
        "Sets a knowledge record to an array of doubles"))
        
    // sets a knowledge record to an integer
    .def( "set",
      static_cast<
        int (madara::knowledge::KnowledgeBase::*)(
          const std::string &,
          madara::knowledge::KnowledgeRecord::Integer,
          const madara::knowledge::EvalSettings &)
      > (&madara::knowledge::KnowledgeBase::set),
      m_set_2_of_3 (
        args("key", "value", "settings"),
        "Sets a knowledge record to an integer"))
        
    // sets a knowledge record to an array of integer
    .def( "set",
      static_cast<
        int (madara::knowledge::KnowledgeBase::*)(
          const std::string &,
          const std::vector <madara::knowledge::KnowledgeRecord::Integer> &,
          const madara::knowledge::EvalSettings &)
      > (&madara::knowledge::KnowledgeBase::set),
      m_set_2_of_3 (
        args("key", "value", "settings"),
        "Sets a knowledge record to an array of integers"))
        
    // sets a knowledge record to a string
    .def( "set",
      static_cast<
        int (madara::knowledge::KnowledgeBase::*)(
          const std::string &,
          const std::string &,
          const madara::knowledge::EvalSettings &)
      > (&madara::knowledge::KnowledgeBase::set),
      m_set_2_of_3 (
        args("key", "value", "settings"),
        "Sets a knowledge record to a string"))
        
    // sets an array index to an integer
    .def( "set_index",
      static_cast<
        int (madara::knowledge::KnowledgeBase::*)(
          const std::string &,
          size_t,
          madara::knowledge::KnowledgeRecord::Integer,
          const madara::knowledge::EvalSettings &)
      > (&madara::knowledge::KnowledgeBase::set_index),
      m_set_index_2_of_3 (
        args("key", "value", "settings"),
        "Sets an array index to an integer"))
        
    // sets an array index to a double
    .def( "set_index",
      static_cast<
        int (madara::knowledge::KnowledgeBase::*)(
          const std::string &,
          size_t,
          double,
          const madara::knowledge::EvalSettings &)
      > (&madara::knowledge::KnowledgeBase::set_index),
      m_set_index_2_of_3 (
        args("key", "value", "settings"),
        "Sets an array index to a double"))
            
    // unlocks the knowledge base
    .def ("unlock",
      &madara::knowledge::KnowledgeBase::unlock,
      "Unlocks the knowledge base and allows other threads to access")
  
    // wait on an expression
    .def( "wait",
      static_cast<
        madara::knowledge::KnowledgeRecord (madara::knowledge::KnowledgeBase::*)(
          const std::string &, const madara::knowledge::WaitSettings &)
      > (&madara::knowledge::KnowledgeBase::wait),
        m_wait_1_of_2 (
        args("expression", "settings"),
        "Waits for an expression to evaluate to true"))

  ;

}

BOOST_PYTHON_MODULE (madara)
{
  // Launch the appropriate thread management initialization
  PyEval_InitThreads();

  // Declare classes inside Madara namespace (top namespace of Python module)
  
  class_ <std::vector <std::string> > ("StringVector")
    .def(vector_indexing_suite<std::vector<std::string> >());

  class_ <std::vector <madara::knowledge::KnowledgeRecord> > ("KnowledgeRecordVector")
    .def(vector_indexing_suite<std::vector<madara::knowledge::KnowledgeRecord> >());
  
  class_ <std::map <std::string, madara::knowledge::KnowledgeRecord> > (
    "KnowledgeRecordMap")
    .def(map_indexing_suite<std::map <
      std::string, madara::knowledge::KnowledgeRecord> >());

  define_knowledge_record ();
  define_filters ();
  define_transport ();
  define_knowledge_engine ();
}
