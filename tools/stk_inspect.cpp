
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
#include "madara/utility/NamedVectorCombinator.h"
#include "madara/logger/GlobalLogger.h"
#include "madara/knowledge/containers/FlexMap.h"

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
namespace containers = knowledge::containers;

typedef knowledge::KnowledgeRecord::Integer Integer;
typedef std::vector<std::string> StringVector;

// checkpoint settings
knowledge::CheckpointSettings load_checkpoint_settings;

// options for analysis
bool print_knowledge = false;
bool print_stats = false;
std::vector<std::string> print_prefixes;
std::string save_file;

// Capnp types and globals
kj::Vector<kj::StringPtr> capnp_import_dirs;
std::vector<std::string> capnp_msg;
std::vector<std::string> capnp_type;
bool capnp_msg_type_param_flag = false;
bool capnp_import_dirs_flag = false;
bool summary = true;

// default KaRL config file
std::string default_config_file =
    madara::utility::expand_envs("$(HOME)/.madara/stk_inspect.cfg");

// config file recursion limit
const size_t default_recursion_limit = 10;

// check for stats
std::string checkfile;
std::string check;

// keep track of first and last observed time of insertion
uint64_t first_toi = 0;
uint64_t last_toi = 0;

// debugging printouts
bool debug = false;

// recursively loads a config file(s) and processe with handle_arguments
bool load_config_file(
    std::string full_path, size_t recursion_limit = default_recursion_limit);

/**
 * Class for keeping track of updates on a variable
 **/
class VariableStats
{
public:
  VariableStats() : name(""), first(0), last(0), updates(0), bytes(0) {}

  std::ostream& print(std::ostream& output) const
  {
    uint64_t total_ns = last_toi - first_toi;
    uint64_t total_s = total_ns / 1000000000;
    double hertz =(double)updates / total_s;
    double kbs =(double)bytes / 1000 / total_s;

    output << std::fixed << std::setprecision(2);
    output << name << ": " << updates << " updates ";
    output << "(@" << hertz << "hz)(@" << kbs << "KB/s)(Size: Min ";
    output << min_size << " B, Max: " << max_size << " B)\n";

    return output;
  }

  void save(knowledge::KnowledgeBase kb) const
  {
    uint64_t total_ns = last_toi - first_toi;
    uint64_t total_s = total_ns / 1000000000;
    double hertz =(double)updates / total_s;
    double kbs =(double)bytes / 1000 / total_s;

    kb.set(name + ".first",(int64_t)first);
    kb.set(name + ".last",(int64_t)last);
    kb.set(name + ".updates",(int64_t)updates);
    kb.set(name + ".bytes",(int64_t)bytes);
    kb.set(name + ".max_size",(int64_t)max_size);
    kb.set(name + ".min_size",(int64_t)min_size);
    kb.set(name + ".duration",(int64_t)total_ns);
    kb.set(name + ".hz", hertz);
    kb.set(name + ".kbs", kbs);
  }

  std::string name;
  uint64_t first;
  uint64_t last;
  uint64_t updates;
  uint64_t bytes;
  uint64_t max_size;
  uint64_t min_size;
  knowledge::KnowledgeRecord value;
};

/// convenience typedef for Variable updates
typedef std::map<std::string, VariableStats> VariableUpdates;

// print variables to a specified output stream
void print_variables(const VariableUpdates & variables, 
  std::ostream & output, bool print_value,
  bool print, bool save_stats,
  knowledge::KnowledgeBase stats)
{
  for(auto variable : variables)
  {
    bool prefix_match = print_prefixes.size() == 0;

    if(!prefix_match)
    {
      for(auto prefix : print_prefixes)
      {
        if(utility::begins_with(variable.first, prefix))
        {
          prefix_match = true;
          break;
        }
      }
    }

    if(prefix_match)
    {
      // if user wants to print a value
      if(print_value)
      {
        output << variable.first << "=" << variable.second.value << "\n";
      }

      // if(summary)
      if(print)
      {
        variable.second.print(output);
      }

      // if(print_stats || check != "" || checkfile != "")
      if(save_stats)
      {
        variable.second.save(stats);
      }
    }
  }
}

