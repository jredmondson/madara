
//#define MADARA_NTRACE    0
//#define ACE_NTRACE    0
////#define ACE_NLOGGING  0
//#define ACE_NDEBUG    0

#include <string>
#include <sstream>
#include <vector>
#include <iostream>
#include <assert.h>

#include "ace/Log_Msg.h"
#include "ace/Get_Opt.h"
#include <signal.h>
#include "ace/Sched_Params.h"

#include "madara/knowledge/CompiledExpression.h"
#include "madara/knowledge/KnowledgeBase.h"
#include "madara/logger/GlobalLogger.h"

namespace logger = madara::logger;

bool logical_print = false;
madara::knowledge::KnowledgeRecord::Integer id = 2;
madara::knowledge::KnowledgeRecord::Integer left = 0;
madara::knowledge::KnowledgeRecord::Integer processes = 3;
madara::knowledge::KnowledgeRecord::Integer stop = 3;
madara::knowledge::KnowledgeRecord::Integer value = 0;
volatile bool terminated = 0;
std::string host;
std::string domain ("three_state");
std::string multicast ("239.255.0.1:4150");

// signal handler for someone hitting control+c
void shutdown (int)
{
  terminated = true;
}

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

// command line arguments
int parse_args (int argc, ACE_TCHAR * argv[]);

int ACE_TMAIN (int argc, ACE_TCHAR * argv[])
{
  int retcode = parse_args (argc, argv);

  if (retcode < 0)
    return retcode;
  
#ifndef _MADARA_NO_KARL_
  // use ACE real time scheduling class
  int prio  = ACE_Sched_Params::next_priority
    (ACE_SCHED_FIFO,
     ACE_Sched_Params::priority_max (ACE_SCHED_FIFO),
     ACE_SCOPE_THREAD);
  ACE_OS::thr_setprio (prio);

  // signal handler for clean exit
  signal (SIGINT, shutdown);

  // transport settings
  madara::transport::TransportSettings ts;
  ts.write_domain = domain;
  ts.type = madara::transport::MULTICAST;
  ts.hosts.resize (1);
  ts.hosts[0] = multicast;

  // start the knowledge engine
  madara::knowledge::KnowledgeBase knowledge (
    host, ts);

  // variables for compiled expressions and wait settings
  madara::knowledge::CompiledExpression compiled;
  madara::knowledge::CompiledExpression self_state_broadcast;
  madara::knowledge::WaitSettings wait_settings;

  // set my id
  knowledge.set (".self", id);
  knowledge.set (".processes", processes);

  // The state of the process to my left dictates my next state
  // if I am the bottom process, I look at the last process
  knowledge.set (".left", id ? id - 1 : processes - 1);

  // The state of the process to my right can also dictate my state
  // if I am the top process (i.e. id == processes -1), then my
  // right process is the bottom process (i.e. process with id == 0)
  // we're basically wrapping around at both ends. See Dijkstra's third
  // synchronization algorithm defined in Self-stabilizing Systems  in 
  // Spite of Distributed Control
  knowledge.set (".right", id == processes - 1 ? 0 : id + 1);

  // set my stop state
  knowledge.set (".stop", stop);
  // set my initial value
  knowledge.set (".init", value);

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
  //knowledge.evaluate ("S{.self}=.init");
  knowledge.set ("S{.self}", value);

  if (id == 0)
  {
    expression = 
    // if we are the bottom process, (id == 0), then logic is
    // if (S+1)     % 3 == R       then S        = (S-1)          % 3
      "(S{.self}+1) % 3 == S{.right} => S{.self} = (S{.self}+3-1) % 3";
  }
  else if (id == processes - 1)
  {
    // top process logic
    expression = 
    // if  L   == R        && (L       +l)%3 != S       then S       = (L        + 1)%3
      "S{.left}==S{.right} && (S{.left}+1)%3 != S{.self} " \
      "        => S{.self} = (S{.left} + 1)%3";
  }
  else
  {
    // every other process is looking at both its left and right
    // to try to synchronize with two sources. Here is the logic:
    expression = 
    // if( S     + 1)%3 == L       then S        = L
      "(S{.self} + 1)%3 == S{.left} => S{.self} = S{.left};" \
    // if( S      + 1) % 3 ==    R    then   S     =    R
      "(S{.self} + 1)%3 == S{.right} => S{.self} = S{.right}";
  }

  // setup the main loop
  compiled = knowledge.compile (expression);
  wait_settings.pre_print_statement = "";
  wait_settings.post_print_statement = build_state_print ();

  knowledge.print (wait_settings.post_print_statement);

  madara::knowledge::EvalSettings default_eval;

  // termination is done via signalling from the user (Control+C)
  while (!terminated)
  {
    knowledge.wait (compiled, wait_settings);

    ACE_OS::sleep (1);

    knowledge.evaluate (self_state_broadcast, default_eval);
  }

  knowledge.print ();
  
#else
  std::cout << "This test is disabled due to karl feature being disabled.\n";
#endif
  return 0;
}



