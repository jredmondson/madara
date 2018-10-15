
#include <string>
#include <vector>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <sstream>
#include <assert.h>
#include <fstream>

#include "madara/knowledge/CheckpointPlayer.h"
#include "madara/knowledge/CheckpointStreamer.h"
#include "madara/knowledge/KnowledgeBase.h"
#include "madara/knowledge/AnyRegistry.h"
#include "madara/threads/Threader.h"

#include "madara/utility/Utility.h"
#include "madara/utility/NamedVectorCombinator.h"
#include "madara/filters/GenericFilters.h"
#include "madara/filters/PrefixPrint.h"
#include "madara/logger/GlobalLogger.h"

#ifdef _USE_SSL_
#include "madara/filters/ssl/AESBufferFilter.h"
#endif

#ifdef _USE_LZ4_
#include "madara/filters/lz4/LZ4BufferFilter.h"
#endif

#include "capnp/schema-parser.h"
#include "capnp/schema.h"
#include <boost/algorithm/string.hpp>

// convenience namespaces and typedefs
namespace knowledge = madara::knowledge;
namespace transport = madara::transport;
namespace utility = madara::utility;
namespace filters = madara::filters;
namespace logger = madara::logger;
namespace threads = madara::threads;

typedef knowledge::KnowledgeRecord::Integer Integer;
typedef std::vector<std::string> StringVector;

// default KaRL config file
std::string default_karl_config =
    madara::utility::expand_envs("$(HOME)/.madara/karl.cfg");

// default transport settings
std::string host("");
const std::string default_multicast("239.255.0.1:4150");
transport::QoSTransportSettings settings;

// checkpoint settings
knowledge::CheckpointSettings load_checkpoint_settings;
knowledge::CheckpointSettings save_checkpoint_settings;

// initial logic is empty
std::string logic;

// filename to save knowledge base as KaRL to
std::string save_location;

// filename to save knowledge base as JSON to
std::string save_json;

// filename to save knowledge base changes as checkpoint
std::string save_checkpoint;

// filename to save knowledge base as binary to
std::string save_binary;

// filename to load a stream from
std::string stream_from;

// filename to save a stream to
std::string stream_to;

// knowledge base var name to save checkpoint timestamp to
std::string meta_prefix;

// filename to save transport settings to
std::string save_transport;
std::string save_transport_prefix;
std::string save_transport_text;
std::string load_transport_prefix;

// list of filenames
StringVector filenames;
StringVector initfiles;
StringVector initlogics;
StringVector print_prefixes;
std::string initbinaries;

// print debug information
bool debug(false);
bool print_knowledge(false);
bool print_knowledge_frequency(false);
bool print_updates(false);
bool after_wait(false);
bool check_result(false);
bool use_id(false);
bool print_stats(false);
bool print_stats_periodic(false);

// wait information
bool waiting(false), waiting_for_periodic(false);
double wait_time(0.0);
double wait_for_periodic(0.0);
double frequency(-1.0);

#ifdef _USE_SSL_
filters::AESBufferFilter ssl_transport_filter;
std::vector<filters::AESBufferFilter> ssl_filters;
#endif

#ifdef _USE_LZ4_
filters::LZ4BufferFilter lz4_transport_filter;
std::vector<filters::LZ4BufferFilter> lz4_filters;
#endif

// config file recursion limit
const size_t default_recursion_limit = 10;

// recursively loads a config file(s) and processe with handle_arguments
bool load_config_file(
    std::string full_path, size_t recursion_limit = default_recursion_limit);

/**
 * Class for keeping track of originator updates on a variable
 **/
class OriginatorStats
{
public:
  OriginatorStats() : name(""), first(0), last(0), updates(0), bytes(0) {}

  void print(void)
  {
    uint64_t total_ns = last - first;
    uint64_t total_s = total_ns / 1000000000;
    double hertz = (double)updates / total_s;
    double kbs = (double)bytes / 1000 / total_s;

    std::cerr << std::fixed << std::setprecision(2);
    std::cerr << "from: " << name << ": " << updates << " updates ";
    std::cerr << " (@" << hertz << "hz) (@" << kbs << "KB/s)\n";
  }

  std::string name;
  uint64_t first;
  uint64_t last;
  uint64_t updates;
  uint64_t bytes;
};

/// convenience typedef for originator updates
typedef std::map<std::string, OriginatorStats> OriginatorUpdates;

/**
 * Class for keeping track of variable updates
 **/
class VariableStats
{
public:
  /**
   * prints variable stats including originator stats for the variable
   **/
  void print(void)
  {
    std::cerr << name << ":\n";
    for(auto update : updates)
    {
      std::cerr << "  ";
      update.second.print();
    }
  }

  std::string name;
  OriginatorUpdates updates;
};

/// convenience typedef for variable name to stats mapping
typedef std::map<std::string, VariableStats> VariableUpdates;

/// specialized debug filter for tracking originator updates
class StatsFilter : public madara::filters::AggregateFilter
{
public:
  /// tracks updates to the variables by originator
  VariableUpdates var_updates;

  /// tracks originator updates
  OriginatorUpdates orig_updates;

  /**
   * prints the originator and variable stats
   **/
  void print(void)
  {
    std::cerr << "****Originator Stats****\n";
    for(auto origs : orig_updates)
    {
      origs.second.print();
    }

    std::cerr << "****Variable Stats****\n";
    for(auto vars : var_updates)
    {
      bool prefix_match = print_prefixes.size() == 0;

      if(!prefix_match)
      {
        for(auto prefix : print_prefixes)
        {
          if(utility::begins_with(vars.first, prefix))
          {
            prefix_match = true;
            break;
          }
        }
      }

      if(prefix_match)
      {
        vars.second.print();
      }
    }
  }

