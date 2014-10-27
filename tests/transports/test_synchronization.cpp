
//#define MADARA_NTRACE    0
//#define ACE_NTRACE    0
////#define ACE_NLOGGING  0
//#define ACE_NDEBUG    0

#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <assert.h>

#include "ace/Log_Msg.h"
#include "ace/Get_Opt.h"
#include "ace/Signal.h"
#include "ace/Sched_Params.h"

#include "madara/knowledge_engine/Knowledge_Base.h"

Madara::Knowledge_Record::Integer id = 0;
Madara::Knowledge_Record::Integer left = 0;
Madara::Knowledge_Record::Integer processes = 2;
Madara::Knowledge_Record::Integer stop = 10;
Madara::Knowledge_Record::Integer value = 0;
std::string host;
std::string domain ("n_state");
std::string multicast ("239.255.0.1:4150");

volatile bool terminated = 0;

// command line arguments
int parse_args (int argc, ACE_TCHAR * argv[]);

// signal handler for someone hitting control+c
extern "C" void terminate (int)
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

int ACE_TMAIN (int argc, ACE_TCHAR * argv[])
{
  int retcode = parse_args (argc, argv);

  if (retcode < 0)
    return retcode;

  // use ACE real time scheduling class
  int prio  = ACE_Sched_Params::next_priority
    (ACE_SCHED_FIFO,
     ACE_Sched_Params::priority_max (ACE_SCHED_FIFO),
     ACE_SCOPE_THREAD);
  ACE_OS::thr_setprio (prio);

  // transport settings
  Madara::Transport::Settings ts;
  ts.domains = domain;
  ts.type = Madara::Transport::MULTICAST;
  ts.hosts.resize (1);
  ts.hosts[0] = multicast;

  // start the knowledge engine
  Madara::Knowledge_Engine::Knowledge_Base knowledge (
    host, ts);

  // signal handler for clean exit
  ACE_Sig_Action sa ((ACE_SignalHandler) terminate, SIGINT);

  Madara::Knowledge_Engine::Compiled_Expression compiled;
  Madara::Knowledge_Engine::Compiled_Expression self_state_broadcast;
  Madara::Knowledge_Engine::Wait_Settings wait_settings;

  // set my id
  knowledge.set (".self", id);
  knowledge.set (".processes", processes);

  // The state of the process to my left dictates my next state
  // if I am the bottom process, I look at the last process
  knowledge.set (".left", id ? id - 1 : processes - 1);

  // keep track of the right since they require knowledge from us
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
  knowledge.set ("S{.self}", value);

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
  
  Madara::Knowledge_Engine::Eval_Settings default_eval;

  // termination is done via signalling from the user (Control+C)
  while (!terminated)
  {
    knowledge.wait (compiled, wait_settings);

    ACE_OS::sleep (1);
    
    knowledge.evaluate (self_state_broadcast, default_eval);
  }

  knowledge.print ();

  return 0;
}



int parse_args (int argc, ACE_TCHAR * argv[])
{
  // options string which defines all short args
  ACE_TCHAR options [] = ACE_TEXT ("d:i:s:p:o:v:h");

  // create an instance of the command line args
  ACE_Get_Opt cmd_opts (argc, argv, options);

  // set up an alias for '-n' to be '--name'
  cmd_opts.long_option (ACE_TEXT ("domain"), 'd', ACE_Get_Opt::ARG_REQUIRED);
  cmd_opts.long_option (ACE_TEXT ("multicast"), 'm', ACE_Get_Opt::ARG_REQUIRED);
  cmd_opts.long_option (ACE_TEXT ("id"), 'i', ACE_Get_Opt::ARG_REQUIRED);
  cmd_opts.long_option (ACE_TEXT ("stop"), 's', ACE_Get_Opt::ARG_REQUIRED);
  cmd_opts.long_option (ACE_TEXT ("processes"), 'p', ACE_Get_Opt::ARG_REQUIRED);
  cmd_opts.long_option (ACE_TEXT ("help"), 'h', ACE_Get_Opt::ARG_REQUIRED);
  cmd_opts.long_option (ACE_TEXT ("host"), 'o', ACE_Get_Opt::ARG_REQUIRED);
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
    case 's':
      {
        std::stringstream buffer;
        buffer << cmd_opts.opt_arg ();
        buffer >> stop;
      }
      break;
    case 'v':
      {
        std::stringstream buffer;
        buffer << cmd_opts.opt_arg ();
        buffer >> value;
      }
      break;
    case ':':
      ACE_ERROR_RETURN ((LM_ERROR, 
        ACE_TEXT ("ERROR: -%c requires an argument"), 
           cmd_opts.opt_opt ()), -2); 
    case 'h':
    default:
      ACE_DEBUG ((LM_DEBUG, "Program Summary for %s:\n\n\
      This distributed application uses n-state Dijkstra synchronization\n\
      to form a self-healing ring of legitimate state machines. To execute\n\
      the logic requires 2+ processes with ids set from 0 to n-1. Control+C\n\
      ends the application.\n\n\
      -d (--domain)    domain to separate all traffic into\n\
      -i (--id)        set process id (0 default)  \n\
      -o (--host)      this host ip/name (\"\" default) \n\
      -m (--multicast) the multicast ip to send and listen to \n\
                       (239.255.0.1:4150 default)\n\
      -p (--processes) number of processes that will be running (2 default)\n\
      -s (--stop)      stop condition (10 default) \n\
      -v (--value)     start process with a certain value (0 default) \n\
      -h (--help)      print this menu             \n\n", argv[0]));
      ACE_ERROR_RETURN ((LM_ERROR, 
        ACE_TEXT ("Returning from Help Menu\n")), -1); 
      break;
    }
  }

  return 0;
}
