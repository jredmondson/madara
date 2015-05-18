
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <assert.h>

#include "madara/knowledge_engine/Knowledge_Base.h"
#include "madara/filters/Generic_Filters.h"
#include "madara/utility/Log_Macros.h"
#include "madara/filters/Peer_Discovery.h"
#include "madara/knowledge_engine/containers/Map.h"

namespace transport = Madara::Transport;
namespace filters = Madara::Filters;
namespace engine = Madara::Knowledge_Engine;
namespace containers = engine::Containers;
typedef  Madara::Knowledge_Record::Integer   Integer;

std::string host ("");
const std::string default_multicast ("239.255.0.1:4150");
transport::QoS_Transport_Settings settings;

Integer heart_beat = -1;
double  max_time = 10.0;

void handle_arguments (int argc, char ** argv)
{
  for (int i = 1; i < argc; ++i)
  {
    std::string arg1 (argv[i]);

    if (arg1 == "-m" || arg1 == "--multicast")
    {
      if (i + 1 < argc)
        settings.hosts[0] = argv[i + 1];

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
    else if (arg1 == "-i" || arg1 == "--id")
    {
      if (i + 1 < argc)
      {
        std::stringstream buffer (argv[i + 1]);
        buffer >> settings.id;
      }

      ++i;
    }
    else if (arg1 == "-f" || arg1 == "--logfile")
    {
      if (i + 1 < argc)
      {
        Madara::Knowledge_Engine::Knowledge_Base::log_to_file (argv[i + 1]);
      }

      ++i;
    }
    else if (arg1 == "-t" || arg1 == "--heart-beat")
    {
      if (i + 1 < argc)
      {
        std::stringstream buffer (argv[i + 1]);
        buffer >> heart_beat;
      }

      ++i;
    }
    else if (arg1 == "-l" || arg1 == "--level")
    {
      if (i + 1 < argc)
      {
        std::stringstream buffer (argv[i + 1]);
        buffer >> MADARA_debug_level;
      }

      ++i;
    }
    else if (arg1 == "-m" || arg1 == "--max-time")
    {
      if (i + 1 < argc)
      {
        std::stringstream buffer (argv[i + 1]);
        buffer >> max_time;
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
    else if (arg1 == "-r" || arg1 == "--reduced")
    {
      settings.send_reduced_message_header = true;
    }
    else
    {
      MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_DEBUG, 
        "\nProgram summary for %s:\n\n" \
        "  Test the multicast transport. Requires 2+ processes. The result of\n" \
        "  running these processes should be that each process reports\n" \
        "  var2 and var3 being set to 1.\n\n" \
        " [-o|--host hostname]     the hostname of this process (def:localhost)\n" \
        " [-m|--multicast ip:port] the multicast ip to send and listen to\n" \
        " [-d|--domain domain]     the knowledge domain to send and listen to\n" \
        " [-i|--id id]             the id of this agent (should be non-negative)\n" \
        " [-l|--level level]       the logger level (0+, higher is higher detail)\n" \
        " [-f|--logfile file]      log to a file\n" \
        " [-m|--max-time]          the maximum time to discover in seconds\n" \
        " [-r|--reduced]           use the reduced message header\n" \
        " [-t|--heart-beat]        the heart beat in seconds\n" \
        "\n",
        argv[0]));
      exit (0);
    }
  }
}

int main (int argc, char ** argv)
{
  settings.hosts.resize (1);
  settings.hosts[0] = default_multicast;
  handle_arguments (argc, argv);
  filters::Peer_Discovery peer_discovery (".peers", heart_beat);
  
  settings.type = Madara::Transport::MULTICAST;
  settings.add_receive_filter (&peer_discovery);
  settings.add_send_filter (&peer_discovery);

  Madara::Knowledge_Engine::Wait_Settings wait_settings;
  wait_settings.max_wait_time = max_time;
  wait_settings.poll_frequency = 1.0;

#ifndef _MADARA_NO_KARL_
  Madara::Knowledge_Engine::Knowledge_Base knowledge (host, settings);

  knowledge.wait ("info=1 ;> #print ('Announcing presence\n') ;> 0", wait_settings);

  containers::Map peers (".peers", knowledge);

  std::vector <std::string> keys;
  peers.keys (keys);

  std::cerr << "\nFinal peers discovered: " << keys.size () << " peers\n";
  for (size_t i = 0; i < keys.size (); ++i)
  {
    std::cerr << keys[i] << "\n";
  }

#else
  std::cout << "This test is disabled due to karl feature being disabled.\n";
#endif

  return 0;
}