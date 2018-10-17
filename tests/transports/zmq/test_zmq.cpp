
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <assert.h>

#include "madara/knowledge/KnowledgeBase.h"
#include "madara/logger/GlobalLogger.h"
#include "madara/transport/zmq/ZMQContext.h"

namespace logger = madara::logger;

std::string host("");
const std::string default_multicast("pgm://192.168.1.1;239.192.1.1:5555");
madara::transport::QoSTransportSettings settings;

void handle_arguments(int argc, char** argv)
{
  for(int i = 1; i < argc; ++i)
  {
    std::string arg1(argv[i]);

    if(arg1 == "--zmq" || arg1 == "-a")
    {
      if(i + 1 < argc)
        settings.hosts.push_back(argv[i + 1]);

      ++i;
    }
    else if(arg1 == "-o" || arg1 == "--host")
    {
      if(i + 1 < argc)
        host = argv[i + 1];

      ++i;
    }
    else if(arg1 == "-d" || arg1 == "--domain")
    {
      if(i + 1 < argc)
        settings.write_domain = argv[i + 1];

      ++i;
    }
    else if(arg1 == "-i" || arg1 == "--id")
    {
      if(i + 1 < argc)
      {
        std::stringstream buffer(argv[i + 1]);
        buffer >> settings.id;
      }

      ++i;
    }
    else if(arg1 == "-l" || arg1 == "--level")
    {
      if(i + 1 < argc)
      {
        std::stringstream buffer(argv[i + 1]);
        int level;
        buffer >> level;
        logger::global_logger->set_level(level);
      }

      ++i;
    }
    else if(arg1 == "-p" || arg1 == "--drop-rate")
    {
      if(i + 1 < argc)
      {
        double drop_rate;
        std::stringstream buffer(argv[i + 1]);
        buffer >> drop_rate;

        settings.update_drop_rate(
            drop_rate, madara::transport::PACKET_DROP_DETERMINISTIC);
      }

      ++i;
    }
    else if(arg1 == "-f" || arg1 == "--logfile")
    {
      if(i + 1 < argc)
      {
        logger::global_logger->add_file(argv[i + 1]);
      }

      ++i;
    }
    else if(arg1 == "-r" || arg1 == "--reduced")
    {
      settings.send_reduced_message_header = true;
    }
    else
    {
      madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_ALWAYS,
          "\nProgram summary for %s:\n\n"
          "  Test the multicast transport. Requires 2+ processes. The result "
          "of\n"
          "  running these processes should be that each process reports\n"
          "  var2 and var3 being set to 1.\n\n"
          " [-o|--host hostname]     the hostname of this process "
          "(def:localhost)\n"
          " [-a|--zmq prot:ip:port]  the ZMQ protocol, ip, and port to bind "
          "and\n"
          "                          connect to. The first ZMQ endpoint is "
          "the\n"
          "                          bind and everything else is subscribed "
          "to.\n"
          " [-d|--domain domain]     the knowledge domain to send and listen "
          "to\n"
          " [-i|--id id]             the id of this agent (should be "
          "non-negative)\n"
          " [-l|--level level]       the logger level (0+, higher is higher "
          "detail)\n"
          " [-f|--logfile file]      log to a file\n"
          " [-r|--reduced]           use the reduced message header\n"
          "\n",
          argv[0]);
      exit(0);
    }
  }
}

int main(int argc, char** argv)
{
  settings.type = madara::transport::ZMQ;
  settings.read_thread_hertz = 20;

  handle_arguments(argc, argv);

  if(settings.hosts.size() == 0)
  {
    settings.hosts.push_back(default_multicast);
  }

#ifndef _MADARA_NO_KARL_
  madara::knowledge::WaitSettings wait_settings;
  wait_settings.max_wait_time = 10;
  wait_settings.delay_sending_modifieds = false;

  madara::knowledge::KnowledgeBase knowledge(host, settings);

  knowledge.set(".id", (madara::knowledge::KnowledgeRecord::Integer)settings.id,
      madara::knowledge::EvalSettings::SEND);

  if(settings.id == 0)
  {
    madara::knowledge::CompiledExpression compiled = knowledge.compile(
        "(var2 = 1) ;> (var1 = 0) ;> (var4 = -2.0/3) ;> var3");

    knowledge.wait(compiled, wait_settings);

    // just to be sure the other guy ends
    knowledge.evaluate(compiled);
    knowledge.evaluate(compiled);
  }
  else
  {
    madara::knowledge::CompiledExpression compiled =
        knowledge.compile("!var1 && var2 => var3 = 1");

    knowledge.wait(compiled, wait_settings);

    // just to be sure the other guy ends
    knowledge.evaluate(compiled);
    knowledge.evaluate(compiled);
  }

  knowledge.evaluate(".updates_required = #get_clock ()");

  knowledge.print();

#else
  madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_ALWAYS,
      "This test is disabled due to karl feature being disabled.\n");
#endif

  return 0;
}