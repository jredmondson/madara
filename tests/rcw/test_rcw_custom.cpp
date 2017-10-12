#include "madara/knowledge/KnowledgeBase.h"
#include "madara/knowledge/rcw/Transaction.h"
#include "madara/knowledge/rcw/Tracked.h"
#include "madara/utility/Utility.h"
#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>

#include "test.h"

using namespace madara::knowledge;
using namespace madara::knowledge::rcw;

struct my_type
{
  int a;
  int b;
  int c;
};

std::vector<int64_t> get_value(const my_type &v) { return {v.a, v.b, v.c}; }
void set_value(my_type &v, std::vector<int64_t> n) { v.a = n[0]; v.b = n[1]; v.c = n[2]; }

struct my_tracked_type
{
  int a;
  int b;
  int c;
  bool dirty;
};

std::vector<int64_t> get_value(const my_tracked_type &v) { return {v.a, v.b, v.c}; }
void set_value(my_tracked_type &v, std::vector<int64_t> n) { v.a = n[0]; v.b = n[1]; v.c = n[2]; }

bool is_dirty(const my_tracked_type &v) { return v.dirty; }
void clear_dirty(my_tracked_type &v) { v.dirty = false; }

/// Test and demonstrate custom types
int main()
{
  KnowledgeBase kb;
  Transaction tx(kb);

  my_type a, b = {4, 5, 6};
  Tracked<my_type> j, k({14, 15, 16});
  my_tracked_type x, y = {24, 25, 26, false};

  tx.add("a", a);
  tx.add_init("b", b);

  tx.add("j", j);
  tx.add_init("k", k);

  tx.add("x", x);
  tx.add_init("y", y);

  test(!kb.exists("a"));
  test(kb.exists("b"));
  test_eq(kb.get("a").to_integers(), std::vector<int64_t>());
  test_eq(kb.get("b").to_integers(), std::vector<int64_t>({4, 5, 6}));

  test(!kb.exists("j"));
  test(kb.exists("k"));
  test_eq(kb.get("j").to_integers(), std::vector<int64_t>());
  test_eq(kb.get("k").to_integers(), std::vector<int64_t>({14, 15, 16}));

  test(!kb.exists("x"));
  test(kb.exists("y"));
  test_eq(kb.get("x").to_integers(), std::vector<int64_t>());
  test_eq(kb.get("y").to_integers(), std::vector<int64_t>({24, 25, 26}));

  a = {0, 0, 0};
  b = {0, 0, 0};
  j = {0, 0, 0};
  k = {0, 0, 0};
  x = {0, 0, 0, true};
  y = {0, 0, 0, true};

  kb.set("a", std::vector<int64_t>({1, 2, 3}));
  kb.set("j", std::vector<int64_t>({11, 12, 13}));
  kb.set("x", std::vector<int64_t>({21, 22, 23}));

  tx.pull();

  test_eq(a.a, 1);
  test_eq(a.b, 2);
  test_eq(a.c, 3);
  test_eq(b.a, 4);
  test_eq(b.b, 5);
  test_eq(b.c, 6);

  test_eq(j->a, 11);
  test_eq(j->b, 12);
  test_eq(j->c, 13);
  test_eq(k->a, 14);
  test_eq(k->b, 15);
  test_eq(k->c, 16);

  test_eq(x.a, 21);
  test_eq(x.b, 22);
  test_eq(x.c, 23);
  test_eq(x.dirty, false);
  test_eq(y.a, 24);
  test_eq(y.b, 25);
  test_eq(y.c, 26);
  test_eq(y.dirty, false);

  a.a = 51;
  b.a = 54;

  j.get_mut().a = 61;
  k.get_mut().a = 64;

  x.a = 71;
  x.dirty = true;
  y.b = 74;

  tx.push();

  test_eq(kb.get("a").to_integers(), std::vector<int64_t>({51, 2, 3}));
  test_eq(kb.get("b").to_integers(), std::vector<int64_t>({54, 5, 6}));

  test_eq(kb.get("j").to_integers(), std::vector<int64_t>({61, 12, 13}));
  test_eq(kb.get("k").to_integers(), std::vector<int64_t>({64, 15, 16}));

  test_eq(kb.get("x").to_integers(), std::vector<int64_t>({71, 22, 23}));
  test_eq(kb.get("y").to_integers(), std::vector<int64_t>({24, 25, 26}));

  tests_finalize();
}
