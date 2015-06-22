
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <sstream>

#include "madara/knowledge_engine/Knowledge_Base.h"

#include "madara/threads/Threader.h"
#include "madara/utility/Utility.h"
#include "madara/knowledge_engine/containers/Queue.h"
#include "madara/knowledge_engine/containers/Integer.h"
#include "madara/logger/Global_Logger.h"

// shortcuts
namespace engine = Madara::Knowledge_Engine;
namespace containers = engine::Containers;
namespace utility = Madara::Utility;
namespace threads = Madara::Threads;
namespace logger = Madara::Logger;

typedef Madara::Knowledge_Record::Integer Integer;

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
        std::stringstream buffer (argv[i + 1]);
        int level;
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
      logger::global_logger->log (logger::LOG_ALWAYS,
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

class Consumer: public threads::Base_Thread
{
public:
  /**
    * Initializes thread with MADARA context
    * @param   context   context for querying current program state
    **/
  virtual void init (engine::Knowledge_Base & context)
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
    // dequeue until terminated
    Madara::Knowledge_Record job = jobs.dequeue (false);

    if (job.is_valid ())
    {
      /**
        * use std::cerr instead of data.print to minimize time spent in
        * critical sections
        **/
      if (job.to_integer () == 0)
      {
        std::cerr << "Checking News\n";
      }
      else if (job.to_integer () == 1)
      {
        std::cerr << "Checking Stocks\n";
      }
      else if (job.to_integer () == 2)
      {
        std::cerr << "Checking Email\n";
      }
      else if (job.to_integer () == 3)
      {
        std::cerr << "Checking Schedule\n";
      }
      else
      {
        std::cerr << "Request to terminate from main thread.\n";
      }
        
      // Update the global counter of jobs done.
      ++jobs_completed;
    }
  }

private:
  containers::Queue jobs;
  containers::Integer jobs_completed;
  engine::Knowledge_Base data;
};

class Producer: public threads::Base_Thread
{
public:
  /**
    * Initializes thread with MADARA context
    * @param   context   context for querying current program state
    **/
  virtual void init (engine::Knowledge_Base & context)
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
    jobs.enqueue (Madara::Utility::rand_int (0, 3, false));
  }

private:
  containers::Queue jobs;
  engine::Knowledge_Base data;
};

int main (int argc, char ** argv)
{
  // handle all user arguments
  handle_arguments (argc, argv);
  
  // create a knowledge base and setup our id
  engine::Knowledge_Base knowledge;

  std::cerr << "Hertz rate set to " << hertz << "\n";
  std::cerr << "Starting " << producers << " producer threads\n";
  std::cerr << "Starting " << consumers << " consumer threads\n";
  std::cerr << "Job queue length is " << queue_length << "\n";
  std::cerr << "Target is set to " << target << "\n";

  containers::Integer jobs_completed (".jobs_completed", knowledge);
  containers::Queue jobs ("jobs", knowledge);
  jobs.resize (queue_length);

  // create a threader for running threads
  threads::Threader threader (knowledge);

  // explicitly set random seed to right now for randomizer engine
  Madara::Utility::rand_int (0, 1, true);

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
    
    // producers operate at a certain hertz
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

  std::cerr << "The consumers completed " <<
    *jobs_completed << " jobs\n";
  knowledge.print ("Distributed count took {.total_time_in_seconds}s\n");

  // print all knowledge to the screen
  knowledge.print ();

  threader.wait ();

  return 0;
}