  /**
   * filter called on receive
   * @param  records             the updates
   * @param  transport_context   the context of the transport
   **/
  virtual void filter(knowledge::KnowledgeMap& records,
      const transport::TransportContext& transport_context,
      knowledge::Variables&)
  {
    std::string originator = transport_context.get_originator();
    OriginatorStats& orig_stats = orig_updates[originator];

    // update the high level originator stats
    ++orig_stats.updates;
    orig_stats.last = transport_context.get_current_time();

    if(orig_stats.first == 0)
    {
      orig_stats.name = originator;
      orig_stats.first = orig_stats.last;
    }

    // iterate through each record
    for(auto record : records)
    {
      VariableStats& var_stats = var_updates[record.first];

      if(var_stats.name == "")
      {
        var_stats.name = record.first;
      }

      OriginatorStats& var_orig_stats = var_stats.updates[originator];

      // update each record's originator stats
      ++var_orig_stats.updates;
      var_orig_stats.last = orig_stats.last;
      if(var_orig_stats.first == 0)
      {
        var_orig_stats.name = originator;
        var_orig_stats.first = var_orig_stats.last;
      }

      // calculate bytes
      size_t bytes = record.second.size();
      if(record.second.is_integer_type())
      {
        bytes *= sizeof(int64_t);
      }
      else if(record.second.is_double_type())
      {
        bytes *= sizeof(double);
      }

      // update bytes sent by originator
      var_orig_stats.bytes += record.second.size();
      orig_stats.bytes += record.second.size();
    }
  }
};

// originator debug filter to add if requested
StatsFilter stats_filter;

// Capnp types and globals
kj::Vector<kj::StringPtr> capnp_import_dirs;
std::vector<std::string> capnp_msg;
std::vector<std::string> capnp_type;
bool capnp_msg_type_param_flag = false;
bool capnp_import_dirs_flag = false;

