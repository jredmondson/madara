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

namespace ns
{
  struct B
  {
    double d;
    double e;
    double f;
  };

  template<typename Fun, typename T>
  auto for_each_field(Fun fun, T&& val) -> enable_if_same_decayed<T, B>
  {
    fun("d", val.d);
    fun("e", val.e);
    fun("f", val.f);
  }
}

struct Tracker
{
  int copied = 0;
  int moved = 0;

  Tracker() = default;
  Tracker(const Tracker &t)
    : copied(t.copied + 1), moved(t.moved) {}
  Tracker(Tracker &&t)
    : copied(t.copied), moved(t.moved + 1) {}

  Tracker &operator=(const Tracker &) = delete;
  Tracker &operator=(Tracker &&) = delete;

  template<typename Archive>
  void serialize(Archive &ar, unsigned int)
  {
    ar & copied & moved;
  }
};

void test_any()
{
  static_assert(supports_for_each_field<ns::B>::value, "B must support for_each_field");
  Any a0(123);
  Any a1(type<int>{}, 456);

  TEST_EQ(a0.ref<int>(), 123);
  TEST_EQ(a1.ref<int>(), 456);

  std::vector<char> buf;

  a0.serialize(buf);
  a0.set(0);
  TEST_EQ(a0.ref<int>(), 0);

  a0.unserialize<int>(buf.data(), buf.size());
  TEST_EQ(a0.ref<int>(), 123);

  a1.unserialize<short>(buf.data(), buf.size());
  TEST_EQ(a1.ref<short>(), 123);

  a0.set(A{2, 5, 9});
  a0.serialize(buf);
  a0.unserialize<A>(buf.data(), buf.size());
  TEST_EQ(a0.ref<A>().b, 5);

  VAL(a0.to_json());

  a0.set(std::string("asdf"));
  a1.set<std::string>("zxcv");

  TEST_EQ(a0.ref<std::string>(), "asdf");
  TEST_EQ(a1.ref<std::string>(), "zxcv");

  a0.serialize(buf);
  VAL(buf.size());
  Any a2(raw_data, buf.data(), buf.size());
  TEST_EQ(a2.ref<std::string>(), "asdf");

  Any a3(std::move(a1));
  TEST_EQ(a1.empty(), true);
  TEST_EQ(a3.ref(type<std::string>{}), "zxcv");

  Any acopy(a3);
  TEST_EQ(a3.ref(type<std::string>{}), "zxcv");
  TEST_EQ(a3.ref(type<std::string>{}), "zxcv");

  Any at(type<Tracker>{});
  Tracker t(at.take<Tracker>());
  TEST_EQ(t.copied, 0);
  TEST_LE(t.moved, 2);
}

void test_record()
{
  KnowledgeRecord k0(tags::any<std::vector<std::string>>{}, {"a", "b", "c"});
  TEST_EQ(k0.get_any_cref<std::vector<std::string>>()[1], "b");
  k0.get_any_ref<std::vector<std::string>>()[1] = "d";
  TEST_EQ(k0.get_any_cref<std::vector<std::string>>()[1], "d");

  Any a0(type<std::string>{}, "hello");
  KnowledgeRecord k1(std::move(a0));
  TEST_EQ(k1.get_any_cref<std::string>(), "hello");
  TEST_EQ(a0.empty(), true);

  k1.set_any(std::array<double, 3>{{5, 10, 15}});
  TEST_EQ((k1.get_any_cref<std::array<double, 3>>()[1]), 10);
}

template<typename T>
void test_map(T &kb)
{
  kb.set ("hello_str", "world");
  kb.set_any("hello", std::string("world"));
  TEST_EQ(kb.get("hello").template get_any_cref<std::string>(), "world");

  kb.emplace_any("asdf", type<std::vector<std::string>>{}, mk_init({"a","b","c"}));
  TEST_EQ(kb.get("asdf").template get_any_cref<std::vector<std::string>>()[1], "b");

  auto a0 = kb.share_any("asdf");
  TEST_EQ(a0->template ref<std::vector<std::string>>()[2], "c");

  kb.set_any("B", ns::B{3, 3.5, 4.25});

  std::string kb_dump;
  kb.to_string(kb_dump);
  VAL(kb_dump);
  kb.save_context("/tmp/madara_test_map.kb");

  auto a1 = kb.take_any("asdf");
  TEST_EQ(a1->template ref<std::vector<std::string>>()[0], "a");

  VAL(a1->to_json());

  TEST_EQ(kb.exists("asdf"), false);

  kb.clear();

  std::string id;
  kb.load_context("/tmp/madara_test_map.kb", id);
  TEST_EQ(kb.get("hello").template get_any_ref<std::string>(), "world");
  TEST_EQ(kb.get("asdf").template get_any_ref<std::vector<std::string>>()[1], "b");
}

int main (int, char **)
{
  madara::logger::global_logger->set_level(9);

  test_any();
  test_record();

  {
    KnowledgeBase kb;
    test_map(kb.get_context());
  }
  {
    //KnowledgeBase kb;
    //test_map(kb);
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