int parse_args (int argc, ACE_TCHAR * argv[])
{
  // options string which defines all short args
  ACE_TCHAR options [] = ACE_TEXT ("d:i:p:o:v:lh");

  // create an instance of the command line args
  ACE_Get_Opt cmd_opts (argc, argv, options);

  // set up an alias for '-n' to be '--name'
  cmd_opts.long_option (ACE_TEXT ("domain"), 'd', ACE_Get_Opt::ARG_REQUIRED);
  cmd_opts.long_option (ACE_TEXT ("help"), 'h', ACE_Get_Opt::NO_ARG);
  cmd_opts.long_option (ACE_TEXT ("multicast"), 'm', ACE_Get_Opt::ARG_REQUIRED);
  cmd_opts.long_option (ACE_TEXT ("id"), 'i', ACE_Get_Opt::ARG_REQUIRED);
  cmd_opts.long_option (ACE_TEXT ("logical"), 'l', ACE_Get_Opt::NO_ARG);
  cmd_opts.long_option (ACE_TEXT ("host"), 'o', ACE_Get_Opt::ARG_REQUIRED);
  cmd_opts.long_option (ACE_TEXT ("processes"), 'p', ACE_Get_Opt::ARG_REQUIRED);
  cmd_opts.long_option (ACE_TEXT ("value"), 'v', ACE_Get_Opt::ARG_REQUIRED);
 
  // temp for current switched option
  int option;
//  ACE_TCHAR * arg;

  // iterate through the options until done
  while ((option = cmd_opts ()) != EOF)
  {
    //arg = cmd_opts.opt_arg ();
    switch (option)
    {
    case 'd':
      domain = cmd_opts.opt_arg ();
      break;
    case 'i':
      {
        std::stringstream buffer;
        buffer << cmd_opts.opt_arg ();
        buffer >> id;
      }
      break;
    case 'm':
      multicast = cmd_opts.opt_arg ();
      break;
    case 'o':
      host = cmd_opts.opt_arg ();
      break;
    case 'p':
      {
        std::stringstream buffer;
        buffer << cmd_opts.opt_arg ();
        buffer >> processes;
      }
      break;
    case 'v':
      {
        std::stringstream buffer;
        buffer << cmd_opts.opt_arg ();
        buffer >> value;
      }

      if (value > 2 || value < 0)
      ACE_ERROR_RETURN ((LM_ERROR, 
        ACE_TEXT ("ERROR: -%c requires an argument that is >= 0 and <= 2"), 
           cmd_opts.opt_opt ()), -3); 

      break;
    case ':':
      ACE_ERROR_RETURN ((LM_ERROR, 
        ACE_TEXT ("ERROR: -%c requires an argument"), 
           cmd_opts.opt_opt ()), -2); 
    case 'h':
    default:
      madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_ALWAYS,
        "Program Summary for %s:\n\n\
      This distributed application uses three-state Dijkstra synchronization\n\
      to form a self-healing ring of legitimate state machines. To execute\n\
      the logic requires 3+ processes with ids set from 0 to n-1. Control+C\n\
      ends the application.\n\n\
      -d (--domain)    domain to separate all traffic into\n\
      -i (--id)        set process id (0 default)  \n\
      -o (--host)      this host ip/name (\"\" default) \n\
      -m (--multicast) the multicast ip to send and listen to \n\
                       (239.255.0.1:4150 default)\n\
      -p (--processes) number of processes that will be running (3 default)\n\
      -v (--value)     start process with a certain value (0 default) \n\
      -h (--help)      print this menu             \n\n", argv[0]);

      exit (-1);
      break;
    }
  }

  return 0;
}
