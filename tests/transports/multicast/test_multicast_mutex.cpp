
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <assert.h>

#include "madara/knowledge_engine/Knowledge_Base.h"

std::string host ("");
const std::string default_multicast ("239.255.0.1:4150");
Madara::Transport::QoS_Transport_Settings settings;
Madara::Knowledge_Record::Integer processes (2);

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
    else if (arg1 == "-l" || arg1 == "--level")
    {
      if (i + 1 < argc)
      {
        std::stringstream buffer (argv[i + 1]);
        buffer >> MADARA_debug_level;
      }

      ++i;
    }
    else if (arg1 == "-n" || arg1 == "--processes")
    {
      if (i + 1 < argc)
      {
        std::stringstream buffer (argv[i + 1]);
        buffer >> processes;
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
        Madara::Knowledge_Engine::Knowledge_Base::log_to_file (argv[i + 1]);
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
        " [-r|--reduced]           use the reduced message header\n" \
        " [-n|--processes]         the number of processes in the network\n" \
        "\n",
        argv[0]));
      exit (0);
    }
  }
}


Madara::Knowledge_Record
maekawa_receive (
  Madara::Knowledge_Engine::Function_Arguments & args,
  Madara::Knowledge_Engine::Variables & vars)
{
  if (args.size () >= Madara::Filters::TOTAL_ARGUMENTS)
  {
    if (Madara::Utility::begins_with (
      args[Madara::Filters::RECORD_NAME].to_string (), "MUTEX."))
    {
      std::vector <std::string> splitters, tokens, pivot_list;
      splitters.push_back (".");
        
      Madara::Utility::tokenizer (args[Madara::Filters::RECORD_NAME].to_string (),
        splitters, tokens, pivot_list);

      if (tokens.size () == 4)
      {
        std::string & resource_name = tokens[1];
        std::string & requester = tokens[2];
        std::string & third_arg = tokens[3];

        if (third_arg == "request")
        {

        }
        else if (third_arg == "release")
        {

        }
        else
        {

        }
      }
      return Madara::Knowledge_Record ();
    }
    else
      return args[0];
  }
  else
    return Madara::Knowledge_Record ();
}


int main (int argc, char ** argv)
{
  settings.hosts.resize (1);
  settings.hosts[0] = default_multicast;
  handle_arguments (argc, argv);
  
#ifndef _MADARA_NO_KARL_
  settings.type = Madara::Transport::MULTICAST;
  Madara::Knowledge_Engine::Wait_Settings wait_settings;
  wait_settings.max_wait_time = 10;

  Madara::Knowledge_Engine::Eval_Settings eval_settings;
  eval_settings.treat_globals_as_locals = true;

  Madara::Knowledge_Engine::Knowledge_Base knowledge (host, settings);

  knowledge.set ("SYSTEM.id",
    (Madara::Knowledge_Record::Integer) settings.id, eval_settings);
  
  knowledge.set ("SYSTEM.processes",
    (Madara::Knowledge_Record::Integer) processes, eval_settings);

  knowledge.print ();
  
#else
  std::cout << "This test is disabled due to karl feature being disabled.\n";
#endif
  return 0;
}