
#include <string>
#include <vector>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <sstream>
#include <assert.h>

#include "madara/knowledge/CheckpointPlayer.h"
#include "madara/knowledge/KnowledgeBase.h"
#include "madara/knowledge/AnyRegistry.h"
#include "madara/threads/Threader.h"

#include "madara/utility/Utility.h"
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

typedef knowledge::KnowledgeRecord::Integer  Integer;
typedef std::vector <std::string>            StringVector;

// checkpoint settings
knowledge::CheckpointSettings load_checkpoint_settings;

// options for analysis
bool print_knowledge = false;
std::vector <std::string> print_prefixes;
std::string save_file;

// Capnp types and globals
kj::Vector<kj::StringPtr> capnp_import_dirs;
std::vector<std::string> capnp_msg;
std::vector<std::string> capnp_type;
bool capnp_msg_type_param_flag = false;
bool capnp_import_dirs_flag = false;

/**
 * Class for keeping track of updates on a variable
 **/
class VariableStats
{
public:
  VariableStats ()
  : name (""), first (0), last (0), updates (0), bytes (0)
  {

  }

  std::ostream & print (std::ostream & output)
  {
    uint64_t total_ns = last - first;
    uint64_t total_s = total_ns / 1000000000;
    double hertz = (double)updates / total_s;
    double kbs = (double)bytes / 1000 / total_s;

    output << std::fixed << std::setprecision (2);
    output << name << ": " << updates << " updates ";
    output << " (@" << hertz << "hz) (@" << kbs << "KB/s) (Size: Min ";
    output << min_size << " B, Max: " << max_size << " B)\n";

    return output;
  }

  std::string name;
  uint64_t first;
  uint64_t last;
  uint64_t updates;
  uint64_t bytes;
  uint64_t max_size;
  uint64_t min_size;
};

/// convenience typedef for Variable updates
typedef std::map <std::string, VariableStats> VariableUpdates;

