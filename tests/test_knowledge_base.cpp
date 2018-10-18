#include "madara/knowledge/KnowledgeBase.h"
#include "madara/transport/Transport.h"
#include "madara/transport/TransportSettings.h"
#include "madara/logger/GlobalLogger.h"
#include "madara/utility/Utility.h"
#include "test.h"

namespace logger = madara::logger;
namespace utility = madara::utility;

int main(int, char**)
{
  // Create static and dynamic KnowledgeBase objects
  madara::knowledge::KnowledgeBase knowledge(
      "", madara::transport::Types::NO_TRANSPORT);
  madara::knowledge::KnowledgeBase* knowledge1 =
      new madara::knowledge::KnowledgeBase(
          "", madara::transport::Types::NO_TRANSPORT);
  TEST_NE((unsigned long)knowledge1, (unsigned long)NULL);
  // Terminal failure lets get out of her..
  if (knowledge1 == NULL)
  {
    std::cerr << "DYNAMIC OBJECT CREATION: FAILURE.\n";
    return madara_tests_fail_count;
  }

  // Method print tests
  knowledge.print("Testing print method, print works!\n");

  // Test some of the transport methods
  madara::transport::TransportSettings tpSettings;
  tpSettings.type = madara::transport::Types::BROADCAST;
  tpSettings.add_read_domain("domain1");
  size_t numTp;
  size_t tp1r;
  numTp = knowledge1->get_num_transports();
  TEST_EQ((unsigned long)numTp, (unsigned long)0);
  size_t tp1 = knowledge1->attach_transport(
      types_to_string(madara::transport::Types::BROADCAST), tpSettings);
  TEST_GT((unsigned long)tp1, (unsigned long)0);
  tpSettings.add_read_domain("domain2");
  tpSettings.type = madara::transport::Types::UDP;
  size_t tp2 = knowledge1->attach_transport(
      types_to_string(madara::transport::Types::UDP), tpSettings);
  TEST_GT((unsigned long)tp2, (unsigned long)1);
  numTp = knowledge.get_num_transports();
  TEST_EQ((unsigned long)numTp, (unsigned long)0);
  tp1r = knowledge1->remove_transport(tp1);
  TEST_EQ((unsigned long)tp1r, (unsigned long)1);
  numTp = knowledge1->get_num_transports();
  TEST_EQ((unsigned long)numTp, (unsigned long)1);

  // Method get/set tests
  int retval = 0;
  knowledge.print("Testing KnowledgeBase set method: ");
  madara::knowledge::KnowledgeRecord testRecord1(123);
  retval = knowledge.set("testRecord1", testRecord1);
  TEST_EQ(retval, 0);
  if (retval == 0)
    knowledge.print("Success\n");
  else
    knowledge.print("Fail\n");
  knowledge.print("Testing KnowledgeBase get method: ");
  madara::knowledge::KnowledgeRecord record1 = knowledge.get("testRecord1");
  TEST_EQ(record1, testRecord1);

  // get variable refs and brethern methods tests coming soon...
  // VariableReference
  //  get_ref (const std::string & key,
  //      const KnowledgeReferenceSettings & settings =
  //              KnowledgeReferenceSettings (false));

  knowledge.print("Testing KnowledgeRecord status method, return value is: ");
  int record1Status = record1.status();
  char str[16];
  sprintf(str, "%d", record1Status);
  knowledge.print(str);
  knowledge.print("\n");

  // MADARA_debug_level = 10;

  // print to a file
  knowledge.print("Printing to file test_knowledgebase.txt\n");
  logger::global_logger->set_level(logger::LOG_DETAILED);
  logger::global_logger->clear();
  logger::global_logger->add_file("test_knowledgebase.txt");

#ifndef _MADARA_NO_KARL_
  knowledge.evaluate("#print ('Testing print to file only\n')");
#else
  knowledge.print("Testing print to file only\n");
#endif  // _MADARA_NO_KARL_

  // Print to stderr
  logger::global_logger->clear();
  logger::global_logger->add_term();
#ifndef _MADARA_NO_KARL_
  knowledge.evaluate("#print ('Testing KnowledgeBase methods\n')");
#else
  knowledge.print("Testing print to stderr only\n");
#endif  // _MADARA_NO_KARL_

  // Test read a file into the knowledgebase object knowledge
  knowledge.print("Test loading a file in the KnowledgeBase\n");
  knowledge.read_file("testRecord1", "test_knowledgebase.txt");

  // Test logging methods of the knowledgebase object knowledge directly
  knowledge.print("Test logging methods in the KnowledgeBase\n");
  knowledge.print("Test logging methods: get_log_level(), set_log_level()\n");
  knowledge.set_log_level(7);
  int logLevel = knowledge.get_log_level();
  TEST_EQ(logLevel, 7);
  if (logLevel == 7)
    knowledge.print("Success\n");
  else
    knowledge.print("Fail\n");
  knowledge.print("Test logging methods: attach_logger(), get_logger()\n");
  logger::Logger localLogger;
  knowledge.attach_logger(localLogger);
  logger::Logger* localLoggerCopy = &(knowledge.get_logger());
  TEST_EQ(localLoggerCopy, &localLogger);
  if (localLoggerCopy == &localLogger)
    knowledge.print("Success\n");
  else
    knowledge.print("Fail\n");

  knowledge.print("Done with test.\n");

  knowledge.set("a", 1);
  knowledge.set("b", 2);
  knowledge.set("c", 3);

  size_t kcount = 0;
  knowledge.for_each(
    [&](const std::pair<const std::string,
        madara::knowledge::KnowledgeRecord>& cur)
    {
      ++kcount;
      std::cout << cur.first << std::endl;
    });
  std::cerr << "Found " << kcount << " records" << std::endl;

  // Cleanup
  std::cerr << "KnowledgeBase Object Cleanup Started...\n\n";
  delete knowledge1;

  std::cerr << "\nTOTAL TESTS RUN: 11\n";
  if (madara_tests_fail_count > 0)
  {
    std::cerr << "OVERALL: FAIL. " << madara_tests_fail_count
              << " tests failed.\n";
  }
  else
  {
    std::cerr << "OVERALL: SUCCESS.\n";
  }

  return madara_tests_fail_count;
}
