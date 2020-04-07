#ifndef _MADARA_PYTHON_PORT_MADARA_KNOWLEGE_CPP_
#define _MADARA_PYTHON_PORT_MADARA_KNOWLEGE_CPP_

#include <boost/python/detail/wrap_python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <boost/python/suite/indexing/map_indexing_suite.hpp>
#include <boost/python/dict.hpp>
#include <boost/python/import.hpp>
#include <boost/python/enum.hpp>

#include "madara/knowledge/KnowledgeBase.h"
#include "madara/knowledge/FileFragmenter.h"
#include "madara/knowledge/FileRequester.h"
#include "madara/knowledge/CheckpointPlayer.h"
#include "madara/filters/GenericFilters.h"
#include "FunctionDefaults.h"
#include "MadaraKnowledgeContainers.h"
#include "MadaraKnowledge.h"

/**
 * @file MadaraModule.cpp
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains Boost.Python mappings for the madara.knowledge module
 **/

using namespace boost::python;

class knowledge_NS
{
};

template<typename T>
struct DontDestruct
{
  union {
    T val;
  };

  DontDestruct(T val) : val(std::move(val)){};
  ~DontDestruct() {}

  DontDestruct(const DontDestruct& o) : val(o.val) {}
  DontDestruct(DontDestruct&& o) : val(std::move(o.val)) {}

  DontDestruct& operator=(const DontDestruct& o)
  {
    val = o.val;
    return *this;
  }

  DontDestruct& operator=(DontDestruct&& o)
  {
    val = std::move(o.val);
    return *this;
  }

  T* operator->()
  {
    return &val;
  }
  const T* operator->() const
  {
    return &val;
  }
  T& operator*()
  {
    return val;
  }
  const T& operator*() const
  {
    return val;
  }
};