int64_t process_command(const std::string & command,
    knowledge::KnowledgeBase kb, knowledge::KnowledgeBase stats,
    const VariableUpdates & variables)
{
  int64_t result = 0;
  std::string logic = command;
  bool shell_mode = false;

  utility::strip_extra_white_space(logic);

  for(size_t execs = 0; execs < 1 || shell_mode; ++execs)
  {
    if(utility::begins_with(logic, "add_prefix"))
    {
      // format: add_prefix first second third fourth
      if(debug)
      {
        std::cout << "\n";
        std::cout << last_toi << ": Event trigger: adding var prefixes ";
        std::cout << logic.substr(11) << "\n";
        std::cout << "\n";
      }

      // split by space and discard first token(add_prefix)
      std::vector<std::string> splitters, tokens, pivot_list;
      splitters.push_back(" ");

      utility::tokenizer(logic, splitters, tokens, pivot_list);

      for(size_t i = 1; i < tokens.size(); ++i)
      {
        print_prefixes.push_back(tokens[i]);
        ++result;
      }

      // ensure uniqueness
      utility::NamedVectorCombinator combinator;
      combinator.add("current", print_prefixes);
      combinator.merge({"current"}, print_prefixes); 
    }
    else if(utility::begins_with(logic, "clear_prefixes"))
    {
      // format: clear_prefixes
      if(debug)
      {
        std::cout << "\n";
        std::cout << last_toi << ": Event trigger: clearing var prefixes\n";
        std::cout << "\n";
      }

      print_prefixes.clear();

      ++result;
    }
    // note that there is a really good reason to have this before eval
    else if(utility::begins_with(logic, "eval_stats"))
    {
      // format: clear_prefixes
      if(debug)
      {
        std::cout << "\n";
        std::cout << last_toi << ": Event trigger: evaluating stats logic\n";
        std::cout << "\n";
      }

      result += stats.evaluate(
        logic.substr(11), knowledge::EvalSettings::DELAY_EXPAND).to_integer();

      ++result;
    }
    else if(utility::begins_with(logic, "eval"))
    {
      // format: clear_prefixes
      if(debug)
      {
        std::cout << "\n";
        std::cout << last_toi << ": Event trigger: evaluating kb logic\n";
        std::cout << "\n";
      }

      result += kb.evaluate(
        logic.substr(5), knowledge::EvalSettings::DELAY_EXPAND).to_integer();

      ++result;
    }
    else if(utility::begins_with(logic, "exit"))
    {
      // split by space and discard first token(add_prefix)
      std::vector<std::string> splitters, tokens, pivot_list;
      splitters.push_back(" ");

      utility::tokenizer(logic, splitters, tokens, pivot_list);
      int exit_code = 0;

      if(tokens.size() == 2)
      {
        std::stringstream buffer(tokens[1]);
        buffer >> exit_code;
      }

      if(debug)
      {   
        std::cout << "\n";
        std::cout << last_toi << ": Event trigger: exiting app with code ";
        std::cout << exit_code << "\n";
        std::cout << "\n";
      }

      exit(exit_code);
    }
    else if(utility::begins_with(logic, "help"))
    {
      // format: help
      std::cout << "\n";
      std::cout << last_toi << ": Executing help... commands available:\n\n";
      
      std::cout << "  add_prefix arg: add a prefix limiter to prints\n";
      std::cout << "  clear_prefixes: clears all prefix limiters\n";
      std::cout << "        eval arg: evaluates logic in current state\n";
      std::cout << "  eval_stats arg: evaluates logic in current stats\n";
      std::cout << "       exit code: exit the application with a code\n";
      std::cout << "            help: print this menu\n";
      std::cout << "   list_prefixes: lists all prefix limiters\n";
      std::cout << "           print: print current knowledge\n";
      std::cout << "     print_stats: print current var stats\n";
      std::cout << "       print_all: print current knowledge & var stats\n";
      std::cout << "            quit: leave interactive shell\n";
      std::cout << "           shell: enter interactive shell\n\n";

      ++result;
    }
    else if(utility::begins_with(logic, "list_prefixes"))
    {
      // format: list_prefixes
      if(debug)
      {
        std::cout << "\n";
        std::cout << last_toi << ": Event trigger: list all prefixes\n\n";
        std::cout << "\n";
      }

      for(auto prefix: print_prefixes)
      {
        std::cout << "  " << prefix << "\n";
      }

      ++result;
    }
    // note that there is a really good reason to have this before print
    else if(utility::begins_with(logic, "print_stats"))
    {
      // format: print_stats
      if(debug)
      {
        std::cout << "\n";
        std::cout << last_toi << ": Event trigger: printing stats\n";
        std::cout << "\n";
      }

      print_variables(variables, std::cout, false, true, false, stats);

      ++result;
    }
    // note that there is a really good reason to have this before print
    else if(utility::begins_with(logic, "print_all"))
    {
      // format: print_all
      if(debug)
      {
        std::cout << "\n";
        std::cout << last_toi << ": Event trigger: printing all info\n";
        std::cout << "\n";
      }

      print_variables(variables, std::cout, true, true, false, stats);

      ++result;
    }
    else if(utility::begins_with(logic, "print"))
    {
      // format: print
      if(debug)
      {
        std::cout << "\n";
        std::cout << last_toi << ": Event trigger: printing knowledge\n";
        std::cout << "\n";
      }

      print_variables(variables, std::cout, true, false, false, stats);

      ++result;
    }
    else if(utility::begins_with(logic, "shell"))
    {
      // format: shell
      if(!shell_mode)
      {
        if(debug)
        {
          std::cout << "\n";
          std::cout << last_toi << ": Event trigger: entering interactive mode\n";
          std::cout << "\n";
        }

        shell_mode = true;
      }
      else
      {
        if(debug)
        {
          std::cout << "\n";
          std::cout << last_toi << ": You're already in interactive mode\n";
          std::cout << "\n";
        }
      }

      ++result;
    }
    else if(utility::begins_with(logic, "quit"))
    {
      // format: quit
      if(debug)
      {
        std::cout << "\n";
        std::cout << last_toi << ": Event trigger: quitting shell mode\n";
        std::cout << "\n";
      }

      shell_mode = false;

      ++result;
    }
    else
    {
      std::cout << "\n";
      std::cout << last_toi <<
        ": Event trigger: unknown event " << logic << "\n";
      std::cout << "\n";
    }

    // if we are in shell mode, ask the user what to do
    if(shell_mode)
    {
      std::cout << "What would you like to do?(help for usage info): ";
      std::getline(std::cin, logic);
    }
  }
  return result;
}

