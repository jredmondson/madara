#ifndef _MADARA_PYTHON_PORT_MADARA_FILTERS_H_
#define _MADARA_PYTHON_PORT_MADARA_FILTERS_H_

#include <boost/python/detail/wrap_python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <boost/python/suite/indexing/map_indexing_suite.hpp>
#include <boost/python/dict.hpp>
#include <boost/python/import.hpp>
#include <boost/python/enum.hpp>

#include "madara/knowledge/KnowledgeBase.h"
#include "madara/filters/GenericFilters.h"
#include "FunctionDefaults.h"
#include "madara/filters/ssl/AESBufferFilter.h"
#include "madara/filters/lz4/LZ4BufferFilter.h"

/**
 * @file MadaraModule.cpp
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains Boost.Python mappings for the C++ MADARA library
 * to a Python module called Madara.
 **/

using namespace boost::python;

class Filters_NS {};

struct BufferFilterWrap : madara::filters::BufferFilter,
  wrapper<madara::filters::BufferFilter>
{

    int encode (char * source, int size, int max_size) const
    {
      return this->get_override("encode")(source, size, max_size);
    }

    int decode (char * source, int size, int max_size) const
    {
      return this->get_override("decode")(source, size, max_size);
    }

    std::string get_id (void)
    {
      return this->get_override("get_id")();
    }

    uint32_t get_version (void)
    {
      return this->get_override("get_version")();
    }
};

/********************************************************
  * Filters namespace definitions
  ********************************************************/
void define_filters (void)
{
  object filters = object (handle<> (
    PyModule_New ("madara.filters")));

  filters.attr("__file__")="<synthetic>";
  scope().attr ("filters") = filters;
  filters.attr ("__doc__") = "Provides filters for records, transports and files";

  // this was the missing piece: sys.modules['modA.modB']=modB
  extract <dict> (getattr (
    import ("sys"), "modules"))()["madara.filters"] = filters;

  scope filters_scope = filters;

  /********************************************************
   * BufferFilter Definitions
   ********************************************************/

  class_<BufferFilterWrap, boost::noncopyable> ("BufferFilter",
    "Abstract base class for filters that perform actions on a character "
    "buffer")
    .def("get_id",
      pure_virtual (&madara::filters::BufferFilter::get_id))
    .def("get_version",
      pure_virtual (&madara::filters::BufferFilter::get_version))
  ;

#ifdef _USE_SSL_
  class_<madara::filters::AESBufferFilter,
          bases<madara::filters::BufferFilter> > (
            "AESBufferFilter",
            "Filter for encrypting and decrypting buffers using AES "
            "256 bit encryption", init<> ())
    // Clears the rebroadcast filters for a specified type
    .def ("generate_key",
      &madara::filters::AESBufferFilter::generate_key,
      "Generates a new key for AES 256 bit encryption based on a password")
  ;     
#endif

#ifdef _USE_LZ4_
  class_<madara::filters::LZ4BufferFilter,
          bases<madara::filters::BufferFilter> > (
            "LZ4BufferFilter",
            "Filter for compressing and decompressing buffers using LZ4 "
            , init<> ())
  ;
#endif
}


#endif // _MADARA_PYTHON_PORT_MADARA_FILTERS_H_

