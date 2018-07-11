#include <string>
#include <iostream>

#include "madara/knowledge/Any.h"
#include "madara/knowledge/KnowledgeRecord.h"
#include "madara/knowledge/KnowledgeBase.h"

#include "test.h"

namespace logger = madara::logger;

using namespace madara;
using namespace knowledge;

struct A
{
  double a;
  double b;
  double c;

  template<typename Archive>
  void serialize(Archive &ar, unsigned int)
  {
    ar & a & b & c;
  }
};

struct B
{
  double d;
  double e;
  double f;

  template<typename Archive>
  void serialize(Archive &ar, unsigned int)
  {
    ar & d & e & f;
  }
};

void test_any()
{
  Any a0(123);
  Any a1(type<int>{}, 456);

  TEST_EQ(a0.get<int>(), 123);
  TEST_EQ(a1.get<int>(), 456);

  std::vector<char> buf;

  a0.serialize(buf);
  a0.store(0);
  TEST_EQ(a0.get<int>(), 0);

  a0.unserialize<int>(buf.data(), buf.size());
  TEST_EQ(a0.get<int>(), 123);

  a1.unserialize<short>(buf.data(), buf.size());
  TEST_EQ(a1.get<short>(), 123);

  a0.store(A{2, 5, 9});
  a0.serialize(buf);
  a0.unserialize<A>(buf.data(), buf.size());
  TEST_EQ(a0.get<A>().b, 5);

  a0.unserialize<B>(buf.data(), buf.size());
  TEST_EQ(a0.get<B>().f, 9);

  a0.store(std::string("asdf"));
  a1.store<std::string>("zxcv");

  TEST_EQ(a0.get<std::string>(), "asdf");
  TEST_EQ(a1.get<std::string>(), "zxcv");

  a0.serialize(buf);
  VAL(buf.size());
  Any a2(raw_data, buf.data(), buf.size());
  TEST_EQ(a2.get<std::string>(), "asdf");

  Any a3(std::move(a1));
  TEST_EQ(a1.empty(), true);
  TEST_EQ(a3.get(type<std::string>{}), "zxcv");

  Any acopy(a3);
  TEST_EQ(a3.get(type<std::string>{}), "zxcv");
  TEST_EQ(a3.get(type<std::string>{}), "zxcv");
}

void test_record()
{
  KnowledgeRecord k0(tags::any<std::vector<std::string>>{}, {"a", "b", "c"});
  TEST_EQ(k0.get_any<std::vector<std::string>>()[1], "b");
  k0.get_any<std::vector<std::string>>()[1] = "d";
  TEST_EQ(k0.get_any<std::vector<std::string>>()[1], "d");

  Any a0(type<std::string>{}, "hello");
  KnowledgeRecord k1(std::move(a0));
  TEST_EQ(k1.get_any<std::string>(), "hello");
  TEST_EQ(a0.empty(), true);

  k1.set_any(std::array<double, 3>{5, 10, 15});
  TEST_EQ((k1.get_any<std::array<double, 3>>()[1]), 10);
}

template<typename T>
void test_map(T &kb)
{
  kb.set_any("hello", std::string("world"));
  TEST_EQ(kb.get("hello").template get_any<std::string>(), "world");

  kb.emplace_any("asdf", type<std::vector<std::string>>{}, mk_init({"a","b","c"}));
  TEST_EQ(kb.get("asdf").template get_any<std::vector<std::string>>()[1], "b");

  auto a0 = kb.share_any("asdf");
  TEST_EQ(a0->template get<std::vector<std::string>>()[2], "c");

  auto a1 = kb.take_any("asdf");
  TEST_EQ(a1->template get<std::vector<std::string>>()[0], "a");
  TEST_EQ(kb.exists("asdf"), false);
}

int main (int, char **)
{
  test_any();
  test_record();

  {
    KnowledgeBase kb;
    test_map(kb.get_context());
  }
  {
    KnowledgeBase kb;
    test_map(kb);
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
