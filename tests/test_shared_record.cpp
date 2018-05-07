#include <string>
#include <atomic>

#include "madara/knowledge/ThreadSafeContext.h"
#include "madara/knowledge/KnowledgeBase.h"
#include "madara/utility/Utility.h"

#include "test.h"

using namespace madara::knowledge;
namespace logger = madara::logger;

typedef  KnowledgeRecord::Integer  Integer;

void test_unshared_record (void)
{
  KnowledgeRecord rec;

  std::string str = "Hello World";
  const char *orig_ptr = str.c_str();
  rec.set_value (std::move(str));

  std::string str_out = rec.to_string();

  TEST_NE(orig_ptr, str_out.c_str());
}

void test_shared_record (void)
{
  KnowledgeBase kb;
  KnowledgeRecord rec;

  std::string str = "Hello World";
  auto s1 = std::make_shared<std::string>(std::move(str));
  const char *orig_ptr = s1->c_str();
  rec.emplace_shared_string (std::move(s1));

  std::shared_ptr<std::string> str_out = rec.share_string();

  TEST_EQ(orig_ptr, str_out->c_str());

  std::string big_str = "This is a string that might be much longer and be expensive to copy.";
  const char * orig_sptr = big_str.c_str();

  // Creates a vector with 4000 entries, all 42, without any copying
  KnowledgeRecord ints(tags::integers, 4000, 42);

  std::shared_ptr<std::vector<int64_t>> iptr = ints.share_integers();
  TEST_NE(iptr.get(), (void*)0);
  int64_t *orig_iptr = &(*iptr)[0];

  std::unique_ptr<std::vector<double>> unique_dptr (
    new std::vector<double> (iptr->begin(), iptr->end()));
  double *orig_dptr = &(*unique_dptr)[0];
  *orig_dptr += 0.5;
  KnowledgeRecord dbls(tags::shared(tags::doubles), std::move(unique_dptr));

  kb.set(".my_string", std::move(big_str)); // std::move avoids copying the string data
  kb.set(".my_array", ints);   // std::move not needed here to avoid copying the integer data,
  kb.set(".my_doubles", std::move(dbls)); // but would be slightly more efficient as it would
                                          // avoid touching ref counts

  TEST_EQ(kb.get(".my_array").retrieve_index(0).to_integer(), 42);

  std::shared_ptr<std::string> big_str_out = kb.take_string(".my_string"); // leaves .my_string empty
  const char *out_sptr = big_str_out->c_str();

  std::shared_ptr<std::vector<int64_t>> ints_out = kb.share_integers(".my_array"); // shared with .my_array still in kb
  int64_t *out_iptr = &(*ints_out)[0];

  std::shared_ptr<std::vector<double>> dbls_out = kb.share_doubles(".my_doubles");
  double *out_dptr = &(*dbls_out)[0];

  TEST_EQ((void*)orig_sptr, (void*)out_sptr);
  TEST_EQ((void*)orig_iptr, (void*)out_iptr);
  TEST_EQ((void*)orig_dptr, (void*)out_dptr);

  TEST_EQ(ints_out->at(0), 42);
  TEST_EQ(dbls_out->at(0), 42.5);

  kb.set_index(".my_array", 0, 47); // Causes a copy to be made, so we can modify without changing ints_out

  TEST_EQ(kb.get(".my_array").retrieve_index(0).to_integer(), 47);
  TEST_EQ(ints_out->at(0), 42);
}

int main (int, char **)
{
  TEST_EQ(sizeof(KnowledgeRecord), 48UL);
  test_unshared_record ();
  test_shared_record ();

  return 0;
}
