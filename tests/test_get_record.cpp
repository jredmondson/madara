
#include "madara/knowledge/KnowledgeRecord.h"
#include "madara/knowledge/GetRecord.h"

#include "test.h"

using namespace madara::knowledge;
using namespace madara::exceptions;

#define EXPECT_EXCEPTION(exception_type, expr) \
  try { \
    (expr); \
    log("FAIL    : %s did not throw %s\n", #expr, #exception_type); \
    madara_tests_fail_count++; \
  } catch (exception_type e) { \
    log("SUCCESS : %s threw %s\n", #expr, #exception_type); \
  }


int main(){
  KnowledgeRecord kr_int(4);
  TEST_EQ(4, get<int>(kr_int));
  TEST_EQ(4, get<char>(kr_int));
  TEST_EQ(true, get<bool>(kr_int));

  EXPECT_EXCEPTION(MissingValueException, get<int>(KnowledgeRecord()));
  EXPECT_EXCEPTION(MismatchedTypeException, get<int>(KnowledgeRecord(2.0)));
  EXPECT_EXCEPTION(MismatchedTypeException, get<int>(KnowledgeRecord("test")));

  KnowledgeRecord kr_double(4.0);
  TEST_EQ(4.0, get<double>(kr_double));
  TEST_EQ(4.0, get<float>(kr_double));

  EXPECT_EXCEPTION(MissingValueException, get<double>(KnowledgeRecord()));
  EXPECT_EXCEPTION(MismatchedTypeException, get<double>(KnowledgeRecord(2)));
  EXPECT_EXCEPTION(MismatchedTypeException, get<int>(KnowledgeRecord("test")));

  TEST_EQ("test", get<std::string>(KnowledgeRecord("test")));

  std::vector<double> double_arr {1.2, 1.4};
  KnowledgeRecord kr_double_arr(double_arr);
  get<std::vector<double>>(kr_double_arr);
  get<std::vector<float>>(kr_double_arr);
  EXPECT_EXCEPTION(MismatchedTypeException, get<std::vector<int>>(kr_double_arr));
  EXPECT_EXCEPTION(MismatchedTypeException, get<double>(kr_double_arr));

  std::vector<int64_t> int_arr {1, 4};
  KnowledgeRecord kr_int_arr(int_arr);
  get<std::vector<int>>(kr_int_arr);
  get<std::vector<bool>>(kr_int_arr);
  EXPECT_EXCEPTION(MismatchedTypeException, get<std::vector<double>>(kr_int_arr));
  EXPECT_EXCEPTION(MismatchedTypeException, get<int>(kr_int_arr));

  if (madara_tests_fail_count > 0)
  {
    std::cerr << "OVERALL: FAIL. " << madara_tests_fail_count <<
      " tests failed.\n";
  }
  else
  {
    std::cerr << "OVERALL: SUCCESS.\n";
  }

  return madara_tests_fail_count;

}