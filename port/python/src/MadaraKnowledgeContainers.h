#ifndef _MADARA_PYTHON_PORT_MADARA_KNOWLEDGE_CONTAINERS_H_
#define _MADARA_PYTHON_PORT_MADARA_KNOWLEDGE_CONTAINERS_H_

#include <boost/python/detail/wrap_python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <boost/python/suite/indexing/map_indexing_suite.hpp>
#include <boost/python/dict.hpp>
#include <boost/python/import.hpp>
#include <boost/python/enum.hpp>

#include "madara/knowledge/KnowledgeBase.h"
#include "madara/knowledge/containers/CircularBuffer.h"
#include "madara/knowledge/containers/CircularBufferConsumer.h"
#include "madara/filters/GenericFilters.h"
#include "FunctionDefaults.h"

/**
 * @file MadaraKnowledgeContainers.cpp
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains Boost.Python mappings for the C++ MADARA library
 * to a Python module called Madara.
 **/

using namespace boost::python;

/********************************************************
  * Filters namespace definitions
  ********************************************************/
void define_knowledge_containers (void)
{
  object containers = object (handle<> (
    PyModule_New ("madara.knowledge.containers")));

  containers.attr("__file__")="<synthetic>";
  scope().attr ("filters") = containers;
  containers.attr ("__doc__") = "Provides specialized knowledge containers";

  // this was the missing piece: sys.modules['modA.modB']=modB
  extract <dict> (getattr (
    import ("sys"), "modules"))()["madara.knowledge.containers"] = containers;

  scope containers_scope = containers;

  class_<madara::knowledge::containers::CircularBuffer> (
     "CircularBuffer",
     "Stores a thread-safe circular buffer with O(1) access times",
     init <> ())

    // constructors
    .def (init <const madara::knowledge::KnowledgeUpdateSettings &> ())

    .def (init <const std::string &, madara::knowledge::KnowledgeBase &,
      const madara::knowledge::KnowledgeUpdateSettings &> ())
    
    // methods

    // add a record
    .def ("add", 
      static_cast<
        void (madara::knowledge::containers::CircularBuffer::*)(
        const madara::knowledge::KnowledgeRecord &
        )
      > (&madara::knowledge::containers::CircularBuffer::add),
      "Adds a record to the end of the buffer")

    // add a vector of records
    .def ("add", 
      static_cast<
        void (madara::knowledge::containers::CircularBuffer::*)(
          const std::vector <madara::knowledge::KnowledgeRecord> &
        )
      > (&madara::knowledge::containers::CircularBuffer::add),
      "Adds a record to the end of the buffer")

    // retrieves an index of an array
    .def ("clear", &madara::knowledge::containers::CircularBuffer::clear,
      "Clears the buffer of all records")

    // retrieves an index of an array
    .def ("count", &madara::knowledge::containers::CircularBuffer::count,
      "Returns the number of records in the buffer")

    // get latest
    .def ("get", 
      static_cast<
        madara::knowledge::KnowledgeRecord
        (madara::knowledge::containers::CircularBuffer::*)(
          void
        ) const
      > (&madara::knowledge::containers::CircularBuffer::get),
      "Gets the latest added record to the buffer")

    // gets the earliest n records
    .def ("get_earliest", 
      static_cast<
        std::vector <madara::knowledge::KnowledgeRecord>
        (madara::knowledge::containers::CircularBuffer::*)(
          size_t
        ) const
      > (&madara::knowledge::containers::CircularBuffer::get_earliest),
      "Gets the oldest n elements in the buffer")

    // gets the latest n records
    .def ("get_latest", 
      static_cast<
        std::vector <madara::knowledge::KnowledgeRecord>
        (madara::knowledge::containers::CircularBuffer::*)(
          size_t
        ) const
      > (&madara::knowledge::containers::CircularBuffer::get_latest),
      "Gets the oldest n elements in the buffer")

    // gets the underlying prefix/name
    .def ("get_name", &madara::knowledge::containers::CircularBuffer::get_name,
      "Returns the underlying name of the container")

    // get latest
    .def ("inspect", 
      static_cast<
        madara::knowledge::KnowledgeRecord
        (madara::knowledge::containers::CircularBuffer::*)(
          madara::knowledge::KnowledgeRecord::Integer
        ) const
      > (&madara::knowledge::containers::CircularBuffer::inspect),
      "Inspects the record at the indicated position in the buffer. "
      "This position can be positive or negative from current position")

    // gets the latest n records
    .def ("inspect", 
      static_cast<
        std::vector <madara::knowledge::KnowledgeRecord>
        (madara::knowledge::containers::CircularBuffer::*)(
          madara::knowledge::KnowledgeRecord::Integer, size_t
        ) const
      > (&madara::knowledge::containers::CircularBuffer::inspect),
      "Inspects the records at the indicated position in the buffer. "
      "This position can be positive or negative from current position")

    // resizes the container
    .def ("resize", &madara::knowledge::containers::CircularBuffer::resize,
      "Resizes the container")

    // sets the index
    .def ("set_index", &madara::knowledge::containers::CircularBuffer::set_index,
      "Sets the buffer index to an arbitrary position")

    // sets the name
    .def ("set_name", 
      static_cast<
        void (madara::knowledge::containers::CircularBuffer::*)(
          const std::string &, madara::knowledge::KnowledgeBase &
        )
      > (&madara::knowledge::containers::CircularBuffer::set_name),
      "Sets the name inside of the Knowledge Base for the container to use")

    // sets the name
    .def ("set_name", 
      static_cast<
        void (madara::knowledge::containers::CircularBuffer::*)(
          const std::string &, madara::knowledge::Variables &
        )
      > (&madara::knowledge::containers::CircularBuffer::set_name),
      "Sets the name inside of the Knowledge Base for the container to use")

    // sets the index
    .def ("set_quality", &madara::knowledge::containers::CircularBuffer::set_quality,
      "Sets the write quality of updates")

    // sets the index
    .def ("set_settings", &madara::knowledge::containers::CircularBuffer::set_settings,
      "Sets the write quality of updates")

    // returns the size of the buffer
    .def ("size", &madara::knowledge::containers::CircularBuffer::size,
      "Returns the size of the buffer")

  ;

  class_<madara::knowledge::containers::CircularBufferConsumer> (
     "CircularBufferConsumer",
     "References a circular buffer created by a CircularBuffer container."
     " This provides a personalized consumer with a local index for iteration",
     init <> ())

    .def (init <const std::string &, madara::knowledge::KnowledgeBase &> ())
    
    // methods

    // retrieves the number of records
    .def ("count",
      &madara::knowledge::containers::CircularBufferConsumer::count,
      "Returns the number of records in the buffer")

    // get latest
    .def ("consume", 
      static_cast<
        madara::knowledge::KnowledgeRecord
        (madara::knowledge::containers::CircularBufferConsumer::*)(
          void
        ) const
      > (&madara::knowledge::containers::CircularBufferConsumer::consume),
      "Consumes earliest record in the buffer")

    // gets the earliest n records
    .def ("consume_earliest", 
      static_cast<
        std::vector <madara::knowledge::KnowledgeRecord>
        (madara::knowledge::containers::CircularBufferConsumer::*)(
          size_t
        ) const
      > (&madara::knowledge::containers::CircularBufferConsumer::consume_earliest),
      "Consumes the oldest n elements in the buffer")

    // gets the latest n records
    .def ("consume_latest", 
      static_cast<
        std::vector <madara::knowledge::KnowledgeRecord>
        (madara::knowledge::containers::CircularBufferConsumer::*)(
          size_t
        ) const
      > (&madara::knowledge::containers::CircularBufferConsumer::consume_latest),
      "Consumes the oldest n elements in the buffer")

    // gets the underlying prefix/name
    .def ("get_dropped",
      &madara::knowledge::containers::CircularBufferConsumer::get_dropped,
      "Returns the number of records dropped since last consume")

    // gets the underlying prefix/name
    .def ("get_name",
      &madara::knowledge::containers::CircularBufferConsumer::get_name,
      "Returns the underlying name of the container")

    // get latest
    .def ("inspect", 
      static_cast<
        madara::knowledge::KnowledgeRecord
        (madara::knowledge::containers::CircularBufferConsumer::*)(
          madara::knowledge::KnowledgeRecord::Integer
        ) const
      > (&madara::knowledge::containers::CircularBufferConsumer::inspect),
      "Inspects the record at the indicated position in the buffer. "
      "This position can be positive or negative from current position")

    // gets the latest n records
    .def ("inspect", 
      static_cast<
        std::vector <madara::knowledge::KnowledgeRecord>
        (madara::knowledge::containers::CircularBufferConsumer::*)(
          madara::knowledge::KnowledgeRecord::Integer, size_t
        ) const
      > (&madara::knowledge::containers::CircularBufferConsumer::inspect),
      "Inspects the records at the indicated position in the buffer. "
      "This position can be positive or negative from current position")

    // get latest
    .def ("peek_latest", 
      static_cast<
        madara::knowledge::KnowledgeRecord
        (madara::knowledge::containers::CircularBufferConsumer::*)(
          void
        ) const
      > (&madara::knowledge::containers::CircularBufferConsumer::peek_latest),
      "Peek at the newest record in the buffer")

    // gets the latest n records
    .def ("peek_latest", 
      static_cast<
        std::vector <madara::knowledge::KnowledgeRecord>
        (madara::knowledge::containers::CircularBufferConsumer::*)(
          size_t
        ) const
      > (&madara::knowledge::containers::CircularBufferConsumer::peek_latest),
      "Peek at the newest n elements in the buffer")

    // returns the remaining records
    .def ("remaining",
      &madara::knowledge::containers::CircularBufferConsumer::resize,
      "Returns the remaining records in the buffer")

    // resizes the container
    .def ("resize",
      &madara::knowledge::containers::CircularBufferConsumer::resize,
      "Resizes the container")

    // resizes the container
    .def ("resync",
      &madara::knowledge::containers::CircularBufferConsumer::resize,
      "Resyncs the local index to the producer index in the buffer")

    // sets the index
    .def ("set_index",
      &madara::knowledge::containers::CircularBufferConsumer::set_index,
      "Sets the buffer index to an arbitrary position")

    // sets the name
    .def ("set_name", 
      static_cast<
        void (madara::knowledge::containers::CircularBufferConsumer::*)(
          const std::string &, madara::knowledge::KnowledgeBase &
        )
      > (&madara::knowledge::containers::CircularBufferConsumer::set_name),
      "Sets the name inside of the Knowledge Base for the container to use")

    // sets the name
    .def ("set_name", 
      static_cast<
        void (madara::knowledge::containers::CircularBufferConsumer::*)(
          const std::string &, madara::knowledge::Variables &
        )
      > (&madara::knowledge::containers::CircularBufferConsumer::set_name),
      "Sets the name inside of the Knowledge Base for the container to use")

    // returns the size of the buffer
    .def ("size",
      &madara::knowledge::containers::CircularBufferConsumer::size,
      "Returns the size of the buffer")

  ;


  class_<madara::knowledge::containers::Integer> (
     "Integer",
     "References an integer in the knowledge base",
     init <> ())

    // constructors
    .def (init <const madara::knowledge::KnowledgeUpdateSettings &> ())

    .def (init <const std::string &, madara::knowledge::KnowledgeBase &,
      const madara::knowledge::KnowledgeUpdateSettings &> ())
    
    .def (init <const std::string &, madara::knowledge::KnowledgeBase &,
      madara::knowledge::KnowledgeRecord::Integer,
      const madara::knowledge::KnowledgeUpdateSettings &> ())
    
    // methods

    // check for existence
    .def ("exists",
      &madara::knowledge::containers::Integer::exists,
      "Checks if the underlying record has ever been modified/created")

    // gets the underlying prefix/name
    .def ("get_name",
      &madara::knowledge::containers::Integer::get_name,
      "Returns the underlying name of the container")

    // gets the underlying prefix/name
    .def ("get_settings",
      &madara::knowledge::containers::Integer::get_settings,
      "Returns the read/write settings of the container")

    // returns the remaining records
    .def ("modify",
      &madara::knowledge::containers::Integer::modify,
      "Marks the record/container as modified to send over transport")

    // sets the name
    .def ("set_name", 
      static_cast<
        void (madara::knowledge::containers::Integer::*)(
          const std::string &, madara::knowledge::KnowledgeBase &
        )
      > (&madara::knowledge::containers::Integer::set_name),
      "Sets the name inside of the Knowledge Base for the container to use")

    // sets the name
    .def ("set_name", 
      static_cast<
        void (madara::knowledge::containers::Integer::*)(
          const std::string &, madara::knowledge::Variables &
        )
      > (&madara::knowledge::containers::Integer::set_name),
      "Sets the name inside of the Knowledge Base for the container to use")

    // gets the underlying prefix/name
    .def ("set_settings",
      &madara::knowledge::containers::Integer::set_settings,
      "Sets the read/write settings for the container")

    // returns the double
    .def ("to_double",
      &madara::knowledge::containers::Integer::to_double,
      "Returns the double")

    // returns the integer
    .def ("to_integer",
      &madara::knowledge::containers::Integer::to_integer,
      "Returns the integer")

    // returns the record
    .def ("to_record",
      &madara::knowledge::containers::Integer::to_record,
      "Returns the underlying record")

    // returns the container as a string
    .def ("to_string",
      &madara::knowledge::containers::Integer::to_string,
      "Returns the container as a string")

    // returns the container as a string
    .def ("set",
      static_cast<
        madara::knowledge::KnowledgeRecord::Integer 
        (madara::knowledge::containers::Integer::*)(
          madara::knowledge::KnowledgeRecord::Integer
        )
      > (&madara::knowledge::containers::Integer::operator=),
      "Sets the value")

    .def (self += madara::knowledge::KnowledgeRecord::Integer())

    .def (self -= madara::knowledge::KnowledgeRecord::Integer())

    .def (self < madara::knowledge::KnowledgeRecord::Integer())

    .def (self <= madara::knowledge::KnowledgeRecord::Integer())

    .def (self != madara::knowledge::KnowledgeRecord::Integer())

    .def (self == madara::knowledge::KnowledgeRecord::Integer())

    .def (self >= madara::knowledge::KnowledgeRecord::Integer())

    .def (self > madara::knowledge::KnowledgeRecord::Integer())

    .def ("operator++",
    &madara::knowledge::containers::Integer::operator++,
    "Adds one to the integer")

    .def ("operator--",
    &madara::knowledge::containers::Integer::operator--,
    "Subtracts one from the integer")

  ;
}


#endif // _MADARA_PYTHON_PORT_MADARA_KNOWLEDGE_CONTAINERS_H_