// handle command line arguments
void handle_arguments(int argc, const char** argv, size_t recursion_limit = 10)
{
  for(int i = 1; i < argc; ++i)
  {
    std::string arg1(argv[i]);

    if(arg1 == "-a" || arg1 == "--after-wait")
    {
      after_wait = true;
    }
    else if(arg1 == "-b" || arg1 == "--broadcast")
    {
      if(i + 1 < argc)
      {
        settings.hosts.push_back(argv[i + 1]);
        settings.type = transport::BROADCAST;
      }
      ++i;
    }
    if(arg1 == "-c" || arg1 == "--check-result")
    {
      check_result = true;
    }
    else if(arg1 == "-d" || arg1 == "--domain")
    {
      if(i + 1 < argc)
        settings.write_domain = argv[i + 1];

      ++i;
    }
    if(arg1 == "-cf" || arg1 == "--config-file")
    {
      madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_TRACE,
          "Found user karl config file flag, param: %s\n", argv[i + 1]);
      if(recursion_limit > 0)
      {
        load_config_file(argv[i + 1], recursion_limit);
      }
      else
      {
        madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_ERROR,
            "Config file recursion limit exceeded with %s\n", argv[i + 1]);
      }
      ++i;
    }
    else if(arg1 == "--debug")
    {
      debug = true;
    }
    else if(arg1 == "-f" || arg1 == "--logfile")
    {
      if(i + 1 < argc)
      {
        logger::global_logger->add_file(argv[i + 1]);
      }

      ++i;
    }
    else if (arg1 == "-fql" || arg1 == "--fragment-queue-length")
    {
      if (i + 1 < argc)
      {
        std::stringstream buffer(argv[i + 1]);
        buffer >> settings.fragment_queue_length;
      }

      ++i;
    }
    else if(arg1 == "-h" || arg1 == "--help")
    {
      madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_ALWAYS,
          "\nProgram summary for %s [options] [Logic]:\n\n"
          "Evaluates KaRL logic from command line or file.\n\noptions:\n"
          "  [-a|--after-wait]        Evaluate after wait, rather than before "
          "wait\n"
          "  [-b|--broadcast ip:port] the broadcast ip to send and listen to\n"
          "  [-c|--check-result]      check result of eval. If not zero, then "
          "terminate\n"
          "  [-cf|--config-file]      Config file full path, file contains "
          "karl cmd line\n"
          "                           flags, also uses default config file\n"
          "                           $(HOME)/.madara/karl.cfg\n"
          "  [-d|--domain domain]     the knowledge domain to send and listen "
          "to\n"
          "  [--debug]                print all sent, received, and final "
          "knowledge\n"
          "  [-f|--logfile file]      log to a file\n"
          "  [-fql--fragment-queue-length num] the depth of the fragment lst\n"
          "                           more depth is better for big packets\n"
          "  [-h|--help]              print help menu (i.e., this menu)\n"
          "  [-i|--input file]        file containing MADARA logic to "
          "evaluate\n"
          "  [-k|--print-knowledge]   print final knowledge\n"
          "  [-kp|--print-prefix pfx] filter prints by prefix. Can be "
          "multiple.\n"
          "  [-kpl|--print-prefix-list file] filter prints by prefix. The "
          "file is a \n"
          "                           new-line delimited prefix list that "
          "can be read\n"
          "                           by NamedVectorCombinator.\n"
          "  [-ky]                    print knowledge after frequent "
          "evaluations\n"
          "  [-l|--level level]       the logger level (0+, higher is higher "
          "detail)\n"
          "  [-lcp|--load-checkpoint-prefix prfx]\n"
          "                           prefix of knowledge to load from "
          "checkpoint\n"
          "  [-lcpl|--load-prefix-list file] filter loads by prefix. The "
          "file is a \n"
          "                           new-line delimited prefix list that "
          "can be read\n"
          "                           by NamedVectorCombinator.\n"
          "  [-ls|--load-size bytes]  size of buffer needed for file load\n"
          "  [-lt|--load-transport file] a file to load transport settings "
          "from\n"
          "  [-ltp|--load-transport-prefix prfx] prefix of saved settings\n"
          "  [-ltt|--load-transport-text file] a text file to load transport "
          "settings from\n"
          "  [-lz4|--lz4]             compress with LZ4 over network\n"
          "  [-m|--multicast ip:port] the multicast ip to send and listen to\n"
          "  [-n|--capnp tag:msg_type] register tag with given message schema. "
          "See also -nf and -ni.\n"
          "  [-nf|--capnp-file]       load capnp file. Must appear after all "
          "-n and -ni.\n"
          "  [-ni|--capnp-import ]    add directory to capnp directory "
          "imports. Must appear before all -n and -nf.\n"
          "  [-o|--host hostname]     the hostname of this process "
          "(def:localhost)\n"
          "  [-ps|--print-stats]      print variable/originator stats at the "
          "end\n"
          "  [-pu|--print-updates]    print variables received, respects "
          "prefixes from -kp \n"
          "  [-py|--print-stats-periodic] print variable/originator stats at "
          "each period\n"
          "  [-q|--queue-length size] size of network buffers in bytes\n"
          "  [-r|--reduced]           use the reduced message header\n"
          "  [-rhz|--read-hz hz]      hertz rate of read threads\n"
          "  [-s|--save file]         save the resulting knowledge base as "
          "karl\n"
          "  [-sb|--save-binary file] save the resulting knowledge base as a\n"
          "                           binary checkpoint\n"
          "  [-sc|--save-checkpoint file] save any changes by logics since "
          "initial\n"
          "                           loads as a checkpoint diff to the "
          "specified\n"
          "                           file in an appended layer\n"
          "  [-scp|--save-checkpoint-prefix prfx]\n"
          "                           prefix of knowledge to save in "
          "checkpoint\n"
          "  [-scpl|--save-prefix-list file] filter saves by prefix. The "
          "file is a \n"
          "                           new-line delimited prefix list that "
          "can be read\n"
          "                           by NamedVectorCombinator.\n"
          "  [-sj|--save-json file]   save the resulting knowledge base as "
          "JSON\n"
          "  [-sff|--stream-from file] stream knowledge from a file\n"
          "  [-ss|--save-size bytes]  size of buffer needed for file saves\n"
          "  [-ssl|--ssl password]    encrypt with 256bit AES over network\n"
          "  [-st|--save-transsport file] a file to save transport settings "
          "to\n"
          "  [-sff|--stream-to file]  stream knowledge to a file\n"
          "  [-stp|--save-transport-prefix prfx] prefix to save settings at\n"
          "  [-stt|--save-transport-text file] a text file to save transport "
          "settings to\n"
          "  [-t|--time time]         time to wait for results. Same as -w.\n"
          "  [-tdp|--transport-debug-prefix pfx] prefix in the knowledge base\n"
          "                           to save transport debug info\n"
          "  [-u|--udp ip:port]       the udp ips to send to (first is self to "
          "bind to)\n"
          "  [-w|--wait seconds]      Wait for number of seconds before "
          "exiting\n"
          "  [-wy|-wp|--wait-for-periodic seconds]  Wait for number of "
          "seconds\n"
          "                           before performing periodic evaluation\n"
          "  [-y|--frequency hz]      frequency to perform evaluation. If "
          "negative,\n"
          "                           only runs once. If zero, hertz is "
          "infinite.\n"
          "                           If positive, hertz is that hertz rate.\n"
          "  [--zmq|--0mq proto://ip:port] a ZeroMQ endpoint to connect to.\n"
          "                           examples include tcp://127.0.0.1:30000\n"
          "                           or any of the other endpoint types like\n"
          "                           pgm://. For tcp, remember that the "
          "first\n"
          "                           endpoint defined must be your own, the\n"
          "                           one you are binding to, and all other\n"
          "                           agent endpoints must also be defined or\n"
          "                           no messages will ever be sent to them.\n"
          "                           Similarly, all agents will have to have\n"
          "                           this endpoint added to their list or\n"
          "                           this karl agent will not see them.\n"
          "  [-0|--init-logic logic]  logic containing initial variables (only "
          "ran once)\n"
          "  [-0f|--init-file file]   file containing initial variables (only "
          "ran once)\n"
          "  [-0b|--init-bin file]    file containing binary knowledge base, "
          "the result\n"
          "                           of save_context (only ran once)\n"
          "  [--meta-prefix prefix]   store checkpoint meta data at knowledge "
          "prefix\n"
          "  [--use-id]               use the id of the checkpointed binary "
          "load\n"
          "  [-v|--version]           print current MADARA version\n"
          "\n",
          argv[0]);

      exit(0);
    }
    else if(arg1 == "-i" || arg1 == "--input")
    {
      if(i + 1 < argc)
      {
        std::string filename = argv[i + 1];

        if(debug)
        {
          madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_ALWAYS,
              "\nReading logic from file %s:\n", filename.c_str());
        }

        filenames.push_back(filename);
      }

      ++i;
    }
    else if(arg1 == "-k" || arg1 == "--print-knowledge")
    {
      print_knowledge = true;
    }
    else if(arg1 == "-kp" || arg1 == "--print-prefix")
    {
      if(i + 1 < argc)
      {
        for(int j = i + 1;
             j < argc && strlen(argv[j]) > 0 && argv[j][0] != '-'; ++i, ++j)
        {
          if(debug)
          {
            std::cout << "  Limiting results to prefix " << argv[j] << "\n";
          }

          print_prefixes.push_back(argv[j]);
        }

        // ensure uniqueness
        utility::NamedVectorCombinator combinator;
        combinator.add("current", print_prefixes);
        combinator.merge({"current"}, print_prefixes);
      }
    }
    else if(arg1 == "-kpl" || arg1 == "--print-prefix-list")
    {
      if(i + 1 < argc)
      {
        // add file and ensure uniqueness
        utility::NamedVectorCombinator combinator;
        combinator.add("current", print_prefixes);
        combinator.from_file("new", argv[i + 1]);

        combinator.merge({"current", "new"}, print_prefixes);
      }
      
      ++i;
    }
    else if(arg1 == "-ky")
    {
      print_knowledge_frequency = true;
    }
    else if(arg1 == "-l" || arg1 == "--level")
    {
      if(i + 1 < argc)
      {
        int level;
        std::stringstream buffer(argv[i + 1]);
        buffer >> level;
        logger::global_logger->set_level(level);
      }

      ++i;
    }
    else if(arg1 == "-lcp" || arg1 == "--load-checkpoint-prefix")
    {
      if(i + 1 < argc)
      {
        load_checkpoint_settings.prefixes.push_back(argv[i + 1]);
      }

      ++i;
    }
    else if(arg1 == "-lcpl" || arg1 == "--load-prefix-list")
    {
      if(i + 1 < argc)
      {
        // add file and ensure uniqueness
        utility::NamedVectorCombinator combinator;
        combinator.add("current", print_prefixes);
        combinator.from_file("new", argv[i + 1]);

        combinator.merge({"current", "new"},
          load_checkpoint_settings.prefixes);
      }
      
      ++i;
    }
    else if(arg1 == "-ls" || arg1 == "--load-size")
    {
      if(i + 1 < argc)
      {
        std::stringstream buffer(argv[i + 1]);
        buffer >> load_checkpoint_settings.buffer_size;
      }

      ++i;
    }
    else if(arg1 == "-lt" || arg1 == "--load-transport")
    {
      if(i + 1 < argc)
      {
        if(load_transport_prefix == "")
          settings.load(argv[i + 1]);
        else
          settings.load(argv[i + 1], load_transport_prefix);
      }

      ++i;
    }
    else if(arg1 == "-ltp" || arg1 == "--load-transport-prefix")
    {
      if(i + 1 < argc)
      {
        load_transport_prefix = argv[i + 1];
      }

      ++i;
    }
    else if(arg1 == "-ltt" || arg1 == "--load-transport-text")
    {
      if(i + 1 < argc)
      {
        if(load_transport_prefix == "")
          settings.load_text(argv[i + 1]);
        else
          settings.load_text(argv[i + 1], load_transport_prefix);
      }

      ++i;
    }
    else if(arg1 == "-lz4" || arg1 == "--lz4")
    {
#ifdef _USE_LZ4_
      settings.add_filter(&lz4_transport_filter);
#else
      madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_ERROR,
          "ERROR: parameter (-lz4|--lz4) requires feature lz4\n");
