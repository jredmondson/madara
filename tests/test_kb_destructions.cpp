#include "madara/knowledge/KnowledgeBase.h"

void kb_scoped_level_2(void)
{
  madara::knowledge::KnowledgeBase knowledge;
  knowledge.set("var2", 2.0);
}

void kb_scoped_level_1(void)
{
  madara::knowledge::KnowledgeBase knowledge;
  knowledge.set("var1", 1.0);

  kb_scoped_level_2();
}

void kb_copy_level_2(madara::knowledge::KnowledgeBase knowledge)
{
  knowledge.set("var2", 2.0);
}

void kb_copy_level_1(void)
{
  madara::knowledge::KnowledgeBase knowledge;
  knowledge.set("var1", 1.0);

  kb_copy_level_2(knowledge);
}

void kb_ref_level_2(madara::knowledge::KnowledgeBase& knowledge)
{
  knowledge.set("var2", 2.0);
}

void kb_ref_level_1(void)
{
  madara::knowledge::KnowledgeBase knowledge;
  knowledge.set("var1", 1.0);

  kb_copy_level_2(knowledge);
}

int main(int, char**)
{
  kb_scoped_level_1();
  kb_scoped_level_1();
  kb_scoped_level_1();

  kb_copy_level_1();
  kb_copy_level_1();
  kb_copy_level_1();

  kb_ref_level_1();
  kb_ref_level_1();
  kb_ref_level_1();

  // this test is really checking for memory errors on KB operations
  // so if this fails, there will be an exception or out-of-bounds
  // which means there's no need for a madara_fails here

  std::cerr << "OVERALL: SUCCESS.\n";

  return 0;
}