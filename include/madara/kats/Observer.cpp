
//#define MADARA_NTRACE    0
//#define ACE_NTRACE    0
////#define ACE_NLOGGING  0
//#define ACE_NDEBUG    0

#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <assert.h>
#include <stdio.h>
#include <iomanip>

#include "ace/Log_Msg.h"
#include "ace/Get_Opt.h"
#include "ace/High_Res_Timer.h"
#include "ace/OS_NS_Thread.h"
#include "ace/Sched_Params.h"
#include "ace/Process.h"
#include "ace/Signal.h"

#include "madara/utility/LogMacros.h"
#include "madara/utility/Utility.h"
#include "madara/utility/tinyxml.h"
#include "madara/kats/TestFramework.h"

madara::kats::Settings settings;
ACE_Process_Options process_options;
std::string test_name;
std::string process_name;
std::stringstream command_line;
std::string print_statement;
bool realtime = false;
bool process_set = false;
bool test_set = false;
bool kill_time_set = false;
bool delay_time_set = false;
bool debug_printing = false;
bool timing = false;
ACE_Time_Value kill_time (0);
ACE_Time_Value delay_time (0);

// default is SIGTERM, though we use terminate
// unless a signal is set to be more portable
bool signal_set = false;
int kill_signal = 15;

std::string pre_condition;
std::string post_condition;
std::string post_delay;
std::string post_launch;

volatile bool terminated = 0;

// command line arguments
int parse_args (int argc, ACE_TCHAR * argv[]);

// signal handler for someone hitting control+c
extern "C" void terminate (int)
{
  terminated = true;
}

int
process_transport_file (const std::string & file)
{
  // read the file
  TiXmlDocument doc (file);

  if (!doc.LoadFile ())
  {
    ACE_DEBUG ((LM_INFO, 
      "KATS_BATCH:  Unable to open transport file %s\n", file.c_str ()));
    return -2;
  }

  TiXmlElement * root  = doc.FirstChildElement ("transport");
  TiXmlElement * current = 0;

  if (root)
  {
    current = root->FirstChildElement ("type");

    if (current && current->GetText ())
    {
      std::string value (current->GetText ());
      madara::Utility::lower (value);

      if      ("ndds"      == value)
        settings.type = madara::transport::NDDS;
      else if ("splice"    == value)
        settings.type = madara::transport::SPLICE;
      else if ("none"      == value)
        settings.type = madara::transport::NO_TRANSPORT;
    }

    current = root->FirstChildElement ("persistence");
    
    if (current && current->GetText ())
    {

    }

  }
  else
    return -1;
  
  return 0;
}

