
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <assert.h>
#include <iomanip>
#include <algorithm>

#include "madara/knowledge/KnowledgeBase.h"
#include "madara/logger/GlobalLogger.h"

#include "madara/utility/Utility.h"
#include "madara/utility/Timer.h"

namespace logger = madara::logger;

std::vector<std::string> tests;
std::vector<uint64_t> max_times;
std::vector<uint64_t> min_times;
std::vector<uint64_t> average_times;
std::vector<uint64_t> compile_times;
#ifndef _MADARA_NO_KARL_
std::vector<madara::knowledge::CompiledExpression> compiled_expressions;
#endif  // _MADARA_NO_KARL_

std::string profile_file =
    "$(MADARA_ROOT)/tests/settings/profile_expressions.txt";

void handle_arguments(int argc, char** argv)
{
  for (int i = 1; i < argc; ++i)
  {
    std::string arg1(argv[i]);

    if (arg1 == "-f" || arg1 == "--profile")
    {
      if (i + 1 < argc)
        profile_file = argv[i + 1];

      ++i;
    }
    else
    {
      madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_ALWAYS,
          "\nProgram arguments for %s:\n"
          " [-f|--profile profile]  the expression profile. This profile "
          "contains\n"
          "                         a list of KaRL expression to evaluate and "
          "profile.\n"
          "                         "
          "$(MADARA_ROOT)/tests/settings/profile_expressions.txt\n"
          "                         is an example expression profile.\n"
          "\n",
          argv[0]);
      exit(0);
    }
  }
}

std::string to_legible_hertz(uint64_t hertz)
{
  std::stringstream buffer;

  std::locale loc("C");
  buffer.imbue(loc);

  const int ghz_mark = 1000000000;
  const int mhz_mark = 1000000;
  const int khz_mark = 1000;

  double freq = (double)hertz / ghz_mark;

  if (freq >= 1)
  {
    buffer << std::setprecision(2) << std::fixed;
    buffer << freq;
    buffer << " ghz";
    return buffer.str().c_str();
  }

  freq = (double)hertz / mhz_mark;

  if (freq >= 1)
  {
    buffer << std::setprecision(2) << std::fixed;
    buffer << freq;
    buffer << " mhz";
    return buffer.str().c_str();
  }

  freq = (double)hertz / khz_mark;

  if (freq >= 1)
  {
    buffer << std::setprecision(2) << std::fixed;
    buffer << freq;
    buffer << " khz";
    return buffer.str().c_str();
  }

  freq = (double)hertz;

  buffer << std::setprecision(2) << std::fixed;
  buffer << freq;
  buffer << "  hz";
  return buffer.str().c_str();
}

#ifndef _MADARA_NO_KARL_
void warmup(madara::knowledge::KnowledgeBase& knowledge)
{
  madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_ALWAYS,
      "Warming up compilation and evaluation caches...\n");

  knowledge.compile("var1 = 10");
  knowledge.compile("var2 = 15");
  knowledge.compile("++var3");
  knowledge.compile("++var4");

  for (unsigned int i = 0; i < 50000; ++i)
    knowledge.evaluate("++var2");

  for (unsigned int i = 0; i < 50000; ++i)
    knowledge.evaluate("var2 += 1");
}

void compile_expressions(madara::knowledge::KnowledgeBase& knowledge)
{
  for (unsigned int i = 0; i < tests.size(); ++i)
  {
    // keep track of time
    uint64_t measured(0);
    madara::utility::Timer<std::chrono::steady_clock> timer;

    timer.start();
    compiled_expressions[i] = knowledge.compile(tests[i]);
    timer.stop();

    measured = timer.duration_ns();

    compile_times[i] = measured;

    if (tests[i].size() > 18)
      tests[i].resize(18);
  }
}

void evaluate_expressions(madara::knowledge::KnowledgeBase& knowledge)
{
  for (unsigned int i = 0; i < tests.size(); ++i)
  {
    // set the min time to the max possible
    min_times[i] = 0 - 1;

    // set the max time to 0
    max_times[i] = 0;

    madara::utility::Timer<std::chrono::steady_clock> overall_timer;
    madara::knowledge::EvalSettings defaults;
    uint64_t evaluate_time(0);

    madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_ALWAYS,
        "  [%d] Evaluating %s\n", i, tests[i].c_str());

    // try to establish min/max times with 10 runs
    for (unsigned int j = 0; j < 100; ++j)
    {
      madara::utility::Timer<std::chrono::steady_clock> timer;

      timer.start();
      knowledge.evaluate(compiled_expressions[i], defaults);
      timer.stop();

      evaluate_time = timer.duration_ns();

      // update min and max times
      min_times[i] = std::min<uint64_t>(min_times[i], evaluate_time);
      max_times[i] = std::max<uint64_t>(max_times[i], evaluate_time);
    }

    // get an overall_timer for 1000 of the ops for a useful average
    overall_timer.start();
    for (unsigned int j = 0; j < 10000; ++j)
    {
      knowledge.evaluate(compiled_expressions[i], defaults);
    }
    overall_timer.stop();

    // compute the average time
    evaluate_time = overall_timer.duration_ns();
    average_times[i] = evaluate_time / 10000;
  }
}

