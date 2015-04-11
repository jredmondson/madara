

#include "madara/knowledge_engine/File_Header.h"
#include "madara/knowledge_engine/Thread_Safe_Context.h"
#include "madara/knowledge_engine/Knowledge_Base.h"
#include "madara/utility/Log_Macros.h"
#include "madara/utility/Utility.h"

#include <stdio.h>
#include <iostream>

#include "madara/knowledge_engine/Knowledge_Base.h"
#include "madara/knowledge_engine/containers/String_Vector.h"

namespace engine = Madara::Knowledge_Engine;
namespace containers = engine::Containers;

typedef  Madara::Knowledge_Record::Integer  Integer;

void test_copy (void)
{
  engine::Knowledge_Base source, dest;
  containers::String_Vector kids ("kids", source, 1);
  engine::Copy_Set copy_set;

  // create some information in the source knowledge base
  source.set ("name", "John Smith");
  source.set ("age", Integer (20));
  source.set ("occupation", "Explorer");
  source.set ("spouse", "Pocahontas");
  kids.set (0, "Thomas Rolfe");

  // TEST 1: Copy everything to destination
  dest.copy (source, copy_set, false);
  if (dest.get ("name") == "John Smith" &&
      dest.get ("age") == Integer (20) &&
      dest.get ("occupation") == "Explorer" &&
      dest.get ("spouse") == "Pocahontas" &&
      dest.get ("kids.0") == "Thomas Rolfe")
  {
    std::cout << "TEST 1: Full copy is SUCCESS.\n";
  }
  else
  {
    std::cout << "TEST 1: Full copy is FAIL.\n";
    dest.print ();
  }

  // TEST 2: Copy name to destination
  copy_set.clear ();
  copy_set["name"] = true;

  dest.copy (source, copy_set, true);
  if (dest.get ("name") == "John Smith" &&
      !dest.exists ("age") &&
      !dest.exists ("occupation") &&
      !dest.exists ("spouse") &&
      !dest.exists ("kids.0"))
  {
    std::cout << "TEST 2: Lone name copy is SUCCESS.\n";
  }
  else
  {
    std::cout << "TEST 2: Lone name copy is FAIL.\n";
    dest.print ();
  }

  // TEST 3: Add age to destination
  copy_set.clear ();
  copy_set["age"] = true;

  dest.copy (source, copy_set, false);
  if (dest.get ("name") == "John Smith" &&
      dest.get ("age") == Integer (20) &&
      !dest.exists ("occupation") &&
      !dest.exists ("spouse") &&
      !dest.exists ("kids.0"))
  {
    std::cout << "TEST 3: Add age to copy is SUCCESS.\n";
  }
  else
  {
    std::cout << "TEST 3: Add age to copy is FAIL.\n";
    dest.print ();
  }

  std::cout << "\nEND OF TESTS CONTEXT CONTENTS:\n";
  std::cout << "\nSource:\n";
  source.print ();

  std::cout << "\nDest:\n";
  dest.print ();
}

int main (int, char **)
{
  test_copy ();

  return 0;
}
