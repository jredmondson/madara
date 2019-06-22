#define BOOST_PP_VARIADICS 1

#include <string>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>

#include "madara/knowledge/KnowledgeBase.h"
#include "madara/knowledge/containers/Integer.h"

#ifdef _USE_CAPNP_
#include "../capnfiles/Geo.capnp.h"
#endif

#include "../test.h"

using namespace madara;
using namespace knowledge;
using namespace transport;

using Int = containers::Integer;

int log_level = 2;

void handle_arguments(int argc, char** argv)
{
  for (int i = 1; i < argc; ++i)
  {
    std::string arg1(argv[i]);
    if (arg1 == "-l" || arg1 == "--level")
    {
      if (i + 1 < argc)
      {
        std::stringstream buffer(argv[i + 1]);
        buffer >> log_level;
      }

      ++i;
    }
    else
    {
      madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_ALWAYS,
          "\nProgram summary for %s:\n\n"
          "  Test the checkpointing functionality.\n\n"
          " [-l|--level level]       the logger level (0+, higher is higher "
          "detail)\n"
          "\n",
          argv[0]);
      exit(0);
    }
  }

  logger::global_logger->set_level(log_level);
}

int main(int argc, char** argv)
{
  handle_arguments(argc, argv);

#ifdef _USE_CAPNP_
  KnowledgeBase from_kb;
  TransportSettings from_settings;
  from_settings.type = UDP;
  from_settings.send_history = true;
  from_settings.hosts.push_back("127.0.0.1:52312");
  from_settings.hosts.push_back("127.0.0.1:52313");
  from_settings.read_thread_hertz = 1;
  from_kb.attach_transport("udp_from", from_settings);

  KnowledgeBase to_kb;
  TransportSettings to_settings;
  to_settings.type = UDP;
  to_settings.hosts.push_back("127.0.0.1:52313");
  to_settings.hosts.push_back("127.0.0.1:52312");
  to_settings.read_thread_hertz = 10;
  to_kb.attach_transport("udp_to", to_settings);

  from_kb.set_history_capacity("cap3", 3);
  from_kb.set_history_capacity("cap5", 5);
  from_kb.set_history_capacity("cap9", 9);

  to_kb.set_history_capacity("cap3", 3);
  to_kb.set_history_capacity("cap5", 5);
  to_kb.set_history_capacity("cap9", 9);

  Int x("x", from_kb);
  Int cap3("cap3", from_kb);
  Int cap5("cap5", from_kb);
  Int cap9("cap9", from_kb);

  x = 42;
  cap3 = 1;
  cap5 = 11;
  cap9 = 21;

  from_kb.send_modifieds();
  utility::sleep(0.5);

  Int rx("x", to_kb);
  Int rcap3("cap3", to_kb);
  Int rcap5("cap5", to_kb);
  Int rcap9("cap9", to_kb);

  TEST_EQ(*x, 42);
  TEST_EQ(*rcap3, 1);
  TEST_EQ(*rcap5, 11);
  TEST_EQ(*rcap9, 21);

  TEST_EQ(to_kb.get_history_size("cap3"), 2UL);
  TEST_EQ(to_kb.get_history_size("cap5"), 2UL);
  TEST_EQ(to_kb.get_history_size("cap9"), 2UL);

  cap3 = 2;
  cap3 = 3;
  cap3 = 4;
  cap5 = 12;
  cap5 = 13;
  cap5 = 14;

  TEST_EQ(from_kb.get_oldest("cap3").to_integer(), 2);
  TEST_EQ(from_kb.get_history("cap5", -1).to_integer(), 14);
  TEST_EQ(from_kb.get_history("cap5", -2).to_integer(), 13);
  TEST_EQ(from_kb.get_history("cap5", -3).to_integer(), 12);
  TEST_EQ(from_kb.get_history("cap5", -4).to_integer(), 11);

  from_kb.send_modifieds();
  utility::sleep(0.5);

  TEST_EQ(*rcap3, 4);
  TEST_EQ(*rcap5, 14);
  TEST_EQ(*rcap9, 21);

  TEST_EQ(to_kb.get_history_size("cap3"), 3UL);
  TEST_EQ(to_kb.get_oldest("cap3").to_integer(), 2);
  TEST_EQ(to_kb.get_history_size("cap5"), 5UL);
  TEST_EQ(to_kb.get_history("cap5", -1).to_integer(), 14);
  TEST_EQ(to_kb.get_history("cap5", -2).to_integer(), 13);
  TEST_EQ(to_kb.get_history("cap5", -3).to_integer(), 12);
  TEST_EQ(to_kb.get_history("cap5", -4).to_integer(), 11);
  TEST_EQ(to_kb.get_history("cap5", -5).exists(), false);
  TEST_EQ(to_kb.get_history_size("cap9"), 2UL);
#else
  return 0;
#endif _USE_CAPNP_
}
