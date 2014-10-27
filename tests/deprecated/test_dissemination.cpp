
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
Madara::Knowledge_Record::Integer processes = 1;
Madara::Knowledge_Record::Integer stop = 10;
Madara::Knowledge_Record::Integer value = 0;
Madara::Knowledge_Record::Integer iterations = 100000;
// 1Mhz rate
Madara::Knowledge_Record::Integer rate = 1000000;

// test is 3 minutes long
Madara::Knowledge_Record::Integer time_limit = 3 * 60 * 
  Madara::Knowledge_Record::Integer (1000000000);

std::string host = "";

volatile bool terminated = false;

// command line arguments
int parse_args (int argc, ACE_TCHAR * argv[]);

// signal handler for someone hitting control+c
extern "C" void terminate (int)
{
  terminated = true;
}

std::string 
to_legible_hertz (Madara::Knowledge_Record::Integer hertz)
{
  std::stringstream buffer;

  std::locale loc(""); 
  buffer.imbue (loc); 

  const int ghz_mark = 1000000000;
  const int mhz_mark = 1000000;
  const int khz_mark = 1000;

  double freq = (double) hertz / ghz_mark;

  if (freq >= 1)
  {
    buffer << std::setprecision (2) << std::fixed;
    buffer << freq;
    buffer << " ghz";
    return buffer.str ().c_str ();
  }

  freq = (double) hertz / mhz_mark;

  if (freq >= 1)
  {
    buffer << std::setprecision (2) << std::fixed;
    buffer << freq;
    buffer << " mhz";
    return buffer.str ().c_str ();
  }

  freq = (double) hertz / khz_mark;

  if (freq >= 1)
  {
    buffer << std::setprecision (2) << std::fixed;
    buffer << freq;
    buffer << " khz";
    return buffer.str ().c_str ();
  }

  freq = (double) hertz;

  buffer << std::setprecision (2) << std::fixed;
  buffer << freq;
  buffer << "  hz";
  return buffer.str ().c_str ();
  
}

