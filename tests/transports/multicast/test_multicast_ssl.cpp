
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <assert.h>

#include "madara/knowledge/KnowledgeBase.h"
#include "madara/logger/GlobalLogger.h"
#include "madara/filters/ssl/AESBufferFilter.h"
#include "madara/utility/Utility.h"

namespace  utility = madara::utility;
namespace  filters = madara::filters;
namespace  logger = madara::logger;
typedef  filters::AESBufferFilter  AESBufferFilter;

std::string host ("");
const std::string default_multicast ("239.255.0.1:4150");
madara::transport::QoSTransportSettings settings;
std::string password ("");
std::string ssl_key_file ("");

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
    else if (arg1 == "-p" || arg1 == "--drop-rate")
    {
      if (i + 1 < argc)
      {
        double drop_rate;
        std::stringstream buffer (argv[i + 1]);
        buffer >> drop_rate;
        
        settings.update_drop_rate (drop_rate,
          madara::transport::PACKET_DROP_DETERMINISTIC);
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
    else if (arg1 == "-s" || arg1 == "--password")
    {
      if (i + 1 < argc)
      {
        password = argv[i + 1];
      }

      ++i;
    }
    else if (arg1 == "-k" || arg1 == "--key-file")
    {
      if (i + 1 < argc)
      {
        ssl_key_file = argv[i + 1];
      }

      ++i;
    }
    else if (arg1 == "-r" || arg1 == "--reduced")
    {
      settings.send_reduced_message_header = true;
    }
    else
    {
      madara_logger_ptr_log (logger::global_logger.get (), logger::LOG_ALWAYS,
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
        "\n",
        argv[0]);

      exit (0);
    }
  }
}



int main (int argc, char ** argv)
{
  settings.hosts.resize (1);
  settings.hosts[0] = default_multicast;
  handle_arguments (argc, argv);
  
#ifndef _MADARA_NO_KARL_
  settings.type = madara::transport::MULTICAST;

  AESBufferFilter * encryption (0);

  if (password != "")
  {
    encryption = new AESBufferFilter ();
    encryption->generate_key (password);
  }
  else if (ssl_key_file != "")
  {
    void * buffer (0);
    size_t buffer_size (0);

    utility::read_file (ssl_key_file, buffer, buffer_size);

    encryption = new AESBufferFilter (
      (unsigned char *)buffer, (int)buffer_size);

    delete[] (unsigned char *)buffer;
  }
  else
  {
    encryption = new AESBufferFilter ();
    encryption->generate_key ("testingPassword*417");
  }

  settings.add_filter (encryption);

  madara::knowledge::WaitSettings wait_settings;
  wait_settings.max_wait_time = 10;
  wait_settings.delay_sending_modifieds = false;

  madara::knowledge::KnowledgeBase knowledge (host, settings);

  knowledge.set (".id",
    (madara::knowledge::KnowledgeRecord::Integer) settings.id,
    madara::knowledge::EvalSettings::SEND);

  if (settings.id == 0)
  {
    madara::knowledge::CompiledExpression compiled = 
      knowledge.compile (
        "(var2 = 1) ;> (var1 = 0) ;> (var4 = -2.0/3) ;> var3"
      );

    knowledge.wait (compiled, wait_settings);
  }
  else
  {
    madara::knowledge::CompiledExpression compiled = 
      knowledge.compile ("!var1 && var2 => var3 = 1");

    knowledge.wait (compiled, wait_settings);
  }

  knowledge.evaluate (".updates_required = #get_clock ()",
    madara::knowledge::EvalSettings::SEND);

  knowledge.print ();
  
#else
  std::cout << "This test is disabled due to karl feature being disabled.\n";
#endif
  return 0;
}