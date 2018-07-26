
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
  filters::AESBufferFilter ssl_filter;
#endif

#ifdef _USE_LZ4_
  filters::LZ4BufferFilter lz4_filter;
#endif

// time to run for (-1 is forever)
double run_time (-1);

// filename to save knowledge base as KaRL to
std::string save_location;

// filename to save knowledge base as JSON to
std::string save_json;

// filename to save knowledge base changes as checkpoint
std::string save_checkpoint;

// filename to save knowledge base as binary to
std::string save_binary;

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

// the prefix to use
std::string prefix ("agent.0");

struct Sandbox
{
  std::string id;
  std::string path;
  containers::String name;
  containers::String description;
  bool recursive;
  bool valid;
  containers::Map files;

  inline void read (containers::FlexMap & map,
    const std::string sandbox_id, knowledge::KnowledgeBase kb)
  {
    id = sandbox_id;
    path = map.to_string ();
    recursive = map["recursive"].is_true ();
    valid = filesystem::is_directory (path);

    description.set_name (prefix + ".sandbox." + id + ".description", kb);
    name.set_name (prefix + ".sandbox." + id + ".name", kb);
    files.set_name (prefix + ".sandbox." + id + ".file", kb);

    description = map["description"].to_string ();
    name = map["name"].to_string ();
  }

  template <typename T>
  inline void refresh_iterate (void)
  {
    T end, p;

    for (p = T (path); p != end; ++p)
    {
      auto file_path = p->path ();
      if (filesystem::is_regular_file (file_path))
      {
        // grab the filename
        std::string full_path = file_path.string ();

        // remove the path (we can't just use the filename () if recursive)
        std::string filename = full_path.erase (0, path.size () + 1);

        // get the size and set this in the map
        Integer size = filesystem::file_size (file_path);
        files.set (filename, size);
      }
    }
  }

  inline void refresh_files (void)
  {
    if (valid)
    {
      if (recursive)
      {
        refresh_iterate <filesystem::recursive_directory_iterator> ();
      }
      else
      {
        refresh_iterate <filesystem::directory_iterator> ();
      }
    }
  }
};

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
    else if (arg1 == "-p" || arg1 == "--prefix")
    {
      if (i + 1 < argc)
        prefix = argv[i + 1];

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
    else if (arg1 == "-sj" || arg1 == "--save-json")
    {
      if (i + 1 < argc)
        save_json = argv[i + 1];

      ++i;
    }
    else if (arg1 == "-sb" || arg1 == "--save-binary")
    {
      if (i + 1 < argc)
        save_binary = argv[i + 1];

      ++i;
    }
    else if (arg1 == "-sc" || arg1 == "--save-checkpoint")
    {
      if (i + 1 < argc)
        save_checkpoint = argv[i + 1];

      ++i;
    }
    else if (arg1 == "-scp" || arg1 == "--save-checkpoint-prefix")
    {
      if (i + 1 < argc)
      {
        save_checkpoint_settings.prefixes.push_back (argv[i + 1]);
      }

      ++i;
    }
    else if (arg1 == "-ss" || arg1 == "--save-size")
    {
      if (i + 1 < argc)
      {
        std::stringstream buffer (argv[i + 1]);
        buffer >> save_checkpoint_settings.buffer_size;
      }

      ++i;
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
        "  [-p|--prefix prefix]     prefix of this agent / service (e.g. agent.0)\n" \
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
      "Reading binary checkpoint from file %s:\n",
      file.c_str ());

    load_checkpoint_settings.filename = file;
    load_checkpoint_settings.clear_knowledge = false;
    kb.load_context (load_checkpoint_settings, silent);
  }

  load_checkpoint_settings.ignore_header_check = true;

  // allow user-readable text files to overwrite binary settings
  for (auto & file : initfiles)
  {
    madara_logger_ptr_log (logger::global_logger.get (), logger::LOG_ALWAYS,
      "Reading karl logic from file %s:\n",
      file.c_str ());

    load_checkpoint_settings.filename = file;
    load_checkpoint_settings.clear_knowledge = false;
    kb.evaluate_file (load_checkpoint_settings, silent);
  }

  // map that is accessible to all filters and threads
  containers::FlexMap sandbox_map;

  // construct sandbox map out of the loaded files
  sandbox_map.set_name (".sandbox", kb);
  knowledge::KnowledgeRules keys;
  sandbox_map.keys (keys, true);

  if (keys.size () == 0)
  {
    sandbox_map["default"].set (filesystem::current_path ().string ());
    sandbox_map.keys (keys, true);
    madara_logger_ptr_log (logger::global_logger.get (), logger::LOG_ALWAYS,
      "Using default sandbox mapping of default => '.'\n");
  }

  std::vector <Sandbox> sandboxes (keys.size ());

  for (size_t i = 0; i < keys.size (); ++i)
  {
    containers::FlexMap sandbox = sandbox_map[keys[i]];
    sandboxes[i].read (sandbox, keys[i], kb);
    sandboxes[i].refresh_files ();
  }
  
  kb.print ();

  // save as checkpoint of changes by logics and input files
  if (save_checkpoint.size () > 0)
  {
    save_checkpoint_settings.filename = save_checkpoint;
    kb.save_checkpoint (save_checkpoint_settings);
  }

  // save as karl if requested
  if (save_location.size () > 0)
  {
    save_checkpoint_settings.filename = save_location;
    kb.save_as_karl (save_checkpoint_settings);
  }

  // save as karl if requested
  if (save_json.size () > 0)
  {
    save_checkpoint_settings.filename = save_json;
    kb.save_as_json (save_checkpoint_settings);
  }

  // save as binary if requested
  if (save_binary.size () > 0)
  {
    save_checkpoint_settings.filename = save_binary;
    kb.save_context (save_checkpoint_settings);
  }

  return 0;
}

