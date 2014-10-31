
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
#include "madara/utility/Log_Macros.h"

// default transport settings
std::string host ("");
const std::string default_multicast ("239.255.0.1:4150");
Madara::Transport::QoS_Transport_Settings settings;

std::string filename =
  Madara::Utility::expand_envs (
    "$(MADARA_ROOT)/tests/images/manaus_hotel_900x1500.jpg");

std::string target_location;

Madara::Knowledge_Record::Integer target_id (1);
Madara::Knowledge_Record trusted ("0");
std::string self ("0");

// payload size to burst
unsigned int data_size = 0;
unsigned int data_size_increment = 1000;
unsigned int data_size_end = 0;
double max_wait = 20.0;
bool is_terminator = false;


Madara::Knowledge_Engine::Variable_Reference ack;
Madara::Knowledge_Engine::Compiled_Expression id0_wait;

// keep track of time
ACE_hrtime_t elapsed_time, maximum_time;
ACE_High_Res_Timer timer;

// handle command line arguments
void handle_arguments (int argc, char ** argv)
{
  for (int i = 1; i < argc; ++i)
  {
    std::string arg1 (argv[i]);

    if (arg1 == "-m" || arg1 == "--multicast")
    {
      if (i + 1 < argc)
      {
        settings.hosts.push_back (argv[i + 1]);
        settings.type = Madara::Transport::MULTICAST;
      }
      ++i;
    }
    else if (arg1 == "-b" || arg1 == "--broadcast")
    {
      if (i + 1 < argc)
      {
        settings.hosts.push_back (argv[i + 1]);
        settings.type = Madara::Transport::BROADCAST;
      }
      ++i;
    }
    else if (arg1 == "-u" || arg1 == "--udp")
    {
      if (i + 1 < argc)
      {
        settings.hosts.push_back (argv[i + 1]);
        settings.type = Madara::Transport::UDP;
      }
      ++i;
    }
    else if (arg1 == "-o" || arg1 == "--host")
    {
      if (i + 1 < argc)
        host = argv[i + 1];

      ++i;
    }
    else if (arg1 == "-p" || arg1 == "--payload")
    {
      if (i + 1 < argc)
        filename = argv[i + 1];

      ++i;
    }
    else if (arg1 == "-d" || arg1 == "--domain")
    {
      if (i + 1 < argc)
        settings.domains = argv[i + 1];

      ++i;
    }
    else if (arg1 == "-t" || arg1 == "--target")
    {
      if (i + 1 < argc)
        target_location = argv[i + 1];

      ++i;
    }
    else if (arg1 == "-i" || arg1 == "--id")
    {
      if (i + 1 < argc)
      {
        std::stringstream buffer (argv[i + 1]);
        buffer >> settings.id;
        self = argv[i + 1];
      }

      ++i;
    }
    else if (arg1 == "-l" || arg1 == "--level")
    {
      if (i + 1 < argc)
      {
        std::stringstream buffer (argv[i + 1]);
        buffer >> MADARA_debug_level;
      }

      ++i;
    }
    else if (arg1 == "-f" || arg1 == "--logfile")
    {
      if (i + 1 < argc)
      {
        Madara::Knowledge_Engine::Knowledge_Base::log_to_file (argv[i + 1]);
      }

      ++i;
    }
    else if (arg1 == "-r" || arg1 == "--reduced")
    {
      settings.send_reduced_message_header = true;
    }
    else if (arg1 == "-e" || arg1 == "--rebroadcasts")
    {
      if (i + 1 < argc)
      {
        int hops;
        std::stringstream buffer (argv[i + 1]);
        buffer >> hops;

        settings.set_rebroadcast_ttl (hops);
        settings.enable_participant_ttl (hops);
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
    else if (arg1 == "-z" || arg1 == "--target-id")
    {
      if (i + 1 < argc)
      {
        std::stringstream buffer (argv[i + 1]);
        buffer >> target_id;
      }

      ++i;
    }
    else if (arg1 == "-s" || arg1 == "--size")
    {
      if (i + 1 < argc)
      {
        std::stringstream buffer (argv[i + 1]);
        buffer >> data_size;
      }

      ++i;
    }
    else if (arg1 == "-si" || arg1 == "--size-increment")
    {
      if (i + 1 < argc)
      {
        std::stringstream buffer (argv[i + 1]);
        buffer >> data_size_increment;
      }

      ++i;
    }
    else if (arg1 == "-se" || arg1 == "--size-end")
    {
      if (i + 1 < argc)
      {
        std::stringstream buffer (argv[i + 1]);
        buffer >> data_size_end;
      }

      ++i;
    }
    else if (arg1 == "-st" || arg1 == "--slack-time")
    {
      if (i + 1 < argc)
      {
        std::stringstream buffer (argv[i + 1]);
        buffer >> settings.slack_time;
      }

      ++i;
    }
    else if (arg1 == "-w" || arg1 == "--max-wait")
    {
      if (i + 1 < argc)
      {
        std::stringstream buffer (argv[i + 1]);
        buffer >> max_wait;
      }

      ++i;
    }
    else if (arg1 == "-9" || arg1 == "--terminate")
    {
      is_terminator = true;

      ++i;
    }
    else
    {
      MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_DEBUG, 
"\nProgram summary for %s:\n\n" \
"  Attempts to send a file over the network with a certain number\n" \
"  of rebroadcasts (-e|--rebroadcasts controls the number of rebroadcasts)\n\n" \
" [-b|--broadcast ip:port] the broadcast ip to send and listen to\n" \
" [-d|--domain domain]     the knowledge domain to send and listen to\n" \
" [-e|--rebroadcasts hops] maximum number of rebroadcasts allowed (def 5)\n" \
" [-f|--logfile file]      log to a file\n" \
" [-i|--id id]             the id of this agent (should be non-negative)\n" \
" [-l|--level level]       the logger level (0+, higher is higher detail)\n" \
" [-m|--multicast ip:port] the multicast ip to send and listen to\n" \
" [-o|--host hostname]     the hostname of this process (def:localhost)\n" \
" [-p|--payload filename]  file to use as payload for the message\n" \
" [-q|--queue-length length] length of transport queue in bytes\n" \
" [-r|--reduced]           use the reduced message header\n" \
" [-s|--size size]         size of packet to send in bytes (overrides file)\n"\
" [-si|--size-increment inc] increment of size from start to end (see -se)\n"\
" [-se|--size-end end]     last automated size to test (see -s and -si)\n"\
" [-st|--slack-time time]  time in seconds to sleep between fragment sends\n" \
"                          (.001 seconds by default)\n" \
" [-t|--target path]       file system location to save received files to\n" \
" [-u|--udp ip:port]       a udp ip to send to (first is self to bind to)\n" \
" [-w|--max-wait time]     maximum time to wait in seconds (double format)\n"\
" [-z|--target-id id]      id of the entity that must acknowledge receipt\n" \
" [-9|--terminator]        send terminate message\n" \
"\n",
        argv[0]));
      exit (0);
    }
  }
}

