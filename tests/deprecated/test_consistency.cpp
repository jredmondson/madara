
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <assert.h>

#include "ace/Get_Opt.h"
#include "ace/Signal.h"
#include "ace/Sched_Params.h"
#include "ace/High_Res_Timer.h"

#include "madara/knowledge_engine/Knowledge_Base.h"

Madara::Knowledge_Record::Integer id = 0;
Madara::Knowledge_Record::Integer left = 0;
Madara::Knowledge_Record::Integer processes = 1;
Madara::Knowledge_Record::Integer stop = 10;
Madara::Knowledge_Record::Integer value = 0;
uint32_t transport = Madara::Transport::SPLICE;

ACE_Time_Value max_tv (10, 0);

bool skip_barrier = false;

std::string host = "";
std::string domain = "consistency";

// transport settings
Madara::Transport::Settings ts;

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
  return "{x},{y},{z},{.inconsistent},{.states}\n";
}

int ACE_TMAIN (int argc, ACE_TCHAR * argv[])
{
  int retcode = parse_args (argc, argv);

  if (retcode < 0)
    return retcode;

  // allow for setting a timer
  ACE_High_Res_Timer timer;
  ACE_hrtime_t elapsed (0);
  ACE_hrtime_t maximum (0);

  // setup the maximum elapsed time we can have before ending
  maximum = max_tv.sec () * 1000000000;
  maximum += max_tv.usec () * 1000;

  // use ACE real time scheduling class
  int prio  = ACE_Sched_Params::next_priority
    (ACE_SCHED_FIFO,
     ACE_Sched_Params::priority_max (ACE_SCHED_FIFO),
     ACE_SCOPE_THREAD);
  ACE_OS::thr_setprio (prio);

  ts.domains = domain;
  ts.type = transport;
  ts.queue_length = 10000000;

  // start the knowledge engine
  Madara::Knowledge_Engine::Knowledge_Base knowledge (
    host, ts);

  // signal handler for clean exit
  ACE_Sig_Action sa ((ACE_SignalHandler) terminate, SIGINT);

  Madara::Knowledge_Engine::Compiled_Expression compiled;
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

  // set initial value of this state to the initial value
  knowledge.set (".inconsistent", Madara::Knowledge_Record::Integer (0));
  knowledge.set (".states", Madara::Knowledge_Record::Integer (0));

  std::string expression;

  if (!skip_barrier)
  {
    // by default, the expression to evaluate is for a non-bottom process
    // if my state does not equal the left state, change my state to left state
    expression = build_wait ();
    wait_settings.pre_print_statement = 
      "  Waiting on all {.processes} processes to join\n";
    wait_settings.post_print_statement = 
      "  Finished waiting on S{.left}.started and S{.right}.started\n";
    compiled = knowledge.compile (expression);

    // wait for left and right processes to startup 
    // before executing application logic
    knowledge.wait (compiled, wait_settings);
  }

  // by default, the expression to evaluate is for a non-bottom process
  // if my state does not equal the left state, change my state to left state
  expression = "(.oldx != x || .oldy != y || .oldz != z) => "
    "(++.states; y != x * 2 => ++.inconsistent; z != x * 3 => ++.inconsistent;"
    " .oldx = x; .oldy = y; .oldz = z)";

  // if I am the bottom process, however, I do NOT want to be my left state
  // so if the top process becomes my state, I move on to my next state
  // this allows for a ring of legitimate states that progress towards an
  // end goal (in our case the .stop condition)
  if (id == 0)
  {
    expression = "++.states; ++x; y = x * 2; z = x * 3; .inconsistent=0";
  }

  wait_settings.pre_print_statement = "";
  wait_settings.post_print_statement = build_state_print ();
  //wait_settings.max_wait_time = 10.0;
  //wait_settings.poll_frequency = 0.5;

  compiled = knowledge.compile (expression);

  knowledge.print (wait_settings.post_print_statement);

  timer.start ();

  // termination is done via signalling from the user (Control+C)
  while (!terminated && maximum > elapsed)
  {
    knowledge.wait (compiled, wait_settings);
    timer.stop ();
    timer.elapsed_time (elapsed);
  }

  knowledge.print_knowledge ();

  return 0;
}



