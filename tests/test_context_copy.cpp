

#include "madara/knowledge/FileHeader.h"
#include "madara/knowledge/ThreadSafeContext.h"
#include "madara/knowledge/KnowledgeBase.h"

#include "madara/utility/Utility.h"

#include <stdio.h>
#include <iostream>

#include "madara/knowledge/KnowledgeBase.h"
#include "madara/knowledge/containers/StringVector.h"
#include "madara/logger/GlobalLogger.h"

namespace knowledge = madara::knowledge;
namespace containers = knowledge::containers;
namespace logger = madara::logger;

typedef  madara::KnowledgeRecord::Integer  Integer;

void test_copy (void)
{
  knowledge::KnowledgeBase source, dest;
  containers::StringVector kids ("kids", source, 1);
  knowledge::CopySet copy_set;

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
    madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_ALWAYS,
      "TEST 1: Full copy is SUCCESS.\n");
  }
  else
  {
    madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_ALWAYS,
      "TEST 1: Full copy is FAIL.\n");
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
    madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_ALWAYS,
      "TEST 2: Lone name copy is SUCCESS.\n");
  }
  else
  {
    madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_ALWAYS,
      "TEST 2: Lone name copy is FAIL.\n");
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
    madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_ALWAYS,
      "TEST 3: Add age to copy is SUCCESS.\n");
  }
  else
  {
    madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_ALWAYS,
      "TEST 3: Add age to copy is FAIL.\n");
    dest.print ();
  }

  madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_ALWAYS,
    "\nEND OF TESTS CONTEXT CONTENTS:\n"
    "\nSource:\n");
  source.print ();

  madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_ALWAYS, "\nDest:\n");
  dest.print ();
}

int main (int, char **)
{
  test_copy ();

  return 0;
}
