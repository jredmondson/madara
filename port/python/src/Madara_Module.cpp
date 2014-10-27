#include "madara/knowledge_engine/Knowledge_Base.h"
#include "madara/filters/Generic_Filters.h"
#include "Function_Defaults.h"

#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <boost/python/suite/indexing/map_indexing_suite.hpp>

/**
 * @file Madara_Module.cpp
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains Boost.Python mappings for the C++ MADARA library
 * to a Python module called Madara.
 **/

using namespace boost::python;

class Filters_NS {};
class Knowledge_Engine_NS {};
class Transport_NS {};

void define_knowledge_record (void)
{
  scope kr_class (
    class_<Madara::Knowledge_Record> ("KnowledgeRecord", init <> ())
  
  // integer constructor
  .def (init <int64_t> ())

  .def (init <double> ())

  .def (init <std::vector <int64_t> > ())
    
  .def (init <std::vector <double> > ())

  .def (init <std::string> ())
    
  .def (init <const Madara::Knowledge_Record &> ())

  // clears the value to a 0 integer
  .def ("clear_value", &Madara::Knowledge_Record::clear_value,
      "Sets the value to 0 and type to integer")
    
  // sets the contents of the record to a jpeg
  .def ("read_file", &Madara::Knowledge_Record::read_file,
      "Reads the contents of a file into the record")
    
  // gets the double precision
  .def ("get_precision", &Madara::Knowledge_Record::get_precision,
      "Gets the double precision used in to_string")
    
  // decrements an index of an array
  .def ("dec_index", &Madara::Knowledge_Record::inc_index,
      "Decrements an array element at a particular index")
    
  // increments an index of an array
  .def ("inc_index", &Madara::Knowledge_Record::inc_index,
      "Increments an array element at a particular index")
    
  // retrieves an index of an array
  .def ("retrieve_index", &Madara::Knowledge_Record::retrieve_index,
      "Retrieves an array element at a particular index")
    
  // sets the double precision
  .def ("set_precision", &Madara::Knowledge_Record::set_precision,
      "Sets the double precision, generally for to_string")
    
  // reset the record to UNCREATED
  .def ("reset_value", &Madara::Knowledge_Record::reset_value,
      "Resets the record to an UNCREATED status (faster than clear_value)")
    
  // sets the contents of the record to a jpeg
  .def ("set_jpeg", &Madara::Knowledge_Record::set_jpeg,
      "Sets the value to a jpeg")
    
  // sets the contents of the record to a file
  .def ("set_file", &Madara::Knowledge_Record::set_file,
      "Sets the value to a file's contents")
    
  // sets a knowledge record to a double
  .def( "set",
    static_cast<
      void (Madara::Knowledge_Record::*)(
        const double &
      )
    > (&Madara::Knowledge_Record::set_value),
      "Sets the value to a double") 
        
  // sets a knowledge record to an array of doubles
  .def( "set",
    static_cast<
      void (Madara::Knowledge_Record::*)(
        const std::vector <double> &
      )
    > (&Madara::Knowledge_Record::set_value),
      "Sets the value to an array of doubles") 
        
  // sets a knowledge record to an integer
  .def( "set",
    static_cast<
      void (Madara::Knowledge_Record::*)(
        const Madara::Knowledge_Record::Integer &
      )
    > (&Madara::Knowledge_Record::set_value),
      "Sets the value to an integer") 
        
  // sets a knowledge record to an array of integer
  .def( "set",
    static_cast<
      void (Madara::Knowledge_Record::*)(
        const std::vector <Madara::Knowledge_Record::Integer> &
      )
    > (&Madara::Knowledge_Record::set_value),
      "Sets the value to an array of integers") 
        
  // sets a knowledge record to a string
  .def( "set",
    static_cast<
      void (Madara::Knowledge_Record::*)(
        const std::string &
      )
    > (&Madara::Knowledge_Record::set_value),
      "Sets the value to a string") 
      
  // sets an array index to an integer
  .def( "set_index",
    static_cast<
      void (Madara::Knowledge_Record::*)(
        size_t,
        Madara::Knowledge_Record::Integer)
    > (&Madara::Knowledge_Record::set_index),
      "Sets an array index to an integer") 
        
  // sets an array index to a double
  .def( "set_index",
    static_cast<
      void (Madara::Knowledge_Record::*)(
        size_t,
        double)
    > (&Madara::Knowledge_Record::set_index),
      "Sets an array index to a double (and converts the array to double)") 
        
  // sets the contents of the record to a jpeg
  .def ("size", &Madara::Knowledge_Record::size,
      "Returns the size of the value")
    
  // convert to a string
  .def ("status", &Madara::Knowledge_Record::status,
      "Returns the status of the record")

  // convert to a string
  .def ("to_string", &Madara::Knowledge_Record::to_string,
      m_to_string_0_of_1 (
        args("delimiter"),
        "Converts the record to a string"))

  // convert to an integer
  .def ("to_integer", &Madara::Knowledge_Record::to_integer,
      "Converts the record to an integer")
    
  // convert to a std::vector of integers
  .def ("to_integers", &Madara::Knowledge_Record::to_integers,
      "Converts the record to an array of integers")

  // convert to a double
  .def ("to_double", &Madara::Knowledge_Record::to_double,
      "Converts the record to a double")
    
  // convert to a std::vector of doubles
  .def ("to_doubles", &Madara::Knowledge_Record::to_doubles,
      "Converts the record to an array of doubles")

  // save value to a file
  .def ("to_file", &Madara::Knowledge_Record::to_file,
      "Saves the value of the record to a file")
    
  // gets the type of the record
  .def ("type", &Madara::Knowledge_Record::type,
      "Returns the value type")
    
  // fragments the record
  .def ("fragment", &Madara::Knowledge_Record::fragment,
      "Fragments the record into components")
    
  // checks if the record is false
  .def ("is_false", &Madara::Knowledge_Record::is_false,
      "Checks if the record is false")
    
  // checks if the record is true
  .def ("is_true", &Madara::Knowledge_Record::is_true,
      "Checks if the record is true")
    
  // checks if record is a binary file type
  .def( "is_binary_file_type",
    static_cast<bool (Madara::Knowledge_Record::*)(void) const> (
      &Madara::Knowledge_Record::is_binary_file_type),
      "Checks if the record is a binary file type") 
        
  // checks if record is a double type
  .def( "is_double_type",
    static_cast<bool (Madara::Knowledge_Record::*)(void) const> (
      &Madara::Knowledge_Record::is_double_type),
      "Checks if the record is a double type") 
        
  // checks if record is a file type
  .def ("is_file_type",
    static_cast<bool (Madara::Knowledge_Record::*)(void) const> (
      &Madara::Knowledge_Record::is_file_type),
      "Checks if the record is a file type") 
        
  // checks if the record is an image type
  .def ("is_image_type",
    static_cast<bool (Madara::Knowledge_Record::*)(void) const> (
      &Madara::Knowledge_Record::is_image_type),
      "Checks if the record is an image type")
      
  // checks if the record is an integer type
  .def ("is_integer_type",
    static_cast<bool (Madara::Knowledge_Record::*)(void) const> (
      &Madara::Knowledge_Record::is_integer_type),
      "Checks if the record is an integer type") 

  // checks if the record is a string type
  .def ("is_string_type",
    static_cast<bool (Madara::Knowledge_Record::*)(void) const> (
      &Madara::Knowledge_Record::is_string_type),
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

  .def ("operator=", &Madara::Knowledge_Record::operator=,
      "Assigns the value of one record to another")
        
  .def( "operator==",
    static_cast<Madara::Knowledge_Record::Integer (
      Madara::Knowledge_Record::*)(
        const Madara::Knowledge_Record &) const> (
          &Madara::Knowledge_Record::operator==),
        "Compares two records for equality") 

  .def( "operator==",
    static_cast<bool (Madara::Knowledge_Record::*)(
      Madara::Knowledge_Record::Integer) const> (
        &Madara::Knowledge_Record::operator==),
      "Compares two records for equality") 

  .def( "operator&&",
    static_cast<Madara::Knowledge_Record::Integer (
      Madara::Knowledge_Record::*)(
        const Madara::Knowledge_Record &) const> (
          &Madara::Knowledge_Record::operator&&),
      "Logically and two records together") 

  .def( "operator||",
    static_cast<Madara::Knowledge_Record::Integer (
      Madara::Knowledge_Record::*)(
        const Madara::Knowledge_Record &) const> (
          &Madara::Knowledge_Record::operator||),
      "Logically ors two records together") 

  .def( "operator++",
    static_cast<Madara::Knowledge_Record (Madara::Knowledge_Record::*)(
      void)> (
        &Madara::Knowledge_Record::operator++),
      "Adds one to the record") 

  .def( "operator--",
    static_cast<Madara::Knowledge_Record (Madara::Knowledge_Record::*)(
      void)> (
        &Madara::Knowledge_Record::operator--),
      "Subtracts one from the record") 
  );

  // Boost.python does not appear to support this type of function
  //.def ("to_unmanaged_buffer", &Madara::Knowledge_Record::to_unmanaged_buffer)
  
  // the types of packet drop policies for QoS_Transport_Settings class
  enum_<Madara::Knowledge_Record::Value_Types>("ValueTypes")
    .value("UNINITIALIZED",
      Madara::Knowledge_Record::UNINITIALIZED)
    .value("INTEGER",
      Madara::Knowledge_Record::INTEGER)
    .value("STRING",
      Madara::Knowledge_Record::STRING)
    .value("DOUBLE",
      Madara::Knowledge_Record::DOUBLE)
    .value("FLOAT",
      Madara::Knowledge_Record::FLOAT)
    .value("UNKNOWN_FILE_TYPE",
      Madara::Knowledge_Record::UNKNOWN_FILE_TYPE)
    .value("XML",
      Madara::Knowledge_Record::XML)
    .value("TEXT_FILE",
      Madara::Knowledge_Record::TEXT_FILE)
    .value("DOUBLE_ARRAY",
      Madara::Knowledge_Record::DOUBLE_ARRAY)
    .value("INTEGER_ARRAY",
      Madara::Knowledge_Record::INTEGER_ARRAY)
    .value("IMAGE_JPEG",
      Madara::Knowledge_Record::IMAGE_JPEG)
    .value("ALL_ARRAYS",
      Madara::Knowledge_Record::ALL_ARRAYS)
    .value("ALL_INTEGERS",
      Madara::Knowledge_Record::ALL_INTEGERS)
    .value("ALL_DOUBLES",
      Madara::Knowledge_Record::ALL_DOUBLES)
    .value("ALL_PRIMITIVE_TYPES",
      Madara::Knowledge_Record::ALL_PRIMITIVE_TYPES)
    .value("ALL_FILE_TYPES",
      Madara::Knowledge_Record::ALL_FILE_TYPES)
    .value("ALL_IMAGES",
      Madara::Knowledge_Record::ALL_IMAGES)
    .value("ALL_TEXT_FORMATS",
      Madara::Knowledge_Record::ALL_TEXT_FORMATS)
    .value("ALL_TYPES",
      Madara::Knowledge_Record::ALL_TYPES)
  ;
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
      &Madara::Filters::discard,
      "Drops the current record from the filtering process")
        
    // update the drop rate
    .def ("discard_nonprimitives",
      &Madara::Filters::discard_nonprimitives,
      "Drops nonprimitive records from the filtering process")
        
    // update the drop rate
    .def ("discard_nonfiles",
      &Madara::Filters::discard_nonfiles,
      "Drops nonfile records from the filtering process")
        
    // update the drop rate
    .def ("log_args",
      &Madara::Filters::log_args,
      "Logs all arguments passed via the filtering process")
        
    // update the drop rate
    .def ("log_aggregate",
      &Madara::Filters::log_aggregate,
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

  // the types of packet drop policies for QoS_Transport_Settings class
  enum_<Madara::Transport::PacketDropType>("DropTypes")
    .value("PACKET_DROP_DETERMINISTIC",
      Madara::Transport::PACKET_DROP_DETERMINISTIC)
    .value("PACKET_DROP_PROBABLISTIC",
      Madara::Transport::PACKET_DROP_PROBABLISTIC)
  ;
    
  // the types of packet drop policies for QoS_Transport_Settings class
  enum_<Madara::Transport::Types>("TransportTypes")
    .value("NO_TRANSPORT",
      Madara::Transport::NO_TRANSPORT)
    .value("SPLICE",
      Madara::Transport::SPLICE)
    .value("NDDS",
      Madara::Transport::NDDS)
    .value("TCP",
      Madara::Transport::TCP)
    .value("UDP",
      Madara::Transport::UDP)
    .value("MULTICAST",
      Madara::Transport::MULTICAST)
    .value("BROADCAST",
      Madara::Transport::BROADCAST)
  ;
    
  {
    /********************************************************
      * Transport Context definitions
      ********************************************************/
    
    scope Transport_Context = 
      class_<Madara::Transport::Transport_Context> ("TransportContext",
      "Container for transport-related state information", init <> ())

      // copy constructor
      .def (init<const Madara::Transport::Transport_Context> ())

      // clear records
      .def ("clear_records",
        &Madara::Transport::Transport_Context::clear_records,
        "Clears any uncommitted records associated with the context")
        
      // Retrieves the operation type
      .def ("get_operation",
        &Madara::Transport::Transport_Context::get_operation,
        "Retrieves the operation type")
        
      // Retrieves receive bandwidth usage
      .def ("get_receive_bandwidth",
        &Madara::Transport::Transport_Context::get_receive_bandwidth,
        "Returns received bandwidth usage in B/s over past 10s")
        
      // Retrieves send bandwidth usage
      .def ("get_send_bandwidth",
        &Madara::Transport::Transport_Context::get_send_bandwidth,
        "Returns send bandwidth usage in B/s over past 10s")
        
      // Retrieves time that message was generated
      .def ("get_message_time",
        &Madara::Transport::Transport_Context::get_message_time,
        "Retrieves time that message was generated")
        
      // Retrieves time that message is being processed
      .def ("get_current_time",
        &Madara::Transport::Transport_Context::get_current_time,
        "Retrieves time that message is being processed")
          
      // Adds a record to the context for rebroadcasting or applying
      .def ("add_record",
        &Madara::Transport::Transport_Context::add_record,
        "Adds a record to the context for rebroadcasting or applying")
          
      // Retrieves the knowledge domain (similar to topics in pub/sub)
      .def ("get_domain",
        &Madara::Transport::Transport_Context::get_domain, 
        return_value_policy<reference_existing_object>(),
        "Retrieves the knowledge domain (similar to topics in pub/sub)")
          
      // Retrieves the message originator
      .def ("get_originator",
        &Madara::Transport::Transport_Context::get_originator, 
        return_value_policy<reference_existing_object>(),
        "Retrieves the message originator")
          
      // Retrieves the current uncommitted record list"
      .def ("get_records",
        &Madara::Transport::Transport_Context::get_records, 
        return_value_policy<reference_existing_object>(),
        "Retrieves the current uncommitted record list")

    ;

    // the types of packet drop policies for QoS_Transport_Settings class
    enum_<Madara::Transport::Transport_Context::Operations> ("Operations")
      .value("IDLE_OPERATION",
        Madara::Transport::Transport_Context::IDLE_OPERATION)
      .value("SENDING_OPERATION",
        Madara::Transport::Transport_Context::SENDING_OPERATION)
      .value("RECEIVING_OPERATION",
        Madara::Transport::Transport_Context::RECEIVING_OPERATION)
      .value("REBROADCASTING_OPERATION",
        Madara::Transport::Transport_Context::REBROADCASTING_OPERATION)
      ;
  }
      
  /********************************************************
    * Transport Settings definitions
    ********************************************************/

  class_<Madara::Transport::Settings> ("Settings",
    "The main transport settings class", init <> ())
      
    .def (init<const Madara::Transport::Settings &> ())

    // define readwrite variables within the class
    .def_readwrite ("domains",
      &Madara::Transport::Settings::domains,
      "Informs the transport of the requested knowledge domain")

    .def_readwrite ("queue_length",
      &Madara::Transport::Settings::queue_length,
      "Informs the transport of the requested queue_length in bytes")

    .def_readwrite ("type",
      &Madara::Transport::Settings::type,
      "Indicates the type of transport (see Transport_Types)")

    .def_readwrite ("reliability",
      &Madara::Transport::Settings::reliability,
      "Informs the transport of the requested reliability (0==BEST_EFFORT)")

    .def_readwrite ("id",
      &Madara::Transport::Settings::id,
      "Informs the transport of the process id")

    .def_readwrite ("processes",
      &Madara::Transport::Settings::processes,
      "Informs the transport of the number of expected processes")

    .def_readwrite ("on_data_received_logic",
      &Madara::Transport::Settings::on_data_received_logic,
      "Provides a KaRL expression to evaluate when data is received")

    .def_readwrite ("delay_launch",
      &Madara::Transport::Settings::delay_launch,
      "Indicates that the transport should delay launch until activated")

    .def_readwrite ("never_exit",
      &Madara::Transport::Settings::never_exit,
      "Indicates that the transport should never exit, even in bad states")

    .def_readwrite ("send_reduced_message_header",
      &Madara::Transport::Settings::send_reduced_message_header,
      "Indicates that a reduced message header should be used for messages")

    .def_readwrite ("hosts",
      &Madara::Transport::Settings::hosts,
      "List of hosts for the transport layer")
  ;
    
  /********************************************************
    * QoS_Transport_Settings definitions
    ********************************************************/

  class_<Madara::Transport::QoS_Transport_Settings,
          bases<Madara::Transport::Settings> > (
            "QoS_Transport_Settings",
            "Transport settings with quality-of-service focus", init<> ())
      
    .def (init <const Madara::Transport::QoS_Transport_Settings &> ())
      
    .def (init <const Madara::Transport::Settings &> ())
      
    // adds a python callback to the typed send filter chains
    .def( "add_rebroadcast_filter",
      static_cast<
        void (Madara::Transport::QoS_Transport_Settings::*)(
              uint32_t types, boost::python::object & object)
      > (&Madara::Transport::QoS_Transport_Settings::add_rebroadcast_filter),
      "Adds a python callback to the typed rebroadcst filter chains")
        
    // adds a python callback to the aggregate send filter chains
    .def( "add_rebroadcast_filter",
      static_cast<
        void (Madara::Transport::QoS_Transport_Settings::*)(
              boost::python::object & object)
      > (&Madara::Transport::QoS_Transport_Settings::add_rebroadcast_filter),
      "Adds a python callback to the aggregate rebroadcast filter chains")
        
    // adds a python callback to the typed receive filter chains
    .def( "add_receive_filter",
      static_cast<
        void (Madara::Transport::QoS_Transport_Settings::*)(
              uint32_t types, boost::python::object & object)
      > (&Madara::Transport::QoS_Transport_Settings::add_receive_filter),
      "Adds a python callback to the typed receive filter chains")
        
    // adds a python callback to the aggregate receive filter chains
    .def( "add_receive_filter",
      static_cast<
        void (Madara::Transport::QoS_Transport_Settings::*)(
              boost::python::object & object)
      > (&Madara::Transport::QoS_Transport_Settings::add_receive_filter),
      "Adds a python callback to the aggregate receive filter chains")
        
    // adds a python callback to the typed send filter chains
    .def( "add_send_filter",
      static_cast<
        void (Madara::Transport::QoS_Transport_Settings::*)(
              uint32_t types, boost::python::object & object)
      > (&Madara::Transport::QoS_Transport_Settings::add_send_filter),
      "Adds a python callback to the typed send filter chains")
        
    // adds a python callback to the aggregate send filter chains
    .def( "add_send_filter",
      static_cast<
        void (Madara::Transport::QoS_Transport_Settings::*)(
              boost::python::object & object)
      > (&Madara::Transport::QoS_Transport_Settings::add_send_filter),
      "Adds a python callback to the aggregate send filter chains")
        
        
    // Clears the rebroadcast filters for a specified type
    .def ("clear_rebroadcast_filters",
      &Madara::Transport::QoS_Transport_Settings::clear_rebroadcast_filters,
      "Clears the rebroadcast filters for a specified type")
        
    // Clears the receive filters for a specified type
    .def ("clear_receive_filters",
      &Madara::Transport::QoS_Transport_Settings::clear_receive_filters,
      "Clears the receive filters for a specified type")
        
    // Clears the send filters for a specified type
    .def ("clear_send_filters",
      &Madara::Transport::QoS_Transport_Settings::clear_send_filters,
      "Clears the send filters for a specified type")
        
    // Clears the rebroadcast filters for a specified type
    .def ("clear_rebroadcast_aggregate_filters",
      &Madara::Transport::QoS_Transport_Settings::clear_rebroadcast_aggregate_filters,
      "Clears the rebroadcast aggregate filters")
        
    // Clears the receive filters for a specified type
    .def ("clear_receive_aggregate_filters",
      &Madara::Transport::QoS_Transport_Settings::clear_receive_aggregate_filters,
      "Clears the receive aggregate filters")
        
    // Clears the send filters for a specified type
    .def ("clear_send_aggregate_filters",
      &Madara::Transport::QoS_Transport_Settings::clear_send_aggregate_filters,
      "Clears the send aggregate filters")
        

    // get the drop burst rate
    .def ("get_drop_burst",
      &Madara::Transport::QoS_Transport_Settings::get_drop_burst,
      "Retrieves the drop bursts")
        
    // update the drop rate
    .def ("get_drop_rate",
      &Madara::Transport::QoS_Transport_Settings::get_drop_rate,
      "Retrieves the rate of drops (0.8 is 80%)")
        
    // get the drop type
    .def ("get_drop_type",
      &Madara::Transport::QoS_Transport_Settings::get_drop_type,
      "Retrieves the type of drops")
        
    // Prints the number of rebroadcast filters for a specified type
    .def ("print_num_filters_rebroadcast",
      &Madara::Transport::QoS_Transport_Settings::print_num_filters_rebroadcast,
      "Prints the number of rebroadcast filters for a specified type")
        
    // Prints the number of receive filters for a specified type
    .def ("print_num_filters_receive",
      &Madara::Transport::QoS_Transport_Settings::print_num_filters_receive,
      "Prints the number of receive filters for a specified type")
        
    // Prints the number of send filters for a specified type
    .def ("print_num_filters_send",
      &Madara::Transport::QoS_Transport_Settings::print_num_filters_send,
      "Prints the number of send filters for a specified type")
        
    // Gets the number of rebroadcast aggregate filters
    .def ("get_number_of_rebroadcast_aggregate_filters",
      &Madara::Transport::QoS_Transport_Settings::get_number_of_rebroadcast_aggregate_filters,
      "Gets the number of receive aggregate filters")
        
    // Gets the number of receive filters for a specified type
    .def ("get_number_of_receive_aggregate_filters",
      &Madara::Transport::QoS_Transport_Settings::get_number_of_receive_aggregate_filters,
      "Gets the number of receive aggregate filters")
        
    // Gets the number of send aggregate filters
    .def ("get_number_of_send_aggregate_filters",
      &Madara::Transport::QoS_Transport_Settings::get_number_of_send_aggregate_filters,
      "Gets the number of send aggregate filters")
        
    // Gets the number of rebroadcast filters for a specified type
    .def ("get_number_of_rebroadcast_filtered_types",
      &Madara::Transport::QoS_Transport_Settings::get_number_of_rebroadcast_filtered_types,
      "Gets the number of rebroadcast filters for a specified type")
        
    // Gets the number of receive filters for a specified type
    .def ("get_number_of_receive_filtered_types",
      &Madara::Transport::QoS_Transport_Settings::get_number_of_receive_filtered_types,
      "Gets the number of receive filters for a specified type")
        
    // Gets the number of send filters for a specified type
    .def ("get_number_of_send_filtered_types",
      &Madara::Transport::QoS_Transport_Settings::get_number_of_send_filtered_types,
      "Gets the number of send filters for a specified type")

    // update the drop rate
    .def ("update_drop_rate",
      &Madara::Transport::QoS_Transport_Settings::update_drop_rate,
      "Informs the transport of the drop rate, type, and bursts")
        
    // the rebroadcast time-to-live for the transport
    .add_property("participant_ttl",
      &Madara::Transport::QoS_Transport_Settings::get_participant_ttl,
      &Madara::Transport::QoS_Transport_Settings::enable_participant_ttl,
      "Informs the transport that it should participate in rebroadcasting")

    // the rebroadcast time-to-live for the transport
    .add_property("rebroadcast_ttl",
      &Madara::Transport::QoS_Transport_Settings::get_rebroadcast_ttl,
      &Madara::Transport::QoS_Transport_Settings::set_rebroadcast_ttl,
      "Informs the transport of the time-to-live attached to all updates")

    // the send bandwidth for the transport
    .add_property("send_bandwidth",
    &Madara::Transport::QoS_Transport_Settings::get_send_bandwidth_limit,
      &Madara::Transport::QoS_Transport_Settings::set_send_bandwidth_limit,
      "Informs the transport of the send bandwidth limit in B/s that"
      " throttles the sending of new updates")

    // the total bandwidth for the transport
    .add_property("total_bandwidth",
    &Madara::Transport::QoS_Transport_Settings::get_total_bandwidth_limit,
      &Madara::Transport::QoS_Transport_Settings::set_total_bandwidth_limit,
      "Informs the transport of the total bandwidth limit in B/s that"
      " throttles the sending of new updates")

    // the deadline for messages
    .add_property("deadline",
      &Madara::Transport::QoS_Transport_Settings::get_deadline,
      &Madara::Transport::QoS_Transport_Settings::set_deadline,
      "Informs the transport of the deadline, in seconds, for useful info")
  ;
}
  
void define_knowledge_engine (void)
{
  object ke = object (handle<> (
    PyModule_New ("madara.knowledge_engine")));

   ke.attr("__file__")="<synthetic>";
   scope().attr ("knowledge_engine") = ke;
   ke.attr ("__doc__") = "Provides access to the knowledge engine";

   // this was the missing piece: sys.modules['modA.modB']=modB
   extract <dict> (getattr (
     import ("sys"), "modules"))()["madara.knowledge_engine"] = ke;

   scope Knowledge_Engine = ke;

/*
   scope Knowledge_Engine
    = class_ <Knowledge_Engine_NS, boost::noncopyable> (
      "knowledge_engine", no_init);
   */
  /********************************************************
    * Knowledge_Reference_Settings definitions
    ********************************************************/
 
  class_<Madara::Knowledge_Engine::Knowledge_Reference_Settings> (
      "KnowledgeReferenceSettings",
      "Settings for referencing knowledge records", init<> ())
        
    .def (init <bool> ())
    .def (init <
      const Madara::Knowledge_Engine::Knowledge_Reference_Settings &> ())

    // define readwrite variables within the class
    .def_readwrite ("expand_variables",
      &Madara::Knowledge_Engine::Knowledge_Reference_Settings::expand_variables,
      "Always attempt to expand variable names")
    .def_readwrite ("never_exit",
      &Madara::Knowledge_Engine::Knowledge_Reference_Settings::never_exit,
      "Do not exit, even if in invalid states")

  ; // end class Knowledge_Reference_Settings
          
  /********************************************************
    * Knowledge_Update_Settings definitions
    ********************************************************/
 
  class_<Madara::Knowledge_Engine::Knowledge_Update_Settings,
          bases<Madara::Knowledge_Engine::Knowledge_Reference_Settings> > (
            "KnowledgeUpdateSettings",
            "Settings for updating knowledge records", init <> ())
    
    // other constructors here
    .def (init <
      const Madara::Knowledge_Engine::Knowledge_Update_Settings &> ())

    // define readwrite variables within the class
    .def_readwrite ("treat_globals_as_locals",
      	&Madara::Knowledge_Engine::Knowledge_Update_Settings::treat_globals_as_locals,
        "Treat global variable changes as if they were local variables")
    .def_readwrite ("signal_changes",
      	&Madara::Knowledge_Engine::Knowledge_Update_Settings::signal_changes,
        "Always signal on mutation (useful for multi-threaded applications)")
    .def_readwrite ("always_overwrite",
      	&Madara::Knowledge_Engine::Knowledge_Update_Settings::always_overwrite,
        "Always overwrite records, regardless of quality or consistency checks")
    .def_readwrite ("track_local_changes",
      	&Madara::Knowledge_Engine::Knowledge_Update_Settings::track_local_changes,
        "Track local changes for checkpointing purposes")
    .def_readwrite ("clock_increment",
      	&Madara::Knowledge_Engine::Knowledge_Update_Settings::clock_increment,
        "Integer increment for updates to Lamport clocks")
  ; // end class Knowledge_Update_Settings
          
  /********************************************************
    * Eval_Settings definitions
    ********************************************************/
 
  class_<Madara::Knowledge_Engine::Eval_Settings,
          bases<Madara::Knowledge_Engine::Knowledge_Update_Settings> > (
            "EvalSettings",
            "Settings for evaluate calls", init <> ())
    
    // other constructors here
    .def (init <
      const Madara::Knowledge_Engine::Eval_Settings &> ())
    
    // define readwrite variables within the class
    .def_readwrite ("delay_sending_modifieds",
      	&Madara::Knowledge_Engine::Eval_Settings::delay_sending_modifieds,
        "Do not send modifications after an evaluation")
    .def_readwrite ("pre_print_statement",
      	&Madara::Knowledge_Engine::Eval_Settings::pre_print_statement,
        "Statement to atomically expand and print before an evaluate")
    .def_readwrite ("post_print_statement",
      	&Madara::Knowledge_Engine::Eval_Settings::post_print_statement,
        "Statement to atomically expand and print after an evaluate")
    .def_readwrite ("send_list",
      	&Madara::Knowledge_Engine::Eval_Settings::send_list,
        "List of variables that are allowed to be sent, if changed")

  ; // end class Eval_Settings
          
  /********************************************************
    * Wait_Settings definitions
    ********************************************************/
 
  class_<Madara::Knowledge_Engine::Wait_Settings,
          bases<Madara::Knowledge_Engine::Eval_Settings> > ("WaitSettings",
          "Settings for wait calls", init <> ())
    
    // other constructors here
    .def (init <
      const Madara::Knowledge_Engine::Wait_Settings &> ())
        
    // define readwrite variables within the class
    .def_readwrite ("poll_frequency",
      	&Madara::Knowledge_Engine::Wait_Settings::poll_frequency,
        "Frequency, in seconds, to evaluate the expression")
    .def_readwrite ("max_wait_time",
      	&Madara::Knowledge_Engine::Wait_Settings::max_wait_time,
        "Maximum amount of time to wait for truth")
  ; // end class Wait_Settings

  /********************************************************
    * Variables definitions
    ********************************************************/
    
  class_<Madara::Knowledge_Engine::Variables> ("Variables", init <> ())
      
    // prints all knowledge variables
    .def ("expand_statement",
      &Madara::Knowledge_Engine::Variables::expand_statement,
      "Expand a statement")
        
    // evaluate an expression
    .def( "print",
      static_cast<
        void (Madara::Knowledge_Engine::Variables::*)(
          unsigned int) const
      > (&Madara::Knowledge_Engine::Variables::print),
      "Prints all knowledge in the context") 
        
    // evaluate an expression
    .def( "print",
      static_cast<
        void (Madara::Knowledge_Engine::Variables::*)(
          const std::string &, unsigned int) const
      > (&Madara::Knowledge_Engine::Variables::print),
      "Prints a statement") 

    // evaluate an expression
    .def( "evaluate",
      static_cast<
        Madara::Knowledge_Record (Madara::Knowledge_Engine::Variables::*)(
          const std::string &,
          const Madara::Knowledge_Engine::Knowledge_Update_Settings &)
      > (&Madara::Knowledge_Engine::Variables::evaluate),
        m_eval_1_of_2 (
        args("expression", "settings"),
        "Evaluates an expression"))

    // get a knowledge record
    .def( "get",
      static_cast<
        Madara::Knowledge_Record (Madara::Knowledge_Engine::Variables::*)(
          const std::string &,
          const Madara::Knowledge_Engine::Knowledge_Reference_Settings &)
      > (&Madara::Knowledge_Engine::Variables::get),
      m_get_1_of_2 (
        args("key", "settings"),
        "Retrieves a knowledge record"))
          
    // get a knowledge record at an index
    .def( "retrieve_index",
      static_cast<
        Madara::Knowledge_Record (Madara::Knowledge_Engine::Variables::*)(
          const std::string &,
            size_t,
            const Madara::Knowledge_Engine::Knowledge_Reference_Settings &)
      > (&Madara::Knowledge_Engine::Variables::retrieve_index),
      m_retrieve_index_2_of_3 (
        args("key", "index", "settings"),
        "Retrieves a knowledge record from an index"))
          
    // sets a knowledge record to a double
    .def( "set",
      static_cast<
        int (Madara::Knowledge_Engine::Variables::*)(
          const std::string &,
          double,
          const Madara::Knowledge_Engine::Knowledge_Update_Settings &)
      > (&Madara::Knowledge_Engine::Variables::set),
      "sets a knowledge record to a double") 
        
    // sets a knowledge record to an array of doubles
    .def( "set",
      static_cast<
        int (Madara::Knowledge_Engine::Variables::*)(
          const std::string &,
          const std::vector <double> &,
          const Madara::Knowledge_Engine::Knowledge_Update_Settings &)
      > (&Madara::Knowledge_Engine::Variables::set),
      "sets a knowledge record to an array of doubles") 
        
    // sets a knowledge record to an integer
    .def( "set",
      static_cast<
        int (Madara::Knowledge_Engine::Variables::*)(
          const std::string &,
          Madara::Knowledge_Record::Integer,
          const Madara::Knowledge_Engine::Knowledge_Update_Settings &)
      > (&Madara::Knowledge_Engine::Variables::set),
      "sets a knowledge record to a integer") 
        
    // sets a knowledge record to an array of integer
    .def( "set",
      static_cast<
        int (Madara::Knowledge_Engine::Variables::*)(
          const std::string &,
          const std::vector <Madara::Knowledge_Record::Integer> &,
          const Madara::Knowledge_Engine::Knowledge_Update_Settings &)
      > (&Madara::Knowledge_Engine::Variables::set),
      "sets a knowledge record to an array of integers") 
        
    // sets a knowledge record to a string
    .def( "set",
      static_cast<
        int (Madara::Knowledge_Engine::Variables::*)(
          const std::string &,
          const std::string &,
          const Madara::Knowledge_Engine::Knowledge_Update_Settings &)
      > (&Madara::Knowledge_Engine::Variables::set),
      "sets a knowledge record to a string") 
  ;
    
  /********************************************************
    * Function_Arguments definitions
    ********************************************************/
 
  class_ <std::vector <Madara::Knowledge_Engine::Function_Arguments> > (
        "Function_Arguments", "List of arguments to a function")
  ;
    
  /********************************************************
    * Knowledge_Base definitions
    ********************************************************/
 
  class_<Madara::Knowledge_Engine::Knowledge_Base> ("KnowledgeBase",
    "Network-enabled, thread-safe knowledge context", init <> ())
      
    // define constructors
    .def (init <const std::string &,
          const Madara::Transport::Settings &> ())
    
    // define constructors
    .def (init <const Madara::Knowledge_Engine::Knowledge_Base &> ())

    // defines a python function
    .def( "define_function",
      static_cast<
        void (Madara::Knowledge_Engine::Knowledge_Base::*)(
          const std::string &,
          object)
      > (&Madara::Knowledge_Engine::Knowledge_Base::define_function),
      "defines a named function that can be called within evaluates")
        
    // evaluate an expression
    .def( "evaluate",
      static_cast<
        Madara::Knowledge_Record (Madara::Knowledge_Engine::Knowledge_Base::*)(
          const std::string &, const Madara::Knowledge_Engine::Eval_Settings &)
      > (&Madara::Knowledge_Engine::Knowledge_Base::evaluate),
        m_eval_1_of_2 (
        args("expression", "settings"),
        "Evaluates an expression"))
      
    // expands a statement with variable expansion
    .def ("expand_statement",
      &Madara::Knowledge_Engine::Knowledge_Base::expand_statement,
      "Expand a statement")
            
    // get a knowledge record
    .def( "get",
      static_cast<
        Madara::Knowledge_Record (Madara::Knowledge_Engine::Knowledge_Base::*)(
          const std::string &,
          const Madara::Knowledge_Engine::Knowledge_Reference_Settings &)
      > (&Madara::Knowledge_Engine::Knowledge_Base::get),
      m_get_1_of_2 (
        args("key", "settings"),
        "Retrieves a knowledge record"))
        
    // expands and prints a statement
    .def ("load_context",
      &Madara::Knowledge_Engine::Knowledge_Base::load_context,
      m_load_context_1_of_3 (
        args("filename", "use_id", "settings"),
        "Loads a variable context from a file"))
          
    // locks the knowledge base from updates from other threads
    .def ("lock",
      &Madara::Knowledge_Engine::Knowledge_Base::lock,
      "Locks the knowledge base from updates from other threads")
        
    // Sets a logging level
    .def( "log_level",
      static_cast<
        int (*)(int)
      > (&Madara::Knowledge_Engine::Knowledge_Base::log_level),
      "Sets a logging level") 
        
    // Gets the logging level
    .def( "log_level",
      static_cast<
        int (*)(void)
      > (&Madara::Knowledge_Engine::Knowledge_Base::log_level),
      "Gets a logging level") 
        
    .def ("log_to_file",
      &Madara::Knowledge_Engine::Knowledge_Base::log_to_file,
      "Logs to a file")
        
    .def ("log_to_stderr",
      &Madara::Knowledge_Engine::Knowledge_Base::log_to_stderr,
      "Logs to stderr")
        
    .def ("log_to_system_log",
      &Madara::Knowledge_Engine::Knowledge_Base::log_to_system_log,
      "Logs to a system logger")
          
    // evaluate an expression
    .def( "print",
      static_cast<
        void (Madara::Knowledge_Engine::Knowledge_Base::*)(
          unsigned int) const
      > (&Madara::Knowledge_Engine::Knowledge_Base::print),
      m_print_0_of_1 (
        args("level"), "Prints all variables in the knowledge base"))
        
    // evaluate an expression
    .def( "print",
      static_cast<
        void (Madara::Knowledge_Engine::Knowledge_Base::*)(
          const std::string &, unsigned int) const
      > (&Madara::Knowledge_Engine::Knowledge_Base::print),
      m_print_1_of_2 (
        args("statement", "level"), "Expands and prints a statement"))

    // prints all knowledge variables
    .def ("print_knowledge",
      &Madara::Knowledge_Engine::Knowledge_Base::print_knowledge,
      m_print_knowledge_0_of_1 (
        args("level"),
        "Alias of print(level). Prints all variables in the knowledge base"))

    // get a knowledge record at an index
    .def( "retrieve_index",
      static_cast<
        Madara::Knowledge_Record (Madara::Knowledge_Engine::Knowledge_Base::*)(
          const std::string &,
            size_t,
            const Madara::Knowledge_Engine::Knowledge_Reference_Settings &)
      > (&Madara::Knowledge_Engine::Knowledge_Base::retrieve_index),
      m_retrieve_index_2_of_3 (
        args("key", "index", "settings"),
        "Retrieves a knowledge record from an index"))
          
    // expands and prints a statement
    .def ("save_context",
      &Madara::Knowledge_Engine::Knowledge_Base::save_context,
        "Saves the context to a file")
          
    // Sends all modified variables through the attached transports
    .def( "send_modifieds",
      static_cast<
        int (Madara::Knowledge_Engine::Knowledge_Base::*)(
          const std::string &,
          const Madara::Knowledge_Engine::Eval_Settings &)
      > (&Madara::Knowledge_Engine::Knowledge_Base::send_modifieds),
      m_send_modifieds_0_of_2 (
        args("prefix", "settings"),
        "Sends all modified variables through the attached transports."))

    // sets a knowledge record to a double
    .def( "set",
      static_cast<
        int (Madara::Knowledge_Engine::Knowledge_Base::*)(
          const std::string &,
          double,
          const Madara::Knowledge_Engine::Eval_Settings &)
      > (&Madara::Knowledge_Engine::Knowledge_Base::set),
      m_set_2_of_3 (
        args("key", "value", "settings"),
        "Sets a knowledge record to a double"))
        
    // sets a knowledge record to an array of doubles
    .def( "set",
      static_cast<
        int (Madara::Knowledge_Engine::Knowledge_Base::*)(
          const std::string &,
          const std::vector <double> &,
          const Madara::Knowledge_Engine::Eval_Settings &)
      > (&Madara::Knowledge_Engine::Knowledge_Base::set),
      m_set_2_of_3 (
        args("key", "value", "settings"),
        "Sets a knowledge record to an array of doubles"))
        
    // sets a knowledge record to an integer
    .def( "set",
      static_cast<
        int (Madara::Knowledge_Engine::Knowledge_Base::*)(
          const std::string &,
          Madara::Knowledge_Record::Integer,
          const Madara::Knowledge_Engine::Eval_Settings &)
      > (&Madara::Knowledge_Engine::Knowledge_Base::set),
      m_set_2_of_3 (
        args("key", "value", "settings"),
        "Sets a knowledge record to an integer"))
        
    // sets a knowledge record to an array of integer
    .def( "set",
      static_cast<
        int (Madara::Knowledge_Engine::Knowledge_Base::*)(
          const std::string &,
          const std::vector <Madara::Knowledge_Record::Integer> &,
          const Madara::Knowledge_Engine::Eval_Settings &)
      > (&Madara::Knowledge_Engine::Knowledge_Base::set),
      m_set_2_of_3 (
        args("key", "value", "settings"),
        "Sets a knowledge record to an array of integers"))
        
    // sets a knowledge record to a string
    .def( "set",
      static_cast<
        int (Madara::Knowledge_Engine::Knowledge_Base::*)(
          const std::string &,
          const std::string &,
          const Madara::Knowledge_Engine::Eval_Settings &)
      > (&Madara::Knowledge_Engine::Knowledge_Base::set),
      m_set_2_of_3 (
        args("key", "value", "settings"),
        "Sets a knowledge record to a string"))
        
    // sets an array index to an integer
    .def( "set_index",
      static_cast<
        int (Madara::Knowledge_Engine::Knowledge_Base::*)(
          const std::string &,
          size_t,
          Madara::Knowledge_Record::Integer,
          const Madara::Knowledge_Engine::Eval_Settings &)
      > (&Madara::Knowledge_Engine::Knowledge_Base::set_index),
      m_set_index_2_of_3 (
        args("key", "value", "settings"),
        "Sets an array index to an integer"))
        
    // sets an array index to a double
    .def( "set_index",
      static_cast<
        int (Madara::Knowledge_Engine::Knowledge_Base::*)(
          const std::string &,
          size_t,
          double,
          const Madara::Knowledge_Engine::Eval_Settings &)
      > (&Madara::Knowledge_Engine::Knowledge_Base::set_index),
      m_set_index_2_of_3 (
        args("key", "value", "settings"),
        "Sets an array index to a double"))
            
    // unlocks the knowledge base
    .def ("unlock",
      &Madara::Knowledge_Engine::Knowledge_Base::unlock,
      "Unlocks the knowledge base and allows other threads to access")
  
    // wait on an expression
    .def( "wait",
      static_cast<
        Madara::Knowledge_Record (Madara::Knowledge_Engine::Knowledge_Base::*)(
          const std::string &, const Madara::Knowledge_Engine::Wait_Settings &)
      > (&Madara::Knowledge_Engine::Knowledge_Base::wait),
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

  class_ <std::vector <Madara::Knowledge_Record> > ("KnowledgeRecordVector")
    .def(vector_indexing_suite<std::vector<Madara::Knowledge_Record> >());
  
  class_ <std::map <std::string, Madara::Knowledge_Record> > (
    "KnowledgeRecordMap")
    .def(map_indexing_suite<std::map <
      std::string, Madara::Knowledge_Record> >());

  define_knowledge_record ();
  define_filters ();
  define_transport ();
  define_knowledge_engine ();
}
