#include <atomic>

#include "madara/knowledge/KnowledgeBase.h"
#include "madara/logger/GlobalLogger.h"
#include "madara/filters/AggregateFilter.h"

#include "madara/utility/Utility.h"

namespace utility = madara::utility;
namespace logger = madara::logger;
namespace knowledge = madara::knowledge;
namespace transport = madara::transport;

// default transport settings
std::string host ("");
const std::string default_multicast ("239.255.0.1:4150");
madara::transport::QoSTransportSettings settings;
double test_time (60);
size_t data_size (128);

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
        settings.type = madara::transport::MULTICAST;
      }
      ++i;
    }
    else if (arg1 == "-b" || arg1 == "--broadcast")
    {
      if (i + 1 < argc)
      {
        settings.hosts.push_back (argv[i + 1]);
        settings.type = madara::transport::BROADCAST;
      }
      ++i;
    }
    else if (arg1 == "-u" || arg1 == "--udp")
    {
      if (i + 1 < argc)
      {
        settings.hosts.push_back (argv[i + 1]);
        settings.type = madara::transport::UDP;
      }
      ++i;
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
        settings.write_domain = argv[i + 1];

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
          madara::transport::PACKET_DROP_DETERMINISTIC);
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
        buffer >> test_time;
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
    else if (arg1 == "--zmq" || arg1 == "--0mq")
    {
      if (i + 1 < argc)
      {
        settings.hosts.push_back (argv[i + 1]);
        settings.type = transport::ZMQ;
      }
      ++i;
    }
    else
    {
      madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_ALWAYS,
        "\nProgram summary for %s:\n\n" \
        "  Profiles a network transport. Requires 2 processes. The result of\n" \
        "  running these processes should be that each process reports\n" \
        "  latency and throughput statistics.\n\n" \
        " [-a|--no-latency]        do not test for latency (throughput only)\n" \
        " [-b|--broadcast ip:port] the broadcast ip to send and listen to\n" \
        " [-d|--domain domain]     the knowledge domain to send and listen to\n" \
        " [-e|--threads threads]   number of read threads\n" \
        " [-f|--logfile file]      log to a file\n" \
        " [-i|--id id]             the id of this agent (should be non-negative)\n" \
        " [-l|--level level]       the logger level (0+, higher is higher detail)\n" \
        " [-m|--multicast ip:port] the multicast ip to send and listen to\n" \
        " [-o|--host hostname]     the hostname of this process (def:localhost)\n" \
        " [-q|--queue-length len   the buffer size to use for the test\n" \
        " [-r|--reduced]           use the reduced message header\n" \
        " [-s|--size size]         size of data packet to send in bytes\n" \
        " [-t|--time time]         time to burst messages for throughput test\n" \
        " [-u|--udp ip:port]       the udp ips to send to (first is self to bind to)\n" \
        " [-z|--read-hertz hertz]  read thread hertz speed\n" \
        " [--zmq|--0mq proto://ip:port] a ZeroMQ endpoint to connect to.\n" \
        "                          examples include tcp://127.0.0.1:30000\n" \
        "                          or any of the other endpoint types like\n" \
        "                          pgm://. For tcp, remember that the first\n" \
        "                          endpoint defined must be your own, the\n" \
        "                          one you are binding to, and all other\n" \
        "                          agent endpoints must also be defined or\n" \
        "                          no messages will ever be sent to them.\n" \
        "                          Similarly, all agents will have to have\n" \
        "                          this endpoint added to their list or\n" \
        "                          this karl agent will not see them.\n" \
        "\n",
        argv[0]);

      exit (0);
    }
  }
}

class Profiler : public madara::filters::AggregateFilter
{
  public:
    Profiler ()
    : total_latency (0), received (0), min_latency (0), max_latency (0)
    {
    }

