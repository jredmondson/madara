
//#define MADARA_NTRACE    0
//#define ACE_NTRACE    0
////#define ACE_NLOGGING  0
//#define ACE_NDEBUG    0

#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include <assert.h>
#include <stdlib.h>

#include "ace/Log_Msg.h"
#include "ace/Get_Opt.h"
#include "ace/Signal.h"

#include "madara/knowledge_engine/Knowledge_Base.h"
#include "madara/utility/Utility.h"

Madara::Knowledge_Record::Integer id = 0;
Madara::Knowledge_Record::Integer left = 0;
Madara::Knowledge_Record::Integer processes = 1;
Madara::Knowledge_Record::Integer stop = 10;
Madara::Knowledge_Record::Integer value = 0;
std::string host = "";


volatile bool terminated = 0;

// command line arguments
int parse_args (int argc, ACE_TCHAR * argv[]);

// signal handler for someone hitting control+c
extern "C" void terminate (int)
{
  terminated = true;
}

int ACE_TMAIN (int argc, ACE_TCHAR * argv[])
{
  int retcode = parse_args (argc, argv);

  if (retcode < 0)
    return retcode;

  ACE_LOG_MSG->priority_mask (LM_INFO, ACE_Log_Msg::PROCESS);

  ACE_TRACE (ACE_TEXT ("main"));

  // signal handler for clean exit
  ACE_Sig_Action sa ((ACE_SignalHandler) terminate, SIGINT);

  Madara::Knowledge_Engine::Knowledge_Base knowledge(host, Madara::Transport::SPLICE);

  ACE_DEBUG ((LM_INFO, "(%P|%t) (%d of %d) synchronizing to %d\n",
                        id, processes, stop));

  // set my id
  knowledge.set (".id", id);

  // time related knowledge values
  // .time_left_in_yellow              // time left in yellow light
  // .time_in_phase                    // time that has been spent in phased
  // .cut_off_time                     // cutoff time for being in phase  
  // .braking_time                     // time required to come to a stop
  // .time_min                         // minimum time in a phase

  std::stringstream main_logic;

  // main logic file
  std::string filename = getenv ("MADARA_ROOT");
  filename += "/configs/logics/traffic_light.klf";

  std::string logic = Madara::Utility::file_to_string (filename);

  // setup file for the lights
  filename = getenv ("MADARA_ROOT");
  filename += "/configs/logics/traffic_light_setup.klf";

  std::string setup = Madara::Utility::file_to_string (filename);

  std::ofstream main_logic_file;

  main_logic_file.open ("tl_main_logic.klf");

  if (main_logic_file.is_open ())
  {
    main_logic_file << logic;
    main_logic_file.close ();
  }

  ACE_DEBUG ((LM_INFO, "(%P|%t) Main logic is the following:\n%s\n\n",
    logic.c_str ()));

  ACE_DEBUG ((LM_DEBUG, "(%P|%t) Starting Knowledge\n"));
  knowledge.print_knowledge ();

  // termination is done via signalling from the user (Control+C)
  while (!terminated)
  {
    knowledge.wait (".cur_time < simulation_time || simulation_finished");

    if (knowledge.get ("simulation_finished").is_true ())
    {
      terminated = true;
    }
    else
    {
      knowledge.evaluate (logic);
      knowledge.print("{.cur_time} RULE: {.rule}  P:{tl{.id}.phase}.{.time_in_phase} "\
        "Y:{.switching_yellow}.{.time_left_in_yellow} " \
        "NSR: {tl{.id}.northsouth.reinforcement}, " \
        "EWR: {tl{.id}.eastwest.reinforcement}\n");
    }
  }

  ACE_DEBUG ((LM_DEBUG, "(%P|%t) Final Knowledge\n"));
  knowledge.print_knowledge ();

  ACE_DEBUG ((LM_DEBUG, "(%P|%t) Exiting\n"));
  return 0;
}



int parse_args (int argc, ACE_TCHAR * argv[])
{
  // options string which defines all short args
  ACE_TCHAR options [] = ACE_TEXT ("i:s:p:o:v:h");

  // create an instance of the command line args
  ACE_Get_Opt cmd_opts (argc, argv, options);

  // set up an alias for '-n' to be '--name'
  cmd_opts.long_option (ACE_TEXT ("id"), 'i', ACE_Get_Opt::ARG_REQUIRED);
  cmd_opts.long_option (ACE_TEXT ("start"), 's', ACE_Get_Opt::ARG_REQUIRED);
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
    case 'i':
      // thread number
      id = atoi (cmd_opts.opt_arg ());
      break;
    case 's':
      // thread number
      stop = atoi (cmd_opts.opt_arg ());
      break;
    case 'p':
      // thread number
      processes = atoi (cmd_opts.opt_arg ());
      break;
    case 'v':
      // thread number
      value = atoi (cmd_opts.opt_arg ());
      break;
    case 'o':
      host = cmd_opts.opt_arg ();
      ACE_DEBUG ((LM_DEBUG, "(%P|%t) host set to %s\n", host.c_str ()));
      break;
    case ':':
      ACE_ERROR_RETURN ((LM_ERROR, 
        ACE_TEXT ("ERROR: -%c requires an argument"), 
           cmd_opts.opt_opt ()), -2); 
    case 'h':
    default:
      ACE_DEBUG ((LM_DEBUG, "Program Options:      \n\
      -p (--processes) number of processes that will be running\n\
      -i (--id)        set process id (0 default)  \n\
      -s (--start)     start condition (10 default) \n\
      -o (--host)      this host ip/name (localhost default) \n\
      -v (--value)     start process with a certain value (0 default) \n\
      -h (--help)      print this menu             \n"));
      ACE_ERROR_RETURN ((LM_ERROR, 
        ACE_TEXT ("Returning from Help Menu")), -1); 
      break;
    }
  }

  return 0;
}