int parse_args (int argc, ACE_TCHAR * argv[])
{
  // options string which defines all short args
  ACE_TCHAR options [] = ACE_TEXT ("kd:l:i:s:m:p:o:v:t:xh");

  // create an instance of the command line args
  ACE_Get_Opt cmd_opts (argc, argv, options);

  // set up an alias for '-n' to be '--name'
  cmd_opts.long_option (ACE_TEXT ("domain"), 'd', ACE_Get_Opt::ARG_REQUIRED);
  cmd_opts.long_option (ACE_TEXT ("id"), 'i', ACE_Get_Opt::ARG_REQUIRED);
  cmd_opts.long_option (ACE_TEXT ("stop"), 's', ACE_Get_Opt::ARG_REQUIRED);
  cmd_opts.long_option (ACE_TEXT ("processes"), 'p', ACE_Get_Opt::ARG_REQUIRED);
  cmd_opts.long_option (ACE_TEXT ("help"), 'h', ACE_Get_Opt::NO_ARG);
  cmd_opts.long_option (ACE_TEXT ("time"), 'm', ACE_Get_Opt::ARG_REQUIRED);
  cmd_opts.long_option (ACE_TEXT ("host"), 'o', ACE_Get_Opt::ARG_REQUIRED);
  cmd_opts.long_option (ACE_TEXT ("skip"), 'k', ACE_Get_Opt::ARG_REQUIRED);
  cmd_opts.long_option (ACE_TEXT ("transport"), 't', ACE_Get_Opt::ARG_REQUIRED);
  cmd_opts.long_option (ACE_TEXT ("inconsistent"), 'x', ACE_Get_Opt::NO_ARG);
  cmd_opts.long_option (ACE_TEXT ("loglevel"), 'v', ACE_Get_Opt::ARG_REQUIRED);
 
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
    case 'k':
      skip_barrier = true;
      break;
    case 'l':
      {
        std::stringstream buffer;
        buffer << cmd_opts.opt_arg ();
        buffer >> value;
      }
      break;
    case 'm':
      {
        double kill_time = 0.0;
        std::stringstream buffer;
        buffer << cmd_opts.opt_arg ();
        buffer >> kill_time;

        max_tv.set (kill_time);
      }
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
    case 't':
      {
        std::stringstream buffer;
        buffer << cmd_opts.opt_arg ();
        buffer >> transport;
      }
      break;
    case 'x':
      transport = 100;
      break;
    case 'v':
      // log level
      {
        std::stringstream buffer;
        buffer << cmd_opts.opt_arg ();
        buffer >> MADARA_debug_level;
      }

      MADARA_DEBUG (MADARA_LOG_MINOR_EVENT, (LM_DEBUG,
        DLINFO "KATS_PROCESS: logging level set to %u\n",
        MADARA_debug_level));
      break;
    case ':':
      ACE_ERROR_RETURN ((LM_ERROR, 
        ACE_TEXT ("ERROR: -%c requires an argument"), 
           cmd_opts.opt_opt ()), -2); 
    case 'h':
    default:
      ACE_DEBUG ((LM_DEBUG, "Program Options:      \n\
      -d (--domain)    domain to separate all traffic into\n\
      -i (--id)        set process id (0 default)  \n\
      -l (--value)     start process with a certain value (0 default) \n\
      -m (--time)      time in seconds to run for (10 default)\n\
      -o (--host)      this host ip/name (localhost default) \n\
      -p (--processes) number of processes that will be running\n\
      -s (--stop)      stop condition (10 default) \n\
      -t (--transport) \n\
                       None (0), Splice (1), NDDS (2), Inconsistent (100)\n\
      -v (--loglevel)  set log level (1 default) \n\
      -x (--inconsistent) Use an inconsistent transport (-t 100) \n\
      -h (--help)      print this menu             \n"));
      ACE_ERROR_RETURN ((LM_ERROR, 
        ACE_TEXT ("Returning from Help Menu")), -1); 
      break;
    }
  }

  return 0;
}
