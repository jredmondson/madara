
#include <string>
#include <vector>
#include <iostream>
#include <sstream>

#include "madara/knowledge/KnowledgeBase.h"
#include "madara/logger/GlobalLogger.h"
#include "madara/utility/Utility.h"
#include "madara/utility/Timer.h"

namespace logger = madara::logger;
namespace utility = madara::utility;

madara::knowledge::WaitSettings wait_settings;
int madara_fails = 0;

void handle_arguments(int argc, char** argv)
{
  for(int i = 1; i < argc; ++i)
  {
    std::string arg1(argv[i]);

    if(arg1 == "-p" || arg1 == "--poll-frequency")
    {
      if(i + 1 < argc)
      {
        std::stringstream buffer(argv[i + 1]);
        buffer >> wait_settings.poll_frequency;
      }

      ++i;
    }
    else if(arg1 == "-m" || arg1 == "--max")
    {
      if(i + 1 < argc)
      {
        std::stringstream buffer(argv[i + 1]);
        buffer >> wait_settings.max_wait_time;
      }

      ++i;
    }
    else if(arg1 == "-l" || arg1 == "--level")
    {
      if(i + 1 < argc)
      {
        std::stringstream buffer(argv[i + 1]);
        int level;
        buffer >> level;
        logger::global_logger->set_level(level);
      }

      ++i;
    }
    else if(arg1 == "-f" || arg1 == "--logfile")
    {
      if(i + 1 < argc)
      {
        logger::global_logger->add_file(argv[i + 1]);
      }

      ++i;
    }
    else
    {
      madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_ALWAYS,
          "\nProgram summary for %s:\n\n"
          "  Test periodic waits for accuracy of timers.\n\n"
          " [-p|--poll-frequency period] period between evals (def 0.5s)\n"
          " [-m|--max max_wait_time]  the maximum number of seconds to wait\n"
          " [-l|--level level]       the logger level (0+, higher is higher "
          "detail)\n"
          " [-f|--logfile file]      log to a file\n"
          "\n",
          argv[0]);
      exit(0);
    }
  }
}

int main(int argc, char* argv[])
{
  // defaults are to poll every 0.5s for 5s
  wait_settings.poll_frequency = 0.50;
  wait_settings.max_wait_time = 5;

  handle_arguments(argc, argv);

#ifndef _MADARA_NO_KARL_
  madara::knowledge::KnowledgeBase knowledge;

  // for convenience with calcuations, set the main arguments
  knowledge.set("max_wait", wait_settings.max_wait_time);
  knowledge.set("poll_frequency", wait_settings.poll_frequency);

  utility::Timer<utility::Clock> timer;
  timer.start();

  // "," returns a minimum, so this wait will timeout
  knowledge.wait("++count, 0", wait_settings);

  timer.stop();

  knowledge.evaluate("hertz = #to_double (count) / max_wait");
  knowledge.set("actual_wait_time_ns", timer.duration_ns());

  knowledge.print();

  std::cerr << "Checking if wait time was greater than 5 s...";
  if(timer.duration_s() >= 5 && timer.duration_s() < 7)
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "SUCCESS\n";
    ++madara_fails;
  }

  if(madara_fails > 0)
  {
    std::cerr << "OVERALL: FAIL. " << madara_fails << " tests failed.\n";
  }
  else
  {
    std::cerr << "OVERALL: SUCCESS.\n";
  }

#else
  madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_ALWAYS,
      "This test is disabled due to karl feature being disabled.\n");
#endif
  return madara_fails;
}
