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

/********************************************************
 * Settings overloads
 ********************************************************/

/********************************************************
 * Knowledge Record overloads
 ********************************************************/

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS (m_to_string_0_of_1,
  to_string, 0, 1)
  

/********************************************************
 * Knowledge Base overloads
 ********************************************************/

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS (m_get_1_of_2,
  get, 1, 2)
  
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS (m_send_modifieds_0_of_2,
  send_modifieds, 0, 2)
  
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


#endif
