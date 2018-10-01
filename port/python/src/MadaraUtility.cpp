#ifndef _MADARA_PYTHON_PORT_MADARA_UTILITY_CPP_
#define _MADARA_PYTHON_PORT_MADARA_UTILITY_CPP_

#include <boost/python/detail/wrap_python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <boost/python/suite/indexing/map_indexing_suite.hpp>
#include <boost/python/dict.hpp>
#include <boost/python/import.hpp>
#include <boost/python/enum.hpp>

#include "madara/utility/Utility.h"
#include "MadaraUtility.h"
#include "FunctionDefaults.h"

/**
 * @file MadaraUtility.cpp
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains Boost.Python mappings for the madara.utility module
 **/

using namespace boost::python;

class Utility_NS
{
};

/********************************************************
 * Filters namespace definitions
 ********************************************************/
void define_utility(void)
{
  object utility = object(handle<>(PyModule_New("madara.utility")));

  utility.attr("__file__") = "<synthetic>";
  scope().attr("utility") = utility;
  utility.attr("__doc__") = "Provides general purpose utility functions";

  // this was the missing piece: sys.modules['modA.modB']=modB
  extract<dict>(getattr(import("sys"), "modules"))()["madara.utility"] =
      utility;

  scope utility_scope = utility;

  /********************************************************
   * Utility function Definitions
   ********************************************************/

  def("file_from_fragments", &madara::utility::file_from_fragments,
      file_from_fragments_2_of_4(
          args("filename", "crc", "delete_incomplete", "delete_fragments"),
          "Constructs a file from fragments sent by the MADARA File Service"));

  def("file_crc", &madara::utility::file_crc,
      file_crc_1_of_2(
          args("filename", "max_block"), "Returns the 32-bit crc of a file"));

  def("get_file_progress", &madara::utility::get_file_progress,
      get_file_progress_3_of_4(
          args("filename", "crc", "expected_size", "fragment_size"),
          "Checks the filesystem for file completion and returns the"
          " current received file size in the directory for the crc"));
}

#endif  // _MADARA_PYTHON_PORT_MADARA_UTILITY_CPP_
