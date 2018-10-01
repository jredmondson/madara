#include <string>
#include <iostream>

#include "madara/knowledge/Any.h"
#include "madara/knowledge/KnowledgeRecord.h"
#include "madara/knowledge/KnowledgeBase.h"
#include "madara/utility/SupportTest.h"

namespace logger = madara::logger;

using namespace madara;
using namespace knowledge;

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
  Quaternion(double w, double i, double j, double k) : w(w), i(i), j(j), k(k) {}
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
    : Point(x, y, z), Quaternion(w, i, j, k)
  {
  }
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
  StampedPose(int64_t time, std::string frame, double x, double y, double z = 0)
    : Pose(x, y, z), stamp{time, frame}
  {
  }
  StampedPose(int64_t time, std::string frame, double x, double y, double z,
      double w, double i, double j, double k)
    : Pose(x, y, z, w, i, j, k), stamp{time, frame}
  {
  }
};

template<typename Fun>
void for_each_field(Fun fun, StampedPose& val)
{
  fun("stamp", val.stamp);
  for_each_field(fun, static_cast<Pose&>(val));
};

void register_types()
{
  Any::register_type<Point>("Point");
  Any::register_type<Quaternion>("Quaternion");
  Any::register_type<Pose>("Pose");
  Any::register_type<Stamp>("Stamp");
  Any::register_type<StampedPose>("StampedPose");
}
}

extern "C" void madara_register_test_types(void) __attribute__((constructor));

extern "C" void madara_register_test_types(void)
{
  geo::register_types();
}
