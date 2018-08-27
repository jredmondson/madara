#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpragmas"
#pragma GCC diagnostic ignored "-Wunused-local-typedef"
#endif

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
#include "MadaraKnowledge.h"

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

/**
 * @file Madara.cpp
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains Boost.Python mappings for the C++ MADARA library
 * to a Python module called madara.
 **/

using namespace boost::python;

BOOST_PYTHON_MODULE (madara)
{
  // Launch the appropriate thread management initialization
  PyEval_InitThreads();

  // Declare classes inside Madara namespace (top namespace of Python module)
  
  class_ <std::vector <std::string> > ("StringVector")
    .def(vector_indexing_suite<std::vector<std::string> >());

  define_filters ();
  define_transport ();
  define_knowledge ();
}