#endif
    }
    else if(arg1 == "-lz4l" || arg1 == "--lz4-load")
    {
#ifdef _USE_LZ4_
      lz4_filters.push_back(filters::LZ4BufferFilter());
      load_checkpoint_settings.buffer_filters.push_back(
          &(*lz4_filters.rbegin()));
#else
      madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_ERROR,
          "ERROR: parameter (-lz4l|--lz4-load) requires feature lz4\n");
#endif
    }
    else if(arg1 == "-lz4s" || arg1 == "--lz4-save")
    {
#ifdef _USE_LZ4_
      lz4_filters.push_back(filters::LZ4BufferFilter());
      save_checkpoint_settings.buffer_filters.push_back(
          &(*lz4_filters.rbegin()));
#else
      madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_ERROR,
          "ERROR: parameter (-lz4s|--lz4-save) requires feature lz4\n");
#endif
    }
    else if(arg1 == "-m" || arg1 == "--multicast")
    {
      if(i + 1 < argc)
      {
        settings.hosts.push_back(argv[i + 1]);
        settings.type = transport::MULTICAST;
      }
      ++i;
    }
    else if(arg1 == "-ni")
    {
      if(i + 1 < argc)
      {
        std::string dirnames = argv[i + 1];

        std::vector<std::string> splitters, tokens, pivot_list;
        splitters.push_back(":");

        utility::tokenizer(dirnames, splitters, tokens, pivot_list);

        for(auto token : tokens)
        {
          capnp_import_dirs.add(token);
        }

        capnp_import_dirs_flag = true;
      }
      else
      {
        // print out error log
        madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_ERROR,
            "ERROR: parameter -ni dir1[:dir2:dir3]\n");
        exit(-1);
      }

      ++i;
    }
    else if(arg1 == "-n")
    {
      if(i + 1 < argc)
      {
        std::string msgtype_pair = argv[i + 1];

        std::vector<std::string> splitters, tokens, pivot_list;
        splitters.push_back(":");

        utility::tokenizer(msgtype_pair, splitters, tokens, pivot_list);

        if(tokens.size() == 2)
        {
          capnp_msg.push_back(tokens[0]);
          capnp_type.push_back(tokens[1]);
        }
        else
        {
          // print out error log
          madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_ERROR,
              "ERROR: parameter -n requires two tokens, "
              "in the form 'msg:type'\n");
          exit(-1);
        }

        capnp_msg_type_param_flag = true;
      }
      else
      {
        // print out error log
        madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_ERROR,
            "ERROR: parameter [-n|] msg:type\n");
        exit(-1);
      }

      ++i;
    }
    else if(arg1 == "-nf" || arg1 == "--capnp")
    {
      if(i + 1 < argc)
      {
        // capnp_import_dirs_flag && capnp_msg_type_param_flag
        if(!capnp_import_dirs_flag)
        {
          // write loggercode and continue
          madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_ERROR,
              "ERROR: parameter -ni is missing or must precede -nf param\n");
          ++i;
          continue;
        }

        if(!capnp_msg_type_param_flag)
        {
          // write loggercode and continue
          madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_ERROR,
              "ERROR: parameter -n is missing or must precede -nf param\n");
          ++i;
          continue;
        }

        std::string tagname;

        std::string filename = argv[i + 1];

        static capnp::SchemaParser schparser;
        capnp::ParsedSchema ps;
        ps = schparser.parseDiskFile(utility::extract_filename(filename),
            filename, capnp_import_dirs.asPtr());

        std::string msg;
        std::string typestr;
        capnp::ParsedSchema ps_type;
        size_t idx = 0;

        for(idx = 0; idx < capnp_msg.size(); ++idx)
        {
          msg = capnp_msg[idx];
          typestr = capnp_type[idx];
          ps_type = ps.getNested(typestr);

          if(!madara::knowledge::AnyRegistry::register_schema(
                  capnp_msg[idx].c_str(), ps_type.asStruct()))
          {
            madara_logger_ptr_log(logger::global_logger.get(),
                logger::LOG_ERROR, "CAPNP Failed on file  %s ",
                utility::extract_filename(filename).c_str());
          }
          else
          {
            madara_logger_ptr_log(logger::global_logger.get(),
                logger::LOG_TRACE, "CAPNP Loaded file  %s ",
                utility::extract_filename(filename).c_str());
          }
        }
      }
      else
      {
        madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_ERROR,
            "ERROR: parameter [-nf|--capnp] filename\n");
      }

      ++i;
    }
    else if(arg1 == "-o" || arg1 == "--host")
    {
      if(i + 1 < argc)
      {
        host = argv[i + 1];
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
            drop_rate, transport::PACKET_DROP_DETERMINISTIC);
      }

      ++i;
    }
    else if(arg1 == "-ps" || arg1 == "--print-stats")
    {
      print_stats = true;
    }
    else if(arg1 == "-py" || arg1 == "--print-stats-periodic")
    {
      print_stats_periodic = true;
    }
    else if(arg1 == "-pu" || arg1 == "--print-updates")
    {
      print_updates = true;
    }
    else if(arg1 == "-q" || arg1 == "--queue-length")
    {
      if(i + 1 < argc)
      {
        std::stringstream buffer(argv[i + 1]);
        buffer >> settings.queue_length;
      }

      ++i;
    }
    else if(arg1 == "-r" || arg1 == "--reduced")
    {
      settings.send_reduced_message_header = true;
    }
    else if(arg1 == "-rhz" || arg1 == "--read-hz")
    {
      if(i + 1 < argc)
      {
        std::stringstream buffer(argv[i + 1]);
        buffer >> settings.read_thread_hertz;
      }

      ++i;
    }
    else if(arg1 == "-s" || arg1 == "--save")
    {
      if(i + 1 < argc)
      {
        save_location = argv[i + 1];
      }

      ++i;
    }
    else if(arg1 == "-sb" || arg1 == "--save-binary")
    {
      if(i + 1 < argc)
      {
        save_binary = argv[i + 1];
      }

      ++i;
    }
    else if(arg1 == "-sc" || arg1 == "--save-checkpoint")
    {
      if(i + 1 < argc)
      {
        save_checkpoint = argv[i + 1];
      }

      ++i;
    }
    else if(arg1 == "-scp" || arg1 == "--save-checkpoint-prefix")
    {
      if(i + 1 < argc)
      {
        save_checkpoint_settings.prefixes.push_back(argv[i + 1]);
      }

      ++i;
    }
    else if(arg1 == "-scpl" || arg1 == "--save-prefix-list")
    {
      if(i + 1 < argc)
      {
        // add file and ensure uniqueness
        utility::NamedVectorCombinator combinator;
        combinator.add("current", print_prefixes);
        combinator.from_file("new", argv[i + 1]);

        combinator.merge({"current", "new"},
          save_checkpoint_settings.prefixes);
      }
      
      ++i;
    }
    else if(arg1 == "-sff" || arg1 == "--stream-from")
    {
      if(i + 1 < argc)
      {
        stream_from = argv[i + 1];
      }

      ++i;
    }
    else if(arg1 == "-sj" || arg1 == "--save-json")
    {
      if(i + 1 < argc)
      {
        save_json = argv[i + 1];
      }

      ++i;
    }
    else if(arg1 == "-ss" || arg1 == "--save-size")
    {
      if(i + 1 < argc)
      {
        std::stringstream buffer(argv[i + 1]);
        buffer >> save_checkpoint_settings.buffer_size;
      }

      ++i;
    }
    else if(arg1 == "-ssl" || arg1 == "--ssl")
    {
#ifdef _USE_SSL_
      if(i + 1 < argc)
      {
        ssl_transport_filter.generate_key(argv[i + 1]);
        settings.add_filter(&ssl_transport_filter);
        ++i;
      }
      else
      {
        madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_ERROR,
            "ERROR: parameter (-ssl|--ssl) requires password\n");
      }
