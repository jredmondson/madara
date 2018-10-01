#include <string>
#include <iostream>

#include "madara/utility/SupportTest.h"
#include "madara/utility/CircularBuffer.h"
#include "madara/knowledge/KnowledgeRecord.h"
#include "madara/knowledge/KnowledgeBase.h"
#include "madara/knowledge/containers/NativeCircularBufferConsumer.h"
#include "test.h"

namespace logger = madara::logger;

using namespace madara;
using namespace knowledge;
using namespace utility;
using namespace containers;

template<typename T>
std::ostream& operator<<(std::ostream& o, const CircularBuffer<T>& buf)
{
  for (const auto& cur : buf)
  {
    o << cur << " ";
  }
  return o;
}

void test_circular_int_buffer()
{
  CircularBuffer<int> b(10);

  VAL(b.capacity());

  for (int i = 0; i < 5; ++i)
  {
    VAL(b.size());
    b.push_back(i);
  }

  std::cout << b << std::endl;

  for (int i = 5; i < 10; ++i)
  {
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

  for (auto iter =
           std::reverse_iterator<typename CircularBuffer<int>::iterator>(
               b.end());
       iter !=
       std::reverse_iterator<typename CircularBuffer<int>::iterator>(b.begin());
       ++iter)
  {
    std::cout << *iter << std::endl;
  }
  std::cout << std::endl;

  {
    CircularBuffer<KnowledgeRecord> empty(10);
  }
}

template<typename T, typename R>
void test_history_vector(const std::vector<R>& hist,
    std::initializer_list<std::pair<size_t, T>> vals)
{
  for (const auto& cur : vals)
  {
    TEST_EQ(hist[cur.first], cur.second);
  }
}

template<typename T, typename R>
void test_consume_earliest(
    const std::vector<R>& ce, std::initializer_list<std::pair<size_t, T>> vals)
{
  for (const auto& cur : vals)
  {
    TEST_EQ(ce[cur.first], cur.second);
  }
}

void test_record_buffer()
{
  KnowledgeRecord rec;

  rec.set_history_capacity(10);
  for (int i = 1; i < 5; ++i)
  {
    rec.set_value(i);
  }

  TEST_EQ(rec.get_newest(), 4);
  TEST_EQ(rec.get_oldest().exists(), true);

  for (int i = 7; i < 27; ++i)
  {
    rec.set_value(i);
  }

  TEST_EQ(rec.get_newest(), 26);
  TEST_EQ(rec.get_oldest(), 17);
  TEST_EQ(rec.get_history(-2), 25);
  TEST_EQ(rec.get_history(2), 19);

  auto hist = rec.get_history();
  TEST_EQ(hist.size(), 10UL);

  test_history_vector<int>(
      rec.get_history(), {{7, 24}, {2, 19}, {4, 21}, {5, 22}});
  test_history_vector<int>(rec.get_newest(4), {{0, 23}, {3, 26}});
  test_history_vector<std::string>(rec.get_newest<std::string>(4),
      {{0, std::string("23")}, {3, std::string("26")}});
  test_history_vector<const char*>(
      rec.get_oldest(6), {{0, "17"}, {3, "20"}, {5, "22"}});
  test_history_vector<double>(
      rec.get_oldest<double>(6), {{0, 17}, {3, 20}, {5, 22}});
  test_history_vector<int>(rec.get_history(3, 4), {{0, 20}, {2, 22}});
  test_history_vector<int>(rec.get_history(-5, 4), {{0, 22}, {2, 24}});

  LOG("Resized buffer to 6");
  rec.set_history_capacity(6);
  test_history_vector<int>(rec.get_history(), {{3, 24}, {0, 21}, {1, 22}});

  LOG("Resized buffer to 10");
  rec.set_history_capacity(10);
  test_history_vector<int>(rec.get_history(), {{3, 24}, {0, 21}, {1, 22}});
}

template<typename Key>
void test_kb(KnowledgeBase& kb, Key key)
{
  kb.set_history_capacity(key, 10);
  for (int i = 1; i < 5; ++i)
  {
    kb.set(key, i);
  }

  TEST_EQ(kb.get_newest(key), 4);
  TEST_EQ(kb.get_oldest(key).exists(), true);

  for (int i = 7; i < 27; ++i)
  {
    kb.set(key, i);
  }

  TEST_EQ(kb.get_newest(key), 26);
  TEST_EQ(kb.get_oldest(key), 17);
  TEST_EQ(kb.get_history(key, -2), 25);
  TEST_EQ(kb.get_history(key, 2), 19);

  test_history_vector<int>(
      kb.get_history(key), {{7, 24}, {2, 19}, {4, 21}, {5, 22}});
  test_history_vector<int>(kb.get_newest(key, 4), {{0, 23}, {3, 26}});
  test_history_vector<std::string>(kb.get_newest<std::string>(key, 4),
      {{0, std::string("23")}, {3, std::string("26")}});
  test_history_vector<const char*>(
      kb.get_oldest(key, 6), {{0, "17"}, {3, "20"}, {5, "22"}});
  test_history_vector<double>(
      kb.get_oldest<double>(key, 6), {{0, 17}, {3, 20}, {5, 22}});
  test_history_vector<int>(kb.get_history(key, 3, 4), {{0, 20}, {2, 22}});
  test_history_vector<int>(kb.get_history(key, -5, 4), {{0, 22}, {2, 24}});
}

void test_container()
{
  KnowledgeBase kb;
  std::string key = "test";
  NativeCircularBufferConsumer buf(key, kb);
  kb.set_history_capacity(key, 10);
  kb.set(key, (int)42);
  kb.set(key, (int)53);

  TEST_EQ(buf.size(), 10UL);
  TEST_EQ(buf.remaining(), 2UL);
  TEST_EQ(buf.count(), 2UL);

  TEST_EQ(buf.consume(), 42);
  TEST_EQ(buf.consume(), 53);

  for (int i = 1; i < 6; ++i)
  {
    kb.set(key, i);
  }

  TEST_EQ(buf.remaining(), 5UL);
  TEST_EQ(buf.count(), 7UL);

  TEST_EQ(buf.consume(), 1);
  TEST_EQ(buf.consume(), 2);
  TEST_EQ(buf.consume(), 3);
  TEST_EQ(buf.consume(), 4);
  TEST_EQ(buf.consume(), 5);

  TEST_EQ(buf.remaining(), 0UL);
  TEST_EQ(buf.count(), 7UL);

  for (int i = 6; i < 31; ++i)
  {
    kb.set(key, i);
  }

  TEST_EQ(buf.remaining(), 25UL);
  TEST_EQ(buf.count(), 10UL);

  size_t dropped;
  TEST_EQ(buf.consume(dropped), 21);
  TEST_EQ(dropped, 15UL);
  TEST_EQ(buf.consume(dropped), 22);
  TEST_EQ(dropped, 0UL);

  TEST_EQ(buf.remaining(), 8UL);
  TEST_EQ(buf.count(), 10UL);

  int c = buf.get_record().get_history_oldest_index();
  for (const auto& cur : buf.get_record().get_history())
  {
    std::cerr << c << ": " << cur << std::endl;
    ++c;
  }
  VAL(buf.get_index());

  KnowledgeRecord rec;
  TEST_EQ(rec = buf.inspect(0), 23);
  TEST_EQ(rec = buf.inspect(3), 26);

  std::vector<KnowledgeRecord> v;
  v = buf.inspect(2, 3);
  test_consume_earliest<int>(v, {{0, 25}, {1, 26}, {2, 27}});

  kb.set_history_capacity(key, 5);

  v = buf.inspect(2, 3);
  test_consume_earliest<int>(v, {{0, 28}, {1, 29}, {2, 30}});

  kb.set_history_capacity(key, 10);

  v = buf.consume_many(1);
  test_consume_earliest<int>(v, {{0, 26}});

  for (int i = 32; i < 45; ++i)
  {
    kb.set(key, i);
  }

  size_t mydropped;
  v = buf.consume_many(2, mydropped);

  TEST_EQ(mydropped, 7UL);
  test_consume_earliest<int>(v, {{0, 35}, {1, 36}});

  std::vector<KnowledgeRecord> krvec;
  buf.consume_many(3, krvec);
  test_consume_earliest<int>(krvec, {{0, 37}, {1, 38}, {2, 39}});

  auto all = kb.to_map("");
  TEST_EQ(all.size(), 1UL);

  auto deep = kb.get_context().get_actual("test").deep_copy();
  TEST_EQ(deep.to_integer(), 44);

  key = "foo";
  NativeCircularBufferConsumer buf2(key, kb);
  kb.set_history_capacity(key, 10);

  TEST_EQ(buf2.consume(dropped).exists(), false);

  kb.set(key, 123);
  TEST_EQ(buf2.consume(), 123);
}

int main(int, char**)
{
  madara::logger::global_logger->set_level(2);

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

  std::cerr << "Test NativeCircularBufferConsumer" << std::endl;
  test_container();

  if (madara_tests_fail_count > 0)
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
