// non-MADARA includes
#include <memory>
#include <iostream>

// MADARA includes
#include "madara/knowledge/KnowledgeBase.h"
#include "madara/knowledge/CheckpointStreamer.h"
#include "madara/threads/Threader.h"
#include "madara/utility/EpochEnforcer.h"
#include "madara/utility/Utility.h"
#include "madara/knowledge/containers/Integer.h"

// DO NOT DELETE THIS SECTION

// begin thread includes
// end thread includes

// begin transport includes
// end transport includes

// begin filter includes
// end filter includes

// END DO NOT DELETE THIS SECTION

const std::string default_broadcast("192.168.1.255:15000");
// default transport settings
std::string host("");
const std::string default_multicast("239.255.0.1:4150");
madara::transport::QoSTransportSettings settings;

// create shortcuts to MADARA classes and namespaces
namespace utility = madara::utility;
namespace knowledge = madara::knowledge;
typedef knowledge::KnowledgeRecord   Record;
typedef Record::Integer Integer;

// periodicity and run time of app
double hertz = 1.0;
double run_time = 60.0;

// madara commands from a file
std::string madara_commands = "";

// filename to save transport settings to
std::string save_transport;
std::string save_transport_prefix;
std::string save_transport_text;
std::string load_transport_prefix;

// checkpoint settings
knowledge::CheckpointSettings chkpt_settings;
double checkpoint_hertz = 100;

// CHANGE: custom variables for command line options
Integer id = 0;
Integer num_processes = 2;
Integer initial_state = 7;
Integer K = 5;

void print_usage(char * prog_name)
{
  // CHANGE: added -i and -n options
  madara_logger_ptr_log(madara::logger::global_logger.get(),
    madara::logger::LOG_ALWAYS,
"\nProgram summary for %s:\n\n" 
"K State Self-Stabilizing Distributed Test (from Dijkstra 1974):\n\n" 
" [-b |--broadcast ip:port]     the broadcast ip to send and listen to\n" 
" [-c |--checkpoint filename]   save state to a checkpoint file for retrieval\n" 
" [-cz |--checkpoint-hz hz]     maximum periodicity to save checkpoints at\n" 
" [-d |--domain domain]         the knowledge domain to send and listen to\n" 
" [-e |--rebroadcasts num]      number of hops for rebroadcasting messages\n" 
" [-f |--logfile file]          log to a file\n" 
" [-i |--id num]                the identifier of the process (0-(n-1))\n" 
" [-lt|--load-transport file] a file to load transport settings from\n" 
" [-ltp|--load-transport-prefix prfx] prefix of saved settings\n" 
" [-ltt|--load-transport-text file] a text file to load transport settings from\n" 
" [--madara-level level]        the MADARA logger level(0+, higher is higher detail)\n"
" [-k |--states num]            max states (default k=5)\n" 
" [-m |--multicast ip:port]     the multicast ip to send and listen to\n" 
" [-M |--madara-file <file>]    file containing madara commands to execute\n" 
"                               multiple space-delimited files can be used\n" 
" [-n |--num-processes num]     the max number of participating processes\n" 
" [-o |--host hostname]         the hostname of this process(def:localhost)\n" 
" [-q |--queue-length length]   length of transport queue in bytes\n" 
" [-r |--reduced]               use the reduced message header\n" 
" [-st|--save-transport file] a file to save transport settings to\n" 
" [-stp|--save-transport-prefix prfx] prefix to save settings at\n" 
" [-stt|--save-transport-text file] a text file to save transport settings to\n" 
" [-s |--initial-state state]   initial state (default = 7)\n" 
" [-t |--time|--run-time time]  time in seconds to run (e.g., 60.5)\n" 
" [-u |--udp ip:port]           a udp ip to send to(first is self to bind to)\n" 
" [-z |--hertz hertz]           actions per second to perform (e.g., 5.5)\n" 
" [--zmq proto:ip:port]         specifies a 0MQ transport endpoint\n"
"\n",
        prog_name);
  exit(0);
}