#else
      madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_ERROR,
          "ERROR: parameter (-ssl|--ssl) requires feature ssl compiled into "
          "MADARA\n");
      ++i;
#endif
    }
    else if(arg1 == "-ssll" || arg1 == "--ssl-load")
    {
#ifdef _USE_SSL_
      if(i + 1 < argc)
      {
        ssl_filters.push_back(filters::AESBufferFilter());
        ssl_filters[ssl_filters.size() - 1].generate_key(argv[i + 1]);
        load_checkpoint_settings.buffer_filters.push_back(
            &(*ssl_filters.rbegin()));
        ++i;
      }
      else
      {
        madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_ERROR,
            "ERROR: parameter (-ssll|--ssl-load) requires password\n");
      }
#else
      madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_ERROR,
          "ERROR: parameter (-ssll|--ssl-load) requires feature ssl\n");
      ++i;
#endif
    }
    else if(arg1 == "-ssls" || arg1 == "--ssl-save")
    {
#ifdef _USE_SSL_
      if(i + 1 < argc)
      {
        ssl_filters.push_back(filters::AESBufferFilter());
        ssl_filters[ssl_filters.size() - 1].generate_key(argv[i + 1]);
        save_checkpoint_settings.buffer_filters.push_back(
            &(*ssl_filters.rbegin()));
        ++i;
      }
      else
      {
        madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_ERROR,
            "ERROR: parameter (-ssls|--ssl-save) requires password\n");
      }