// handle command line arguments
void handle_arguments (int argc, char ** argv)
{
  for (int i = 1; i < argc; ++i)
  {
    std::string arg1 (argv[i]);

    if (arg1 == "-f" || arg1 == "--logfile")
    {
      if (i + 1 < argc)
      {
        load_checkpoint_settings.filename = argv[i + 1];

        std::cout << "  Loading file " << argv[i + 1] << "\n";
      }

      ++i;
    }
    else if (arg1 == "-k" || arg1 == "--print-knowledge")
    {
      print_knowledge = true;
    }
    else if (arg1 == "-kp" || arg1 == "--print-prefixes")
    {
      if (i + 1 < argc)
      {
        for (int j = i + 1;
             j < argc && strlen (argv[j]) > 0 && argv[j][0] != '-'; ++i, ++j)
        {
          std::cout << "  Limiting results to prefix " << argv[j] << "\n";
          print_prefixes.push_back (argv[j]);
        }
      }
    }
    else if (arg1 == "-l" || arg1 == "--level")
    {
      if (i + 1 < argc)
      {
        int level;
        std::stringstream buffer (argv[i + 1]);
        buffer >> level;
        logger::global_logger->set_level (level);
        std::cout << "  Setting log level to " << level << "\n";
      }

      ++i;
    }
    else if (arg1 == "-lcp" || arg1 == "--load-checkpoint-prefix")
    {
      if (i + 1 < argc)
      {
        load_checkpoint_settings.prefixes.push_back (argv[i + 1]);
        std::cout << "  Limiting load to prefix " << argv[i + 1] << "\n";
      }

      ++i;
    }
    else if (arg1 == "-ls" || arg1 == "--load-size")
    {
      if (i + 1 < argc)
      {
        std::stringstream buffer (argv[i + 1]);
        buffer >> load_checkpoint_settings.buffer_size;
        std::cout << "  Setting load size to " << argv[i + 1] << "\n";
      }

      ++i;
    }
    else if (arg1 == "-ni" )
    {
      if (i + 1 < argc)
      { 
        std::string dirnames = argv[i + 1];

        std::vector<std::string> splitters, tokens, pivot_list;
        splitters.push_back (":");
      
        utility::tokenizer (dirnames, splitters, tokens, pivot_list);

        for (auto token : tokens)
        {
          capnp_import_dirs.add (token);
        }

        capnp_import_dirs_flag = true;
      }
      else
      {
        //print out error log
        madara_logger_ptr_log (logger::global_logger.get (), logger::LOG_ERROR,
          "ERROR: parameter -ni dir1[:dir2:dir3]\n");
        exit (-1);
      }

      ++i;
    }
    else if (arg1 == "-n")
    {
      if (i + 1 < argc)
      {
        std::string msgtype_pair = argv[i + 1];

        std::vector<std::string> splitters, tokens, pivot_list;
        splitters.push_back (":");
      
        utility::tokenizer (msgtype_pair, splitters, tokens, pivot_list);

        if (tokens.size () == 2)
        {
          capnp_msg.push_back (tokens[0]);
          capnp_type.push_back (tokens[1]);
        }
        else
        {
          //print out error log
          madara_logger_ptr_log (logger::global_logger.get (), logger::LOG_ERROR,
            "ERROR: parameter -n requires two tokens, "
            "in the form 'msg:type'\n");
          exit (-1);
        }

        capnp_msg_type_param_flag = true;
      }
      else
      {
        //print out error log
        madara_logger_ptr_log (logger::global_logger.get (), logger::LOG_ERROR,
          "ERROR: parameter [-n|] msg:type\n");
        exit (-1);
      }

      ++i;
    }
    else if (arg1 == "-nf" || arg1 == "--capnp")
    {
      if (i + 1 < argc)
      {
        //capnp_import_dirs_flag && capnp_msg_type_param_flag
        if (!capnp_import_dirs_flag)
        {
          //write loggercode and continue
          madara_logger_ptr_log (
            logger::global_logger.get (), logger::LOG_ERROR,
            "ERROR: parameter -ni is missing or must precede -nf param\n");
          ++i;
          continue;
        }
        
        if (!capnp_msg_type_param_flag)
        {
          //write loggercode and continue
          madara_logger_ptr_log (
            logger::global_logger.get (), logger::LOG_ERROR,
            "ERROR: parameter -n is missing or must precede -nf param\n");
          ++i;
          continue;
        }

        std::string tagname;

        std::string filename = argv[i + 1];

        static capnp::SchemaParser schparser;
        capnp::ParsedSchema ps;
        ps = schparser.parseDiskFile (
          utility::extract_filename (filename), filename, capnp_import_dirs.asPtr ());

        std::string msg;
        std::string typestr;
        capnp::ParsedSchema ps_type;
        size_t idx = 0;

        for (idx = 0; idx < capnp_msg.size (); ++idx)
        {
          msg = capnp_msg[idx];
          typestr = capnp_type[idx];
          ps_type = ps.getNested (typestr);

          if (!madara::knowledge::AnyRegistry::register_schema (
            capnp_msg[idx].c_str (), ps_type.asStruct ()))
          {
            madara_logger_ptr_log (
              logger::global_logger.get (), logger::LOG_ERROR,
              "CAPNP Failed on file  %s ",
                utility::extract_filename (filename).c_str ());
          }
          else
          {
            madara_logger_ptr_log (
              logger::global_logger.get (), logger::LOG_TRACE,
                 "CAPNP Loaded file  %s ",
                 utility::extract_filename (filename).c_str ());
          }
        }
      }
      else
      {
        madara_logger_ptr_log (logger::global_logger.get (), logger::LOG_ERROR,
          "ERROR: parameter [-nf|--capnp] filename\n");
      }

      ++i;
    }
    else if (arg1 == "-s" || arg1 == "--save")
    {
      if (i + 1 < argc)
      {
        save_file = argv[i + 1];
        std::cout << "  Saving results to " << argv[i + 1] << "\n";
      }

      ++i;
    }
    else if (arg1 == "-v" || arg1 == "--version")
    {
      madara_logger_ptr_log (logger::global_logger.get (), logger::LOG_ALWAYS,
        "MADARA version: %s\n",
        utility::get_version ().c_str ());
    }
    else
    {
      madara_logger_ptr_log (logger::global_logger.get (), logger::LOG_ALWAYS,
        "\nOption %s:\n" \
        "\nProgram summary for %s [options] [Logic]:\n\n" \
        "Inspects STK for summary info on knowledge updates.\n\noptions:\n" \
        "  [-f|--stk-file file]     STK file to load and analyze\n" \
        "  [-h|--help]              print help menu (i.e., this menu)\n" \
        "  [-i|--input file]        file containing MADARA logic to evaluate\n" \
        "  [-k|--print-knowledge]   print final knowledge\n" \
        "  [-kp|--print-prefix pfx] filter prints by prefix. Can be multiple.\n" \
        "  [-ky]                    print knowledge after frequent evaluations\n" \
        "  [-l|--level level]       the logger level (0+, higher is higher detail)\n" \
        "  [-lcp|--load-checkpoint-prefix prfx]\n" \
        "                           prefix of knowledge to load from checkpoint\n" \
        "  [-ls|--load-size bytes]  size of buffer needed for file load\n" \
        "  [-n|--capnp tag:msg_type] register tag with given message schema.\n" \
        "                            See also -nf and -ni.\n"\
        "  [-nf|--capnp-file]       load capnp file. Must appear after all -n and -ni.\n" \
        "  [-ni|--capnp-import ]    add directory to capnp directory imports. "\
        "                           Must appear before all -n and -nf.\n" \
        "  [-s|--save file]         save the results to a file\n" \
        "\n",
        arg1.c_str (), argv[0]);
      exit (0);
    }
  }
}