int ACE_TMAIN (int argc, ACE_TCHAR * argv[])
{
  // timers relating to launching process  
  ACE_High_Res_Timer barrier_timer;
  ACE_High_Res_Timer allkatsconditions_timer;
  ACE_High_Res_Timer allconditions_timer;
  ACE_High_Res_Timer starttofinish_timer;
  ACE_High_Res_Timer process_timer;

  // for complete timing
  starttofinish_timer.start ();

  // signal handler for clean exit
  ACE_Sig_Action sa ((ACE_SignalHandler) terminate, SIGINT);

  // set the default working directory to the current directory
  process_options.working_directory (".");

  // by default, see LM_INFO and LM_ERROR logging messages
  // the user has to provide -g to see LM_DEBUG
  ACE_LOG_MSG->priority_mask (LM_INFO | LM_ERROR, ACE_Log_Msg::PROCESS);

  // parse any arguments
  int retcode = parse_args (argc, argv);

  // for returning back to the user after process spawn
  int return_value = 0;

  if (retcode < 0)
    return retcode;

  if (realtime)
  {
    // use ACE real time scheduling class
    int prio  = ACE_Sched_Params::next_priority
      (ACE_SCHED_FIFO,
       ACE_Sched_Params::priority_max (ACE_SCHED_FIFO),
       ACE_SCOPE_THREAD);
    ACE_OS::thr_setprio (prio);
  }

  madara::kats::TestFramework testing (settings);

  testing.event (
    "updatevars","",".kats.id=.madara.id;.kats.processes=.madara.processes");

  if(debug_printing)
    testing.dump ();

  // start timers for conditions related to KATS for record keeping
  allconditions_timer.start ();
  allkatsconditions_timer.start ();
  barrier_timer.start ();

  // handle the case where there is no barrier name. Barrier now
  // handles case where processes <= 1
  if (test_name != "")
    testing.barrier (test_name);

  // stop the barrier timer
  barrier_timer.stop ();

  // Before we check for delay, we first check for a precondition

  if (pre_condition != "")
  {
    std::stringstream buffer;
    buffer << test_name;
    buffer << ".pre.";
    buffer << "{.madara.id}";

    std::stringstream cond_buffer;
    cond_buffer << ".kats.precondition=";
    cond_buffer << pre_condition;

    testing.event (buffer.str (), "", cond_buffer.str (), "");
  }

  allkatsconditions_timer.stop ();

  // sleep for a set amount of time after the barrier (if specified)
  if (delay_time_set)
  {
    ACE_OS::sleep (delay_time);
  }

  if (post_delay != "")
  {
    std::stringstream buffer;
    buffer << test_name;
    buffer << ".post_delay.";
    buffer << "{.madara.id}";

    std::stringstream cond_buffer;
    cond_buffer << ".kats.postdelay=";
    cond_buffer << post_delay;

    testing.event (buffer.str (), "", cond_buffer.str (), "");
  }

  // stop the clock for all conditions (including OS temporal one)
  allconditions_timer.stop ();

  if(debug_printing)
    testing.dump ();

  std::string statement;

  ACE_Time_Value to_sleep = delay_time;
  time_t max_elapsed = kill_time.sec ();

  // clock the process time
  process_timer.start ();

  MADARA_DEBUG (MADARA_LOG_MAJOR_EVENT, (LM_DEBUG, 
      DLINFO "KATS_OBS: Beginning observation. Kill time set to %Q seconds\n",
      max_elapsed));

  // post launch
  if (post_launch != "")
  {
    std::stringstream buffer;
    buffer << test_name;
    buffer << ".post_launch.";
    buffer << "{.madara.id}";

    std::stringstream cond_buffer;
    cond_buffer << ".kats.postlaunch=";
    cond_buffer << post_launch;

    testing.event (buffer.str (), "", cond_buffer.str (), "");
  }

  // termination is done via signalling from the user (Control+C)
  while (!terminated)
  {
    // make sure we at least sleep for 1 second or we will constantly spin
    if (to_sleep.sec () == 0)
      to_sleep.sec (1);

    ACE_Time_Value process_elapsed;

    if (print_statement == "")
      testing.dump ();
    else
      testing.log (print_statement, 0);

    // sleep for a set amount of time after the barrier (if specified)
    ACE_OS::sleep (to_sleep);

    process_timer.stop ();
    process_timer.elapsed_time (process_elapsed);

    // for older compilers that don't optimize functions out
    time_t cur_elapsed = process_elapsed.sec ();

    MADARA_DEBUG (MADARA_LOG_MAJOR_EVENT, (LM_DEBUG, 
        DLINFO "KATS_OBS: time is at %Q of %Q\n",
        cur_elapsed, max_elapsed));

    if (cur_elapsed >= max_elapsed)
      terminated = true;
    else if(max_elapsed - cur_elapsed < delay_time.sec ())
      to_sleep.sec (max_elapsed - cur_elapsed);

  }

  process_timer.stop ();

  if (post_condition != "")
  {
    std::stringstream buffer;
    buffer << test_name;
    buffer << ".post.";
    buffer << "{.madara.id}";

    std::stringstream cond_buffer;
    cond_buffer << ".kats.postcondition=";
    cond_buffer << post_condition;

    testing.event (buffer.str (), "", cond_buffer.str (), "");
  }

  starttofinish_timer.stop ();

  if(debug_printing)
  {
    MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_INFO, 
      DLINFO "KATS_OBS: Printing final knowledge values\n"));

    testing.dump ();

    if (timing)
    {
      ACE_hrtime_t barrier_elapsed;
      ACE_hrtime_t allkatsconditions_elapsed;
      ACE_hrtime_t allconditions_elapsed;
      ACE_hrtime_t starttofinish_elapsed;

      barrier_timer.elapsed_time (barrier_elapsed);
      allkatsconditions_timer.elapsed_time (allkatsconditions_elapsed);
      allconditions_timer.elapsed_time (allconditions_elapsed);
      starttofinish_timer.elapsed_time (starttofinish_elapsed);

      MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_INFO, 
        DLINFO "KATS_OBS: Barrier took %Q ns\n",
        barrier_elapsed ));
      MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_INFO, 
        DLINFO "KATS_OBS: All KATS conditions took %Q ns\n",
        allkatsconditions_elapsed ));
      MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_INFO, 
        DLINFO "KATS_OBS: All conditions took %Q ns\n",
        allconditions_elapsed ));
      MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_INFO, 
        DLINFO "KATS_OBS: From start to finish took %Q ns\n",
        starttofinish_elapsed ));
    }
  }

  if (timing)
  {
    ACE_hrtime_t process_elapsed;
    process_timer.elapsed_time (process_elapsed);

    MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_INFO, 
      DLINFO "KATS_OBS: %s runtime was %Q ns\n",
      process_name.c_str (), process_elapsed ));
  }

  return 0;
}



