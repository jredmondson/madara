
#include <string>
#include <sstream>
#include <vector>
#include <iostream>
#include <assert.h>
#include <signal.h>

#include "madara/knowledge/CompiledExpression.h"
#include "madara/knowledge/KnowledgeBase.h"
#include "madara/logger/GlobalLogger.h"
#include "madara/utility/Utility.h"

namespace logger = madara::logger;

bool logical_print = false;
madara::knowledge::KnowledgeRecord::Integer id = 2;
madara::knowledge::KnowledgeRecord::Integer left = 0;
madara::knowledge::KnowledgeRecord::Integer processes = 3;
madara::knowledge::KnowledgeRecord::Integer stop = 3;
madara::knowledge::KnowledgeRecord::Integer value = 0;
volatile bool terminated = 0;
std::string host;
std::string domain("three_state");
std::string multicast("239.255.0.1:4150");

// signal handler for someone hitting control+c
void shutdown(int)
{
  terminated = true;
}

std::string build_wait()
{
  std::stringstream buffer;
  buffer << "(S" << id << ".init = 1)";

  for(int i = 0; i < processes; ++i)
    buffer << " && S" << i << ".init";

  return buffer.str();
}

std::string build_state_print()
{
  std::stringstream buffer;
  buffer << " ";

  for(int i = 0; i < processes; ++i)
    buffer << " {S" << i << "}";

  buffer << "\n";
  return buffer.str();
}

int main(int, char**)
{
#ifndef _MADARA_NO_KARL_
  // set thread priority to high
  madara::utility::set_thread_priority();

  // signal handler for clean exit
  signal(SIGINT, shutdown);

  // transport settings
  madara::transport::TransportSettings ts;
  ts.write_domain = domain;
  ts.type = madara::transport::MULTICAST;
  ts.hosts.resize(1);
  ts.hosts[0] = multicast;

  // start the knowledge engine
  madara::knowledge::KnowledgeBase knowledge(host, ts);

  // variables for compiled expressions and wait settings
  madara::knowledge::CompiledExpression compiled;
  madara::knowledge::CompiledExpression self_state_broadcast;
  madara::knowledge::WaitSettings wait_settings;
  wait_settings.delay_sending_modifieds = false;

  // set my id
  knowledge.set(".self", id, madara::knowledge::EvalSettings::SEND);
  knowledge.set(".processes", processes, madara::knowledge::EvalSettings::SEND);

  // The state of the process to my left dictates my next state
  // if I am the bottom process, I look at the last process
  knowledge.set(".left", id ? id - 1 : processes - 1,
      madara::knowledge::EvalSettings::SEND);

  // The state of the process to my right can also dictate my state
  // if I am the top process (i.e. id == processes -1), then my
  // right process is the bottom process (i.e. process with id == 0)
  // we're basically wrapping around at both ends. See Dijkstra's third
  // synchronization algorithm defined in Self-stabilizing Systems  in
  // Spite of Distributed Control
  knowledge.set(".right", id == processes - 1 ? 0 : id + 1,
      madara::knowledge::EvalSettings::SEND);

  // set my stop state
  knowledge.set(".stop", stop, madara::knowledge::EvalSettings::SEND);
  // set my initial value
  knowledge.set(".init", value, madara::knowledge::EvalSettings::SEND);

  // by default, the expression to evaluate is for a non-bottom process
  // if my state does not equal the left state, change my state to left state
  std::string expression = build_wait();
  wait_settings.pre_print_statement =
      "  Waiting on all {.processes} processes to join\n";
  wait_settings.post_print_statement =
      "  Finished waiting on S{.left}.started and S{.right}.started\n";
  wait_settings.max_wait_time = 30.0;

  compiled = knowledge.compile(expression);
  self_state_broadcast = knowledge.compile("S{.self} = S{.self}");

  // wait for left and right processes to startup before executing application
  // logic
  knowledge.wait(compiled, wait_settings);

  // set initial value of this state to the initial value
  // knowledge.evaluate ("S{.self}=.init");
  knowledge.set("S{.self}", value, madara::knowledge::EvalSettings::SEND);

  if(id == 0)
  {
    expression =
        // if we are the bottom process, (id == 0), then logic is
        // if(S+1)     % 3 == R       then S        = (S-1)          % 3
        "(S{.self}+1) % 3 == S{.right} => S{.self} = (S{.self}+3-1) % 3";
  }
  else if(id == processes - 1)
  {
    // top process logic
    expression =
        // if  L   == R        && (L       +l)%3 != S       then S       = (L
        // + 1)%3
        "S{.left}==S{.right} && (S{.left}+1)%3 != S{.self} "
        "        => S{.self} = (S{.left} + 1)%3";
  }
  else
  {
    // every other process is looking at both its left and right
    // to try to synchronize with two sources. Here is the logic:
    expression =
        // if( S     + 1)%3 == L       then S        = L
        "(S{.self} + 1)%3 == S{.left} => S{.self} = S{.left};"  // if( S      +
                                                                // 1) % 3 ==
                                                                // R    then   S
                                                                // =    R
        "(S{.self} + 1)%3 == S{.right} => S{.self} = S{.right}";
  }

  // setup the main loop
  compiled = knowledge.compile(expression);
  wait_settings.pre_print_statement = "";
  wait_settings.post_print_statement = build_state_print();

  knowledge.print(wait_settings.post_print_statement);

  madara::knowledge::EvalSettings default_eval;
  default_eval.delay_sending_modifieds = false;

  // termination is done via signalling from the user (Control+C)
  while(!terminated)
  {
    knowledge.wait(compiled, wait_settings);

    madara::utility::sleep(1);

    knowledge.evaluate(self_state_broadcast, default_eval);
  }

  knowledge.print();

#else
  std::cout << "This test is disabled due to karl feature being disabled.\n";
#endif
  return 0;
}
