
#include <string>
#include <vector>
#include <iostream>
#include <assert.h>

#include "madara/knowledge/KnowledgeBase.h"

typedef madara::knowledge::KnowledgeRecord::Integer Integer;

int madara_fails = 0;

// test functions
void test_print_statement(madara::knowledge::KnowledgeBase& knowledge);

int main(int, char**)
{
  madara::knowledge::KnowledgeBase knowledge;

  // run tests
  test_print_statement(knowledge);

  knowledge.print();

  return 0;
}

/// Tests logicals operators (&&, ||)
void test_print_statement(madara::knowledge::KnowledgeBase& knowledge)
{
  std::string statement, exp_statement;

  knowledge.clear();

#ifndef _MADARA_NO_KARL_
  knowledge.evaluate(".var1 = 1; \
    .var2 = 2; \
    .var3 = 3; \
    .var2is2 = 1; \
    .var22 = 2; \
    MyState.1.2 = 10; \
    ");

#else
  knowledge.set(".var1", Integer(1));
  knowledge.set(".var2", Integer(2));
  knowledge.set(".var3", Integer(3));
  knowledge.set(".var2is2", Integer(1));
  knowledge.set(".var22", Integer(2));
  knowledge.set("MyState.1.2", Integer(10));
#endif

  statement = "Hello kitty {.var1}\n";

  exp_statement = knowledge.expand_statement(statement);
  std::cerr << "Test 1: ";
  if (statement == "Hello kitty 1")
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n";
    ++madara_fails;
  }

  statement = "Hello kitty {.var{.var2}is{.var22}}\n";

  exp_statement = knowledge.expand_statement(statement);
  std::cerr << "Test 2: ";
  if (statement == "Hello kitty 1")
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n";
    ++madara_fails;
  }

  statement = "MyState.{.var1}.{.var2} = {MyState.{.var1}.{.var2}}\n";

  exp_statement = knowledge.expand_statement(statement);
  std::cerr << "Test 3: ";
  if (statement == "MyState.1.2 = 10")
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n";
    ++madara_fails;
  }
}
