
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <sstream>

#include "madara/knowledge/KnowledgeBase.h"
#include "madara/logger/GlobalLogger.h"

#include "madara/threads/Threader.h"
#include "madara/utility/Utility.h"
#include "madara/knowledge/containers/Queue.h"
#include "madara/knowledge/containers/Integer.h"

// shortcuts
namespace knowledge = madara::knowledge;
namespace containers = knowledge::containers;
namespace utility = madara::utility;
namespace threads = madara::threads;
namespace logger = madara::logger;

typedef madara::knowledge::KnowledgeRecord::Integer Integer;

// default transport settings

double max_wait (10.0);
double hertz (10);

Integer target (50);
int queue_length (100);

Integer producers (1);
Integer consumers (1);

// handle command line arguments
void handle_arguments (int argc, char ** argv)
{
  for (int i = 1; i < argc; ++i)
  {
    std::string arg1 (argv[i]);
    
    if (arg1 == "-c" || arg1 == "--consumers")
    {
      if (i + 1 < argc)
      {
        std::stringstream buffer (argv[i + 1]);
        buffer >> consumers;
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
        int level;
        std::stringstream buffer (argv[i + 1]);
        buffer >> level;
        logger::global_logger->set_level (level);
      }

      ++i;
    }
    else if (arg1 == "-p" || arg1 == "--producers")
    {
      if (i + 1 < argc)
      {
        std::stringstream buffer (argv[i + 1]);
        buffer >> producers;
      }

      ++i;
    }
    else if (arg1 == "-q" || arg1 == "--queue-length")
    {
      if (i + 1 < argc)
      {
        std::stringstream buffer (argv[i + 1]);
        buffer >> queue_length;
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
    else
    {
      madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_ALWAYS,
"\nProgram summary for %s:\n\n" \
"  Attempts to start a number of producer and consumer threads\n\n" \
" [-c|--consumers consumers] the number of information consumerss to start\n" \
" [-f|--logfile file]      log to a file\n" \
" [-l|--level level]       the logger level (0+, higher is higher detail)\n" \
" [-p|--producers producers] the number of information producers to start\n" \
" [-c|--consumers consumers]   the number of reader threads to start\n" \
" [-t|--target target]     the desired distributed count total\n"\
" [-q|--queue-length len]  the depth of the job queue\n"\
" [-w|--max-wait time]     maximum time to wait in seconds (double format)\n"\
" [-z|--hertz hertz]       the frequency of counts per second per thread\n" \
"\n",
        argv[0]);
      exit (0);
    }
  }
}

class Consumer: public threads::BaseThread
{
public:
  /**
   * Explicitly create virtual destructor for g++, since it does not
   * appear smart enough to do this by default
   **/
  virtual ~Consumer ()
  {
  }

  /**
    * Initializes thread with MADARA context
    * @param   context   context for querying current program state
    **/
  virtual void init (knowledge::KnowledgeBase & context)
  {
    // initialize references to variables in the knowledge base
    jobs.set_name ("jobs", context);
    jobs_completed.set_name (".jobs_completed", context);

    data = context;
  }

  /**
    * Checks the job queue until terminated for new tasks to perform
    **/
  virtual void run (void)
  {
    // dequeue until terminate
    madara::knowledge::KnowledgeRecord job = jobs.dequeue (false);

    if (job.is_valid ())
    {
      // Update the global counter of jobs done.
      ++jobs_completed;
    }
  }

private:
  containers::Queue jobs;
  containers::Integer jobs_completed;
  knowledge::KnowledgeBase data;
};

class Producer: public threads::BaseThread
{
public:
  /**
  * Explicitly create virtual destructor for g++, since it does not
  * appear smart enough to do this by default
  **/
  virtual ~Producer ()
  {
  }

  /**
    * Initializes thread with MADARA context
    * @param   context   context for querying current program state
    **/
  virtual void init (knowledge::KnowledgeBase & context)
  {
    jobs.set_name ("jobs", context);

    data = context;
  }

  /**
    * Generate a job. Jobs can be one of four types of events:
    * 0: Check News
    * 1: Check Stocks
    * 2: Check Email
    * 3: Check Schedule
    **/
  virtual void run (void)
  {
    /**
      * generate job consisting of 4 possible events: (int: 0-3)
      **/
    jobs.enqueue (madara::utility::rand_int (0, 3, false));
  }

private:
  containers::Queue jobs;
  knowledge::KnowledgeBase data;
};

int main (int argc, char ** argv)
{
  // handle all user arguments
  handle_arguments (argc, argv);
  
  // create a knowledge base and setup our id
  knowledge::KnowledgeBase knowledge;

  madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_ALWAYS,
    "Hertz rate set to %f\n"
    "Starting %ll producer threads\n"
    "Starting %ll consumer threads\n"
    "Job queue length is %d\n"
    "Target is set to %ll\n",
    hertz, producers, consumers, queue_length, target);

  containers::Integer jobs_completed (".jobs_completed", knowledge);
  containers::Queue jobs ("jobs", knowledge);
  jobs.resize (queue_length);

  // create a threader for running threads
  threads::Threader threader (knowledge);

  // explicitly set random seed to right now for randomizer engine
  madara::utility::rand_int (0, 1, true);

  for (Integer i = 0; i < producers; ++i)
  {
    std::stringstream buffer;
    buffer << "producer";
    buffer << i;

    // producers operate at a certain hertz
    threader.run (hertz, buffer.str (), new Producer (), true);
  }
  
  for (Integer i = 0; i < consumers; ++i)
  {
    std::stringstream buffer;
    buffer << "consumer";
    buffer << i;

    // consumers consume as quickly as possible
    threader.run (hertz, buffer.str (), new Consumer (), true);
  }
  
  Integer start_time = utility::get_time ();

  // awaken all threads to start work
  threader.resume ();

  while (jobs_completed < target)
  {
    // sleep until we have the target number of jobs completed
    utility::sleep (0.5);
  }

  // request all threads to terminate
  threader.terminate ();
  
  Integer end_time = utility::get_time ();
  Integer total_time = end_time - start_time;
  double total_time_in_secs = total_time;
  total_time_in_secs /= 1000000000;

  knowledge.set (".total_time_in_seconds", total_time_in_secs);

  madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_ALWAYS,
    "The consumers completed %ll jobs\n", *jobs_completed);

  knowledge.print ("Distributed count took {.total_time_in_seconds}s\n");

  threader.wait ();

  return 0;
}