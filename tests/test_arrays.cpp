
#include <string>
#include <vector>
#include <iostream>
#include <assert.h>

#include "madara/knowledge/KnowledgeBase.h"
#include "madara/utility/Utility.h"
#include "madara/logger/GlobalLogger.h"

namespace logger = madara::logger;
namespace utility = madara::utility;
typedef madara::knowledge::KnowledgeRecord::Integer Integer;

std::string host("");
const std::string default_multicast("239.255.0.1:4150");
madara::transport::TransportSettings settings;

// command line arguments
int parse_args(int argc, char* argv[]);

// test functions
void create_arrays(madara::knowledge::KnowledgeBase& knowledge);
void write_transported_arrays(madara::knowledge::KnowledgeBase& knowledge);

int main(int argc, char* argv[])
{
  settings.hosts.resize(1);
  settings.hosts[0] = default_multicast;
  int retcode = parse_args(argc, argv);

  // set type to multicast and change queue length to 512K
  settings.type = madara::transport::MULTICAST;
  settings.queue_length = 512000;
  madara::knowledge::WaitSettings wait_settings;
  wait_settings.max_wait_time = 10;
  wait_settings.poll_frequency = 1.0;
  wait_settings.delay_sending_modifieds = false;

  if (settings.id == 1)
  {
    wait_settings.pre_print_statement =
        "Waiting on finished_transmitting to change from process.id==1...\n";
  }

  if (retcode < 0)
    return retcode;

  madara::knowledge::KnowledgeBase knowledge("", settings);

  knowledge.set(".id", (madara::knowledge::KnowledgeRecord::Integer)settings.id,
      madara::knowledge::EvalSettings::SEND);

  // run tests
  //  test_tree_compilation (knowledge);
  if (settings.id == 0)
    create_arrays(knowledge);
  else
  {
    utility::wait_true(knowledge, "finished_transmitting", wait_settings);
    write_transported_arrays(knowledge);
  }

  // knowledge.print ();

  return 0;
}

/// Tests logicals operators (&&, ||)
void create_arrays(madara::knowledge::KnowledgeBase& knowledge)
{
  knowledge.clear();

  std::vector<double> doubles_vector;
  doubles_vector.resize(5);
  doubles_vector[0] = 2.0 / 3;
  doubles_vector[1] = 5.0 / 4;
  doubles_vector[2] = -3.14159;
  doubles_vector[3] = 42.0;
  doubles_vector[4] = 3000.5238;

  knowledge.set(
      "doubles_vector", doubles_vector, madara::knowledge::EvalSettings::SEND);

  madara::knowledge::KnowledgeRecord::Integer* integer_array =
      new madara::knowledge::KnowledgeRecord::Integer[3];
  integer_array[0] = 0;
  integer_array[1] = 1;
  integer_array[2] = 2;

  knowledge.set("integers_vector", integer_array, 3,
      madara::knowledge::EvalSettings::SEND);

#ifndef _MADARA_NO_KARL_
  knowledge.evaluate(
      "var_array[1] = 3.0", madara::knowledge::EvalSettings::SEND);
  knowledge.evaluate(
      "var_array[3] = 20", madara::knowledge::EvalSettings::SEND);
#else
  knowledge.set_index(
      "var_array", 1, 3.0, madara::knowledge::EvalSettings::SEND);
  knowledge.set_index(
      "var_array", 3, Integer(3.0), madara::knowledge::EvalSettings::SEND);
#endif  // _MADARA_NO_KARL_

  knowledge.set(
      "finished_transmitting", 1, madara::knowledge::EvalSettings::SEND);
  knowledge.print("Sending the following data sets to id 1\n");
  knowledge.print("doubles_vector = [{doubles_vector}]\n");
  knowledge.print("integers_vector = [{integers_vector}]\n");
  knowledge.print("var_array = [{var_array}]\n");

  madara::knowledge::KnowledgeRecord::set_precision(10);

  knowledge.print("Setting precision to 10 and reprinting\n");
  knowledge.print("doubles_vector = [{doubles_vector}]\n");
  knowledge.print("integers_vector = [{integers_vector}]\n");
  knowledge.print("var_array = [{var_array}]\n");

  delete[] integer_array;
}

