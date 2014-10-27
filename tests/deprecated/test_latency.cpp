
//#define MADARA_NTRACE    0
//#define ACE_NTRACE    0
////#define ACE_NLOGGING  0
//#define ACE_NDEBUG    0

#include <string>
#include <vector>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <assert.h>

#include "ace/Log_Msg.h"
#include "ace/Get_Opt.h"
#include "ace/High_Res_Timer.h"
#include "ace/OS_NS_Thread.h"
#include "ace/Sched_Params.h"

#include "ace/Signal.h"

#include "madara/knowledge_engine/Knowledge_Base.h"
#include "madara/transport/Transport.h"
#include "madara/utility/Utility.h"

Madara::Knowledge_Record::Integer id = 0;
Madara::Knowledge_Record::Integer left = 0;
Madara::Knowledge_Record::Integer processes = 2;
Madara::Knowledge_Record::Integer stop = 10;
Madara::Knowledge_Record::Integer value = 0;
Madara::Knowledge_Record::Integer iterations = 100000;

std::string host = "";

volatile bool terminated = false;

// command line arguments
int parse_args (int argc, ACE_TCHAR * argv[]);

// signal handler for someone hitting control+c
extern "C" void terminate (int)
{
  terminated = true;
}


/**
 * Converts an uint64_t into a string
 * @param   source   parameter to convert into a string
 * @return  comma separated version of the source in a string
 */
std::string
ull_to_string (uint64_t source)
{
  std::stringstream buffer;

  // add commas
  std::locale loc(""); 
  buffer.imbue (loc); 

  // stream the source into the buffer
  buffer << source;

  // return the string representation
  return buffer.str ().c_str ();
}

/**
 * Build a wait string based on some attribute
 * @param    attribute     extension added to P0 on which to build checks for
 * @param    count         number of processes, can be a subset of group
 */
std::string
  build_wait_string (Madara::Knowledge_Record::Integer id,
  const std::string & attribute,
  Madara::Knowledge_Record::Integer count)
{
  std::stringstream buffer;

  // set our attribute to 1
  buffer << "((P";
  buffer << id;
  buffer << ".";
  buffer << attribute;
  buffer << " = 1)";

  // add logic to wait for all processes in the ring to set their
  // attribute to not-zero
  for (int i = 0; i < count; ++i)
  {
    buffer << " && P";
    buffer << i;
    buffer << ".";
    buffer << attribute;
  }

  // make an escape clause due to our usage of best_effort
  // if we see the attribute set globally, then move on because
  // we have missed a message, even though we know everyone has
  // arrived
  buffer << ") || ";
  buffer << attribute;

  return buffer.str ();
}

/**
 * Runs a series of latency tests.
 * @param     knowledge      the knowledge base
 * @param     iterations     the number of iterations to run
 * @param     compile_latency   return value of compile time
 * @return    total latency for all iterations
 */
uint64_t test_latency (
                     Madara::Knowledge_Engine::Knowledge_Base & knowledge,
                     Madara::Knowledge_Record::Integer iterations, 
                     ACE_hrtime_t & compile_latency,
                     ACE_hrtime_t & eval_latency
                     )
{
  std::string expression;

  // keep track of time
  ACE_hrtime_t count = 0;
  ACE_High_Res_Timer compile_timer;
  ACE_High_Res_Timer eval_timer;
  ACE_High_Res_Timer latency_timer;


  // make the processes play tag
  if (id == 0)
  {
    // p0 gets started early. Make him stay for the return trip.
    ++iterations;
    // if p1 has the same value as p0, then p0 increments
    expression = "P0 == P1 => ++P0";
  }
  else if (id == 1)
  {
    // if p0 does not equal p1, then make it so!
    expression = "P0 != P1 => P1 = P0";
  }

  Madara::Knowledge_Engine::Wait_Settings wait_settings;
  Madara::Knowledge_Engine::Eval_Settings eval_settings;
  eval_settings.send_modifieds = false;

  // test compilation latency
  compile_timer.start ();
  Madara::Knowledge_Engine::Compiled_Expression compiled = 
    knowledge.compile (expression);
  compile_timer.stop ();

  // get the amount of time it took
  compile_timer.elapsed_time (compile_latency);


  // test evaluation latency
  eval_timer.start ();
  knowledge.evaluate (compiled, eval_settings);
  eval_timer.stop ();

  // get the amount of time it took
  eval_timer.elapsed_time (eval_latency);

  ACE_DEBUG ((LM_INFO, "(%P|%t) (%d of %d) KaRL logic compile latency was %s ns\n",
    id, processes, ull_to_string (compile_latency).c_str ()));

  ACE_DEBUG ((LM_INFO, "(%P|%t) (%d of %d) KaRL logic post-compile latency was %s ns\n",
    id, processes, ull_to_string (eval_latency).c_str ()));

  // reset the counter to 0, in case test_latencies has been called before
  // do not send out modifieds or this may cause issues because the
  // expression to be evaluated requires this count to start at 0 for it
  // to make sense. .old on the other hand is a local variable, so there is
  // no need to be cautious.
  knowledge.set ("P0", Madara::Knowledge_Record::Integer (0), false);
  knowledge.set ("P1", Madara::Knowledge_Record::Integer (0), false);


  ACE_DEBUG ((LM_INFO, "(%P|%t) (%d of %d) KaRL logic will be %s\n",
    id, processes, expression.c_str ()));

  ACE_hrtime_t total_latency = 0;

  knowledge.print ("Beginning {.iterations} roundtrip tests.\n");

  latency_timer.start ();

  for (uint32_t i = 0; i < iterations; ++i)
  {
    knowledge.wait (compiled, wait_settings);
  }

  latency_timer.stop ();
  latency_timer.elapsed_time (total_latency);

  return total_latency;
}


