
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <sstream>

#include "madara/knowledge/KnowledgeBase.h"

#include "madara/threads/Threader.h"
#include "madara/knowledge/containers/Integer.h"
#include "madara/logger/GlobalLogger.h"

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

class CounterThread : public threads::BaseThread
{
public:
  /**
    * Initializes thread with MADARA context
    * @param   context   context for querying current program state
    **/
  virtual void init (knowledge::KnowledgeBase & context)
  {
    counter.set_name ("counter", context);

    data = context;

    message = "Counter set to {counter} by thread " + name + "\n";
  }

  /**
    * Executes the main thread logic
    **/
  virtual void run (void)
  {
    ++counter;
    data.print (message);
    int rnd = std::rand() % 19 + (-9);
    int logger_rnd = std::rand() % 3 + (-1);
    
    madara::logger::Logger::set_thread_level(logger_rnd);
    madara_logger_ptr_log (logger::global_logger.get(), rnd,//logger::LOG_ALWAYS,
                           "CounterThread::Run MGT: %MGT  rnd=%d lrnd=%d\n",rnd,logger_rnd);

    madara_logger_ptr_log (logger::global_logger.get(), rnd,//logger::LOG_ALWAYS,
                           "CounterThread::Run MTN: %MTN rnd=%d lrnd=%d\n",rnd,logger_rnd);

    madara_logger_ptr_log (logger::global_logger.get(), rnd,//logger::LOG_ALWAYS,
                           "CounterThread::Run MTZ: %MTZ rnd=%d lrnd=%d\n",rnd,logger_rnd);
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
  
  logger::global_logger.get()->set_timestamp_format("%x %X %MTZ %MGT (%MTN): ");

  // create a knowledge base and setup our id
  knowledge::KnowledgeBase knowledge;

  madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_ALWAYS,
    "Hertz rate set to %f\n"
    "Second hertz rate set to %f\n"
    "Counters is set to %ll\n"
    "Target is set to %ll\n",
    hertz, second_hertz, counters, target);

  madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_ALWAYS,
                         "MGT: %MGT\n");

  madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_ALWAYS,
                         "MTN: %MTN\n");

  madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_ALWAYS,
                         "MTZ: %MTZ\n");
// create a counter
  containers::Integer counter ("counter", knowledge);

  // create a threader for running threads
  threads::Threader threader (knowledge);

  for (Integer i = 0; i < counters; ++i)
  {
    std::stringstream buffer;
    buffer << "thread";
    buffer << i;

    threader.run (hertz, buffer.str (), new CounterThread (), true);
  }

  Integer start_time = utility::get_time ();

  threader.resume ();

  // sleep for 5 seconds before starting second hertz rate
  utility::sleep (5.0);

  knowledge.print ("After 5s, count is {counter}.\n");
  knowledge.print ("Changing hertz to second hertz.\n");

  for (Integer i = 0; i < counters; ++i)
  {
    std::stringstream buffer;
    buffer << "thread";
    buffer << i;

    threader.change_hertz (buffer.str (), second_hertz);
  }

  // wait for the counter to reach the target number
  while (*counter < target)
  {
    // sleep for half a second and try again
    utility::sleep (0.5);
  }

  threader.terminate ();
  
  Integer end_time = utility::get_time ();
  Integer total_time = end_time - start_time;
  double total_time_in_secs = (double)total_time;
  total_time_in_secs /= 1000000000;

  knowledge.set (".total_time_in_seconds", total_time_in_secs);

  std::cerr << "The final tally of the distributed counter was " <<
    *counter << "\n";
  knowledge.print ("Distributed count took {.total_time_in_seconds}s\n");


  // print the aggregate counter to the screen
  knowledge.print ();

  threader.wait ();

  return 0;
}
