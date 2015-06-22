
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
#include "madara/logger/Global_Logger.h"

#include "madara/utility/Utility.h"

namespace utility = Madara::Utility;
namespace logger = Madara::Logger;
typedef  Madara::Knowledge_Record::Integer Integer;

// default transport settings
std::string host ("");
const std::string default_multicast ("239.255.0.1:4150");
Madara::Transport::QoS_Transport_Settings settings;

// keep track of time
ACE_hrtime_t elapsed_time, maximum_time;
ACE_High_Res_Timer timer;

// publisher array for keeping track of ack latencies
class Timers
{
public:
  /**
   * Constructor
   * @param  quantity  number of timers to track
   **/
  Timers (unsigned int quantity)
    : timers_ (quantity),
      timers_stopped_ (quantity, 0), timers_started_ (quantity, 0)
  {

  }

  /**
   * Starts the timer at the indexed location
   * @param  index     timer identifier
   **/
  void start (unsigned int index)
  {
    // lock the mutex
    Timer_Guard guard (mutex_);

    if (index < timers_.size ())
    {
      timers_[index].start ();
      timers_started_[index] = 1;
    }
  }

  /**
   * Stops the timer at the indexed location
   * @param  index     timer identifier
   **/
  void stop (unsigned int index)
  {
    // lock the mutex
    Timer_Guard guard (mutex_);
    
    if (index < timers_.size ())
    {
      timers_[index].stop ();
      timers_stopped_[index] = 1;
      //if (timers_started_[index])
      //{
      //  std::cerr << index << ": stopped timer had a start timer\n";
      //}
      //else
      //{
      //  std::cerr << index << ": BAD NEWS: stopped timer was never started\n";
      //}
    }
  }

  /**
   * Returns the time that elapsed between start and stop at index
   * @param  index     timer identifier
   * @return the time that has elapsed
   **/
  Madara::Knowledge_Record::Integer elapsed (unsigned int index)
  {
    // lock the mutex
    Timer_Guard guard (mutex_);

    ACE_hrtime_t elapsed_time (0);
    Madara::Knowledge_Record::Integer result (0);
    
    if (index < timers_.size () && timers_stopped_[index] == 1)
    {
      timers_[index].elapsed_time (elapsed_time);
      result = Madara::Knowledge_Record::Integer (elapsed_time);
    }

    return result;
  }

  /**
   * Calculates minimum, maximum, and average latencies
   **/
  void calculate (void)
  {
    // lock the mutex
    Timer_Guard guard (mutex_);

    ACE_hrtime_t elapsed_time (0);
    Madara::Knowledge_Record::Integer cur_latency (0), received (0);
    min_latency_ = 2000000000;
    max_latency_ = 0;
    avg_latency_ = 0;

    for (unsigned int i = 0; i < timers_.size (); ++i)
    {
      if (timers_stopped_[i] != 0)
      {
        received++;
        timers_[i].elapsed_time (elapsed_time);
        cur_latency = Madara::Knowledge_Record::Integer (elapsed_time);

        avg_latency_ += cur_latency;
        min_latency_ = (std::min) (min_latency_, cur_latency);
        max_latency_ = (std::max) (max_latency_, cur_latency);
      }
    }

    if (received > 0)
      avg_latency_ = avg_latency_ / received;
  }

  /**
   * Returns the average latency in the list
   * @return average latency
   **/
  Madara::Knowledge_Record::Integer average (void)
  {
    // lock the mutex
    Timer_Guard guard (mutex_);
    return avg_latency_;
  }

  /**
   * Returns the minimum latency in the list
   * @return the minimum latency
   **/
  Madara::Knowledge_Record::Integer minimum (void)
  {
    // lock the mutex
    Timer_Guard guard (mutex_);
    return min_latency_;
  }
  
  /**
   * Returns the maximum latency in the list
   * @return the maximum latency
   **/
  Madara::Knowledge_Record::Integer maximum (void)
  {
    // lock the mutex
    Timer_Guard guard (mutex_);
    return max_latency_;
  }