/**
 * Class for keeping track of batch requests
 **/
class Event
{
  public:
  uint64_t trigger = 0;
  std::vector <std::string> logics;
  int64_t result = 0;
  bool fired = false;
  uint64_t triggered_toi = 0;

  // read an event from a string
  inline void read(const std::string & input)
  {
    std::vector <std::string> splitters, tokens, pivot_list;
    splitters.push_back(":");
    splitters.push_back(";");
    splitters.push_back(",");

    utility::tokenizer(input, splitters, tokens, pivot_list);

    // if tokens are 2+, then we have a timestamp:event
    if(tokens.size() > 1)
    {
      double offset;
      std::stringstream buffer(tokens[0]);
      buffer >> offset;

      // save the timestamp as an offset from first_toi(we have to fix later)
      trigger = utility::seconds_to_nanoseconds(offset);

      // the rest of the tokens are logics
      logics.insert(logics.end(), tokens.begin() + 1, tokens.end());
    }
  }

  // evaluate the logics
  inline void evaluate(
    knowledge::KnowledgeBase kb, knowledge::KnowledgeBase stats,
    const VariableUpdates & variables)
  {
    fired = true;
    triggered_toi = last_toi;
    
    for(size_t i = 0; i < logics.size(); ++i)
    {
      result += process_command(logics[i], kb, stats, variables);;
    }
  }