ACE_Time_Value
rate_to_sleep_time (Madara::Knowledge_Record::Integer rate)
{
  ACE_Time_Value sleep_time (0, 0);
  
  // 1,000,000,000 ns in 1 second. This should be the best number
  // to divide by. 
  uint32_t nanoseconds = 100000000 / (uint32_t)rate;
  uint32_t microseconds = nanoseconds / 1000;

  // if rate was too fast, make it at least 1 microsecond
  if (microseconds == 0)
    ++microseconds;

  // setup the microseconds
  sleep_time.usec (microseconds);

  std::stringstream buffer;

  // set our attribute to 1
  buffer << "Rate ";
  buffer << rate;
  buffer << " hz >> ";
  buffer << microseconds;
  buffer << " us";

  ACE_DEBUG ((LM_INFO, "(%P|%t) %s\n",
    buffer.str ().c_str ()));

  return sleep_time;
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

void
broadcast (Madara::Knowledge_Engine::Knowledge_Base & knowledge, 
           Madara::Knowledge_Record::Integer iterations)
{
  ACE_Time_Value sleep_time = rate_to_sleep_time (rate);


  #ifdef WIN32
    // windows does not have microsecond timer precision so we have to improvise
    // j is used to accumulate the number of microsecond sleeps that we've missed
    // until we reach 1ms in time that should be slept

    // so, we're supposed to sleep "backed_inc" every time we publish something.
    // "backed_cur" is the amount of sleep we haven't taken that we need to
    // "backed_limit is 1,000 microseconds (i.e. 1ms), the upper limit

    uint64_t backed_cur = 0;
    uint64_t backed_inc;
    uint64_t backed_limit = 1000;

    // store the microseconds per publish into backed_inc
    sleep_time.to_usec (backed_inc);

    // reset the sleep time to 1ms, since Windows can't be any more precise
    sleep_time.sec (0);
    sleep_time.usec (0);
    sleep_time.set_msec (1);

    ACE_DEBUG ((LM_DEBUG, 
      "(%P|%t) Windows detected. Running in bursty mode\n"));
  #endif

  for (Madara::Knowledge_Record::Integer i = 1; i <= iterations && !terminated; ++i)
  {
    knowledge.set ("info", i);

    // if we're on windows, only sleep if we're over the 1ms limit 
    #ifdef WIN32
      if (backed_cur >= backed_limit)
      {
    #endif // if WIN32

    ACE_OS::sleep (sleep_time);

    // if we're on windows, reset the backed_cur to 0, since we just
    // slept, or increment the backed_cur by backed_inc
    #ifdef WIN32
        backed_cur = 0;
        ACE_DEBUG ((LM_DEBUG, 
          "(%P|%t) Windows detected. Sleeping and resetting backed_cur.\n"));
      }
      else
      {
        backed_cur += backed_inc;
        ACE_DEBUG ((LM_DEBUG, 
          "(%P|%t) Windows detected. backed_cur = %d\n", backed_cur));
      }
    #endif // if WIN32
  }

  //if (terminated)
  // set terminated just in case, so everyone cleans up cleanly
  knowledge.set ("terminated");
}

int ACE_TMAIN (int argc, ACE_TCHAR * argv[])
{
  int retcode = parse_args (argc, argv);

  if (retcode < 0)
    return retcode;

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

  Madara::Transport::Settings settings;
  settings.type = Madara::Transport::MULTICAST;
  settings.domains = "KaRL_Dissemination";
  settings.queue_length = 12000000;
  settings.reliability = Madara::Transport::BEST_EFFORT;

  Madara::Knowledge_Engine::Knowledge_Base knowledge(host, settings);

  // set my id
  knowledge.set (".self", id);
  knowledge.set (".processes", processes);
  knowledge.set (".iterations", iterations);

  std::string type;

  ACE_DEBUG ((LM_INFO, "(%P|%t) (%d of %d) waiting for other processes to join\n",
                        id, processes, iterations));

  // build a logic based on the started attribute and then wait
  // for all processes to start
  knowledge.wait (build_wait_string (id, "started", processes));
  knowledge.set ("started");

  ACE_DEBUG ((LM_INFO, "(%P|%t) (%d of %d) starting dissemination of %d events\n",
                        id, processes, iterations));

  // at this point, all processes are accounted for. Let's begin our tests.

  // keep track of time
  ACE_hrtime_t measured;
  ACE_High_Res_Timer timer;

  // everyone starts their timer
  timer.start ();

  if (id == 0)
  {
    type = "Publisher";
    // root process is the broadcaster
    // Let's make 1,000,000 modifications and broadcast to everyone
    broadcast (knowledge, iterations);
  }
  else
  {
    type = "Subscriber";
    // everyone else is a receiver
    knowledge.wait ("info >= .iterations || terminated");
  }

  timer.stop ();
  timer.elapsed_time (measured);

  ACE_DEBUG ((LM_INFO, "(%P|%t) (%d of %d) finished dissemination of %d events\n",
                        id, processes, iterations));

  //ACE_DEBUG ((LM_INFO, "(%P|%t) (%d of %d) waiting for other processes to stop\n",
  //                      id, processes, iterations));

  // find the dissemination rate per process
  uint64_t hertz = ((uint64_t) 1000000000 * iterations)
                               / measured;


  ACE_DEBUG ((LM_INFO, 
    "\n=========================================================================\n\n"));


  {
    std::stringstream buffer;
    buffer << " ";
    buffer << type;
    buffer << "\t\t";
    buffer << std::setw (33);
    buffer << to_legible_hertz (hertz);
    buffer << "\n\n";

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
  ACE_TCHAR options [] = ACE_TEXT ("i:s:p:o:v:n:r:h");

  // create an instance of the command line args
  ACE_Get_Opt cmd_opts (argc, argv, options);

  // set up an alias for '-n' to be '--name'
  cmd_opts.long_option (ACE_TEXT ("id"), 'i', ACE_Get_Opt::ARG_REQUIRED);
  cmd_opts.long_option (ACE_TEXT ("iterations"), 'n', ACE_Get_Opt::ARG_REQUIRED);
  cmd_opts.long_option (ACE_TEXT ("stop"), 's', ACE_Get_Opt::ARG_REQUIRED);
  cmd_opts.long_option (ACE_TEXT ("processes"), 'p', ACE_Get_Opt::ARG_REQUIRED);
  cmd_opts.long_option (ACE_TEXT ("help"), 'h', ACE_Get_Opt::ARG_REQUIRED);
  cmd_opts.long_option (ACE_TEXT ("host"), 'o', ACE_Get_Opt::ARG_REQUIRED);
  cmd_opts.long_option (ACE_TEXT ("rate"), 'r', ACE_Get_Opt::ARG_REQUIRED);
 
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
      // processes
      buffer << cmd_opts.opt_arg ();
      buffer >> processes;
      break;
    case 'r':


      ACE_DEBUG ((LM_INFO, "(%P|%t) Changing rate\n"));

      // publication rate
      buffer << cmd_opts.opt_arg ();

      ACE_DEBUG ((LM_INFO, "(%P|%t) Rate string is %s\n", 
        buffer.str ().c_str ()));

      buffer >> rate;
      buffer >> extra;

      ACE_DEBUG ((LM_INFO, "(%P|%t) Rate is %u, extra is %s\n", 
        rate, extra.c_str ()));

      // convert the extra into lowercase
      extra = Madara::Utility::lower (extra);

      if (extra != "")
      {
        if      (extra == "ghz")
        {
          ACE_DEBUG ((LM_INFO, "(%P|%t) ghz change rate detected\n"));

          rate *= 1000000000;
        }
        else if (extra == "mhz")
        {
          ACE_DEBUG ((LM_INFO, "(%P|%t) mhz change rate detected\n"));

          rate *= 1000000;
        }
        else if (extra == "khz")
        {
          ACE_DEBUG ((LM_INFO, "(%P|%t) khz change rate detected\n"));

          rate *= 1000;
        }
      }

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
      -p (--processes) number of processes that will be running\n\
      -i (--id)        set process id (0 default)  \n\
      -n (--iterations) set number of iterations (100,000 by default)  \n\
      -s (--stop)      stop condition (10 default) \n\
      -o (--host)      this host ip/name (localhost default) \n\
      -r (--rate)      the publication rate \n\
      -h (--help)      print this menu             \n"));
      ACE_ERROR_RETURN ((LM_ERROR, 
        ACE_TEXT ("Returning from Help Menu")), -1); 
      break;
    }
  }

  return 0;
}