#endif  // _MADARA_NO_KARL_

void print_results(void)
{
  madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_ALWAYS,
      "\n%-18s|%-13s|%-13s|%-13s|%-13s\n\n", "Expression", "Compile time",
      "Min eval time", "Max eval time", "Avg eval time");
  // std::cout << "\n|" << std::setw (18) << "Expression" << "|" <<
  //                    std::setw (13) << "Compile time" << "|" <<
  //                    std::setw (13) << "Min eval time" << "|" <<
  //                    std::setw (13) << "Max eval time" << "|" <<
  //                    std::setw (13) << "Avg eval time" << "|\n\n";
  for (unsigned int i = 0; i < tests.size(); ++i)
  {
    madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_ALWAYS,
        "%-18s|%-13llu|%-13llu|%-13llu|%-13llu\n",
        tests[i].substr(0, 17).c_str(), compile_times[i], min_times[i],
        max_times[i], average_times[i]);

    // std::cout << std::setw (19) << tests[i] << " ";
    // std::cout << std::setw (13) << compile_times[i] << " ";
    // std::cout << std::setw (13) << min_times[i] << " ";
    // std::cout << std::setw (13) << max_times[i] << " ";
    // std::cout << std::setw (13) << average_times[i];
    // std::cout << "\n";
  }
}

int main(int argc, char** argv)
{
  handle_arguments(argc, argv);

#ifndef _MADARA_NO_KARL_
  // set thread priority to high
  madara::utility::set_thread_priority();

  madara::knowledge::KnowledgeBase knowledge;

  // read the profile expressions into a string
  madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_ALWAYS,
      "\nReading expressions from:\n\n%s\n\n",
      madara::utility::clean_dir_name(
          madara::utility::expand_envs(profile_file))
          .c_str());

  madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_ALWAYS,
      "*****************************************************************\n"
      "* If you'd like to change which expressions are being profiled, *\n"
      "* simply change the above file.                                 *\n"
      "*****************************************************************\n\n");

  std::string expressions_text =
      madara::utility::file_to_string(madara::utility::clean_dir_name(
          madara::utility::expand_envs(profile_file)));

  // remove pesky control characters that may mess with tokenizer printing
  madara::utility::string_remove(expressions_text, '\r');

  // split the expressions_text by newlines into a tests vector of strings
  std::vector<std::string> splitters(1, "\n");
  std::vector<std::string> pivot_list;

  madara::utility::tokenizer(expressions_text, splitters, tests, pivot_list);

  if (tests.size() > 0)
  {
    // resize all the vectors that rely on the size of this list of expressions
    max_times.resize(tests.size());
    min_times.resize(tests.size());
    average_times.resize(tests.size());
    compile_times.resize(tests.size());
    compiled_expressions.resize(tests.size());

    // use locale settings to print large numbers with commas
    std::locale loc("C");
    std::cout.imbue(loc);

    warmup(knowledge);

    madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_ALWAYS,
        "Compiling all expressions...\n");
    compile_expressions(knowledge);

    madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_ALWAYS,
        "Evaluating all expressions 10,000 times...\n");
    evaluate_expressions(knowledge);

    madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_ALWAYS,
        "\nResults of system profile (times in ns):\n");
    print_results();
  }
  else
  {
    madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_ALWAYS,
        "\n*********************************************************\n"
        "No tests in %s. Potential culprits are:\n\n"
        "1) environment variables have not been set properly for ACE or "
        "MADARA\n"
        "2) the path you specified to -f was invalid or no read "
        "permission\n\n"
        "*********************************************************\n",
        profile_file.c_str());
  }

#else
  madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_ALWAYS,
      "This test is disabled due to karl feature being disabled.\n");
#endif  // _MADARA_NO_KARL_
  return 0;
}
