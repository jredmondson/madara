
#include <iostream>
#include "madara/knowledge/Knowledge_Base.h"
#include "madara/threads/Threader.h"

// some shortcuts for common MADARA namespaces
namespace knowledge = madara::knowledge;
namespace threads = madara::Threads;

// 1: Extend a Base_Thread
class Hello_World : public threads::Base_Thread
{
  public:
    void run (void)
    {
      std::cerr << "Hello World!\n";
    }
};

int main (int, char **)
{
  // 2: Create a Knowledge Base
  knowledge::Knowledge_Base knowledge;

  // 3: Create a Threader 
  threads::Threader threader (knowledge);

  // 4: Run threads
  threader.run ("hello_world", new Hello_World ());

  // 5: Wait for threads to finish
  threader.wait ();

  return 0;
}
