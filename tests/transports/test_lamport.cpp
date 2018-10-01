#include <atomic>

#include "madara/knowledge/KnowledgeBase.h"
#include "madara/knowledge/containers/Integer.h"
#include "madara/logger/GlobalLogger.h"
#include "madara/filters/AggregateFilter.h"

#include "madara/utility/Utility.h"
#include "madara/utility/EpochEnforcer.h"

namespace utility = madara::utility;
namespace logger = madara::logger;
namespace knowledge = madara::knowledge;
namespace containers = knowledge::containers;
namespace transport = madara::transport;

// default transport settings
std::string host("");
const std::string default_multicast("239.255.0.1:4150");
madara::transport::QoSTransportSettings settings;
double test_time(20);
size_t data_size(128);
double send_hertz(-1);
size_t num_vars(1);
int madara_fails(0);

// handle command line arguments
void handle_arguments(int argc, char** argv)
{
  for (int i = 1; i < argc; ++i) {
    std::string arg1(argv[i]);

    if (arg1 == "-m" || arg1 == "--multicast") {
      if (i + 1 < argc) {
        settings.hosts.push_back(argv[i + 1]);
        settings.type = madara::transport::MULTICAST;
      }
      ++i;
    } else if (arg1 == "-b" || arg1 == "--broadcast") {
      if (i + 1 < argc) {
        settings.hosts.push_back(argv[i + 1]);
        settings.type = madara::transport::BROADCAST;
      }
      ++i;
    } else if (arg1 == "-u" || arg1 == "--udp") {
      if (i + 1 < argc) {
        settings.hosts.push_back(argv[i + 1]);
        settings.type = madara::transport::UDP;
      }
      ++i;
    } else if (arg1 == "-o" || arg1 == "--host") {
      if (i + 1 < argc)
        host = argv[i + 1];

      ++i;
    } else if (arg1 == "-d" || arg1 == "--domain") {
      if (i + 1 < argc)
        settings.write_domain = argv[i + 1];

      ++i;
    } else if (arg1 == "-e" || arg1 == "--threads") {
      if (i + 1 < argc) {
        std::stringstream buffer(argv[i + 1]);
        buffer >> settings.read_threads;
      }

      ++i;
    } else if (arg1 == "-f" || arg1 == "--logfile") {
      if (i + 1 < argc) {
        logger::global_logger->add_file(argv[i + 1]);
      }

      ++i;
    } else if (arg1 == "-i" || arg1 == "--id") {
      if (i + 1 < argc) {
        std::stringstream buffer(argv[i + 1]);
        buffer >> settings.id;
      }

      ++i;
    } else if (arg1 == "-l" || arg1 == "--level") {
      if (i + 1 < argc) {
        int level;
        std::stringstream buffer(argv[i + 1]);
        buffer >> level;

        logger::global_logger->set_level(level);
      }

      ++i;
    } else if (arg1 == "-p" || arg1 == "--drop-rate") {
      if (i + 1 < argc) {
        double drop_rate;
        std::stringstream buffer(argv[i + 1]);
        buffer >> drop_rate;

        settings.update_drop_rate(
            drop_rate, madara::transport::PACKET_DROP_DETERMINISTIC);
      }

      ++i;
    } else if (arg1 == "-q" || arg1 == "--queue-length") {
      if (i + 1 < argc) {
        std::stringstream buffer(argv[i + 1]);
        buffer >> settings.queue_length;
      }

      ++i;
    } else if (arg1 == "-r" || arg1 == "--reduced") {
      settings.send_reduced_message_header = true;
    } else if (arg1 == "--send-hz") {
      if (i + 1 < argc) {
        std::stringstream buffer(argv[i + 1]);
        buffer >> send_hertz;
      }

      ++i;
    } else if (arg1 == "-t" || arg1 == "--time") {
      if (i + 1 < argc) {
        std::stringstream buffer(argv[i + 1]);
        buffer >> test_time;
      }

      ++i;
    } else if (arg1 == "-z" || arg1 == "--read-hertz") {
      if (i + 1 < argc) {
        std::stringstream buffer(argv[i + 1]);
        buffer >> settings.read_thread_hertz;
      }

      ++i;
    } else if (arg1 == "--zmq" || arg1 == "--0mq") {
      if (i + 1 < argc) {
        settings.hosts.push_back(argv[i + 1]);
        settings.type = transport::ZMQ;
      }
      ++i;
    } else {
      madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_ALWAYS,
          "\nProgram summary for %s:\n\n"
          "  Publishes variable updates to try to induce lamport clock "
          "failures.\n\n"
          " [-a|--no-latency]        do not test for latency (throughput "
          "only)\n"
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
          " [-o|--host hostname]     the hostname of this process "
          "(def:localhost)\n"
          " [-q|--queue-length len   the buffer size to use for the test\n"
          " [-r|--reduced]           use the reduced message header\n"
          " [--send-hz hertz]        hertz to send at\n"
          " [-t|--time time]         time to burst messages for throughput "
          "test\n"
          " [-u|--udp ip:port]       the udp ips to send to (first is self to "
          "bind to)\n"
          " [-z|--read-hertz hertz]  read thread hertz speed\n"
          " [--zmq|--0mq proto://ip:port] a ZeroMQ endpoint to connect to.\n"
          "                          examples include tcp://127.0.0.1:30000\n"
          "                          or any of the other endpoint types like\n"
          "                          pgm://. For tcp, remember that the first\n"
          "                          endpoint defined must be your own, the\n"
          "                          one you are binding to, and all other\n"
          "                          agent endpoints must also be defined or\n"
          "                          no messages will ever be sent to them.\n"
          "                          Similarly, all agents will have to have\n"
          "                          this endpoint added to their list or\n"
          "                          this karl agent will not see them.\n"
          "\n",
          argv[0]);

      exit(0);
    }
  }
}