void
write_file (Madara::Knowledge_Map & records,
  const Madara::Transport::Transport_Context & context,
  Madara::Knowledge_Engine::Variables & vars)
{
  Madara::Knowledge_Map::iterator last = records.find ("last");

  if (last != records.end () && last->second == trusted.to_string ())
  {
    std::stringstream last_message;
    last_message << "Accepting updates from " << last->second << "\n";
    vars.print (last_message.str ());

    Madara::Knowledge_Map::iterator file = records.find ("file");
    if (file != records.end ())
    {
      std::stringstream filename;

      if (target_location == "")
        filename << records["file_location"];
      else
        filename << target_location;

      filename << "/";
      filename << records["file_name"];

      if (!Madara::Utility::file_exists (filename.str ()))
      {
        std::stringstream output_buffer;
        output_buffer << "Received file ";
        output_buffer << records["file_name"];
        output_buffer << ". Saving to ";
        output_buffer << filename.str ();
        output_buffer << "\n";

        vars.print (output_buffer.str ());

        size_t size = 0;

        file->second.to_file (filename.str ());
      }
      else
      {
        vars.print ("File already exists in folder. Not saving.\n");
      }

      records["last"] = self;
      vars.set (ack, Madara::Knowledge_Record::Integer (file->second.size ()));
      vars.print (
        "Received file. Sending file ack {file.{.id}.ack} for id {.id}.\n");
    }
  }
  else if (last != records.end ())
  {
    std::stringstream last_message;
    last_message << "Discarding updates from " << last->second << "\n";
    vars.print (last_message.str ());
    records.clear ();
  }
}

