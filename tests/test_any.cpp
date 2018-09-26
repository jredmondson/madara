#define BOOST_PP_VARIADICS 1

#include <string>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "madara/knowledge/Any.h"
#include "madara/knowledge/KnowledgeRecord.h"
#include "madara/knowledge/KnowledgeBase.h"
#include "madara/utility/SupportTest.h"
#include "madara/knowledge/CapnAdapt.h"

#include "capnfiles/Geo.capnp.h"

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

MADARA_USE_CEREAL(A);

namespace madara { namespace utility { inline namespace core {

template<typename Fun, typename T>
void for_each_field(Fun fun, std::vector<T> &val)
{
  (void)fun; (void)val;
}

}}}

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

  enum E
  {
    X,
    Y,
    Z
  };

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
    E e;
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
    fun("e", val.e);
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
      {{"x", 13}, {"y", 14}}, ns::Z});
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
  TEST_EQ(aC("e")(type<ns::E>{}), ns::Z);

  Any aE(type<ns::E>{}, ns::Z);
  TEST_EQ(aE(type<ns::E>{}), ns::Z);

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

  Any a4(std::vector<ns::B>{{1, 2, 3}, {4, 5, 6}, {7, 8, 9}});
  a4.serialize(buf);
  VAL(a4);
}

void test_record()
{
  KnowledgeRecord k0(tags::any<std::vector<std::string>>{}, {"a", "b", "c"});
  TEST_EQ(k0.get_any_cref<std::vector<std::string>>()[1], "b");

  Any a0(type<std::string>{}, "hello");
  KnowledgeRecord k1(std::move(a0));
  TEST_EQ(k1.get_any_cref<std::string>(), "hello");
  TEST_EQ(a0.empty(), true);

  k1.set_any(std::array<double, 3>{{5, 10, 15}});
  TEST_EQ((k1.get_any_cref<std::array<double, 3>>()[1]), 10);

  KnowledgeRecord empty;
  try {
    VAL(empty.to_any<int>());
    TEST_EQ("Expected exception thrown", (void*)0);
  } catch (const exceptions::BadAnyAccess &ex) {
    VAL(std::string("Exception: ") + ex.what());
  }
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
  kb.save_context("/tmp/madara_test_any.kb");

  auto a1 = kb.share_any("asdf");
  TEST_EQ(a1->template ref<std::vector<std::string>>()[0], "a");

  VAL(a1->to_json());

  TEST_EQ(kb.exists("asdf"), true);

  kb.clear();

  std::string id;
  kb.load_context("/tmp/madara_test_any.kb", id);
  kb.to_string(kb_dump);
  VAL(kb_dump);
  TEST_EQ(kb.get("hello").template get_any_ref<std::string>(), "world");
  TEST_EQ(kb.get("asdf").template get_any_ref<std::vector<std::string>>()[1], "b");
  kb.save_as_json("/tmp/madara_test_any.json");
}

namespace geo
{
  struct Point
  {
    double x = 0, y = 0, z = 0;

    Point() = default;
    Point(double x, double y, double z = 0) : x(x), y(y), z(z) {}

    double &getX() { return x; }
    double &getY() { return y; }
    double &getZ() { return z; }
  };

  MADARA_CAPN_MEMBERS(Point, geo_capn::Point,
      (x, X, [](Point &p) -> double & { return p.x; })
      (y, Y, &Point::getY)
      (z, Z)
    )

  static_assert(::madara::knowledge::supports_for_each_member<Point>::value, "");
  static_assert(!madara_use_cereal(type<Point>{}), "");
  static_assert(!madara_use_cereal(type<std::vector<Point>>{}), "");

  struct Quaternion
  {
    double w = 0, i = 0, j = 0, k = 0;

    Quaternion() = default;
    Quaternion(double w, double i, double j, double k)
      : w(w), i(i), j(j), k(k) {}
  };

  MADARA_CAPN_MEMBERS(Quaternion, geo_capn::Quaternion,
      (w, W)
      (i, I)
      (j, J)
      (k, K)
    )

  struct Pose : Point, Quaternion
  {
    Pose() = default;
    Pose(double x, double y, double z = 0) : Point(x, y, z) {}
    Pose(double x, double y, double z, double w, double i, double j, double k)
      : Point(x, y, z), Quaternion(w, i, j, k) {}
  };