int main(int argc, char** argv)
{
  // initialize settings
  settings.type = transport::MULTICAST;
  settings.queue_length = 1000000;

  // parse the user command line arguments
  handle_arguments(argc, argv);

  // setup default transport as multicast
  if (settings.hosts.size() == 0) {
    settings.hosts.push_back(default_multicast);
  }

  // setup a knowledge base
  knowledge::KnowledgeBase kb(host, settings);
  containers::Integer var("var", kb);
  utility::EpochEnforcer<utility::Clock> enforcer(1 / send_hertz, test_time);

  kb.get_context().set_clock(100);

  // id == 0 ? "publisher" : "subscriber"
  if (settings.id == 0) {
    if (num_vars > 1) {
      data_size = data_size / num_vars;
    } else {
      num_vars = 1;
    }

    std::cerr << "Publishing integer packets for " << test_time << " s on "
              << transport::types_to_string(settings.type) << " transport\n";

    size_t counter = 0;
    knowledge::KnowledgeUpdateSettings decrementer;
    decrementer.clock_increment = -5;
    // use epoch enforcer"
    while (!enforcer.is_done()) {
      // Periodically rollback lamport clock to check enforcement
      if (counter % 10 == 0) {
        knowledge::ContextGuard guard(kb);
        kb.get_context().inc_clock(decrementer);
      }

      ++counter;
      ++var;

      kb.send_modifieds();

      if (send_hertz > 0.0) {
        enforcer.sleep_until_next();
      }
    }

    std::cerr << "Publisher is done. Check results on subscriber.\n";
  }  // end publisher
  else {
    std::cerr << "Receiving for " << test_time << " s on "
              << transport::types_to_string(settings.type) << " transport\n";

    uint64_t last_context_clock = 0;
    uint64_t last_get_kr_clock = 0;
    uint64_t last_container_kr_clock = 0;
    int64_t last_value = 0;
    knowledge::VariableReference var_ref = kb.get_ref("var");

    // use epoch enforcer"
    while (!enforcer.is_done() && madara_fails == 0) {
      uint64_t cur_context_clock;
      uint64_t cur_get_kr_clock;
      uint64_t cur_container_kr_clock;
      int64_t cur_value = 0;

      {
        knowledge::ContextGuard guard(kb);
        cur_context_clock = kb.get_context().get_clock();
        cur_get_kr_clock = kb.get(var_ref).clock;
        cur_container_kr_clock = var.to_record().clock;
        cur_value = *var;
      }

      // std::cerr << cur_context_clock << " " <<  cur_get_kr_clock <<
      //" " << cur_container_kr_clock << " " << cur_value << std::endl;

      if (cur_value < last_value) {
        std::cerr << "  Integer.to_integer(): " << last_value;
        std::cerr << "->" << cur_value << ". Test: ";
        std::cerr << "FAIL\n";
        ++madara_fails;
      }
      last_value = cur_value;

      if (cur_context_clock < last_context_clock) {
        std::cerr << "  context.clock: " << last_context_clock;
        std::cerr << "->" << cur_context_clock << ". Test: ";
        std::cerr << "FAIL\n";
        ++madara_fails;
      }
      last_context_clock = cur_context_clock;

      if (cur_get_kr_clock < last_get_kr_clock) {
        std::cerr << "  get(\"var\").clock: " << last_get_kr_clock;
        std::cerr << "->" << cur_get_kr_clock << ". Test: ";
        std::cerr << "FAIL\n";
        ++madara_fails;
      }
      last_get_kr_clock = cur_get_kr_clock;

      if (cur_container_kr_clock < last_container_kr_clock) {
        std::cerr << "  Integer.to_record().clock: " << last_container_kr_clock;
        std::cerr << "->" << cur_container_kr_clock << ". Test: ";
        std::cerr << "FAIL\n";
        ++madara_fails;
      }
      last_container_kr_clock = cur_container_kr_clock;
    }
  }

  if (madara_fails > 0) {
    std::cerr << "OVERALL: FAIL. " << madara_fails << " tests failed.\n";
  } else {
    std::cerr << "OVERALL: SUCCESS.\n";
  }

  return madara_fails;
}