#else
      madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_ERROR,
          "ERROR: parameter (-ssls|--ssl-save) requires feature ssl\n");
      ++i;
#endif
    }
    else if(arg1 == "-st" || arg1 == "--save-transport")
    {
      if(i + 1 < argc)
      {
        save_transport = argv[i + 1];
      }

      ++i;
    }
    else if(arg1 == "-stf" || arg1 == "--stream-to")
    {
      if(i + 1 < argc)
        stream_to = argv[i + 1];

      ++i;
    }
    else if(arg1 == "-stp" || arg1 == "--save-transport-prefix")
    {
      if(i + 1 < argc)
      {
        save_transport_prefix = argv[i + 1];
      }

      ++i;
    }
    else if(arg1 == "-stt" || arg1 == "--save-transport-text")
    {
      if(i + 1 < argc)
      {
        save_transport_text = argv[i + 1];
      }

      ++i;
    }
    else if(arg1 == "-tdp" || arg1 == "--transport-debug-prefix")
    {
      if(i + 1 < argc)
      {
        settings.debug_to_kb(argv[i + 1]);
      }

      ++i;
    }
    else if(arg1 == "-u" || arg1 == "--udp")
    {
      if(i + 1 < argc)
      {
        settings.hosts.push_back(argv[i + 1]);
        settings.type = transport::UDP;
      }
      ++i;
    }
    else if(arg1 == "-v" || arg1 == "--version")
    {
      madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_ALWAYS,
          "MADARA version: %s\n", utility::get_version().c_str());
    }
    else if(arg1 == "-w" || arg1 == "--wait" || arg1 == "-t" ||
             arg1 == "--time")
    {
      if(i + 1 < argc)
      {
        waiting = true;
        std::stringstream buffer(argv[i + 1]);
        buffer >> wait_time;
      }
      ++i;
    }
    else if(arg1 == "-wy" || arg1 == "-wp" || arg1 == "--wait-for-periodic")
    {
      if(i + 1 < argc)
      {
        waiting_for_periodic = true;
        std::stringstream buffer(argv[i + 1]);
        buffer >> wait_for_periodic;
      }
      ++i;
    }
    else if(arg1 == "-y" || arg1 == "--frequency")
    {
      if(i + 1 < argc)
      {
        std::stringstream buffer(argv[i + 1]);
        buffer >> frequency;
      }

      ++i;
    }
    else if(arg1 == "--zmq" || arg1 == "--0mq")
    {
      if(i + 1 < argc)
      {
        settings.hosts.push_back(argv[i + 1]);
        settings.type = transport::ZMQ;
      }
      ++i;
    }
    else if(arg1 == "-0" || arg1 == "--init-logic")
    {
      if(i + 1 < argc)
      {
        initlogics.push_back(argv[i + 1]);
      }

      ++i;
    }
    else if(arg1 == "-0f" || arg1 == "--init-file")
    {
      if(i + 1 < argc)
      {
        std::string filename;
        std::stringstream buffer(argv[i + 1]);
        buffer >> filename;

        if(debug)
        {
          madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_ALWAYS,
              "\nReading logic from file %s:\n", filename.c_str());
        }

        initfiles.push_back(filename);
      }

      ++i;
    }
    else if(arg1 == "-0b" || arg1 == "--init-bin")
    {
      if(i + 1 < argc)
      {
        initbinaries = argv[i + 1];

        if(debug)
        {
          madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_ALWAYS,
              "Will be reading binary checkpoint from file %s:\n",
              initbinaries.c_str());
        }
      }

      ++i;
    }
    else if(arg1 == "--meta-prefix")
    {
      if(i + 1 < argc)
      {
        meta_prefix = argv[i + 1];

        if(debug)
        {
          madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_ALWAYS,
              "Will be saving checkpoint meta data to to prefix %s:\n",
              meta_prefix.c_str());
        }
      }

      ++i;
    }
    else if(arg1 == "--use-id")
    {
      use_id = true;
    }
    else if(logic == "")
    {
      logic = arg1;
    }
  }
}

