
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <assert.h>

#include "ace/Signal.h"
#include "madara/knowledge_engine/Knowledge_Base.h"
#include "madara/utility/Log_Macros.h"

std::string host ("");
const std::string default_multicast ("239.255.0.1:4150");
Madara::Transport::Settings settings;
volatile bool terminated = 0;
int max_drones = 10;

// signal handler for someone hitting control+c
extern "C" void terminate (int)
{
  terminated = true;
}

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
    else if (arg1 == "-x" || arg1 == "--max")
    {
      if (i + 1 < argc)
      {
        std::stringstream buffer (argv[i + 1]);
        buffer >> max_drones;
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
    else
    {
      MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_DEBUG, 
        "\nProgram summary for %s:\n\n" \
        "  Test the multicast transport. Requires 2+ processes. The result of\n" \
        "  running these processes should be that each process reports\n" \
        "  shutdown being set to 1.\n\n" \
        " [-o|--host hostname]     the hostname of this process (def:localhost)\n" \
        " [-m|--multicast ip:port] the multicast ip to send and listen to\n" \
        " [-d|--domain domain]     the knowledge domain to send and listen to\n" \
        " [-x|--max max]           maximum number of drones\n" \
        " [-l|--level level]       the logger level (0+, higher is higher detail)\n" \
        "\n",
        argv[0]));
      exit (0);
    }
  }
}


/**
 * Madara function to use the thermal sensor
 **/
Madara::Knowledge_Record
  build_view (Madara::Knowledge_Engine::Function_Arguments & args,
             Madara::Knowledge_Engine::Variables & variables)
{
  return Madara::Knowledge_Record::Integer (0);
}


int main (int argc, char ** argv)
{
  // signal handler for clean exit
  ACE_Sig_Action sa ((ACE_SignalHandler) terminate, SIGINT);

  settings.hosts.resize (1);
  settings.hosts[0] = default_multicast;
  handle_arguments (argc, argv);

  Madara::Knowledge_Engine::Knowledge_Base knowledge (host, settings);
  
  Madara::Knowledge_Engine::Eval_Settings eval_settings;
  char input_buffer [1024];

  // until the user presses ctrl+c in this terminal, check for input
  while (!terminated)
  {
    std::cin.getline (input_buffer, 1024);
    knowledge.evaluate (input_buffer, eval_settings);
  }

  knowledge.print ();

  return 0;
}