  void filter (knowledge::KnowledgeMap &,
    const transport::TransportContext &transport_context,
    knowledge::Variables &)
  {
    uint64_t current =
      transport_context.get_current_time () - transport_context.get_message_time ();
    total_latency += current;

    // there are race conditions here with multiple read threads but meh
    if (received == 0)
    {
      min_latency = current;
      max_latency = current;
    }
    else
    {
      uint64_t cur_min = min_latency;
      uint64_t cur_max = max_latency;

      min_latency = current < cur_min ? current : cur_min;
      max_latency = current > cur_max ? current : cur_max;
    }

    ++received;
  }

  std::atomic <uint64_t> total_latency;
  std::atomic <uint64_t> received;
  std::atomic <uint64_t> min_latency;
  std::atomic <uint64_t> max_latency;
};

int main (int argc, char ** argv)
{
  // initialize settings
  settings.type = transport::MULTICAST;
  settings.queue_length = 1000000;

  // parse the user command line arguments
  handle_arguments (argc, argv);

  // setup default transport as multicast
  if (settings.hosts.size () == 0)
  {
    settings.hosts.push_back (default_multicast);
  }

  // id == 0 ? "publisher" : "subscriber"
  if (settings.id == 0)
  {
    // setup a knowledge base
    knowledge::KnowledgeBase kb (host, settings);

    // get a handle to the data
    knowledge::VariableReference var = kb.get_ref ("data");

    unsigned char * data = new unsigned char [data_size]; 
    kb.set_file (var, data, data_size, knowledge::EvalSettings::DELAY_NO_EXPAND);

    std::cerr << "Publishing " << data_size << " B packets for "
      << test_time << " s on " << transport::types_to_string (settings.type)
      << " transport\n";

    // keep track of test time
    utility::TimeValue current = madara::utility::Clock::now ();
    madara::utility::TimeValue end_time = current +
      madara::utility::seconds_to_duration (test_time);

    // spin and modify
    while (utility::Clock::now () < end_time)
    {
      kb.mark_modified (var);
      kb.send_modifieds ();
    }

    delete [] data;

    std::cerr << "Publisher is done. Check results on subscriber.\n";
  } // end publisher
  else
  {
    // add a receive filter to keep track of latency
    Profiler profiler;
    settings.add_receive_filter (&profiler);

    // setup a knowledge base
    knowledge::KnowledgeBase kb (host, settings);

    std::cerr << "Receiving for " << test_time << " s on " <<
      transport::types_to_string (settings.type) << " transport\n";

    // subscriber lives the hard life (TM)
    utility::sleep (test_time);

    // stop the knowledge base from receiving anything else
    kb.close_transport ();

    // print stats
    if (profiler.received > 0)
    {
      uint64_t received = profiler.received;
      uint64_t avg_latency = profiler.total_latency / received;
      uint64_t min_latency = profiler.min_latency;
      uint64_t max_latency = profiler.max_latency;
      data_size = (size_t) kb.get ("data").size ();
      uint64_t data_transfered = (uint64_t) data_size * received;
      double data_rate = (double)data_transfered / test_time;
      double msg_rate = (double)received / test_time;

      std::cerr << "Test: SUCCESS\n";
      std::cerr << "Settings:\n";
      std::cerr << "  Transport type: " <<
        transport::types_to_string (settings.type) << "\n";
      std::cerr << "  Buffer size: " << settings.queue_length << " B\n";

      std::cerr << "  Hosts: \n";

      for (auto host : settings.hosts)
      {
        std::cerr << "    " << host << "\n";
      }

      std::cerr << "  Data size: " << data_size << " B\n";
      std::cerr << "  Test time: " << test_time << " s\n";
      std::cerr << "Latency:\n";
      std::cerr << "  Min: " << min_latency << " ns\n";
      std::cerr << "  Avg: " << avg_latency << " ns\n";
      std::cerr << "  Max: " << max_latency << " ns\n";
      std::cerr << "Throughput:\n";
      std::cerr << "  Messages received: " << received << "\n";
      std::cerr << "  Message rate: " << msg_rate << " packets/s\n";
      std::cerr << "  Data received: " << data_transfered << " B\n";
      std::cerr << "  Data rate: " << data_rate << " B/s\n";
    }
    else
    {
      std::cerr << "Subscriber received no data.\nTest: FAIL.\n";
      return -1;
    }
  }

  return 0;
}
