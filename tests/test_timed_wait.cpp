
#include <string>
#include <vector>
#include <iostream>
#include <sstream>

#include "madara/knowledge/KnowledgeBase.h"
#include "madara/logger/GlobalLogger.h"

namespace logger = madara::logger;

int num_fails = 0;

int main (int, char **)
{
#ifndef _MADARA_NO_KARL_
  // debug level for logging
  //logger::global_logger->set_level (7);

  // don't use a transport for this test.
  madara::transport::TransportSettings transport_settings;
  transport_settings.type = madara::transport::NO_TRANSPORT;
  
  // holder for expression trees
  madara::knowledge::CompiledExpression expression;

  madara::knowledge::KnowledgeBase knowledge ("nohost",
    transport_settings);

  madara::knowledge::WaitSettings wait_settings;
  std::string logic = "++.count && 0";
  wait_settings.pre_print_statement =
    "WAIT STARTED: Waiting for 10 seconds.\n";
  wait_settings.post_print_statement =
    "WAIT ENDED: Number of executed waits was {.count}.\n";

  // set poll frequency to once every millisecond and wait for 10 seconds
  wait_settings.poll_frequency = .001;
  wait_settings.max_wait_time = 10.0;

  // **********************************************
  // * Test 1: Waiting on expression that will never finish, but
  // *         since we are setting a 10 second limit, it will finish
  // **********************************************

  expression = knowledge.compile (logic);

  // start waiting
  knowledge.wait (expression, wait_settings);

  knowledge.print ("Test 1: count >= 1000, actual = {.count}. ");
  if (knowledge.get (".count") > 1000)
  {
    knowledge.print ("SUCCESS\n");
  }
  else
  {
    knowledge.print ("FAIL\n"); ++num_fails;
  }

  // **********************************************
  // * Test 2: Waiting and evaluating only once
  // **********************************************

  // clear any existing knowledge
  knowledge.clear ();

  logic = "++.count && 1";
  wait_settings.pre_print_statement =
    "WAIT STARTED: .count == {.count}. Waiting for .count == 1.\n";

  expression = knowledge.compile (logic);

  // start waiting
  knowledge.wait (expression, wait_settings);

  knowledge.print ("Test 2: count == 1, actual = {.count}. ");
  if (knowledge.get (".count") == 1)
  {
    knowledge.print ("SUCCESS\n");
  }
  else
  {
    knowledge.print ("FAIL\n"); ++num_fails;
  }

  // **********************************************
  // * Test 3: Waiting and evaluating twice
  // **********************************************

  // clear any existing knowledge
  knowledge.clear ();

  logic = "++.count && .count == 2";
  wait_settings.pre_print_statement =
    "WAIT STARTED: .count == {.count}. Waiting for .count == 2.\n";
  expression = knowledge.compile (logic);

  // start waiting
  knowledge.wait (expression, wait_settings);

  knowledge.print ("Test 3: count == 2, actual = {.count}. ");
  if (knowledge.get (".count") == 2)
  {
    knowledge.print ("SUCCESS\n");
  }
  else
  {
    knowledge.print ("FAIL\n"); ++num_fails;
  }

  // **********************************************
  // * Test 4: Waiting and evaluating ten times
  // **********************************************

  // clear any existing knowledge
  knowledge.clear ();

  logic = "++.count && .count == 10";
  wait_settings.pre_print_statement =
    "WAIT STARTED: .count == {.count}. Waiting for .count == 10.\n";
  expression = knowledge.compile (logic);

  // start waiting
  knowledge.wait (expression, wait_settings);
  
  knowledge.print ("Test 4: count == 10, actual = {.count}. ");
  if (knowledge.get (".count") == 10)
  {
    knowledge.print ("SUCCESS\n");
  }
  else
  {
    knowledge.print ("FAIL\n"); ++num_fails;
  }

#else
  madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_ALWAYS,
    "This test is disabled due to karl feature being disabled.\n");
#endif

  if (num_fails > 0)
  {
    std::cerr << "OVERALL: FAIL. " << num_fails << " tests failed.\n";
  }
  else
  {
    std::cerr << "OVERALL: SUCCESS.\n";
  }

  return num_fails;
}