void define_knowledge(void)
{
  object ke = object(handle<>(PyModule_New("madara.knowledge")));

  ke.attr("__file__") = "<synthetic>";
  scope().attr("knowledge") = ke;
  ke.attr("__doc__") = "Provides access to the knowledge engine";

  // this was the missing piece: sys.modules['modA.modB']=modB
  extract<dict>(getattr(import("sys"), "modules"))()["madara.knowledge"] = ke;

  scope knowledge = ke;

  /*
     scope knowledge
      = class_ <knowledge_NS, boost::noncopyable> (
        "knowledge_engine", no_init);
     */

  using namespace madara::knowledge;
  using namespace madara::exceptions;

  class_<madara::knowledge::CheckpointSettings>(
      "CheckpointSettings", "Settings for checkpoints and file saves", init<>())

      // class members
      .def_readwrite("buffer_filters",
          &madara::knowledge::CheckpointSettings::buffer_filters,
          "buffer filters. Note that the user must clean up memory of all "
          "filters")

      .def_readwrite("buffer_size",
          &madara::knowledge::CheckpointSettings::buffer_size,
          "size of the buffer needed to hold file in memory")

      .def_readwrite("clear_knowledge",
          &madara::knowledge::CheckpointSettings::clear_knowledge,
          "If true, during loads, clear the KnowledgeBase first")

      .def_readwrite("filename",
          &madara::knowledge::CheckpointSettings::filename,
          "the path and name of the file to load/save")

      .def_readwrite("ignore_header_check",
          &madara::knowledge::CheckpointSettings::ignore_header_check,
          "If true, do not perform a header check. This is useful if you are"
          "loading an arbitrary text file with no buffer filters")

      .def_readwrite("initial_lamport_clock",
          &madara::knowledge::CheckpointSettings::initial_lamport_clock,
          "initial lamport clock saved in the checkpoint ")

      .def_readwrite("initial_state",
          &madara::knowledge::CheckpointSettings::initial_state,
          "initial state of interest (useful for loading ranges of "
          "checkpoints). "
          " Thsi is an inclusive identifier, so 0 means to load from initial"
          "context/checkpoint save")

      .def_readwrite("initial_timestamp",
          &madara::knowledge::CheckpointSettings::initial_timestamp,
          "initial wallclock time saved in the checkpoint")

      .def_readwrite("keep_open",
          &madara::knowledge::CheckpointSettings::keep_open,
          "attempt to keep the file open for later usage (not really "
          "recommended)")

      .def_readwrite("last_lamport_clock",
          &madara::knowledge::CheckpointSettings::last_lamport_clock,
          "final lamport clock in the checkpoint")

      .def_readwrite("last_state",
          &madara::knowledge::CheckpointSettings::last_state,
          "the last state of interest (useful for loading ranges of "
          "checkpoints). "
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

      .def_readwrite("playback_simtime",
          &madara::knowledge::CheckpointSettings::playback_simtime,
          "If true, update simtime during playback to match recorded TOI. Only"
          "operable if MADARA_FEATURE_SIMTIME macro is defined")

      .def_readwrite("prefixes",
          &madara::knowledge::CheckpointSettings::prefixes,
          "A list of prefixes to save/load. If empty, all prefixes are valid")

      .def_readwrite("reset_checkpoint",
          &madara::knowledge::CheckpointSettings::reset_checkpoint,
          "if true, resets the checkpoint to start a new diff from this point "
          "on")

      .def_readwrite("states", &madara::knowledge::CheckpointSettings::states,
          "the number of states checkpointed in the file stream")

      .def_readwrite("variables_lister",
          &madara::knowledge::CheckpointSettings::variables_lister,
          "Object which will be used to extract variables for checkpoint "
          "saving."
          "By default (if left nullptr), use a default implementation which "
          "uses"
          "ThreadSaveContext::get_local_modified")

      .def_readwrite("version", &madara::knowledge::CheckpointSettings::version,
          "the MADARA version used when the checkpoint was saved");

  // Holds a settings object along with a reader referencing it.
  struct PyCheckpointReader
  {
    PyCheckpointReader(CheckpointSettings settings)
      : settings_(std::make_shared<CheckpointSettings>(std::move(settings))),
        reader_(std::make_shared<CheckpointReader>(*settings_))
    {
    }

    std::shared_ptr<CheckpointSettings> settings_;
    std::shared_ptr<CheckpointReader> reader_;
  };

  class_<PyCheckpointReader>("CheckpointReader",
      "Utility class for iterating checkpoint files",
      init<CheckpointSettings>())
      .def("start", +[](PyCheckpointReader& r) { r.reader_->start(); },
          "Begin by reading any header information. Optional. Will be called "
          "automatically by next if need be. This opens the file specified in "
          "the settings based during construction. The file will be closed "
          "when this object is destructed. This object is usable as an "
          "iterator.")
      .def("__iter__", +[](PyCheckpointReader& r) { return r; })
      .def("next",
          +[](PyCheckpointReader& r) {
            auto pair = r.reader_->next();
            if (pair.first.empty())
            {
              PyErr_SetNone(PyExc_StopIteration);
              throw_error_already_set();
              return boost::python::make_tuple();
            }
            return boost::python::make_tuple(pair.first, pair.second);
          },
          "Get the next update from the checkpoint file. Throws StopIteration "
          "if the end is reached.")
      .def("settings",
          +[](PyCheckpointReader& r) -> CheckpointSettings {
            return *r.settings_;
          },
          "Get the CheckpointSettings object this reader is using.");

  class_<madara::knowledge::FileFragmenter>("FileFragmenter",
      "Splits or reforms a file from disk or knowledge base", init<>())

      // create a vector in the KB
      .def("create_vector", &madara::knowledge::FileFragmenter::create_vector,
          m_create_vector_2_of_3(args("key", "kb", "settings"),
              "Creates a Vector in the KB using current file fragments"))

      // split a file
      .def("fragment_buffer",
          &madara::knowledge::FileFragmenter::fragment_buffer,
          m_fragment_buffer_2_of_3(args("buffer", "size", "settings"),
              "Splits a file, given a buffer and size with settings"))

      // split a file, given a filename
      .def("fragment_file", &madara::knowledge::FileFragmenter::fragment_file,
          m_fragment_file_1_of_2(args("filename", "settings"),
              "Splits a file, given a filename and settings"))

      // evaluate an expression
      .def("from_kb", &madara::knowledge::FileFragmenter::from_kb,
          m_from_kb_2_of_3(args("key", "kb", "settings"),
              "Attempts to create a file from a Vector of fragments. The "
              "result is the actual number of bytes available. The result "
              "should be checked against the actual size of the file for "
              "progress information (e.g. of transfer)."))

      .add_property("file_contents",
          &madara::knowledge::FileFragmenter::get_file_contents,
          "Returns the merged file contents (should run from_kb first)")

      // class members

      .def_readonly("file_size", &madara::knowledge::FileFragmenter::file_size,
          "size of file contents")

      ;

  class_<madara::knowledge::KnowledgeRecord>(
      "KnowledgeRecord", "Basic unit of knowledge", init<>())

      // integer constructor
      .def(init<int64_t>())

      .def(init<double>())

      .def(init<std::vector<int64_t>>())

      .def(init<std::vector<double>>())

      .def(init<std::string>())

      .def(init<const madara::knowledge::KnowledgeRecord&>())

      // apply knowledge record to a context
      .def("apply", &madara::knowledge::KnowledgeRecord::apply,
          "Apply the knowledge record to a context, given some quality and "
          "clock")

      // clears the value to a 0 integer
      .def("clear_value", &madara::knowledge::KnowledgeRecord::clear_value,
          "Sets the value to 0 and type to integer")

      // decrements an index of an array
      .def("dec_index", &madara::knowledge::KnowledgeRecord::dec_index,
          "Decrements an array element at a particular index")

      // returns true if record exists
      .def("exists", &madara::knowledge::KnowledgeRecord::exists,
          "Returns whether the knowledge has been set/modified/created")

      // fragments the record
      .def("fragment", &madara::knowledge::KnowledgeRecord::fragment,
          "Fragments the record into components")

      // gets the double precision
      .def("get_precision", &madara::knowledge::KnowledgeRecord::get_precision,
          "Gets the double precision used in to_string")

      // increments an index of an array
      .def("inc_index", &madara::knowledge::KnowledgeRecord::inc_index,
          "Increments an array element at a particular index")

      // checks if record is array type
      .def("is_array_type",
          static_cast<bool (madara::knowledge::KnowledgeRecord::*)(void) const>(
              &madara::knowledge::KnowledgeRecord::is_array_type),
          "returns if the record is array type")

      // checks if record is a binary file type
      .def("is_binary_file_type",
          static_cast<bool (madara::knowledge::KnowledgeRecord::*)(void) const>(
              &madara::knowledge::KnowledgeRecord::is_binary_file_type),
          "Checks if the record is a binary file type")

      // checks if record is a double type
      .def("is_double_type",
          static_cast<bool (madara::knowledge::KnowledgeRecord::*)(void) const>(
              &madara::knowledge::KnowledgeRecord::is_double_type),
          "Checks if the record is a double type")

      // checks if the record is false
      .def("is_false", &madara::knowledge::KnowledgeRecord::is_false,
          "Checks if the record is false")

      // checks if record is a file type
      .def("is_file_type",
          static_cast<bool (madara::knowledge::KnowledgeRecord::*)(void) const>(
              &madara::knowledge::KnowledgeRecord::is_file_type),
          "Checks if the record is a file type")

      // checks if the record is an image type
      .def("is_image_type",
          static_cast<bool (madara::knowledge::KnowledgeRecord::*)(void) const>(
              &madara::knowledge::KnowledgeRecord::is_image_type),
          "Checks if the record is an image type")

      // checks if the record is an integer type
      .def("is_integer_type",
          static_cast<bool (madara::knowledge::KnowledgeRecord::*)(void) const>(
              &madara::knowledge::KnowledgeRecord::is_integer_type),
          "Checks if the record is an integer type")

      // checks if the record is a string type
      .def("is_string_type",
          static_cast<bool (madara::knowledge::KnowledgeRecord::*)(void) const>(
              &madara::knowledge::KnowledgeRecord::is_string_type),
          "Checks if the record is a string type")

      // checks if the record is true
      .def("is_true", &madara::knowledge::KnowledgeRecord::is_true,
          "Checks if the record is true")

      // sets the contents of the record as a file
      .def("read_file", &madara::knowledge::KnowledgeRecord::read_file,
          m_read_file_1_of_2(args("filename", "read_as_type"),
              "Reads the contents of a file into the record. Type "
              "is an optional field that can force a type for reading into."))

      // reset the record to UNCREATED
      .def("reset_value", &madara::knowledge::KnowledgeRecord::reset_value,
          "Resets the record to an UNCREATED status (faster than clear_value)")

      // resize an array
      .def("resize", &madara::knowledge::KnowledgeRecord::resize,
          "Resizes an array to a new size")

      // retrieves an index of an array
      .def("retrieve_index",
          &madara::knowledge::KnowledgeRecord::retrieve_index,
          "Retrieves an array element at a particular index")

      // sets a knowledge record to a double
      .def("set",
          static_cast<void (madara::knowledge::KnowledgeRecord::*)(double)>(
              &madara::knowledge::KnowledgeRecord::set_value),
          "Sets the value to a double")

      // sets a knowledge record to an array of doubles
      .def("set",
          static_cast<void (madara::knowledge::KnowledgeRecord::*)(
              const std::vector<double>&)>(
              &madara::knowledge::KnowledgeRecord::set_value),
          "Sets the value to an array of doubles")

      // sets a knowledge record to an integer
      .def("set",
          static_cast<void (madara::knowledge::KnowledgeRecord::*)(int)>(
              &madara::knowledge::KnowledgeRecord::set_value),
          "Sets the value to an integer")

      // sets a knowledge record to an array of integer
      .def("set",
          static_cast<void (madara::knowledge::KnowledgeRecord::*)(
              const std::vector<madara::knowledge::KnowledgeRecord::Integer>&)>(
              &madara::knowledge::KnowledgeRecord::set_value),
          "Sets the value to an array of integers")

      // sets a knowledge record to a string
      .def("set",
          static_cast<void (madara::knowledge::KnowledgeRecord::*)(
              const std::string&)>(
              &madara::knowledge::KnowledgeRecord::set_value),
          "Sets the value to a string")

      // sets the contents of the record to a file
      .def("set_file",
          static_cast<void (madara::knowledge::KnowledgeRecord::*)(
              const unsigned char* new_value, size_t size)>(
              &madara::knowledge::KnowledgeRecord::set_file),
          "Sets the value to a file's contents")

      // sets fixed precision
      .def("set_fixed", &madara::knowledge::KnowledgeRecord::set_fixed,
          "Set the output format for doubles to std::fixed for madara logging")

      // sets an array index to an integer
      .def("set_index",
          static_cast<void (madara::knowledge::KnowledgeRecord::*)(
              size_t, madara::knowledge::KnowledgeRecord::Integer)>(
              &madara::knowledge::KnowledgeRecord::set_index),
          "Sets an array index to an integer")

      // sets an array index to a double
      .def("set_index",
          static_cast<void (madara::knowledge::KnowledgeRecord::*)(
              size_t, double)>(&madara::knowledge::KnowledgeRecord::set_index),
          "Sets an array index to a double (and converts the array to double)")

      // sets the contents of the record to a jpeg
      .def("set_jpeg",
          static_cast<void (madara::knowledge::KnowledgeRecord::*)(
              const unsigned char* new_value, size_t size)>(
              &madara::knowledge::KnowledgeRecord::set_jpeg),
          "Sets the value to a jpeg")

      // sets the double precision
      .def("set_precision", &madara::knowledge::KnowledgeRecord::set_precision,
          "Sets the double precision, generally for to_string")

      // sets scientific precision
      .def("set_scientific",
          &madara::knowledge::KnowledgeRecord::set_scientific,
          "Sets the output format for doubles to std::scientific")

      // sets the value and meta data from another knowledge record
      .def("set_full",
          static_cast<void (madara::knowledge::KnowledgeRecord::*)(
              const madara::knowledge::KnowledgeRecord&)>(
              &madara::knowledge::KnowledgeRecord::set_full),
          "Sets the value and meta data from another KnowledgeRecord")

      // sets value to an array of ints
      .def("set_value",
          static_cast<void (madara::knowledge::KnowledgeRecord::*)(
              const madara::knowledge::KnowledgeRecord::Integer* new_value,
              uint32_t)>(&madara::knowledge::KnowledgeRecord::set_value),
          "Sets the value to an array of integers")

      // sets value to an array of ints, no copying
      .def("set_value",
          static_cast<void (madara::knowledge::KnowledgeRecord::*)(
              const std::vector<madara::knowledge::KnowledgeRecord::Integer>&)>(
              &madara::knowledge::KnowledgeRecord::set_value),
          "Sets the value to an array of integers, without copying")

      // sets value to a string
      .def("set_value",
          static_cast<void (madara::knowledge::KnowledgeRecord::*)(
              const char* new_value, uint32_t)>(
              &madara::knowledge::KnowledgeRecord::set_value),
          "Sets the value to a string, from a buffer")

      // sets value to a string
      .def("set_value",
          static_cast<void (madara::knowledge::KnowledgeRecord::*)(
              const std::string&)>(
              &madara::knowledge::KnowledgeRecord::set_value),
          "Sets the value to a string")

      // sets value from another knowledge record
      .def("set_value",
          static_cast<void (madara::knowledge::KnowledgeRecord::*)(
              const madara::knowledge::KnowledgeRecord&)>(
              &madara::knowledge::KnowledgeRecord::set_value),
          "Sets the value from another KnowledgeRecord,"
          "does not copy toi, clock, and write_quality")

      // sets value to xml string
      .def("set_xml",
          static_cast<void (madara::knowledge::KnowledgeRecord::*)(
              const char* new_value, size_t)>(
              &madara::knowledge::KnowledgeRecord::set_xml),
          "sets the value to an xml string")

      // sets value to xml string
      .def("set_xml",
          static_cast<void (madara::knowledge::KnowledgeRecord::*)(
              const std::string&)>(
              &madara::knowledge::KnowledgeRecord::set_xml),
          "sets the value to an xml string")

      // sets the value to a plaintext string
      .def("set_text",
          static_cast<void (madara::knowledge::KnowledgeRecord::*)(
              const char* new_value, size_t)>(
              &madara::knowledge::KnowledgeRecord::set_text),
          "sets the value to a plaintext string")

      // sets the value to a plaintext string
      .def("set_text",
          static_cast<void (madara::knowledge::KnowledgeRecord::*)(
              const std::string&)>(
              &madara::knowledge::KnowledgeRecord::set_text),
          "sets the value to a plaintext string")

      // share binary
      .def("share_binary", &madara::knowledge::KnowledgeRecord::share_binary,
          "@return a shared_ptr, sharing with the internal one."
          "If this record is not a binary file value, returns NULL shared_ptr")

      // share doubles
      .def("share_doubles", &madara::knowledge::KnowledgeRecord::share_doubles,
          "@return a shared_ptr, sharing with the internal one."
          "If this record is not a doubles array, returns NULL shared_ptr")

      // share strings
      .def("share_string", &madara::knowledge::KnowledgeRecord::share_string,
          "@return a shared_ptr, sharing with the internal one."
          "If this record is not a string, returns NULL shared_ptr")

      // share integers
      .def("share_integers",
          &madara::knowledge::KnowledgeRecord::share_integers,
          "@return a shared_ptr, sharing with the internal one."
          "If this record is not an int array, returns NULL shared_ptr")

      // sets the contents of the record to a jpeg
      .def("size", &madara::knowledge::KnowledgeRecord::size,
          "Returns the size of the value")

      // show record information
      .def("status", &madara::knowledge::KnowledgeRecord::status,
          "Returns the status of the record")

      // convert to a double
      .def("to_double", &madara::knowledge::KnowledgeRecord::to_double,
          "Converts the record to a double")

      // convert to a std::vector of doubles
      .def("to_doubles", &madara::knowledge::KnowledgeRecord::to_doubles,
          "Converts the record to an array of doubles")

      // save value to a file
      .def("to_file", &madara::knowledge::KnowledgeRecord::to_file,
          "Saves the value of the record to a file")

      // convert to an integer
      .def("to_integer", &madara::knowledge::KnowledgeRecord::to_integer,
          "Converts the record to an integer")

      // convert to a std::vector of integers
      .def("to_integers", &madara::knowledge::KnowledgeRecord::to_integers,
          "Converts the record to an array of integers")

      // convert to a string
      .def("to_string", &madara::knowledge::KnowledgeRecord::to_string,
          m_to_string_0_of_1(
              args("delimiter"), "Converts the record to a string"))

      // gets the type of the record
      .def("type", &madara::knowledge::KnowledgeRecord::type,
          "Returns the value type")

      // unshare knowledge record pointer
      .def("unshare", &madara::knowledge::KnowledgeRecord::unshare,
          "If this record holds a shared_ptr, make a copy of the underlying"
          "value so it has an exclusive copy")

      // overloaded operators
      .def(self < self)
      .def(self <= self)
      .def(self == double())
      .def(self == std::string())
      .def(self == self)
      .def(self != self)
      .def(self > self)
      .def(self >= self)
      .def(!self)
      .def(-self)
      .def(self += self)
      .def(self -= self)
      .def(self *= self)
      .def(self /= self)
      .def(self %= self)
      .def(self * self)
      .def(self / self)
      .def(self % self)
      .def(self + self)
      .def(self - self)

      // class members
      .def_readwrite("clock", &madara::knowledge::KnowledgeRecord::clock,
          "last modification lamport clock")

      .def_readwrite("quality", &madara::knowledge::KnowledgeRecord::quality,
          "write priority for any local updates")

      .def("toi", &madara::knowledge::KnowledgeRecord::toi,
          "last modification system clock time (time of insertion)")

      .def_readwrite("write_quality",
          &madara::knowledge::KnowledgeRecord::write_quality,
          "priority of the update")

      // because of the templated nature of KnowledgeRecord::operator=, I could
      // not figure this out .def ("operator=",
      // static_cast<madara::knowledge::KnowledgeRecord (
      // madara::knowledge::KnowledgeRecord::*)(
      // int &&)> (
      // &madara::knowledge::KnowledgeRecord::operator=),
      // "Assigns an int to the record",
      // return_value_policy<reference_existing_object> ())

      .def("operator==",
          static_cast<bool (madara::knowledge::KnowledgeRecord::*)(
              const madara::knowledge::KnowledgeRecord&) const>(
              &madara::knowledge::KnowledgeRecord::operator==),
          "Compares two records for equality")

      .def("operator<=",
          static_cast<bool (madara::knowledge::KnowledgeRecord::*)(
              const madara::knowledge::KnowledgeRecord&) const>(
              &madara::knowledge::KnowledgeRecord::operator<=),
          "Compares two records with less than or equal to")

      .def("operator>=",
          static_cast<bool (madara::knowledge::KnowledgeRecord::*)(
              const madara::knowledge::KnowledgeRecord&) const>(
              &madara::knowledge::KnowledgeRecord::operator>=),
          "Compares two records with greater than or equal to")

      .def("operator>",
          static_cast<bool (madara::knowledge::KnowledgeRecord::*)(
              const madara::knowledge::KnowledgeRecord&) const>(
              &madara::knowledge::KnowledgeRecord::operator>),
          "Compares two records with greater than")

      .def("operator<",
          static_cast<bool (madara::knowledge::KnowledgeRecord::*)(
              const madara::knowledge::KnowledgeRecord&) const>(
              &madara::knowledge::KnowledgeRecord::operator<),
          "Compares two records with less than")

      .def("operator++", &madara::knowledge::KnowledgeRecord::operator++,
          "Adds one to the record",
          return_value_policy<reference_existing_object>())

      .def("operator--", &madara::knowledge::KnowledgeRecord::operator--,
          "Subtracts one from the record",
          return_value_policy<reference_existing_object>())

      .staticmethod("get_precision")
      .staticmethod("set_fixed")
      .staticmethod("set_precision")
      .staticmethod("set_scientific")

      ;  // end of KnowledgeRecord

  // the types of values supported in KnowledgeRecord
  enum_<madara::knowledge::KnowledgeRecord::ValueTypes>("ValueTypes")
      .value("EMPTY", madara::knowledge::KnowledgeRecord::EMPTY)
      .value("INTEGER", madara::knowledge::KnowledgeRecord::INTEGER)
      .value("STRING", madara::knowledge::KnowledgeRecord::STRING)
      .value("DOUBLE", madara::knowledge::KnowledgeRecord::DOUBLE)
      .value("FLOAT", madara::knowledge::KnowledgeRecord::FLOAT)
      .value("UNKNOWN_FILE_TYPE",
          madara::knowledge::KnowledgeRecord::UNKNOWN_FILE_TYPE)
      .value("XML", madara::knowledge::KnowledgeRecord::XML)
      .value("TEXT_FILE", madara::knowledge::KnowledgeRecord::TEXT_FILE)
      .value("DOUBLE_ARRAY", madara::knowledge::KnowledgeRecord::DOUBLE_ARRAY)
      .value("INTEGER_ARRAY", madara::knowledge::KnowledgeRecord::INTEGER_ARRAY)
      .value("IMAGE_JPEG", madara::knowledge::KnowledgeRecord::IMAGE_JPEG)
      .value("ALL_ARRAYS", madara::knowledge::KnowledgeRecord::ALL_ARRAYS)
      .value("ALL_INTEGERS", madara::knowledge::KnowledgeRecord::ALL_INTEGERS)
      .value("ALL_DOUBLES", madara::knowledge::KnowledgeRecord::ALL_DOUBLES)
      .value("ALL_PRIMITIVE_TYPES",
          madara::knowledge::KnowledgeRecord::ALL_PRIMITIVE_TYPES)
      .value(
          "ALL_FILE_TYPES", madara::knowledge::KnowledgeRecord::ALL_FILE_TYPES)
      .value("ALL_IMAGES", madara::knowledge::KnowledgeRecord::ALL_IMAGES)
      .value("ALL_TEXT_FORMATS",
          madara::knowledge::KnowledgeRecord::ALL_TEXT_FORMATS)
      .value("ALL_TYPES", madara::knowledge::KnowledgeRecord::ALL_TYPES)
      .value("ALL_CLEARABLES",
          madara::knowledge::KnowledgeRecord::ALL_CLEARABLES)

      ;  // end of ValueTypes

  // Boost.python does not appear to support this type of function
  //.def ("to_unmanaged_buffer",
  //&madara::knowledge::KnowledgeRecord::to_unmanaged_buffer)

  /********************************************************
   * KnowledgeReferenceSettings definitions
   ********************************************************/

  class_<madara::knowledge::KnowledgeReferenceSettings>(
      "KnowledgeReferenceSettings",
      "Settings for referencing knowledge records", init<>())

      .def(init<bool>())
      .def(init<const madara::knowledge::KnowledgeReferenceSettings&>())

      // define readwrite variables within the class
      .def_readwrite("expand_variables",
          &madara::knowledge::KnowledgeReferenceSettings::expand_variables,
          "Always attempt to expand variable names")
      .def_readwrite("never_exit",
          &madara::knowledge::KnowledgeReferenceSettings::never_exit,
          "Do not exit, even if in invalid states")

      ;  // end class KnowledgeReferenceSettings

  /********************************************************
   * KnowledgeUpdateSettings definitions
   ********************************************************/

  class_<madara::knowledge::KnowledgeUpdateSettings,
      bases<madara::knowledge::KnowledgeReferenceSettings>>(
      "KnowledgeUpdateSettings", "Settings for updating knowledge records",
      init<>())

      // other constructors here
      .def(init<const madara::knowledge::KnowledgeUpdateSettings&>())

      // define readwrite variables within the class
      .def_readwrite("treat_globals_as_locals",
          &madara::knowledge::KnowledgeUpdateSettings::treat_globals_as_locals,
          "Treat global variable changes as if they were local variables")
      .def_readwrite("signal_changes",
          &madara::knowledge::KnowledgeUpdateSettings::signal_changes,
          "Always signal on mutation (useful for multi-threaded applications)")
      .def_readwrite("always_overwrite",
          &madara::knowledge::KnowledgeUpdateSettings::always_overwrite,
          "Always overwrite records, regardless of quality or consistency "
          "checks")
      .def_readwrite("track_local_changes",
          &madara::knowledge::KnowledgeUpdateSettings::track_local_changes,
          "Track local changes for checkpointing purposes")
      .def_readwrite("clock_increment",
          &madara::knowledge::KnowledgeUpdateSettings::clock_increment,
          "Integer increment for updates to Lamport clocks")
      .def_readwrite("treat_locals_as_globals",
          &madara::knowledge::KnowledgeUpdateSettings::treat_locals_as_globals,
          "Toggle whether updates to local variables are treated as"
          "global variables that should be sent over the transport. It"
          "should be stressed that this is dangerous and should only"
          "be used for debugging. If you toggle this to true, all local"
          "variables will be sent over the network where they will"
          "overwrite local variables in remote systems, unless the"
          "remote system filters out the local variable changes with"
          "an on-receive filter")
      .def_readwrite("stream_changes",
          &madara::knowledge::KnowledgeUpdateSettings::stream_changes,
          "Toggle for streaming support. If this is true, all changes"
          "will be streamed to the attached streamer, if any");  // end class
                                                                 // KnowledgeUpdateSettings

  /********************************************************
   * EvalSettings definitions
   ********************************************************/

  class_<madara::knowledge::EvalSettings,
      bases<madara::knowledge::KnowledgeUpdateSettings>>(
      "EvalSettings", "Settings for evaluate calls", init<>())

      // other constructors here
      .def(init<const madara::knowledge::EvalSettings&>())

      // define readwrite variables within the class
      .def_readwrite("delay_sending_modifieds",
          &madara::knowledge::EvalSettings::delay_sending_modifieds,
          "Do not send modifications after an evaluation")
      .def_readwrite("pre_print_statement",
          &madara::knowledge::EvalSettings::pre_print_statement,
          "Statement to atomically expand and print before an evaluate")
      .def_readwrite("post_print_statement",
          &madara::knowledge::EvalSettings::post_print_statement,
          "Statement to atomically expand and print after an evaluate")
      .def_readwrite("send_list", &madara::knowledge::EvalSettings::send_list,
          "List of variables that are allowed to be sent, if changed")

      ;  // end class EvalSettings

  /********************************************************
   * WaitSettings definitions
   ********************************************************/

  class_<madara::knowledge::WaitSettings,
      bases<madara::knowledge::EvalSettings>>(
      "WaitSettings", "Settings for wait calls", init<>())

      // other constructors here
      .def(init<const madara::knowledge::WaitSettings&>())

      // define readwrite variables within the class
      .def_readwrite("poll_frequency",
          &madara::knowledge::WaitSettings::poll_frequency,
          "Frequency, in seconds, to evaluate the expression")
      .def_readwrite("max_wait_time",
          &madara::knowledge::WaitSettings::max_wait_time,
          "Maximum amount of time to wait for truth");  // end class
                                                        // WaitSettings

  /********************************************************
   * Variables definitions
   ********************************************************/

  class_<madara::knowledge::Variables>("Variables", init<>())

      // add list of current modified list
      .def("add_modifieds", &madara::knowledge::KnowledgeBase::add_modifieds,
          "Adds a list of VariableReferences to the current modified list")

      // modifies all global variables
      .def("apply_modified", &madara::knowledge::Variables::apply_modified,
          "Applies modified to all global variables")

      // compile prototype
      .def("compile", &madara::knowledge::Variables::compile,
          "Compiles a KaRL expression into an expression tree. Always do this"
          "before calling evaluate because it puts the expression into an"
          "optimized format. Best practice is to save the CompiledExpression"
          "in a global variable or in some kind of persistent store. Pair with"
          "expand_statement if you know that variable expansion is used but"
          "the variable values that are expanded never change (e.g. an id that"
          "is set through the command line and thus stays the same after it is"
          "initially set")

      // decrements the value of a variable
      .def("dec",
          static_cast<madara::knowledge::KnowledgeRecord (
              madara::knowledge::Variables::*)(const std::string&,
              const madara::knowledge::KnowledgeUpdateSettings&)>(
              &madara::knowledge::Variables::dec),
          "Atomically decrements the value of the variable")

      // decrements the value of a variable
      .def("dec",
          static_cast<madara::knowledge::KnowledgeRecord (
              madara::knowledge::Variables::*)(
              const madara::knowledge::VariableReference&,
              const madara::knowledge::KnowledgeUpdateSettings&)>(
              &madara::knowledge::Variables::dec),
          "Atomically decrements the value of the variable")

      // evaluate an expression
      .def("evaluate",
          static_cast<madara::knowledge::KnowledgeRecord (
              madara::knowledge::Variables::*)(const std::string&,
              const madara::knowledge::KnowledgeUpdateSettings&)>(
              &madara::knowledge::Variables::evaluate),
          m_eval_1_of_2(
              args("expression", "settings"), "Evaluates an expression"))

      // evaluate an expression
      .def("evaluate",
          static_cast<madara::knowledge::KnowledgeRecord (
              madara::knowledge::Variables::*)(
              madara::knowledge::CompiledExpression&,
              const madara::knowledge::KnowledgeUpdateSettings&)>(
              &madara::knowledge::Variables::evaluate),
          m_eval_1_of_2(
              args("expression", "settings"), "Evaluates an expression"))

      // check to see if variable exists
      .def("exists",
          static_cast<bool (madara::knowledge::Variables::*)(const std::string&,
              const madara::knowledge::KnowledgeReferenceSettings&) const>(
              &madara::knowledge::Variables::exists),
          "Checks if a knowledge location exists in the context")

      // check to see if variable exists
      .def("exists",
          static_cast<bool (madara::knowledge::Variables::*)(
              const madara::knowledge::VariableReference&,
              const madara::knowledge::KnowledgeReferenceSettings&) const>(
              &madara::knowledge::Variables::exists),
          "Checks if a knowledge variable exists in the context")

      // prints all knowledge variables
      .def("expand_statement", &madara::knowledge::Variables::expand_statement,
          "Expand a statement")

      // get a knowledge record
      .def("get",
          static_cast<madara::knowledge::KnowledgeRecord (
              madara::knowledge::Variables::*)(const std::string&,
              const madara::knowledge::KnowledgeReferenceSettings&)>(
              &madara::knowledge::Variables::get),
          m_get_1_of_2(
              args("key", "settings"), "Retrieves the value of a variable"))

      // get a knowledge record
      .def("get",
          static_cast<madara::knowledge::KnowledgeRecord (
              madara::knowledge::Variables::*)(
              const madara::knowledge::VariableReference&,
              const madara::knowledge::KnowledgeReferenceSettings&)>(
              &madara::knowledge::Variables::get),
          m_get_1_of_2(args("variable", "settings"),
              "Retrieves the value of a variable"))

      // converts to a string
      .def("get_ref", &madara::knowledge::Variables::get_ref,
          "Retrieves the value of a variable")

      // get matches in an iterable form
      .def("get_matches", &madara::knowledge::Variables::get_matches,
          "Creates an iteration of VariableReferences to all keys matching"
          "the prefix and suffix")

      // increments the value of a variable
      .def("inc",
          static_cast<madara::knowledge::KnowledgeRecord (
              madara::knowledge::Variables::*)(const std::string&,
              const madara::knowledge::KnowledgeUpdateSettings&)>(
              &madara::knowledge::Variables::inc),
          "Atomically increments the value of the variable")

      // increments the value of a variable
      .def("inc",
          static_cast<madara::knowledge::KnowledgeRecord (
              madara::knowledge::Variables::*)(
              const madara::knowledge::VariableReference&,
              const madara::knowledge::KnowledgeUpdateSettings&)>(
              &madara::knowledge::Variables::inc),
          "Atomically increments the value of the variable")

      // Loads the context from a file
      .def("load_context", &madara::knowledge::Variables::load_context,
          "Loads the context from a file")

      // print statement
      .def("print",
          static_cast<void (madara::knowledge::Variables::*)(unsigned int)
                  const>(&madara::knowledge::Variables::print),
          "Prints all knowledge in the context")

      // print statement
      .def("print",
          static_cast<void (madara::knowledge::Variables::*)(const std::string&,
              unsigned int) const>(&madara::knowledge::Variables::print),
          "Prints a statement")

      // get a knowledge record at an index
      .def("retrieve_index",
          static_cast<madara::knowledge::KnowledgeRecord (
              madara::knowledge::Variables::*)(const std::string&, size_t,
              const madara::knowledge::KnowledgeReferenceSettings&)>(
              &madara::knowledge::Variables::retrieve_index),
          m_retrieve_index_2_of_3(args("key", "index", "settings"),
              "Retrieves a knowledge record from an index"))

      // get a knowledge record at an index
      .def("retrieve_index",
          static_cast<madara::knowledge::KnowledgeRecord (
              madara::knowledge::Variables::*)(const VariableReference&, size_t,
              const madara::knowledge::KnowledgeReferenceSettings&)>(
              &madara::knowledge::Variables::retrieve_index),
          m_retrieve_index_2_of_3(args("key", "index", "settings"),
              "Retrieves a knowledge record from an index"))

      // save context file to karl
      .def("save_as_karl", &madara::knowledge::Variables::save_as_karl,
          "Saves the context to a file as karl assignments, rather than binary")

      // save a checkpoint to a file
      .def("save_checkpoint", &madara::knowledge::Variables::save_checkpoint,
          "Saves a checkpoint of a list of changes to a file")

      // save context to a file
      .def("save_context", &madara::knowledge::Variables::save_context,
          "Saves the context to a file")

      // sets a knowledge record to a double
      .def("set",
          static_cast<int (madara::knowledge::Variables::*)(const std::string&,
              double, const madara::knowledge::KnowledgeUpdateSettings&)>(
              &madara::knowledge::Variables::set),
          "sets a knowledge record to a double")

      // sets a knowledge record to an array of doubles
      .def("set",
          static_cast<int (madara::knowledge::Variables::*)(const std::string&,
              const std::vector<double>&,
              const madara::knowledge::KnowledgeUpdateSettings&)>(
              &madara::knowledge::Variables::set),
          "sets a knowledge record to an array of doubles")

      // sets a knowledge record to an integer
      .def("set",
          static_cast<int (madara::knowledge::Variables::*)(const std::string&,
              madara::knowledge::KnowledgeRecord::Integer,
              const madara::knowledge::KnowledgeUpdateSettings&)>(
              &madara::knowledge::Variables::set),
          "sets a knowledge record to a integer")

      // sets a knowledge record to an array of integer
      .def("set",
          static_cast<int (madara::knowledge::Variables::*)(const std::string&,
              const std::vector<madara::knowledge::KnowledgeRecord::Integer>&,
              const madara::knowledge::KnowledgeUpdateSettings&)>(
              &madara::knowledge::Variables::set),
          "sets a knowledge record to an array of integers")

      // sets a knowledge record to a string
      .def("set",
          static_cast<int (madara::knowledge::Variables::*)(const std::string&,
              const std::string&,
              const madara::knowledge::KnowledgeUpdateSettings&)>(
              &madara::knowledge::Variables::set),
          "sets a knowledge record to a string")

      // fill a variable map with knowledge records
      .def("to_map", &madara::knowledge::Variables::to_map,
          "Fills a variable map with Knowledge Records that match an "
          "expression."
          "At the moment, this expression must be of the form 'subject'")

      // converts to a string
      .def("to_string", &madara::knowledge::Variables::to_string,
          "Converts to string")

      // fill a vector with knowledge records
      .def("to_vector", &madara::knowledge::Variables::to_vector,
          "Fills a vector with Knowledge Records that begin with a common "
          "subject"
          "and have a finite range of integer values")

      // write a file to a location
      .def("write_file", &madara::knowledge::Variables::write_file,
          "Write a file from the context to a specified location")

      ;

  /********************************************************
   * FunctionArguments definitions
   ********************************************************/

  class_<std::vector<madara::knowledge::FunctionArguments>>(
      "FunctionArguments", "List of arguments to a function");

  class_<madara::knowledge::FileRequester>("FileRequester",
      "Reconstructs files and requests fragments if incomplete", init<>())

      // builds a fragment request vector
      .def("build_fragment_request",
          &madara::knowledge::FileRequester::build_fragment_request,
          "Builds an arry of the request that is necessary under "
          "the current max_fragments")

      // clears the fragments on the file system
      .def("clear_fragments",
          &madara::knowledge::FileRequester::clear_fragments,
          "Clears any lingering fragments on the file system.")

      // gets the crc
      .def("get_crc", &madara::knowledge::FileRequester::get_crc,
          "Retrieves the file CRC from the KB")

      // gets the filename
      .def("get_filename", &madara::knowledge::FileRequester::get_filename,
          "Returns the name of the file being reconstructed")

      // gets the percentage of completion
      .def("get_percent_complete",
          &madara::knowledge::FileRequester::get_percent_complete,
          "Returns the percentage of completion")

      // gets the size
      .def("get_size", &madara::knowledge::FileRequester::get_size,
          "Retrieves the file size from the KB")

      // initializes the object
      // .def ("init",
      //   &madara::knowledge::FileRequester::init,
      //   m_init_4_of_5 (
      //     args("prefix", "sync_key", "filename", "kb",
      //     "max_fragment_requests"), "Initialize the requester object"))

      .def("init",
          static_cast<void (madara::knowledge::FileRequester::*)(
              const std::string&, const std::string&, const std::string&,
              madara::knowledge::KnowledgeBase, int)>(
              &madara::knowledge::FileRequester::init),
          m_init_4_of_5(args("prefix", "sync_key", "filename", "kb",
                            "max_fragment_requests"),
              "Initialize the requester object"))

      // modifies the file sync key
      .def("modify", &madara::knowledge::FileRequester::modify,
          "Modifies the current sync key to mark the request ready to resend")

      // checks if a request is needed and builds the request
      .def("needs_request", &madara::knowledge::FileRequester::needs_request,
          "Checks if a new request is necessary and builds the sync request. "
          "Note that the caller must call kb.send_modifieds to send the "
          "request.")

      .def_readwrite("max_fragments",
          &madara::knowledge::FileRequester::max_fragments,
          "the maximum fragments to attempt in a resend request")

      ;

  class_<madara::knowledge::KnowledgeBase>("KnowledgeBase",
      "Network-enabled, thread-safe knowledge context", init<>())

      // define constructors
      .def(init<const std::string&,
          const madara::transport::TransportSettings&>())

      // define constructors
      .def(init<const madara::knowledge::KnowledgeBase&>())

      // acquire recursive lock on KB
      .def("acquire", &madara::knowledge::KnowledgeBase::acquire,
          "Acquires the recursive lock on the knowledge base. This will"
          "block any other thread from updating or using the knowledge"
          "base until you call @ release")

      // starts the transport mechanism
      .def("activate_transport",
          &madara::knowledge::KnowledgeBase::activate_transport,
          "Starts the transport mechanism for dissemination if it is closed")

      // apply current time to all global variables
      .def("apply_modified", &madara::knowledge::KnowledgeBase::apply_modified,
          "Applies current time and modified to all global variables and tries"
          "to send them")

      // attach logger for printing
      .def("attach_logger", &madara::knowledge::KnowledgeBase::attach_logger,
          "Attaches a logger to be used for printing")

      // attach a transport to knowledge engine
      .def("attach_transport",
          static_cast<size_t (madara::knowledge::KnowledgeBase::*)(
              madara::transport::Base * transport)>(
              &madara::knowledge::KnowledgeBase::attach_transport),
          "Attaches a transport to the Knowledge Engine. Note that the"
          "transport should use the same ThreadSafeContext as the"
          "Knowledge Engine")

      // attach a transport to knowledge engine with specific setting
      .def("attach_transport",
          static_cast<size_t (madara::knowledge::KnowledgeBase::*)(
              const std::string&, madara::transport::TransportSettings&)>(
              &madara::knowledge::KnowledgeBase::attach_transport),
          "Adds a built-in transport with the specified settings")

      // clear a variable
      .def("clear",
          static_cast<bool (madara::knowledge::KnowledgeBase::*)(
              const std::string&,
              const madara::knowledge::KnowledgeReferenceSettings&)>(
              &madara::knowledge::KnowledgeBase::clear),
          "Clears a variable. This is safer than erasing the variable."
          "It clears the memory used in the variable and marks it as UNCREATED,"
          "meaning that it is effectively deleted, will not show up in"
          "@see print statements or @see save_checkpoint")

      // Clears the whole knowledge base
      .def("clear",
          static_cast<void (madara::knowledge::KnowledgeBase::*)(bool erase)>(
              &madara::knowledge::KnowledgeBase::clear),
          "Clears the knowledge base")

      // clear history record
      .def("clear_history",
          static_cast<void (madara::knowledge::KnowledgeBase::*)(
              const std::string&, const madara::knowledge::EvalSettings&)>(
              &madara::knowledge::KnowledgeBase::clear_history),
          "Clear all history for this record, keeping the current value")

      // clear history record
      .def("clear_history",
          static_cast<void (madara::knowledge::KnowledgeBase::*)(
              const madara::knowledge::VariableReference&,
              const madara::knowledge::EvalSettings&)>(
              &madara::knowledge::KnowledgeBase::clear_history),
          "Clear all history for this record, keeping the current value")

      // clear the KB
      .def("clear_map", &madara::knowledge::KnowledgeBase::clear_map,
          "Clears the knowledge base")

      // clear all modifications to KB
      .def("clear_modifieds",
          &madara::knowledge::KnowledgeBase::clear_modifieds,
          "Clear all modifications to the knowledge base. This action may"
          "be useful if you are wanting to keep local changes but not"
          "inform other agents (possibly due to a need to further process"
          "and verify the information). The knowledge stays in its current"
          "form (i.e., this does not roll back state or anything like that)")

      // close the transport mechanism
      .def("close_transport",
          &madara::knowledge::KnowledgeBase::close_transport,
          "Closes the transport mechanism so no dissemination is possible")

      // copy variables and values to a context
      .def("copy",
          static_cast<void (madara::knowledge::KnowledgeBase::*)(
              const madara::knowledge::KnowledgeBase&,
              const madara::knowledge::KnowledgeRequirements&,
              const madara::knowledge::EvalSettings&)>(
              &madara::knowledge::KnowledgeBase::copy),
          "Copies variables and values from source to this context."
          "PERFORMANCE NOTES: predicates with prefixes can limit"
          "copying to O(log n). predices with suffixes and no prefix"
          "force O(n) copy cost since all records could fit requirements")

      // copy variables and values to a context
      .def("copy",
          static_cast<void (madara::knowledge::KnowledgeBase::*)(
              const madara::knowledge::KnowledgeBase&,
              const madara::knowledge::CopySet&, bool clean_copy,
              const madara::knowledge::EvalSettings&)>(
              &madara::knowledge::KnowledgeBase::copy),
          "Copies variables and values from source to this context. PERFORMANCE"
          "NOTES: worst case depends on size of copy_set. If empty, performance"
          "is always O (n), where n is number of variables in the source "
          "context."
          "If copy_set is not empty, then performance is O (m log n) where m is"
          "the number of variables in copy_set and n is number of variables in"
          "the source context")

      // compile KaRL expression
      .def("compile", &madara::knowledge::KnowledgeBase::compile,
          "Compiles a KaRL expression into an expression tree")

      // expands a statement with variable expansion
      .def("debug_modifieds",
          &madara::knowledge::KnowledgeBase::debug_modifieds,
          "Retrieves a stringified list of all modified variables that are "
          "ready"
          "to send over transport on next send_modifieds call")

      // defines a python function
      .def("define_function",
          static_cast<void (madara::knowledge::KnowledgeBase::*)(
              const std::string&, object)>(
              &madara::knowledge::KnowledgeBase::define_function),
          "defines a named function that can be called within evaluates")

      // defines a python function
      .def("define_function",
          static_cast<void (madara::knowledge::KnowledgeBase::*)(
              const std::string&, const std::string&)>(
              &madara::knowledge::KnowledgeBase::define_function),
          "Defines a MADARA KaRL function")

      // defines a python function
      .def("define_function",
          static_cast<void (madara::knowledge::KnowledgeBase::*)(
              const std::string&,
              const madara::knowledge::CompiledExpression&)>(
              &madara::knowledge::KnowledgeBase::define_function),
          "Defines a MADARA KaRL function")

      // evaluate an expression
      .def("evaluate",
          static_cast<madara::knowledge::KnowledgeRecord (
              madara::knowledge::KnowledgeBase::*)(
              const std::string&, const madara::knowledge::EvalSettings&)>(
              &madara::knowledge::KnowledgeBase::evaluate),
          m_eval_1_of_2(
              args("expression", "settings"), "Evaluates an expression"))

      // evaluate an expression
      .def("evaluate",
          static_cast<madara::knowledge::KnowledgeRecord (
              madara::knowledge::KnowledgeBase::*)(
              madara::knowledge::CompiledExpression&,
              const madara::knowledge::EvalSettings&)>(
              &madara::knowledge::KnowledgeBase::evaluate),
          m_eval_1_of_2(
              args("expression", "settings"), "Evaluates an expression"))

      // Evaluates a root-based tree
      .def("evaluate",
          static_cast<madara::knowledge::KnowledgeRecord (
              madara::knowledge::KnowledgeBase::*)(
              madara::expression::ComponentNode * root,
              const madara::knowledge::EvalSettings&)>(
              &madara::knowledge::KnowledgeBase::evaluate),
          m_eval_1_of_2(args("expression", "settings"),
              "Evaluates a root-based tree (result of compile)"))

      // load and evaluate KaRL file
      .def("evaluate_file", &madara::knowledge::KnowledgeBase::evaluate_file,
          "Loads and evaluates a karl script from a file")

      // check if a knowledge location exists
      .def("exists",
          static_cast<bool (madara::knowledge::KnowledgeBase::*)(
              const std::string&,
              const madara::knowledge::KnowledgeReferenceSettings&) const>(
              &madara::knowledge::KnowledgeBase::exists),
          "Checks if a knowledge location exists in the context")

      // check if a knowledge location exists
      .def("exists",
          static_cast<bool (madara::knowledge::KnowledgeBase::*)(
              const madara::knowledge::VariableReference&,
              const madara::knowledge::KnowledgeReferenceSettings&) const>(
              &madara::knowledge::KnowledgeBase::exists),
          "Checks if a knowledge location exists in the context")

      // expands a statement with variable expansion
      .def("expand_statement",
          &madara::knowledge::KnowledgeBase::expand_statement,
          "Expand a statement")

      // wait for a change to happen
      .def("facade_for", &madara::knowledge::KnowledgeBase::facade_for,
          "Change the knowledge base to become a facade for another context."
          "It is extremely important that the context stays within scope for"
          "the duration of the life of this Knowledge Base. Otherwise, the"
          "Knowledge Base will eventually point to invalid memory")

      // convert file to string
      .def("file_to_string", &madara::knowledge::KnowledgeBase::file_to_string,
          "Loads and returns a karl script from a file with encode/decode")

      // Retrieve a knowledge value
      .def("get",
          static_cast<madara::knowledge::KnowledgeRecord (
              madara::knowledge::KnowledgeBase::*)(const std::string&,
              const madara::knowledge::KnowledgeReferenceSettings&)>(
              &madara::knowledge::KnowledgeBase::get),
          m_get_1_of_2(args("key", "settings"), "Retrieves a knowledge value"))

      // get a knowledge record
      .def("get",
          static_cast<madara::knowledge::KnowledgeRecord (
              madara::knowledge::KnowledgeBase::*)(
              const madara::knowledge::VariableReference&,
              const madara::knowledge::KnowledgeReferenceSettings&)>(
              &madara::knowledge::KnowledgeBase::get),
          m_get_1_of_2(args("variable", "settings"),
              "Atomically returns the value of a variable."))

      // get entire stored history
      .def("get_history",
          static_cast<std::vector<KnowledgeRecord> (
              madara::knowledge::KnowledgeBase::*)(const std::string&,
              const madara::knowledge::KnowledgeReferenceSettings&) const>(
              &madara::knowledge::KnowledgeBase::get_history),
          "Get a copy of the entire stored history of this record")

      // get range of history
      .def("get_history",
          static_cast<std::vector<KnowledgeRecord> (
              madara::knowledge::KnowledgeBase::*)(const std::string&,
              size_t index, size_t count,
              const madara::knowledge::KnowledgeReferenceSettings&) const>(
              &madara::knowledge::KnowledgeBase::get_history),
          "Return a copy of the given range of history in a vector. Indexing"
          "starts from oldest history entry in the buffer at index 0. Negative"
          "indices count from newest entries (-1 is newest)")

      // get history at certain index
      .def("get_history",
          static_cast<KnowledgeRecord (madara::knowledge::KnowledgeBase::*)(
              const std::string&, size_t index,
              const madara::knowledge::KnowledgeReferenceSettings&) const>(
              &madara::knowledge::KnowledgeBase::get_history),
          "Return the given entry in this record's history. Indexing"
          "starts from oldest history entry in the buffer at index 0. Negative"
          "indices count from newest entries (-1 is newest)")

      // get entire history
      .def("get_history",
          static_cast<std::vector<KnowledgeRecord> (
              madara::knowledge::KnowledgeBase::*)(
              const madara::knowledge::VariableReference&,
              const madara::knowledge::KnowledgeReferenceSettings&) const>(
              &madara::knowledge::KnowledgeBase::get_history),
          "Get a copy of the entire stored history of this record")

      // get history within a range
      .def("get_history",
          static_cast<std::vector<KnowledgeRecord> (
              madara::knowledge::KnowledgeBase::*)(
              const madara::knowledge::VariableReference&, size_t index,
              size_t count,
              const madara::knowledge::KnowledgeReferenceSettings&) const>(
              &madara::knowledge::KnowledgeBase::get_history),
          "Return a copy of the given range of history in a vector. Indexing"
          "starts from oldest history entry in the buffer at index 0. Negative"
          "indices count from newest entries (-1 is newest)")

      // get history at a given entry
      .def("get_history",
          static_cast<KnowledgeRecord (madara::knowledge::KnowledgeBase::*)(
              const madara::knowledge::VariableReference&, size_t index,
              const madara::knowledge::KnowledgeReferenceSettings&) const>(
              &madara::knowledge::KnowledgeBase::get_history),
          "Return the given entry in this record's history. Indexing"
          "starts from oldest history entry in the buffer at index 0. Negative"
          "indices count from newest entries (-1 is newest)")

      // get history size
      .def("get_history_size",
          static_cast<size_t (madara::knowledge::KnowledgeBase::*)(
              const std::string&,
              const madara::knowledge::KnowledgeReferenceSettings&) const>(
              &madara::knowledge::KnowledgeBase::get_history_size),
          "Return the amount of history this record holds")

      // get history size
      .def("get_history_size",
          static_cast<size_t (madara::knowledge::KnowledgeBase::*)(
              const madara::knowledge::VariableReference&,
              const madara::knowledge::KnowledgeReferenceSettings&) const>(
              &madara::knowledge::KnowledgeBase::get_history_size),
          "Return the amount of history this record holds")

      // get history capacity
      .def("get_history_capacity",
          static_cast<size_t (madara::knowledge::KnowledgeBase::*)(
              const std::string&,
              const madara::knowledge::KnowledgeReferenceSettings&) const>(
              &madara::knowledge::KnowledgeBase::get_history_capacity),
          "Return the maximum amount of history this record can hold. Use"
          "set_history_capacity to adjust this")

      // get history capacity
      .def("get_history_capacity",
          static_cast<size_t (madara::knowledge::KnowledgeBase::*)(
              const madara::knowledge::VariableReference&,
              const madara::knowledge::KnowledgeReferenceSettings&) const>(
              &madara::knowledge::KnowledgeBase::get_history_capacity),
          "Return the maximum amount of history this record can hold. Use"
          "set_history_capacity to adjust this")

      // return unique host
      .def("get_id", &madara::knowledge::KnowledgeBase::get_id,
          "Returns the unique host and ephemeral binding for this Knowlede "
          "Base")

      // get log level
      .def("get_log_level", &madara::knowledge::KnowledgeBase::get_log_level,
          "Gets the log level")

      // get matching variables
      .def("get_matches", &madara::knowledge::KnowledgeBase::get_matches,
          "Creates an iteration of VariableReferences to all keys matching"
          "the prefix and suffix")

      // get newest history entry for record
      .def("get_newest",
          static_cast<KnowledgeRecord (madara::knowledge::KnowledgeBase::*)(
              const std::string&,
              const madara::knowledge::KnowledgeReferenceSettings&) const>(
              &madara::knowledge::KnowledgeBase::get_newest),
          "Return the newest stored history entry of this record")

      // get newest history entry for record
      .def("get_newest",
          static_cast<std::vector<KnowledgeRecord> (
              madara::knowledge::KnowledgeBase::*)(const std::string&,
              size_t count,
              const madara::knowledge::KnowledgeReferenceSettings&) const>(
              &madara::knowledge::KnowledgeBase::get_newest),
          "Return the @a count newest stored history entries of this record in"
          "a vector")

      // get newest history entry for record
      .def("get_newest",
          static_cast<KnowledgeRecord (madara::knowledge::KnowledgeBase::*)(
              const madara::knowledge::VariableReference&,
              const madara::knowledge::KnowledgeReferenceSettings&) const>(
              &madara::knowledge::KnowledgeBase::get_newest),
          "Return the newest stored history entry of this record")

      // get newest history entry for record
      .def("get_newest",
          static_cast<std::vector<KnowledgeRecord> (
              madara::knowledge::KnowledgeBase::*)(
              const madara::knowledge::VariableReference&, size_t count,
              const madara::knowledge::KnowledgeReferenceSettings&) const>(
              &madara::knowledge::KnowledgeBase::get_newest),
          "Return the @a count newest stored history entries of this record in"
          "a vector")

      // get the number of transports
      .def("get_num_transports",
          &madara::knowledge::KnowledgeBase::get_num_transports,
          "Gets the number of transports")

      // get oldest history entry for record
      .def("get_oldest",
          static_cast<KnowledgeRecord (madara::knowledge::KnowledgeBase::*)(
              const std::string&,
              const madara::knowledge::KnowledgeReferenceSettings&) const>(
              &madara::knowledge::KnowledgeBase::get_oldest),
          "Return the oldest stored history entry of this record")

      // get oldest history entry for record
      .def("get_oldest",
          static_cast<std::vector<KnowledgeRecord> (
              madara::knowledge::KnowledgeBase::*)(const std::string&,
              size_t count,
              const madara::knowledge::KnowledgeReferenceSettings&) const>(
              &madara::knowledge::KnowledgeBase::get_oldest),
          "Return the @a count oldest stored history entries of this record in"
          "a vector")

      // get oldest history entry for record
      .def("get_oldest",
          static_cast<KnowledgeRecord (madara::knowledge::KnowledgeBase::*)(
              const madara::knowledge::VariableReference&,
              const madara::knowledge::KnowledgeReferenceSettings&) const>(
              &madara::knowledge::KnowledgeBase::get_oldest),
          "Return the oldest stored history entry of this record")

      // get oldest history entry for record
      .def("get_oldest",
          static_cast<std::vector<KnowledgeRecord> (
              madara::knowledge::KnowledgeBase::*)(
              const madara::knowledge::VariableReference&, size_t count,
              const madara::knowledge::KnowledgeReferenceSettings&) const>(
              &madara::knowledge::KnowledgeBase::get_oldest),
          "Return the @a count oldest stored history entries of this record in"
          "a vector")

      // return a reference to a variable
      .def("get_ref", &madara::knowledge::KnowledgeBase::get_ref,
          "Atomically returns a reference to the variable. Variable references "
          "are"
          "efficient mechanisms for reference variables individually--similar "
          "to"
          "speedups seen from CompiledExpression")

      // see if there is history in KB
      .def("has_history",
          static_cast<bool (madara::knowledge::KnowledgeBase::*)(
              const std::string&,
              const madara::knowledge::KnowledgeReferenceSettings&) const>(
              &madara::knowledge::KnowledgeBase::has_history),
          "Return true if this record has a circular buffer history. Use"
          "set_history_capacity to add a buffer")

      // see if there is history in KB
      .def("has_history",
          static_cast<bool (madara::knowledge::KnowledgeBase::*)(
              const madara::knowledge::VariableReference&,
              const madara::knowledge::KnowledgeReferenceSettings&) const>(
              &madara::knowledge::KnowledgeBase::has_history),
          "Return true if this record has a circular buffer history. Use"
          "set_history_capacity to add a buffer")

      // expands and prints a statement
      .def("load_context",
          static_cast<int64_t (madara::knowledge::KnowledgeBase::*)(
              const std::string&, bool,
              const madara::knowledge::KnowledgeUpdateSettings&)>(
              &madara::knowledge::KnowledgeBase::load_context),
          m_load_context_1_of_3(args("filename", "use_id", "settings"),
              "Loads a variable context from a file"))

      // expands and prints a statement
      .def("load_context",
          static_cast<int64_t (madara::knowledge::KnowledgeBase::*)(
              const std::string&, FileHeader&, bool use_id,
              const madara::knowledge::KnowledgeUpdateSettings&)>(
              &madara::knowledge::KnowledgeBase::load_context),
          m_load_context_1_of_3(args("filename", "use_id", "settings"),
              "Loads a variable context from a file"))

      // expands and prints a statement
      .def("load_context",
          static_cast<int64_t (madara::knowledge::KnowledgeBase::*)(
              madara::knowledge::CheckpointSettings&,
              const madara::knowledge::KnowledgeUpdateSettings&)>(
              &madara::knowledge::KnowledgeBase::load_context),
          m_load_context_1_of_2(args("checkpoint_settings", "update_settings"),
              "Loads a variable context from a file with settings "
              "for checkpoint and knowledge updates"))

      // locks the knowledge base from updates from other threads
      .def("lock", &madara::knowledge::KnowledgeBase::lock,
          "Locks the knowledge base from updates from other threads")

      // mark a variable as modified
      .def("mark_modified",
          static_cast<void (madara::knowledge::KnowledgeBase::*)(
              const madara::knowledge::VariableReference&,
              const madara::knowledge::KnowledgeUpdateSettings&)>(
              &madara::knowledge::KnowledgeBase::mark_modified),
          "Marks the variable reference as updated")

      // mark a variable as modified
      .def("mark_modified",
          static_cast<void (madara::knowledge::KnowledgeBase::*)(
              const std::string&,
              const madara::knowledge::KnowledgeUpdateSettings&)>(
              &madara::knowledge::KnowledgeBase::mark_modified),
          "Marks the variable as updated")

      // same as apply_modified
      .def("modify", &madara::knowledge::KnowledgeBase::modify,
          "Alias for apply_modified. @see apply_modified")

      // evaluate an expression
      .def("print",
          static_cast<void (madara::knowledge::KnowledgeBase::*)(unsigned int)
                  const>(&madara::knowledge::KnowledgeBase::print),
          m_print_0_of_1(
              args("level"), "Prints all variables in the knowledge base"))

      // evaluate an expression
      .def("print",
          static_cast<void (madara::knowledge::KnowledgeBase::*)(
              const std::string&, unsigned int) const>(
              &madara::knowledge::KnowledgeBase::print),
          m_print_1_of_2(
              args("statement", "level"), "Expands and prints a statement"))

      // prints all knowledge variables
      .def("print_knowledge",
          &madara::knowledge::KnowledgeBase::print_knowledge,
          m_print_knowledge_0_of_1(args("level"), "Alias of print(level). "
                                                  "Prints all variables in the "
                                                  "knowledge base"))

      // evaluate an expression
      .def("read_file",
          static_cast<int (madara::knowledge::KnowledgeBase::*)(
              const std::string&, const std::string&,
              const madara::knowledge::EvalSettings&)>(
              &madara::knowledge::KnowledgeBase::read_file),
          m_read_file_2_of_3(args("knowledge_key", "filename", "settings"),
              "Read a file into the knowledge base"))

      // evaluate an expression
      .def("read_file",
          static_cast<int (madara::knowledge::KnowledgeBase::*)(
              const madara::knowledge::VariableReference&, const std::string&,
              const madara::knowledge::EvalSettings&)>(
              &madara::knowledge::KnowledgeBase::read_file),
          m_read_file_2_of_3(args("variable", "filename", "settings"),
              "Read a file into the knowledge base"))

      // releases a recursive lock on KB
      .def("release", &madara::knowledge::KnowledgeBase::release,
          "Releases a recursive lock on the knowledge base. This will"
          "allow other thread to access the knowledge base if you had"
          "previously called @ acquire")

      // removes transport
      .def("remove_transport",
          &madara::knowledge::KnowledgeBase::remove_transport,
          "Removes a transport")

      // reset the checkpoint
      .def("reset_checkpoint",
          &madara::knowledge::KnowledgeBase::reset_checkpoint,
          "Resets the local changed map, which tracks checkpointing modifieds")

      // get a knowledge record at an index
      .def("retrieve_index",
          static_cast<madara::knowledge::KnowledgeRecord (
              madara::knowledge::KnowledgeBase::*)(const std::string&, size_t,
              const madara::knowledge::KnowledgeReferenceSettings&)>(
              &madara::knowledge::KnowledgeBase::retrieve_index),
          m_retrieve_index_2_of_3(args("key", "index", "settings"),
              "Retrieves a knowledge record from an index"))

      // saves the context as JSON
      .def("save_as_json",
          static_cast<int64_t (madara::knowledge::KnowledgeBase::*)(
              const std::string&) const>(
              &madara::knowledge::KnowledgeBase::save_as_json),
          m_save_as_json_1_of_1(
              args("settings"), "Saves the context to a file as JSON"))

      // saves the context as JSON
      .def("save_as_json",
          static_cast<int64_t (madara::knowledge::KnowledgeBase::*)(
              const madara::knowledge::CheckpointSettings&) const>(
              &madara::knowledge::KnowledgeBase::save_as_json),
          m_save_as_json_1_of_1(
              args("settings"), "Saves the context to a file as JSON"))

      // saves the context as karl
      .def("save_as_karl",
          static_cast<int64_t (madara::knowledge::KnowledgeBase::*)(
              const std::string&) const>(
              &madara::knowledge::KnowledgeBase::save_as_karl),
          m_save_as_karl_1_of_1(args("settings"), "Saves the context to a file "
                                                  "as karl assignments, rather "
                                                  "than binary"))

      // saves the context as karl
      .def("save_as_karl",
          static_cast<int64_t (madara::knowledge::KnowledgeBase::*)(
              const madara::knowledge::CheckpointSettings&) const>(
              &madara::knowledge::KnowledgeBase::save_as_karl),
          m_save_as_karl_1_of_1(args("settings"), "Saves the context to a file "
                                                  "as karl assignments, rather "
                                                  "than binary"))

      // saves a diff of the context as binary
      .def("save_checkpoint",
          static_cast<int64_t (madara::knowledge::KnowledgeBase::*)(
              const std::string&, bool reset_modifieds)>(
              &madara::knowledge::KnowledgeBase::save_checkpoint),
          m_save_checkpoint_1_of_1(args("settings"),
              "Saves a checkpoint of a list of changes to a file"))

      // saves a diff of the context as binary
      .def("save_checkpoint",
          static_cast<int64_t (madara::knowledge::KnowledgeBase::*)(
              madara::knowledge::CheckpointSettings&) const>(
              &madara::knowledge::KnowledgeBase::save_checkpoint),
          m_save_checkpoint_1_of_1(args("settings"),
              "Saves a checkpoint of a list of changes to a file"))

      // saves the context as binary
      .def("save_context",
          static_cast<int64_t (madara::knowledge::KnowledgeBase::*)(
              const std::string&) const>(
              &madara::knowledge::KnowledgeBase::save_context),
          m_save_context_1_of_1(
              args("settings"), "Saves the context to a file"))

      // saves the context as binary
      .def("save_context",
          static_cast<int64_t (madara::knowledge::KnowledgeBase::*)(
              madara::knowledge::CheckpointSettings&) const>(
              &madara::knowledge::KnowledgeBase::save_context),
          m_save_context_1_of_1(
              args("settings"), "Saves the context to a file with settings"))

      // Saves the list of modified records to use later for resending
      .def("save_modifieds", &madara::knowledge::KnowledgeBase::save_modifieds,
          "Saves the list of modified records to use later for resending. This"
          "does not clear the modified list. This feature is useful if you"
          "want to remember what has been modified and then resend later, e.g.,"
          "if you believe packets may be dropped and want to resend "
          "information."
          "Use this function in conjunction with @see add_modifieds to remodify"
          "@return a vector of VariableReferences to the current modified list")

      // Sends all modified variables through the attached transports
      .def("send_modifieds",
          static_cast<int (madara::knowledge::KnowledgeBase::*)(
              const std::string&, const madara::knowledge::EvalSettings&)>(
              &madara::knowledge::KnowledgeBase::send_modifieds),
          m_send_modifieds_0_of_2(args("prefix", "settings"),
              "Sends all modified variables through the attached transports."))

      // sets a knowledge record to a specified value
      .def("set",
          static_cast<int (madara::knowledge::KnowledgeBase::*)(
              const VariableReference &,
              const KnowledgeRecord &,
              const EvalSettings & )>(
              &madara::knowledge::KnowledgeBase::set),
          m_set_2_of_3(args("key", "value", "settings"),
              "Atomically sets the value of a variable to a KnowledgeRecord."))

      // sets a knowledge record to a specified value
      .def("set",
          static_cast<int (madara::knowledge::KnowledgeBase::*)(
              const std::string &,
              const KnowledgeRecord &,
              const EvalSettings & )>(
              &madara::knowledge::KnowledgeBase::set),
          m_set_2_of_3(args("key", "value", "settings"),
              "Atomically sets the value of a variable to a KnowledgeRecord."))

      // sets a knowledge record to a double
      .def("set",
          static_cast<int (madara::knowledge::KnowledgeBase::*)(
              const std::string&, double,
              const madara::knowledge::EvalSettings&)>(
              &madara::knowledge::KnowledgeBase::set),
          m_set_2_of_3(args("key", "value", "settings"),
              "Sets a knowledge record to a double"))

      // sets a knowledge record to an array of doubles
      .def("set",
          static_cast<int (madara::knowledge::KnowledgeBase::*)(
              const std::string&, const std::vector<double>&,
              const madara::knowledge::EvalSettings&)>(
              &madara::knowledge::KnowledgeBase::set),
          m_set_2_of_3(args("key", "value", "settings"),
              "Sets a knowledge record to an array of doubles"))

      // sets a knowledge record to an integer
      .def("set",
          static_cast<int (madara::knowledge::KnowledgeBase::*)(
              const std::string&, madara::knowledge::KnowledgeRecord::Integer,
              const madara::knowledge::EvalSettings&)>(
              &madara::knowledge::KnowledgeBase::set),
          m_set_2_of_3(args("key", "value", "settings"),
              "Sets a knowledge record to an integer"))

      // sets a knowledge record to an array of integer
      .def("set",
          static_cast<int (madara::knowledge::KnowledgeBase::*)(
              const std::string&,
              const std::vector<madara::knowledge::KnowledgeRecord::Integer>&,
              const madara::knowledge::EvalSettings&)>(
              &madara::knowledge::KnowledgeBase::set),
          m_set_2_of_3(args("key", "value", "settings"),
              "Sets a knowledge record to an array of integers"))

      // sets a knowledge record to a string
      .def("set",
          static_cast<int (madara::knowledge::KnowledgeBase::*)(
              const std::string&, const std::string&,
              const madara::knowledge::EvalSettings&)>(
              &madara::knowledge::KnowledgeBase::set),
          m_set_2_of_3(args("key", "value", "settings"),
              "Sets a knowledge record to a string"))

      // set the log level
      .def("set_log_level", &madara::knowledge::KnowledgeBase::set_log_level,
          "Sets the log level")

      // set quality of writing
      .def("set_quality", &madara::knowledge::KnowledgeBase::set_quality,
          "Sets the quality of writing to a certain variable from this entity")

      // set values of a variable to a string
      .def("set_file",
          static_cast<int (madara::knowledge::KnowledgeBase::*)(
              const std::string& key, const unsigned char* value, size_t size,
              const madara::knowledge::EvalSettings&)>(
              &madara::knowledge::KnowledgeBase::set_file),
          "Atomically sets the value of a variable to an arbitrary string")

      // set values of a variable to a string
      .def("set_file",
          static_cast<int (madara::knowledge::KnowledgeBase::*)(
              const madara::knowledge::VariableReference&,
              const unsigned char* value, size_t size,
              const madara::knowledge::EvalSettings&)>(
              &madara::knowledge::KnowledgeBase::set_file),
          "Atomically sets the value of a variable to an arbitrary string")

      // set history size
      .def("set_history_capacity",
          static_cast<void (madara::knowledge::KnowledgeBase::*)(
              const std::string&, size_t size,
              const madara::knowledge::EvalSettings&)>(
              &madara::knowledge::KnowledgeBase::set_history_capacity),
          "Set the capacity of this record's history circular buffer. Every"
          "modification to this record will write a new entry in this history."
          "Once the capacity is met, the oldest entry will be discarded as new"
          "entries are added")

      // set history size of circular buffer
      .def("set_history_capacity",
          static_cast<void (madara::knowledge::KnowledgeBase::*)(
              const madara::knowledge::VariableReference&, size_t size,
              const madara::knowledge::EvalSettings&)>(
              &madara::knowledge::KnowledgeBase::set_history_capacity),
          "Set the capacity of this record's history circular buffer. Every"
          "modification to this record will write a new entry in this history."
          "Once the capacity is met, the oldest entry will be discarded as new"
          "entries are added")

      // set values of a variable to a jpeg image
      .def("set_jpeg",
          static_cast<int (madara::knowledge::KnowledgeBase::*)(
              const std::string&, const unsigned char* value, size_t size,
              const madara::knowledge::EvalSettings&)>(
              &madara::knowledge::KnowledgeBase::set_jpeg),
          "Atomically sets the value of a variable to a JPEG image")

      // set values of a variable to a jpeg image
      .def("set_jpeg",
          static_cast<int (madara::knowledge::KnowledgeBase::*)(
              const madara::knowledge::VariableReference&,
              const unsigned char* value, size_t size,
              const madara::knowledge::EvalSettings&)>(
              &madara::knowledge::KnowledgeBase::set_jpeg),
          "Atomically sets the value of a variable to a JPEG image")

      // set a unique hostport
      .def("setup_unique_hostport",
          &madara::knowledge::KnowledgeBase::setup_unique_hostport,
          ""
          "Binds to an ephemeral port for unique tie breakers in global "
          "ordering."
          "Call this function if you want to generate a unique id for a"
          "custom transport to use (or a built-in transport if you are creating"
          "one outside of the KnowledgeBase")

      .def("to_map",
          +[](madara::knowledge::KnowledgeBase& kb, const std::string& prefix) {
            return kb.to_map(prefix);
          },
          "Get all records starting with the given prefix and return as "
          "KnowledgeRecordMap.")

      .def("to_map",
          +[](madara::knowledge::KnowledgeBase& kb, const std::string& prefix,
               const std::string& delimiter, const std::string& suffix) {
            std::vector<std::string> next_keys;
            std::map<std::string, madara::knowledge::KnowledgeRecord> result;
            kb.to_map(prefix, delimiter, suffix, next_keys, result, false);
            return result;
          },
          "Fills a variable map with list of keys according to a matching "
          "prefix,\n"
          "suffix, and delimiter hierarchy. This is useful for understanding "
          "the\n"
          "logical hierarchy of your variables\n"
          "param 0 prefix: Text that must be present at the front of the key\n"
          "param 1 delimiter: Text that signifies a logical boundary in "
          "hierarchy If\n"
          "                     empty, no check is performed.\n"
          "param 2 suffix: Text that must be present at the end of the key. "
          "If\n"
          "                     empty, no check is performed.\n"
          "returns a KnowledgeRecordMap containing the matching keys with "
          "values")

      // Write a file from the knowledge base to a specified location
      .def("to_map_stripped",
          &madara::knowledge::KnowledgeBase::to_map_stripped,
          "Creates a map with Knowledge Records that begin with the given"
          "prefix. Runs in O(log n + m) time, where n is the size of the"
          "KnowledgeBase, and m is the number of matching records")

      // convert a value to a string
      .def("to_string", &madara::knowledge::KnowledgeBase::to_string,
          "Saves all keys and values into a string, using the underlying"
          "knowledge::KnowledgeRecord::to_string function. This is an optimized"
          "version that allows the specification of a target string to"
          "avoid string copying (which can be expensive with longer"
          "strings across multiple function boundaries). This function differs"
          "from knowledge::KnowledgeRecord to_string in that it is intended to "
          "save the"
          "database in a format that can be easily parseable. Consequently,"
          "strings are delineated in this function by being included in single"
          "quotes. Arrays are delineated with array indices")

      // sets value of a variable to an XML string
      .def("to_vector", &madara::knowledge::KnowledgeBase::to_vector,
          "Fills a vector with Knowledge Records that begin with a common "
          "subject"
          "and have a finite range of integer values")

      // unlocks the knowledge base
      .def("unlock", &madara::knowledge::KnowledgeBase::unlock,
          "Unlocks the knowledge base and allows other threads to access")

      // refer to another knowledge base
      .def("use", &madara::knowledge::KnowledgeBase::use,
          "Refer to and use another knowledge base's context")

      // wait on an expression
      .def("wait",
          static_cast<madara::knowledge::KnowledgeRecord (
              madara::knowledge::KnowledgeBase::*)(
              const std::string&, const madara::knowledge::WaitSettings&)>(
              &madara::knowledge::KnowledgeBase::wait),
          m_wait_1_of_2(args("expression", "settings"),
              "Waits for an expression to evaluate to true"))

      // wait on an expression
      .def("wait",
          static_cast<madara::knowledge::KnowledgeRecord (
              madara::knowledge::KnowledgeBase::*)(
              madara::knowledge::CompiledExpression&,
              const madara::knowledge::WaitSettings&)>(
              &madara::knowledge::KnowledgeBase::wait),
          m_wait_1_of_2(args("expression", "settings"),
              "Waits for an expression to evaluate to true"))

      // wait for a change to happen
      .def("wait_for_change",
          &madara::knowledge::KnowledgeBase::wait_for_change,
          "Wait for a change to happen to the context (e.g., from transports)")

      // Write a file from the knowledge base to a specified location
      .def("write_file", &madara::knowledge::KnowledgeBase::write_file,
          "Write a file from the knowledge base to a specified location")

      ;

  class_<std::vector<madara::knowledge::KnowledgeRecord>>(
      "KnowledgeRecordVector")
      .def(vector_indexing_suite<
          std::vector<madara::knowledge::KnowledgeRecord>>());

  class_<std::map<std::string, madara::knowledge::KnowledgeRecord>>(
      "KnowledgeRecordMap")
      .def(map_indexing_suite<
          std::map<std::string, madara::knowledge::KnowledgeRecord>>());

  // define madara.knowledge.containers
  define_knowledge_containers();
}

#endif  // _MADARA_PYTHON_PORT_MADARA_KNOWLEGE_CPP_
