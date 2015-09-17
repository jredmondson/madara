
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <assert.h>

#include "ace/High_Res_Timer.h"
#include "ace/OS_NS_Thread.h"
#include "ace/Sched_Params.h"
#include "ace/Guard_T.h"
#include "ace/Recursive_Thread_Mutex.h"

#include "madara/knowledge_engine/Knowledge_Base.h"
#include "madara/filters/Aggregate_Filter.h"

#include "madara/knowledge_engine/containers/Integer.h"
#include "madara/knowledge_engine/containers/Integer_Vector.h"
#include "madara/knowledge_engine/containers/Barrier.h"
#include "madara/knowledge_engine/containers/String.h"
#include "madara/utility/Utility.h"
#include "madara/logger/Global_Logger.h"
#include "madara/threads/Threader.h"

namespace logger = Madara::Logger;
namespace engine = Madara::Knowledge_Engine;
namespace containers = engine::Containers;
namespace utility = Madara::Utility;
namespace threads = Madara::Threads;
namespace filters = Madara::Filters;

// useful namespace aliases and typedefs

namespace engine = Madara::Knowledge_Engine;
namespace containers = engine::Containers;
namespace transport = Madara::Transport;

typedef Madara::Knowledge_Record::Integer Integer;

// default transport settings
std::string host ("");
const std::string default_multicast ("239.255.0.1:4150");
transport::QoS_Transport_Settings settings;

bool started = false;

// amount of time in seconds to burst payloads
double publish_time = 10.0;

// payload size to burst
unsigned int data_size = 1000;

// unlimited publish hertz by default
double publish_hertz = 0.0;