int main (int argc, char ** argv)
{
  // set defaults
  settings.type = Madara::Transport::MULTICAST;
  settings.queue_length = 2500000;
  settings.enable_participant_ttl (5);
  settings.set_rebroadcast_ttl (5);
  settings.slack_time = 0.001;

  // add receive filter for writing a file
  settings.add_receive_filter (write_file);
 
  // handle all user arguments
  handle_arguments (argc, argv);
  
  if (settings.id != 0)
  {
    trusted = Madara::Knowledge_Record::Integer (settings.id - 1);
  }

  if (settings.hosts.size () == 0)
  {
    // setup default transport as multicast
    settings.hosts.push_back (default_multicast);
  }

  // settings for delaying the sending of modifications
  Madara::Knowledge_Engine::Eval_Settings delay_sending;
  delay_sending.delay_sending_modifieds = true;

  Madara::Knowledge_Engine::Eval_Settings suppress_globals;
  suppress_globals.treat_globals_as_locals = true;
  
  Madara::Knowledge_Engine::Wait_Settings wait_settings;
  wait_settings.poll_frequency = 4;
  wait_settings.max_wait_time = max_wait;

  // create a knowledge base and setup our id
  Madara::Knowledge_Engine::Knowledge_Base knowledge (host, settings);
  knowledge.set (".id", Madara::Knowledge_Record::Integer (settings.id));
  knowledge.set (".target", target_id);

  ack = knowledge.get_ref (knowledge.expand_statement (
    "file.{.id}.ack"));

  if (is_terminator)
    knowledge.set ("terminated", 1.0);

  if (settings.id == 0)
  {
    if (target_location == "")
      target_location = ".";

    id0_wait = knowledge.compile (
      "(\n"
      "  file.{.target}.ack >= #size (file)\n"
      "  ||\n"
      "  terminated)\n"
      " ||\n"
      "((\n"
      "  file = file;\n"
      "  file_name = file_name;\n"
      "  file_location = file_location;\n"
      "  last = last\n"
      " ) && 0"
      ")");
    
    knowledge.print (
      "Sending file until id {.target} acknowledges receipt.\n");

    if (data_size == 0)
    {
      knowledge.read_file ("file", filename, delay_sending);
      knowledge.set ("file_name",
        Madara::Utility::extract_filename (filename), delay_sending);
    }
    else
    {
      std::stringstream new_name;
      new_name << "text_payload";
      new_name << data_size;
      new_name << ".txt";

      std::string text (data_size - 1, ' ');

      if (data_size > 3)
      {
        text[0] = 't';
        text[1] = 'e';
        text[2] = 's';
        text[3] = 't';
      }

      knowledge.set ("file", text, delay_sending);
      knowledge.set ("file_name", new_name.str (), delay_sending);
    }
    
    knowledge.set ("last", self, delay_sending);
    knowledge.set ("file_location", target_location, delay_sending);

    knowledge.evaluate ("file.{.id}.ack = #size (file)", suppress_globals);

    knowledge.print (
      "Writing {file_name} to network. "
      "Suggested target is {file_location}.\n");

    knowledge.send_modifieds ();
    
    Madara::Utility::sleep (4);

    knowledge.wait (id0_wait, wait_settings);

    for (data_size += data_size_increment; data_size <= data_size_end;
         data_size += data_size_increment)
    {
      std::stringstream new_name;
      new_name << "text_payload";
      new_name << data_size;
      new_name << ".txt";

      std::string text (data_size - 1, ' ');

      if (data_size > 3)
      {
        text[0] = 't';
        text[1] = 'e';
        text[2] = 's';
        text[3] = 't';
      }

      knowledge.set (".size", Madara::Knowledge_Record::Integer (data_size));
      knowledge.set ("file", text, delay_sending);
      knowledge.set ("file_name", new_name.str (), delay_sending);
      
      knowledge.print (
        "Writing {file_name} to network. "
        "Suggested target is {file_location}.\n");

      knowledge.wait (id0_wait, wait_settings);
    }

    knowledge.evaluate ("terminated = 1");

    knowledge.print (
      "Finished waiting."
      " file.{.target}.ack == {file.{.target}.ack}\n");

    Madara::Utility::sleep (4.0);

    knowledge.evaluate ("terminated = 1");
  }
  else
  {
    wait_settings.poll_frequency = 0.5;
    knowledge.set (".wait", max_wait);
    knowledge.print ("Waiting for {.wait}s or id 0 to signal terminate.\n");

    knowledge.wait ("terminated", wait_settings);

    knowledge.print ("Finished waiting.\n");
  }

  knowledge.print ();

  return 0;
}