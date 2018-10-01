
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <assert.h>
#include <math.h>
#include <iomanip>
#include <algorithm>

#include <madara/knowledge/KnowledgeRecord.h>

namespace mk = madara::knowledge;

int madara_fails = 0;

void handle_arguments(int argc, char** argv)
{
  for (int i = 1; i < argc; ++i) {
    std::string arg1(argv[i]);

    if (argc > 1) {
      std::cout
          << "\nProgram Summary for " << argv[0]
          << ":\n\n"
             "  This stand-alone application tests the safe-bool feature of\n"
             "  KnowledgeRecord included in the MADARA distribution.\n"
             "\n";
      exit(0);
    }
  }
}

int main(int argc, char** argv)
{
  handle_arguments(argc, argv);

  mk::KnowledgeRecord truthy(42);
  mk::KnowledgeRecord falsey(0);
  mk::KnowledgeRecord struthy("true");
  mk::KnowledgeRecord sfalsey("");

  if (truthy) {
    std::cout << "SUCCESS  truthy is true" << std::endl;
  } else {
    std::cout << "FAIL    truthy is false" << std::endl;
    ++madara_fails;
  }

  if (falsey) {
    std::cout << "FAIL    falsey is true" << std::endl;
    ++madara_fails;
  } else {
    std::cout << "SUCCESS  falsey is false" << std::endl;
  }

  if (truthy && struthy) {
    std::cout << "SUCCESS  truthy && struthy is true" << std::endl;
  } else {
    std::cout << "FAIL    truthy && struthy is false" << std::endl;
    ++madara_fails;
  }

  if (truthy && sfalsey) {
    std::cout << "FAIL    truthy && sfalsey is true" << std::endl;
    ++madara_fails;
  } else {
    std::cout << "SUCCESS  truthy && sfalsey is false" << std::endl;
  }

  if (truthy || struthy) {
    std::cout << "SUCCESS  truthy || sfalsey is true" << std::endl;
  } else {
    std::cout << "FAIL    truthy || sfalsey is false" << std::endl;
    ++madara_fails;
  }

  // These lines compile OK, and work as expected:
  bool bool_false = (bool)falsey;
  bool bool_true = (bool)truthy;

  if (bool_true) {
    std::cout << "SUCCESS  bool_true is true" << std::endl;
  } else {
    std::cout << "FAIL    bool_true is false" << std::endl;
    ++madara_fails;
  }

  if (bool_false) {
    std::cout << "FAIL    bool_false is true" << std::endl;
    ++madara_fails;
  } else {
    std::cout << "SUCCESS  bool_false is false" << std::endl;
  }

  // The following lines should cause compile errors if uncommented:

  // naive operator bool() would compile the following:
  // int bad_int = truthy; // bad_int would unexpectedly have 1, not 42
  // int bad_shift = truthy << 3;

  // naive Safe Bool returning void* would compile the following:
  // delete truthy;

  if (madara_fails > 0) {
    std::cerr << "OVERALL: FAIL. " << madara_fails << " tests failed.\n";
  } else {
    std::cerr << "OVERALL: SUCCESS.\n";
  }

  return madara_fails;
}
