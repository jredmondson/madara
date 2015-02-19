
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <assert.h>

#include "madara/knowledge_engine/Knowledge_Base.h"
#include "madara/utility/Log_Macros.h"

std::string host ("");
const std::string default_host1 ("127.0.0.1:43110");
const std::string default_host2 ("127.0.0.1:43111");
Madara::Transport::QoS_Transport_Settings settings;

void handle_arguments (int argc, char ** argv)
{
  for (int i = 1; i < argc; ++i)
  {
    std::string arg1 (argv[i]);

    if (arg1 == "-a" || arg1 == "--address")
    {
      if (i + 1 < argc)
        settings.hosts.push_back (argv[i + 1]);

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
    else if (arg1 == "-l" || arg1 == "--level")
    {
      if (i + 1 < argc)
      {
        std::stringstream buffer (argv[i + 1]);
        int debug_level;
        buffer >> debug_level;
        Madara::Knowledge_Engine::Knowledge_Base::log_level (debug_level);
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
        "  Test the UDP transport. Requires 2+ processes. The result of\n" \
        "  running these processes should be that each process reports\n" \
        "  shutdown being set to 1.\n\n" \
        " [-o|--host hostname]     the hostname of this process (def:localhost)\n" \
        " [-a|--address ip:port]   add a udp ip:port to send to (first add is self ip:port)\n" \
        " [-d|--domain domain]     the knowledge domain to send and listen to\n" \
        " [-i|--id id]             the id of this agent (should be non-negative)\n" \
        " [-f|--logfile file]      log to a file\n" \
        " [-l|--level level]       the logger level (0+, higher is higher detail)\n" \
        " [-r|--reduced]           use the reduced message header\n" \
        "\n",
        argv[0]));
      exit (0);
    }
  }
}


int main (int argc, char ** argv)
{
  handle_arguments (argc, argv);
  
#ifndef _MADARA_NO_KARL_
  // if the user wants us to do defaults for either host1 or 2
  if (settings.hosts.size () < 2)
  {
    size_t cur_size = settings.hosts.size ();

    settings.hosts.resize (2);
    
    // if we are id 0, use host1 as our ip
    if (settings.id == 0)
    {
      if (cur_size == 0)
      {
        settings.hosts[0] = default_host1;
      }

      // and host2 as the other ip
      settings.hosts[1] = default_host2;
    }
    else
    {
      // if we are id 1, use host2 as our ip
      if (cur_size == 0)
      {
        settings.hosts[0] = default_host2;
      }

      // and host1 as the other ip
      settings.hosts[1] = default_host1;
    }
  }

  settings.type = Madara::Transport::UDP;
  Madara::Knowledge_Engine::Wait_Settings wait_settings;
  wait_settings.max_wait_time = 10.0;

  if (settings.id == 0)
    settings.on_data_received_logic = "out_of_resources => emergency = 1; emergency => shutdown = 1";
  else  
    settings.on_data_received_logic = "heavy_processes > 0 => out_of_resources = 1; emergency => shutdown = 1";

  Madara::Knowledge_Engine::Knowledge_Base knowledge (host, settings);

  knowledge.set (".id", (Madara::Knowledge_Record::Integer) settings.id);
  
  if (settings.id == 0)
  {
    Madara::Knowledge_Engine::Compiled_Expression compiled = 
      knowledge.compile ("heavy_processes = 1 ;> shutdown");

    knowledge.wait (compiled, wait_settings);
  }
  else
  {
    Madara::Knowledge_Engine::Compiled_Expression compiled = 
      knowledge.compile ("shutdown");

    knowledge.wait (compiled, wait_settings);
  }

  knowledge.print ();
  
#else
  std::cout << "This test is disabled due to karl feature being disabled.\n";
#endif
  return 0;
}