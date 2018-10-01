#include "madara/knowledge/KnowledgeBase.h"
#include "madara/knowledge/rcw/Transaction.h"
#include "madara/utility/Utility.h"
#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>

#include "test.h"

using namespace madara::knowledge;
using namespace madara::knowledge::rcw;

int main(int, char**)
{
  KnowledgeBase kb;
  Transaction tx(kb);

  int x, y = 2, z;
  std::string a, b = "bar", c;
  std::vector<int64_t> u, v = {4, 5, 6}, w;

  tx.add("x", x);
  tx.add_init("y", y);
  tx.build("z", z).init(3).add();

  tx.add("a", a);
  tx.add_init("b", b);
  tx.build("c", c).init("baz").add();

  tx.add("u", u);
  tx.add_init("v", v);
  tx.build("w", w).init({7, 8, 9}).add();

  test(!kb.exists("x"));
  test(kb.exists("y"));
  test(kb.exists("z"));
  test_eq(kb.get("x").to_integer(), 0);
  test_eq(kb.get("y").to_integer(), 2);
  test_eq(kb.get("z").to_integer(), 3);

  test(!kb.exists("a"));
  test(kb.exists("b"));
  test(kb.exists("c"));
  test_eq(kb.get("a").to_string(), "");
  test_eq(kb.get("b").to_string(), "bar");
  test_eq(kb.get("c").to_string(), "baz");

  test(!kb.exists("u"));
  test(kb.exists("v"));
  test(kb.exists("w"));
  test_eq(kb.get("u").to_integers(), std::vector<int64_t>());
  test_eq(kb.get("v").to_integers(), std::vector<int64_t>({4, 5, 6}));
  test_eq(kb.get("w").to_integers(), std::vector<int64_t>({7, 8, 9}));

  x = -1;
  y = 0;
  z = 0;
  a = "";
  b = "";
  c = "";
  v.clear();
  w.clear();

  tx.pull();

  test_eq(x, 0);
  test_eq(y, 2);
  test_eq(z, 3);

  test_eq(a, "");
  test_eq(b, "bar");
  test_eq(c, "baz");

  test_eq(u, std::vector<int64_t>());
  test_eq(v, std::vector<int64_t>({4, 5, 6}));
  test_eq(w, std::vector<int64_t>({7, 8, 9}));

  x = 42;
  y++;
  --z;
  a += "X";
  b += b;
  c[1] = 'q';
  u.push_back(x);
  v.clear();
  w[1] = 13;

  test_eq(kb.get("x").to_integer(), 0);
  test_eq(kb.get("y").to_integer(), 2);
  test_eq(kb.get("z").to_integer(), 3);

  test_eq(kb.get("a").to_string(), "");
  test_eq(kb.get("b").to_string(), "bar");
  test_eq(kb.get("c").to_string(), "baz");

  test_eq(kb.get("u").to_integers(), std::vector<int64_t>());
  test_eq(kb.get("v").to_integers(), std::vector<int64_t>({4, 5, 6}));
  test_eq(kb.get("w").to_integers(), std::vector<int64_t>({7, 8, 9}));

  kb.set_index("w", 0, (int64_t)17);
  test_eq(kb.get("w").to_integers(), std::vector<int64_t>({17, 8, 9}));

  tx.push();

  test_eq(kb.get("x").to_integer(), 42);
  test_eq(kb.get("y").to_integer(), 3);
  test_eq(kb.get("z").to_integer(), 2);

  test_eq(kb.get("a").to_string(), "X");
  test_eq(kb.get("b").to_string(), "barbar");
  test_eq(kb.get("c").to_string(), "bqz");

  test_eq(kb.get("u").to_integers(), std::vector<int64_t>{42});
  test_eq(kb.get("v").to_integers(), std::vector<int64_t>({}));
  test_eq(kb.get("w").to_integers(), std::vector<int64_t>({7, 13, 9}));

  tests_finalize();
}
