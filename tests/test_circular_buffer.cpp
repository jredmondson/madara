#include <string>
#include <iostream>

#include "madara/utility/SupportTest.h"
#include "madara/utility/CircularBuffer.h"
#include "madara/knowledge/KnowledgeRecord.h"
#include "madara/knowledge/KnowledgeBase.h"
#include "test.h"

namespace logger = madara::logger;

using namespace madara;
using namespace knowledge;
using namespace utility;

template<typename T>
std::ostream &operator<<(std::ostream &o, const CircularBuffer<T> &buf)
{
  for (const auto &cur : buf) {
    o << cur << " ";
  }
  return o;
}

void test_circular_int_buffer()
{
  CircularBuffer<int> b(10);

  VAL(b.capacity());

  for (int i = 0; i < 5; ++i) {
    VAL(b.size());
    b.push_back(i);
  }

  std::cout << b << std::endl;

  for (int i = 5; i < 10; ++i) {
    VAL(b.size());
    b.push_back(i);
  }

  std::cout << b << std::endl;

  b.push_back(10);

  std::cout << b << std::endl;

  b.push_back(11);

  std::cout << b << std::endl;

  b.push_back(12);

  std::cout << b << std::endl;

  b.pop_front();
  b.pop_front();

  std::cout << b << std::endl;

  b.push_back(13);

  std::cout << b << std::endl;

  b.push_back(14);

  std::cout << b << std::endl;

  b.pop_back();
  b.pop_back();

  std::cout << b << std::endl;

  b.push_back(15);

  std::cout << b << std::endl;

  for (auto iter = std::reverse_iterator<typename CircularBuffer<int>::iterator>(b.end());
      iter != std::reverse_iterator<typename CircularBuffer<int>::iterator>(b.begin()); ++iter)
  {
    std::cout << *iter << std::endl;
  }
  std::cout << std::endl;
}

template<typename T, typename R>
void test_history_vector(const std::vector<R> &hist,
    std::initializer_list<std::pair<size_t, T>> vals)
{
  for (const auto &cur : vals) {
    TEST_EQ(hist[cur.first], cur.second);
  }
}

void test_record_buffer()
{
  KnowledgeRecord rec;

  rec.set_history_capacity(10);
  for (int i = 1; i < 5; ++i) {
    rec.set_value(i);
  }

  TEST_EQ(rec.get_newest(), 4);
  TEST_EQ(rec.get_oldest().exists(), false);

  for (int i = 7; i < 27; ++i) {
    rec.set_value(i);
  }

  TEST_EQ(rec.get_newest(), 26);
  TEST_EQ(rec.get_oldest(), 17);
  TEST_EQ(rec.get_history(-2), 25);
  TEST_EQ(rec.get_history(2), 19);

  test_history_vector<int>(rec.get_history(),
      {{7, 24}, {2, 19}, {4, 21}, {5, 22}});
  test_history_vector<int>(rec.get_newest(4),
      {{0, 23}, {3, 26}});
  test_history_vector<std::string>(rec.get_newest<std::string>(4),
      {{0, std::string("23")}, {3, std::string("26")}});
  test_history_vector<const char *>(rec.get_oldest(6),
      {{0, "17"}, {3, "20"}, {5, "22"}});
  test_history_vector<double>(rec.get_oldest<double>(6),
      {{0, 17}, {3, 20}, {5, 22}});
  test_history_vector<int>(rec.get_history(3, 4),
      {{0, 20}, {2, 22}});
  test_history_vector<int>(rec.get_history(-5, 4),
      {{0, 22}, {2, 24}});
}

template<typename Key>
void test_kb(KnowledgeBase &kb, Key key)
{
  kb.set_history_capacity(key, 10);
  for (int i = 1; i < 5; ++i) {
    kb.set(key, i);
  }

  TEST_EQ(kb.get_newest(key), 4);
  TEST_EQ(kb.get_oldest(key).exists(), false);

  for (int i = 7; i < 27; ++i) {
    kb.set(key, i);
  }

  TEST_EQ(kb.get_newest(key), 26);
  TEST_EQ(kb.get_oldest(key), 17);
  TEST_EQ(kb.get_history(key, -2), 25);
  TEST_EQ(kb.get_history(key, 2), 19);

  test_history_vector<int>(kb.get_history(key),
      {{7, 24}, {2, 19}, {4, 21}, {5, 22}});
  test_history_vector<int>(kb.get_newest(key, 4),
      {{0, 23}, {3, 26}});
  test_history_vector<std::string>(kb.get_newest<std::string>(key, 4),
      {{0, std::string("23")}, {3, std::string("26")}});
  test_history_vector<const char *>(kb.get_oldest(key, 6),
      {{0, "17"}, {3, "20"}, {5, "22"}});
  test_history_vector<double>(kb.get_oldest<double>(key, 6),
      {{0, 17}, {3, 20}, {5, 22}});
  test_history_vector<int>(kb.get_history(key, 3, 4),
      {{0, 20}, {2, 22}});
  test_history_vector<int>(kb.get_history(key, -5, 4),
      {{0, 22}, {2, 24}});
}

int main (int, char **)
{
  madara::logger::global_logger->set_level(9);

  std::cerr << "Test CircularBuffer directly" << std::endl;
  test_circular_int_buffer();

  std::cerr << "Test KnowledgeRecord with history" << std::endl;
  test_record_buffer();

  std::cerr << "Test KnowledgeBase record with string index" << std::endl;
  {
    std::string key = "test";
    KnowledgeBase kb;

    test_kb(kb, key);
  }

  std::cerr << "Test KnowledgeBase record with VariableReference" << std::endl;
  {
    std::string key = "test";
    KnowledgeBase kb;
    auto ref = kb.get_ref(key);

    test_kb(kb, ref);
  }

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