  // evaluate logic if appropriate time
  inline bool evaluate_if_ready(uint64_t current,
    knowledge::KnowledgeBase kb, knowledge::KnowledgeBase stats,
    const VariableUpdates & variables)
  {
    if(!fired && current >= trigger)
    {
      evaluate(kb, stats, variables);
      return true;
    }
    else
    {
      return false;
    }
  }

  inline void print(std::ostream & output)
  {
    if(logics.size() > 0)
    {
      output << "  ";
      if(trigger == 0)
      {
        output << trigger << " ns: ";
      }
      else
      {
        output <<(trigger - first_toi) << " ns: ";
      }

      output << logics[0];
      for(size_t i = 1; i < logics.size(); ++i)
      {
        output << ";" << logics[i]; 
      }

      output << ": ";

      if(fired)
      {
        output << "fired_at=" << triggered_toi << ", ";
        output << "result=" << result << "\n";
      }
      else
      {
        output << "not fired yet\n";
      }
    }
    else
    {
      output << "ERROR: empty event\n";
    } 
  }

  inline void update_from_first(void)
  {
    trigger += first_toi;
  }
};

bool operator<(const Event & lhs, const Event & rhs)
{
  return lhs.trigger < rhs.trigger;
}

// batch evaluations
std::string batchfile;
std::string batch;
std::vector <Event> events;

