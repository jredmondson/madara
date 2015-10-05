
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <assert.h>
#include <iomanip>

#include "ace/ACE.h"
#include "ace/Log_Msg.h"
#include "ace/Get_Opt.h"
#include "ace/High_Res_Timer.h"
#include "ace/OS_NS_Thread.h"
#include "ace/Sched_Params.h"
#include "ace/Process.h"

#include "madara/kats/Test_Framework.h"
#include "madara/utility/tinyxml.h"
#include "madara/utility/Utility.h"
#include "madara/utility/Log_Macros.h"
#include "madara/logger/Global_Logger.h"

madara::kats::Settings settings;

// settings that may be true or false
bool realtime = false;
bool debug_printing = false;
bool run_in_parallel = false;

// flags for whether or not command line arguments
// were supplied
bool stdin_set = false;
bool stderr_set = false;
bool stdout_set = false;
bool process_set = false;
bool kill_time_set = false;
bool id_set = false;
bool processes_set = false;
bool host_set = false;
bool domain_set = false;
bool realtime_set = false;
bool debug_set = false;
bool pre_condition_set = false;
bool post_condition_set = false;
bool post_delay_set = false;
bool post_launch_set = false;
bool parallel_set = false;
bool delay_time_set = false;
bool test_name_set = false;
bool loglevel_set = false;
bool timing = false;
bool transport_set = false;

std::string test_name;
std::string pre_condition;
std::string post_condition;
std::string post_delay;
std::string post_launch;
std::string tests_file;
ACE_Time_Value kill_time (0);
ACE_Time_Value delay_time (0);

std::string path;
std::string working_dir;

/**
 * @class KATS_Process
 * @brief Builder class for setting up a call to one of the kats tools (i.e.,
 * kats_process, kats_batch, kats_observer).
 **/
class KATS_Process
{
public:
  KATS_Process ()
  {
    kats_type ("process");
  }

  KATS_Process (const KATS_Process & rhs)
    : process (), process_options (),
      executable (rhs.executable), command_line (rhs.command_line.str ())
  {
    // we're not allowed to copy an ACE_Process
  }
  
  ~KATS_Process ()
  {}

  KATS_Process & 
    operator= (const KATS_Process & rhs)
  {
    executable = rhs.executable;
    command_line.str (rhs.command_line.str ());
    //process_options = rhs.process_options;

    return *this;
  }

  void enable_realtime (void)
  {
    command_line << " -r";
  }

  void enable_debug (void)
  {
    command_line << " -g";
  }

  void enable_timing (void)
  {
    command_line << " -m";
  }

  void set_precondition (const std::string & value)
  {
    command_line << " -b \"";
    command_line << value;
    command_line << "\"";
  }

  void set_executable (const std::string & value)
  {
    command_line << " -x ";
    command_line << value;
  }

  void set_postcondition (const std::string & value)
  {
    command_line << " -s \"";
    command_line << value;
    command_line << "\"";
  }

  void set_postdelay (const std::string & value)
  {
    command_line << " -y \"";
    command_line << value;
    command_line << "\"";
  }

  void set_transport (const std::string & value)
  {
    command_line << " -p \"";
    command_line << value;
    command_line << "\"";
  }

  void set_transport_file (const std::string & value)
  {
    command_line << " -8 \"";
    command_line << value;
    command_line << "\"";
  }

  void set_postlaunch (const std::string & value)
  {
    command_line << " -z \"";
    command_line << value;
    command_line << "\"";
  }

  void set_commandline (const std::string & value)
  {
    command_line << " -c \"";
    command_line << value;
    command_line << "\"";
  }

  void set_killtime (const std::string & value)
  {
    command_line << " -t ";
    command_line << value;
  }

  void set_killsignal (const std::string & value)
  {
    command_line << " -k ";
    command_line << value;
  }

  void set_delaytime (const std::string & value)
  {
    command_line << " -l ";
    command_line << value;
  }

  void set_id (const std::string & value)
  {
    command_line << " -i ";
    command_line << value;
  }

  void set_host (const std::string & value)
  {
    command_line << " -o ";
    command_line << value;
  }

  void set_stdin (const std::string & value)
  {
    command_line << " -0 ";
    command_line << value;
  }

  void set_stdout (const std::string & value)
  {
    command_line << " -1 ";
    command_line << value;
  }

  void set_stderr (const std::string & value)
  {
    command_line << " -2 ";
    command_line << value;
  }

  void set_loglevel (const std::string & value)
  {
    command_line << " -v ";
    command_line << value;
  }

  void set_processes (const std::string & value)
  {
    command_line << " -n ";
    command_line << value;
  }

  void set_workingdir (const std::string & value)
  {
    command_line << " -w ";
    command_line << value;
  }

  void set_environment (const std::string & value)
  {
    command_line << " -e ";
    command_line << value;
  }

  void set_domain (const std::string & value)
  {
    command_line << " -d ";
    command_line << value;
  }

  void set_file (const std::string & value)
  {
    command_line << " -f ";
    command_line << value;
  }

  void set_testname (const std::string & value)
  {
    command_line << " -a ";
    command_line << value;
  }

  std::string
  query_setup (void)
  {
    std::stringstream buffer;
    buffer << executable;
    buffer << " ";
    buffer << command_line.str ();

    return buffer.str ();
  }

  void launch (void)
  {
    // setup the process options
    process_options.command_line ("%s", 
      query_setup ().c_str ());

    process.spawn (process_options);
  }

  int running (void)
  {
    return process.running ();
  }

  ACE_exitcode return_value (void)
  {
    return process.return_value ();
  }

  int terminate (void)
  {
    return process.terminate ();
  }

  pid_t wait (void)
  {
    return process.wait ();
  }

  pid_t wait (const ACE_Time_Value & value)
  {
    return process.wait (value);
  }

  void kats_type (const std::string & type)
  {
    std::stringstream buffer;
    buffer << madara::Utility::clean_dir_name (
      madara::Utility::expand_envs ("$(MADARA_ROOT)/bin/"));

    if (type == "process")
      buffer << "kats_process";
    else if (type == "group")
      buffer << "kats_batch";
    else if (type == "maml")
      buffer << "kats_maml";
    else if (type == "sleep")
      buffer << "kats_sleep";
    else if (type == "observer")
      buffer << "kats_observer";

    executable = buffer.str ();
  }

protected:

  ACE_Process process;
  ACE_Process_Options process_options;
  std::string executable;
  std::stringstream command_line;
};

