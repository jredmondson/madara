#include "madara/knowledge/KnowledgeBase.h"
#include "madara/threads/RCWThread.h"
#include "madara/knowledge/rcw/Tracked.h"
#include "madara/knowledge/rcw/Transaction.h"
#include "madara/utility/Utility.h"
#include <stdio.h>
#include <iostream>
#include <string>
#include "madara/logger/GlobalLogger.h"

namespace logger = madara::logger;

namespace knowledge = madara::knowledge;
namespace utility = madara::utility;
namespace threads = madara::threads;
namespace rcw = madara::knowledge::rcw;

void handle_arguments(int argc, char** argv)
{
  for (int i = 1; i < argc; ++i)
  {
    std::string arg1(argv[i]);

    if (true)
    {
    }
    else
    {
      madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_ALWAYS,
          "\nProgram summary for %s:\n\n"
          "  Test the timed executor, which executes periodic tasks.\n\n"
          " [-l|--level level]       the logger level (0+, higher is higher "
          "detail)\n"
          " [-f|--logfile file]      log to a file\n"
          "\n",
          argv[0]);
      exit(0);
    }
  }
}

struct my_type
{
  int a;
  int b;
  int c;
};

std::vector<int64_t> get_value(const my_type& v)
{
  return {v.a, v.b, v.c};
}
void set_value(my_type& v, std::vector<int64_t> n)
{
  v.a = n[0];
  v.b = n[1];
  v.c = n[2];
}

struct my_tracked_type
{
  int a;
  int b;
  int c;
  bool dirty;
};

std::vector<int64_t> get_value(const my_tracked_type& v)
{
  return {v.a, v.b, v.c};
}
void set_value(my_tracked_type& v, std::vector<int64_t> n)
{
  v.a = n[0];
  v.b = n[1];
  v.c = n[2];
}

bool is_dirty(const my_tracked_type& v)
{
  return v.dirty;
}
void clear_dirty(my_tracked_type& v)
{
  v.dirty = false;
}

class MyRCW : public threads::RCWThread
{
public:
  int x;
  rcw::TrackedInt y;
  rcw::TrackedInt z;
  int x_ro;
  int y_ro;
  std::vector<int64_t> vec;
  rcw::TrackedIntVector tvec;
  rcw::TrackedIntVector pvec;
  std::vector<rcw::TrackedI64> uvec;
  std::string s;
  rcw::TrackedString t;
  rcw::Tracked<std::vector<std::string>> strvec;
  my_type m;
  my_tracked_type d;

  void setup(rcw::Transaction& tx) override
  {
    tx.add("x", x);
    tx.add("y", y);
    tx.add("z", z);
    tx.build("x", x_ro).ro().add();
    tx.build("y", y_ro).ro().add();
    tx.add("vec", vec);

    tx.add_init("tvec", tvec);
    tx.add_prefix("pvec", pvec);
    std::cout << "strvec " << &strvec << " " << (void*)&strvec << std::endl;
    tx.build("strvec", strvec).init({"a", "b", "c"}).prefix().add();
    tx.add_init("uvec", uvec);
    tx.add_init("s", s);

    tx.build("t", t).init("asdf").add();

    m.a = 4;
    m.b = 5;
    m.c = 6;
    tx.add_init("m", m);
    std::cout << "m: {" << m.a << ", " << m.b << ", " << m.c << "}"
              << std::endl;

    d.a = 7;
    d.b = 8;
    d.c = 9;
    tx.add_init("d", d);
    std::cout << "d: {" << d.a << ", " << d.b << ", " << d.c << "}"
              << std::endl;
  }

  void compute(const rcw::Transaction& tx) override
  {
    std::cout << "x: " << x << std::endl;
    std::cout << "y: " << y << std::endl;
    std::cout << "z: " << *z << std::endl;
    std::cout << "x_ro: " << x_ro << std::endl;
    std::cout << "y_ro: " << y_ro << std::endl;
    std::cout << "vec.size(): " << vec.size() << std::endl;

    std::cout << "tvec.size(): " << tvec.size() << std::endl;
    std::cout << "   ";
    for (auto& cur : tvec)
    {
      std::cout << cur << " ";
    }
    std::cout << std::endl;

    std::cout << "uvec.size(): " << uvec.size() << std::endl;
    std::cout << "   ";
    for (auto& cur : uvec)
    {
      std::cout << *cur << " ";
    }
    std::cout << std::endl;

    std::cout << "pvec.size(): " << pvec.size() << std::endl;
    std::cout << "   ";
    for (auto& cur : pvec)
    {
      std::cout << cur << " ";
    }
    std::cout << std::endl;

    std::cout << "strvec.size(): " << strvec.size() << std::endl;
    std::cout << "   ";
    for (auto& cur : strvec)
    {
      std::cout << cur << " ";
    }
    std::cout << std::endl;

    std::cout << "s: " << s << "  [" << s.size() << "]" << std::endl;
    std::cout << "t: " << *t << "  [" << t.size() << "]" << std::endl;
    std::cout << "m: {" << m.a << ", " << m.b << ", " << m.c << "}"
              << std::endl;
    std::cout << "d: {" << d.a << ", " << d.b << ", " << d.c << "}"
              << std::endl;

    ++x;
    y--;
    --y;
    x_ro *= 42;
    y_ro *= 42;
    vec.push_back(x);
    tvec.push_back((int)y);
    pvec.push_back((int)y * 10);
    strvec.push_back("FOOBAR");
    int foo = 2;
    uvec.push_back(rcw::track(y * foo));
    std::cout << std::endl;
    s += "x";
    t += "T";
    ++m.a;
    --m.b;
    ++d.a;
    --d.b;
    d.dirty = true;

    std::cout << "strvec " << &strvec << " " << (void*)&strvec << std::endl;
    std::cout << "Remove strvec by ref: " << tx.remove(&strvec) << std::endl;
    std::cout << "Remove tvec by name: " << tx.remove("tvec") << std::endl;

    std::cout << "y.dirty: " << y.is_dirty() << std::endl;
    std::cout << "z.dirty: " << z.is_dirty() << std::endl;
    std::cout << "d.dirty: " << is_dirty(d) << std::endl;
    std::cout << "pvec.all_dirty: " << is_all_dirty(pvec) << std::endl;
    std::cout << "pvec.size_dirty: " << is_size_dirty(pvec) << std::endl;
  }
};

void test_rcwthread(void)
{
  knowledge::KnowledgeBase knowledge;
  MyRCW my_thread;

  threads::BaseThread* thread = &my_thread;
  thread->init(knowledge);
  for (int x = 0; x < 10; ++x)
  {
    thread->run();

    my_thread.pvec.clear();
    my_thread.uvec.clear();
    my_thread.tvec.clear();
    my_thread.strvec.clear();
  }
  thread->cleanup();
}

int main(int argc, char** argv)
{
  handle_arguments(argc, argv);

  test_rcwthread();

  return 0;
}
