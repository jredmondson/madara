
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <assert.h>

#include "madara/knowledge/KnowledgeBase.h"
#include "madara/logger/GlobalLogger.h"

namespace logger = madara::logger;


std::string host ("");
const std::string default_multicast ("239.255.0.1:4150");
madara::transport::QoSTransportSettings settings;

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
    else if (arg1 == "-r" || arg1 == "--reduced")
    {
      settings.send_reduced_message_header = true;
    }
    else
    {
      madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_ALWAYS, 
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
  settings.read_thread_hertz = 1;
  handle_arguments (argc, argv);
  
#ifndef _MADARA_NO_KARL_
  settings.type = madara::transport::MULTICAST;
  madara::knowledge::WaitSettings wait_settings;
  wait_settings.max_wait_time = 10;
  wait_settings.delay_sending_modifieds = false;

  using strvec = std::vector<std::string>;
  using strfloat = std::vector<float>;

  madara::knowledge::Any::register_type<strvec>("strvec");

  madara::knowledge::KnowledgeBase knowledge (host, settings);

  knowledge.get_context().set_clock(10);
  knowledge.set (".id", settings.id);

  auto print_clocks = [&]() {
      std::cerr << "kb clock: " << knowledge.get_context().get_clock() << std::endl;
      std::cerr << "var1 clock: " << knowledge.get("var1").clock << std::endl;
      std::cerr << "var2 clock: " << knowledge.get("var2").clock << std::endl;
      std::cerr << "var3 clock: " << knowledge.get("var3").clock << std::endl;
      std::cerr << "var4 clock: " << knowledge.get("var4").clock << std::endl;
      std::cerr << "test_any clock: " << knowledge.get("test_any").clock << std::endl;
      std::cerr << "test_any_0 clock: " << knowledge.get("test_any_0").clock << std::endl;
    };

  if (settings.id == 0)
  {
    madara::knowledge::Any::register_type<strfloat>("strfloat");

    madara::knowledge::CompiledExpression compiled =
      knowledge.compile (
        "(var2 = 1) ;> (var1 = 0) ;> (var4 = -2.0/3) ;> var3"
      );
    do {
      print_clocks();
      knowledge.set_any ("test_any_0", strvec{"e", "f", "g"},
          madara::knowledge::EvalSettings::DELAY);

      knowledge.set_any ("test_unreg_any_0", strfloat{1.0, 2.0, 3.0},
          madara::knowledge::EvalSettings::DELAY);

      //knowledge.wait (compiled, wait_settings);
      madara::utility::sleep(1);
    } while (!knowledge.evaluate (compiled, wait_settings));
    size_t asize = knowledge.get("test_any").get_any_ref<strvec>().size();
    if (asize != 4) {
      madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_ERROR,
        "Expected 4 long test_any_0, got %d.\n", asize);
    }
  }
  else
  {
    madara::knowledge::CompiledExpression compiled = 
      knowledge.compile ("!var1 && var2 => var3 = 1");
    do {
      print_clocks();

      knowledge.set_any ("test_any", strvec{"a", "b", "c", "d"},
          madara::knowledge::EvalSettings::DELAY);
      madara::utility::sleep(1);
    } while (!knowledge.evaluate (compiled, wait_settings));
    //knowledge.wait (compiled, wait_settings);
    size_t asize = knowledge.get("test_any_0").get_any_ref<strvec>().size();
    if (asize != 3) {
      madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_ERROR,
        "Expected 3 long test_any_0, got %d.\n", asize);
    }
    std::string unreg_tostr = knowledge.get("test_unreg_any_0").to_string();
    if (unreg_tostr != "\"GenericCapnObject<strfloat>\"") {
      madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_ERROR,
        "Expected test_unreg_any_0 == \"GenericCapnObject<strfloat>\", "
        "got %s.\n", unreg_tostr.c_str());
    }
    size_t unreg_size = knowledge.get("test_unreg_any_0").size();
    if (unreg_size < 10) {
      madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_ERROR,
        "Expected test_unreg_any_0 size() >= 10, got %d.\n", unreg_size);
    }
  }

  knowledge.evaluate (".updates_required = #get_clock ()",
    madara::knowledge::EvalSettings::SEND);

  print_clocks();

  knowledge.print ();
  
#else
  madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_ALWAYS,
    "This test is disabled due to karl feature being disabled.\n");
#endif
  return 0;
}
