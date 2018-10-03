
#include <iostream>
#include <stdint.h>

#include "madara/knowledge/KnowledgeBase.h"
#include "madara/knowledge/containers/String.h"
#include "madara/tools/karl.cpp"

namespace knowledge = madara::knowledge;
namespace expression = madara::expression;
namespace exceptions = madara::exceptions;

int madara_fails (0);

void test_basic_config_file (void)
{
  std::cerr << "#####################\n";
  std::cerr << "TESTING UNDEFINED FUNCTIONS\n";
  std::cerr << "#####################\n";
  knowledge::KnowledgeBase knowledge;

  std::cerr << "Testing undefined function: ";
  try
  {
    knowledge.evaluate ("not_defined ()");
    std::cerr << "FAIL\n";
    ++madara_fails;
  }
  catch (exceptions::KarlException &)
  {
    std::cerr << "SUCCESS\n";
  }


  std::cerr << "Testing undefined system function: ";
  try
  {
    knowledge.evaluate ("#not_defined ()");
    std::cerr << "FAIL\n";
    ++madara_fails;
  }
  catch (exceptions::KarlException &)
  {
    std::cerr << "SUCCESS\n";
  }
}



void test_recursive_config_file (void)
{
  std::cerr << "#####################\n";
  std::cerr << "TESTING NAN FUNCTIONS\n";
  std::cerr << "#####################\n";
  knowledge::KnowledgeBase knowledge;

  std::cerr << "Evaluating var1 = 1/0...\n";
  knowledge.evaluate ("var1 = 1/0");

  std::cerr << "Evaluating var2 = var1/0...\n";
  knowledge.evaluate ("var2 = var1/0");

  std::cerr << "Evaluating var3 = 1/var0...\n";
  knowledge.evaluate ("var3 = 1/var0");

  std::cerr << "Evaluating var4 = #sqrt (-1)...\n";
  knowledge.evaluate ("var4 = #sqrt (-1)");

  std::cerr << "Evaluating var5 = nan...\n";
  knowledge.evaluate ("var5 = nan");

  std::cerr << "Evaluating var6 = var5 != var5...\n";
  knowledge.evaluate ("var6 = var5 != var5");

  std::cerr << "Testing divide by zero: ";
  if (knowledge.get ("var1") != knowledge.get ("var1"))
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n";
    ++madara_fails;
  }

  std::cerr << "Testing variable divide by zero: ";
  if (knowledge.get ("var2") != knowledge.get ("var2"))
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n";
    ++madara_fails;
  }

  std::cerr << "Testing divide by variable zero: ";
  if (knowledge.get ("var3") != knowledge.get ("var3"))
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n";
    ++madara_fails;
  }

  std::cerr << "Testing sqrt(-1): ";
  if (knowledge.get ("var4") != knowledge.get ("var4"))
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n";
    ++madara_fails;
  }

  std::cerr << "Testing assignment of nan: ";
  if (knowledge.get ("var5") != knowledge.get ("var5"))
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n";
    ++madara_fails;
    std::cerr << "var5 = " << knowledge.get ("var5") << "\n";
  }

  std::cerr << "Testing nan check (var != var): ";
  if (knowledge.get ("var6").is_true ())
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n";
    ++madara_fails;
    std::cerr << "var6 = " << knowledge.get ("var6") << "\n";
  }

  try
  {
    std::cerr << "Testing array creation with nan var name: ";
    knowledge.evaluate ("nan[3]");
    std::cerr << "FAIL\n";
    ++madara_fails;
  }
  catch (exceptions::KarlException &)
  {
    std::cerr << "SUCCESS\n";
  }

}
  

void test_min (void)
{
  knowledge::KnowledgeBase knowledge;

  std::cerr << "#####################\n";
  std::cerr << "TESTING MIN/MAX INTEGERS\n";
  std::cerr << "#####################\n";

  knowledge.set (".min_int", INT64_MIN);
  knowledge.set (".max_int", INT64_MAX);

  std::string karl_min = knowledge.expand_statement ("min_int = {.min_int}");
  std::string karl_max = knowledge.expand_statement ("max_int = {.max_int}");

  knowledge.evaluate (karl_min);
  knowledge.evaluate (karl_max);

  int64_t local_min = knowledge.get (".min_int").to_integer ();
  int64_t local_max = knowledge.get (".max_int").to_integer ();
  int64_t global_min = knowledge.get ("min_int").to_integer ();
  int64_t global_max = knowledge.get ("max_int").to_integer ();

  std::cerr << "Testing " << karl_min << " evaluation: ";
  
  if (local_min == global_min)
    std::cerr << "SUCCESS\n";
  else
  {
    std::cerr << "FAIL.";
    ++madara_fails;
    std::cerr << " .min_int=" << local_min << ", " <<
      "min_int=" << global_min << "\n";
  }

  std::cerr << "Testing " << karl_max << " evaluation: ";

  if (local_max == global_max)
    std::cerr << "SUCCESS\n";
  else
  {
    std::cerr << "FAIL.";
    ++madara_fails;
    std::cerr << " .max_int=" << local_max << ", " <<
      "max_int=" << global_max << "\n";
  }

}

int main (int, char **)
{
  test_functions ();
  test_nan ();
  test_min ();
  test_empty_assignment ();
  test_bad_operators ();

  if (madara_fails > 0)
  {
    std::cerr << "OVERALL: FAIL. " << madara_fails << " tests failed.\n";
  }
  else
  {
    std::cerr << "OVERALL: SUCCESS.\n";
  }

  return madara_fails;
}

