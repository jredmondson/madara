#include "madara/knowledge/KnowledgeBase.h"
#include "madara/logger/GlobalLogger.h"
#include "madara/utility/Utility.h"

namespace logger = madara::logger;
namespace utility = madara::utility;

int main (int, char **)
{
  // Create static KnowledgeBase object using the default constructor
  madara::knowledge::KnowledgeBase knowledge;  

  // method print tests
  knowledge.print("Testing print method, print works!\n");

  // method get/set tests
  int retval = 0;
  knowledge.print("Testing KnowledgeBase set method: ");
  madara::knowledge::KnowledgeRecord testRecord1;
  retval = knowledge.set("testRecord1", testRecord1);
  if (retval == 0)
     knowledge.print("Success\n");
  else
     knowledge.print("Fail\n");

  knowledge.print ("Testing KnowledgeBase get method: ");
  madara::knowledge::KnowledgeRecord record1 = knowledge.get("testRecord1");

  knowledge.print("Testing KnowledgeRecord status method, return value is: ");
  int record1Status = record1.status();
  char str[16];
  sprintf(str, "%d", record1Status);
  knowledge.print (str);
  knowledge.print ("\n");

  // MADARA_debug_level = 10;

  // print to a file
  knowledge.print("Printing to file test_knowledgebase.txt\n");
  logger::global_logger->set_level(logger::LOG_DETAILED);
  logger::global_logger->clear();
  logger::global_logger->add_file ("test_knowledgebase.txt");

#ifndef _MADARA_NO_KARL_
  knowledge.evaluate("#print ('Testing print to file only\n')");
#else
  knowledge.print ("Testing print to file only\n");
#endif // _MADARA_NO_KARL_

  // print to stderr
  logger::global_logger->clear();
  logger::global_logger->add_term();
#ifndef _MADARA_NO_KARL_
  knowledge.evaluate("#print ('Testing KnowledgeBase methods\n')");
#else
  knowledge.print ("Testing print to stderr only\n");
#endif // _MADARA_NO_KARL_

  // Test read a file into the knowledgebase object knowledge
  knowledge.print("Test loading a file in the KnowledgeBase\n");
  knowledge.read_file("testRecord1", "test_knowledgebase.txt");

  // Test logging methods of the knowledgebase object knowledge directly
  knowledge.print("Test logging methods in the KnowledgeBase\n");
  knowledge.print("Test logging methods: get_log_level(), set_log_level()\n");
  knowledge.set_log_level (7);
  int logLevel = knowledge.get_log_level();
  if (logLevel == 7)
     knowledge.print("Success\n");
  else
     knowledge.print("Fail\n");
  knowledge.print("Test logging methods: attach_logger(), get_logger()\n");
  logger::Logger localLogger;
  knowledge.attach_logger (localLogger);
  logger::Logger *localLoggerCopy = &(knowledge.get_logger());
  if (localLoggerCopy == &localLogger)
      knowledge.print("Success\n");
   else
      knowledge.print("Fail\n");

  knowledge.print("Sleeping for 5 seconds\n");

  utility::sleep(5.0);

  knowledge.print("Finished sleeping. Done with test.\n");

  return 0;
}
