
//#define MADARA_NTRACE    0
//#define ACE_NTRACE    0
////#define ACE_NLOGGING  0
//#define ACE_NDEBUG    0

#include <string>
#include <vector>
#include <iostream>
#include <assert.h>
#include <sstream>

#include "ace/Log_Msg.h"
#include "ace/Get_Opt.h"

#include "madara/knowledge_engine/Knowledge_Base.h"

bool finisher_mode = false;
std::string host = "";
Madara::Knowledge_Record::Integer id = 0;
Madara::Knowledge_Record::Integer processes = 1;

// command line arguments
int parse_args (int argc, ACE_TCHAR * argv[]);

int ACE_TMAIN (int argc, ACE_TCHAR * argv[])
{
  int retcode = parse_args (argc, argv);

  if (retcode < 0)
    return retcode;

  ACE_LOG_MSG->priority_mask (LM_INFO | LM_DEBUG, ACE_Log_Msg::PROCESS);

  ACE_TRACE (ACE_TEXT ("main"));

  Madara::Knowledge_Engine::Knowledge_Base knowledge (host, 
    Madara::Transport::SPLICE);

  Madara::Knowledge_Engine::Knowledge_Base specialized_knowledge (
    host, Madara::Transport::SPLICE, "specialized");

  knowledge.set (".id", id);
  specialized_knowledge.set (".id", id);

  std::stringstream processes_to_wait_for;
  processes_to_wait_for << "(Running{.id} = 1) ";

  for(int i = 0; i < processes; ++i)
  {
    processes_to_wait_for << " && Running" << i;
  }


  ACE_DEBUG ((LM_INFO, "(%P|%t) Waiting on %s\n",
    processes_to_wait_for.str ().c_str ()));
  knowledge.wait (processes_to_wait_for.str ());

  specialized_knowledge.evaluate ("Special{.id}=3 * .id");

  processes_to_wait_for.str ("");
  processes_to_wait_for << "(Running{.id} = 0); 1";

  for(int i = 0; i < processes; ++i)
  {
    processes_to_wait_for << " && !Running" << i;
  }

  ACE_DEBUG ((LM_INFO, "(%P|%t) Waiting on %s\n",
    processes_to_wait_for.str ().c_str ()));

  // need to fix this
  knowledge.wait (processes_to_wait_for.str ());


  ACE_DEBUG ((LM_DEBUG, "(%P|%t) Knowledge\n"));
  knowledge.print_knowledge ();
  
  ACE_DEBUG ((LM_DEBUG, "(%P|%t) Special Knowledge\n"));
  specialized_knowledge.print_knowledge ();

  ACE_OS::sleep (3);

  ACE_DEBUG ((LM_DEBUG, "(%P|%t) Exiting\n"));
  return 0;
}



int parse_args (int argc, ACE_TCHAR * argv[])
{
  // options string which defines all short args
  ACE_TCHAR options [] = ACE_TEXT ("i:p:o:fh");

  // create an instance of the command line args
  ACE_Get_Opt cmd_opts (argc, argv, options);

  // set up an alias for '-n' to be '--name'
  cmd_opts.long_option (ACE_TEXT ("finisher"), 'f', ACE_Get_Opt::ARG_REQUIRED);
  cmd_opts.long_option (ACE_TEXT ("host"), 'o', ACE_Get_Opt::ARG_REQUIRED);
  cmd_opts.long_option (ACE_TEXT ("help"), 'h', ACE_Get_Opt::ARG_REQUIRED);
  cmd_opts.long_option (ACE_TEXT ("id"), 'i', ACE_Get_Opt::ARG_REQUIRED);
  cmd_opts.long_option (ACE_TEXT ("processes"), 'p', ACE_Get_Opt::ARG_REQUIRED);
 
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
    case 'p':
      // thread number
      processes = atoi (cmd_opts.opt_arg ());
      break;
    case 'f':
      // thread number
      finisher_mode = true;
      break;
    case 'o':
      host = cmd_opts.opt_arg ();
      ACE_DEBUG ((LM_DEBUG, "(%P|%t) host set to %s\n", host.c_str ()));
      break;
    case ':':
      ACE_ERROR_RETURN ((LM_ERROR, 
        ACE_TEXT ("ERROR: -%c requires an argument"), cmd_opts.opt_opt ()), -2); 
    case 'h':
    default:
      ACE_DEBUG ((LM_DEBUG, "Program Options:      \n\
      -o (--host)      this host ip/name (localhost default) \n\
      -p (--processes) number of processes that will be running\n\
      -i (--id)        set process id (0 default)  \n\
      -f (--finisher)  signal finish               \n\
      -h (--help)      print this menu             \n"));
      ACE_ERROR_RETURN ((LM_ERROR, 
        ACE_TEXT ("Returning from Help Menu")), -1); 
      break;
    }
  }

  return 0;
}