// loads a config file into a pased in string vector.
// and then it calls handle_arguments() for processing
bool load_config_file(std::string full_path, size_t recursion_limit)
{
  std::string flag;
  std::string param;
  std::string flag_param;
  std::string config_file;

  // storage for config file arguments
  std::deque<std::string> argv_config_files;
  std::vector<const char*> argvp_config_files;

  // load the a karl config file
  std::ifstream file(full_path.c_str());
  if(!file)
  {
    madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_ERROR,
        "Unable to open karl config file: %s\n", full_path.c_str());
    return false;
  }

  // load progam path for first arg
  argv_config_files.push_back("");

  madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_TRACE,
      "Found karl config file: %s\n", full_path.c_str());

  // read each line of the text formatted file in, each line contains
  // one flag followed by a space then the param if there is a parameter
  while(std::getline(file, flag_param))
  {
    flag = flag_param.substr(0, flag_param.find_first_of(" "));
    if(flag_param.find_first_of(" ") == std::string::npos)
    {
      param = "";
    }
    else
    {
      param = flag_param.substr(flag_param.find_first_of(" ") + 1,
          flag_param.length() - (flag_param.find_first_of(" ") + 1));      
      param = madara::utility::expand_envs(param);          
    }
    madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_TRACE,
        "Flag: %s, Param: %s\n", flag.c_str(), param.c_str());
    madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_TRACE,
        "Found a flag saving now...\n");
    argv_config_files.push_back(flag);

    if(param != "")
    {
      madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_TRACE,
          "Found a param saving now...\n");
      argv_config_files.push_back(param);
    }
  }
  file.close();

  argvp_config_files.reserve(argv_config_files.size() + 1);
  argvp_config_files.push_back("");

  std::transform(argv_config_files.begin(), argv_config_files.end(),
      std::back_inserter(argvp_config_files),
      [](const std::string &s) { return s.c_str(); });

  handle_arguments(argvp_config_files.size(), argvp_config_files.data(),
      recursion_limit - 1);

  return true;
}

void print_all_prefixes(knowledge::KnowledgeBase& context)
{
  madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_ALWAYS,
      "\nKnowledge in Knowledge Base (filtered by prefixes):\n");
  for(size_t i = 0; i < print_prefixes.size(); ++i)
  {
    knowledge::KnowledgeMap matches = context.to_map(print_prefixes[i]);

    for(knowledge::KnowledgeMap::const_iterator j = matches.begin();
         j != matches.end(); ++j)
    {
      madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_ALWAYS,
          "%s=%s\n", j->first.c_str(), j->second.to_string(", ").c_str());
    }
  }
}

class Evaluator : public threads::BaseThread
{
public:
  Evaluator(knowledge::KnowledgeBase& knowledge,
      std::vector<knowledge::CompiledExpression>& expressions)
    : knowledge_(&knowledge), expressions_(expressions)
  {
  }

  virtual void run(void)
  {
    for(size_t i = 0; i < expressions_.size(); ++i)
    {
#ifndef _MADARA_NO_KARL_
      knowledge::KnowledgeRecord result =
          knowledge_->evaluate(expressions_[i], knowledge::EvalSettings::SEND);

      if(check_result && result.is_true())
      {
        this->terminated = 1;
      }

#endif  // _MADARA_NO_KARL_
    }

    if(print_knowledge_frequency)
    {
      if(print_prefixes.size() == 0)
      {
        knowledge_->print();
      }
      else
      {
        print_all_prefixes(*knowledge_);
      }
    }

    if(print_stats_periodic)
    {
      stats_filter.print();
    }
  }

private:
  knowledge::KnowledgeBase* knowledge_;
  std::vector<knowledge::CompiledExpression>& expressions_;
};

