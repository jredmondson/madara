
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

#include "madara/knowledge/KnowledgeBase.h"
#include "madara/threads/Threader.h"

#include "madara/utility/Utility.h"
#include "madara/filters/GenericFilters.h"
#include "madara/logger/GlobalLogger.h"

namespace knowledge = madara::knowledge;
namespace transport = madara::transport;
namespace utility = madara::utility;
namespace filters = madara::filters;
namespace logger = madara::logger;
namespace threads = madara::threads;

typedef  std::vector <std::string>  StringVector;

// default transport settings
std::string host ("");
const std::string default_multicast ("239.255.0.1:4150");
transport::QoSTransportSettings settings;

// initial logic is empty
std::string logic;

// filename to save knowledge base as KaRL to
std::string save_location;

// list of filenames
StringVector filenames;
StringVector initfiles;
StringVector initlogics;

// print debug information
bool debug (false);
bool print_knowledge (false);
bool print_knowledge_frequency (false);
bool after_wait (false);
bool check_result (false);

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
        "  [-ky]                    print knowledge after frequent evaluations\n" \
        "  [-l|--level level]       the logger level (0+, higher is higher detail)\n" \
        "  [-m|--multicast ip:port] the multicast ip to send and listen to\n" \
        "  [-o|--host hostname]     the hostname of this process (def:localhost)\n" \
        "  [-q|--queue-length size] size of network buffers in bytes\n" \
        "  [-r|--reduced]           use the reduced message header\n" \
        "  [-s|--save file]         save the resulting knowledge base as karl\n" \
        "  [-t|--time time]         time to wait for results\n" \
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
        "                           this karl agent will not see them.s\n" \
        "  [-0|--init-logic logic]  logic containing initial variables (only ran once)\n" \
        "  [-0f|--init-file file]   file containing initial variables (only ran once)\n" \
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
    else if (arg1 == "-k" || arg1 == "--print-knowledge")
    {
      print_knowledge = true;
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
            "\nReading logic from file %s:\n\n" \
            "\n",
            filename.c_str ());
        }

        initfiles.push_back (filename);
      }

      ++i;
    }
    else if (logic == "")
    {
      logic = arg1;
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
      knowledge_->print ();
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

  // create a knowledge base and setup our id
  knowledge::KnowledgeBase knowledge (host, settings);

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
    knowledge.print_knowledge ();
  }

  if (save_location.size () > 0)
  {
    knowledge.save_as_karl (save_location);
  }

  return 0;
}
