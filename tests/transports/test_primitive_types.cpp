
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <assert.h>

#include "madara/knowledge/KnowledgeBase.h"


#include "ace/Signal.h"
#include "ace/Log_Msg.h"
#include "ace/Get_Opt.h"
#include "ace/Signal.h"
#include "ace/Sched_Params.h"
#include "madara/logger/GlobalLogger.h"

namespace logger = madara::logger;

std::string host ("");
const std::string default_multicast ("239.255.0.1:4150");
madara::transport::Settings settings;
volatile bool terminated = 0;

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
        int level;
        buffer >> level;
        logger::global_logger->set_level (level);
      }

      ++i;
    }
    else
    {
      madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_ALWAYS, 
        "\nProgram summary for %s:\n\n" \
        "  Test strings, integers, and doubles over a multicast transport.\n" \
        "  Priority and training.completion will increment with each update.\n" \
        "  Press Control+C to exit.\n\n" \
        " [-o|--host hostname]     the hostname of this process (def:localhost)\n" \
        " [-m|--multicast ip:port] the multicast ip to send and listen to\n" \
        " [-d|--domain domain]     the knowledge domain to send and listen to\n" \
        " [-i|--id id]             the id of this agent (should be non-negative)\n" \
        " [-l|--level level]       the logger level (0+, higher is higher detail)\n" \
        "\n",
        argv[0]);
      exit (0);
    }
  }
}

// signal handler for someone hitting control+c
extern "C" void terminate (int)
{
  terminated = true;
}



int ACE_TMAIN (int argc, char ** argv)
{
  settings.hosts.resize (1);
  settings.hosts[0] = default_multicast;
  handle_arguments (argc, argv);
  
#ifndef _MADARA_NO_KARL_
  settings.type = madara::transport::MULTICAST;
  madara::knowledge::WaitSettings wait_settings;
  wait_settings.max_wait_time = 10.0;
  wait_settings.post_print_statement =
    "{update}: name == {name}, " \
    "position = {position}, " \
    "priority = {priority}, " \
    "department = {department}, " \
    "training.completion = {training.completion}\n";
  
  // signal handler for clean exit
  ACE_Sig_Action sa ((ACE_SignalHandler) terminate, SIGINT);

  madara::knowledge::KnowledgeBase knowledge (host, settings);

  knowledge.set (".id", (madara::knowledge::KnowledgeRecord::Integer) settings.id);
  
  madara::knowledge::CompiledExpression compiled;

  
  if (settings.id == 0)
  {
    compiled = 
      knowledge.compile (
        "++update; name='Jeff Stoley'; position='Operator'; " \
        "++priority; department='IT'; training.completion+=0.1");

  }
  else
  {
    compiled = 
      knowledge.compile ("name == 'Jeff Stoley' && position == 'Operator' " \
                      "&& priority >= 1 && department == 'IT' " \
                      "&& training.completion >= 0.1");
  }

  // termination is done via signalling from the user (Control+C)
  while (!terminated)
  {
    knowledge.wait (compiled, wait_settings);

    ACE_OS::sleep (1);
  }

  knowledge.print ();
  
#else
  madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_ALWAYS,
    "This test is disabled due to karl feature being disabled.\n");
#endif
  return 0;
}