
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <assert.h>

#include "ace/High_Res_Timer.h"
#include "ace/OS_NS_Thread.h"
#include "ace/Sched_Params.h"
#include "ace/Guard_T.h"
#include "ace/Recursive_Thread_Mutex.h"

#include "madara/knowledge_engine/Knowledge_Base.h"

#include "madara/utility/Utility.h"
#include "madara/filters/Generic_Filters.h"
#include "madara/logger/Global_Logger.h"

namespace engine = Madara::Knowledge_Engine;
namespace transport = Madara::Transport;
namespace utility = Madara::Utility;
namespace filters = Madara::Filters;
namespace logger = Madara::Logger;

typedef  std::vector <std::string>  String_Vector;

// default transport settings
std::string host ("");
const std::string default_multicast ("239.255.0.1:4150");
transport::QoS_Transport_Settings settings;

// initial logic is empty
std::string logic;

// list of filenames
String_Vector filenames;

// print debug information
bool debug (false);

// wait information
bool waiting (false);
double wait_time (0.0);

// handle command line arguments
void handle_arguments (int argc, char ** argv)
{
  for (int i = 1; i < argc; ++i)
  {
    std::string arg1 (argv[i]);

    if (arg1 == "-b" || arg1 == "--broadcast")
    {
      if (i + 1 < argc)
      {
        settings.hosts.push_back (argv[i + 1]);
        settings.type = transport::BROADCAST;
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
    else if (arg1 == "-h" || arg1 == "--help")
    {
      madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_ALWAYS,
        "\nProgram summary for %s [options] [Logic]:\n\n" \
        "Evaluates KaRL logic from command line or file.\n\noptions:\n" \
        "  [-a|--no-latency]        do not test for latency (throughput only)\n" \
        "  [-b|--broadcast ip:port] the broadcast ip to send and listen to\n" \
        "  [-d|--domain domain]     the knowledge domain to send and listen to\n" \
        "  [--debug]                print all sent, received, and final knowledge\n" \
        "  [-f|--logfile file]      log to a file\n" \
        "  [-h|--help]              print help menu (i.e., this menu)\n" \
        "  [-i|--input file]        file containing MADARA logic to evaluate\n" \
        "  [-l|--level level]       the logger level (0+, higher is higher detail)\n" \
        "  [-m|--multicast ip:port] the multicast ip to send and listen to\n" \
        "  [-o|--host hostname]     the hostname of this process (def:localhost)\n" \
        "  [-q|--queue-length size] size of network buffers in bytes\n" \
        "  [-r|--reduced]           use the reduced message header\n" \
        "  [-s|--size size]         size of data packet to send in bytes\n" \
        "  [-t|--time time]         time to burst messages for throughput test\n" \
        "  [-u|--udp ip:port]       the udp ips to send to (first is self to bind to)\n" \
        "  [-w|--wait seconds]      After evaluating all logics, wait for seconds\n" \
        "\n",
        argv[0]);
      exit (0);
    }
    else if (arg1 == "-i" || arg1 == "--input")
    {
      if (i + 1 < argc)
      {
        std::string filename;
        std::stringstream buffer (argv[i + 1]);
        buffer >> filename;

        if (debug)
        {
          madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_ALWAYS,
            "\nReading logic from file %s:\n\n" \
            "\n",
            filename.c_str ());
        }

        filenames.push_back (filename);
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
    else if (arg1 == "-m" || arg1 == "--multicast")
    {
      if (i + 1 < argc)
      {
        settings.hosts.push_back (argv[i + 1]);
        settings.type = transport::MULTICAST;
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
    else if (arg1 == "-u" || arg1 == "--udp")
    {
      if (i + 1 < argc)
      {
        settings.hosts.push_back (argv[i + 1]);
        settings.type = transport::UDP;
      }
      ++i;
    }
    else if (arg1 == "-v" || arg1 == "--version")
    {
      madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_ALWAYS,
        "MADARA version: %s\n",
        utility::get_version ().c_str ());
    }
    else if (arg1 == "-w" || arg1 == "--wait")
    {
      if (i + 1 < argc)
      {
        waiting = true;
        std::stringstream buffer (argv[i + 1]);
        buffer >> wait_time;
      }
      ++i;
    }
    else if (logic == "")
    {
      logic = arg1;
    }
  }
}

int main (int argc, char ** argv)
{
  // handle all user arguments
  handle_arguments (argc, argv);

  if (debug)
  {
    settings.add_send_filter (filters::log_aggregate);
    settings.add_receive_filter (filters::log_aggregate);
  }

  // create a knowledge base and setup our id
  engine::Knowledge_Base knowledge (host, settings);

  for (String_Vector::const_iterator i = filenames.begin ();
       i != filenames.end (); ++i)
  {
    if (*i != "")
    {
      std::string file_logic = utility::file_to_string (*i);
      if (file_logic != "")
      {
#ifndef _MADARA_NO_KARL_
        knowledge.evaluate (file_logic);
#endif // _MADARA_NO_KARL_
      }
    }
  }
  
  // evaluate any logic from the command line last
  if (logic != "")
  {
#ifndef _MADARA_NO_KARL_
    knowledge.evaluate (logic);
#endif // _MADARA_NO_KARL_
  }

  // if user requests to wait, do so before the debug print
  if (waiting)
  {
    utility::sleep (wait_time);
  }

  // if the user requests debugging information, print final knowledge
  if (debug)
  {
    knowledge.print_knowledge ();
  }

  return 0;
}
