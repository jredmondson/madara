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

  template<typename Fun>
  void for_each_field(Fun fun, B& val)
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
    std::vector<double> dv;
    std::vector<B> v;
    B b;
    std::map<std::string, int> m;
  };

  template<typename Fun>
  void for_each_field(Fun fun, C& val)
  {
    fun("g", val.g);
    fun("h", val.h);
    fun("i", val.i);
    fun("iv", val.iv);
    fun("dv", val.dv);
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

  a0("d") = 42;
  a0("e") = "42";
  a0("f") = 42.0;
  TEST_EQ(a0("d").ref<double>(), 42);
  TEST_EQ(a0("e").ref<double>(), 42);
  TEST_EQ(a0("f").ref<double>(), 42);

  Any aC(ns::C{1, 2.5, "asdf",
      {10, 20, 30}, {1.1, 2.2, 3.3},
      {}, {4, 7, 9},
      {{"x", 13}, {"y", 14}}});
  auto fields = aC.list_fields();
  for (const auto &cur : fields)
  {
    std::cerr << "Field " << cur.name() << " " <<
      (void*)&cur.handler() << " " <<
      cur.type_name() << " " <<
      cur.data() << " " <<
      aC(cur) << std::endl;
  }

  TEST_EQ(aC.ref<std::string>("i"), "asdf");
  TEST_EQ(aC("i")(tags::str), "asdf");
  TEST_EQ(aC("iv").size(), 3UL);
  TEST_EQ(aC("iv")[1](type<int>{}), 20);
  VAL(aC("iv").to_string());
  TEST_EQ(aC("m").size(), 2UL);
  TEST_EQ(aC("m")["x"](type<int>{}), 13);
  aC("m")["x"](type<int>{}) = 23;
  TEST_EQ(aC("m")["x"](type<int>{}), 23);
  aC("m")["x"] = 26;
  TEST_EQ(aC("m")["x"](type<int>{}), 26);
  aC("m")["x"] = "29";
  TEST_EQ(aC("m")["x"](type<int>{}), 29);

  VAL(aC.ref(aC.find_field("g")));
  auto field = aC.find_field("i");
  aC(field) = std::string("zxcv");
  VAL(aC.ref(field));
  aC(field) = std::string("qwerty");
  VAL(aC(field).to_record());
  aC("h") = 4.5;
  VAL(aC.ref("h"));
  TEST_EQ(aC("b")("f")(tags::dbl), 9);
  TEST_EQ(aC("b")("f").to_string(), "9");
  aC("b")("f")(tags::dbl) = 10;
  TEST_EQ(aC("dv")(tags::dbls)[1], 2.2);
  aC("dv")(tags::dbls)[1] = 2.4;
  TEST_EQ(aC("dv")(tags::dbls)[1], 2.4);

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

namespace geo
{
  struct Point
  {
    double x = 0, y = 0, z = 0;

    Point() = default;
    Point(double x, double y, double z = 0) : x(x), y(y), z(z) {}
  };

  template<typename Fun>
  void for_each_field(Fun fun, Point& val)
  {
    fun("x", val.x);
    fun("y", val.y);
    fun("z", val.z);
  };

  struct Quaternion
  {
    double w = 0, i = 0, j = 0, k = 0;

    Quaternion() = default;
    Quaternion(double w, double i, double j, double k)
      : w(w), i(i), j(j), k(k) {}
  };

  template<typename Fun>
  void for_each_field(Fun fun, Quaternion& val)
  {
    fun("w", val.w);
    fun("i", val.i);
    fun("j", val.j);
    fun("k", val.k);
  };

  struct Pose : Point, Quaternion
  {
    Pose() = default;
    Pose(double x, double y, double z = 0) : Point(x, y, z) {}
    Pose(double x, double y, double z, double w, double i, double j, double k)
      : Point(x, y, z), Quaternion(w, i, j, k) {}
  };

  template<typename Fun>
  void for_each_field(Fun fun, Pose& val)
  {
    for_each_field(fun, static_cast<Point&>(val));
    for_each_field(fun, static_cast<Quaternion&>(val));
  };

  struct Stamp
  {
    int64_t time;
    std::string frame;
  };

  template<typename Fun>
  void for_each_field(Fun fun, Stamp& val)
  {
    fun("time", val.time);
    fun("frame", val.frame);
  }

  struct StampedPose : Pose
  {
    Stamp stamp = {0, ""};

    StampedPose() = default;

    using Pose::Pose;
    StampedPose(int64_t time, std::string frame,
        double x, double y, double z = 0)
      : Pose(x, y, z), stamp{time, frame} {}
    StampedPose(int64_t time, std::string frame,
        double x, double y, double z,
        double w, double i, double j, double k)
      : Pose(x, y, z, w, i, j, k), stamp{time, frame} {}
  };

  template<typename Fun>
  void for_each_field(Fun fun, StampedPose& val)
  {
    fun("stamp", val.stamp);
    for_each_field(fun, static_cast<Pose&>(val));
  };

  void register_types()
  {
    Any::register_type<geo::Point>("Point");
    MADARA_ANY_REGISTER_TYPE(Quaternion);
    MADARA_ANY_REGISTER_TYPE(Pose);
    MADARA_ANY_REGISTER_TYPE(StampedPose);
  }
}

void test_geo()
{
  KnowledgeBase kb;
  using namespace geo;

  kb.set_any("l0", Point(1, 2, 3));
  kb.set_any("q0", Quaternion(1, 2, 3, 4));
  kb.set_any("p0", Pose(1, 2, 3, 4, 5, 6, 7));
  kb.set_any("s0", StampedPose(1234, "default", 1, 2, 3, 4, 5, 6, 7));

  VAL(kb.get("l0").to_any());
  VAL(kb.get("q0").to_any());
  VAL(kb.get("p0").to_any());
  VAL(kb.get("s0").to_any());

  TEST_EQ(kb.get("s0").share_any()->ref("stamp")("time").to_integer(), 1234);
  TEST_EQ(kb.get("s0").share_any()->ref("stamp")("time").to_string(), "1234");
  TEST_EQ(kb.get("s0").share_any()->ref("stamp")("frame").to_string(), "default");

  Any s0 = std::move(*kb.get("s0").take_any());
  s0("stamp")(&Stamp::time) = 4321;
  kb.emplace_any("s0", std::move(s0));
  TEST_EQ(kb.get("s0").share_any()->ref("stamp")("time").to_integer(), 4321);
  kb.get("s0").share_any()->ref("stamp")("frame")(&std::string::push_back, '!');
  TEST_EQ(kb.get("s0").share_any()->ref("stamp")("frame").to_string(), "default!");
  TEST_EQ(kb.get("s0").share_any()->cref("stamp")("frame")(
    (size_t (std::string::*)(char, size_t)const)&std::string::find, 'f', 0), 2UL);

  std::vector<StampedPose> vs0 = {
    {0, "frame0", 1, 2},
    {1, "frame1", 2, 3},
    {2, "frame2", 3, 4},
  };
  kb.set_any("vs0", std::move(vs0));

  VAL(kb.get("vs0").to_any());
  TEST_EQ(kb.get("vs0").share_any()->size(), 3UL);
  TEST_EQ(kb.get("vs0").get_any_cref()[1]("stamp")("frame").to_string(), "frame1");

  std::vector<char> buf;

  Any p0 = kb.get("p0").to_any();
  p0.serialize(buf);
  Any p1;

  p1.unserialize<Pose>(buf.data(), buf.size());
  VAL(p1);

  p1.unserialize("Pose", buf.data(), buf.size());
  VAL(p1);
  VAL(p1.tag());
  VAL((void*)s0.tag());

  p1.tagged_serialize(buf);
  Any p2;
  p2.tagged_unserialize(buf.data(), buf.size());
  VAL(p2);
  VAL(p2.tag());
}

struct Example {
    int i;
    double d;
    std::vector<double> dv;
    std::vector<Example> ev;
};

template<typename Fun, typename T>
auto for_each_field(Fun fun, T&& val) ->
  enable_if_same_decayed<T, Example>
{
  fun("i", val.i);
  fun("d", val.d);
  fun("dv", val.dv);
  fun("ev", val.ev);
};

void test_example()
{
  KnowledgeBase kb;
  kb.set_any("example", Example{3, 5.5,
    {1.1, 2.2, 3.3}, {
      {4, 6.6, {4.4, 5.5, 6.6}, {}},
      {5, 7.7, {7.7, 8.8, 9.9}, {}}}});

  Any any = kb.get("example").to_any(); // When given no type, to_any() returns a copy of stored Any

  TEST_EQ(any("i").to_integer(), 3L);
  TEST_EQ(any("dv")[1].to_double(), 2.2);
  TEST_EQ(any("ev")[0]("dv")[2].to_integer(), 6);

  // Iterate over each field, and print. Note that printing Anys always works, as it falls back
  // to serializing to JSON if no other printing support is available.
  for (const AnyField &field : any.list_fields()) {
    std::cerr << field.name() << ": " << any(field) << std::endl;
  }

  any("ev")[1]("i").ref<int>() = 42; // assign through direct reference
  any("ev")[1]("i") = 47; // will assign directly, as the literal is an int and matches the stored value
  any("ev")[1]("i") = 52.1; // will convert to a KnowledgeRecord, then call `from_record`, since the types don't match
  any("ev")[1]("i") = "57"; // will also convert to a KnowledgeRecord
}

int main (int, char **)
{
  madara::logger::global_logger->set_level(3);

  Any::register_type<geo::Pose>("Pose");

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

  test_geo();

  test_example();

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
