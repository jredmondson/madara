using Cxx = import "c++.capnp.inc";
using Java = import "java.capnp.inc";

@0xe3c04ff389251d41;

$Cxx.namespace("geo_capn");
$Java.package("ai.madara.tests.capnp");
$Java.outerClassname("Geo");

struct Point {
  x @0 :Float64;
  y @1 :Float64;
  z @2 :Float64;
}

struct Quaternion {
  w @0 :Float64;
  i @1 :Float64;
  j @2 :Float64;
  k @3 :Float64;
}

struct Pose {
  x @0 :Float64;
  y @1 :Float64;
  z @2 :Float64;
  w @3 :Float64;
  i @4 :Float64;
  j @5 :Float64;
  k @6 :Float64;
}

struct Stamp {
  time @0 :Int64;
  frame @1 :Text;
}

struct StampedPose {
  stamp @0 :Stamp;
  x @1 :Float64;
  y @2 :Float64;
  z @3 :Float64;
  w @4 :Float64;
  i @5 :Float64;
  j @6 :Float64;
  k @7 :Float64;
}

enum Test {
  a @0;
  b @1;
  c @2;
}

struct StampedPoseList {
  list @0 :List(StampedPose);
  data @1 :List(Int32);
  arr3 @2 :List(Int32);
  i @3 :Int32;
  strs @4 :List(Text);
  en @5 :Test = c;
  i16 @6 :Int16;
}
