
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

void handle_arguments (int argc, char ** argv)
{
  for (int i = 1; i < argc; ++i)
  {
    std::string arg1 (argv[i]);
    
    if (argc > 1)
    {
      std::cout <<
        "\nProgram Summary for " << argv[0] << ":\n\n" \
        "  This stand-alone application tests the safe-bool feature of\n"
        "  KnowledgeRecord included in the MADARA distribution.\n"
        "\n";
      exit (0);
    }
  }
}

int main (int argc, char ** argv)
{
  handle_arguments (argc, argv);

  mk::KnowledgeRecord truthy(42L), falsey(0L), struthy("true"), sfalsey("");

  if(truthy)
  {
    std::cout << "SUCCESS  truthy is true" << std::endl;
  }
  else
  {
    std::cout << "ERROR    truthy is false" << std::endl;
  }

  if(falsey)
  {
    std::cout << "ERROR    falsey is true" << std::endl;
  }
  else
  {
    std::cout << "SUCCESS  falsey is false" << std::endl;
  }

  if(truthy && struthy)
  {
    std::cout << "SUCCESS  truthy && struthy is true" << std::endl;
  }
  else
  {
    std::cout << "ERROR    truthy && struthy is false" << std::endl;
  }

  if(truthy && sfalsey)
  {
    std::cout << "ERROR    truthy && sfalsey is true" << std::endl;
  }
  else
  {
    std::cout << "SUCCESS  truthy && sfalsey is false" << std::endl;
  }

  if(truthy || struthy)
  {
    std::cout << "SUCCESS  truthy || sfalsey is true" << std::endl;
  }
  else
  {
    std::cout << "ERROR    truthy || sfalsey is false" << std::endl;
  }

  // These lines compile OK, and work as expected:
  bool bool_false = falsey;
  bool bool_true = truthy;

  if(bool_true)
  {
    std::cout << "SUCCESS  bool_true is true" << std::endl;
  }
  else
  {
    std::cout << "ERROR    bool_true is false" << std::endl;
  }

  if(bool_false)
  {
    std::cout << "ERROR    bool_false is true" << std::endl;
  }
  else
  {
    std::cout << "SUCCESS  bool_false is false" << std::endl;
  }

  // The following lines should cause compile errors if uncommented:

  // naive operator bool() would compile the following:
  //int bad_int = truthy; // bad_int would unexpectedly have 1, not 42
  //int bad_shift = truthy << 3;

  // naive Safe Bool returning void* would compile the following:
  //delete truthy;

  return 0;
}
