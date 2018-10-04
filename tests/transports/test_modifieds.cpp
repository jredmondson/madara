
#include <string>
#include <vector>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <sstream>
#include <assert.h>

#include "madara/knowledge/KnowledgeBase.h"
#include "madara/knowledge/containers/Integer.h"
#include "madara/utility/Utility.h"
#include "madara/logger/GlobalLogger.h"
#include "madara/filters/CounterFilter.h"
#include "madara/threads/Threader.h"
#include "madara/knowledge/containers/String.h"

namespace logger = madara::logger;
namespace utility = madara::utility;
namespace filters = madara::filters;

// useful namespace aliases and typedefs

namespace knowledge = madara::knowledge;
namespace containers = knowledge::containers;
namespace transport = madara::transport;
namespace threads = madara::threads;

typedef madara::knowledge::KnowledgeRecord::Integer Integer;

// default transport settings
std::string host("");
const std::string default_multicast("239.255.0.1:4150");
transport::QoSTransportSettings settings;

// target seconds to be active
double active_time(10.0);

double send_hertz = 0.0;

bool started = false;

// amount of time in seconds to burst payloads
double publish_time = 10.0;

// payload size to burst
unsigned int data_size = 1000;

// handle command line arguments
void handle_arguments(int argc, char** argv)
{
  for (int i = 1; i < argc; ++i)
  {
    std::string arg1(argv[i]);

    if (arg1 == "-m" || arg1 == "--multicast")
    {
      if (i + 1 < argc)
      {
        settings.hosts.push_back(argv[i + 1]);
        settings.type = madara::transport::MULTICAST;
      }
      ++i;
    }
    else if (arg1 == "-b" || arg1 == "--broadcast")
    {
      if (i + 1 < argc)
      {
        settings.hosts.push_back(argv[i + 1]);
        settings.type = madara::transport::BROADCAST;
      }
      ++i;
    }
    else if (arg1 == "-u" || arg1 == "--udp")
    {
      if (i + 1 < argc)
      {
        settings.hosts.push_back(argv[i + 1]);
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
        std::stringstream buffer(argv[i + 1]);
        buffer >> settings.read_threads;
      }

      ++i;
    }
    else if (arg1 == "-i" || arg1 == "--id")
    {
      if (i + 1 < argc)
      {
        std::stringstream buffer(argv[i + 1]);
        buffer >> settings.id;
      }

      ++i;
    }
    else if (arg1 == "-l" || arg1 == "--level")
    {
      if (i + 1 < argc)
      {
        std::stringstream buffer(argv[i + 1]);
        int level;
        buffer >> level;
        logger::global_logger->set_level(level);
      }

      ++i;
    }
    else if (arg1 == "-p" || arg1 == "--drop-rate")
    {
      if (i + 1 < argc)
      {
        double drop_rate;
        std::stringstream buffer(argv[i + 1]);
        buffer >> drop_rate;

        settings.update_drop_rate(
            drop_rate, madara::transport::PACKET_DROP_DETERMINISTIC);
      }

      ++i;
    }
    else if (arg1 == "-f" || arg1 == "--logfile")
    {
      if (i + 1 < argc)
      {
        logger::global_logger->add_file(argv[i + 1]);
      }

      ++i;
    }
    else if (arg1 == "-q" || arg1 == "--queue-length")
    {
      if (i + 1 < argc)
      {
        std::stringstream buffer(argv[i + 1]);
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
        std::stringstream buffer(argv[i + 1]);
        buffer >> data_size;
      }

      ++i;
    }
    else if (arg1 == "-t" || arg1 == "--time")
    {
      if (i + 1 < argc)
      {
        std::stringstream buffer(argv[i + 1]);
        buffer >> active_time;
      }

      ++i;
    }
    else if (arg1 == "-z" || arg1 == "--read-hertz")
    {
      if (i + 1 < argc)
      {
        std::stringstream buffer(argv[i + 1]);
        buffer >> settings.read_thread_hertz;
      }

      ++i;
    }
    else if (arg1 == "--sz" || arg1 == "--send-hertz")
    {
      if (i + 1 < argc)
      {
        std::stringstream buffer(argv[i + 1]);
        buffer >> send_hertz;
      }

      ++i;
    }
    else
    {
      madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_ALWAYS,
          "\nProgram summary for %s:\n\n"
          "  Tests the save_modifieds and add_modifieds functions.\n\n"
          " [-b|--broadcast ip:port] the broadcast ip to send and listen to\n"
          " [-d|--domain domain]     the knowledge domain to send and listen "
          "to\n"
          " [-e|--threads threads]   number of read threads\n"
          " [-f|--logfile file]      log to a file\n"
          " [-i|--id id]             the id of this agent (should be "
          "non-negative)\n"
          " [-l|--level level]       the logger level (0+, higher is higher "
          "detail)\n"
          " [-m|--multicast ip:port] the multicast ip to send and listen to\n"
          " [-t|--target target]     number of payloads to send/target\n"
          " [-o|--host hostname]     the hostname of this process "
          "(def:localhost)\n"
          " [-q|--queue-length len   the queue size to use for the test\n"
          " [-r|--reduced]           use the reduced message header\n"
          " [-s|--size size]         size of data packet to send in bytes\n"
          " [--sz|--send-hertz hertz] send thread hertz for send speed\n"
          " [-t|--time time]         time to stay active for throughput test\n"
          " [-u|--udp ip:port]       the udp ips to send to (first is self to "
          "bind to)\n"
          " [-z|--read-hertz hertz]  read thread hertz speed\n"
          "\n",
          argv[0]);
      exit(0);
    }
  }
}

