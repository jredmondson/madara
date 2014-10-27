
#include "madara/knowledge_engine/containers/Reliable_Record.h"
#include "madara/knowledge_engine/Knowledge_Base.h"
#include <iostream>

namespace engine = Madara::Knowledge_Engine;
namespace containers = engine::Containers;

void test_reliable_record (engine::Knowledge_Base & knowledge)
{
  unsigned char buffer[240000];
  knowledge.set_file ("my_file", buffer, 100000);

  containers::Reliable_Record record ("my_file", knowledge);
  record.resize (0, 2);
}

int main (int argc, char * argv[])
{
  engine::Knowledge_Base knowledge;
  test_reliable_record (knowledge);

  return 0;
}