int parse_args (int argc, ACE_TCHAR * argv[])
{
  // options string which defines all short args
  ACE_TCHAR options [] = ACE_TEXT ("0:1:2:8:9:n:i:o:e:w:l:t:x:a:c:d:b:p:s:k:v:y:z:grmh");

  // create an instance of the command line args
  ACE_Get_Opt cmd_opts (argc, argv, options);

  // set up an alias for '-n' to be '--name'
  cmd_opts.long_option (ACE_TEXT ("stdin"), '0', ACE_Get_Opt::ARG_REQUIRED);
  cmd_opts.long_option (ACE_TEXT ("stdout"), '1', ACE_Get_Opt::ARG_REQUIRED);
  cmd_opts.long_option (ACE_TEXT ("stderr"), '2', ACE_Get_Opt::ARG_REQUIRED);
  cmd_opts.long_option (ACE_TEXT ("transfile"), '8', ACE_Get_Opt::ARG_REQUIRED);
  cmd_opts.long_option (ACE_TEXT ("transport"), '9', ACE_Get_Opt::ARG_REQUIRED);
  cmd_opts.long_option (ACE_TEXT ("testname"), 'a', ACE_Get_Opt::ARG_REQUIRED);
  cmd_opts.long_option (ACE_TEXT ("barriername"), 'a', ACE_Get_Opt::ARG_REQUIRED);
  cmd_opts.long_option (ACE_TEXT ("precondition"), 'b',
      ACE_Get_Opt::ARG_REQUIRED);
  cmd_opts.long_option (ACE_TEXT ("commandline"), 'c',
      ACE_Get_Opt::ARG_REQUIRED);
  cmd_opts.long_option (ACE_TEXT ("domain"), 'd', ACE_Get_Opt::ARG_REQUIRED);
  cmd_opts.long_option (ACE_TEXT ("environment"), 'e',
      ACE_Get_Opt::ARG_REQUIRED);
  cmd_opts.long_option (ACE_TEXT ("debug"), 'g', ACE_Get_Opt::NO_ARG);
  cmd_opts.long_option (ACE_TEXT ("help"), 'h', ACE_Get_Opt::NO_ARG);
  cmd_opts.long_option (ACE_TEXT ("id"), 'i', ACE_Get_Opt::ARG_REQUIRED);
  cmd_opts.long_option (ACE_TEXT ("signal"), 'k', ACE_Get_Opt::ARG_REQUIRED);
  cmd_opts.long_option (ACE_TEXT ("delay"), 'l', ACE_Get_Opt::ARG_REQUIRED);
  cmd_opts.long_option (ACE_TEXT ("timing"), 'm', ACE_Get_Opt::NO_ARG);
  cmd_opts.long_option (ACE_TEXT ("processes"), 'n', ACE_Get_Opt::ARG_REQUIRED);
  cmd_opts.long_option (ACE_TEXT ("host"), 'o', ACE_Get_Opt::ARG_REQUIRED);
  cmd_opts.long_option (ACE_TEXT ("print"), 'p', ACE_Get_Opt::ARG_REQUIRED);
  cmd_opts.long_option (ACE_TEXT ("realtime"), 'r', ACE_Get_Opt::NO_ARG);
  cmd_opts.long_option (ACE_TEXT ("postcondition"), 's',
      ACE_Get_Opt::ARG_REQUIRED);
  cmd_opts.long_option (ACE_TEXT ("killtime"), 't', ACE_Get_Opt::ARG_REQUIRED);
  cmd_opts.long_option (ACE_TEXT ("working"), 'w', ACE_Get_Opt::ARG_REQUIRED);
  cmd_opts.long_option (ACE_TEXT ("executable"), 'x',
      ACE_Get_Opt::ARG_REQUIRED);
  cmd_opts.long_option (ACE_TEXT ("loglevel"), 'v', ACE_Get_Opt::ARG_REQUIRED);
  cmd_opts.long_option (ACE_TEXT ("postdelay"), 'y', ACE_Get_Opt::ARG_REQUIRED);
  cmd_opts.long_option (ACE_TEXT ("postlaunch"), 'z', ACE_Get_Opt::ARG_REQUIRED);
 
  // temp for current switched option
  int option;

  // iterate through the options until done
  while ((option = cmd_opts ()) != EOF)
  {
    //arg = cmd_opts.opt_arg ();
    switch (option)
    {
    case '0':
      // redirecting stdin

      freopen (cmd_opts.opt_arg (), "r", stdin);

      MADARA_DEBUG (MADARA_LOG_MINOR_EVENT, (LM_DEBUG, 
        DLINFO "KATS_OBSERVER: stdin redirected from %s\n",
        cmd_opts.opt_arg ()));

      break;
    case '1':
      // redirecting stdout

      freopen (cmd_opts.opt_arg (), "w", stdout);

      MADARA_DEBUG (MADARA_LOG_MINOR_EVENT, (LM_DEBUG, 
        DLINFO "KATS_OBSERVER: stdout redirected to %s\n",
        cmd_opts.opt_arg ()));

      break;
    case '2':
      // redirecting stderr
      freopen (cmd_opts.opt_arg (), "w", stderr);

      MADARA_DEBUG (MADARA_LOG_MINOR_EVENT, (LM_DEBUG, 
        DLINFO "KATS_OBSERVER: stderr redirected to %s\n",
        cmd_opts.opt_arg ()));

      break;
    case '8':
      MADARA_DEBUG (MADARA_LOG_MINOR_EVENT, (LM_DEBUG, 
        DLINFO "KATS_BATCH: reading transport settings from %s\n",
        cmd_opts.opt_arg ()));

      process_transport_file (cmd_opts.opt_arg ());
      
      break;
    case '9':
      // transport protocol
      {
        std::stringstream buffer;
        buffer << cmd_opts.opt_arg ();
        buffer >> settings.type;
      }

      MADARA_DEBUG (MADARA_LOG_MINOR_EVENT, (LM_DEBUG, 
        DLINFO "KATS_OBSERVER: transport protocol set to %u\n",
        settings.type));

      break;
    case 'a':
      // thread number
      test_name = cmd_opts.opt_arg ();
      test_set = true;

      MADARA_DEBUG (MADARA_LOG_MINOR_EVENT, (LM_DEBUG, 
        DLINFO "KATS_OBSERVER: test name set to %s\n",
        test_name.c_str ()));

      break;
    case 'b':
      // a precondition

      pre_condition = cmd_opts.opt_arg ();

      MADARA_DEBUG (MADARA_LOG_MINOR_EVENT, (LM_DEBUG, 
        DLINFO "KATS_OBSERVER: precondition set to %s\n",
        pre_condition.c_str ()));

      break;
    case 'c':
      // the command line arguments

      //process_options.command_line ("%s", cmd_opts.opt_arg ());

      if (cmd_opts.opt_arg ())
        command_line << cmd_opts.opt_arg ();

      MADARA_DEBUG (MADARA_LOG_MINOR_EVENT, (LM_DEBUG, 
        DLINFO "KATS_OBSERVER: command line set to %s\n",
        command_line.str ().c_str ()));

      break;
    case 'd':
      // the knowledge domain

      settings.domains = cmd_opts.opt_arg ();

      MADARA_DEBUG (MADARA_LOG_MINOR_EVENT, (LM_DEBUG, 
        DLINFO "KATS_OBSERVER: knowledge domain set to %s\n",
        settings.domains.c_str ()));

      break;
    case 'e':
      // environment for process

      process_options.setenv (cmd_opts.opt_arg (), 
        strlen (cmd_opts.opt_arg ()));

      MADARA_DEBUG (MADARA_LOG_MINOR_EVENT, (LM_DEBUG, 
        DLINFO "KATS_OBSERVER: environment set to %s\n",
        cmd_opts.opt_arg ()));

      break;
    case 'g':
      // thread number
      debug_printing = true;

      if(debug_printing)
        ACE_LOG_MSG->priority_mask (LM_DEBUG | LM_INFO | LM_ERROR, ACE_Log_Msg::PROCESS);

      MADARA_DEBUG (MADARA_LOG_MINOR_EVENT, (LM_DEBUG, 
        DLINFO "KATS_OBSERVER: enabling debug printing\n"));

      break;
    case 'i':
      // id
      {
        std::stringstream buffer;
        buffer << cmd_opts.opt_arg ();
        buffer >> settings.id;
      }

      MADARA_DEBUG (MADARA_LOG_MINOR_EVENT, (LM_DEBUG, 
        DLINFO "KATS_OBSERVER: id in test process ring set to %u\n",
        settings.id));

      break;
    case 'k':
      // kill signal
      {
        std::stringstream buffer;
        buffer << cmd_opts.opt_arg ();
        buffer >> kill_signal;
        signal_set = true;
      }

      MADARA_DEBUG (MADARA_LOG_MINOR_EVENT, (LM_DEBUG, 
        DLINFO "KATS_OBSERVER: signal to kill executable set to %d\n",
        kill_signal));

      break;
    case 'l':
      // time to delay after barrier before starting the process
      {
        time_t time_in_seconds;
        std::stringstream buffer;
        buffer << cmd_opts.opt_arg ();
        buffer >> time_in_seconds;

        delay_time.sec (time_in_seconds);
        delay_time_set = true;
      }

      MADARA_DEBUG (MADARA_LOG_MINOR_EVENT, (LM_DEBUG, 
        DLINFO "KATS_OBSERVER: delay executable by %: seconds\n",
        delay_time.sec ()));

      break;
    case 'm':
      // print timing information?
      timing = true;

      MADARA_DEBUG (MADARA_LOG_MINOR_EVENT, (LM_DEBUG, 
        DLINFO "KATS_PROCESS: printing timing information\n",
        settings.processes));

      break;
    case 'n':
      // processes
      {
        std::stringstream buffer;
        buffer << cmd_opts.opt_arg ();
        buffer >> settings.processes;
      }

      MADARA_DEBUG (MADARA_LOG_MINOR_EVENT, (LM_DEBUG, 
        DLINFO "KATS_OBSERVER: processes involved in test set to %u\n",
        settings.processes));

      break;
    case 'o':
      // host name
      settings.host = cmd_opts.opt_arg ();

      MADARA_DEBUG (MADARA_LOG_MINOR_EVENT, (LM_DEBUG, 
        DLINFO "KATS_OBSERVER: host set to %s\n",
        settings.host.c_str ()));

      break;
    case 'p':
      // special print statement

      {
        std::stringstream buffer;
        buffer << cmd_opts.opt_arg ();
        buffer << "\n";
        print_statement = buffer.str ();
      }

      MADARA_DEBUG (MADARA_LOG_MINOR_EVENT, (LM_DEBUG, 
        DLINFO "KATS_OBSERVER: print statement set to %s\n",
        cmd_opts.opt_arg ()));

      break;
    case 'r':
      // enable realtime scheduling
      realtime = true;

      MADARA_DEBUG (MADARA_LOG_MINOR_EVENT, (LM_DEBUG, 
        DLINFO "KATS_OBSERVER: enabling realtime scheduling\n"));

      break;
    case 's':
      // a postcondition

      post_condition = cmd_opts.opt_arg ();

      MADARA_DEBUG (MADARA_LOG_MINOR_EVENT, (LM_DEBUG, 
        DLINFO "KATS_OBSERVER: postcondition set to %s\n",
        post_condition.c_str ()));

      break;
    case 't':
      // time to kill the process
      {
        int time_in_seconds;
        std::stringstream buffer;
        buffer << cmd_opts.opt_arg ();
        buffer >> time_in_seconds;

        kill_time.sec (time_in_seconds);
        kill_time_set = true;
      }

      MADARA_DEBUG (MADARA_LOG_MINOR_EVENT, (LM_DEBUG, 
        DLINFO "KATS_OBSERVER: kill executable after by %: seconds\n",
        kill_time.sec ()));

      break;
    case 'v':
      // log level
    {
      int debug_level;
      std::stringstream buffer;
      buffer << cmd_opts.opt_arg ();
      buffer >> debug_level;

      madara::logger::global_logger->set_level (debug_level);

      MADARA_DEBUG (MADARA_LOG_MINOR_EVENT, (LM_DEBUG,
        DLINFO "KATS_PROCESS: logging level set to %u\n",
        debug_level));
      }

      break;
    case 'w':
      // working directory for process

      process_options.working_directory (cmd_opts.opt_arg ());

      MADARA_DEBUG (MADARA_LOG_MINOR_EVENT, (LM_DEBUG, 
        DLINFO "KATS_OBSERVER: working directory set to %s\n",
        cmd_opts.opt_arg ()));

      break;
    case 'y':
      // a postdelay

      post_delay = cmd_opts.opt_arg ();

      MADARA_DEBUG (MADARA_LOG_MINOR_EVENT, (LM_DEBUG, 
        DLINFO "KATS_OBS: postdelay set to %s\n",
        post_delay.c_str ()));

      break;
    case 'z':
      // a postdelay

      post_launch = cmd_opts.opt_arg ();

      MADARA_DEBUG (MADARA_LOG_MINOR_EVENT, (LM_DEBUG, 
        DLINFO "KATS_OBS: postlaunch set to %s\n",
        post_launch.c_str ()));

    case ':':
      MADARA_ERROR_RETURN (MADARA_LOG_TERMINAL_ERROR, (LM_ERROR, 
        ACE_TEXT ("KATS_OBSERVER: ERROR: -%c requires an argument"),
           cmd_opts.opt_opt ()), -2); 
    default:
      {
        char c_option = (char) option;
        MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_DEBUG, 
          DLINFO "KATS_OBSERVER: unrecognized argument %c\n",
          c_option));

        MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_DEBUG, 
          DLINFO "KATS_OBSERVER: printing help...\n",
          c_option));
      }

    case 'h':
      MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_INFO,