  MADARA_CAPN_MEMBERS(Pose, geo_capn::Pose,
      (x, X)
      (y, Y)
      (z, Z)
      (w, W)
      (i, I)
      (j, J)
      (k, K)
    )

  struct Stamp
  {
    int64_t time;
    std::string frame;
  };

  MADARA_CAPN_MEMBERS(Stamp, geo_capn::Stamp,
      (time, Time)
      (frame, Frame)
    )

  static_assert(supports_for_each_member<Stamp>::value, "");
  static_assert(supports_madara_capn_struct_Stamp_hasFrame<
      typename geo_capn::Stamp::Builder>::value, "");

  static_assert(!supports_madara_capn_struct_Stamp_hasTime<
      typename geo_capn::Stamp::Builder>::value, "");

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

  MADARA_CAPN_MEMBERS(StampedPose, geo_capn::StampedPose,
      (stamp, Stamp)
      (x, X)
      (y, Y)
      (z, Z)
      (w, W)
      (i, I)
      (j, J)
      (k, K)
    )

  enum class TestEnum
  {
    a, b, c
  };

  struct StampedPoseList
  {
    std::vector<StampedPose> list;
    std::vector<int> data;
    std::array<int, 3> arr3;
    int i;
    std::vector<std::string> strs;
    int en;
    TestEnum i16;
  };

  //static_assert(supports_infer_capn<Point>::value, "");
  static_assert(is_same<decltype(infer_capn_type(type<std::string>{})), capnp::Text>(), "");
  //static_assert(supports_infer_capn<std::string>::value, "");
  static_assert(is_same<decltype(infer_capn_type(type<std::vector<std::string>>{})), capnp::List<capnp::Text>>(), "");
  static_assert(sizeof(infer_capn_type(type<std::vector<std::string>>{})) == sizeof(capnp::List<capnp::Text>), "");
  //static_assert(supports_infer_capn<std::vector<std::string>>::value, "");
  //static_assert(supports_infer_capn<StampedPoseList>::value, "");

  MADARA_CAPN_MEMBERS(StampedPoseList, geo_capn::StampedPoseList,
      (list, List)
      (data, Data)
      (arr3, Arr3)
      (i, I)
      (strs, Strs)
      (en, En)
      (i16, I16)
    )

  std::unique_ptr<StampedPoseList> capn_preprocess(const StampedPoseList &list)
  {
    auto ret = list;
    ret.i += 10;
    return into_unique(std::move(ret));
  }

  void capn_postprocess(StampedPoseList &list)
  {
    list.i -= 5;
  }

  static_assert(is_same<
      decltype(get_capnproto_type_info_StampedPoseList_Strs(
          type<geo_capn::StampedPoseList>{}, select_overload())),
        CapnStrList<geo_capn::StampedPoseList>>(), "");

  static_assert(supports_for_each_member<StampedPoseList>::value, "");
  static_assert(supports_capn_get<StampedPoseList>::value, "");
  static_assert(supports_capn_set<StampedPoseList>::value, "");
  static_assert(supports_capn_set<StampedPoseList>::value, "");

  void register_types()
  {
    Any::register_type<Point>("Point");
    Any::register_type<Quaternion>("Quaternion");
    Any::register_type<Pose>("Pose");
    Any::register_type<Stamp>("Stamp");
    Any::register_type<StampedPose>("StampedPose");

    Any::register_schema("Point",
        capnp::Schema::from<geo_capn::Point>());
    Any::register_schema("PointSchema",
        capnp::Schema::from<geo_capn::Point>());
    Any::register_schema("StampedPoseList",
        capnp::Schema::from<geo_capn::StampedPoseList>());
  }
}

