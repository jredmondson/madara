#include <string>
#include <atomic>
#include <iostream>

#include "madara/knowledge/ThreadSafeContext.h"
#include "madara/knowledge/KnowledgeBase.h"
#include "madara/utility/Utility.h"

#include "test.h"

using namespace madara;
using namespace knowledge;
namespace logger = madara::logger;

typedef KnowledgeRecord::Integer Integer;

void test_unshared_record(void)
{
  KnowledgeRecord rec;

  std::string str = "Hello World";
  const char* orig_ptr = str.c_str();
  rec.set_value(std::move(str));

  std::string str_out = rec.to_string();

  VAL(orig_ptr != str_out.c_str());

  std::cerr << "test_unshared_record: madara_fails: " << madara_tests_fail_count
            << "\n";
}

void test_shared_record(void)
{
  KnowledgeBase kb;
  KnowledgeRecord rec;

  std::string str = "Hello World";
  auto s1 = mk_unique<std::string>(std::move(str));
  const char* orig_ptr = s1->c_str();
  rec.set_value(std::move(s1));

  std::shared_ptr<const std::string> str_out = rec.share_string();

  TEST_EQ(orig_ptr, str_out->c_str());

  std::string big_str =
      "This is a string that might be much longer and be expensive to copy.";
  const char* orig_sptr = big_str.c_str();

  // Creates a vector with 4000 entries, all 42, without any copying
  KnowledgeRecord ints(tags::integers, 4000, 42);

  std::shared_ptr<const std::vector<int64_t>> iptr = ints.share_integers();
  TEST_NE(iptr.get(), (void*)0);

  const int64_t* orig_iptr = &(*iptr)[0];

  std::unique_ptr<std::vector<double>> unique_dptr(
      new std::vector<double>(iptr->begin(), iptr->end()));
  double* orig_dptr = &(*unique_dptr)[0];
  *orig_dptr += 0.5;
  KnowledgeRecord dbls(std::move(unique_dptr));

  // std::move avoids copying the string data
  kb.set(".my_string", std::move(big_str));

  // std::move not needed here to avoid copying the integer data,
  kb.set(".my_array", ints);

  // but would be slightly more efficient as it would
  // avoid touching ref counts
  kb.set(".my_doubles", std::move(dbls));

  TEST_EQ(kb.get(".my_array").retrieve_index(0).to_integer(), 42);

  // shared with .my_string
  std::shared_ptr<const std::string> big_str_out =
      kb.share_string(".my_string");
  const char* out_sptr = big_str_out->c_str();

  // shared with .my_array still in kb
  std::shared_ptr<const std::vector<int64_t>> ints_out =
      kb.share_integers(".my_array");
  const int64_t* out_iptr = &(*ints_out)[0];

  std::shared_ptr<const std::vector<double>> dbls_out =
      kb.share_doubles(".my_doubles");
  const double* out_dptr = &(*dbls_out)[0];

  TEST_EQ((void*)orig_sptr, (void*)out_sptr);
  TEST_EQ((void*)orig_iptr, (void*)out_iptr);
  TEST_EQ((void*)orig_dptr, (void*)out_dptr);

  TEST_EQ(ints_out->at(0), 42);
  TEST_EQ(dbls_out->at(0), 42.5);

  // Causes a copy to be made, so we can modify without changing ints_out
  kb.set_index(".my_array", 0, 47);

  TEST_EQ(kb.get(".my_array").retrieve_index(0).to_integer(), 47);
  TEST_EQ(ints_out->at(0), 42);

  std::cerr << "test_shared_record: madara_fails: " << madara_tests_fail_count
            << "\n";
}

int main(int, char**)
{
  VAL(sizeof(KnowledgeRecord));

  test_unshared_record();
  test_shared_record();

  if(madara_tests_fail_count > 0)
  {
    std::cerr << "OVERALL: FAIL. " << madara_tests_fail_count
              << " tests failed.\n";
  }
  else
  {
    std::cerr << "OVERALL: SUCCESS.\n";
  }

  return madara_tests_fail_count;
}