std::string
extract_file (const std::string & name)
{
  std::string::size_type start = name.size () - 1;
  for (std::string::size_type i = 0; i < name.size (); ++i)
  {
    // check for directory delimiters and update start
    if (name[start] != '/' && name[start] != '\\')
    {
      break;
    }
    --start;
  }

  // return the substring from 0 with start number of elements
  return name.substr (start, name.size () - start);
}

std::string
extract_path (const std::string & name)
{
  std::string result;

  const char * c_result = ACE::dirname (name.c_str ());
  if (c_result)
    result = c_result;

  return result;
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

      transport_set = true;

      if      ("ndds"      == value)
        settings.type = madara::transport::NDDS;
      else if ("splice"    == value)
        settings.type = madara::transport::SPLICE;
      else if ("none"      == value)
        settings.type = madara::transport::NO_TRANSPORT;
      else
        transport_set = false;

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

// command line arguments
int parse_args (int argc, ACE_TCHAR * argv[]);


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

  path = extract_path (argv[0]);

  // by default, see LM_INFO and LM_ERROR logging messages
  // the user has to provide -g to see LM_DEBUG
  ACE_LOG_MSG->priority_mask (LM_INFO | LM_ERROR, ACE_Log_Msg::PROCESS);

  std::vector <KATS_Process> processes;

  // parse any arguments
  int retcode = parse_args (argc, argv);

  // for returning back to the user after process spawn

  if (retcode < 0)
    return retcode;

  if (tests_file == "")
  {
    MADARA_ERROR (MADARA_LOG_TERMINAL_ERROR, (LM_ERROR, DLINFO
      "\nKATS_BATCH:"\
      " no process group file was specified. Try -h or --help\n"));
    return -1;

  }

  working_dir = extract_path (tests_file);

  // read the file
  TiXmlDocument doc (tests_file);

  if (!doc.LoadFile ())
  {
    ACE_DEBUG ((LM_INFO, 
      "KATS_BATCH:  Unable to open file %s\n", tests_file.c_str ()));
    return -2;
  }

  TiXmlElement * el_tests = 0; 
  TiXmlElement * el_globals = 0; 
  TiXmlElement * element = 0;
  TiXmlAttribute * attribute = 0;

  el_tests  = doc.FirstChildElement ("group");;

  if (!el_tests)
  {
    ACE_DEBUG ((LM_INFO, 
      "KATS_BATCH:  group was not the root element\n"));
    return -2;
  }

  ACE_DEBUG ((LM_DEBUG, 
    "KATS_BATCH:  Reading process group setup...\n"));

  el_globals = el_tests->FirstChildElement ("setup");

  // we have a globals element
  if (el_globals)
  {
    // only load globals from the file if they were not
    // supplied via command line (command line has highest priority)

    if (!transport_set)
    {
      // check for stdout redirect
      element = el_globals->FirstChildElement ("transport");
      if (element && element->Attribute ("file"))
      {
        std::string value (madara::Utility::clean_dir_name (
          madara::Utility::expand_envs (element->Attribute ("file"))));

        if (value != "")
        {
          std::stringstream buffer;
          buffer << working_dir;
          buffer << "/";
          buffer << value;
          
          value = madara::Utility::clean_dir_name (buffer.str ().c_str ()); 

          ACE_DEBUG ((LM_DEBUG, 
            "KATS_BATCH:    Read transport file = %s from process group file\n",
               value.c_str ()));

          process_transport_file (value);
        }
      }
    }

    if (!stdout_set)
    {
      // check for stdout redirect
      element = el_globals->FirstChildElement ("stdout");
      if (element && element->GetText ())
      {
        // expand the string for environment variables, then clean the path up
        std::string value (madara::Utility::clean_dir_name (
          madara::Utility::expand_envs (element->GetText ())));

        if (value != "")
        {
          ACE_DEBUG ((LM_DEBUG, 
            "KATS_BATCH:    Read stdout redirect = %s from process group file\n",
               value.c_str ()));

          freopen (value.c_str (),"w", stdout);
        }
      }
    }

    if (!stderr_set)
    {
      // check for stderr redirect
      element = el_globals->FirstChildElement ("stderr");
      if (element && element->GetText ())
      {
        // expand the string for environment variables, then clean the path up
        std::string value (madara::Utility::clean_dir_name (
          madara::Utility::expand_envs (element->GetText ())));

        if (value != "")
        {
          ACE_DEBUG ((LM_DEBUG, 
            "KATS_BATCH:    Read stderr redirect = %s from process group file\n",
               value.c_str ()));

          freopen (value.c_str (),"w", stderr);
        }
      }
    }

    if (!stdin_set)
    {
      // check for stdin redirect
      element = el_globals->FirstChildElement ("stdin");
      if (element && element->GetText ())
      {
        // expand the string for environment variables, then clean the path up
        std::string value (madara::Utility::clean_dir_name (
          madara::Utility::expand_envs (element->GetText ())));

        if (value != "")
        {
          ACE_DEBUG ((LM_DEBUG, 
            "KATS_BATCH:    Read stdin redirect = %s from process group file\n",
            value.c_str ()));

          freopen (value.c_str (),"r", stdin);
        }
      }
    }
    if (!id_set)
    {
      element = el_globals->FirstChildElement ("id");
      if (element)
      {
        if (element->GetText ())
        {
          std::string temp = 
            madara::Utility::expand_envs (element->GetText ());
          if (temp != "")
          {
            std::stringstream buffer;
            buffer << madara::Utility::expand_envs (temp);
            buffer >> settings.id;

            ACE_DEBUG ((LM_DEBUG, 
              "KATS_BATCH:    Read id = %d from process group file\n",
              settings.id));
          }
        }
      } // if the id element existed
    } // if the user didn't specify id from the command line

    if (!domain_set)
    {
      element = el_globals->FirstChildElement ("domain");
      if (element)
      {
        if (element->GetText ())
        {
          std::string temp = 
            madara::Utility::expand_envs (element->GetText ());
          if (temp != "")
          {
            settings.domains = temp;
            domain_set = true;

            ACE_DEBUG ((LM_DEBUG, 
              "KATS_BATCH:    Read domain = %s from process group file\n",
              settings.domains.c_str ()));
          }
        }
      } // if the domain element existed
    } // if the user didn't specify domain from the command line

    if (!host_set)
    {
      element = el_globals->FirstChildElement ("host");
      if (element)
      {
        if (element->GetText ())
        {
          std::string temp = madara::Utility::expand_envs (element->GetText ());
          if (temp != "")
          {
            settings.host = temp;
            host_set = true;

            ACE_DEBUG ((LM_DEBUG, 
              "KATS_BATCH:    Read host = %s from group file\n",
              settings.host.c_str ()));
          }
        }
      } // if the host element existed
    } // if the user didn't specify host from the command line

    if (!processes_set)
    {
      element = el_globals->FirstChildElement ("processes");
      if (element)
      {
        if (element->GetText ())
        {
          std::string temp = 
            madara::Utility::expand_envs (element->GetText ());
          if (temp != "")
          {
            std::stringstream buffer;
            buffer << madara::Utility::expand_envs (temp);
            buffer >> settings.processes;

            ACE_DEBUG ((LM_DEBUG, 
              "KATS_BATCH:    Read processes = %d from group file\n",
              settings.processes));
          }
        }
      } // if the processes element existed
    } // if the user didn't specify processes from the command line

    if (!loglevel_set)
    {
      element = el_globals->FirstChildElement ("loglevel");
      if (element)
      {
        if (element->GetText ())
        {
          std::string temp = 
            madara::Utility::expand_envs (element->GetText ());
          if (temp != "")
          {
            int debug_level;
            std::stringstream buffer;
            buffer << madara::Utility::expand_envs (temp);
            buffer >> debug_level;

            madara::logger::global_logger->set_level (debug_level);

            MADARA_DEBUG (MADARA_LOG_MINOR_EVENT, (LM_DEBUG,
              DLINFO "KATS_BATCH: logging level set to %d\n",
              debug_level));

            loglevel_set = true;
          }
        }
      } // if the processes element existed
    } // if the user didn't specify processes from the command line

    if (!kill_time_set)
    {
      element = el_globals->FirstChildElement ("kill");
      if (element)
      {
        element = element->FirstChildElement ("time");
        if (element)
        {
          if (element->GetText ())
          {
            std::string temp = 
              madara::Utility::expand_envs (element->GetText ());
            if (temp != "")
            {
              time_t time_in_seconds;
              std::stringstream buffer;
              buffer << madara::Utility::expand_envs (temp);
              buffer >> time_in_seconds;

              kill_time.sec (time_in_seconds);
              kill_time_set = true;

              ACE_DEBUG ((LM_DEBUG,
                "KATS_BATCH:    Read kill time = %d s from process group\n",
                time_in_seconds));
            }
          }
        } // if element kill/time exists
      } // if element kill exists
    } // if the user didn't specify a kill time from the command line

    if (!pre_condition_set)
    {
      element = el_globals->FirstChildElement ("precondition");
      if (element)
      {
        if (element->GetText ())
        {
          std::string temp = 
            madara::Utility::expand_envs (element->GetText ());
          if (temp != "")
          {
            pre_condition = temp;

            ACE_DEBUG ((LM_DEBUG, 
              "KATS_BATCH:    Read precondition = %s from process group\n",
              pre_condition.c_str ()));
          }
        }
      } // if precondition element existed
    } // if the user didn't specify a precondition from the command line

    if (!post_condition_set)
    {
      element = el_globals->FirstChildElement ("postcondition");
      if (element)
      {
        if (element->GetText ())
        {
          std::string temp = 
            madara::Utility::expand_envs (element->GetText ());
          if (temp != "")
          {
            post_condition = temp;

            ACE_DEBUG ((LM_DEBUG, 
              "KATS_BATCH:    Read postcondition = %s from process group\n",
              post_condition.c_str ()));
          }
        }
      } // if postcondition element existed
    } // if the user didn't specify a postcondition from the command line

    if (!post_delay_set)
    {
      element = el_globals->FirstChildElement ("postdelay");
      if (element)
      {
        if (element->GetText ())
        {
          std::string temp = 
            madara::Utility::expand_envs (element->GetText ());
          if (temp != "")
          {
            post_delay = temp;

            ACE_DEBUG ((LM_DEBUG, 
              "KATS_BATCH:    Read postdelay = %s from process group\n",
              post_delay.c_str ()));
          }
        }
      } // if postcondition element existed
    } // if the user didn't specify a postcondition from the command line

    if (!test_name_set)
    {
      element = el_globals->FirstChildElement ("barrier");
      if (element)
      {
        std::string temp;
        if (element->Attribute ("name"))
          temp = madara::Utility::expand_envs (element->Attribute ("name"));
        else if (element->GetText ())
          temp = madara::Utility::expand_envs (element->GetText ());

        if (temp != "")
        {
          test_name = temp;

          ACE_DEBUG ((LM_DEBUG, 
            "KATS_BATCH:    Read barrier name = %s from process group file\n",
            test_name.c_str ()));
        }
      } // if name element existed
    } // if the user didn't specify a test name from the command line

    if (!realtime_set)
    {
      element = el_globals->FirstChildElement ("realtime");
      if (element)
      {
        realtime = true;
        realtime_set = true;
        ACE_DEBUG ((LM_DEBUG, 
          "KATS_BATCH:    Enabled realtime scheduling from process group file\n"));
      } // if realtime element existed
    } // if the user didn't specify real time scheduling from the command line

    if (!timing)
    {
      element = el_globals->FirstChildElement ("timing");
      if (element)
      {
        timing = true;
        ACE_DEBUG ((LM_DEBUG, 
          "KATS_BATCH:    Enabled printing of timing information " \
          "from process group file\n"));
      } // if realtime element existed
    } // if the user didn't specify real time scheduling from the command line

    if (!parallel_set)
    {
      element = el_globals->FirstChildElement ("parallel");
      if (element)
      {
        run_in_parallel = true;
        parallel_set = true;
        ACE_DEBUG ((LM_DEBUG, 
          "KATS_BATCH:    Enabled parallel execution from process group file\n"));
      } // if debug element existed
    } // if the user didn't specify debug mode from the command line

    if (!debug_set)
    {
      element = el_globals->FirstChildElement ("debug");
      if (element)
      {
        debug_printing = true;
        debug_set = true;
        ACE_DEBUG ((LM_DEBUG, 
          "KATS_BATCH:    Enabled debug mode printing from process group file\n"));
      } // if debug element existed
    } // if the user didn't specify debug mode from the command line
  }

  ACE_DEBUG ((LM_DEBUG, 
    "KATS_BATCH:  Finished reading tests setup. Proceeding to KATS setup...\n"));

  // *************** BEGIN KATS PRELIMINARY WORK ******************
  if (realtime)
  {
    ACE_DEBUG ((LM_DEBUG, 
      "KATS_BATCH:    Setting scheduling priority to max...\n"));

    // use ACE real time scheduling class
    int prio  = ACE_Sched_Params::next_priority
      (ACE_SCHED_FIFO,
       ACE_Sched_Params::priority_max (ACE_SCHED_FIFO),
       ACE_SCOPE_THREAD);
    ACE_OS::thr_setprio (prio);
  }

  ACE_DEBUG ((LM_DEBUG, 
    "KATS_BATCH:    Starting framework (if hangs, check transport)...\n"));

  madara::kats::Test_Framework testing (settings);

  if(debug_printing)
    testing.dump ();

  ACE_DEBUG ((LM_DEBUG, 
    "KATS_BATCH:    Starting barrier (if necessary)...\n"));

  testing.event (
    "updatevars","",".kats.id=.madara.id;.kats.processes=.madara.processes");

  if (test_name != "")
    testing.barrier (test_name);

  // start timers for conditions related to KATS for record keeping
  allconditions_timer.start ();
  allkatsconditions_timer.start ();
  barrier_timer.start ();

  // stop the barrier timer
  barrier_timer.stop ();

  // Before we check for delay, we first check for a precondition

  if (pre_condition != "")
  {
    ACE_DEBUG ((LM_DEBUG, 
      "KATS_BATCH:    Checking precondition...\n"));

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
    ACE_DEBUG ((LM_DEBUG, 
      "KATS_BATCH:    Delaying %d s according to user preferences...\n",
      delay_time.sec ()));

    ACE_OS::sleep (delay_time);
  }

  // set the post_delay
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

  ACE_DEBUG ((LM_DEBUG, 
    "KATS_BATCH:  Finished KATS setup. Proceeding to processes...\n"));

  // *************** END KATS PRELIMINARY WORK ******************


  size_t cur = 0;


  // count the processes we should be spawning
  for (element = el_tests->FirstChildElement ();
        element; element = element->NextSiblingElement ())
  {
    std::string type = element->Value ();
    if (type == "process" || type == "group" ||
        type == "sleep" || type == "observer")
    {
      ++cur;
    }
  }

  ACE_DEBUG ((LM_DEBUG, 
    "KATS_BATCH:  %d processes found in %s...\n", cur, tests_file.c_str ()));

  //processes.reserve (cur);
  processes.resize (cur + 1);

  ACE_High_Res_Timer timer;

  timer.start ();

  // clock the process time
  process_timer.start ();

  for (cur = 0, element = el_tests->FirstChildElement ();
        element; element = element->NextSiblingElement ())
  {
    std::string type = element->Value ();
    if (type != "process" && type != "group" &&
        type != "sleep" && type != "observer")
    {
      continue;
    }

    TiXmlElement * el_temp2 = 0;

    if (type == "process")
    {
      ACE_DEBUG ((LM_DEBUG, 
        "KATS_BATCH:    Processing a process...\n"));

      el_temp2 = element->FirstChildElement ("executable");
      
      // if the file didn't have an executable for an individual test
      // then let them know about the problem
      if (!el_temp2 || !el_temp2->GetText ())
      {
        ACE_DEBUG ((LM_INFO, 
          "KATS_BATCH:    Each test must have an <executable>\n"));
        return -3;
      }

      // expand the string for environment variables, then clean the path up
      std::string value (madara::Utility::clean_dir_name (
        madara::Utility::expand_envs (el_temp2->GetText ())));

      if (value == "")
      {
        ACE_DEBUG ((LM_INFO, 
          "KATS_BATCH:    Each test must have an <executable>\n"));
        return -3;
      }
      // set the executable
      processes[cur].set_executable (value);
    }
    else if (type == "group")
    {
      ACE_DEBUG ((LM_DEBUG, 
        "KATS_BATCH:    Processing a process group...\n"));

      if (element->Attribute ("file"))
      {
        ACE_DEBUG ((LM_DEBUG, 
          "KATS_BATCH:      File is set to %s...\n",
          element->Attribute ("file")));

        ACE_DEBUG ((LM_DEBUG, 
          "KATS_BATCH:      XML dir is set to %s...\n",
          working_dir.c_str ()));

        ACE_DEBUG ((LM_DEBUG, 
          "KATS_BATCH:      dirname of file is %s...\n",
          ACE::dirname (element->Attribute ("file"))));

        ACE_DEBUG ((LM_DEBUG, 
          "KATS_BATCH:      basename of file is %s...\n",
          ACE::basename (element->Attribute ("file"))));

        std::stringstream buffer;

        if ("." == extract_path (
          madara::Utility::expand_envs (element->Attribute ("file"))))
        {
          // build up a file string
          buffer << working_dir;
          buffer << "/";
          buffer << element->Attribute ("file");
        }
        else
        {
          buffer << element->Attribute ("file");
        }

        // expand the string for environment variables, then clean the path up
        std::string final_file (madara::Utility::clean_dir_name (
                      madara::Utility::expand_envs (buffer.str ())));

        ACE_DEBUG ((LM_DEBUG, 
          "KATS_BATCH:      expanded file is set to %s...\n",
          final_file.c_str ()));

        if (final_file != "")
        {
          processes[cur].kats_type ("group");
          processes[cur].set_file (final_file);
          ACE_DEBUG ((LM_DEBUG, 
            "KATS_BATCH:      Read group file = %s from process group file\n",
            final_file.c_str ()));
        }
        else
        {
          ACE_DEBUG ((LM_DEBUG, 
            "KATS_BATCH:      Process group did not have valid group file\n"
            ));
        }

      } // if name element existed
    }
    else
    {
      // currently this is a catch all for:
      // sleep for kats_sleep
      // observer for kats_observer

      processes[cur].kats_type (type);
    }

    //KATS_Process process;

    // do we have a barrier name?
    TiXmlElement * el_temp1 = element->FirstChildElement ("barrier");
    if (el_temp1)
    {
      if (el_temp1->Attribute ("name"))
      {
        std::string temp = 
          madara::Utility::expand_envs (el_temp1->Attribute ("name"));
        if (temp != "")
        {
          processes[cur].set_testname (temp);
          ACE_DEBUG ((LM_DEBUG, 
            "KATS_BATCH:    Read barrier name = %s from process group file\n",
            temp.c_str ()));
        }
      }
      else if (el_temp1->GetText ())
      {
        std::string temp = 
          madara::Utility::expand_envs (el_temp1->GetText ());
        if (temp != "")
        {
          processes[cur].set_testname (temp);
          ACE_DEBUG ((LM_DEBUG, 
            "KATS_BATCH:    Read barrier name = %s from process group file\n",
            temp.c_str ()));
        }
      }
    }

    // check the loglevel setting
    if (!loglevel_set)
    {
      el_temp1 = element->FirstChildElement ("loglevel");
      if (el_temp1 && el_temp1->GetText ())
      {
        std::string temp = 
          madara::Utility::expand_envs (el_temp1->GetText ());
        if (temp != "")
        {
          ACE_DEBUG ((LM_DEBUG, 
            "KATS_BATCH:    Read loglevel = %s from process group file\n",
            temp.c_str ()));
          processes[cur].set_loglevel (temp);
        }
      }
    }
    else
    {
      std::stringstream buffer;
      buffer << madara::logger::global_logger->get_level ();  
      processes[cur].set_loglevel (buffer.str ());
    }

    // check the processes setting
    el_temp1 = element->FirstChildElement ("processes");
    if (el_temp1 && el_temp1->GetText ())
    {
        std::string temp = 
          madara::Utility::expand_envs (el_temp1->GetText ());
        if (temp != "")
        {
          ACE_DEBUG ((LM_DEBUG, 
            "KATS_BATCH:    Read processes = %s from process group file\n",
            temp.c_str ()));
          processes[cur].set_processes (temp);
        }
    }

    // check the id setting
    el_temp1 = element->FirstChildElement ("id");
    if (el_temp1 && el_temp1->GetText ())
    {
      std::string temp = 
        madara::Utility::expand_envs (el_temp1->GetText ());
      if (temp != "")
      {
        ACE_DEBUG ((LM_DEBUG, 
          "KATS_BATCH:    Read id = %s from process group file\n",
          temp.c_str ()));
        processes[cur].set_id (temp);
      }
    }

    // check the executable environment
    el_temp1 = element->FirstChildElement ("environment");
    if (el_temp1 && el_temp1->GetText ())
    {
      std::string temp = 
        madara::Utility::expand_envs (el_temp1->GetText ());
      if (temp != "")
      {
        ACE_DEBUG ((LM_DEBUG, 
          "KATS_BATCH:    Read env = %s from process group file\n",
          temp.c_str ()));
        processes[cur].set_environment (temp);
      }
    }

    // check the delay
    el_temp1 = element->FirstChildElement ("delay");
    if (el_temp1 && el_temp1->GetText ())
    {
      std::string temp = 
        madara::Utility::expand_envs (el_temp1->GetText ());
      if (temp != "")
      {
        ACE_DEBUG ((LM_DEBUG, 
          "KATS_BATCH:    Read delay = %s from process group file\n",
          temp.c_str ()));
        processes[cur].set_delaytime (temp);
      }
    }

    // check for stdout redirect
    el_temp1 = element->FirstChildElement ("stdout");
    if (el_temp1 && el_temp1->GetText ())
    {
      // expand the string for environment variables, then clean the path up
      std::string value (madara::Utility::clean_dir_name (
        madara::Utility::expand_envs (el_temp1->GetText ())));

      if (value != "")
      {
        ACE_DEBUG ((LM_DEBUG, 
          "KATS_BATCH:    Read stdout redirect = %s from process group file\n",
             value.c_str ()));

        processes[cur].set_stdout (value);
      }
    }

    // check for stderr redirect
    el_temp1 = element->FirstChildElement ("stderr");
    if (el_temp1 && el_temp1->GetText ())
    {
      // expand the string for environment variables, then clean the path up
      std::string value (madara::Utility::clean_dir_name (
        madara::Utility::expand_envs (el_temp1->GetText ())));

      if (value != "")
      {
        ACE_DEBUG ((LM_DEBUG, 
          "KATS_BATCH:    Read stderr redirect = %s from process group file\n",
             value.c_str ()));
        processes[cur].set_stderr (value);
      }
    }

    // check for stderr redirect
    el_temp1 = element->FirstChildElement ("stdin");
    if (el_temp1 && el_temp1->GetText ())
    {
      // expand the string for environment variables, then clean the path up
      std::string value (madara::Utility::clean_dir_name (
        madara::Utility::expand_envs (el_temp1->GetText ())));

      if (value != "")
      {
        ACE_DEBUG ((LM_DEBUG, 
          "KATS_BATCH:    Read stdin redirect = %s from process group file\n",
          value.c_str ()));
        processes[cur].set_stdin (value);
      }
    }

    // check the precondition
    el_temp1 = element->FirstChildElement ("precondition");
    if (el_temp1 && el_temp1->GetText ())
    {
      std::string temp = 
        madara::Utility::expand_envs (el_temp1->GetText ());
      if (temp != "")
      {
        ACE_DEBUG ((LM_DEBUG, 
          "KATS_BATCH:    Read precondition = %s from process group file\n",
          temp.c_str ()));
        processes[cur].set_precondition (temp);
      }
    }

    // check the postcondition
    el_temp1 = element->FirstChildElement ("postcondition");
    if (el_temp1 && el_temp1->GetText ())
    {
      std::string temp = 
        madara::Utility::expand_envs (el_temp1->GetText ());
      if (temp != "")
      {
        ACE_DEBUG ((LM_DEBUG, 
          "KATS_BATCH:    Read postcondition = %s from process group file\n",
          temp.c_str ()));
        processes[cur].set_postcondition (temp);
      }
    }

    // check the postdelay
    el_temp1 = element->FirstChildElement ("postdelay");
    if (el_temp1 && el_temp1->GetText ())
    {
      std::string temp = 
        madara::Utility::expand_envs (el_temp1->GetText ());
      if (temp != "")
      {
        ACE_DEBUG ((LM_DEBUG, 
          "KATS_BATCH:    Read postdelay = %s from process group file\n",
          temp.c_str ()));
        processes[cur].set_postdelay (temp);
      }
    }

    // check the postdelay
    el_temp1 = element->FirstChildElement ("postlaunch");
    if (el_temp1 && el_temp1->GetText ())
    {
      std::string temp = 
        madara::Utility::expand_envs (el_temp1->GetText ());
      if (temp != "")
      {
        ACE_DEBUG ((LM_DEBUG, 
          "KATS_BATCH:    Read postlaunch = %s from process group file\n",
          temp.c_str ()));
        processes[cur].set_postlaunch (temp);
      }
    }

    // check the commandline
    el_temp1 = element->FirstChildElement ("commandline");
    if (el_temp1 && el_temp1->GetText ())
    {
      // expand the string for environment variables, then clean the path up
      std::string value (madara::Utility::expand_envs (el_temp1->GetText ()));

      if (value != "")
      {
        ACE_DEBUG ((LM_DEBUG, 
          "KATS_BATCH:    Read commandline = %s from process group file\n",
            value.c_str ()));
        processes[cur].set_commandline (value);
      }
    }

    // kill may be a nested element, e.g. the following:
    // <kill>
    //   <time>5</time>
    //   <signal>15</time>
    // </kill>
    el_temp1 = element->FirstChildElement ("kill");
    if (el_temp1)
    {
      // we have a kill element

      // check for a timeout
      el_temp2 = el_temp1->FirstChildElement ("time");
      if (el_temp2)
      {
        // we have a kill/time
        if (el_temp2->GetText ())
        {
          std::string temp = 
            madara::Utility::expand_envs (el_temp2->GetText ());
          if (temp != "")
          {
            ACE_DEBUG ((LM_DEBUG, 
              "KATS_BATCH:    Read kill time = %s from process group file\n",
              temp.c_str ()));
            processes[cur].set_killtime (temp);
          }
        }
      }

      // check for a signal
      el_temp2 = el_temp1->FirstChildElement ("signal");
      if (el_temp2)
      {
        // we have a kill/signal
        if (el_temp2->GetText ())
        {
          std::string temp = 
            madara::Utility::expand_envs (el_temp2->GetText ());
          if (temp != "")
          {
            ACE_DEBUG ((LM_DEBUG, 
              "KATS_BATCH:    Read kill signal = %s from process group file\n",
              temp.c_str ()));
            processes[cur].set_killsignal (temp);
          }
        }
      }
    }

    // check the workingdir
    el_temp1 = element->FirstChildElement ("workingdir");
    if (el_temp1 && el_temp1->GetText ())
    {
      // expand the string for environment variables, then clean the path up
      std::string value (madara::Utility::clean_dir_name (
        madara::Utility::expand_envs (el_temp1->GetText ())));

      if (value != "")
      {
        ACE_DEBUG ((LM_DEBUG, 
          "KATS_BATCH:    Read workingdir = %s from process group file\n",
          value.c_str ()));

        processes[cur].set_workingdir (value);
      }
    }

    // check for stdout redirect
    el_temp1 = el_globals->FirstChildElement ("transport");
    if (el_temp1 && el_temp1->Attribute ("file"))
    {
      std::string value (madara::Utility::clean_dir_name (
        madara::Utility::expand_envs (el_temp1->Attribute ("file"))));

      if (value != "")
      {
        std::stringstream buffer;
        buffer << working_dir;
        buffer << "/";
        buffer << value;

        value = madara::Utility::clean_dir_name (buffer.str ().c_str ()); 

        ACE_DEBUG ((LM_DEBUG, 
          "KATS_BATCH:    Read transport file = %s from process group file\n",
          value.c_str ()));

        processes[cur].set_transport_file (value);
      }
    }


    // check the settings that are overridable from command line

    if (realtime || element->FirstChildElement ("realtime"))
    {
      ACE_DEBUG ((LM_DEBUG, 
        "KATS_BATCH:    Enabling real time scheduling in process\n"));
      processes[cur].enable_realtime (); 
    }

    if (timing || element->FirstChildElement ("timing"))
    {
      ACE_DEBUG ((LM_DEBUG, 
        "KATS_BATCH:    Enabling printing of timing info in process\n"));
      processes[cur].enable_timing ();
    }

    if (debug_printing ||  element->FirstChildElement ("debug"))
    {
      ACE_DEBUG ((LM_DEBUG, 
        "KATS_BATCH:    Enabling debug printing in process\n"));
      processes[cur].enable_debug ();
    }

    // check the host setting
    if (!host_set)
    {
      el_temp1 = element->FirstChildElement ("host");
      if (el_temp1 && el_temp1->GetText ())
      {
        std::string temp = 
          madara::Utility::expand_envs (el_temp1->GetText ());
        if (temp != "")
        {
          ACE_DEBUG ((LM_DEBUG, 
            "KATS_BATCH:    Read host = %s from test entry\n",
            temp.c_str ()));
          processes[cur].set_host (temp);
        }
      }
    }
    // user can override host on the command line
    else
    {
      ACE_DEBUG ((LM_DEBUG, 
        "KATS_BATCH:    Using host = %s from globals\n",
        settings.host.c_str ()));
      processes[cur].set_host (settings.host);
    }

    // check the domain setting
    if (!domain_set)
    {
      el_temp1 = element->FirstChildElement ("domain");
      if (el_temp1 && el_temp1->GetText ())
      {
        std::string temp = 
          madara::Utility::expand_envs (el_temp1->GetText ());
        if (temp != "")
        {
          ACE_DEBUG ((LM_DEBUG, 
            "KATS_BATCH:    Read domain = %s from test entry\n",
            temp.c_str ()));
          processes[cur].set_domain (temp);
        }
      }
    }
    // user can override domain on the command line
    else
    {
      ACE_DEBUG ((LM_DEBUG, 
        "KATS_BATCH:    Using domain = %s from globals\n",
        settings.domains.c_str ()));
      processes[cur].set_domain (settings.domains);
    }

    // print the command line
    ACE_DEBUG ((LM_INFO, 
      "KATS_BATCH:  Launching %s\n",
      processes[cur].query_setup ().c_str ()));    

    processes[cur].launch ();

    // this isn't going to cut it. We need to start a timer before
    // the loop and check against it.
    if (!run_in_parallel && !kill_time_set)
    {
      ACE_DEBUG ((LM_DEBUG, 
        "KATS_BATCH:  Waiting on process to finish.\n"));
      processes[cur].wait ();
    }
    else if (!run_in_parallel)
    {
      timer.stop ();
      // check the elapsed time
      ACE_Time_Value elapsed;
      timer.elapsed_time (elapsed);

      // if we haven't already gone past our time limit,
      // then wait for the process with the remaining time
      if (elapsed.sec () <= kill_time.sec ())
      {
        ACE_Time_Value new_wait_time;
        new_wait_time.sec (kill_time.sec () - elapsed.sec ());

        ACE_DEBUG ((LM_DEBUG, 
          "KATS_BATCH:  Waiting for %d s or process to finish.",
          new_wait_time.sec ()));
        processes[cur].wait (new_wait_time);
      }

      timer.stop ();
      timer.elapsed_time (elapsed);
      
      // if we haven't already gone past our time limit,
      // then wait for the process with the remaining time
      if (elapsed.sec () > kill_time.sec ())
      {
        processes[cur].terminate ();
      }
    }
    ++cur;
  }

  // clear the XML document so we're not wasting memory anymore
  //doc.Clear ();

  // post launch occurs after all processes have been launched.
  // if parallel was set, then this will be between start and finish
  // if sequential mode was set, then this will be after the last process
  // was finally launched and maybe returned, depending on settings.
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


  if (run_in_parallel)
  {
    ACE_DEBUG ((LM_DEBUG, 
      "KATS_BATCH:  Waiting for %d s before terminating all processes.",
      kill_time.sec ()));

    ACE_OS::sleep (kill_time);

    for (size_t i = 0; i < cur; ++i)
    {
      if (kill_time_set)
      {
        processes[i].terminate ();
      }
      else
        processes[i].wait ();
    }
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
      DLINFO "KATS_BATCH: Printing final knowledge values\n"));

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
        DLINFO "KATS_BATCH: Barrier took %Q ns\n",
        barrier_elapsed ));
      MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_INFO, 
        DLINFO "KATS_BATCH: All KATS conditions took %Q ns\n",
        allkatsconditions_elapsed ));
      MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_INFO, 
        DLINFO "KATS_BATCH: All conditions took %Q ns\n",
        allconditions_elapsed ));
      MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_INFO, 
        DLINFO "KATS_BATCH: From start to finish took %Q ns\n",
        starttofinish_elapsed ));
    }
  }

  if (timing)
  {
    ACE_hrtime_t process_elapsed;
    process_timer.elapsed_time (process_elapsed);

    MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_INFO, 
      DLINFO "KATS_BATCH: group runtime was %Q ns\n",
      process_elapsed ));
  }

  return 0;
}

