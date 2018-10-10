#!/usr/bin/env python2

import sys
import os
import capnp
from madara.knowledge import *

geo = capnp.load(os.environ["MADARA_ROOT"] + "/tests/capnfiles/Geo.capnp")

Any.register_int32("i32");
Any.register_class("Point", geo.Point)
Any.register_class("Pose", geo.Pose)

a = Any("i32")

a.assign(10);

print(a.to_integer())

p = geo.Point.new_message()
p.x = 2
p.y = 4
p.z = 6

b = Any(p)
print(b.tag())
print(b)
print(b.reader())

pose = geo.Pose.new_message()
pose.x = 42
pose.i = 3.14

b.replace(pose)
print(b)
print(b.reader())

kb = KnowledgeBase()

kb.set("asdf", b)
p.clear_write_flag()
kb.set("zxcv", Any(p))

c = kb.get("asdf")

print(c.to_any().reader())

print(c.to_any().get_capnp_bytes())

print(kb.get("zxcv").to_any().reader())

kb.set("apple", 42)
kb.set("bar", "foo")
kb.set("jar", "baz")

print(list(kb.to_map("a")));
print(list(kb.to_map("", ".", "ar")));

b = geo.Pose.from_bytes(c.to_any().get_capnp_bytes())
print(b)
