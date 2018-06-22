
#include <string>
#include <vector>
#include <iostream>
#include <assert.h>

#include "madara/knowledge/KnowledgeBase.h"
#include "madara/logger/GlobalLogger.h"

namespace logger = madara::logger;
namespace knowledge = madara::knowledge;
typedef  knowledge::KnowledgeRecord  KnowledgeRecord;

int num_fails = 0;

// command line arguments
int parse_args (int argc, char * argv[]);

// test functions
void test_system_calls (madara::knowledge::KnowledgeBase & knowledge);

int main (int argc, char * argv[])
{
  int retcode = parse_args (argc, argv);

  if (retcode < 0)
    return retcode;

  madara::knowledge::KnowledgeBase knowledge;
  
  test_system_calls (knowledge);

  knowledge.print ();


  if (
    knowledge.get ("sample").type () == 
       (int32_t) knowledge.get ("sample.type").to_integer () &&
    knowledge.get ("sample").size () == 
       (uint32_t) knowledge.get ("sample.size").to_integer () &&
    knowledge.get ("sample.clock").to_string () == "20" &&
    knowledge.get ("sample.size.clock").to_string () == "22" &&
    knowledge.get ("sample.type.clock").to_string () == "37" &&
    knowledge.get ("var1").to_string () == "10" &&
    knowledge.get ("var2").to_string () == "200" &&
    knowledge.get ("var3").to_string () == "300" &&
    knowledge.get ("small").to_string () == "ello" &&
    logger::global_logger->get_level () == 5)
  {
    knowledge.print ("TEST SUCCESS\n");
  }
  else
  {
    knowledge.print ("TEST FAILED\n");
    ++num_fails;
  }

  if (num_fails > 0)
  {
    std::cerr << "OVERALL: FAIL. " << num_fails << " tests failed.\n";
  }
  else
  {
    std::cerr << "OVERALL: SUCCESS.\n";
  }

  return num_fails;
}

/// Tests logicals operators (&&, ||)
void test_system_calls (
  madara::knowledge::KnowledgeBase & knowledge)
{
#ifndef _MADARA_NO_KARL_
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
    "two_to_4th = #pow(2,4);"
    "square_of_16 = #sqrt(16);"
    "square_two_to_4th = #sqrt(two_to_4th);"
    "cos_1_1 = #cos(1.1);"
    "sin_1_1 = #sin(1.1);"
    "tan_1_1 = #tan(1.1);"
    );

  knowledge.evaluate (
    "two_to_4th = #pow(2, 4);"
    );

  knowledge.evaluate (
    "tan_1_1 = #tan(1.1);"
    );

  knowledge.evaluate (
    "begin_s = #get_time_seconds();"
    "begin_ns = #get_time();"
    "#sleep(3);"
    "end_s = #get_time_seconds();"
    "end_ns = #get_time();"
    "diff_s = end_s - begin_s;"
    "diff_ns = end_ns - begin_ns;"
    );

  KnowledgeRecord::Integer two_to_4th =
    knowledge.get ("two_to_4th").to_integer ();

  KnowledgeRecord::Integer square_of_16 =
    knowledge.get ("square_of_16").to_integer ();

  KnowledgeRecord::Integer square_two_to_4th =
    knowledge.get ("square_two_to_4th").to_integer ();

  KnowledgeRecord::Integer diff_s =
    knowledge.get ("diff_s").to_integer ();

  KnowledgeRecord::Integer diff_ns =
    knowledge.get ("diff_ns").to_integer ();

  if (two_to_4th != 16)
  {
    std::cerr << "FAIL: pow(2,4) returned " << two_to_4th <<
      " instead of 16.\n";
    ++num_fails;
  }

  if (square_of_16 != 4)
  {
    std::cerr << "FAIL: sqrt(16) returned " << square_of_16 << 
      " instead of 4.\n";
    ++num_fails;
  }

  if (square_two_to_4th != 4)
  {
    std::cout << "FAIL: sqrt(pow(2,4)) returned " << square_two_to_4th <<
      " instead of 4. [1]\n";
    ++num_fails;
  }

  if (diff_s != 3 && diff_s != 4)
  {
    std::cout << "FAIL: #get_time_seconds returned diff of " << diff_s <<
      " instead of 3 (.\n";
    ++num_fails;
  }

  if (diff_ns < 3000000000 || diff_ns > 4000000000)
  {
    std::cout << "FAIL: #get_time_ns returned diff of " << diff_ns <<
      " instead of ~3000000000 (.\n";
    ++num_fails;
  }

  std::cerr << "Testing #set_scientific...\n";

  knowledge.evaluate (
    "#set_scientific()"
    );

  knowledge.print ();

  std::cerr << "Output being reverted with #set_fixed...\n";

  knowledge.evaluate (
    "#set_fixed()"
    );

#else
  std::cout << "This test is disabled due to karl feature being disabled.\n";
#endif
}

int parse_args (int argc, char * argv[])
{
  for (int i = 1; i < argc; ++i)
  {
    std::string arg1 (argv[i]);

    if (arg1 == "-l" || arg1 == "--level")
    {
      if (i + 1 < argc)
      {
        std::stringstream buffer (argv[i + 1]);
        int level;
        buffer >> level;
        logger::global_logger->set_level (level);
      }

      ++i;
    }
    else
    {
      madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_ALWAYS, 
        "\nProgram summary for %s:\n\n" \
"This test checks the functionality of invoking MADARA system calls\n\n",
        " [-l|--level level]       the logger level (0+, higher is higher detail)\n" \
        "\n",
        argv[0]);
      exit (0);
    }
  }

  return 0;
}