"kats_observer options:    \n\
      -0 (--stdin)       redirect stdin from a file \n\
      -1 (--stdout)      redirect stdout to a file \n\
      -2 (--stderr)      redirect stderr to a file \n\
      -8 (--transfile)   read transport settings from a file \n\
      -9 (--transport)   use the specified transport protocol: \n\
                         0   ==  No transport \n\
                         1   ==  Open Splice DDS \n\
                         2   ==  NDDS         \n\
      -a (--testname)    name of the test (for barriers) \n\
         (--barriername) \n\
      -b (--precondition) precondition to wait for after barrier \n\
      -d (--domain)      knowledge domain to isolate knowledge into \n\
      -e (--environment) environment variables (\"key=value\") \n\
      -g (--debug)       prints KATS debug messages \n\
      -h (--help)        print this menu        \n\
      -i (--id)          this process id        \n\
      -l (--delay)       time delay (secs) after barrier to wait before spawn\n\
      -m (--timing)      print timing information \n\
                         (add -g for kats conditional timing) \n\
      -n (--processes)   number of testing processes \n\
      -o (--host)        host identifier        \n\
      -p (--print)       statement that the user is interested in watching\n\
                         This statement can be a KaRL logic and does perform\n\
                         variable expansion (e.g., {.i} would be 5 if .i=5)\n\
      -r (--realtime)    run the process with real time scheduling \n\
      -s (--postcondition) postcondition to set after process exits \n\
      -t (--timeout)     time in seconds to wait before killing \n\
         -k --signal     kill signal to send after timeout \n\
      -v (--loglevel)    maximum log level to print from MADARA messages\n\
      -w (--working)     working directory (default=.) \n\
      -y (--postdelay)   condition to evaluate after temporal delay and \n\
                         before postlaunch\n\
      -z (--postlaunch)  condition to evaluate after postdelay\n\
"
      ));
      MADARA_ERROR_RETURN (MADARA_LOG_TERMINAL_ERROR, (LM_ERROR, 
        ACE_TEXT ("Returning from Help Menu\n")), -1); 
      break;
    }
  }

  return 0;
}
