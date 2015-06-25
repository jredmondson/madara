
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


#include "madara/knowledge_engine/containers/Integer.h"
#include "madara/utility/Utility.h"
#include "madara/logger/Global_Logger.h"

namespace logger = Madara::Logger;
namespace utility = Madara::Utility;

// useful namespace aliases and typedefs

namespace engine = Madara::Knowledge_Engine;
namespace containers = engine::Containers;
namespace transport = Madara::Transport;

typedef Madara::Knowledge_Record::Integer Integer;

// default transport settings
std::string host ("");
const std::string default_multicast ("239.255.0.1:4150");
transport::QoS_Transport_Settings settings;

// number of sent and received messages

containers::Integer num_sent;
containers::Integer num_received;
containers::Integer finished;
containers::Integer payload_size;

// target sends
Integer target (1000000);

bool started = false;

// amount of time in seconds to burst payloads
double publish_time = 10.0;

// payload size to burst
unsigned int data_size = 1000;

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
    else if (arg1 == "-t" || arg1 == "--target")
    {
      if (i + 1 < argc)
      {
        std::stringstream buffer (argv[i + 1]);
        buffer >> target;
      }

      ++i;
    }
    else if (arg1 == "-z" || arg1 == "--read-hertz")
    {
      if (i + 1 < argc)
      {
        std::stringstream buffer (argv[i + 1]);
        buffer >> settings.read_thread_hertz;
      }

      ++i;
    }
    else
    {
      madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_ALWAYS, 
        "\nProgram summary for %s:\n\n" \
        "  Profiles a network transport. Requires 2 processes. The result of\n" \
        "  running these processes should be that each process reports\n" \
        "  latency and throughput statistics.\n\n" \
        " [-b|--broadcast ip:port] the broadcast ip to send and listen to\n" \
        " [-d|--domain domain]     the knowledge domain to send and listen to\n" \
        " [-e|--threads threads]   number of read threads\n" \
        " [-i|--id id]             the id of this agent (should be non-negative)\n" \
        " [-l|--level level]       the logger level (0+, higher is higher detail)\n" \
        " [-m|--multicast ip:port] the multicast ip to send and listen to\n" \
        " [-t|--target target]     number of payloads to send/target\n" \
        " [-o|--host hostname]     the hostname of this process (def:localhost)\n" \
        " [-f|--logfile file]      log to a file\n" \
        " [-r|--reduced]           use the reduced message header\n" \
        " [-s|--size size]         size of data packet to send in bytes\n" \
        " [-t|--time time]         time to burst messages for throughput test\n" \
        " [-u|--udp ip:port]       the udp ips to send to (first is self to bind to)\n" \
        " [-z|--read-hertz hertz]  read thread hertz speed\n" \
        "\n",
        argv[0]);
      exit (0);
    }
  }
}


// filter for counting received packets
Madara::Knowledge_Record
count_received (
  Madara::Knowledge_Engine::Function_Arguments & args,
  Madara::Knowledge_Engine::Variables & vars)
{
  if (args[0].is_binary_file_type ())
  {
    ++num_received;

    if (!started)
    {
      payload_size = args[0].size ();
      vars.set (".start_time",
        Madara::Knowledge_Record::Integer (Madara::Utility::get_time ()));
      started = true;
    }

      vars.set (".end_time",
        Madara::Knowledge_Record::Integer (Madara::Utility::get_time ()));
  }

  return args[0];
}

int main (int argc, char ** argv)
{
  settings.type = transport::MULTICAST;
  settings.queue_length = data_size * 1000;

  // unlimited hertz
  settings.read_thread_hertz = 0.0;
  settings.read_threads = 1;

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
  
  if (settings.id != 0)
  {
    // count the number of received packets
    settings.add_receive_filter (
      Madara::Knowledge_Record::ALL_FILE_TYPES,
      count_received);
  }

  // create a knowledge base and setup our id
  engine::Knowledge_Base knowledge (host, settings);
  
  // setup wait settings to wait for 2 * publish_time seconds
  engine::Wait_Settings wait_settings;
  wait_settings.max_wait_time = publish_time * 2;

  Madara::Knowledge_Engine::Variable_Reference file =
    knowledge.get_ref (".ref_file");
  
  // get variable references for real-time, constant-time operations
  num_received.set_name (".num_received", knowledge);
  num_sent.set_name (".num_sent", knowledge);
  finished.set_name ("finished", knowledge);
  payload_size.set_name (".payload_size", knowledge);
  
  if (settings.id == 0)
  {
    // publisher

    // set up the payload
    unsigned char * ref_file = new unsigned char[data_size];
    knowledge.set_file (".ref_file", ref_file, data_size);
    
    knowledge.print ("Running throughput test...");
    
    Integer i = 0;
    payload_size = data_size;

    knowledge.set (".start_time",
      Madara::Knowledge_Record::Integer (Madara::Utility::get_time ()));

    // publish the payload repeatedly until max time has passed
    for (i = 0; i < target; ++i)
    {
      knowledge.mark_modified (file);
      knowledge.send_modifieds ();
    }

    num_sent = i;
    
    knowledge.set (".end_time",
      Madara::Knowledge_Record::Integer (Madara::Utility::get_time ()));

    finished = 1;
    
    knowledge.set (".messages", knowledge.get (".num_sent").to_integer ());
  }
  else
  {

#ifndef _MADARA_NO_KARL_
    knowledge.wait ("finished", wait_settings);
#else
    utility::wait_true (knowledge, "finished", wait_settings);

#endif // _MADARA_NO_KARL_

    
    knowledge.set (".messages", knowledge.get (".num_received").to_integer ());
  }
  
#ifndef _MADARA_NO_KARL_
  knowledge.evaluate (".bytes = .messages * .payload_size");
  knowledge.evaluate (".elapsed_time = .end_time - .start_time");
  knowledge.evaluate (
    ".elapsed_time *= 0.000000001 ;"
    ".elapsed_time > 0.0 => .bytes_per_sec = .bytes / .elapsed_time");
  knowledge.evaluate (
    ".elapsed_time > 0.0 => .throughput = .messages / .elapsed_time");
#else
  Integer messages = knowledge.get (".messages").to_integer ();
  Integer bytes = messages * payload_size.to_integer (); 
  double elapsed_time = knowledge.get ("elapsed_time").to_double ();
  elapsed_time *= 0.000000001;

  if (elapsed_time > 0)
  {
    knowledge.set (".bytes_per_sec", bytes / elapsed_time);

    Integer num_received = messages;
    num_received /= elapsed_time;
    knowledge.set ("throughput", num_received);
  }


#endif // _MADARA_NO_KARL_

  knowledge.print ("\nNetwork profile for transport\n");
  knowledge.print ("  Time taken:  {.elapsed_time}s\n");
  knowledge.print ("  Messages sent/received:  {.messages}\n");
  knowledge.print ("  Bytes sent/received:  {.bytes}B\n");
  knowledge.print ("Throughput ({.elapsed_time}s blast test)\n");
  knowledge.print ("  Messages:  {.throughput} payloads/s\n");
  knowledge.print ("  Data rate: {.bytes_per_sec} B/s\n");

  return 0;
}