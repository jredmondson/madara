#include "madara/knowledge/KnowledgeBase.h"
#include "madara/knowledge/rcw/Transaction.h"
#include "madara/knowledge/rcw/Tracked.h"
#include "madara/utility/Utility.h"
#include <stdio.h>
#include <iostream>
#include <string>
#include <array>
#include <vector>

#include "test.h"

using namespace madara;
using namespace knowledge;
using namespace madara::knowledge::rcw;

int main(int, char**)
{
  KnowledgeBase kb;
  Transaction tx(kb);

  Tracked<int> x, y(2), z;
  Tracked<std::string> a, b("bar"), c;
  Tracked<std::vector<int64_t>> u, v({4, 5, 6}), w;
  Tracked<std::vector<int64_t>> pu, pv({14, 15, 16}), pw;

  tx.add("x", x);
  tx.add_init("y", y);
  tx.build("z", z).init(3).add();

  tx.add("a", a);
  tx.add_init("b", b);
  tx.build("c", c).init("baz").add();

  tx.add("u", u);
  tx.add_init("v", v);
  tx.build("w", w).init({7, 8, 9}).add();

  tx.add_prefix("pu", pu);
  tx.add_prefix_init("pv", pv);
  tx.build("pw", pw).prefix().init({17, 18, 19}).add();

  test(!kb.exists("x"));
  test(kb.exists("y"));
  test(kb.exists("z"));
  test_eq(kb.get("x"), 0);
  test_eq(kb.get("y"), 2);
  test_eq(kb.get("z"), 3);

  test(!kb.exists("a"));
  test(kb.exists("b"));
  test(kb.exists("c"));
  test_eq(kb.get("a"), "");
  test_eq(kb.get("b"), "bar");
  test_eq(kb.get("c"), "baz");

  test(!kb.exists("u"));
  test(kb.exists("v"));
  test(kb.exists("w"));
  test_eq(kb.get("u").to_integers(), std::vector<int64_t>());
  test_eq(kb.get("v").to_integers(), std::vector<int64_t>({4, 5, 6}));
  test_eq(kb.get("w").to_integers(), std::vector<int64_t>({7, 8, 9}));

  test(!kb.exists("pu.size"));
  test(kb.exists("pv.size"));
  test(kb.exists("pw.size"));
  test_eq(kb.get("pu.size"), 0);
  test_eq(kb.get("pv.size"), 3);
  test_eq(kb.get("pv.0"), 14L);
  test_eq(kb.get("pv.1"), 15L);
  test_eq(kb.get("pv.2"), 16L);
  test_eq(kb.get("pw.size"), 3);
  test_eq(kb.get("pw.0"), 17L);
  test_eq(kb.get("pw.1"), 18L);
  test_eq(kb.get("pw.2"), 19L);

  x = -1;
  y = 0;
  z = 0;
  a = "";
  b = "";
  c = "";
  v.clear();
  w.clear();
  pv.clear();
  pw.clear();

  tx.pull();

  test_eq(x, 0);
  test_eq(y, 2);
  test_eq(z, 3);

  test_eq(a, "");
  test_eq(b, "bar");
  test_eq(c, "baz");

  test_eq(*u, std::vector<int64_t>());
  test_eq(*v, std::vector<int64_t>({4, 5, 6}));
  test_eq(*w, std::vector<int64_t>({7, 8, 9}));

  x = 42;
  y++;
  --z;
  a += "X";
  b.get_mutable() += b;
  c.set(1, 'q');
  u.push_back(*x);
  v.clear();
  w.set(1, 13);

  pu.push_back(*x);
  pv.clear();
  pw.set(1, 13);

  test_eq(kb.get("x"), 0);
  test_eq(kb.get("y"), 2);
  test_eq(kb.get("z"), 3);

  test_eq(kb.get("a"), "");
  test_eq(kb.get("b"), "bar");
  test_eq(kb.get("c"), "baz");

  test_eq(kb.get("u").to_integers(), std::vector<int64_t>());
  test_eq(kb.get("v").to_integers(), std::vector<int64_t>({4, 5, 6}));
  test_eq(kb.get("w").to_integers(), std::vector<int64_t>({7, 8, 9}));

  kb.set_index("w", 0, (int64_t)17);
  test_eq(kb.get("w").to_integers(), std::vector<int64_t>({17, 8, 9}));

  kb.set("pw.0", (int64_t)27);

  test_eq(kb.get("pw.size"), 3);
  test_eq(kb.get("pw.0"), 27);
  test_eq(kb.get("pw.1"), 18);
  test_eq(kb.get("pw.2"), 19);

  tx.push();

  test_eq(kb.get("x"), 42);
  test_eq(kb.get("y"), 3);
  test_eq(kb.get("z"), 2);

  test_eq(kb.get("a"), "X");
  test_eq(kb.get("b"), "barbar");
  test_eq(kb.get("c"), "bqz");

  test_eq(kb.get("u").to_integers(), std::vector<int64_t>{42});
  test_eq(kb.get("v").to_integers(), std::vector<int64_t>({}));
  test_eq(kb.get("w").to_integers(), std::vector<int64_t>({17, 13, 9}));

  test_eq(kb.get("pu.size"), 1);
  test_eq(kb.get("pu.0"), 42);
  test_eq(kb.get("pv.size"), 0);
  test(!kb.exists("pv.0"));
  test(!kb.exists("pv.1"));
  test(!kb.exists("pv.2"));
  test_eq(kb.get("pw.size"), 3);
  test_eq(kb.get("pw.0"), 27);
  test_eq(kb.get("pw.1"), 13);
  test_eq(kb.get("pw.2"), 19);

  class Inconvertible
  {
  };

  test(!supports_knowledge_cast<Inconvertible>::value);

  test_eq(knowledge_cast(123), 123L);
  test(supports_knowledge_cast<int>::value);

  class Convertible
  {
  public:
    operator int() const
    {
      return 42;
    }
  };

  test(!supports_knowledge_cast<Convertible>::value);

  test_eq(knowledge_cast(123), 123L);
  test_eq(knowledge_cast(123.25).to_double(), 123.25L);
  test_eq(knowledge_cast<int>(KnowledgeRecord(123)), 123L);
  test_eq(knowledge_cast<float>(KnowledgeRecord(123.5)), 123.5);

  test(!knowledge_cast<bool>(KnowledgeRecord()));
  test(!knowledge_cast<bool>(KnowledgeRecord(0)));
  test(!knowledge_cast<bool>(KnowledgeRecord("")));
  test(knowledge_cast<bool>(KnowledgeRecord(1)));
  test(knowledge_cast<bool>(KnowledgeRecord("foo")));
  test_eq(KnowledgeRecord("0").type(), KnowledgeRecord::STRING);
  test(knowledge_cast<bool>(KnowledgeRecord("0")));
  test_eq(knowledge_cast(true), 1);
  test_eq(knowledge_cast(false), 0);

  int64_t iarray[] = {1, 2, 3, 4, 5};
  test_eq(knowledge_cast(iarray).share_integers()->at(3), 4L);

  uint16_t uiarray[] = {1, 2, 3, 4, 5};
  test_eq(knowledge_cast(uiarray).share_integers()->at(3), 4L);

  float farray[] = {1.1F, 2.2F, 3.3F, 4.4F, 5.5F};
  test_eq(knowledge_cast(farray).share_doubles()->at(4), 5.5L);

  unsigned char ucarray[] = {1, 2, 3, 4, 5};
  test_eq(knowledge_cast(ucarray).share_binary()->at(3), 4L);

  char sarray[] = "asdf";
  test_eq(knowledge_cast(sarray).share_string()->at(3), 'f');

  std::vector<int> ivector = {2, 4, 6, 8};
  test_eq(knowledge_cast(ivector).share_integers()->at(1), 4L);

  std::vector<int64_t> i64vector = {2, 4, 6, 8};
  void* i64vector_orig = &i64vector[0];
  test_eq((void*)&knowledge_cast(std::move(i64vector)).share_integers()->at(0),
      i64vector_orig);

  auto uni_farray = mk_unique<std::vector<double>>(&farray[0], &farray[5]);
  auto ptr_farray = uni_farray.get();
  auto rec_farray = knowledge_cast(std::move(uni_farray));
  test_eq(rec_farray.share_doubles()->at(4), 5.5L);
  test_eq((void*)rec_farray.share_doubles().get(), (void*)ptr_farray);

  uint16_t iarray_in[] = {4, 8, 12, 16};
  int iarray_out[] = {0, 0, 0, 0, 1, 1, 1};
  knowledge_cast(knowledge_cast(iarray_in), iarray_out);
  test_eq(iarray_out[2], 12);
  test_eq(iarray_out[6], 0);

  static_assert(
      !madara::knowledge::impl::supports_resize<std::array<int, 7>&>::value,
      "");
  std::array<int32_t, 7> istdarray_out{{0, 0, 0, 0, 1, 1, 1}};

  knowledge_cast(knowledge_cast(iarray_in), istdarray_out);
  test_eq(istdarray_out[3], 16);
  test_eq(istdarray_out[5], 0);

  float farray_in[] = {4.5, 8.5, 12.5, 16.5};
  float farray_out[] = {0, 0, 0, 0, 1, 1, 1};
  knowledge_cast(knowledge_cast(farray_in), farray_out);
  test_eq(farray_out[2], 12.5);
  test_eq(farray_out[6], 0);

  farray_out[2] = 0;
  farray_out[3] = 0;
  knowledge_cast(knowledge_cast(farray_in), farray_out, 3);
  test_eq(farray_out[2], 12.5);
  test_eq(farray_out[3], 0);

  std::array<float, 7> fstdarray_out{{0, 0, 0, 0, 1, 1, 1}};
  knowledge_cast(knowledge_cast(farray_in), fstdarray_out);
  test_eq(fstdarray_out[3], 16.5);
  test_eq(fstdarray_out[5], 0);

  std::vector<float> fstdvector_out = {1, 2, 3};
  knowledge_cast(knowledge_cast(farray_in), fstdvector_out);
  test_eq(fstdvector_out.size(), 4UL);
  test_eq(fstdvector_out[2], 12.5);

  //int int_out;
  //knowledge_cast(knowledge_cast(13), int_out);
  //test_eq(int_out, 13);

  std::string hello = "hello world!";
  std::string hello_out;
  test_eq(hello, knowledge_cast<std::string>(knowledge_cast(hello)));
  test_eq(hello, knowledge_cast(knowledge_cast(hello), hello_out));

  hello_out = "";

  KnowledgeRecord empty_record;
  test_eq((bool)(empty_record == NULL), true);
  test_eq((bool)(knowledge_cast(1) == NULL), false);
  test_eq((bool)(knowledge_cast(0) == NULL), true);
  test_eq((bool)(knowledge_cast(0) == nullptr), true);

  // Two-param version should not have explicit type given
  // test_eq(hello, knowledge_cast<std::string>(knowledge_cast(hello),
  // hello_out));

  // wide-char strings not supported
#if 0
  std::u32string hello32 = U"hello world!";
  std::u32string hello32_out;
  test_eq(hello32, knowledge_cast(knowledge_cast(hello32)));
  test_eq(hello, knowledge_cast(knowledge_cast(hello32), hello32_out));
#endif

  tests_finalize();
}
