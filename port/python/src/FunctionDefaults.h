#ifndef _MADARA_PYTHON_FUNCTION_DEFAULTS_
#define _MADARA_PYTHON_FUNCTION_DEFAULTS_

/**
 * @file FunctionDefaults.cpp
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains Boost.Python function overload definitions
 * for various MADARA functions.
 **/

#include "madara/knowledge/KnowledgeBase.h"
#include <boost/python/module.hpp>
#include <boost/python/def.hpp>
#include <boost/python/args.hpp>
#include <boost/python/tuple.hpp>
#include <boost/python/class.hpp>
#include <boost/python/overloads.hpp>
#include <boost/python/return_internal_reference.hpp>

#include "madara/utility/Utility.h"

/********************************************************
 * Settings overloads
 ********************************************************/

/********************************************************
 * Class members (functions inside of classes)
 ********************************************************/

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS (m_to_string_0_of_1,
  to_string, 0, 1)

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS (m_get_1_of_2,
  get, 1, 2)
  
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS (m_send_modifieds_0_of_2,
  send_modifieds, 0, 2)
  
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS (m_load_context_1_of_2,
  load_context, 1, 2)
  
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS (m_load_context_1_of_3,
  load_context, 1, 3)
  
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS (m_set_2_of_3,
  set, 2, 3)
  
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS (m_set_index_2_of_3,
  set_index, 3, 4)

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS (m_eval_1_of_2,
  evaluate, 1, 2)

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS (m_wait_1_of_2,
  wait, 1, 2)

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS (m_print_1_of_2,
  print, 1, 2)
  
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS (m_print_0_of_1,
  print, 0, 1)

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS (m_print_knowledge_0_of_1,
  print_knowledge, 0, 1)

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS (m_retrieve_index_2_of_3,
  retrieve_index, 2, 3)

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS (m_save_as_json_1_of_1,
  save_as_json, 1, 1)

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS (m_save_as_karl_1_of_1,
  save_as_karl, 1, 1)

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS (m_save_context_1_of_1,
  save_context, 1, 1)

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS (m_save_checkpoint_1_of_1,
  save_checkpoint, 1, 1)

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS (m_resize_1_of_1,
  resize, 1, 1)

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS (m_resize_0_of_2,
  resize, 0, 2)

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS (m_fragment_file_1_of_2,
  fragment_file, 1, 2)

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS (m_fragment_buffer_2_of_3,
  fragment_buffer, 2, 3)

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS (m_create_vector_2_of_3,
  create_vector, 2, 3)

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS (m_from_kb_2_of_3,
  from_kb, 2, 3)

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS (m_read_file_1_of_2,
  read_file, 1, 2)

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS (m_read_file_2_of_3,
  read_file, 2, 3)


/********************************************************
 * Static functions (standalone functions)
 ********************************************************/

BOOST_PYTHON_FUNCTION_OVERLOADS (file_from_fragments_2_of_4,
  madara::utility::file_from_fragments, 2, 4)

BOOST_PYTHON_FUNCTION_OVERLOADS (get_file_progress_3_of_4,
  madara::utility::get_file_progress, 3, 4)

BOOST_PYTHON_FUNCTION_OVERLOADS (file_crc_1_of_2,
  madara::utility::file_crc, 1, 2)

#endif