int main (int argc, char ** argv)
{
  std::cout << "Inspection settings:\n" << std::flush;

  // handle all user arguments
  handle_arguments (argc, argv);

  knowledge::KnowledgeBase kb;
  std::cout << "Creating CheckpointReader with file contents... " <<
    std::flush;
  knowledge::CheckpointReader reader (load_checkpoint_settings);
  std::cout << "done\n";

  VariableUpdates variables;

  std::cout << "Iterating through updates... " << std::flush;
  while (true)
  {
    auto cur = reader.next ();
    if (cur.first == "")
      break;

    VariableStats & variable = variables[cur.first];

    uint64_t size = cur.second.size ();

    if (cur.second.is_integer_type ())
    {
      size *= sizeof (int64_t);
    }
    else if (cur.second.is_double_type ())
    {
      size *= sizeof (double);
    }

    if (variable.name == "")
    {
      variable.name = cur.first;
      variable.first = cur.second.toi ();
      variable.min_size = size;
      variable.max_size = size;
    }
    else
    {
      if (variable.min_size > size)
      {
        variable.min_size = size;
      }
      if (variable.max_size < size)
      {
        variable.max_size = size;
      }
    }

    variable.last = cur.second.toi ();
    variable.bytes += size;
    ++variable.updates;
  }
  std::cout << "done\n";

  if (save_file != "")
  {
    std::cout << "Saving results to " << save_file << "..." << std::flush;
    std::ofstream output (save_file);

    if (output)
    {
      for (auto variable : variables)
      {
        bool prefix_match = print_prefixes.size () == 0;

        if (!prefix_match)
        {
          for (auto prefix : print_prefixes)
          {
            if (utility::begins_with (variable.first, prefix))
            {
              prefix_match = true;
              break;
            }
          }
        }

        if (prefix_match)
        {
          variable.second.print (output);
        }
      }
    }

    output.close ();
    std::cout << " done\n";
  }
  else
  {
    std::cout << "Printing results to stdout...\n" << std::flush;
    for (auto variable : variables)
    {
      bool prefix_match = print_prefixes.size () == 0;

      if (!prefix_match)
      {
        for (auto prefix : print_prefixes)
        {
          if (utility::begins_with (variable.first, prefix))
          {
            prefix_match = true;
            break;
          }
        }
      }

      if (prefix_match)
      {
        variable.second.print (std::cout);
      }
    }
  }

  if (print_knowledge)
  {
    kb.print ();
  }

  return 0;
}
