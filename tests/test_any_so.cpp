#include <string>
#include <iostream>

#include "madara/knowledge/Any.h"
#include "madara/knowledge/KnowledgeRecord.h"
#include "madara/knowledge/KnowledgeBase.h"
#include "madara/utility/SupportTest.h"
#include "test.h"

namespace logger = madara::logger;

using namespace madara;
using namespace knowledge;

int main(int, char**)
{

#ifdef _USE_CAPNP_
  Any any = Any::construct("Point");
  std::cerr << any << std::endl;

  any.emplace("Pose");
  std::cerr << any << std::endl;

  any.emplace("Trajectory");
  std::cerr << any << std::endl;

  if (madara_tests_fail_count > 0)
  {
    std::cerr << "OVERALL: FAIL. " << madara_tests_fail_count
              << " tests failed.\n";
  }
  else
  {
    std::cerr << "OVERALL: SUCCESS.\n";
  }
#endif   // _USE_CAPNP_

  return madara_tests_fail_count;
}
