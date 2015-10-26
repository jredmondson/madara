
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <sstream>

#include "ace/Signal.h"

#include "madara/knowledge/KnowledgeBase.h"
#include "madara/threads/Threader.h"
#include "madara/filters/EndpointClear.h"
#include "madara/knowledge/containers/Map.h"
#include "madara/knowledge/containers/Integer.h"

#include "madara/utility/Utility.h"
#include "madara/logger/GlobalLogger.h"

namespace knowledge = madara::knowledge;
namespace transport = madara::transport;
namespace utility = madara::utility;
namespace filters = madara::filters;
namespace logger = madara::logger;
namespace threads = madara::threads;

// default transport settings
std::string host ("");
const std::string default_port ("127.0.0.1:49200");
transport::QoSTransportSettings settings;

// filename to save knowledge base as KaRL to
std::string save_location;

// print debug information
bool debug (false);
bool print_knowledge (false);

// wait information
bool waiting (false);
double wait_time (0.0);

// deadline for re-registering
knowledge::KnowledgeRecord::Integer deadline (2.0);
double publish_hertz (1.0);
threads::Threader * manager (0);

// signal handler for someone hitting control+c
extern "C" void terminate (int)
{
  if (manager)
  {
    manager->terminate ("publisher");
  }
}

// handle command line arguments
void handle_arguments (int argc, char ** argv)
{
  for (int i = 1; i < argc; ++i)
  {
    std::string arg1 (argv[i]);

    if (arg1 == "-a" || arg1 == "--deadline")
    {
      if (i + 1 < argc)
      {
        std::stringstream buffer (argv[i + 1]);
        buffer >> deadline;
      }

      ++i;
    }
    else if (arg1 == "-d" || arg1 == "--domain")
    {
      if (i + 1 < argc)
        settings.domains = argv[i + 1];

      ++i;
    }
    else if (arg1 == "--debug")
    {
      debug = true;
    }
    else if (arg1 == "-f" || arg1 == "--logfile")
    {
      if (i + 1 < argc)
      {
        logger::global_logger->add_file (argv[i + 1]);
      }

      ++i;
    }
    else if (arg1 == "-k" || arg1 == "--print-knowledge")
    {
      print_knowledge = true;
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
    else if (arg1 == "-o" || arg1 == "--host")
    {
      if (i + 1 < argc)
        host = argv[i + 1];

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
          transport::PACKET_DROP_DETERMINISTIC);
      }

      ++i;
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
    else if (arg1 == "-r" || arg1 == "--reduced")
    {
      settings.send_reduced_message_header = true;
    }
    else if (arg1 == "-s" || arg1 == "--save")
    {
      if (i + 1 < argc)
        save_location = argv[i + 1];

      ++i;
    }
    else if (arg1 == "-u" || arg1 == "--udp")
    {
      if (i + 1 < argc)
      {
        settings.hosts.push_back (argv[i + 1]);
      }
      ++i;
    }
    else if (arg1 == "-v" || arg1 == "--version")
    {
      madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_ALWAYS,
        "MADARA version: %s\n",
        utility::get_version ().c_str ());
    }
    else if (arg1 == "-z" || arg1 == "--publish-hertz")
    {
      if (i + 1 < argc)
      {
        std::stringstream buffer (argv[i + 1]);
        buffer >> publish_hertz;
      }

      ++i;
    }
    else if (arg1 == "-h" || arg1 == "--help")
    {
      madara_logger_ptr_log (logger::global_logger.get (), logger::LOG_ALWAYS,
        "\nProgram summary for %s [options] [Logic]:\n\n" \
        "Evaluates KaRL logic from command line or file.\n\noptions:\n" \
        "  [-a|--deadline secs]     deadline in seconds for hearing from registered endpoints\n" \
        "  [-d|--domain domain]     the knowledge domain to send and listen to\n" \
        "  [--debug]                print all sent, received, and final knowledge\n" \
        "  [-f|--logfile file]      log to a file\n" \
        "  [-h|--help]              print help menu (i.e., this menu)\n" \
        "  [-k|--print-knowledge]   print final knowledge\n" \
        "  [-l|--level level]       the logger level (0+, higher is higher detail)\n" \
        "  [-o|--host hostname]     the hostname of this process (def:localhost)\n" \
        "  [-q|--queue-length size] size of network buffers in bytes\n" \
        "  [-r|--reduced]           use the reduced message header\n" \
        "  [-s|--save file]         save the resulting knowledge base as karl\n" \
        "  [-u|--udp ip:port]       the udp ips to send to (first is self to bind to)\n" \
        "  [-w|--wait seconds]      Wait for number of seconds before exiting\n" \
        "  [-z|--publish-hertz hz]  the rate to publish endpoint information (def: 1 hz)\n" \
        "\n",
        argv[0]);
      exit (0);
    }
  }
}


class Publisher : public threads::BaseThread
{
public:
  /**
  * Constructor
  **/
  Publisher (knowledge::KnowledgeBase & context)
    : knowledge (&context),
    endpoints ("domain." + settings.domains + ".endpoints", context),
    value ("value", context)
  {
    logger::global_logger->log (logger::LOG_ALWAYS,
      "Initializing publisher thread\n");
  }

  /**
  * Initializes thread with MADARA context
  * @param   context   context for querying current program state
  **/
  virtual void init (knowledge::KnowledgeBase & context)
  {
  }

  /**
  * Executes the main thread logic
  **/
  virtual void run (void)
  {
    ++value;

    if (debug || print_knowledge)
    {
      knowledge->print ();
    }

    knowledge->send_modifieds ();
  }

private:
  knowledge::KnowledgeBase * knowledge;
  knowledge::containers::Map endpoints;
  knowledge::containers::Integer value;
};


int main (int argc, char ** argv)
{
  // set the type of transport to be a registry server
  settings.type = transport::REGISTRY_CLIENT;

  // handle all user arguments
  handle_arguments (argc, argv);

  if (settings.hosts.size () == 0)
  {
    settings.hosts.push_back (default_port);
  }

  filters::EndpointClear endpointclear (
    "domain." + settings.domains + ".endpoints");

  settings.add_receive_filter (&endpointclear);

  knowledge::KnowledgeBase knowledge (host, settings);
  threads::Threader threader (knowledge);

  // save handle to the threader for the signal handler
  manager = &threader;

  // signal handler for clean exit
  ACE_Sig_Action sa ((ACE_SignalHandler)terminate, SIGINT);

  threader.run (publish_hertz, "publisher", new Publisher (knowledge));

  threader.wait ();

  knowledge.print ("Registry shutting down...\n");

  if (debug || print_knowledge)
  {
    knowledge.print ();
  }

  return 0;
}
