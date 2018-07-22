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

/**
 * @file MadaraModule.cpp
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains Boost.Python mappings for the C++ MADARA library
 * to a Python module called Madara.
 **/

using namespace boost::python;

class Filters_NS {};

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

}


#endif // _MADARA_PYTHON_PORT_MADARA_FILTERS_H_