// handle command line arguments
void handle_arguments (int argc, char ** argv)
{
  for (int i = 1; i < argc; ++i)
  {
    std::string arg1 (argv[i]);

    if (arg1 == "-m" || arg1 == "--multicast")
    {
      if (i + 1 < argc)
      {
        settings.hosts.push_back (argv[i + 1]);
        settings.type = Madara::Transport::MULTICAST;
      }
      ++i;
    }
    else if (arg1 == "-b" || arg1 == "--broadcast")
    {
      if (i + 1 < argc)
      {
        settings.hosts.push_back (argv[i + 1]);
        settings.type = Madara::Transport::BROADCAST;
      }
      ++i;
    }
    else if (arg1 == "-u" || arg1 == "--udp")
    {
      if (i + 1 < argc)
      {
        settings.hosts.push_back (argv[i + 1]);
        settings.type = Madara::Transport::UDP;
      }
      ++i;
    }
    else if (arg1 == "-o" || arg1 == "--host")
    {
      if (i + 1 < argc)
        host = argv[i + 1];

      ++i;
    }
    else if (arg1 == "-d" || arg1 == "--domain")
    {
      if (i + 1 < argc)
        settings.domains = argv[i + 1];

      ++i;
    }
    else if (arg1 == "-e" || arg1 == "--threads")
    {
      if (i + 1 < argc)
      {
        std::stringstream buffer (argv[i + 1]);
        buffer >> settings.read_threads;
      }

      ++i;
    }
    else if (arg1 == "-i" || arg1 == "--id")
    {
      if (i + 1 < argc)
      {
        std::stringstream buffer (argv[i + 1]);
        buffer >> settings.id;
      }

      ++i;
    }
    else if (arg1 == "-p" || arg1 == "--processes")
    {
      if (i + 1 < argc)
      {
        std::stringstream buffer (argv[i + 1]);
        buffer >> settings.processes;
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
    else if (arg1 == "-p" || arg1 == "--drop-rate")
    {
      if (i + 1 < argc)
      {
        double drop_rate;
        std::stringstream buffer (argv[i + 1]);
        buffer >> drop_rate;
        
        settings.update_drop_rate (drop_rate,
          Madara::Transport::PACKET_DROP_DETERMINISTIC);
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
    else if (arg1 == "-q" || arg1 == "--queue-length")
    {
      if (i + 1 < argc)
      {
        std::stringstream buffer (argv[i + 1]);
        buffer >> settings.queue_length;
      }

      ++i;
    }
    else if (arg1 == "-r" || arg1 == "--reduced")
    {
      settings.send_reduced_message_header = true;
    }
    else if (arg1 == "-s" || arg1 == "--size")
    {
      if (i + 1 < argc)
      {
        std::stringstream buffer (argv[i + 1]);
        buffer >> data_size;
      }

      ++i;
    }
    else if (arg1 == "-z" || arg1 == "--pub-hertz")
    {
      if (i + 1 < argc)
      {
        std::stringstream buffer (argv[i + 1]);
        buffer >> publish_hertz;
      }

      ++i;
    }
    else if (arg1 == "-0" || arg1 == "--deadline")
    {
      if (i + 1 < argc)
      {
        std::stringstream buffer (argv[i + 1]);
        buffer >> settings.deadline;
      }

      ++i;
    }
    else if (arg1 == "--read-threads")
    {
      if (i + 1 < argc)
      {
        std::stringstream buffer (argv[i + 1]);
        buffer >> settings.read_threads;
      }

      ++i;
    }
    else
    {
      madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_ALWAYS, 
        "\nProgram summary for %s:\n\n" \
        "  Tests one or more chattering agents for throughput.\n\n" \
        " [-b|--broadcast ip:port] the broadcast ip to send and listen to\n" \
        " [-d|--domain domain]     the knowledge domain to send and listen to\n" \
        " [-e|--threads threads]   number of read threads\n" \
        " [-f|--logfile file]      log to a file\n" \
        " [-i|--id id]             the id of this agent (should be non-negative)\n" \
        " [-l|--level level]       the logger level (0+, higher is higher detail)\n" \
        " [-m|--multicast ip:port] the multicast ip to send and listen to\n" \
        " [-o|--host hostname]     the hostname of this process (def:localhost)\n" \
        " [-p|--processes num]     the number of processes (def:2)\n" \
        " [-q|--queue-length len   the queue size to use for the test\n" \
        " [-r|--reduced]           use the reduced message header\n" \
        " [-s|--size size]         size of data packet to send in bytes\n" \
        " [-t|--time time]         time to burst messages for throughput test\n" \
        " [-u|--udp ip:port]       the udp ips to send to (first is self to bind to)\n" \
        " [-z|--pub-hertz hertz]   publish hertz speed\n" \
        " [-0|--deadline seconds]  the deadline for dropping messages from a pub\n" \
        " [--read-threads threads] number of read threads to spawn\n" \
        "\n",
        argv[0]);
      exit (0);
    }
  }
}



class Publisher : public threads::Base_Thread
{
public:
  /**
   * Constructor
   **/
  Publisher (engine::Knowledge_Base & context)
    : knowledge (&context), publishes (".publishes", context),
    id ("id", context, (Integer)settings.id)
  {
    logger::global_logger->log (logger::LOG_ALWAYS,
      "Initializing publisher thread\n");

    // create container and references
    packet = context.get_ref (
      "packet", engine::Knowledge_Reference_Settings (true));

    // set initial packet
    utility::Scoped_Array <unsigned char> buffer (new unsigned char[data_size]);
    context.set_file (packet, buffer.get_ptr (), (size_t)data_size);
  }

  /**
  * Initializes thread with MADARA context
  * @param   context   context for querying current program state
  **/
  virtual void init (engine::Knowledge_Base & context)
  {
  }

  /**
  * Executes the main thread logic
  **/
  virtual void run (void)
  {
    // increment counter
    knowledge->mark_modified (packet);
    id.modify ();
    ++publishes;
    knowledge->send_modifieds ();
  }

private:
  engine::Knowledge_Base * knowledge;
  containers::Integer publishes;
  containers::Integer id;
  engine::Variable_Reference packet;
};

class Receiver : public filters::Aggregate_Filter
{
public:
  Receiver (int publishers)
    : processes (publishers), started (false)
  {
  }

  virtual void filter (Madara::Knowledge_Map & records,
    const transport::Transport_Context & transport_context,
    engine::Variables & vars)
  {
    Madara::Knowledge_Map::iterator packet = records.find ("packet");
    if (packet != records.end ())
    {
      if (!started)
      {
        logger::global_logger->log (logger::LOG_ALWAYS,
          "Initializing receiver filter variable arrays\n");

        counters.set_name (".counters", vars, processes);
        sizes.set_name (".sizes", vars, processes);
        start_times.set_name (".starts", vars, processes);
        end_times.set_name (".ends", vars, processes);
        started = true;
      }

      Madara::Knowledge_Map::iterator id = records.find ("id");

      // if we have a valid packet
      if (id != records.end ())
      {
        // increase the counter for packets from the id
        size_t index = (size_t)id->second.to_integer ();
        Integer result = counters.inc (index);
        Integer cur_time = (Integer)transport_context.get_current_time ();

        end_times.set (index, cur_time);

        // if this is the first time we've counted from this id
        if (result == 1)
        {
          // then update the size
          sizes.set (index, (Integer)packet->second.size ());
          start_times.set (index, cur_time);
        }
      }

      records.clear ();
    }
  }

  containers::Integer_Vector counters;
  containers::Integer_Vector sizes;
  containers::Integer_Vector start_times;
  containers::Integer_Vector end_times;
  int processes;
  bool started;
};


int main (int argc, char ** argv)
{
  settings.type = transport::MULTICAST;
  settings.queue_length = data_size * 1000;

  // unlimited hertz
  settings.read_thread_hertz = 0.0;
  settings.read_threads = 1;
  settings.processes = 2;

  // use ACE real time scheduling class
  int prio  = ACE_Sched_Params::next_priority
    (ACE_SCHED_FIFO,
     ACE_Sched_Params::priority_max (ACE_SCHED_FIFO),
     ACE_SCOPE_THREAD);
  ACE_OS::thr_setprio (prio);

  // handle all user arguments
  handle_arguments (argc, argv);

  if (settings.hosts.size () == 0)
  {
    // setup default transport as multicast
    settings.hosts.resize (1);
    settings.hosts[0] = default_multicast;
  }

  filters::Aggregate_Filter * receiver = new Receiver (settings.processes);

  settings.add_receive_filter (receiver);

  // create a knowledge base and setup our id
  engine::Knowledge_Base knowledge (host, settings);
  
  knowledge.set (".id", Integer (settings.id));
  knowledge.set (".processes", Integer (settings.processes));
  knowledge.set (".hertz", publish_hertz);

  logger::global_logger->log (logger::LOG_ALWAYS,
    "Creating barrier for %d processes\n", (int)settings.processes);

  containers::Barrier barrier ("barrier", knowledge, settings.id, settings.processes);

  logger::global_logger->log (logger::LOG_ALWAYS,
    "Barrier: process %id waiting on %d processes\n",
    (int)settings.id, (int)settings.processes);

  barrier.next ();
  while (!barrier.is_done ())
  {
    knowledge.send_modifieds ();
  }
  
  // send another update just in case a late joiner didn't get a chance to receive all
  barrier.modify ();
  knowledge.send_modifieds ();


  threads::Threader threader (knowledge);
  
  logger::global_logger->log (logger::LOG_ALWAYS,
    "Publishing at %f hz for %f s\n", publish_hertz, publish_time);

  threader.run (publish_hertz, "publisher", new Publisher (knowledge));

  logger::global_logger->log (logger::LOG_ALWAYS,
    "Publishing for %f s\n", publish_time);

  utility::sleep (publish_time);

  logger::global_logger->log (logger::LOG_ALWAYS,
    "Terminating publisher\n");

  threader.terminate ();

  logger::global_logger->log (logger::LOG_ALWAYS,
    "Waiting for publisher to terminate\n");

  threader.wait ();

  std::string agent_id = knowledge.expand_statement ("agent.{.id}");

  // let any listener know which test these results are coming from
  containers::String test_name ("test_name", knowledge);
  test_name = "network_throughput";
  

  // prepare the results to send to whoever is listening
  containers::Integer_Vector receives (agent_id + ".receives", knowledge);
  containers::Integer_Vector durations (agent_id + ".durations", knowledge, settings.processes);
  containers::Integer_Vector receive_hz (agent_id + ".receive_hz", knowledge, settings.processes);
  containers::Integer total_hz (agent_id + ".total_hz", knowledge);
  containers::Integer publishes (agent_id + ".publishes", knowledge,
    knowledge.get (".publishes").to_integer ());
  containers::Integer agents ("agents", knowledge,
    knowledge.get (".processes").to_integer ());

  // refer to variables used in receive filter
  containers::Integer_Vector counters (".counters", knowledge);
  containers::Integer_Vector sizes (".sizes", knowledge);
  containers::Integer_Vector start_times (".starts", knowledge);
  containers::Integer_Vector end_times (".ends", knowledge);

  // transfer the counters to the global receives variables
  counters.transfer_to (receives);

#ifndef _MADARA_NO_KARL_
  // process the durations
  knowledge.evaluate (
    ".i[0->.processes)(.starts.{.i} => agent.{.id}.durations.{.i} = "
    "  .ends.{.i} - .starts.{.i});"
    );

  // process the hertz rates
  knowledge.evaluate (
    ".i[0->.processes)(agent.{.id}.receives.{.i} => agent.{.id}.receive_hz.{.i} = "
    "  agent.{.id}.receives.{.i} / agent.{.id}.durations.{.i});"
  );

  // process the hertz rates
  knowledge.evaluate (
    ".i[0->.processes)(agent.{.id}.total_hz += agent.{.id}.receive_hz.{.i};"
    );

  //for (size_t i = 0; i < (size_t)settings.processes; ++i)
  //{
  //  durations.set (i, end_times[i] - start_times[i]);

  //  if (durations[i] > 0)
  //  {
  //    receive_hz.set (i, receives[i] / durations[i]);
  //  }
  //  else
  //  {
  //    receive_hz.set (i, 0);
  //  }

  //  total_hz += rates[i];
  //}

#else

  for (size_t i = 0; i < (size_t)settings.processes; ++i)
  {
    durations.set (i, end_times[i] - start_times[i]);

    if (durations[i] > 0)
    {
      receive_hz.set (i, receives[i] / durations[i]);
    }
    else
    {
      receive_hz.set (i, 0);
    }

    total_hz += rates[i];
  }

#endif
  knowledge.send_modifieds ();

  // close transport before cleaning up filters
  knowledge.close_transport ();

  knowledge.print ();

  // clean up the receiver aggregate filter
  delete receiver;

  return 0;
}
