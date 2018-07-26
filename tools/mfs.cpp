
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <assert.h>

#include "boost/filesystem.hpp"

#include "madara/knowledge/KnowledgeBase.h"
#include "madara/threads/Threader.h"

#include "madara/utility/Utility.h"
#include "madara/filters/GenericFilters.h"
#include "madara/logger/GlobalLogger.h"

#include "madara/knowledge/containers/FlexMap.h"

#ifdef _USE_SSL_
  #include "madara/filters/ssl/AESBufferFilter.h"
#endif

#ifdef _USE_LZ4_
  #include "madara/filters/lz4/LZ4BufferFilter.h"
#endif

// convenience namespaces and typedefs
namespace knowledge = madara::knowledge;
namespace containers = knowledge::containers;
namespace transport = madara::transport;
namespace utility = madara::utility;
namespace filters = madara::filters;
namespace logger = madara::logger;
namespace threads = madara::threads;
namespace filesystem = boost::filesystem;

typedef knowledge::KnowledgeRecord::Integer  Integer;
typedef std::vector <std::string>            StringVector;

// default transport settings
std::string host ("");
const std::string default_multicast ("239.255.0.1:4150");
transport::QoSTransportSettings settings;

#ifdef _USE_SSL_
  filters::AESBufferFilter ssl_filters;
#endif

#ifdef _USE_LZ4_
  filters::LZ4BufferFilter lz4_filters;
#endif

// time to run for (-1 is forever)
double run_time (-1);

// filename to save transport settings to
std::string save_transport;
std::string save_transport_prefix;
std::string save_transport_text;
std::string load_transport_prefix;

StringVector initfiles;
StringVector initbinaries;

// checkpoint settings
knowledge::CheckpointSettings load_checkpoint_settings;
knowledge::CheckpointSettings save_checkpoint_settings;

// map that is accessible to all filters and threads
containers::FlexMap sandboxes;

struct Sandbox
{
  std::string path;
  std::string name;
  std::string description;
  bool recursive;
  bool valid;
  std::vector <std::string> files;
};

