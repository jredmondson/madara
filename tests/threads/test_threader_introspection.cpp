
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <sstream>

#include "madara/knowledge/KnowledgeBase.h"

#include "madara/threads/Threader.h"
#include "madara/knowledge/containers/Integer.h"
#include "madara/logger/GlobalLogger.h"

#include "madara/utility/Timer.h"


// shortcuts
namespace knowledge = madara::knowledge;
namespace containers = knowledge::containers;
namespace utility = madara::utility;
namespace threads = madara::threads;
namespace logger = madara::logger;

typedef madara::knowledge::KnowledgeRecord::Integer Integer;

// default transport settings

double max_wait (10.0);
double hertz (0.5);
double second_hertz (5);

Integer target (50);

Integer counters (1);
Integer readers (0);

// handle command line arguments
void handle_arguments (int argc, char ** argv)
{
  for (int i = 1; i < argc; ++i)
  {
    std::string arg1 (argv[i]);

    if (arg1 == "-c" || arg1 == "--counters")
    {
      if (i + 1 < argc)
      {
        std::stringstream buffer (argv[i + 1]);
        buffer >> counters;
      }

      ++i;
    }
    else if (arg1 == "-f" || arg1 == "--logfile")
    {
      if (i + 1 < argc)
      {
        logger::global_logger->add_file (argv[i + 1]);
      }

      ++i;
    }
    else if (arg1 == "-l" || arg1 == "--level")
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
    else if (arg1 == "-r" || arg1 == "--readers")
    {
      if (i + 1 < argc)
      {
        std::stringstream buffer (argv[i + 1]);
        buffer >> readers;
      }

      ++i;
    }
    else if (arg1 == "-t" || arg1 == "--target")
    {
      if (i + 1 < argc)
      {
        std::stringstream buffer (argv[i + 1]);
        buffer >> target;
      }

      ++i;
    }
    else if (arg1 == "-w" || arg1 == "--max-wait")
    {
      if (i + 1 < argc)
      {
        std::stringstream buffer (argv[i + 1]);
        buffer >> max_wait;
      }

      ++i;
    }
    else if (arg1 == "-z" || arg1 == "--hertz")
    {
      if (i + 1 < argc)
      {
        std::stringstream buffer (argv[i + 1]);
        buffer >> hertz;
      }

      ++i;
    }
    else if (arg1 == "-sz" || arg1 == "--second-hertz")
    {
      if (i + 1 < argc)
      {
        std::stringstream buffer (argv[i + 1]);
        buffer >> second_hertz;
      }

      ++i;
    }
    else
    {
      madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_ALWAYS,
"\nProgram summary for %s:\n\n" \
"  Attempts to start a counter and then changes the hertz rate after 10s by\n" \
"  5x the hertz rate\n" \
" [-c|--counters counters] the number of counter threads to start\n" \
" [-f|--logfile file]      log to a file\n" \
" [-l|--level level]       the logger level (0+, higher is higher detail)\n" \
" [-r|--readers readers]   the number of reader threads to start\n" \
" [-t|--target target]     the desired distributed count total\n"\
" [-w|--max-wait time]     maximum time to wait in seconds (double format)\n"\
" [-z|--hertz hertz]       the frequency of counts per second per thread\n" \
" [-sz|--second-hertz hertz]  the second frequency of counts per second per thread\n" \
"\n",
        argv[0]);
      exit (0);
    }
  }
}

class EmptyThread : public threads::BaseThread
{
public:
  /**
    * Initializes thread with MADARA context
    **/
  virtual void init (knowledge::KnowledgeBase &)
  {
  }

  /**
    * Executes the main thread logic
    **/
  virtual void run (void)
  {
  }

private:
  containers::Integer counter;
  knowledge::KnowledgeBase data;
  std::string message;
};

int main (int argc, char ** argv)
{
  // handle all user arguments
  handle_arguments (argc, argv);
  
  // create a knowledge base and setup our id
  knowledge::KnowledgeBase knowledge;

  madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_ALWAYS,
    "Hertz rate set to %f\n"
    "Second hertz rate set to %f\n"
    "Counters is set to %" PRId64 "\n"
    "Target is set to %" PRId64 "\n",
    hertz, second_hertz, counters, target);

  int64_t estimated_count = target - counters * hertz * 5;
  uint64_t estimated_time =
    (uint64_t)(estimated_count / (counters * hertz) + 5);

  madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_ALWAYS,
    "Testing. Estimated completion in %" PRIu64 " seconds...\n",
    estimated_time);
    
  // create a threader for running threads
  threads::Threader threader (knowledge);

  threader.enable_debug ();

  utility::Timer <utility::Clock> timer;
  timer.start ();

  for (Integer i = 0; i < counters; ++i)
  {
    std::stringstream buffer;
    buffer << "thread";
    buffer << i;

    threader.run (hertz, buffer.str (), new EmptyThread (), true);
  }

  std::vector <knowledge::containers::Integer> executions (10);

  threader.resume ();

  // sleep for 5 seconds before starting second hertz rate
  utility::sleep (5.0);

  knowledge::KnowledgeBase control = threader.get_control_plane ();

  for (Integer i = 0; i < counters; ++i)
  {
    std::stringstream buffer;
    buffer << "thread";
    buffer << i;

    threader.change_hertz (buffer.str (), second_hertz);
    
    buffer << ".executions";

    executions[i].set_name (buffer.str (), control);
  }

  int64_t counter = 0;

  // wait for the counter to reach the target number
  while (counter < target)
  {
    counter = 0;
    for (auto value : executions)
    {
      counter += *value;
    }
    // sleep for half a second and try again
    utility::sleep (0.5);
  }

  timer.stop ();

  madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_ALWAYS,
    "Test took %" PRIu64 " seconds\n",
    timer.duration_s ());
  
  threader.terminate ();
  
  // print the aggregate counter to the screen
  control.print ();

  threader.wait ();

  madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_ALWAYS,
    "Starting new thread without debugging and running for 5s\n",
    timer.duration_s ());
  
  threader.disable_debug ();
  threader.run (hertz, "no_debug", new EmptyThread (), true);

  // sleep for 5 seconds before printing and stopping
  utility::sleep (5.0);

  // print the aggregate counter to the screen
  control.print ();

  threader.terminate ();
  
  threader.wait ();

  return 0;
}
