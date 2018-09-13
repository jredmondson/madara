
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <assert.h>

#include "madara/knowledge/KnowledgeBase.h"
#include "madara/logger/GlobalLogger.h"
#include "madara/utility/Utility.h"

namespace logger = madara::logger;

madara::knowledge::KnowledgeRecord::Integer id = 0;
madara::knowledge::KnowledgeRecord::Integer left = 0;
madara::knowledge::KnowledgeRecord::Integer processes = 2;
madara::knowledge::KnowledgeRecord::Integer stop = 10;
madara::knowledge::KnowledgeRecord::Integer value = 0;
std::string host;
std::string domain ("n_state");
std::string multicast ("239.255.0.1:4150");

volatile bool terminated = 0;

std::string build_wait ()
{
  std::stringstream buffer;
  buffer << "(S" << id << ".init = 1)";

  for (int i = 0; i < processes; ++i)
    buffer << " && S" << i << ".init";

  return buffer.str ();
}

std::string build_state_print ()
{
  std::stringstream buffer;
  buffer << " ";

  for (int i = 0; i < processes; ++i)
    buffer << " {S" << i << "}";

  buffer << "\n";
  return buffer.str ();
}

int main (int, char **)
{
#ifndef _MADARA_NO_KARL_

  // transport settings
  madara::transport::TransportSettings ts;
  ts.write_domain = domain;
  ts.type = madara::transport::MULTICAST;
  ts.hosts.resize (1);
  ts.hosts[0] = multicast;

  // start the knowledge engine
  madara::knowledge::KnowledgeBase knowledge (
    host, ts);

  madara::knowledge::CompiledExpression compiled;
  madara::knowledge::CompiledExpression self_state_broadcast;
  madara::knowledge::WaitSettings wait_settings;
  wait_settings.delay_sending_modifieds = false;

  // set my id
  knowledge.set (".self", id,
    madara::knowledge::EvalSettings::SEND);
  knowledge.set (".processes", processes,
    madara::knowledge::EvalSettings::SEND);

  // The state of the process to my left dictates my next state
  // if I am the bottom process, I look at the last process
  knowledge.set (".left", id ? id - 1 : processes - 1,
    madara::knowledge::EvalSettings::SEND);

  // keep track of the right since they require knowledge from us
  knowledge.set (".right", id == processes - 1 ? 0 : id + 1,
    madara::knowledge::EvalSettings::SEND);

  // set my stop state
  knowledge.set (".stop", stop,
    madara::knowledge::EvalSettings::SEND);

  // set my initial value
  knowledge.set (".init", value,
    madara::knowledge::EvalSettings::SEND);

  // by default, the expression to evaluate is for a non-bottom process
  // if my state does not equal the left state, change my state to left state
  std::string expression = build_wait ();
  wait_settings.pre_print_statement = 
    "  Waiting on all {.processes} processes to join\n";
  wait_settings.post_print_statement = 
    "  Finished waiting on S{.left}.started and S{.right}.started\n";
  wait_settings.max_wait_time = 30.0;

  compiled = knowledge.compile (expression);
  self_state_broadcast = knowledge.compile ("S{.self} = S{.self}");

  // wait for left and right processes to startup before executing application logic
  knowledge.wait (compiled, wait_settings);

  // set initial value of this state to the initial value
  knowledge.set ("S{.self}", value,
    madara::knowledge::EvalSettings::SEND);

  // by default, the expression to evaluate is for a non-bottom process
  // if my state does not equal the left state, change my state to left state
  expression = "S{.self} != S{.left} => S{.self} = S{.left}";

  // if I am the bottom process, however, I do NOT want to be my left state
  // so if the top process becomes my state, I move on to my next state
  // this allows for a ring of legitimate states that progress towards an
  // end goal (in our case the .stop condition)
  if (id == 0)
  {
    expression = 
      "S{.self} == S{.left} => S{.self} = (S{.self} + 1) % .stop";   
  }

  wait_settings.pre_print_statement = "";
  wait_settings.post_print_statement = build_state_print ();
  wait_settings.max_wait_time = 5.0;

  compiled = knowledge.compile (expression);

  knowledge.print (wait_settings.post_print_statement);
  
  madara::knowledge::EvalSettings default_eval;
  default_eval.delay_sending_modifieds = false;

  // termination is done via signalling from the user (Control+C)
  while (!terminated)
  {
    knowledge.wait (compiled, wait_settings);

    madara::utility::sleep (1);
    
    knowledge.evaluate (self_state_broadcast, default_eval);
  }

  knowledge.print ();
  
#else
  std::cout << "This test is disabled due to karl feature being disabled.\n";
#endif
  return 0;
}