int parse_args (int argc, ACE_TCHAR * argv[])
{
  // options string which defines all short args
  ACE_TCHAR options [] = ACE_TEXT ("0:1:2:8:9:f:n:i:l:o:d:a:s:t:v:y:z:mgrh");

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
  cmd_opts.long_option (ACE_TEXT ("domain"), 'd', ACE_Get_Opt::ARG_REQUIRED);
  cmd_opts.long_option (ACE_TEXT ("file"), 'f', ACE_Get_Opt::ARG_REQUIRED);
  cmd_opts.long_option (ACE_TEXT ("debug"), 'g', ACE_Get_Opt::NO_ARG);
  cmd_opts.long_option (ACE_TEXT ("help"), 'h', ACE_Get_Opt::NO_ARG);
  cmd_opts.long_option (ACE_TEXT ("id"), 'i', ACE_Get_Opt::ARG_REQUIRED);
  cmd_opts.long_option (ACE_TEXT ("timing"), 'm', ACE_Get_Opt::NO_ARG);
  cmd_opts.long_option (ACE_TEXT ("processes"), 'n', ACE_Get_Opt::ARG_REQUIRED);
  cmd_opts.long_option (ACE_TEXT ("host"), 'o', ACE_Get_Opt::ARG_REQUIRED);
  cmd_opts.long_option (ACE_TEXT ("parallel"), 'p', ACE_Get_Opt::NO_ARG);
  cmd_opts.long_option (ACE_TEXT ("postcondition"), 's',
      ACE_Get_Opt::ARG_REQUIRED);
  cmd_opts.long_option (ACE_TEXT ("realtime"), 'r', ACE_Get_Opt::NO_ARG);
  cmd_opts.long_option (ACE_TEXT ("delay"), 'l', ACE_Get_Opt::ARG_REQUIRED);
  cmd_opts.long_option (ACE_TEXT ("killtime"), 't', ACE_Get_Opt::ARG_REQUIRED);
  cmd_opts.long_option (ACE_TEXT ("loglevel"), 'v', ACE_Get_Opt::ARG_REQUIRED); 

  // temp for current switched option
  int option;

  // iterate through the options until done
  while ((option = cmd_opts ()) != EOF)
  {
    //arg = cmd_opts.opt_arg ();
    switch (option)
    {
    case '0':
      // redirecting stdout

      freopen (madara::Utility::clean_dir_name (cmd_opts.opt_arg ()).c_str (),
               "r", stdin);

      stdin_set = true;

      MADARA_DEBUG (MADARA_LOG_MINOR_EVENT, (LM_DEBUG,
        DLINFO "KATS_BATCH: stdin redirected from %s\n",
        madara::Utility::clean_dir_name (cmd_opts.opt_arg ()).c_str ()));

      break;
    case '1':
      // redirecting stdout

      freopen (madara::Utility::clean_dir_name (cmd_opts.opt_arg ()).c_str (),
               "w", stdout);

      stdout_set = true;

      MADARA_DEBUG (MADARA_LOG_MINOR_EVENT, (LM_DEBUG,
        DLINFO "KATS_BATCH: stdout redirected to %s\n",
        madara::Utility::clean_dir_name (cmd_opts.opt_arg ()).c_str ()));

      break;
    case '2':
      // redirecting stderr
      freopen (madara::Utility::clean_dir_name (cmd_opts.opt_arg ()).c_str (),
               "w", stderr);

      stderr_set = true;

      MADARA_DEBUG (MADARA_LOG_MINOR_EVENT, (LM_DEBUG,
        DLINFO "KATS_BATCH: stderr redirected to %s\n",
        madara::Utility::clean_dir_name (cmd_opts.opt_arg ()).c_str ()));

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
      transport_set = true;

      MADARA_DEBUG (MADARA_LOG_MINOR_EVENT, (LM_DEBUG, 
        DLINFO "KATS_BATCH: transport protocol set to %u\n",
        settings.type));

      break;
    case 'a':
      // thread number
      test_name = cmd_opts.opt_arg ();
      test_name_set = true;

      MADARA_DEBUG (MADARA_LOG_MINOR_EVENT, (LM_DEBUG, 
        DLINFO "KATS_BATCH: test name set to %s\n",
        test_name.c_str ()));

      break;
    case 'b':
      // a precondition

      pre_condition = cmd_opts.opt_arg ();
      pre_condition_set = true;

      MADARA_DEBUG (MADARA_LOG_MINOR_EVENT, (LM_DEBUG, 
        DLINFO "KATS_BATCH: precondition set to %s\n",
        pre_condition.c_str ()));

      break;
    case 'd':
      // the knowledge domain

      settings.domains = cmd_opts.opt_arg ();
      domain_set = false;

      MADARA_DEBUG (MADARA_LOG_MINOR_EVENT, (LM_DEBUG, 
        DLINFO "KATS_BATCH: knowledge domain set to %s\n",
        settings.domains.c_str ()));

      break;
    case 'f':
      // the knowledge domain

      tests_file = madara::Utility::clean_dir_name (cmd_opts.opt_arg ());

      MADARA_DEBUG (MADARA_LOG_MINOR_EVENT, (LM_DEBUG,
        "KATS_BATCH:  reading tests from %s.\n",
        tests_file.c_str () ));

      break;
    case 'g':
      // thread number
      debug_printing = true;
      debug_set = true;

      if(debug_printing)
        ACE_LOG_MSG->priority_mask (LM_DEBUG | LM_INFO | LM_ERROR, ACE_Log_Msg::PROCESS);

      MADARA_DEBUG (MADARA_LOG_MINOR_EVENT, (LM_DEBUG, 
        DLINFO "KATS_BATCH: enabling debug printing\n"));

      break;
    case 'i':
      // id
      {
        std::stringstream buffer;
        buffer << cmd_opts.opt_arg ();
        buffer >> settings.id;
      }
      id_set = true;

      MADARA_DEBUG (MADARA_LOG_MINOR_EVENT, (LM_DEBUG, 
        DLINFO "KATS_BATCH: id in test process ring set to %u\n",
        settings.id));

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
        DLINFO "KATS_BATCH: delay processes launch by %: seconds\n",
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
      processes_set = true;

      MADARA_DEBUG (MADARA_LOG_MINOR_EVENT, (LM_DEBUG, 
        DLINFO "KATS_PROCESS: processes set to %u\n",
        settings.processes));

      break;
    case 'o':
      // host name
      settings.host = cmd_opts.opt_arg ();
      host_set = true;

      MADARA_DEBUG (MADARA_LOG_MINOR_EVENT, (LM_DEBUG, 
        DLINFO "KATS_BATCH: host set to %s\n",
        settings.host.c_str ()));

      break;
    case 'p':
      // thread number
      run_in_parallel = true;
      parallel_set = true;

      MADARA_DEBUG (MADARA_LOG_MINOR_EVENT, (LM_DEBUG,
        "KATS_BATCH:  running all batch tests in parallel\n"));

      break;
    case 'r':
      // thread number
      realtime = true;
      realtime_set = true;

      MADARA_DEBUG (MADARA_LOG_MINOR_EVENT, (LM_DEBUG, 
        DLINFO "KATS_BATCH: enabling realtime scheduling\n"));

      break;
    case 's':
      // a postcondition

      post_condition = cmd_opts.opt_arg ();
      post_condition_set = true;

      MADARA_DEBUG (MADARA_LOG_MINOR_EVENT, (LM_DEBUG, 
        DLINFO "KATS_BATCH: postcondition set to %s\n",
        post_condition.c_str ()));

      break;
    case 't':
      // time to kill the process
      {
        time_t time_in_seconds;
        std::stringstream buffer;
        buffer << cmd_opts.opt_arg ();
        buffer >> time_in_seconds;

        kill_time.sec (time_in_seconds);
        kill_time_set = true;
      }

      MADARA_DEBUG (MADARA_LOG_MINOR_EVENT, (LM_DEBUG, 
        DLINFO "KATS_BATCH:  terminate all test processes after %: seconds\n",
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

      loglevel_set = true;

      break;
    case 'y':
      // a postdelay

      post_delay = cmd_opts.opt_arg ();
      post_delay_set = true;

      MADARA_DEBUG (MADARA_LOG_MINOR_EVENT, (LM_DEBUG, 
        DLINFO "KATS_BATCH: postdelay set to %s\n",
        post_delay.c_str ()));

      break;
    case 'z':
      // a postdelay

      post_launch = cmd_opts.opt_arg ();
      post_launch_set = true;

      MADARA_DEBUG (MADARA_LOG_MINOR_EVENT, (LM_DEBUG, 
        DLINFO "KATS_BATCH: postlaunch set to %s\n",
        post_launch.c_str ()));

      break;
    case ':':
      MADARA_ERROR_RETURN (MADARA_LOG_TERMINAL_ERROR, (LM_ERROR, 
        ACE_TEXT ("KATS_BATCH:  ERROR: -%c requires an argument"),
           cmd_opts.opt_opt ()), -2); 
    default:
      {
        char c_option = (char) option;
        MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_DEBUG,
          DLINFO "KATS_PROCESS: unrecognized argument %c\n",
          c_option));

        MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_DEBUG,
          DLINFO "KATS_PROCESS: printing help...\n",
          c_option));
      }

    case 'h':
      MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_INFO,
"kats_batch options:      \n\
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
      -f (--file)        xml file containing testing information \n\
      -g (--debug)       prints KATS debug messages \n\
      -h (--help)        print this menu        \n\
      -i (--id)          this process id        \n\
      -l (--delay)       time delay (secs) after barrier and KaRL\n\
                         precondition to wait before process group spawn\n\
      -m (--timing)      print timing information \n\
                         (add -g for kats conditional timing) \n\
      -n (--processes)   number of testing processes \n\
      -p (--parallel)    launch the processes in parallel:\n\
      -o (--host)        host identifier        \n\
      -r (--realtime)    run the process with real time scheduling \n\
      -s (--postcondition) postcondition to evaluate after process exits \n\
      -t (--timeout)     time in seconds to wait before killing \n\
      -v (--loglevel)    maximum log level to print from MADARA messages\n\
      -y (--postdelay)   condition to evaluate after temporal delay and \n\
                         before user application launch\n\
      -z (--postlaunch)  condition to evaluate after user application launch\n\
"
      ));
      MADARA_ERROR_RETURN (MADARA_LOG_TERMINAL_ERROR, (LM_ERROR, 
        ACE_TEXT ("Returning from Help Menu\n")), -1); 
      break;
    }
  }

  return 0;
}