int ACE_TMAIN (int argc, ACE_TCHAR * argv[])
{
  int retcode = parse_args (argc, argv);

  if (retcode < 0)
    return retcode;

  // role of process in this test
  std::string type;

  //ACE_LOG_MSG->priority_mask (LM_DEBUG | LM_INFO, ACE_Log_Msg::PROCESS);
  ACE_LOG_MSG->priority_mask (LM_INFO, ACE_Log_Msg::PROCESS);

  ACE_TRACE (ACE_TEXT ("main"));

  // use ACE real time scheduling class
  int prio  = ACE_Sched_Params::next_priority
    (ACE_SCHED_FIFO,
     ACE_Sched_Params::priority_max (ACE_SCHED_FIFO),
     ACE_SCOPE_THREAD);
  ACE_OS::thr_setprio (prio);

  // signal handler for clean exit
  ACE_Sig_Action sa ((ACE_SignalHandler) terminate, SIGINT);

  // this test is specifically for 2 processes. If we have an id
  // that is greater than 2, return and let process id 0 and 1 do their jobs

  if (id >= 2)
    return 0;

  Madara::Transport::Settings settings;
  settings.type = Madara::Transport::MULTICAST;
  settings.hosts_.resize (1);
  settings.hosts_[0] = "239.255.0.1:4150";
  settings.domains = "KaRL_Latency";
  settings.reliability = Madara::Transport::RELIABLE;

  Madara::Knowledge_Engine::Knowledge_Base knowledge(host, settings);

  // set my id
  knowledge.set (".self", id);
  knowledge.set (".processes", Madara::Knowledge_Record::Integer (2));
  knowledge.set (".iterations", iterations);

  ACE_DEBUG ((LM_INFO, "(%P|%t) (%d of %d) waiting for other processes to join\n",
                        id, processes, iterations));

  // build a logic based on the started attribute and then wait
  // for all processes to start
  knowledge.wait (build_wait_string (id, "started", processes));
  knowledge.set ("started");

  ACE_DEBUG ((LM_INFO, "(%P|%t) (%d of %d) starting latency tests for %d events\n",
                        id, processes, iterations));

  // at this point, all processes are accounted for. Let's begin our tests.

  // keep track of time
  ACE_hrtime_t compile_time;
  ACE_hrtime_t eval_time;
  ACE_hrtime_t total = test_latency (knowledge, iterations, compile_time, eval_time);
  
  ACE_DEBUG ((LM_INFO, "(%P|%t) (%d of %d) finished dissemination of %d * 2 events\n",
                        id, processes, iterations));

  //ACE_DEBUG ((LM_INFO, "(%P|%t) (%d of %d) waiting for other processes to stop\n",
  //                      id, processes, iterations));

  // find the average event latency. Since round trips were used, this is total latencies
  // divided by iterations divided by 2.
  uint64_t latency_avg = (total / iterations) / 2;

  ACE_DEBUG ((LM_INFO, "(%P|%t) (%d of %d) Total latency was %s ns\n",
                        id, processes, ull_to_string (total).c_str ()));

  // set the role
  type = id == 0 ? "P0" : "P1";

  ACE_DEBUG ((LM_INFO, 
    "\n=========================================================================\n\n"));


  {
    std::stringstream buffer;

    std::locale loc(""); 
    buffer.imbue (loc); 

    buffer << " ";
    buffer << type;
    buffer << " Compile time";
    buffer << "\t\t";
    buffer << std::setw (33);
    buffer << compile_time;
    buffer << " ns\n";

    buffer << " ";
    buffer << type;
    buffer << " Eval time   ";
    buffer << "\t\t";
    buffer << std::setw (33);
    buffer << eval_time;
    buffer << " ns\n";

    buffer << " ";
    buffer << type;
    buffer << " Diss Latency";
    buffer << "\t\t";
    buffer << std::setw (33);
    buffer << latency_avg;
    buffer << " ns\n\n";

    ACE_DEBUG ((LM_INFO, 
      buffer.str ().c_str ()));
  }

  ACE_DEBUG ((LM_INFO, 
    "\n=========================================================================\n\n"));


  ACE_DEBUG ((LM_INFO, "(%P|%t) (%d of %d) waiting for other processes to stop\n",
                        id, processes, iterations));

  // wait for everyone to stop
  knowledge.wait (build_wait_string (id, "stopped", processes));
  knowledge.set ("stopped");

  ACE_DEBUG ((LM_INFO, "(%P|%t) Final Knowledge\n"));
  knowledge.print_knowledge ();

  ACE_DEBUG ((LM_INFO, "(%P|%t) Exiting\n"));
  return 0;
}



