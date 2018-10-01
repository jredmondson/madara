
#include <string>
#include <vector>
#include <iostream>
#include <assert.h>

#include "madara/knowledge/KnowledgeBase.h"

// test functions
void test_expansion(madara::knowledge::KnowledgeBase& knowledge);

int main(int, char**)
{
  madara::knowledge::KnowledgeBase knowledge;

  // run tests
  test_expansion(knowledge);

  knowledge.print();

  // asserts will show failures
  std::cerr << "OVERALL: SUCCESS.\n";

  return 0;
}

/// tests key expansion
void test_expansion(madara::knowledge::KnowledgeBase& knowledge)
{
  madara::knowledge::KnowledgeRecord result;
  knowledge.clear();

#ifndef _MADARA_NO_KARL_
  knowledge.evaluate(".var1 = 1; \
    .var2 = 2; \
    .var3 = 3; \
    .var2is2 = 1; \
    .var22 = 2;");

  result = knowledge.evaluate("v{.var2}{.var2} = 47");
  assert(
      knowledge.get("v22") == (madara::knowledge::KnowledgeRecord::Integer)47);

  result = knowledge.evaluate(".var{.var2}{.var2}");
  assert(result == (madara::knowledge::KnowledgeRecord::Integer)2);

  result = knowledge.evaluate(".var{.var2}is{.var2}");
  assert(result == (madara::knowledge::KnowledgeRecord::Integer)1);

  // test variables in conditionals
  knowledge.evaluate("var{.var3} = .var{.var2}");
  assert(
      knowledge.get("var3") == (madara::knowledge::KnowledgeRecord::Integer)2);

#else
  std::cout << "This test is disabled due to karl feature being disabled.\n";
#endif  // _MADARA_NO_KARL_
}
