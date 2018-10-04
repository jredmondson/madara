
#include <iostream>
#include "madara/knowledge/KnowledgeBase.h"
#include "madara/threads/Threader.h"

// some shortcuts for common MADARA namespaces
namespace knowledge = madara::knowledge;
namespace threads = madara::threads;

// 1: Extend a BaseThread
class HelloWorld : public threads::BaseThread
{
public:
  void run(void)
  {
    std::cerr << "Hello World!\n";
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

  return 0;
}