Sandbox fill_sandbox (const std::string & path,
  const std::string & name,
  const std::string & description,
  bool recursive)
{
  Sandbox sandbox;

  if (filesystem::is_directory (path))
  {
    sandbox.valid = true;
    sandbox.recursive = recursive;
    sandbox.path = path;

    madara_logger_ptr_log (logger::global_logger.get (),
      logger::LOG_ALWAYS,
      "  %s is a directory\n",
      path.c_str ());

    filesystem::directory_iterator end, p;

    for (p = filesystem::directory_iterator (path); p != end; ++p)
    {
      auto file_path = p->path ();
      std::string file = file_path.string ();
      madara_logger_ptr_log (logger::global_logger.get (),
        logger::LOG_ALWAYS,
        "    Checking %s\n",
        file.c_str ());

      if (filesystem::is_regular_file (file_path))
      {
        madara_logger_ptr_log (logger::global_logger.get (),
          logger::LOG_ALWAYS,
          "    %s is a file. Adding to file list\n",
          file.c_str ());

        sandbox.files.push_back (file);
      }
    }
  }
  else
  {
    sandbox.valid = false;
    sandbox.recursive = false;
  }

  sandbox.path = path;
  sandbox.name = name;
  sandbox.description = description;

  return sandbox;
}

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
        settings.write_domain = argv[i + 1];

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
    else if (arg1 == "-lt" || arg1 == "--load-transport")
    {
      if (i + 1 < argc)
      {
        if (load_transport_prefix == "")
          settings.load (argv[i + 1]);
        else
          settings.load (argv[i + 1], load_transport_prefix);
      }

      ++i;
    }
    else if (arg1 == "-ltp" || arg1 == "--load-transport-prefix")
    {
      if (i + 1 < argc)
      {
        load_transport_prefix = argv[i + 1];
      }

      ++i;
    }
    else if (arg1 == "-ltt" || arg1 == "--load-transport-text")
    {
      if (i + 1 < argc)
      {
        if (load_transport_prefix == "")
          settings.load_text (argv[i + 1]);
        else
          settings.load_text (argv[i + 1], load_transport_prefix);
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
    else if (arg1 == "-st" || arg1 == "--save-transport")
    {
      if (i + 1 < argc)
      {
        save_transport = argv[i + 1];
      }

      ++i;
    }
    else if (arg1 == "-stp" || arg1 == "--save-transport-prefix")
    {
      if (i + 1 < argc)
      {
        save_transport_prefix = argv[i + 1];
      }

      ++i;
    }
    else if (arg1 == "-stt" || arg1 == "--save-transport-text")
    {
      if (i + 1 < argc)
      {
        save_transport_text = argv[i + 1];
      }

      ++i;
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
      madara_logger_ptr_log (logger::global_logger.get (), logger::LOG_ALWAYS,
        "MADARA version: %s\n",
        utility::get_version ().c_str ());
    }
    else if (arg1 == "-t" || arg1 == "--time")
    {
      if (i + 1 < argc)
      {
        std::stringstream buffer (argv[i + 1]);
        buffer >> run_time;
      }
      ++i;
    }
    else if (arg1 == "--zmq" || arg1 == "--0mq")
    {
      if (i + 1 < argc)
      {
        settings.hosts.push_back (argv[i + 1]);
        settings.type = transport::ZMQ;
      }
      ++i;
    }
    else if (arg1 == "-0f" || arg1 == "--init-file")
    {
      if (i + 1 < argc)
      {
        initfiles.push_back (argv[i + 1]);
      }

      ++i;
    }
    else if (arg1 == "-0b" || arg1 == "--init-bin")
    {
      if (i + 1 < argc)
      {
        initbinaries.push_back (argv[i + 1]);

      }

      ++i;
    }
    else
    {
      madara_logger_ptr_log (logger::global_logger.get (), logger::LOG_ALWAYS,
        "\nProgram summary for %s [options] [Logic]:\n\n" \
        "Serves files up to clients within MFS-defined sandboxes.\n\noptions:\n" \
        "  [-b|--broadcast ip:port] the broadcast ip to send and listen to\n" \
        "  [-d|--domain domain]     the knowledge domain to send and listen to\n" \
        "  [-f|--logfile file]      log to a file\n" \
        "  [-h|--help]              print help menu (i.e., this menu)\n" \
        "  [-l|--level level]       the logger level (0+, higher is higher detail)\n" \
        "  [-lt|--load-transport file] a file to load transport settings from\n" \
        "  [-ltp|--load-transport-prefix prfx] prefix of saved settings\n" \
        "  [-ltt|--load-transport-text file] a text file to load transport settings from\n" \
        "  [-lz4|--lz4]             add lz4 compression filter\n" \
        "  [-m|--multicast ip:port] the multicast ip to send and listen to\n" \
        "  [-o|--host hostname]     the hostname of this process (def:localhost)\n" \
        "  [-q|--queue-length size] size of network buffers in bytes\n" \
        "  [-r|--reduced]           use the reduced message header\n" \
        "  [-s|--save file]         save the resulting knowledge base as karl\n" \
        "  [-sb|--save-binary file] save the resulting knowledge base as a\n" \
        "                           binary checkpoint\n" \
        "  [-sc|--save-checkpoint file] save any changes by logics since initial\n" \
        "                           loads as a checkpoint diff to the specified\n" \
        "                           file in an appended layer\n"
        "  [-scp|--save-checkpoint-prefix prfx]\n" \
        "                           prefix of knowledge to save in checkpoint\n" \
        "  [-sj|--save-json file]   save the resulting knowledge base as JSON\n" \
        "  [-ss|--save-size bytes]  size of buffer needed for file saves\n" \
        "  [-ssl|--ssl pass]        add an ssl filter with a password\n" \
        "  [-st|--save-transsport file] a file to save transport settings to\n" 
        "  [-stp|--save-transport-prefix prfx] prefix to save settings at\n" \
        "  [-stt|--save-transport-text file] a text file to save transport settings to\n" \
        "  [-t|--time time]         time to wait for results. Same as -w.\n" \
        "  [-u|--udp ip:port]       the udp ips to send to (first is self to bind to)\n" \
        "  [-w|--wait seconds]      Wait for number of seconds before exiting\n" \
        "  [-wy|-wp|--wait-for-periodic seconds]  Wait for number of seconds\n" \
        "                           before performing periodic evaluation\n" \
        "  [-y|--frequency hz]      frequency to perform evaluation. If negative,\n" \
        "                           only runs once. If zero, hertz is infinite.\n" \
        "                           If positive, hertz is that hertz rate.\n" \
        "  [--zmq|--0mq proto://ip:port] a ZeroMQ endpoint to connect to.\n" \
        "                           examples include tcp://127.0.0.1:30000\n" \
        "                           or any of the other endpoint types like\n" \
        "                           pgm://. For tcp, remember that the first\n" \
        "                           endpoint defined must be your own, the\n" \
        "                           one you are binding to, and all other\n" \
        "                           agent endpoints must also be defined or\n" \
        "                           no messages will ever be sent to them.\n" \
        "                           Similarly, all agents will have to have\n" \
        "                           this endpoint added to their list or\n" \
        "                           this karl agent will not see them.\n" \
        "  [-0|--init-logic logic]  logic containing initial variables (only ran once)\n" \
        "  [-0f|--init-file file]   file containing initial variables (only ran once)\n" \
        "  [-0b|--init-bin file]    file containing binary knowledge base, the result\n" \
        "                           of save_context (only ran once)\n" \
        "  [--meta-prefix prefix]   store checkpoint meta data at knowledge prefix\n" \
        "  [--use-id]               use the id of the checkpointed binary load\n" \
        "\n",
        argv[0]);
      exit (0);
    }
  }
}