// handle command line arguments
void handle_arguments(int argc, const char** argv, size_t recursion_limit = 10)
{
  for(int i = 1; i < argc; ++i)
  {
    std::string arg1(argv[i]);

    if(arg1 == "-b" || arg1 == "--batch")
    {
      if(i + 1 < argc)
      {
        if(batch != "")
        {
          batch += "\n";
          batch += argv[i + 1];
        }
        else
        {
          batch = argv[i + 1];
        }
        std::cout << "  Loading batch events " << batch << "\n";
      }

      ++i;
    }
    else if(arg1 == "-bf" || arg1 == "--batch-file")
    {
      if(i + 1 < argc)
      {
        batchfile = argv[i + 1];
        std::cout << "  Loading batch events from file " << batchfile << "\n";
      }

      ++i;
    }
    else if(arg1 == "-c" || arg1 == "--check")
    {
      if(i + 1 < argc)
      {
        check = argv[i + 1];
        std::cout << "  Loading stats check from command line " << check
                  << "\n";
      }

      ++i;
    }
    else if(arg1 == "-chf" || arg1 == "--check-file")
    {
      if(i + 1 < argc)
      {
        checkfile = argv[i + 1];
        std::cout << "  Loading stats check from file " << checkfile << "\n";
      }

      ++i;
    }
    else if(arg1 == "-cf" || arg1 == "--config-file")
    {
      madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_TRACE,
          "Found user config file flag, param: %s\n", argv[i + 1]);
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
    else if(arg1 == "-f" || arg1 == "--logfile")
    {
      if(i + 1 < argc)
      {
        load_checkpoint_settings.filename = argv[i + 1];

        if(debug)
        {
          std::cout << "  Loading file " << argv[i + 1] << "\n";
        }
      }

      ++i;
    }
    else if(arg1 == "-g" || arg1 == "--debug")
    {
      debug = true;
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
    else if(arg1 == "-ks" || arg1 == "--print-stats")
    {
      print_stats = true;
    }
    else if(arg1 == "-l" || arg1 == "--level")
    {
      if(i + 1 < argc)
      {
        int level;
        std::stringstream buffer(argv[i + 1]);
        buffer >> level;
        logger::global_logger->set_level(level);

        if(debug)
        {
          std::cout << "  Setting log level to " << level << "\n";
        }
      }

      ++i;
    }
    else if(arg1 == "-lcp" || arg1 == "--load-checkpoint-prefix")
    {
      if(i + 1 < argc)
      {
        load_checkpoint_settings.prefixes.push_back(argv[i + 1]);

        if(debug)
        {
          std::cout << "  Limiting load to prefix " << argv[i + 1] << "\n";
        }
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

        if(debug)
        {
          std::cout << "  Setting load size to " << argv[i + 1] << "\n";
        }
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
    else if(arg1 == "-ns" || arg1 == "--no-summary")
    {
      summary = false;
    }
    else if(arg1 == "-s" || arg1 == "--save")
    {
      if(i + 1 < argc)
      {
        save_file = argv[i + 1];

        if(debug)
        {
          std::cout << "  Saving results to " << argv[i + 1] << "\n";
        }
      }

      ++i;
    }
    else if(arg1 == "-v" || arg1 == "--version")
    {
      madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_ALWAYS,
          "MADARA version: %s\n", utility::get_version().c_str());
    }
    else
    {
      madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_ALWAYS,
          "\nOption %s:\n"
          "\nProgram summary for %s [options] [Logic]:\n\n"
          "Inspects STK for summary info on knowledge updates.\n\noptions:\n"
          "  [-b|--batch commands]    load batch events in the format of\n"
          "                           time: event; event; event \n"
          "                           where time is in seconds and event \n"
          "                           can be any arbitrary karl expression \n"
          "                           or the following specialized calls \n"
          "\n"
          "                           add_prefix: add prefixes to print list\n"
          "                             prefixes are separated by spaces\n"
          "                           clear_prefixes: clear print prefixes\n"
          "                           eval: evaluates karl logic in kb\n"
          "                           eval_stats: evaluates karl in stats\n"
          "                           exit: exits this program immediately\n"
          "                           help: print all commands\n"
          "                           list_prefixes: list print prefixes\n"
          "                           print: print var=value \n"
          "                           print_stats: print var stats \n"
          "                           print_all: print var value & stats\n"
          "                           shell: enter shell mode(interactive)\n"
          "                           quit: leaves shell mode\n"
          "\n"
          "  [-bf|--batch-file file]  load batch events from file. See\n"
          "                           -b for format \n"
          "  [-f|--stk-file file]     STK file to load and analyze\n"
          "  [-h|--help]              print help menu(i.e., this menu)\n"
          "  [-c|--check logic]       logic to evaluate to check contents.\n"
          "                           a check is a KaRL expression that can\n"
          "                           be a combination of variable stats.\n"
          "                           Each variable has the following stats:.\n"
          "                             {var}.bytes: total bytes\n"
          "                             {var}.duration: ns between first and "
          "last\n"
          "                             {var}.first: ns timestamp of first "
          "update\n"
          "                             {var}.hz: frequency of updates in "
          "duration\n"
          "                             {var}.kbs: KB/s of updates within "
          "duration\n"
          "                             {var}.last: ns timestamp of first "
          "update\n"
          "                             {var}.max_size: max size of update\n"
          "                             {var}.min_size: min size of update\n"
          "                             {var}.updates: total num updates\n"
          "  [-chf|--check-file file] KaRL file with check. See -c for "
          "options\n"
          "  [-cf|--config-file]      Config file full path, file contains "
          "cmd line\n"
          "                           flags, also uses default config file\n"
          "                           $(HOME)/.madara/stk_inspect.cfg\n"
          "  [-g|--debug]             print debug information\n"
          "  [-k|--print-knowledge]   print final knowledge\n"
          "  [-kp|--print-prefix pfx] filter prints by prefix. Can be "
          "multiple.\n"
          "  [-kpl|--print-prefix-list file] filter prints by prefix. The "
          "file is a \n"
          "                           new-line delimited prefix list that "
          "can be read\n"
          "                           by NamedVectorCombinator.\n"
          "  [-ks|--print-stats]      print stats knowledge base contents\n"
          "  [-l|--level level]       the logger level(0+, higher is higher "
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
          "  [-n|--capnp tag:msg_type] register tag with given message "
          "schema.\n"
          "                            See also -nf and -ni.\n"
          "  [-nf|--capnp-file]       load capnp file. Must appear after -n "
          "and -ni.\n"
          "  [-ni|--capnp-import ]    add directory to capnp directory "
          "imports.\n"
          "                           Must appear before all -n and -nf.\n"
          "  [-ns|--no-summary ]      do not print or save summary stats per "
          "var\n"
          "  [-s|--save file]         save the results to a file\n"
          "\n",
          arg1.c_str(), argv[0]);
      exit(0);
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

  // load the a config file
  std::ifstream file(full_path.c_str());
  if(!file)
  {
    madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_ERROR,
        "Unable to open config file: %s\n", full_path.c_str());
    return false;
  }

  // load progam path for first arg
  argv_config_files.push_back("");

  madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_TRACE,
      "Found config file: %s\n", full_path.c_str());

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
          flag_param.length() -(flag_param.find_first_of(" ") + 1));
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
//  argvp_config_files.push_back("");

  std::transform(argv_config_files.begin(), argv_config_files.end(),
      std::back_inserter(argvp_config_files),
      [](const std::string &s) { return s.c_str(); });

  handle_arguments(argvp_config_files.size(), argvp_config_files.data(),
      recursion_limit - 1);

  return true;
}