int main(int argc, char** argv)
{
  // load and incorporate the defaul karl config file arguments, but only once
  // and first
  madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_TRACE,
      "Attempting to load default karl config...\n");
      
  if(utility::file_exists(default_karl_config))
  {
    load_config_file(default_karl_config);
  }


  // handle all user arguments
  handle_arguments(argc, (const char**)argv);

  if(print_stats || print_stats_periodic)
  {
    settings.add_receive_filter(&stats_filter);
  }

  filters::PrefixPrint print_filter(print_prefixes);
  if(debug)
  {
    settings.add_send_filter(&print_filter);
    settings.add_receive_filter(&print_filter);
  }

  // A less verbose form of receiving updates
  filters::PrefixPrint minimal_print_filter(print_prefixes, false);
  if(print_updates)
  {
    settings.add_receive_filter(&minimal_print_filter);
  }

  // save transport always happens after all possible transport chagnes
  if(save_transport != "")
  {
    if(save_transport_prefix == "")
    {
      settings.save(save_transport);
    }
    else
    {
      settings.save(save_transport, save_transport_prefix);
    }
  }

  // save transport always happens after all possible transport chagnes
  if(save_transport_text != "")
  {
    if(save_transport_prefix == "")
    {
      settings.save_text(save_transport_text);
    }
    else
    {
      settings.save_text(save_transport_text, save_transport_prefix);
    }
  }

  // create a knowledge base and setup our id
  knowledge::KnowledgeBase kb(host, settings);

  // stream first, then allow overwrites
  load_checkpoint_settings.filename = stream_from;

  // create a CheckpointPlayer and CheckpointStreamer, just in case
  knowledge::CheckpointPlayer player(
      kb.get_context(), load_checkpoint_settings);
  // knowledge::CheckpointStreamer streamer (save_checkpoint_settings, kb, 100);

  // start a stream if the user asked us to
  if(stream_to != "")
  {
    madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_MAJOR,
        "Streaming to %s:\n", stream_to.c_str());

    save_checkpoint_settings.filename = stream_to;

    auto streamer = utility::mk_unique<knowledge::CheckpointStreamer>(
        save_checkpoint_settings, kb, 100);
    kb.attach_streamer(std::move(streamer));
  }

  if(stream_from != "")
  {
    madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_MAJOR,
        "Streaming from %s:\n", stream_from.c_str());

    // start the player thread and update in step with STK
    player.start();
  }

  if(initbinaries != "")
  {
    madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_MAJOR,
        "\nReading binary checkpoint from file %s:\n", initbinaries.c_str());

    madara::knowledge::EvalSettings silent(true, true, true, true, true);
    // madara::knowledge::FileHeader meta;
    load_checkpoint_settings.filename = initbinaries;
    kb.load_context(load_checkpoint_settings);

    if(meta_prefix != "")
    {
      kb.set(meta_prefix + ".originator", load_checkpoint_settings.originator,
          silent);
      kb.set(
          meta_prefix + ".version", load_checkpoint_settings.version, silent);
      kb.set(meta_prefix + ".last_timestamp",
          (Integer)load_checkpoint_settings.last_timestamp, silent);
      kb.set(meta_prefix + ".initial_timestamp",
          (Integer)load_checkpoint_settings.initial_timestamp, silent);
      kb.set(meta_prefix + ".last_lamport_clock",
          (Integer)load_checkpoint_settings.last_lamport_clock, silent);
      kb.set(meta_prefix + ".initial_lamport_clock",
          (Integer)load_checkpoint_settings.initial_lamport_clock, silent);
      kb.set(meta_prefix + ".states", (Integer)load_checkpoint_settings.states,
          silent);
      kb.set(meta_prefix + ".current_timestamp", (Integer)utility::get_time(),
          silent);
    }
  }

  // build the expressions to evaluate
  std::vector<knowledge::CompiledExpression> expressions;

  // each file logic is evaluated first
  for(StringVector::const_iterator i = filenames.begin(); i != filenames.end();
       ++i)
  {
    if(utility::file_exists(*i))
    {
      // for the moment, we need to do the filter decode call ourself
      knowledge::CheckpointSettings checkpoint_settings(
          load_checkpoint_settings);
      checkpoint_settings.filename = *i;

      // because this is text, ignore header checks if no buffer filters
      if(checkpoint_settings.buffer_filters.size() == 0)
      {
        checkpoint_settings.ignore_header_check = true;
      }

      expressions.push_back(kb.compile(kb.file_to_string(checkpoint_settings)));
    }
    else
    {
      madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_ALWAYS,
          "\nUnable to load file %s. "
          "Check that file exists and that you have permission to read.\n",
          i->c_str());
    }
  }

  // use no harm settings for initialization (faster, doesn't send data)
  knowledge::EvalSettings noharm;
  noharm.treat_globals_as_locals = true;
  noharm.signal_changes = false;
  noharm.track_local_changes = false;

  // set initialization variables from files into the knowledge base
  if(initfiles.size() > 0)
  {
    for(StringVector::const_iterator i = initfiles.begin();
         i != initfiles.end(); ++i)
    {
      if(utility::file_exists(*i))
      {
        kb.evaluate(utility::file_to_string(*i), noharm);
      }
      else
      {
        madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_ALWAYS,
            "\nUnable to load file %s. "
            "Check that file exists and that you have permission to read.\n",
            i->c_str());
      }
    }
  }

  // set initialization variables from command-line logics
  if(initlogics.size() > 0)
  {
    for(StringVector::const_iterator i = initlogics.begin();
         i != initlogics.end(); ++i)
    {
      kb.evaluate(*i, noharm);
    }
  }

  kb.reset_checkpoint();

  // command line logics are evaluated last
  if(logic != "")
  {
    expressions.push_back(kb.compile(logic));
  }

  // check frequency to see if we should only execute once
  if(frequency < 0)
  {
    if(!after_wait)
    {
      for(size_t i = 0; i < expressions.size(); ++i)
      {
#ifndef _MADARA_NO_KARL_
        knowledge::KnowledgeRecord result =
            kb.evaluate(expressions[i], knowledge::EvalSettings::SEND);

        if(check_result && result.is_true())
        {
          break;
        }
#endif  // _MADARA_NO_KARL_
      }
    }

    // if user requests to wait, do so before the debug print
    if(waiting_for_periodic || waiting)
    {
      utility::sleep(wait_time + wait_for_periodic);
    }

    if(after_wait)
    {
      for(size_t i = 0; i < expressions.size(); ++i)
      {
#ifndef _MADARA_NO_KARL_
        kb.evaluate(expressions[i], madara::knowledge::EvalSettings::SEND);
#endif  // _MADARA_NO_KARL_
      }
    }   // if(after_wait)
  }     // if(frequency < 0)
  else  // frequency >= 0
  {
    threads::Threader threader(kb);

    // if the user specified a wait before evaluation, sleep for the time
    if(waiting_for_periodic)
    {
      utility::sleep(wait_for_periodic);
    }

    threader.run(frequency, "evaluator", new Evaluator(kb, expressions), false);

    bool terminated = false;

    // if user requests to wait, do so before the debug print
    if(waiting)
    {
      knowledge::WaitSettings ws;
      ws.max_wait_time = wait_time;

      terminated = threader.wait(ws);
    }

    if(!terminated)
    {
      threader.terminate();
      threader.wait();
    }
  }

  // if the user requests debugging information, print final knowledge
  if(debug || print_knowledge)
  {
    if(print_prefixes.size() == 0)
    {
      kb.print();
    }
    else
    {
      print_all_prefixes(kb);
    }
  }

  if(print_stats)
  {
    stats_filter.print();
  }

  // save as checkpoint of changes by logics and input files
  if(save_checkpoint.size() > 0)
  {
    save_checkpoint_settings.filename = save_checkpoint;
    kb.save_checkpoint(save_checkpoint_settings);
  }

  // save as karl if requested
  if(save_location.size() > 0)
  {
    save_checkpoint_settings.filename = save_location;
    kb.save_as_karl(save_checkpoint_settings);
  }

  // save as karl if requested
  if(save_json.size() > 0)
  {
    save_checkpoint_settings.filename = save_json;
    kb.save_as_json(save_checkpoint_settings);
  }

  // save as binary if requested
  if(save_binary.size() > 0)
  {
    save_checkpoint_settings.filename = save_binary;
    kb.save_context(save_checkpoint_settings);
  }

  return 0;
}