  /**
   * Returns the number of timers
   * @return the number of timers
   **/
  unsigned int size (void)
  {
    // size cannot change after initialization, so no need
    // for mutex

    return (unsigned int)timers_.size ();
  }

private:
  
  // typedef for guard to be used with mutex
  typedef ACE_Guard<ACE_Recursive_Thread_Mutex> Timer_Guard;

  // mutex for protecting accesses
  mutable ACE_Recursive_Thread_Mutex mutex_;

  // vector of timers
  std::vector<ACE_High_Res_Timer> timers_;

  // vector that tracks stopped latencies
  std::vector<int> timers_stopped_;
  
  // vector that tracks started latencies
  std::vector<int> timers_started_;

  // maximum latency in the timer list
  Madara::Knowledge_Record::Integer max_latency_;

  // minimum latency in the timer list
  Madara::Knowledge_Record::Integer min_latency_;

  // average latency in the timer list
  Madara::Knowledge_Record::Integer avg_latency_;

};

Timers latencies (20);

// number of sent and received messages
Madara::Knowledge_Engine::Variable_Reference  num_sent;
Madara::Knowledge_Engine::Variable_Reference  num_received;
Madara::Knowledge_Engine::Variable_Reference  ack;

// to stop sending acks, flip to false
bool send_acks = true;

// amount of time in seconds to burst payloads
double publish_time = 10.0;