void iterate_stk(
  knowledge::KnowledgeBase kb, knowledge::KnowledgeBase stats,
  VariableUpdates & variables)
{
  if(debug)
  {
    std::cout << "Creating CheckpointReader with file contents... " << std::flush;
  }

  knowledge::CheckpointReader reader(load_checkpoint_settings);

  if(debug)
  {
    std::cout << "done\n";
  }

  containers::FlexMap stats_tois("STK_INSPECT.TOI", stats);
  containers::FlexMap stats_ooo = stats_tois["OUT_OF_ORDER"];
  containers::FlexMap stats_ooo_max = stats_ooo["MAX"];

  int out_of_orders = 0;
  uint64_t max_out_of_order = 0;
  uint64_t zeros = 0;
  uint64_t consecutive_toi_ooo = 0;
  std::string last_variable = "";
  std::string last_max_toi = "";

  size_t cur_event = 0;

  if(debug)
  {
    std::cout << "Iterating through updates... " << std::flush;
  }

  while(true)
  {
    // iterate through events and evaluate if they are ready
    for(; cur_event < events.size() &&
      events[cur_event].evaluate_if_ready(
        last_toi, kb, stats, variables); ++cur_event)
    {
      if(debug)
      {
        std::cout << "Event " << cur_event << ": Triggered\n";
      }
    }

    auto cur = reader.next();
    if(cur.first == "")
      break;

    VariableStats& variable = variables[cur.first];

    uint64_t size = cur.second.size();

    if(cur.second.is_integer_type())
    {
      size *= sizeof(int64_t);
    }
    else if(cur.second.is_double_type())
    {
      size *= sizeof(double);
    }

    // first update? initialize the variable
    if(variable.name == "")
    {
      variable.name = cur.first;
      variable.first = cur.second.toi();
      variable.min_size = size;
      variable.max_size = size;

      // update the first toi if it has not be set
      if(first_toi == 0)
      {
        first_toi = variable.first;

        // update events to 
        for(size_t i = 0; i < events.size(); ++i)
        {
          events[i].update_from_first();
        }
      }
    }
    // update min/max for a variable that has already been initialized
    else
    {
      if(variable.min_size > size)
      {
        variable.min_size = size;
      }
      if(variable.max_size < size)
      {
        variable.max_size = size;
      }
    }

    // common updates to each variable
    variable.last = cur.second.toi();
    variable.bytes += size;
    ++variable.updates;
    variable.value = cur.second;

    // we normally don't save the value into the KB, but if we're in
    // batch mode, people can evaluate arbitrary commands that would
    // depend on the KB having values in it
    if(events.size() > 0)
    {
      kb.set(
        cur.first, cur.second, knowledge::EvalSettings::DELAY_NO_EXPAND);
    }

    // error checking: bad tois
    if(last_toi > variable.last)
    {
      ++out_of_orders;
      ++consecutive_toi_ooo;
      if(max_out_of_order < last_toi - variable.last)
      {
        max_out_of_order = last_toi - variable.last;
        stats_ooo_max["name"] = last_max_toi;
        stats_ooo_max["diff"] =(int64_t)max_out_of_order;
        stats_ooo_max["toi"] =(int64_t)last_toi;
        stats_ooo_max["consecutive_violations"] =(int64_t)consecutive_toi_ooo;
      }

      if(variable.last == 0)
      {
        ++zeros;
      }

      // keep track of the out-of-orders per variable
      containers::FlexMap var_entry = stats_ooo[cur.first];
      containers::FlexMap after_entry = var_entry["after"][last_variable];

      var_entry = var_entry.to_integer() + 1;
      after_entry = after_entry.to_integer() + 1;
    }
    else
    {
      last_toi = variable.last;
      last_max_toi = cur.first;
      consecutive_toi_ooo = 0;
    }

    // save for usage in out-of-order info
    last_variable = cur.first;
  }
  if(debug)
  {
    std::cout << "done\n";
  }

  if(out_of_orders > 0)
  {
    std::cout << "Out of order tois=" << out_of_orders << 
      ": max time ooo=" << max_out_of_order << ": zeros=" << zeros << "\n";
  }

  stats_ooo =(int64_t)out_of_orders;
}