class Sender : public threads::BaseThread
{
private:
  /// knowledge engine used for sending
  knowledge::KnowledgeBase* knowledge_;

  /// modified list to re-apply
  knowledge::VariableReferences* modifieds_;

public:
  Sender(knowledge::KnowledgeBase& knowledge,
      knowledge::VariableReferences* modifieds)
    : knowledge_(&knowledge), modifieds_(modifieds)
  {
  }

  virtual ~Sender() {}

  // modify the payload and send the update
  virtual void run(void)
  {
    knowledge_->add_modifieds(*modifieds_);
    knowledge_->send_modifieds();
  }
};

int main(int argc, char** argv)
{
  settings.type = transport::MULTICAST;
  settings.queue_length = data_size * 1000;

  filters::CounterFilter counter;

  // unlimited hertz
  settings.read_thread_hertz = 0.0;
  settings.read_threads = 1;

  // set thread priority to high
  madara::utility::set_thread_priority();

  // handle all user arguments
  handle_arguments(argc, argv);

  if (settings.hosts.size() == 0)
  {
    // setup default transport as multicast
    settings.hosts.resize(1);
    settings.hosts[0] = default_multicast;
  }

  // count the number of sent packets
  settings.add_send_filter(&counter);

  // create a knowledge base and setup our id
  knowledge::KnowledgeBase knowledge(host, settings);

  knowledge::containers::Integer data1("data1", knowledge, 1);
  knowledge::containers::Integer data2("data2", knowledge, 2);
  knowledge::containers::Integer data3("data3", knowledge, 3);

  std::cerr << std::setprecision(2) << std::fixed;
  std::cerr << "Saving modifieds after setting data...\n";

  knowledge::VariableReferences modifieds = knowledge.save_modifieds();

  std::cerr << "Reloading saved modifed list at " << send_hertz << "hz for "
            << active_time << " seconds...\n\n";

  threads::Threader threader(knowledge);
  threader.run(send_hertz, "sender", new Sender(knowledge, &modifieds));
  utility::sleep(active_time);
  threader.terminate();
  threader.wait();

  std::cerr << "Test results:\n";
  std::cerr << "  Packets: " << counter.get_count() << "\n";
  std::cerr << "  Elapsed Time (s): " << counter.get_elapsed() << "\n";
  std::cerr << "  Message Throughput (message/s): " << counter.get_throughput()
            << "\n";
  std::cerr << "  Data Throughput (B/s): " << counter.get_throughput() * 24
            << "\n";

  knowledge.print();

  return 0;
}
