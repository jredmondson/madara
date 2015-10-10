
//#define MADARA_NTRACE    0
//#define ACE_NTRACE    0
////#define ACE_NLOGGING  0
//#define ACE_NDEBUG    0

#include <string>
#include <vector>
#include <iostream>
#include <assert.h>

#include "ace/Log_Msg.h"
#include "ace/Get_Opt.h"

#include "madara/knowledge/KnowledgeBase.h"

typedef madara::knowledge::KnowledgeRecord::Integer Integer;

// command line arguments
int parse_args (int argc, ACE_TCHAR * argv[]);

// test functions
void test_print_statement (madara::knowledge::KnowledgeBase & knowledge);


int ACE_TMAIN (int argc, ACE_TCHAR * argv[])
{
  int retcode = parse_args (argc, argv);

  if (retcode < 0)
    return retcode;

  ACE_LOG_MSG->priority_mask (LM_DEBUG | LM_NOTICE, ACE_Log_Msg::PROCESS);

  ACE_TRACE (ACE_TEXT ("main"));

  madara::knowledge::KnowledgeBase knowledge;

  // run tests
  test_print_statement (knowledge);

  knowledge.print ();

  return 0;
}

/// Tests logicals operators (&&, ||)
void test_print_statement (madara::knowledge::KnowledgeBase & knowledge)
{
  ACE_TRACE (ACE_TEXT ("test_expansion"));

  std::string statement, exp_statement;
  
  knowledge.clear ();
  
#ifndef _MADARA_NO_KARL_
  knowledge.evaluate (
    ".var1 = 1; \
    .var2 = 2; \
    .var3 = 3; \
    .var2is2 = 1; \
    .var22 = 2; \
    MyState.1.2 = 10; \
    ");
  
#else
  knowledge.set (".var1", Integer (1));
  knowledge.set (".var2", Integer (2));
  knowledge.set (".var3", Integer (3));
  knowledge.set (".var2is2", Integer (1));
  knowledge.set (".var22", Integer (2));
  knowledge.set ("MyState.1.2", Integer (10));
#endif

  statement = "Hello kitty {.var1}\n";

  exp_statement = knowledge.expand_statement (statement);
  assert (statement == "Hello kitty 1");
  
  ACE_DEBUG ((LM_DEBUG, exp_statement.c_str ()));

  statement = "Hello kitty {.var{.var2}is{.var22}}\n";

  exp_statement = knowledge.expand_statement (statement);
  assert (statement == "Hello kitty 1");

  ACE_DEBUG ((LM_DEBUG, exp_statement.c_str ()));

  statement = "MyState.{.var1}.{.var2} = {MyState.{.var1}.{.var2}}\n";

  exp_statement = knowledge.expand_statement (statement);
  assert (statement == "MyState.1.2 = 10");

  ACE_DEBUG ((LM_DEBUG, exp_statement.c_str ()));

}

int parse_args (int argc, ACE_TCHAR * argv[])
{
  // options string which defines all short args
  ACE_TCHAR options [] = ACE_TEXT ("h");

  // create an instance of the command line args
  ACE_Get_Opt cmd_opts (argc, argv, options);

  // set up an alias for '-n' to be '--name'
  cmd_opts.long_option (ACE_TEXT ("help"), 'h', ACE_Get_Opt::ARG_REQUIRED);
 
  // temp for current switched option
  int option;
//  ACE_TCHAR * arg;

  // iterate through the options until done
  while ((option = cmd_opts ()) != EOF)
  {
    //arg = cmd_opts.opt_arg ();
    switch (option)
    {
    case ':':
      ACE_ERROR_RETURN ((LM_ERROR, 
        ACE_TEXT ("ERROR: -%c requires an argument"), 
           cmd_opts.opt_opt ()), -2); 
    case 'h':
    default:
      ACE_DEBUG ((LM_DEBUG, "Program Options:      \n\
      -h (--help)      print this menu             \n"));
      ACE_ERROR_RETURN ((LM_ERROR, 
        ACE_TEXT ("Returning from Help Menu")), -1); 
      break;
    }
  }

  return 0;
}