void test_capn()
{
  capnp::MallocMessageBuilder buffer;
  auto builder = buffer.initRoot<geo_capn::Point>();
  builder.setX(3);
  builder.setY(6);
  builder.setZ(9);

  kj::VectorOutputStream vec;
  capnp::writeMessage(vec, buffer);

  auto data = vec.getArray();
  GenericCapnObject obj("Point", data.asChars().begin(), data.size());

  auto reader = obj.reader(type<geo_capn::Point>());
  TEST_EQ(reader.getX(), 3);
  TEST_EQ(reader.getY(), 6);
  TEST_EQ(reader.getZ(), 9);

  auto dynreader = obj.reader(capnp::Schema::from<geo_capn::Point>());
  TEST_EQ(dynreader.get("x").template as<double>(), 3);
  TEST_EQ(dynreader.get("y").template as<double>(), 6);
  TEST_EQ(dynreader.get("z").template as<double>(), 9);
  TEST_EQ(dynreader.get("x").template as<int>(), 3);
  TEST_EQ(dynreader.get("y").template as<int>(), 6);
  TEST_EQ(dynreader.get("z").template as<int>(), 9);

  CapnObject<geo_capn::Point> obj2(buffer);

  auto reader2 = obj2.reader();
  TEST_EQ(reader2.getX(), 3);
  TEST_EQ(reader2.getY(), 6);
  TEST_EQ(reader2.getZ(), 9);

  using CapnPoint = CapnObject<geo_capn::Point>;
  CapnPoint def;
  Any a(std::move(obj2));

  TEST_EQ(a.reader<geo_capn::Point>().getX(), 3);
  TEST_EQ(a.reader<geo_capn::Point>().getY(), 6);
  TEST_EQ(a.reader<geo_capn::Point>().getZ(), 9);

  VAL(a);

  int fd = open(utility::expand_envs(
          "$(MADARA_ROOT)/tests/capnfiles/Geo.capnp.bin").c_str(),
      0, O_RDONLY);
  TEST_GT(fd, 0);
  capnp::StreamFdMessageReader schema_message_reader(fd);
  auto schema_reader = schema_message_reader.getRoot<capnp::schema::CodeGeneratorRequest>();
  LOG("Got schema node reader");
  capnp::SchemaLoader loader;
  std::map<std::string, capnp::Schema> schemas;
  for (auto schema : schema_reader.getNodes()) {
    log("INFO  Loading schema %s\n", schema.getDisplayName());
    schemas[schema.getDisplayName()] = loader.load(schema);
  }
  log("INFO  Loaded %i schemas\n", schemas.size());
  auto schema = schemas.at("tests/capnfiles/Geo.capnp:Point").asStruct();
  auto dynbuilder = buffer.initRoot<capnp::DynamicStruct>(schema);
  dynbuilder.set("x", 4);
  dynbuilder.set("y", 8);
  dynbuilder.set("z", 12);

  GenericCapnObject obj3("Point", buffer);
  dynreader = obj3.reader(schema);
  TEST_EQ(dynreader.get("x").template as<double>(), 4);
  TEST_EQ(dynreader.get("y").template as<double>(), 8);
  TEST_EQ(dynreader.get("z").template as<double>(), 12);

  a.set(std::move(obj3));

  VAL(a);

  auto static_reader = a.reader<geo_capn::Point>();

  TEST_EQ(static_reader.getX(), 4);
  TEST_EQ(static_reader.getY(), 8);
  TEST_EQ(static_reader.getZ(), 12);

  RegCapnObject obj4("Point", buffer);
  dynreader = obj4.reader();
  TEST_EQ(dynreader.get("x").template as<double>(), 4);
  TEST_EQ(dynreader.get("y").template as<double>(), 8);
  TEST_EQ(dynreader.get("z").template as<double>(), 12);

  a.set(std::move(obj4));

  VAL(a);

  Any aj;
  aj.unserialize_json("PointSchema", "{\"x\":1.25,\"y\":2.5,\"z\":4.75}");

  auto ajr = aj.reader();
  TEST_EQ(ajr.get("x").as<double>(), 1.25);
  TEST_EQ(ajr.get("y").as<double>(), 2.5);
  TEST_EQ(ajr.get("z").as<double>(), 4.75);
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

  Any s0 = *kb.get("s0").share_any();
  s0("stamp")(&Stamp::time) = 4321;
  kb.emplace_any("s0", std::move(s0));
  TEST_EQ(kb.get("s0").share_any()->ref("stamp")("time").to_integer(), 4321);
  TEST_EQ(kb.get("s0").share_any()->cref("stamp")("frame")(
    (size_t (std::string::*)(char, size_t)const)&std::string::find, 'f', 0), 2UL);

  StampedPoseList vs0 = {{
    {0, "frame0", 1, 2},
    {1, "frame1", 2, 3},
    {2, "frame2", 3, 4},
  }, {2, 4, 6}, {{3, 6, 9}}, 42, {"foo", "bar"}, 2, TestEnum::b};
  kb.set_any("vs0", std::move(vs0));

  VAL(kb.get("vs0").to_any());
  TEST_EQ(kb.get("vs0").share_any()->ref("list").size(), 3UL);
  TEST_EQ(kb.get("vs0").get_any_cref()("list")[1]("stamp")("frame").to_string(), "frame1");
  TEST_EQ(kb.get("vs0").get_any_cref()("arr3")[1].to_integer(), 6);
  TEST_EQ(kb.get("vs0").get_any_cref()("i").to_integer(), 42);
  TEST_EQ(kb.get("vs0").get_any_cref()("strs")[1].to_string(), "bar");
  TEST_EQ(kb.get("vs0").get_any_cref()("en").to_integer(), 2);
  TEST_EQ(kb.get("vs0").get_any_cref()("i16").to_integer(), (int)TestEnum::b);

  std::vector<char> buf;

  kb.get("vs0").share_any()->tagged_serialize("StampedPoseList", buf);

  Any avs0;
  avs0.tagged_unserialize(buf.data(), buf.size());

  auto &avs0ref = avs0.ref<RegCapnObject>();
  auto avs0_reader = avs0ref.reader();
  auto avs0i = avs0_reader.get("i");
  TEST_EQ(avs0i.template as<int>(), 52);

  auto avs0_sreader = avs0.template reader<geo_capn::StampedPoseList>();
  TEST_EQ(avs0_sreader.getI(), 52);

  auto avs0data = avs0_reader.get("data");
  auto avs0data_reader = avs0data.template as<capnp::List<int>>();
  VAL(avs0data_reader[0]);

  VAL(avs0);

  Any::register_type<StampedPoseList>("StampedPoseList");
  Any l2;
  l2.tagged_unserialize(buf.data(), buf.size());
  TEST_EQ(l2("i").to_integer(), 47);

  Any new_pose = Any::construct("StampedPose");
  VAL(new_pose);

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

  Any al0 = kb.get("l0").to_any();
  al0.tagged_serialize(buf);
  Any al1;
  al1.tagged_unserialize(buf.data(), buf.size());
  VAL(al1);

  al0.serialize(buf);
  al1.unserialize<CapnObject<geo_capn::Point>>(buf.data(), buf.size());
  auto reader = al1.reader();
  TEST_EQ(reader.get("x").template as<double>(), 1);
  TEST_EQ(reader.get("y").template as<double>(), 2);
  TEST_EQ(reader.get("z").template as<double>(), 3);
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

struct Derived : Example
{
  double x;
};
template<typename Fun>
void for_each_field(Fun &&fun, Derived &val)
{
  for_each_field(fun, (Example &)val);
  fun("x", val.x);
}

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

  // Iterate over each elements, and print. Best to get size and AnyRef first.
  AnyRef arr = any("ev")[0]("dv");
  size_t n = arr.size();
  for (size_t i = 0; i < n; ++i) {
    std::cout << i << ": " << arr[i] << std::endl;
  }

  any("ev")[1]("i").ref<int>() = 42; // assign through direct reference
  any("ev")[1]("i") = 47; // will assign directly, as the literal is an int and matches the stored value
  any("ev")[1]("i") = 52.1; // will convert to a KnowledgeRecord, then call `from_record`, since the types don't match
  any("ev")[1]("i") = "57"; // will also convert to a KnowledgeRecord
}

int main (int, char **)
{
  madara::logger::global_logger->set_level(9);

  Any::register_type<A>("A");
  Any::register_type<ns::B>("B");
  Any::register_type<ns::C>("C");
  Any::register_type<ns::E>("E");
  Any::register_type<std::string>("str");
  Any::register_type<std::vector<std::string>>("vecstr");
  geo::register_types();
  Any::register_type<Example>("Example");
  Any::register_type<Derived>("Derived");

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

  test_capn();

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
