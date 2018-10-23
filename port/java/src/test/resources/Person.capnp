@0xebd3cb0ae722ef73;

# Namespacesetup
using Java = import "java.capnp";
$Java.package("ai.madara.tests.capnp");
$Java.outerClassname("Person");


struct Bio {
  name @0 :Text;
  email @1 :Text;
  phones @2 :List(PhoneNumber);
  birthdate @3 :Date;
  photo @4: Data;

  struct PhoneNumber {
    number @0 :Text;
    type @1 :Type;

    enum Type {
      mobile @0;
      home @1;
      work @2;
    }
  }
}

struct Date {
  year @0 :Int16;
  month @1 :UInt8;
  day @2 :UInt8;
}
