#ifndef _MADARA_PYTHON_PORT_MADARA_KNOWLEGE_H_
#define _MADARA_PYTHON_PORT_MADARA_KNOWLEGE_H_

#include <boost/python/detail/wrap_python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <boost/python/suite/indexing/map_indexing_suite.hpp>
#include <boost/python/dict.hpp>
#include <boost/python/import.hpp>
#include <boost/python/enum.hpp>

#include "madara/knowledge/KnowledgeBase.h"
#include "madara/filters/GenericFilters.h"
#include "FunctionDefaults.h"
#include "MadaraKnowledgeContainers.h"

/**
 * @file MadaraModule.cpp
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains Boost.Python mappings for the C++ MADARA library
 * to a Python module called Madara.
 **/

using namespace boost::python;

class knowledge_NS {};

void define_knowledge (void)
{
  object ke = object (handle<> (
    PyModule_New ("madara.knowledge")));

   ke.attr("__file__")="<synthetic>";
   scope().attr ("knowledge") = ke;
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


  class_<madara::knowledge::CheckpointSettings> ("Any",
    "A class that can represent any type in a KnowledgeRecord", init <> ())
  ;

  class_<madara::knowledge::CheckpointSettings> ("CheckpointSettings",
    "Settings for checkpoints and file saves", init <> ())

     // class members
    .def_readwrite("buffer_size",
      &madara::knowledge::CheckpointSettings::buffer_size,
      "size of the buffer needed to hold file in memory")

    .def_readwrite("clear_knowledge",
      &madara::knowledge::CheckpointSettings::clear_knowledge,
      "If true, during loads, clear the KnowledgeBase first")

    .def_readwrite("filename",
      &madara::knowledge::CheckpointSettings::filename,
      "the path and name of the file to load/save")

    .def_readwrite("initial_lamport_clock",
      &madara::knowledge::CheckpointSettings::initial_lamport_clock,
      "initial lamport clock saved in the checkpoint ")

    .def_readwrite("initial_state",
      &madara::knowledge::CheckpointSettings::initial_state,
      "initial state of interest (useful for loading ranges of checkpoints). "
      " Thsi is an inclusive identifier, so 0 means to load from initial"
      "context/checkpoint save")

    .def_readwrite("initial_timestamp",
      &madara::knowledge::CheckpointSettings::initial_timestamp,
      "initial wallclock time saved in the checkpoint")

    .def_readwrite("keep_open",
      &madara::knowledge::CheckpointSettings::keep_open,
      "attempt to keep the file open for later usage (not really recommended)")

    .def_readwrite("last_lamport_clock",
      &madara::knowledge::CheckpointSettings::last_lamport_clock,
      "final lamport clock in the checkpoint")

    .def_readwrite("last_state",
      &madara::knowledge::CheckpointSettings::last_state,
      "the last state of interest (useful for loading ranges of checkpoints). "
      "This is an inclusive identifier. If last_state >= states, it will "
      "essentially indicate the last valid state")

    .def_readwrite("last_timestamp",
      &madara::knowledge::CheckpointSettings::last_timestamp,
      "final wallclock time saved in the checkpoint")

    .def_readwrite("originator",
      &madara::knowledge::CheckpointSettings::originator,
      "the originator id of the checkpoint (who saved it)")

    .def_readwrite("override_lamport",
      &madara::knowledge::CheckpointSettings::override_lamport,
      "use the lamport clocks in this class instead of the KB clock when"
      "writing context or checkpoints")

    .def_readwrite("override_timestamp",
      &madara::knowledge::CheckpointSettings::override_timestamp,
      "use the timestamps in this class instead of wallclock time when"
      "writing context or checkpoints")

    .def_readwrite("prefixes",
      &madara::knowledge::CheckpointSettings::prefixes,
      "a list of prefixes to save/load. If empty, all prefixes are valid")

    .def_readwrite("reset_checkpoint",
      &madara::knowledge::CheckpointSettings::reset_checkpoint,
      "if true, resets the checkpoint to start a new diff from this point on")

    .def_readwrite("states",
      &madara::knowledge::CheckpointSettings::states,
      "the number of states checkpointed in the file stream")

    .def_readwrite("version",
      &madara::knowledge::CheckpointSettings::version,
      "the MADARA version used when the checkpoint was saved")

      
  ;

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
     .def ("set_jpeg",
     static_cast<
     void (madara::knowledge::KnowledgeRecord::*)(
     const unsigned char * new_value, size_t size
     )
     > (&madara::knowledge::KnowledgeRecord::set_jpeg),
     "Sets the value to a jpeg")

     // sets the contents of the record to a file
     .def ("set_file", 
     static_cast<
     void (madara::knowledge::KnowledgeRecord::*)(
     const unsigned char * new_value, size_t size
     )
     > (&madara::knowledge::KnowledgeRecord::set_file),
     "Sets the value to a file's contents")

     // sets a knowledge record to a double
     .def ("set",
     static_cast<
     void (madara::knowledge::KnowledgeRecord::*)(
     double
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
       int
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

     // returns true if record exists
     .def ("exists", &madara::knowledge::KnowledgeRecord::size,
     "Returns whether the knowledge has been set/modified/created")

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

     // class members
    .def_readwrite("clock",
      &madara::knowledge::KnowledgeRecord::clock,
      "last modification lamport clock")

    .def_readwrite("quality",
      &madara::knowledge::KnowledgeRecord::quality,
      "write priority for any local updates")
    
    .def_readwrite("toi",
      &madara::knowledge::KnowledgeRecord::toi,
      "last modification system clock time (time of insertion)")
  
    .def_readwrite("write_quality",
      &madara::knowledge::KnowledgeRecord::write_quality,
      "priority of the update")
      
    // because of the templated nature of KnowledgeRecord::operator=, I could not
    // figure this out
    // .def ("operator=", 
    // static_cast<madara::knowledge::KnowledgeRecord (
    // madara::knowledge::KnowledgeRecord::*)(
    // int &&)> (
    // &madara::knowledge::KnowledgeRecord::operator=),
    // "Assigns an int to the record",
    // return_value_policy<reference_existing_object> ())

    .def ("operator==",
    static_cast<bool (
    madara::knowledge::KnowledgeRecord::*)(
    const madara::knowledge::KnowledgeRecord &) const> (
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
     .value ("EMPTY",
     madara::knowledge::KnowledgeRecord::EMPTY)
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
     .value ("ALL_CLEARABLES",
     madara::knowledge::KnowledgeRecord::ALL_CLEARABLES)
     
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
          
    // expands and prints a statement
    .def ("load_context",
      static_cast<int64_t (madara::knowledge::KnowledgeBase::*)(
        madara::knowledge::CheckpointSettings &,
        const madara::knowledge::KnowledgeUpdateSettings &)
      > (&madara::knowledge::KnowledgeBase::load_context),
      m_load_context_1_of_2 (
        args("checkpoint_settings", "update_settings"),
        "Loads a variable context from a file with settings "
        "for checkpoint and knowledge updates"))
          
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
              
    // saves the context as JSON
    .def ("save_as_json",
      static_cast<int64_t
        (madara::knowledge::KnowledgeBase::*)(
          const madara::knowledge::CheckpointSettings &) const
      > (&madara::knowledge::KnowledgeBase::save_as_json),
        m_save_as_json_1_of_1 (
        args("settings"),
        "Saves the context in JSON markup"))
         
    // saves the context as karl
    .def ("save_as_karl",
      static_cast<int64_t
        (madara::knowledge::KnowledgeBase::*)(
          const madara::knowledge::CheckpointSettings &) const
      > (&madara::knowledge::KnowledgeBase::save_as_karl),
        m_save_as_karl_1_of_1 (
        args("settings"),
        "Saves the context as a human-readable KaRL script"))
         
    // saves the context as binary
    .def ("save_context",
      static_cast<int64_t
        (madara::knowledge::KnowledgeBase::*)(
          madara::knowledge::CheckpointSettings &) const
      > (&madara::knowledge::KnowledgeBase::save_context),
        m_save_context_1_of_1 (
        args("settings"),
        "Saves the context to a file with settings "))
           
    // saves a diff of the context as binary
    .def ("save_checkpoint",
      static_cast<int64_t
        (madara::knowledge::KnowledgeBase::*)(
          madara::knowledge::CheckpointSettings &) const
      > (&madara::knowledge::KnowledgeBase::save_checkpoint),
        m_save_checkpoint_1_of_1 (
        args("settings"),
        "Saves a diff of the context to a file with settings "))
          
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
    // .def( "set_index",
    //   static_cast<
    //     int (madara::knowledge::KnowledgeBase::*)(
    //       const std::string &,
    //       size_t,
    //       madara::knowledge::KnowledgeRecord::Integer,
    //       const madara::knowledge::EvalSettings &)
    //   > (&madara::knowledge::KnowledgeBase::set_index),
    //   m_set_index_2_of_3 (
    //     args("key", "value", "settings"),
    //     "Sets an array index to an integer"))
        
    // sets an array index to a double
    // .def( "set_index",
    //   static_cast<
    //     int (madara::knowledge::KnowledgeBase::*)(
    //       const std::string &,
    //       size_t,
    //       double,
    //       const madara::knowledge::EvalSettings &)
    //   > (&madara::knowledge::KnowledgeBase::set_index),
    //   m_set_index_2_of_3 (
    //     args("key", "value", "settings"),
    //     "Sets an array index to a double"))
            
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

  class_ <std::vector <madara::knowledge::KnowledgeRecord> > ("KnowledgeRecordVector")
    .def(vector_indexing_suite<std::vector<madara::knowledge::KnowledgeRecord> >());
  
  class_ <std::map <std::string, madara::knowledge::KnowledgeRecord> > (
    "KnowledgeRecordMap")
    .def(map_indexing_suite<std::map <
      std::string, madara::knowledge::KnowledgeRecord> >());

  // define madara.knowledge.containers
  define_knowledge_containers ();

}

#endif // _MADARA_PYTHON_PORT_MADARA_KNOWLEGE_H_