// handle command line arguments
void handle_arguments(int argc, char ** argv)
{
  for (int i = 1; i < argc; ++i)
  {
    std::string arg1(argv[i]);

    if (arg1 == "-b" || arg1 == "--broadcast")
    {
      if (i + 1 < argc && argv[i + 1][0] != '-')
      {
        settings.hosts.push_back(argv[i + 1]);
        settings.type = madara::transport::BROADCAST;
      }
      else
      {
        print_usage(argv[0]);
      }

      ++i;
    }
    else if (arg1 == "-c" || arg1 == "--checkpoint")
    {
      if (i + 1 < argc)
      {
        chkpt_settings.filename = argv[i + 1];
      }
      else
      {
        print_usage(argv[0]);
      }

      ++i;
    }
    else if (arg1 == "-cz" || arg1 == "--checkpoint-hz")
    {
      if (i + 1 < argc && argv[i + 1][0] != '-')
      {
        std::stringstream buffer(argv[i + 1]);
        buffer >> checkpoint_hertz;
      }
      else
      {
        print_usage(argv[0]);
      }

      ++i;
    }
    else if (arg1 == "-d" || arg1 == "--domain")
    {
      if (i + 1 < argc && argv[i + 1][0] != '-')
      {
        settings.write_domain = argv[i + 1];
      }
      else
      {
        print_usage(argv[0]);
      }

      ++i;
    }
    else if (arg1 == "-e" || arg1 == "--rebroadcasts")
    {
      if (i + 1 < argc && argv[i + 1][0] != '-')
      {
        int hops;
        std::stringstream buffer(argv[i + 1]);
        buffer >> hops;

        settings.set_rebroadcast_ttl(hops);
        settings.enable_participant_ttl(hops);
      }
      else
      {
        print_usage(argv[0]);
      }

      ++i;
    }
    else if (arg1 == "-f" || arg1 == "--logfile")
    {
      if (i + 1 < argc && argv[i + 1][0] != '-')
      {
        madara::logger::global_logger->add_file(argv[i + 1]);
      }
      else
      {
        print_usage(argv[0]);
      }

      ++i;
    }
    // CHANGE: add -i parameter for configuring ID
    else if (arg1 == "-i" || arg1 == "--id")
    {
      if (i + 1 < argc && argv[i + 1][0] != '-')
      {
        std::stringstream buffer(argv[i + 1]);
        buffer >> id;
      }
      else
      {
        print_usage(argv[0]);
      }

      ++i;
    }
    // CHANGE: add -k parameter for configuring max states
    else if (arg1 == "-k" || arg1 == "--states")
    {
      if (i + 1 < argc && argv[i + 1][0] != '-')
      {
        std::stringstream buffer(argv[i + 1]);
        buffer >> K;

        if (K < 1)
        {
          std::cerr << "Warning: K must be >= 1. Setting K=5.\n";
          num_processes = 5;
        }
      }
      else
      {
        print_usage(argv[0]);
      }

      ++i;
    }
    else if (arg1 == "-lt" || arg1 == "--load-transport")
    {
      if (i + 1 < argc)
      {
        if (load_transport_prefix == "")
          settings.load(argv[i + 1]);
        else
          settings.load(argv[i + 1], load_transport_prefix);
      }
      else
      {
        print_usage(argv[0]);
      }

      ++i;
    }
    else if (arg1 == "-ltp" || arg1 == "--load-transport-prefix")
    {
      if (i + 1 < argc)
      {
        load_transport_prefix = argv[i + 1];
      }
      else
      {
        print_usage(argv[0]);
      }

      ++i;
    }
    else if (arg1 == "-ltt" || arg1 == "--load-transport-text")
    {
      if (i + 1 < argc)
      {
        if (load_transport_prefix == "")
          settings.load_text(argv[i + 1]);
        else
          settings.load_text(argv[i + 1], load_transport_prefix);
      }
      else
      {
        print_usage(argv[0]);
      }

      ++i;
    }
    else if (arg1 == "--madara-level")
    {
      if (i + 1 < argc && argv[i + 1][0] != '-')
      {
        int level = 0;
        std::stringstream buffer(argv[i + 1]);
        buffer >> level;
        madara::logger::global_logger->set_level(level);
      }
      else
      {
        print_usage(argv[0]);
      }

      ++i;
    }
    else if (arg1 == "-m" || arg1 == "--multicast")
    {
      if (i + 1 < argc && argv[i + 1][0] != '-')
      {
        settings.hosts.push_back(argv[i + 1]);
        settings.type = madara::transport::MULTICAST;
      }
      else
      {
        print_usage(argv[0]);
      }

      ++i;
    }
    else if (arg1 == "-M" || arg1 == "--madara-file")
    {
      bool files = false;
      ++i;
      for (;i < argc && argv[i][0] != '-'; ++i)
      {
        std::string filename = argv[i];
        if (madara::utility::file_exists(filename))
        {
          madara_commands += madara::utility::file_to_string(filename);
          madara_commands += ";\n";
          files = true;
        }
      }
      --i;

      if (!files)
      {
        print_usage(argv[0]);
      }
    }
    // CHANGE: add -n parameter for configuring ID
    else if (arg1 == "-n" || arg1 == "--num-processes")
    {
      if (i + 1 < argc && argv[i + 1][0] != '-')
      {
        std::stringstream buffer(argv[i + 1]);
        buffer >> num_processes;

        if (num_processes < 2)
        {
          std::cerr << "Warning: num_processes must be >= 2. Setting n=3.\n";
          num_processes = 3;
        }
      }
      else
      {
        print_usage(argv[0]);
      }

      ++i;
    }
    else if (arg1 == "-o" || arg1 == "--host")
    {
      if (i + 1 < argc && argv[i + 1][0] != '-')
      {
        host = argv[i + 1];
      }
      else
      {
        print_usage(argv[0]);
      }

      ++i;
    }
    else if (arg1 == "-q" || arg1 == "--queue-length")
    {
      if (i + 1 < argc && argv[i + 1][0] != '-')
      {
        std::stringstream buffer(argv[i + 1]);
        buffer >> settings.queue_length;
      }
      else
      {
        print_usage(argv[0]);
      }

      ++i;
    }
    else if (arg1 == "-r" || arg1 == "--reduced")
    {
      settings.send_reduced_message_header = true;
    }
    else if (arg1 == "-s" || arg1 == "--initial-state")
    {
      if (i + 1 < argc)
      {
        std::stringstream buffer(argv[i + 1]);
        buffer >> initial_state;
      }
      else
      {
        print_usage(argv[0]);
      }

      ++i;
    }
    else if (arg1 == "-t" || arg1 == "--time")
    {
      if (i + 1 < argc)
      {
        std::stringstream buffer(argv[i + 1]);
        buffer >> run_time;
      }
      else
      {
        print_usage(argv[0]);
      }

      ++i;
    }
    else if (arg1 == "-st" || arg1 == "--save-transport")
    {
      if (i + 1 < argc)
      {
        save_transport = argv[i + 1];
      }
      else
      {
        print_usage(argv[0]);
      }

      ++i;
    }
    else if (arg1 == "-stp" || arg1 == "--save-transport-prefix")
    {
      if (i + 1 < argc)
      {
        save_transport_prefix = argv[i + 1];
      }
      else
      {
        print_usage(argv[0]);
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
    else if (arg1 == "-t" || arg1 == "--run-time" || arg1 == "--time")
    {
      if (i + 1 < argc && argv[i + 1][0] != '-')
      {
        std::stringstream buffer(argv[i + 1]);
        buffer >> hertz;
      }
      else
      {
        print_usage(argv[0]);
      }

      ++i;
    }
    else if (arg1 == "-u" || arg1 == "--udp")
    {
      if (i + 1 < argc && argv[i + 1][0] != '-')
      {
        settings.hosts.push_back(argv[i + 1]);
        settings.type = madara::transport::UDP;
      }
      else
      {
        print_usage(argv[0]);
      }

      ++i;
    }
    else if (arg1 == "-z" || arg1 == "--hertz")
    {
      if (i + 1 < argc && argv[i + 1][0] != '-')
      {
        std::stringstream buffer(argv[i + 1]);
        buffer >> hertz;
      }
      else
      {
        print_usage(argv[0]);
      }

      ++i;
    }
    else if (arg1 == "--zmq")
    {
      if (i + 1 < argc && argv[i + 1][0] != '-')
      {
        settings.hosts.push_back(argv[i + 1]);
        settings.type = madara::transport::ZMQ;
      }
      else
      {
        print_usage(argv[0]);
      }

      ++i;
    }
    else
    {
      print_usage(argv[0]);
    }
  }
}

// perform main logic of program
int main(int argc, char ** argv)
{
  settings.type = madara::transport::MULTICAST;
 
  // handle all user arguments
  handle_arguments(argc, argv);

  if (settings.hosts.size() == 0)
  {
    // setup default transport as multicast
    settings.hosts.resize(1);
    settings.hosts[0] = default_multicast;
  }
  
  // save transport always happens after all possible transport chagnes
  if (save_transport != "")
  {
    if (save_transport_prefix == "")
      settings.save(save_transport);
    else
      settings.save(save_transport, save_transport_prefix);
  }

  // save transport always happens after all possible transport chagnes
  if (save_transport_text != "")
  {
    if (save_transport_prefix == "")
      settings.save_text(save_transport_text);
    else
      settings.save_text(save_transport_text, save_transport_prefix);
  }

  // create knowledge base
  knowledge::KnowledgeBase kb;

  /*
   * CHANGE: add containers based on id
   * From Dijkstra paper:
   * L: to refer to the state of its lefthand neighbor,
   *    machine nr . (i - 1)mod(N q- 1),
   * S: to refer to the state of itself, machine nr. i,
   * R: to refer to the state of its righth
  */

  // create the state of our self
  Record name ("S");
  name += id;
  knowledge::containers::Integer S (name.to_string(), kb);
  S = initial_state;

  // create the state of our left hand neighbor
  name = "S";
  name += (id + num_processes - 1) % num_processes;
  knowledge::containers::Integer L (name.to_string(), kb);

  // create the state of our right hand neighbor
  name = "S";
  name += (id + 1) % num_processes;
  knowledge::containers::Integer R (name.to_string(), kb);

  std::cerr << "L refers to " << L.get_name() << "\n";
  std::cerr << "S refers to " << S.get_name() << "\n";
  std::cerr << "R refers to " << R.get_name() << "\n";

  // setup checkpoint streaming, if configured on command line
  std::unique_ptr<knowledge::CheckpointStreamer> streamer = 0;

  if (chkpt_settings.filename != "" && checkpoint_hertz > 0)
  {
    std::cerr << "Saving checkpoints to " << chkpt_settings.filename << "\n";

    streamer.reset (new knowledge::CheckpointStreamer(
      chkpt_settings, kb.get_context(), checkpoint_hertz));
    kb.attach_streamer(std::move(streamer));
  }

  // create threader for thread launching and control
  madara::threads::Threader threader (kb);

  // begin on receive filters
  // end on receive filters
  
  // begin on send filters
  // end on send filters
  
  // if you only want to use custom transports, delete following
  kb.attach_transport(host, settings);
  
  // begin transport creation 
  // end transport creation
  
  // read madara initialization
  if (madara_commands != "")
  {
#ifndef _MADARA_NO_KARL_

    kb.evaluate(madara_commands,
      knowledge::EvalSettings(false, true));

#endif  // end karl support
  }

  // begin thread creation
  // end thread creation
  
  /**
   * END WARNING
   **/

  // create an enforcer
  utility::EpochEnforcer<utility::Clock> enforcer(1/hertz, run_time);  

  // main application logic (outside of threads and filters)
  std::cerr << "Initial state:\n";
  kb.print();

  while (!enforcer.is_done ())
  {
    /**
     * CHANGE: Logic from Dijkstra paper
     * for the bottom machine:
     *   if L = S then S = (S+1)mod K fi
     * for the other machines:
     *   if L != S then S = L fi;
     **/
    if (id == 0)
    {
      L == S ? S = (*S + 1) % K : 0;
    }
    else
    {
      L != S ? S = *L : 0;
    }

    // expect communication loss and remodify important knowledge
    S.modify();

    std::cerr << "Modifieds to send: " <<
      kb.save_modifieds().size() << "\n";

    // print knowledge base and send modifieds
    kb.print();
    kb.send_modifieds();

    // sleep until the next execution epoch
    enforcer.sleep_until_next();
  }

  // terminate all threads after the controller
  threader.terminate();
  
  // wait for all threads
  threader.wait();
  
  // print all knowledge values
  kb.print();

  return 0;
}

