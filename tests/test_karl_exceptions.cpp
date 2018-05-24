#include "madara/knowledge/KnowledgeBase.h"
#include <iostream>

namespace knowledge = madara::knowledge;
namespace expression = madara::expression;

void test_functions (void)
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
  }
  catch (expression::KarlException &)
  {
    std::cerr << "SUCCESS\n";
  }


  std::cerr << "Testing undefined system function: ";
  try
  {
    knowledge.evaluate ("#not_defined ()");
    std::cerr << "FAIL\n";
  }
  catch (expression::KarlException &)
  {
    std::cerr << "SUCCESS\n";
  }
}

void test_nan (void)
{
  std::cerr << "#####################\n";
  std::cerr << "TESTING NAN FUNCTIONS\n";
  std::cerr << "#####################\n";
  knowledge::KnowledgeBase knowledge;

  knowledge.evaluate ("var1 = 1/0");
  knowledge.evaluate ("var2 = var1/0");
  knowledge.evaluate ("var3 = 1/var0");
  knowledge.evaluate ("var4 = #sqrt (-1)");
  knowledge.evaluate ("var5 = nan");
  knowledge.evaluate ("var6 = var5 != var5");

  std::cerr << "Testing divide by zero: ";
  if (knowledge.get ("var1") != knowledge.get ("var1"))
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n";
  }

  std::cerr << "Testing variable divide by zero: ";
  if (knowledge.get ("var2") != knowledge.get ("var2"))
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n";
  }

  std::cerr << "Testing divide by variable zero: ";
  if (knowledge.get ("var3") != knowledge.get ("var3"))
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n";
  }

  std::cerr << "Testing sqrt(-1): ";
  if (knowledge.get ("var4") != knowledge.get ("var4"))
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n";
  }

  std::cerr << "Testing assignment of nan: ";
  if (knowledge.get ("var5") != knowledge.get ("var5"))
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n";
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
    std::cerr << "var6 = " << knowledge.get ("var6") << "\n";
  }

  try
  {
    std::cerr << "Testing array creation with nan var name: ";
    knowledge.evaluate ("nan[3]");
    std::cerr << "FAIL\n";
  }
  catch (expression::KarlException &)
  {
    std::cerr << "SUCCESS\n";
  }

}

void test_empty_assignment (void)
{
  std::cerr << "#####################\n";
  std::cerr << "TESTING ASSIGNMENTS WITH NO LEFT-HAND VARS\n";
  std::cerr << "#####################\n";

  knowledge::KnowledgeBase knowledge;

  try
  {
    std::cerr << "Testing assignment (-=): ";
    knowledge.evaluate ("-= 5");
    std::cerr << "FAIL\n";
  }
  catch (expression::KarlException &)
  {
    std::cerr << "SUCCESS\n";
  }

  try
  {
    std::cerr << "Testing assignment (+=): ";
    knowledge.evaluate ("+= 5");
    std::cerr << "FAIL\n";
  }
  catch (expression::KarlException &)
  {
    std::cerr << "SUCCESS\n";
  }

  try
  {
    std::cerr << "Testing assignment (*=): ";
    knowledge.evaluate ("*= 5");
    std::cerr << "FAIL\n";
  }
  catch (expression::KarlException &)
  {
    std::cerr << "SUCCESS\n";
  }

  try
  {
    std::cerr << "Testing assignment (/=): ";
    knowledge.evaluate ("/= 5");
    std::cerr << "FAIL\n";
  }
  catch (expression::KarlException &)
  {
    std::cerr << "SUCCESS\n";
  }

  try
  {
    std::cerr << "Testing assignment (=): ";
    knowledge.evaluate ("= 5");
    std::cerr << "FAIL\n";
  }
  catch (expression::KarlException &)
  {
    std::cerr << "SUCCESS\n";
  }
}

int main (int, char **)
{
  test_functions ();
  test_nan ();
  test_empty_assignment ();

  return 0;
}

