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
#include "madara/knowledge/containers/Double.h"
#include "madara/knowledge/containers/Integer.h"
#include "madara/knowledge/containers/NativeDoubleVector.h"
#include "madara/knowledge/containers/NativeIntegerVector.h"
#include "madara/knowledge/containers/String.h"
#include "madara/knowledge/containers/Vector.h"
#include "madara/filters/GenericFilters.h"
#include "FunctionDefaults.h"
#include "MadaraKnowledgeContainers.h"

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
  scope().attr ("containers") = containers;
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
    
    .def (init <const std::string &, madara::knowledge::Variables &,
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
    
    .def (init <const std::string &, madara::knowledge::Variables &,
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


  class_<madara::knowledge::containers::Double> (
     "Double",
     "References a double in the knowledge base",
     init <> ())

    // constructors
    .def (init <const madara::knowledge::KnowledgeUpdateSettings &> ())

    .def (init <const std::string &, madara::knowledge::KnowledgeBase &,
      const madara::knowledge::KnowledgeUpdateSettings &> ())
    
    .def (init <const std::string &, madara::knowledge::KnowledgeBase &,
      double,
      const madara::knowledge::KnowledgeUpdateSettings &> ())
    
    .def (init <const std::string &, madara::knowledge::Variables &,
      double,
      const madara::knowledge::KnowledgeUpdateSettings &> ())
    
    // methods

    // check for existence
    .def ("exists",
      &madara::knowledge::containers::Double::exists,
      "Checks if the underlying record has ever been modified/created")

    // gets the underlying prefix/name
    .def ("get_name",
      &madara::knowledge::containers::Double::get_name,
      "Returns the underlying name of the container")

    // gets the underlying prefix/name
    .def ("get_settings",
      &madara::knowledge::containers::Double::get_settings,
      "Returns the read/write settings of the container")

    // returns the remaining records
    .def ("modify",
      &madara::knowledge::containers::Double::modify,
      "Marks the record/container as modified to send over transport")

    // sets the name
    .def ("set_name", 
      static_cast<
        void (madara::knowledge::containers::Double::*)(
          const std::string &, madara::knowledge::KnowledgeBase &
        )
      > (&madara::knowledge::containers::Double::set_name),
      "Sets the name inside of the Knowledge Base for the container to use")

    // sets the name
    .def ("set_name", 
      static_cast<
        void (madara::knowledge::containers::Double::*)(
          const std::string &, madara::knowledge::Variables &
        )
      > (&madara::knowledge::containers::Double::set_name),
      "Sets the name inside of the Knowledge Base for the container to use")

    // gets the underlying prefix/name
    .def ("set_settings",
      &madara::knowledge::containers::Double::set_settings,
      "Sets the read/write settings for the container")

    // returns the double
    .def ("to_double",
      &madara::knowledge::containers::Double::to_double,
      "Returns the double")

    // returns the integer
    .def ("to_integer",
      &madara::knowledge::containers::Double::to_integer,
      "Returns the integer")

    // returns the record
    .def ("to_record",
      &madara::knowledge::containers::Double::to_record,
      "Returns the underlying record")

    // returns the container as a string
    .def ("to_string",
      &madara::knowledge::containers::Double::to_string,
      "Returns the container as a string")

    // returns the container as a string
    .def ("set",
      static_cast<
        double
        (madara::knowledge::containers::Double::*)(
          double
        )
      > (&madara::knowledge::containers::Double::operator=),
      "Sets the value")

    .def (self += double ())

    .def (self -= double ())

    .def (self < double ())

    .def (self <= double ())

    .def (self != double ())

    .def (self == double ())

    .def (self >= double ())

    .def (self > double ())

    .def ("operator++",
    &madara::knowledge::containers::Double::operator++,
    "Adds one to the double")

    .def ("operator--",
    &madara::knowledge::containers::Double::operator--,
    "Subtracts one from the double")

  ;

  class_<madara::knowledge::containers::NativeDoubleVector> (
     "NativeDoubleVector",
     "References a double array inside of a knowledge record",
     init <> ())

    // constructors
    .def (init <const madara::knowledge::KnowledgeUpdateSettings &> ())

    .def (init <const std::string &, madara::knowledge::KnowledgeBase &,
      int,
      const madara::knowledge::KnowledgeUpdateSettings &> ())
    
    .def (init <const std::string &, madara::knowledge::Variables &,
      int,
      const madara::knowledge::KnowledgeUpdateSettings &> ())
    
    // methods

    // check for existence
    .def ("exists",
      &madara::knowledge::containers::NativeDoubleVector::exists,
      "Checks if the underlying record has ever been modified/created")

    // gets the underlying prefix/name
    .def ("get_name",
      &madara::knowledge::containers::NativeDoubleVector::get_name,
      "Returns the underlying name of the container")

    // gets the underlying prefix/name
    .def ("get_settings",
      &madara::knowledge::containers::NativeDoubleVector::get_settings,
      "Returns the read/write settings of the container")

    // returns the remaining records
    .def ("modify",
      &madara::knowledge::containers::NativeDoubleVector::modify,
      "Marks the record/container as modified to send over transport")

    // pushes a record onto the back of the vector
    .def ("push_back",
      &madara::knowledge::containers::NativeDoubleVector::push_back,
      "Pushes a double onto the back of the vector")

    // pushes a record onto the back of the vector
    .def ("resize",
      &madara::knowledge::containers::NativeIntegerVector::resize,
      m_resize_1_of_1 (
        args("size"),
        "Resizes the vector to a size"))

    // sets the name
    .def ("set_name", 
      static_cast<
        void (madara::knowledge::containers::NativeDoubleVector::*)(
          const std::string &, madara::knowledge::KnowledgeBase &, int
        )
      > (&madara::knowledge::containers::NativeDoubleVector::set_name),
      "Sets the name inside of the Knowledge Base for the container to use")

    // sets the name
    .def ("set_name", 
      static_cast<
        void (madara::knowledge::containers::NativeDoubleVector::*)(
          const std::string &, madara::knowledge::Variables &, int
        )
      > (&madara::knowledge::containers::NativeDoubleVector::set_name),
      "Sets the name inside of the Knowledge Base for the container to use")

    // gets the underlying prefix/name
    .def ("set_settings",
      &madara::knowledge::containers::NativeDoubleVector::set_settings,
      "Sets the read/write settings for the container")

    // returns the double
    .def ("to_doubles",
      &madara::knowledge::containers::NativeDoubleVector::to_doubles,
      "Returns the double")

    // returns the record
    .def ("to_record",
      static_cast<
        madara::knowledge::KnowledgeRecord
        (madara::knowledge::containers::NativeDoubleVector::*)(
          void
        ) const
      > (&madara::knowledge::containers::NativeDoubleVector::to_record),
      "Returns the underlying record")

    .def ("set",
      static_cast<
        int
        (madara::knowledge::containers::NativeDoubleVector::*)(
          size_t index,
          double value
        )
      > (&madara::knowledge::containers::NativeDoubleVector::set),
      "Sets an index in the vector")

    .def ("set",
      static_cast<
        int
        (madara::knowledge::containers::NativeDoubleVector::*)(
          size_t index,
          double value, 
          const madara::knowledge::KnowledgeUpdateSettings & settings
        )
      > (&madara::knowledge::containers::NativeDoubleVector::set),
      "Sets an index in the vector")

    .def ("set",
      static_cast<
        int
        (madara::knowledge::containers::NativeDoubleVector::*)(
          const std::vector <double> & value
        )
      > (&madara::knowledge::containers::NativeDoubleVector::set),
      "Sets the entire vector")

    .def ("set",
      static_cast<
        int
        (madara::knowledge::containers::NativeDoubleVector::*)(
          const std::vector <double> & value,
          const madara::knowledge::KnowledgeUpdateSettings & settings
        )
      > (&madara::knowledge::containers::NativeDoubleVector::set),
      "Sets the entire vector")

      .def("__getitem__", &madara::knowledge::containers::NativeDoubleVector::operator[],
      "Gets an index of the vector")

      .def("__setitem__", static_cast<
        int
        (madara::knowledge::containers::NativeDoubleVector::*)(
          size_t index,
          double value
        )
      > (&madara::knowledge::containers::NativeDoubleVector::set),
      "Sets an index in the vector")
  ;


  class_<madara::knowledge::containers::NativeIntegerVector> (
     "NativeIntegerVector",
     "References an integer array inside of a knowledge record",
     init <> ())

    // constructors
    .def (init <const madara::knowledge::KnowledgeUpdateSettings &> ())

    .def (init <const std::string &, madara::knowledge::KnowledgeBase &,
      int,
      const madara::knowledge::KnowledgeUpdateSettings &> ())
    
    .def (init <const std::string &, madara::knowledge::Variables &,
      int,
      const madara::knowledge::KnowledgeUpdateSettings &> ())
    
    // methods

    // check for existence
    .def ("exists",
      &madara::knowledge::containers::NativeIntegerVector::exists,
      "Checks if the underlying record has ever been modified/created")

    // gets the underlying prefix/name
    .def ("get_name",
      &madara::knowledge::containers::NativeIntegerVector::get_name,
      "Returns the underlying name of the container")

    // gets the underlying prefix/name
    .def ("get_settings",
      &madara::knowledge::containers::NativeIntegerVector::get_settings,
      "Returns the read/write settings of the container")

    // returns the remaining records
    .def ("modify",
      &madara::knowledge::containers::NativeIntegerVector::modify,
      "Marks the record/container as modified to send over transport")

    // pushes a record onto the back of the vector
    .def ("push_back",
      &madara::knowledge::containers::NativeIntegerVector::push_back,
      "Pushes an integer onto the back of the vector")

    // pushes a record onto the back of the vector
    .def ("resize",
      &madara::knowledge::containers::NativeIntegerVector::resize,
      m_resize_1_of_1 (
        args("size"),
        "Resizes the vector to a size"))

    // sets the name
    .def ("set_name", 
      static_cast<
        void (madara::knowledge::containers::NativeIntegerVector::*)(
          const std::string &, madara::knowledge::KnowledgeBase &, int
        )
      > (&madara::knowledge::containers::NativeIntegerVector::set_name),
      "Sets the name inside of the Knowledge Base for the container to use")

    // sets the name
    .def ("set_name", 
      static_cast<
        void (madara::knowledge::containers::NativeIntegerVector::*)(
          const std::string &, madara::knowledge::Variables &, int
        )
      > (&madara::knowledge::containers::NativeIntegerVector::set_name),
      "Sets the name inside of the Knowledge Base for the container to use")

    // gets the underlying prefix/name
    .def ("set_settings",
      &madara::knowledge::containers::NativeIntegerVector::set_settings,
      "Sets the read/write settings for the container")

    // returns the double
    .def ("to_integers",
      &madara::knowledge::containers::NativeIntegerVector::to_integers,
      "Returns the vector as an iterable vector of integers")

    // returns the record
    .def ("to_record",
      static_cast<
        madara::knowledge::KnowledgeRecord
        (madara::knowledge::containers::NativeIntegerVector::*)(
          void
        ) const
      > (&madara::knowledge::containers::NativeIntegerVector::to_record),
      "Returns the underlying record")

    .def ("set",
      static_cast<
        int
        (madara::knowledge::containers::NativeIntegerVector::*)(
          size_t index,
          int64_t value
        )
      > (&madara::knowledge::containers::NativeIntegerVector::set),
      "Sets an index in the vector")

    .def ("set",
      static_cast<
        int
        (madara::knowledge::containers::NativeIntegerVector::*)(
          size_t index,
          int64_t value, 
          const madara::knowledge::KnowledgeUpdateSettings & settings
        )
      > (&madara::knowledge::containers::NativeIntegerVector::set),
      "Sets an index in the vector")

    .def ("set",
      static_cast<
        int
        (madara::knowledge::containers::NativeIntegerVector::*)(
          const std::vector <int64_t> & value
        )
      > (&madara::knowledge::containers::NativeIntegerVector::set),
      "Sets the entire vector")

    .def ("set",
      static_cast<
        int
        (madara::knowledge::containers::NativeIntegerVector::*)(
          const std::vector <int64_t> & value,
          const madara::knowledge::KnowledgeUpdateSettings & settings
        )
      > (&madara::knowledge::containers::NativeIntegerVector::set),
      "Sets the entire vector")

      .def("__getitem__", &madara::knowledge::containers::NativeIntegerVector::operator[],
      "Gets an index of the vector")

      .def("__setitem__", static_cast<
        int
        (madara::knowledge::containers::NativeIntegerVector::*)(
          size_t index,
          int64_t value
        )
      > (&madara::knowledge::containers::NativeIntegerVector::set),
      "Sets an index in the vector")
  ;


  class_<madara::knowledge::containers::String> (
     "String",
     "References a string in the knowledge base",
     init <> ())

    // constructors
    .def (init <const madara::knowledge::KnowledgeUpdateSettings &> ())

    .def (init <const std::string &, madara::knowledge::KnowledgeBase &,
      const madara::knowledge::KnowledgeUpdateSettings &> ())
    
    .def (init <const std::string &, madara::knowledge::KnowledgeBase &,
      const std::string &,
      const madara::knowledge::KnowledgeUpdateSettings &> ())
    
    // methods

    // check for existence
    .def ("exists",
      &madara::knowledge::containers::String::exists,
      "Checks if the underlying record has ever been modified/created")

    // gets the underlying prefix/name
    .def ("get_name",
      &madara::knowledge::containers::String::get_name,
      "Returns the underlying name of the container")

    // gets the underlying prefix/name
    .def ("get_settings",
      &madara::knowledge::containers::String::get_settings,
      "Returns the read/write settings of the container")

    // returns the remaining records
    .def ("modify",
      &madara::knowledge::containers::String::modify,
      "Marks the record/container as modified to send over transport")

    // sets the name
    .def ("set_name", 
      static_cast<
        void (madara::knowledge::containers::String::*)(
          const std::string &, madara::knowledge::KnowledgeBase &
        )
      > (&madara::knowledge::containers::String::set_name),
      "Sets the name inside of the Knowledge Base for the container to use")

    // sets the name
    .def ("set_name", 
      static_cast<
        void (madara::knowledge::containers::String::*)(
          const std::string &, madara::knowledge::Variables &
        )
      > (&madara::knowledge::containers::String::set_name),
      "Sets the name inside of the Knowledge Base for the container to use")

    // gets the underlying prefix/name
    .def ("set_settings",
      &madara::knowledge::containers::String::set_settings,
      "Sets the read/write settings for the container")

    // returns the double
    .def ("to_double",
      &madara::knowledge::containers::String::to_double,
      "Returns the double")

    // returns the integer
    .def ("to_integer",
      &madara::knowledge::containers::String::to_integer,
      "Returns the integer")

    // returns the record
    .def ("to_record",
      &madara::knowledge::containers::String::to_record,
      "Returns the underlying record")

    // returns the container as a string
    .def ("to_string",
      &madara::knowledge::containers::String::to_string,
      "Returns the container as a string")

    // returns the container as a string
    .def ("set",
      static_cast<
        std::string
        (madara::knowledge::containers::String::*)(
          const std::string &
        )
      > (&madara::knowledge::containers::String::operator=),
      "Sets the value")

    .def (self += std::string ())

    .def (self < std::string ())

    .def (self <= std::string ())

    .def (self != std::string ())

    .def (self == std::string ())

    .def (self >= std::string ())

    .def (self > std::string ())
  ;


  class_<madara::knowledge::containers::Vector> (
     "Vector",
     "References a vector with O(1) access times for variables in a context",
     init <> ())

    // constructors
    .def (init <const madara::knowledge::KnowledgeUpdateSettings &,
      const std::string &> ())

    // methods

    // check for existence
    .def ("exists",
      &madara::knowledge::containers::Vector::exists,
      "Checks if the underlying record has ever been modified/created")

    // gets the underlying prefix/name
    .def ("get_name",
      &madara::knowledge::containers::Vector::get_name,
      "Returns the underlying name of the container")

    // gets the underlying prefix/name
    .def ("get_settings",
      &madara::knowledge::containers::Vector::get_settings,
      "Returns the read/write settings of the container")

    // modifies the container
    .def ("modify",
      static_cast<
        void (madara::knowledge::containers::Vector::*)(
          void
        )
      > (&madara::knowledge::containers::Vector::modify),
      "Marks the record/container as modified to send over transport")

    // modifies the container
    .def ("modify",
      static_cast<
        void (madara::knowledge::containers::Vector::*)(
          size_t
        )
      > (&madara::knowledge::containers::Vector::modify),
      "Marks the index as modified to send over transport")

    // gets the underlying prefix/name
    .def ("modify_size",
      &madara::knowledge::containers::Vector::modify_size,
      "Marks the vector size as modified so it can be sent over transport")

    // pushes a record onto the back of the vector
    .def ("push_back",
      &madara::knowledge::containers::Vector::push_back,
      "Pushes a record onto the back of the vector")

    // pushes a record onto the back of the vector
    .def ("resize",
      &madara::knowledge::containers::Vector::resize,
      m_resize_0_of_2 (
        args("size", "delete_vars"),
        "Resizes the vector to a size and potentially deleting unused vars"))

    // sets the name
    .def ("set_name", 
      static_cast<
        void (madara::knowledge::containers::Vector::*)(
          const std::string &, madara::knowledge::KnowledgeBase &, int
        )
      > (&madara::knowledge::containers::Vector::set_name),
      "Sets the name inside of the Knowledge Base for the container to use")

    // sets the name
    .def ("set_name", 
      static_cast<
        void (madara::knowledge::containers::Vector::*)(
          const std::string &, madara::knowledge::Variables &, int
        )
      > (&madara::knowledge::containers::Vector::set_name),
      "Sets the name inside of the Knowledge Base for the container to use")

    // gets the underlying prefix/name
    .def ("set_settings",
      &madara::knowledge::containers::Vector::set_settings,
      "Sets the read/write settings for the container")

    // returns the record
    .def ("to_record", &madara::knowledge::containers::Vector::to_record,
      "Returns an underlying record")

    .def ("set",
      static_cast<
        int
        (madara::knowledge::containers::Vector::*)(
          size_t index,
          const madara::knowledge::KnowledgeRecord & value
        )
      > (&madara::knowledge::containers::Vector::set),
      "Sets an index in the vector to a KnowledgeRecord")

    .def ("set",
      static_cast<
        int
        (madara::knowledge::containers::Vector::*)(
          size_t index,
          const madara::knowledge::KnowledgeRecord & value,
          const madara::knowledge::KnowledgeUpdateSettings & settings
        )
      > (&madara::knowledge::containers::Vector::set),
      "Sets an index in the vector to a KnowlegeRecord")

    .def ("set",
      static_cast<
        int
        (madara::knowledge::containers::Vector::*)(
          size_t index,
          const std::vector <double> & value
        )
      > (&madara::knowledge::containers::Vector::set),
      "Sets an index in the vector")

    .def ("set",
      static_cast<
        int
        (madara::knowledge::containers::Vector::*)(
          size_t index,
          const std::vector <double> & value,
          const madara::knowledge::KnowledgeUpdateSettings & settings
        )
      > (&madara::knowledge::containers::Vector::set),
      "Sets an index in the vector")

    .def ("set",
      static_cast<
        int
        (madara::knowledge::containers::Vector::*)(
          size_t index,
          const std::vector <int64_t> & value
        )
      > (&madara::knowledge::containers::Vector::set),
      "Sets an integer array at an index in the vector")

    .def ("set",
      static_cast<
        int
        (madara::knowledge::containers::Vector::*)(
          size_t index,
          const std::vector <int64_t> & value,
          const madara::knowledge::KnowledgeUpdateSettings & settings
        )
      > (&madara::knowledge::containers::Vector::set),
      "Sets an integer array at an index in the vector")

      .def("__getitem__", &madara::knowledge::containers::Vector::operator[],
      "Gets an index of the vector")
  ;

}


#endif // _MADARA_PYTHON_PORT_MADARA_KNOWLEDGE_CONTAINERS_H_

