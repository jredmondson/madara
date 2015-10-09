
//#define MADARA_NTRACE    0
//#define ACE_NTRACE    0
////#define ACE_NLOGGING  0
//#define ACE_NDEBUG    0

#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <assert.h>


// Because Microsoft decided to not support stdint.h
// from VS 2003 until VS 2010, we simply typedef the
// symbols that are supported in all MS products when
// we know we have a MS compiler

#ifdef _MSC_VER

typedef __int32             int32_t;
typedef unsigned __int32    uint32_t;
typedef __int64             int64_t;
typedef unsigned __int64    uint64_t;

#else   // !_MSC_VER

// otherwise, we do something more logical
#include <stdint.h>

#endif  // _MSC_VER


#include "ace/Log_Msg.h"
#include "ace/Get_Opt.h"
#include "ace/Signal.h"

#include "madara/knowledge_engine/KnowledgeBase.h"

Madara::KnowledgeRecord::Integer id = 0;
Madara::KnowledgeRecord::Integer left = 0;
Madara::KnowledgeRecord::Integer processes = 1;
uint32_t quality = 0;
Madara::KnowledgeRecord::Integer value = 0;
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

  ACE_LOG_MSG->priority_mask (LM_DEBUG | LM_INFO, ACE_Log_Msg::PROCESS);

  ACE_TRACE (ACE_TEXT ("main"));

  // signal handler for clean exit
  ACE_Sig_Action sa ((ACE_SignalHandler) terminate, SIGINT);

  Madara::KnowledgeEngine::KnowledgeBase knowledge(host, Madara::Transport::SPLICE);

  ACE_DEBUG ((LM_INFO, "(%P|%t) (%d of %d) with quality %d\n",
                        id, processes, quality));

  // set my id
  knowledge.set (".self", id);

  // set the quality of current process in regards to updating
  // the global important_value
  knowledge.set_quality ("important_value", quality);

  // set the value to the initial value
  knowledge.set ("important_value", value);


  ACE_DEBUG ((LM_DEBUG, "(%P|%t) Starting Knowledge\n"));
  knowledge.print_knowledge ();

  // termination is done via signalling from the user (Control+C)
  while (!terminated)
  {
    // try to set the value to the initial value
    int result = knowledge.set ("important_value",
      Madara::KnowledgeRecord::Integer (quality));

    // everyone prints out current value
    knowledge.print("  Cur value: {important_value}\n");

    ACE_DEBUG ((LM_DEBUG, "(%P|%t) Also, result is %d\n", result));

    ACE_OS::sleep (1);
  }

  ACE_DEBUG ((LM_DEBUG, "(%P|%t) Final Knowledge\n"));
  knowledge.print_knowledge ();

  ACE_DEBUG ((LM_DEBUG, "(%P|%t) Exiting\n"));
  return 0;
}



int parse_args (int argc, ACE_TCHAR * argv[])
{
  // options string which defines all short args
  ACE_TCHAR options [] = ACE_TEXT ("i:q:p:o:v:h");

  // create an instance of the command line args
  ACE_Get_Opt cmd_opts (argc, argv, options);

  // set up an alias for '-n' to be '--name'
  cmd_opts.long_option (ACE_TEXT ("id"), 'i', ACE_Get_Opt::ARG_REQUIRED);
  cmd_opts.long_option (ACE_TEXT ("quality"), 'q', ACE_Get_Opt::ARG_REQUIRED);
  cmd_opts.long_option (ACE_TEXT ("processes"), 'p', ACE_Get_Opt::ARG_REQUIRED);
  cmd_opts.long_option (ACE_TEXT ("help"), 'h', ACE_Get_Opt::ARG_REQUIRED);
  cmd_opts.long_option (ACE_TEXT ("host"), 'o', ACE_Get_Opt::ARG_REQUIRED);
  cmd_opts.long_option (ACE_TEXT ("value"), 'v', ACE_Get_Opt::ARG_REQUIRED);
 
  // temp for current switched option
  int option;
//  ACE_TCHAR * arg;

  std::stringstream buffer;
  // iterate through the options until done
  while ((option = cmd_opts ()) != EOF)
  {
    //arg = cmd_opts.opt_arg ();
    switch (option)
    {
    case 'i':
      // thread number
      buffer.str (cmd_opts.opt_arg ());
      buffer >> id;
      break;
    case 'q':
      // thread number
      buffer.str (cmd_opts.opt_arg ());
      buffer >> quality;
      break;
    case 'p':
      // thread number
      buffer.str (cmd_opts.opt_arg ());
      buffer >> processes;
      break;
    case 'v':
      // thread number
      buffer.str (cmd_opts.opt_arg ());
      buffer >> value;
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
      -q (--quality)   write quality (0 default) \n\
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