int parse_args (int argc, ACE_TCHAR * argv[])
{
  // options string which defines all short args
  ACE_TCHAR options [] = ACE_TEXT ("i:s:p:o:v:n:h");

  // create an instance of the command line args
  ACE_Get_Opt cmd_opts (argc, argv, options);

  // set up an alias for '-n' to be '--name'
  cmd_opts.long_option (ACE_TEXT ("id"), 'i', ACE_Get_Opt::ARG_REQUIRED);
  cmd_opts.long_option (ACE_TEXT ("iterations"), 'n', ACE_Get_Opt::ARG_REQUIRED);
  cmd_opts.long_option (ACE_TEXT ("stop"), 's', ACE_Get_Opt::ARG_REQUIRED);
  cmd_opts.long_option (ACE_TEXT ("processes"), 'p', ACE_Get_Opt::ARG_REQUIRED);
  cmd_opts.long_option (ACE_TEXT ("help"), 'h', ACE_Get_Opt::ARG_REQUIRED);
  cmd_opts.long_option (ACE_TEXT ("host"), 'o', ACE_Get_Opt::ARG_REQUIRED);
 
  // temp for current switched option
  int option;

  // used in processing std::stringstream stuff
  std::string extra;
//  ACE_TCHAR * arg;

  // iterate through the options until done
  while ((option = cmd_opts ()) != EOF)
  {
    std::stringstream buffer;
    //arg = cmd_opts.opt_arg ();
    switch (option)
    {
    case 'i':
      // thread number
      buffer << cmd_opts.opt_arg ();
      buffer >> id;
      break;
    case 'n':
      // thread number
      buffer << cmd_opts.opt_arg ();
      buffer >> iterations;
      break;
    case 's':
      // stop value
      buffer << cmd_opts.opt_arg ();
      buffer >> stop;
      break;
    case 'p':
      // changing the processes is not allowed
      break;
    case 'o':
      host = cmd_opts.opt_arg ();
      ACE_DEBUG ((LM_INFO, "(%P|%t) host set to %s\n", host.c_str ()));
      break;
    case ':':
      ACE_ERROR_RETURN ((LM_ERROR, 
        ACE_TEXT ("ERROR: -%c requires an argument"), 
           cmd_opts.opt_opt ()), -2); 
    case 'h':
    default:
      ACE_DEBUG ((LM_INFO, "Program Options:      \n\
      -p (--processes) number of processes (disabled - always 2) \n\
      -i (--id)        set process id (0 default)  \n\
      -n (--iterations) set number of iterations (100,000 by default)  \n\
      -s (--stop)      stop condition (10 default) \n\
      -o (--host)      this host ip/name (localhost default) \n\
      -h (--help)      print this menu             \n"));
      ACE_ERROR_RETURN ((LM_ERROR, 
        ACE_TEXT ("Returning from Help Menu")), -1); 
      break;
    }
  }

  return 0;
}