// payload size to burst
unsigned int data_size = 1000;

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
    else if (arg1 == "-a" || arg1 == "--no-latency")
    {
      send_acks = false;
    }
    else if (arg1 == "-o" || arg1 == "--host")
    {
      if (i + 1 < argc)
        host = argv[i + 1];

      ++i;
    }
    else if (arg1 == "-d" || arg1 == "--domain")
    {
      if (i + 1 < argc)
        settings.domains = argv[i + 1];

      ++i;
    }
    else if (arg1 == "-e" || arg1 == "--threads")
    {
      if (i + 1 < argc)
      {
        std::stringstream buffer (argv[i + 1]);
        buffer >> settings.read_threads;
      }

      ++i;
    }
    else if (arg1 == "-i" || arg1 == "--id")
    {
      if (i + 1 < argc)
      {
        std::stringstream buffer (argv[i + 1]);
        buffer >> settings.id;
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
    else if (arg1 == "-p" || arg1 == "--drop-rate")
    {
      if (i + 1 < argc)
      {
        double drop_rate;
        std::stringstream buffer (argv[i + 1]);
        buffer >> drop_rate;
        
        settings.update_drop_rate (drop_rate,
          Madara::Transport::PACKET_DROP_DETERMINISTIC);
      }

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
    else if (arg1 == "-r" || arg1 == "--reduced")
    {
      settings.send_reduced_message_header = true;
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
    else if (arg1 == "-t" || arg1 == "--time")
    {
      if (i + 1 < argc)
      {
        std::stringstream buffer (argv[i + 1]);
        buffer >> publish_time;
      }

      ++i;
    }
    else if (arg1 == "-z" || arg1 == "--read-hertz")
    {
      if (i + 1 < argc)
      {
        std::stringstream buffer (argv[i + 1]);
        buffer >> settings.read_thread_hertz;
      }

      ++i;
    }
    else
    {
      logger::global_logger->log (logger::LOG_ALWAYS,
        "\nProgram summary for %s:\n\n" \
        "  Profiles a network transport. Requires 2 processes. The result of\n" \
        "  running these processes should be that each process reports\n" \
        "  latency and throughput statistics.\n\n" \
        " [-a|--no-latency]        do not test for latency (throughput only)\n" \
        " [-b|--broadcast ip:port] the broadcast ip to send and listen to\n" \
        " [-d|--domain domain]     the knowledge domain to send and listen to\n" \
        " [-e|--threads threads]   number of read threads\n" \
        " [-i|--id id]             the id of this agent (should be non-negative)\n" \
        " [-l|--level level]       the logger level (0+, higher is higher detail)\n" \
        " [-m|--multicast ip:port] the multicast ip to send and listen to\n" \
        " [-o|--host hostname]     the hostname of this process (def:localhost)\n" \
        " [-f|--logfile file]      log to a file\n" \
        " [-r|--reduced]           use the reduced message header\n" \
        " [-s|--size size]         size of data packet to send in bytes\n" \
        " [-t|--time time]         time to burst messages for throughput test\n" \
        " [-u|--udp ip:port]       the udp ips to send to (first is self to bind to)\n" \
        " [-z|--read-hertz hertz]  read thread hertz speed\n" \
        "\n",
        argv[0]);

      exit (0);
    }
  }
}


// filter for counting received packets and removing payload
Madara::Knowledge_Record
latency_receiver (
  Madara::Knowledge_Engine::Function_Arguments & args,
  Madara::Knowledge_Engine::Variables & vars)
{
  Madara::Knowledge_Record result;
  
  if (args.size () > 0)
  {
    if (args[0].is_binary_file_type ())
    {
      // this is a safe way to add variables to a rebroadcast
      // without modifying the local context
      args.push_back ("ack");
      args.push_back (vars.inc (ack));
    }
  }

  return result;
}


// filter for counting received packets and removing payload
Madara::Knowledge_Record
count_received (
  Madara::Knowledge_Engine::Function_Arguments & args,
  Madara::Knowledge_Engine::Variables & vars)
{
  Madara::Knowledge_Record result;
  
  if (args.size () > 0)
  {
    if (args[0].is_binary_file_type ())
    {
      // if this is the first message we have received, start
      // the timer.
      if (vars.get (num_received).is_false ())
        timer.start ();

      timer.stop ();

      vars.inc (num_received);
    }
  }

  return result;
}


// filter for counting received acks, stopping timer and removing payload
Madara::Knowledge_Record
handle_acks (
  Madara::Knowledge_Engine::Function_Arguments & args,
  Madara::Knowledge_Engine::Variables & vars)
{
  Madara::Knowledge_Record result;
  
  if (args.size () > 0)
  {
    if (args[0].is_integer_type ())
    {
      Madara::Knowledge_Record::Integer handled =
        args[0].to_integer ();
      
      if (handled >= 0 && handled < latencies.size ())
      {
        latencies.stop (handled);
      }

      vars.inc (num_received);
    }
    result = args[0];
  }

  return result;
}

// filter for counting received acks, stopping timer and removing payload
Madara::Knowledge_Record
init_ack (
  Madara::Knowledge_Engine::Function_Arguments & args,
  Madara::Knowledge_Engine::Variables & vars)
{
  Madara::Knowledge_Record result;
  
  if (args.size () > 0)
  {
    if (args[0].is_binary_file_type ())
    {
      Madara::Knowledge_Record::Integer current_send =
        vars.get (num_sent).to_integer ();

      if (current_send >= 0 && current_send < latencies.size ())
        latencies.start (current_send);

      vars.inc (num_sent);
    }
    result = args[0];
  }

  return result;
}

// filter for counting received acks, stopping timer and removing payload
Madara::Knowledge_Record
count_sent (
  Madara::Knowledge_Engine::Function_Arguments & args,
  Madara::Knowledge_Engine::Variables & vars)
{
  Madara::Knowledge_Record result;
  
  if (args.size () > 0)
  {
    if (args[0].is_binary_file_type ())
    {
      vars.inc (num_sent);
    }
    result = args[0];
  }

  return result;
}

int main (int argc, char ** argv)
{
  settings.type = Madara::Transport::MULTICAST;
  settings.queue_length = data_size * 1000;

  // unlimited hertz
  settings.read_thread_hertz = 0.0;
  settings.read_threads = 1;

  // use ACE real time scheduling class
  int prio  = ACE_Sched_Params::next_priority
    (ACE_SCHED_FIFO,
     ACE_Sched_Params::priority_max (ACE_SCHED_FIFO),
     ACE_SCOPE_THREAD);
  ACE_OS::thr_setprio (prio);

  // handle all user arguments
  handle_arguments (argc, argv);

  if (settings.hosts.size () == 0)
  {
    // setup default transport as multicast
    settings.hosts.resize (1);
    settings.hosts[0] = default_multicast;
  }
  
  if (settings.id != 0)
  {
    // latency test requires client to generate acks
    settings.add_receive_filter (
      Madara::Knowledge_Record::ALL_FILE_TYPES,
      latency_receiver);
    settings.enable_participant_ttl ();
  }
  else
  {
    // latency test requires publisher to init and handle acks
    settings.add_receive_filter (
      Madara::Knowledge_Record::INTEGER,
      handle_acks);
    settings.add_send_filter (
      Madara::Knowledge_Record::ALL_FILE_TYPES,
      init_ack);
  }

  // create a knowledge base and setup our id
  Madara::Knowledge_Engine::Knowledge_Base knowledge (host, settings);
  knowledge.set (".id", (Madara::Knowledge_Record::Integer) settings.id);
  
  // setup wait settings to wait for 2 * publish_time seconds
  Madara::Knowledge_Engine::Wait_Settings wait_settings;
  wait_settings.max_wait_time = publish_time * 2;
  
  // set poll frequency to every 10us
  //wait_settings.poll_frequency = 0.00001;

  // get variable references for real-time, constant-time operations
  num_received = knowledge.get_ref (".num_received");
  num_sent = knowledge.get_ref (".num_sent");
  ack = knowledge.get_ref (".ack");

  if (settings.id == 0)
  {
    // publisher

    // set up the payload
    unsigned char * ref_file = new unsigned char[data_size];
    knowledge.set_file (".ref_file", ref_file, data_size);
    Madara::Knowledge_Engine::Variable_Reference file =
      knowledge.get_ref (".ref_file");
    
    // begin latency test
    knowledge.print ("\nRunning latency test...");

    for (int i = 0; i < 10; ++i)
    {
      knowledge.mark_modified (file);
    
      ACE_OS::sleep (1);
    }
    
    knowledge.print (" done.\n");

    knowledge.set ("latency_finished");
    
    // close the transport so we can use different filters
    knowledge.close_transport ();

    settings.clear_receive_filters (Madara::Knowledge_Record::ALL_TYPES);
    settings.clear_send_filters (Madara::Knowledge_Record::ALL_TYPES);
    
    // add a simple filter for counting the sent packets
    settings.add_send_filter (
      Madara::Knowledge_Record::ALL_FILE_TYPES,
      count_sent);

    // attach the new transport
    knowledge.attach_transport (knowledge.get_id (), settings);
    
    // reannounce latency finished and wait for client before throughput test

    knowledge.set ("latency_finished");
    
    

#ifndef _MADARA_NO_KARL_

    knowledge.wait ("ready_for_throughput", wait_settings);
    
#else
    utility::wait_true (knowledge, "ready_for_throughput", wait_settings);
#endif // _MADARA_NO_KARL_

    knowledge.print ("Running throughput test...");

    // reset timer
    elapsed_time = 0;
    timer.start ();
    timer.stop ();
    timer.elapsed_time (elapsed_time);
    
    ACE_Time_Value max_tv;
    max_tv.set (publish_time);
    maximum_time = max_tv.sec () * 1000000000;
    maximum_time += max_tv.usec () * 1000;
    
    // publish the payload repeatedly until max time has passed
    while (maximum_time > elapsed_time)
    {
      knowledge.mark_modified (file);
      timer.stop ();
      timer.elapsed_time (elapsed_time);
    }
    
    // throughput test over
    knowledge.print (" done.\n");
    knowledge.set ("finished");
    

    // calculate latencies
    latencies.calculate ();

    Madara::Knowledge_Engine::Eval_Settings delay (true);

    knowledge.set ("min_latency", latencies.minimum () / 2, delay);
    knowledge.set ("max_latency", latencies.maximum () / 2, delay);
    knowledge.set ("avg_latency", latencies.average () / 2, delay);

    // rebroadcast that the throughput test is finished
    knowledge.set ("finished");
    
    // only client knows real message throughput. Wait for it.
    knowledge.print ("Waiting for throughput results from client...");
    wait_settings.max_wait_time = 5.0;

#ifndef _MADARA_NO_KARL_

    knowledge.wait ("throughput > 0.0", wait_settings);
    
#else
    utility::wait_true (knowledge, "throughput", wait_settings);

#endif // _MADARA_NO_KARL_

    knowledge.print (" done.\n");

    // send the bytes per second in the throughput test
    knowledge.print ("Sending final information to client...");

#ifndef _MADARA_NO_KARL_

    knowledge.evaluate (
      "bytes_per_sec = #size (.ref_file) * #integer (throughput)");
#else
    knowledge.set ("bytes_per_sec",
      knowledge.get ("throughput").to_integer () *
      knowledge.get (".ref_file").size ());
#endif // _MADARA_NO_KARL_

    knowledge.print (" done.\n");
  }
  else
  {
    // client should always be started first
    knowledge.print ("\nReady for latency test.\n");

    knowledge.set (ack, Madara::Knowledge_Record::Integer (-1),
      Madara::Knowledge_Engine::Eval_Settings (false, true, false));
    
#ifndef _MADARA_NO_KARL_

    // other processes wait for the publisher to send the goods
    knowledge.wait ("latency_finished", wait_settings);
    
#else
    utility::wait_true (knowledge, "latency_finished", wait_settings);

#endif // _MADARA_NO_KARL_

    send_acks = false;
    
    // close transport so we add filters appropriate for throughput
    knowledge.close_transport ();
    settings.clear_receive_filters (Madara::Knowledge_Record::ALL_FILE_TYPES);

    settings.add_receive_filter (
      Madara::Knowledge_Record::ALL_FILE_TYPES,
      count_received);
    knowledge.attach_transport (knowledge.get_id (), settings);
    
    // signal that the client is ready for the throughput test
    knowledge.print ("Ready for throughput test.\n");
    knowledge.set ("ready_for_throughput");

    // reset num_received and re-signal ready for throughput
    knowledge.set (num_received, Madara::Knowledge_Record::Integer (0));
    knowledge.set ("ready_for_throughput");
    
#ifndef _MADARA_NO_KARL_

    // other processes wait for the publisher to send the goods
    knowledge.wait ("finished", wait_settings);
    
#else
    utility::wait_true (knowledge, "finished", wait_settings);

#endif // _MADARA_NO_KARL_

    knowledge.print ("Finished latency and throughput tests.\n");

    timer.elapsed_time (elapsed_time);

    // set the elapsed nanoseconds
    knowledge.set ("elapsed_time",
      Madara::Knowledge_Record::Integer (elapsed_time));
    
    // convert elapsed time from ns -> s and compute throughput
    knowledge.print ("Calculating throughput...");
    
#ifndef _MADARA_NO_KARL_

    knowledge.evaluate (
      "elapsed_time *= 0.000000001 ;"
      "elapsed_time > 0 => throughput = .num_received / elapsed_time");
    
#else
    double elapsed_time = knowledge.get ("elapsed_time").to_double ();
    elapsed_time *= 0.000000001;

    if (elapsed_time > 0)
    {
      Integer num_received = knowledge.get (".num_received").to_integer ();
      num_received /= elapsed_time;
      knowledge.set ("throughput", num_received);
    }

#endif // _MADARA_NO_KARL_

    knowledge.print (" done.\n");
    
    // Wait for bytes per second from publisher
    knowledge.print ("Awaiting final data from publisher...");

#ifndef _MADARA_NO_KARL_

    knowledge.wait ("bytes_per_sec", wait_settings);
    
#else
    utility::wait_true (knowledge, "bytes_per_sec", wait_settings);

#endif // _MADARA_NO_KARL_

    knowledge.print (" done.\n");
  }

  knowledge.print ("\nNetwork profile for transport\n");
  knowledge.print ("Latency (10 message test)\n");
  knowledge.print ("  Minimum:   {min_latency} ns\n");
  knowledge.print ("  Maximum:   {max_latency} ns\n");
  knowledge.print ("  Average:   {avg_latency} ns\n");
  knowledge.print ("Throughput ({elapsed_time}s blast test)\n");
  knowledge.print ("  Messages:  {throughput} payloads/s\n");
  knowledge.print ("  Data rate: {bytes_per_sec} B/s\n");

  return 0;
}
