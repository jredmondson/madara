#include "madara/knowledge/KnowledgeBase.h"

void kb_scoped_level_2 (void)
{
  madara::knowledge::KnowledgeBase knowledge;
  knowledge.set ("var2", 2.0);
}

void kb_scoped_level_1 (void)
{
  madara::knowledge::KnowledgeBase knowledge;
  knowledge.set ("var1", 1.0);

  kb_scoped_level_2 ();
}

void kb_copy_level_2 (madara::knowledge::KnowledgeBase knowledge)
{
  knowledge.set ("var2", 2.0);
}

void kb_copy_level_1 (void)
{
  madara::knowledge::KnowledgeBase knowledge;
  knowledge.set ("var1", 1.0);

  kb_copy_level_2 (knowledge);
}

void kb_ref_level_2 (madara::knowledge::KnowledgeBase & knowledge)
{
  knowledge.set ("var2", 2.0);
}

void kb_ref_level_1 (void)
{
  madara::knowledge::KnowledgeBase knowledge;
  knowledge.set ("var1", 1.0);

  kb_copy_level_2 (knowledge);
}

int main (int argc, char ** argv)
{
  kb_scoped_level_1 ();
  kb_scoped_level_1 ();
  kb_scoped_level_1 ();

  kb_copy_level_1 ();
  kb_copy_level_1 ();
  kb_copy_level_1 ();

  kb_ref_level_1 ();
  kb_ref_level_1 ();
  kb_ref_level_1 ();

  return 0;
}