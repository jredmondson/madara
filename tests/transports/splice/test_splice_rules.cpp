
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <assert.h>

#include "madara/knowledge/KnowledgeBase.h"
#include "madara/logger/GlobalLogger.h"

namespace knowledge = madara::knowledge;
namespace transport = madara::transport;
namespace logger = madara::logger;

typedef knowledge::KnowledgeRecord::Integer Integer;

std::string host("");
madara::transport::QoSTransportSettings settings;

void handle_arguments(int argc, char** argv)
{
  for(int i = 1; i < argc; ++i)
  {
    std::string arg1(argv[i]);

    if(arg1 == "-o" || arg1 == "--host")
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
    else if(arg1 == "-f" || arg1 == "--logfile")
    {
      if(i + 1 < argc)
      {
        logger::global_logger->add_file(argv[i + 1]);
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
    else
    {
      madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_ALWAYS,
          "\nProgram summary for %s:\n\n"
          "  Test the Splice DDS transport. Requires 2+ processes. The result "
          "of\n"
          "  running these processes should be that each process reports\n"
          "  shutdown being set to 1.\n\n"
          " [-o|--host hostname]     the hostname of this process "
          "(def:localhost)\n"
          " [-d|--domain domain]     the knowledge domain to send and listen "
          "to\n"
          " [-i|--id id]             the id of this agent (should be "
          "non-negative)\n"
          " [-l|--level level]       the logger level (0+, higher is higher "
          "detail)\n"
          " [-f|--logfile file]      log to a file\n"
          "\n",
          argv[0]);
      exit(0);
    }
  }
}

int main(int argc, char** argv)
{
  handle_arguments(argc, argv);

  settings.type = madara::transport::SPLICE;
  settings.reliability = madara::transport::RELIABLE;
  madara::knowledge::WaitSettings wait_settings;
  wait_settings.max_wait_time = 10;
  wait_settings.delay_sending_modifieds = false;

  if(settings.id == 0)
    settings.on_data_received_logic =
        "out_of_resources => emergency = 1; emergency => shutdown = 1";
  else
    settings.on_data_received_logic = "heavy_processes > 0 => out_of_resources "
                                      "= 1; emergency => shutdown = 1";

  madara::knowledge::KnowledgeBase knowledge(host, settings);

  knowledge.set(
      ".id", (Integer)settings.id, madara::knowledge::EvalSettings::SEND);

  if(settings.id == 0)
  {
    madara::knowledge::CompiledExpression compiled =
        knowledge.compile("heavy_processes = 1 ;> shutdown");

    knowledge.wait(compiled, wait_settings);
  }
  else
  {
    madara::knowledge::CompiledExpression compiled =
        knowledge.compile("shutdown");

    knowledge.wait(compiled, wait_settings);
  }

  knowledge.print_knowledge();

  return 0;
}