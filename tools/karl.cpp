
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <assert.h>

#include "madara/knowledge/KnowledgeBase.h"
#include "madara/threads/Threader.h"

#include "madara/utility/Utility.h"
#include "madara/filters/GenericFilters.h"
#include "madara/logger/GlobalLogger.h"

// convenience namespaces and typedefs
namespace knowledge = madara::knowledge;
namespace transport = madara::transport;
namespace utility = madara::utility;
namespace filters = madara::filters;
namespace logger = madara::logger;
namespace threads = madara::threads;

typedef knowledge::KnowledgeRecord::Integer  Integer;
typedef std::vector <std::string>            StringVector;

// default transport settings
std::string host ("");
const std::string default_multicast ("239.255.0.1:4150");
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

// filename to save knowledge base as binary to
std::string save_binary;

// knowledge base var name to save checkpoint timestamp to
std::string  meta_prefix;

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
std::string  initbinaries;

// print debug information
bool debug (false);
bool print_knowledge (false);
bool print_knowledge_frequency (false);
bool after_wait (false);
bool check_result (false);
bool use_id (false);

// wait information
bool waiting (false), waiting_for_periodic (false);
double wait_time (0.0);
double wait_for_periodic (0.0);
double frequency (-1.0);

// handle command line arguments
void handle_arguments (int argc, char ** argv)
{
  for (int i = 1; i < argc; ++i)
  {
    std::string arg1 (argv[i]);

    if (arg1 == "-a" || arg1 == "--after-wait")
    {
      after_wait = true;
    }
    else if (arg1 == "-b" || arg1 == "--broadcast")
    {
      if (i + 1 < argc)
      {
        settings.hosts.push_back (argv[i + 1]);
        settings.type = transport::BROADCAST;
      }
      ++i;
    }
    if (arg1 == "-c" || arg1 == "--check-result")
    {
      check_result = true;
    }
    else if (arg1 == "-d" || arg1 == "--domain")
    {
      if (i + 1 < argc)
        settings.write_domain = argv[i + 1];

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
        "  [-a|--after-wait]        Evaluate after wait, rather than before wait\n" \
        "  [-b|--broadcast ip:port] the broadcast ip to send and listen to\n" \
        "  [-c|--check-result]      check result of eval. If not zero, then terminate\n" \
        "  [-d|--domain domain]     the knowledge domain to send and listen to\n" \
        "  [--debug]                print all sent, received, and final knowledge\n" \
        "  [-f|--logfile file]      log to a file\n" \
        "  [-h|--help]              print help menu (i.e., this menu)\n" \
        "  [-i|--input file]        file containing MADARA logic to evaluate\n" \
        "  [-k|--print-knowledge]   print final knowledge\n" \
        "  [-kp]--print-prefix      filter prints by prefix. Can be multiple.\n" \
        "  [-ky]                    print knowledge after frequent evaluations\n" \
        "  [-l|--level level]       the logger level (0+, higher is higher detail)\n" \
        "  [-lcp|--load-checkpoint-prefix prfx]\n" \
        "                           prefix of knowledge to load from checkpoint\n" \
        "  [-ls|--load-size bytes]\n" \
        "                           size of buffer needed for file load\n" \
        "  [-lt|--load-transport file] a file to load transport settings from\n" \
        "  [-ltp|--load-transport-prefix prfx] prefix of saved settings\n" \
        "  [-ltt|--load-transport-text file] a text file to load transport settings from\n" \
        "  [-m|--multicast ip:port] the multicast ip to send and listen to\n" \
        "  [-o|--host hostname]     the hostname of this process (def:localhost)\n" \
        "  [-q|--queue-length size] size of network buffers in bytes\n" \
        "  [-r|--reduced]           use the reduced message header\n" \
        "  [-s|--save file]         save the resulting knowledge base as karl\n" \
        "  [-sb|--save-binary file] save the resulting knowledge base as a\n" \
        "                           binary checkpoint\n" \
        "  [-scp|--save-checkpoint-prefix prfx]\n" \
        "                           prefix of knowledge to save in checkpoint\n" \
        "  [-ss|--save-size bytes]\n" \
        "                           size of buffer needed for file saves\n" \
        "  [-sj|--save-json file]   save the resulting knowledge base as JSON\n" \
        "  [-st|--save-transport file] a file to save transport settings to\n" \
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
            "\nReading logic from file %s:\n",
            filename.c_str ());
        }

        filenames.push_back (filename);
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
          print_prefixes.push_back (argv[j]);
      }
    }
    else if (arg1 == "-ky")
    {
      print_knowledge_frequency = true;
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
    else if (arg1 == "-lcp" || arg1 == "--load-checkpoint-prefix")
    {
      if (i + 1 < argc)
      {
        load_checkpoint_settings.prefixes.push_back (argv[i + 1]);
      }

      ++i;
    }
    else if (arg1 == "-ls" || arg1 == "--load-size")
    {
      if (i + 1 < argc)
      {
        std::stringstream buffer (argv[i + 1]);
        buffer >> load_checkpoint_settings.buffer_size;
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
      madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_ALWAYS,
        "MADARA version: %s\n",
        utility::get_version ().c_str ());
    }
    else if (arg1 == "-w" || arg1 == "--wait"
          || arg1 == "-t" || arg1 == "--time")
    {
      if (i + 1 < argc)
      {
        waiting = true;
        std::stringstream buffer (argv[i + 1]);
        buffer >> wait_time;
      }
      ++i;
    }
    else if (arg1 == "-wy" || arg1 == "-wp" ||
             arg1 == "--wait-for-periodic")
    {
      if (i + 1 < argc)
      {
        waiting_for_periodic = true;
        std::stringstream buffer (argv[i + 1]);
        buffer >> wait_for_periodic;
      }
      ++i;
    }
    else if (arg1 == "-y" || arg1 == "--frequency")
    {
      if (i + 1 < argc)
      {
        std::stringstream buffer (argv[i + 1]);
        buffer >> frequency;
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
    else if (arg1 == "-0" || arg1 == "--init-logic")
    {
      if (i + 1 < argc)
      {
        initlogics.push_back (argv[i + 1]);
      }

      ++i;
    }
    else if (arg1 == "-0f" || arg1 == "--init-file")
    {
      if (i + 1 < argc)
      {
        std::string filename;
        std::stringstream buffer (argv[i + 1]);
        buffer >> filename;

        if (debug)
        {
          madara_logger_ptr_log (logger::global_logger.get (), logger::LOG_ALWAYS,
            "\nReading logic from file %s:\n",
            filename.c_str ());
        }

        initfiles.push_back (filename);
      }

      ++i;
    }
    else if (arg1 == "-0b" || arg1 == "--init-bin")
    {
      if (i + 1 < argc)
      {
        initbinaries = argv[i + 1];

        if (debug)
        {
          madara_logger_ptr_log (logger::global_logger.get (), logger::LOG_ALWAYS,
            "Will be reading binary checkpoint from file %s:\n",
            initbinaries.c_str ());
        }
      }

      ++i;
    }
    else if (arg1 == "--meta-prefix")
    {
      if (i + 1 < argc)
      {
        meta_prefix = argv[i + 1];

        if (debug)
        {
          madara_logger_ptr_log (logger::global_logger.get (), logger::LOG_ALWAYS,
            "Will be saving checkpoint meta data to to prefix %s:\n",
            meta_prefix.c_str ());
        }
      }

      ++i;
    }
    else if (arg1 == "--use-id")
    {
      use_id = true;
    }
    else if (logic == "")
    {
      logic = arg1;
    }
  }
}

void print_all_prefixes (knowledge::KnowledgeBase & context)
{
  madara_logger_ptr_log (logger::global_logger.get (),
    logger::LOG_ALWAYS,
      "\nKnowledge in Knowledge Base (filtered by prefixes):\n");
  for (size_t i = 0; i < print_prefixes.size (); ++i)
  {
    knowledge::KnowledgeMap matches =
      context.to_map (print_prefixes[i]);

    for (knowledge::KnowledgeMap::const_iterator j = matches.begin ();
      j != matches.end (); ++j)
    {
      madara_logger_ptr_log (logger::global_logger.get (),
        logger::LOG_ALWAYS, "%s=%s\n",
        j->first.c_str (), j->second.to_string (", ").c_str ());
    }
  }
}

class Evaluator : public threads::BaseThread
{
public:
  Evaluator (knowledge::KnowledgeBase & knowledge,
    std::vector <knowledge::CompiledExpression> & expressions)
  : knowledge_ (&knowledge), expressions_ (expressions)
  {

  }

  virtual void run (void)
  {
    for (size_t i = 0; i < expressions_.size (); ++i)
    {
#ifndef _MADARA_NO_KARL_
      knowledge::KnowledgeRecord result = knowledge_->evaluate (expressions_[i]);

      if (check_result && result.is_true ())
      {
        this->terminated = 1;
      }

#endif // _MADARA_NO_KARL_
    }

    if (print_knowledge_frequency)
    {
      if (print_prefixes.size () == 0)
      {
        knowledge_->print ();
      }
      else
      {
        print_all_prefixes (*knowledge_);
      }
    }
  }

private:
  knowledge::KnowledgeBase * knowledge_;
  std::vector <knowledge::CompiledExpression> & expressions_;
};

int main (int argc, char ** argv)
{
  // handle all user arguments
  handle_arguments (argc, argv);

  if (debug)
  {
    settings.add_send_filter (filters::log_aggregate);
    settings.add_receive_filter (filters::log_aggregate);
  }

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
  knowledge::KnowledgeBase knowledge (host, settings);

  if (initbinaries != "")
  {
    madara_logger_ptr_log (logger::global_logger.get (), logger::LOG_MAJOR,
      "\nReading binary checkpoint from file %s:\n",
      initbinaries.c_str ());

    madara::knowledge::EvalSettings silent (true, true, true, true, true);
    //madara::knowledge::FileHeader meta;
    load_checkpoint_settings.filename = initbinaries;
    knowledge.load_context (load_checkpoint_settings);

    if (meta_prefix != "")
    {
      knowledge.set (meta_prefix + ".originator",
        load_checkpoint_settings.originator, silent);
      knowledge.set (meta_prefix + ".version",
        load_checkpoint_settings.version, silent);
      knowledge.set (meta_prefix + ".last_timestamp",
        (Integer)load_checkpoint_settings.last_timestamp, silent);
      knowledge.set (meta_prefix + ".initial_timestamp",
        (Integer)load_checkpoint_settings.initial_timestamp, silent);
      knowledge.set (meta_prefix + ".last_lamport_clock",
        (Integer)load_checkpoint_settings.last_lamport_clock, silent);
      knowledge.set (meta_prefix + ".initial_lamport_clock",
        (Integer)load_checkpoint_settings.initial_lamport_clock, silent);
      knowledge.set (meta_prefix + ".states",
        (Integer)load_checkpoint_settings.states, silent);
      knowledge.set (meta_prefix + ".current_timestamp",
        (Integer)utility::get_time (), silent);
    }
  }

  // build the expressions to evaluate
  std::vector <knowledge::CompiledExpression> expressions;

  // each file logic is evaluated first
  for (StringVector::const_iterator i = filenames.begin ();
    i != filenames.end (); ++i)
  {
    if (utility::file_exists (*i))
    {
      expressions.push_back (knowledge.compile (utility::file_to_string (*i)));
    }
    else
    {
      madara_logger_ptr_log (logger::global_logger.get (), logger::LOG_ALWAYS,
        "\nUnable to load file %s. "
        "Check that file exists and that you have permission to read.\n",
        i->c_str ());
    }
  }

  // use no harm settings for initialization (faster, doesn't send data)
  knowledge::EvalSettings noharm;
  noharm.treat_globals_as_locals = true;
  noharm.signal_changes = false;

  // set initialization variables from files into the knowledge base
  if (initfiles.size () > 0)
  {
    for (StringVector::const_iterator i = initfiles.begin ();
      i != initfiles.end (); ++i)
    {
      if (utility::file_exists (*i))
      {
        knowledge.evaluate (utility::file_to_string (*i), noharm);
      }
      else
      {
        madara_logger_ptr_log (logger::global_logger.get (), logger::LOG_ALWAYS,
          "\nUnable to load file %s. "
          "Check that file exists and that you have permission to read.\n",
          i->c_str ());
      }
    }
  }

  // set initialization variables from command-line logics
  if (initlogics.size () > 0)
  {
    for (StringVector::const_iterator i = initlogics.begin ();
      i != initlogics.end (); ++i)
    {
      knowledge.evaluate (*i, noharm);
    }
  }

  // command line logics are evaluated last
  if (logic != "")
  {
    expressions.push_back (knowledge.compile (logic));
  }

  // check frequency to see if we should only execute once
  if (frequency < 0)
  {
    if (!after_wait)
    {
      for (size_t i = 0; i < expressions.size (); ++i)
      {
#ifndef _MADARA_NO_KARL_
        knowledge::KnowledgeRecord result =
          knowledge.evaluate (expressions[i]);

        if (check_result && result.is_true ())
        {
          break;
        }
#endif // _MADARA_NO_KARL_
      }
    }

    // if user requests to wait, do so before the debug print
    if (waiting_for_periodic || waiting)
    {
      utility::sleep (wait_time + wait_for_periodic);
    }

    if (after_wait)
    {
      for (size_t i = 0; i < expressions.size (); ++i)
      {
#ifndef _MADARA_NO_KARL_
        knowledge.evaluate (expressions[i]);
#endif // _MADARA_NO_KARL_
      }
    } // if (after_wait)
  } // if (frequency < 0)
  else // frequency >= 0
  {
    threads::Threader threader (knowledge);

    // if the user specified a wait before evaluation, sleep for the time
    if (waiting_for_periodic)
    {
      utility::sleep (wait_for_periodic);
    }

    threader.run (frequency, "evaluator",
      new Evaluator (knowledge, expressions), false);

    bool terminated = false;

    // if user requests to wait, do so before the debug print
    if (waiting)
    {
      knowledge::WaitSettings ws;
      ws.max_wait_time = wait_time;

      terminated = threader.wait (ws);
    }

    if (!terminated)
    {
      threader.terminate ();
      threader.wait ();
    }
  }

  // if the user requests debugging information, print final knowledge
  if (debug || print_knowledge)
  {
    if (print_prefixes.size () == 0)
    {
      knowledge.print ();
    }
    else
    {
      print_all_prefixes (knowledge);
    }
  }

  // save as karl if requested
  if (save_location.size () > 0)
  {
    save_checkpoint_settings.filename = save_location;
    knowledge.save_as_karl (save_checkpoint_settings);
  }

  // save as karl if requested
  if (save_json.size () > 0)
  {
    save_checkpoint_settings.filename = save_json;
    knowledge.save_as_json (save_checkpoint_settings);
  }

  // save as binary if requested
  if (save_binary.size () > 0)
  {
    save_checkpoint_settings.filename = save_binary;
    knowledge.save_context (save_checkpoint_settings);
  }

  return 0;
}

