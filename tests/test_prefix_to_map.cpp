

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

typedef madara::knowledge::KnowledgeRecord::Integer Integer;

int madara_fails = 0;

void print_map(const knowledge::KnowledgeMap& kmap)
{
  for (knowledge::KnowledgeMap::const_iterator i = kmap.begin();
       i != kmap.end(); ++i)
  {
    std::cout << i->first << " -> " << i->second << std::endl;
  }
}

void test_prefix_to_map(void)
{
  knowledge::KnowledgeBase kbase;

  kbase.set("astart.x", "x");
  kbase.set("test.a", "a");
  kbase.set("test.b", "b");
  kbase.set("test.c", "c");
  kbase.set("test/", "");
  kbase.set("test/d", "d");
  kbase.set("zend.z", "z");
  kbase.set("zend.zz", "zz");

  kbase.print();

  knowledge::KnowledgeMap kmap(kbase.to_map("test."));

  print_map(kmap);

  if (kmap.size() == 3 && kmap["test.a"] == "a" && kmap["test.b"] == "b" &&
      kmap["test.c"] == "c")
  {
    madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_ALWAYS,
        "TEST 1: extracting test. prefix copy is SUCCESS.\n");
  }
  else
  {
    madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_ALWAYS,
        "TEST 1: extracting test. prefix copy is FAIL\n");
    ++madara_fails;
  }

  knowledge::KnowledgeMap kmap2(kbase.to_map("astart."));

  print_map(kmap2);

  if (kmap2.size() == 1 && kmap2["astart.x"] == "x")
  {
    madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_ALWAYS,
        "TEST 2: extracting astart. prefix copy is SUCCESS.\n");
  }
  else
  {
    madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_ALWAYS,
        "TEST 2: extracting astart. prefix copy is FAIL\n");
    ++madara_fails;
  }

  knowledge::KnowledgeMap kmap3(kbase.to_map("zend."));

  print_map(kmap3);

  if (kmap3.size() == 2 && kmap3["zend.z"] == "z" && kmap3["zend.zz"] == "zz")
  {
    madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_ALWAYS,
        "TEST 3: extracting zend. prefix copy is SUCCESS.\n");
  }
  else
  {
    madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_ALWAYS,
        "TEST 3: extracting zend. prefix copy is FAIL\n");
    ++madara_fails;
  }

  knowledge::KnowledgeMap kmap4(kbase.to_map(""));

  print_map(kmap4);

  if (kmap4.size() == 8)
  {
    madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_ALWAYS,
        "TEST 4: extracting empty prefix copy is SUCCESS.\n");
  }
  else
  {
    madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_ALWAYS,
        "TEST 4: extracting empty prefix copy is FAIL\n");
    ++madara_fails;
  }
}

int main(int, char**)
{
  test_prefix_to_map();

  if (madara_fails > 0)
  {
    std::cerr << "OVERALL: FAIL. " << madara_fails << " tests failed.\n";
  }
  else
  {
    std::cerr << "OVERALL: SUCCESS.\n";
  }

  return madara_fails;
}
