
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

class MyRCW : public threads::RCWThread
{
private:
  int x;
  rcw::TrackedInt y;
  rcw::TrackedInt z;
  std::vector<int64_t> vec;
  std::string s;
  rcw::TrackedString t;
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
  }

  virtual void compute (rcw::Transaction &)
  {
    std::cout << "x: " << x << std::endl;
    std::cout << "y: " << y.get() << std::endl;
    std::cout << "z: " << *z << std::endl;
    std::cout << "vec.size(): " << vec.size() << std::endl;
    std::cout << "s: " << s << std::endl;
    std::cout << "t: " << *t << std::endl;

    ++x;
    y = *y - 1;
    vec.push_back(x);
    s += "x";
    t = (*t + "T");

    std::cout << "y.dirty: " << y.is_dirty() << std::endl;
    std::cout << "z.dirty: " << z.is_dirty() << std::endl;
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