void create_events(void)
{
  std::vector<std::string> batch_lines = utility::string_to_vector(
    utility::file_to_string(batchfile));
  
  std::vector<std::string> extra_lines = utility::string_to_vector(batch);

  batch_lines.insert(
    batch_lines.end(), extra_lines.begin(), extra_lines.end());
  
  for(auto line : batch_lines)
  {
    Event event;
    event.read(line);
    events.push_back(event);
  }

  std::sort(events.begin(), events.end());

  if(debug)
  {
    std::cout << "Batch events:\n";
  }

  for(auto event : events)
  {
    event.print(std::cout);
  }
}

int main(int argc, char** argv)
{
  // load and incorporate the defaul config file arguments, but only once
  // and first
  madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_TRACE,
      "Attempting to load default config...\n");

  if(utility::file_exists(default_config_file))
  {
    load_config_file(default_config_file);
  }

  if(debug)
  {
    std::cout << "Inspection settings:\n" << std::flush;
  }

  // handle all user arguments
  handle_arguments(argc,(const char**)argv);

  knowledge::KnowledgeBase kb;
  knowledge::KnowledgeBase stats;
  VariableUpdates variables;

  create_events();
  iterate_stk(kb, stats, variables);

  if(print_stats || summary || checkfile != "" || check != "")
  {
    if(save_file != "")
    {
      if(debug)
      {
        std::cout << "Saving results to " << save_file << "..." << std::flush;
      }
      std::ofstream output(save_file);

      if(output)
      {
        print_variables(variables, output, false, summary,
          print_stats || check != "" || checkfile != "", stats);
      }

      output.close();

      if(debug)
      {
        std::cout << " done\n";
      }
    }
    else
    {
      // user has specified prefixes that must be matched
      if(summary && debug)
      {
        std::cout << "Printing results to stdout...\n" << std::flush;
      }

      if((print_stats || check != "" || checkfile != "") && debug)
      {
        std::cout << "Calculating stats...\n" << std::flush;
      }

      // iterate through all variables and check prefixes
      print_variables(variables, std::cout, false, summary,
        print_stats || check != "" || checkfile != "", stats);
    }
  }

  // see if the user wants to check for variable stats
  if(checkfile != "")
  {
    if(check == "")
    {
      check = utility::file_to_string(checkfile);
    }
    else
    {
      check += "&&(";
      check += utility::file_to_string(checkfile);
      check += ")";
    }
  }

  if(print_stats)
  {
    std::cout << "Printing stats:\n";
    
    print_variables(variables, std::cout, false, true, false, stats);
  }

  if(print_knowledge)
  {
    std::cout << "Printing final KB:\n";

    print_variables(variables, std::cout, true, false, false, stats);
  }

  if(events.size() > 0)
  {
    if(debug)
    {
      std::cout << "\nPrinting batch events:\n";
    }

    for(auto event: events)
    {
      event.print(std::cout);
    }

    if(debug)
    {
      std::cout << "\n";
    }
  }

  // return value is always 0 unless check is specified
  if(check == "" || stats.evaluate(check).is_true())
  {
    if(check != "")
    {
      std::cout << "Result: SUCCESS\n";
    }

    return 0;
  }
  else
  {
    std::cout << "Result: FAIL\n";

    return 1;
  }
}
