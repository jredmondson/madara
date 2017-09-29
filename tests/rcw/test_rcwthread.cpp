
#include "madara/knowledge/KnowledgeBase.h"
#include "madara/threads/RCWThread.h"
#include "madara/utility/Utility.h"
#include <stdio.h>
#include <iostream>
#include <string>
#include "madara/logger/GlobalLogger.h"

namespace logger = madara::logger;

namespace knowledge = madara::knowledge;
namespace utility = madara::utility;
namespace threads = madara::threads;
namespace rcw = madara::rcw;

void handle_arguments (int argc, char ** argv)
{
  for (int i = 1; i < argc; ++i)
  {
    std::string arg1 (argv[i]);

    if (true) {
    } else {
      madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_ALWAYS, 
        "\nProgram summary for %s:\n\n" \
        "  Test the timed executor, which executes periodic tasks.\n\n" \
        " [-l|--level level]       the logger level (0+, higher is higher detail)\n" \
        " [-f|--logfile file]      log to a file\n" \
        "\n",
        argv[0]);
      exit (0);
    }
  }
}

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

class MyRCW : public threads::RCWThread
{
private:
  int x;
  rcw::TrackedInt y;
  rcw::TrackedInt z;
  std::vector<int64_t> vec;
  std::string s;
  rcw::TrackedString t;
  my_type m;
  my_tracked_type d;
public:
  virtual void setup (rcw::Transaction &tx)
  {
    tx.add("x", x);
    tx.add("y", y);
    tx.add("z", z);
    tx.add("vec", vec);
    tx.add_init("s", s);

    t.set("asdf");
    tx.add_init("t", t);

    m.a = 4;
    m.b = 5;
    m.c = 6;
    tx.add_init("m", m);
    std::cout << "m: {" << m.a << ", " << m.b << ", " << m.c << "}" << std::endl;

    d.a = 7;
    d.b = 8;
    d.c = 9;
    tx.add_init("d", d);
    std::cout << "d: {" << d.a << ", " << d.b << ", " << d.c << "}" << std::endl;
  }

  virtual void compute (rcw::Transaction &)
  {
    std::cout << "x: " << x << std::endl;
    std::cout << "y: " << y.get() << std::endl;
    std::cout << "z: " << *z << std::endl;
    std::cout << "vec.size(): " << vec.size() << std::endl;
    std::cout << "s: " << s << std::endl;
    std::cout << "t: " << *t << std::endl;
    std::cout << "m: {" << m.a << ", " << m.b << ", " << m.c << "}" << std::endl;
    std::cout << "d: {" << d.a << ", " << d.b << ", " << d.c << "}" << std::endl;

    ++x;
    y = *y - 1;
    vec.push_back(x);
    s += "x";
    t = (*t + "T");
    ++m.a;
    --m.b;
    ++d.a;
    --d.b;
    d.dirty = true;

    std::cout << "y.dirty: " << y.is_dirty() << std::endl;
    std::cout << "z.dirty: " << z.is_dirty() << std::endl;
    std::cout << "d.dirty: " << is_dirty(d) << std::endl;
  }
};

void test_rcwthread (void)
{
  knowledge::KnowledgeBase knowledge;
  MyRCW my_thread;

  threads::BaseThread *thread = &my_thread;
  thread->init(knowledge);
  for (int x = 0; x < 10; ++x) {
    thread->run();
  }
  thread->cleanup();
}

int main (int argc, char ** argv)
{
  handle_arguments (argc, argv);

  test_rcwthread ();

  return 0;
}