/// Tests logicals operators (&&, ||)
void write_transported_arrays(madara::knowledge::KnowledgeBase& knowledge)
{
  knowledge.print("\nReceived the following arrays from id 0\n");
  knowledge.print("doubles_vector = [{doubles_vector}]\n");
  knowledge.print("integers_vector = [{integers_vector}]\n");
  knowledge.print("var_array = [{var_array}]\n\n");

  madara::knowledge::KnowledgeRecord doubles_vector =
      knowledge.get("doubles_vector");
  doubles_vector.set_index(5, 127.25);
  doubles_vector.set_index(6, 1.2575);

  std::cerr << "adding two doubles (127.25, 1.2575) to the doubles_vector\n";
  std::cerr << "doubles_vector = [" << doubles_vector.to_string(", ") << "]"
            << std::endl;

  madara::knowledge::KnowledgeRecord integers_vector =
      knowledge.get("integers_vector");

  std::cerr << "\nintegers_vector = [" << integers_vector.to_string(", ") << "]"
            << std::endl;

  integers_vector.set_index(7, madara::knowledge::KnowledgeRecord::Integer(7));

  std::cerr << "adding one integer (7) to the integers_vector\n";

  std::cerr << "integers_vector = [" << integers_vector.to_string(", ") << "]"
            << std::endl;

  std::cerr << "adding one double (6.0) to the integers_vector\n";

  integers_vector.set_index(6, 6.0);

  std::cerr << "integers_vector = [" << integers_vector.to_string(", ")
            << "]\n\n";

  knowledge.print("var_array = [{var_array}]\n");
  std::cerr << "copying elements of var_array to var_array\n";

#ifndef _MADARA_NO_KARL_
  knowledge.evaluate(
      "var_array[0] = var_array[3]", madara::knowledge::EvalSettings::SEND);
  knowledge.evaluate(
      "var_array[2] = var_array[1]", madara::knowledge::EvalSettings::SEND);
  knowledge.evaluate(
      "var_array[8] = 100.012", madara::knowledge::EvalSettings::SEND);
#else
  knowledge.set_index("var_array", 0,
      knowledge.retrieve_index("var_array", 3).to_double(),
      madara::knowledge::EvalSettings::SEND);
  knowledge.set_index("var_array", 2,
      knowledge.retrieve_index("var_array", 1).to_double(),
      madara::knowledge::EvalSettings::SEND);
  knowledge.set_index(
      "var_array", 8, 100.012, madara::knowledge::EvalSettings::SEND);
#endif  // _MADARA_NO_KARL_

  knowledge.print("var_array = [{var_array}]\n\n");
}

int parse_args(int argc, char* argv[])
{
  for (int i = 1; i < argc; ++i)
  {
    std::string arg1(argv[i]);

    if (arg1 == "-m" || arg1 == "--multicast")
    {
      if (i + 1 < argc)
        settings.hosts[0] = argv[i + 1];

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
        int level;
        std::stringstream buffer(argv[i + 1]);
        buffer >> level;

        logger::global_logger->set_level(level);
      }

      ++i;
    }
    else
    {
      madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_ALWAYS,
          "\nProgram summary for %s:\n\n"
          "This test checks the functionality of file reading/writing. To "
          "properly\n"
          "test file functionality, please create a file at "
          "/files/sample.jpg,\n"
          "preferably with a real image. Check the sample_copy.jpg for "
          "correctness\n\n",
          " [-o|--host hostname]     the hostname of this process "
          "(def:localhost)\n"
          " [-m|--multicast ip:port] the multicast ip to send and listen to\n"
          " [-d|--domain domain]     the knowledge domain to send and listen "
          "to\n"
          " [-i|--id id]             the id of this agent (should be "
          "non-negative)\n"
          " [-l|--level level]       the logger level (0+, higher is higher "
          "detail)\n"
          "\n",
          argv[0]);

      exit(0);
    }
  }

  return 0;
}
