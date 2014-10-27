//
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
#include "ace/Thread.h"
#include "ace/Atomic_Op_T.h"

#include "madara/knowledge_engine/Knowledge_Base.h"

ACE_Atomic_Op<ACE_Mutex,int> thread_count (0);
static int max_threads = 2;
static Madara::Knowledge_Engine::Knowledge_Base knowledge (
  "localhost", Madara::Transport::SPLICE);

// command line arguments
int parse_args (int argc, ACE_TCHAR * argv[]);

static void * worker(void * &)
{
  int id = ++thread_count;
  int last =  id - 1;

  std::stringstream buffer;

  if (last > 0)
  {
    //ACE_OS::sleep (3);

    // wait for the last thread to update database

	buffer << "thread";
	buffer << last;
    ACE_DEBUG ((LM_DEBUG, "(%P|%t) Waiting for %s\n", buffer.str ().c_str ()));
    knowledge.wait (buffer.str ());
  }

  buffer.str ("");
  buffer << "thread";
  buffer << id;

  ACE_DEBUG ((LM_DEBUG, "(%P|%t) Setting %s\n", buffer.str ().c_str ()));
  knowledge.set (buffer.str (), Madara::Knowledge_Record::Integer (1));

  if (id == max_threads)
  {
    ACE_DEBUG ((LM_DEBUG, "(%P|%t) Setting terminated\n"));
    knowledge.set ("terminated");
  }
  else
  {
    ACE_DEBUG ((LM_DEBUG, "(%P|%t) Waiting for terminated\n"));
    knowledge.wait ("terminated");
  }

  ACE_DEBUG ((LM_DEBUG, "(%P|%t) Thread terminated\n"));
  return 0;
}

int ACE_TMAIN (int argc, ACE_TCHAR * argv[])
{
  int retcode = parse_args (argc, argv);

  if (retcode < 0)
    return retcode;

  ACE_LOG_MSG->priority_mask (LM_DEBUG | LM_NOTICE, ACE_Log_Msg::PROCESS);

  ACE_TRACE (ACE_TEXT ("main"));

  //Spawn off n_threads number of threads
  for (int i = 0; i < max_threads; i++)
  {
    ACE_DEBUG ((LM_DEBUG, "(%P|%t) Spawning thread %d\n", i));
    if (ACE_Thread::spawn ((ACE_THR_FUNC)worker, 0, THR_DETACHED | THR_NEW_LWP) == -1)
      ACE_DEBUG((LM_DEBUG,"Error in spawning thread\n"));
  } 

  ACE_DEBUG ((LM_DEBUG, "(%P|%t) Waiting for terminated\n"));
  knowledge.wait ("terminated");

  ACE_DEBUG ((LM_DEBUG, "(%P|%t) Terminated found to be true\n"));
  knowledge.print_knowledge ();

  return 0;
}



int parse_args (int argc, ACE_TCHAR * argv[])
{
  // options string which defines all short args
  ACE_TCHAR options [] = ACE_TEXT ("t:h");

  // create an instance of the command line args
  ACE_Get_Opt cmd_opts (argc, argv, options);

  // set up an alias for '-n' to be '--name'
  cmd_opts.long_option (ACE_TEXT ("threads"), 't', ACE_Get_Opt::ARG_REQUIRED);
  cmd_opts.long_option (ACE_TEXT ("help"), 'h', ACE_Get_Opt::ARG_REQUIRED);
 
  // temp for current switched option
  int option;
  ACE_TCHAR * arg;

  // iterate through the options until done
  while ((option = cmd_opts ()) != EOF)
  {
    arg = cmd_opts.opt_arg ();
    switch (option)
    {
    case 't':
      // thread number
      max_threads = ACE_OS::atoi(arg);
      break;
    case ':':
      ACE_ERROR_RETURN ((LM_ERROR, 
        ACE_TEXT ("ERROR: -%c requires an argument"), cmd_opts.opt_opt ()), -2);
    case 'h':
    default:
      ACE_DEBUG ((LM_DEBUG, "Program Options:      \n\
      -t (--threads)    number of threads to launch \n\
      -h (--help)      print this menu             \n"));
      ACE_ERROR_RETURN ((LM_ERROR, 
        ACE_TEXT ("Returning from Help Menu")), -1); 
      break; 
    }
  }

  return 0;
}
