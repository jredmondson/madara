
#include <iostream>
#include <atomic>
#include "madara/knowledge/KnowledgeBase.h"
#include "madara/threads/Threader.h"

// some shortcuts for common MADARA namespaces
namespace knowledge = madara::knowledge;
namespace threads = madara::threads;

std::atomic<bool> ran (false);

// 1: Extend a BaseThread
class HelloWorld : public threads::BaseThread
{
public:
  void run(void)
  {
    std::cerr << "Hello World!\n";
    ran = true;
  }
};

int main(int, char**)
{
  // 2: Create a Knowledge Base
  knowledge::KnowledgeBase knowledge;

  // 3: Create a Threader
  threads::Threader threader(knowledge);

  // 4: Run threads
  threader.run("hello_world", new HelloWorld());

  // 5: Wait for threads to finish
  threader.wait();

  std::cout << "test_threader_hello_world:\n";
  std::cout << "  result: " << (ran.load() ? "SUCCESS" : "FAIL") << "\n";

  return ran.load() != true;
}
