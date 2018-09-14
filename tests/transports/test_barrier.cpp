
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <sstream>


#include "madara/knowledge/KnowledgeBase.h"

#include "madara/knowledge/containers/Barrier.h"
#include "madara/filters/GenericFilters.h"
#include "madara/logger/GlobalLogger.h"

namespace logger = madara::logger;

// shortcuts
namespace knowledge = madara::knowledge;
#ifndef _MADARA_NO_KARL_
namespace containers = knowledge::containers;
#endif // _MADARA_NO_KARL_
namespace utility = madara::utility;
namespace transport = madara::transport;
namespace filters = madara::filters;

typedef madara::knowledge::KnowledgeRecord::Integer Integer;

// default transport settings
std::string host ("");
const std::string default_multicast ("239.255.0.1:4150");
transport::QoSTransportSettings settings;

double max_wait (10.0);

Integer target (1);

bool debug (false);

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
        settings.type = transport::MULTICAST;
      }
      ++i;
    }
    else if (arg1 == "-b" || arg1 == "--broadcast")
    {
      if (i + 1 < argc)
      {
        settings.hosts.push_back (argv[i + 1]);
        settings.type = transport::BROADCAST;
      }
      ++i;
    }
    else if (arg1 == "-u" || arg1 == "--udp")
    {
      if (i + 1 < argc)
      {
        settings.hosts.push_back (argv[i + 1]);
        settings.type = transport::UDP;
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
        settings.write_domain = argv[i + 1];

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
        int level;
        std::stringstream buffer (argv[i + 1]);
        buffer >> level;
        logger::global_logger->set_level (level);
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
    else if (arg1 == "-g" || arg1 == "--debug")
    {
      debug = true;
    }
    else if (arg1 == "-r" || arg1 == "--reduced")
    {
      settings.send_reduced_message_header = true;
    }
    else if (arg1 == "-e" || arg1 == "--rebroadcasts")
    {
      if (i + 1 < argc)
      {
        int hops;
        std::stringstream buffer (argv[i + 1]);
        buffer >> hops;

        settings.set_rebroadcast_ttl (hops);
        settings.enable_participant_ttl (hops);
      }

      ++i;
    }
    else if (arg1 == "-n" || arg1 == "--no-transport")
    {
      settings.type = transport::NO_TRANSPORT;
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
    else
    {
      madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_ALWAYS,
"\nProgram summary for %s:\n\n" \
"  Tests the barrier container\n\n" \
" [-b|--broadcast ip:port] the broadcast ip to send and listen to\n" \
" [-d|--domain domain]     the knowledge domain to send and listen to\n" \
" [-e|--rebroadcasts hops] maximum number of rebroadcasts allowed\n" \
" [-f|--logfile file]      log to a file\n" \
" [-g|--debug]             enable debug mode (print sent/recv'd packets)\n" \
" [-i|--id id]             the id of this agent (should be non-negative)\n" \
" [-l|--level level]       the logger level (0+, higher is higher detail)\n" \
" [-m|--multicast ip:port] the multicast ip to send and listen to\n" \
" [-n|--no-transport]      disable transport\n" \
" [-o|--host hostname]     the hostname of this process (def:localhost)\n" \
" [-p|--processes num]     the number of participating processes\n" \
" [-q|--queue-length length] length of transport queue in bytes\n" \
" [-r|--reduced]           use the reduced message header\n" \
" [-u|--udp ip:port]       a udp ip to send to (first is self to bind to)\n" \
" [-t|--target target]     the desired distributed count total\n"\
" [-w|--max-wait time]     maximum time to wait in seconds (double format)\n"\
"\n",
        argv[0]);

      exit (0);
    }
  }
}

int main (int argc, char ** argv)
{
  // set defaults
  settings.type = transport::MULTICAST;

  // handle all user arguments
  handle_arguments (argc, argv);
  
#ifndef _MADARA_NO_KARL_
  if (settings.type != transport::NO_TRANSPORT && settings.hosts.size () == 0)
  {
    // setup default transport as multicast
    settings.hosts.push_back (default_multicast);
  }
  
  if (debug)
  {
    settings.add_receive_filter (filters::log_aggregate);
    settings.add_send_filter (filters::log_aggregate);
  }

  // create a knowledge base and setup our id
  knowledge::KnowledgeBase knowledge (host, settings);

  // create a counter
  containers::Barrier barrier ("my_barrier", knowledge,
    settings.id, settings.processes);

  knowledge.evaluate (".start_time = #get_time()",
    madara::knowledge::EvalSettings::SEND);
  knowledge.set (".target", target,
    madara::knowledge::EvalSettings::SEND);

  // increment the counter until it is at the target
  while (barrier.get_round () < target)
  {
    barrier.next ();
    while (!barrier.is_done ())
      knowledge.send_modifieds ();
  }
  
  // send another update just in case a late joiner didn't get a chance to receive all
  barrier.modify ();
  knowledge.send_modifieds ();

  knowledge.evaluate (".end_time = #get_time();"
    ".total_time = .end_time - .start_time;"
    ".total_time_in_seconds = #double(.total_time) / 1000000000",
    madara::knowledge::EvalSettings::SEND);

  knowledge.print ("Distributed barriers to {.target} took {.total_time_in_seconds}s\n");


  // print the aggregate counter to the screen
  knowledge.print ();
  
#else
  std::cout << "This test is disabled due to karl feature being disabled.\n";
#endif // _MADARA_NO_KARL_
  return 0;
}