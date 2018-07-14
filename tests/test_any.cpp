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

  struct C
  {
    int g;
    double h;
    std::string i;
    std::vector<int> iv;
    std::vector<B> v;
    B b;
    std::map<std::string, int> m;
  };

  template<typename Fun, typename T>
  auto for_each_field(Fun fun, T&& val) -> enable_if_same_decayed<T, C>
  {
    fun("g", val.g);
    fun("h", val.h);
    fun("i", val.i);
    fun("iv", val.iv);
    fun("v", val.v);
    fun("b", val.b);
    fun("m", val.m);
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
  static_assert(supports_for_each_field<ns::C>::value, "C must support for_each_field");

  static_assert(supports_str_index<decltype(ns::C::m)>::value, "C::m must support str_index");
  static_assert(supports_cast_to_record<decltype(ns::C::i)>::value, "C::i must support to_record");

  std::string test;
  auto testkr = knowledge_cast(test);

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

  a0.set(ns::B{2, 5, 9});
  a0.serialize(buf);
  a0.unserialize<ns::B>(buf.data(), buf.size());
  TEST_EQ(a0.ref<ns::B>().f, 9);

  VAL(a0.to_json());

  a0("d") = a0("e") = a0("f") = 42.0;
  TEST_EQ(a0("d")->ref<double>(), 42);
  TEST_EQ(a0("e")->ref<double>(), 42);
  TEST_EQ(a0("f")->ref<double>(), 42);

  Any aC(ns::C{1, 2.5, "asdf",
      {10, 20, 30}, {}, {4, 7, 9},
      {{"x", 13}, {"y", 14}}});
  auto fields = aC.list_fields();
  for (const auto &cur : fields)
  {
    std::cerr << "Field " << cur.name() << " " <<
      (void*)&cur.handler() << " " <<
      cur.type_name() << " " <<
      cur.data() << " " <<
      aC.ref(cur).to_json() << std::endl;
  }

  TEST_EQ(aC.ref<std::string>("i"), "asdf");
  TEST_EQ(aC("i")(tags::str), "asdf");
  TEST_EQ(aC("iv")->size(), 3UL);
  TEST_EQ(aC("iv")[1](type<int>{}), 20);
  VAL(aC("iv")->to_string());
  TEST_EQ(aC("m")->size(), 2UL);
  TEST_EQ(aC("m")["x"](type<int>{}), 13);
  aC("m")["x"](type<int>{}) = 23;
  TEST_EQ(aC("m")["x"](type<int>{}), 23);

  VAL(aC.ref(aC.find_field("g")).to_json());
  auto field = aC.find_field("i");
  aC.set_field(field, Any(std::string("zxcv")));
  VAL(aC.ref(field).to_json());
  aC(field) = std::string("qwerty");
  VAL(aC(field)(tags::record));
  aC.set_field("h", 4.5);
  VAL(aC.ref("h")(tags::json));
  TEST_EQ(aC("b")("f")(tags::dbl), 9);
  aC("b")("f")(tags::dbl) = 10;

  const Any aConst = aC;
  TEST_EQ(aConst("b")("f")(tags::dbl), 10);

  a0.set(std::string("asdf"));
  a1.set<std::string>("zxcv");

  TEST_EQ(a0.ref<std::string>(), "asdf");
  TEST_EQ(a1.ref<std::string>(), "zxcv");

  a0.serialize(buf);
  VAL(buf.size());
  Any a2(tags::raw_data, buf.data(), buf.size());
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
