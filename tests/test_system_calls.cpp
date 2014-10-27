
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

#include "madara/knowledge_engine/Knowledge_Base.h"

// command line arguments
int parse_args (int argc, ACE_TCHAR * argv[]);

// test functions
void test_system_calls (Madara::Knowledge_Engine::Knowledge_Base & knowledge);

int ACE_TMAIN (int argc, ACE_TCHAR * argv[])
{
  int retcode = parse_args (argc, argv);

  if (retcode < 0)
    return retcode;

  ACE_LOG_MSG->priority_mask (LM_INFO | LM_DEBUG, ACE_Log_Msg::PROCESS);

  Madara::Knowledge_Engine::Knowledge_Base knowledge;
  
  test_system_calls (knowledge);

  knowledge.print ();


  if (
    knowledge.get ("sample").type () == 
       (uint32_t) knowledge.get ("sample.type").to_integer () &&
    knowledge.get ("sample").size () == 
       (uint32_t) knowledge.get ("sample.size").to_integer () &&
    knowledge.get ("sample.clock").to_string () == "20" &&
    knowledge.get ("sample.size.clock").to_string () == "22" &&
    knowledge.get ("sample.type.clock").to_string () == "37" &&
    knowledge.get ("var1").to_string () == "10" &&
    knowledge.get ("var2").to_string () == "200" &&
    knowledge.get ("var3").to_string () == "300" &&
    knowledge.get ("small").to_string () == "ello" &&
    MADARA_debug_level == 5)
  {
    knowledge.print ("TEST SUCCESS\n");
  }
  else
  {
    knowledge.print ("TEST FAILED\n");
  }

  return 0;
}

/// Tests logicals operators (&&, ||)
void test_system_calls (
  Madara::Knowledge_Engine::Knowledge_Base & knowledge)
{
  knowledge.evaluate (
    "sample = #read_file ('/files/sample.jpg');"
    "sample.size = #size (sample);"
    "sample.type = #type (sample);"
    "#write_file ('/files/sample_from_system_call.jpg', sample);"
    "logging_level = #log_level (5);"
    "array[5] = 10;"
    "array[2] = 100;"
    "array[0] = 1000;"
    "#set_clock (37);"
    "#set_clock (sample, 20);"
    "#set_clock (sample.size, 22);"
    "#set_clock (sample.type, 37);"
    "sample.clock = #get_clock (sample);"
    "sample.size.clock = #get_clock (sample.size);"
    "sample.type.clock = #get_clock (sample.type);"
    "system_clock = #get_clock ();"
    "message = #expand ('Sample size is {sample.size}');"
    "statement = 'var1=10; var2=200; var3=300';"
    "logging_level = #log_level (10);"
    "small = #fragment ('hello world', 1, 4);"
    "logging_level = #log_level (5);"
    "madara_root = #expand_env ('$(MADARA_ROOT)');"
    "array_string = #to_string (array);"
    "#print ('Printing to log level 0\n');"
    "#print ('Printing to log level 1\n', 1);"
    "#print ('Printing to log level 2\n', 2);"
    "#print ('Printing to log level 3\n', 3);"
    "#print ('Printing to log level 4\n', 4);"
    "#print ('Printing to log level 5\n', 5);"
    "#print ('Printing to log level 6\n', 6);"
    "#print ('Printing to log level 7\n', 7);"
    "#print ('Printing to log level 8\n', 8);"
    "#print ('Printing to log level 9\n', 9);"
    "#print ('Printing to log level 10\n', 10);"
    "#print ('Tried printing 11 statements. 6-10 should not work.\n');"
    "#print ('evaluating ' + statement + '\n');"
    "#eval (statement);"
    "#print ('var1 = {var1}, var2 = {var2}, var3 = {var3}\n');"
    "#print ('Array is {array_string}.\n');"
    "array_string = #to_string (array, ' ');"
    "#print ('Array with space delimiter is {array_string}.\n');"
    );
}

int parse_args (int argc, ACE_TCHAR * argv[])
{
  for (int i = 1; i < argc; ++i)
  {
    std::string arg1 (argv[i]);

    if (arg1 == "-l" || arg1 == "--level")
    {
      if (i + 1 < argc)
      {
        std::stringstream buffer (argv[i + 1]);
        buffer >> MADARA_debug_level;
      }

      ++i;
    }
    else
    {
      MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_DEBUG, 
        "\nProgram summary for %s:\n\n" \
"This test checks the functionality of invoking MADARA system calls\n\n",
        " [-l|--level level]       the logger level (0+, higher is higher detail)\n" \
        "\n",
        argv[0]));
      exit (0);
    }
  }

  return 0;
}