int main (int argc, char ** argv)
{
  // handle all user arguments
  handle_arguments (argc, argv);

  // save transport always happens after all possible transport chagnes
  if (save_transport != "")
  {
    if (save_transport_prefix == "")
      settings.save (save_transport);
    else
      settings.save (save_transport, save_transport_prefix);
  }

  // save transport always happens after all possible transport chagnes
  if (save_transport_text != "")
  {
    if (save_transport_prefix == "")
      settings.save_text (save_transport_text);
    else
      settings.save_text (save_transport_text, save_transport_prefix);
  }

  // create a knowledge base and setup our id
  knowledge::KnowledgeBase kb (host, settings);
  madara::knowledge::EvalSettings silent (true, true, true, true, true);

  // load any binary settings
  for (auto & file : initbinaries)
  {
    madara_logger_ptr_log (logger::global_logger.get (), logger::LOG_ALWAYS,
      "\nReading binary checkpoint from file %s:\n",
      file.c_str ());

    load_checkpoint_settings.filename = file;
    kb.load_context (load_checkpoint_settings, silent);
  }

  // allow user-readable text files to overwrite binary settings
  for (auto & file : initfiles)
  {
    madara_logger_ptr_log (logger::global_logger.get (), logger::LOG_ALWAYS,
      "\nReading karl logic from file %s:\n",
      file.c_str ());

    load_checkpoint_settings.filename = file;
    kb.evaluate_file (load_checkpoint_settings, silent);
  }

  // construct sandbox map out of the loaded files
  sandboxes.set_name (".sandbox", kb);
  knowledge::KnowledgeRules keys;
  sandboxes.keys (keys, true);

  if (keys.size () == 0)
  {
    sandboxes["default"].set (filesystem::current_path ().string ());
    sandboxes.keys (keys, true);
    madara_logger_ptr_log (logger::global_logger.get (), logger::LOG_ALWAYS,
      "\nUsing default sandbox mapping of default => '.'\n");
  }

  for (auto & key : keys)
  {
    containers::FlexMap sandbox = sandboxes[key];
    std::string path = sandbox.to_string ();

    madara_logger_ptr_log (logger::global_logger.get (), logger::LOG_ALWAYS,
      "\nFound sandbox mapping %s => '%s'\n",
      key.c_str (), path.c_str ());

    if (filesystem::is_directory (path))
    {
      Sandbox result = fill_sandbox (
        path,
        sandbox["name"].to_string (),
        sandbox["description"].to_string (),
        false);

        for (auto & file : result.files)
        {
          madara_logger_ptr_log (logger::global_logger.get (),
            logger::LOG_ALWAYS,
            "\n  %s => %d byte file\n",
            file.c_str (), (int)filesystem::file_size (file));
      
        }
    }
  }
  

  return 0;
}

