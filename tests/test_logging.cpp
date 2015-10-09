
#include "madara/knowledge/KnowledgeBase.h"
#include "madara/logger/GlobalLogger.h"
#include "madara/utility/Utility.h"

namespace logger = madara::logger;
namespace utility = madara::utility;

int ACE_TMAIN (int, ACE_TCHAR **)
{
  madara::knowledge::KnowledgeBase knowledge;
  
 // MADARA_debug_level = 10;

  // print to a file
  knowledge.print ("Printing to file madara_log_file_only.txt\n");
  logger::global_logger->set_level (logger::LOG_DETAILED);
  logger::global_logger->clear ();
  logger::global_logger->add_file ("test_logging_to_file.txt");

#ifndef _MADARA_NO_KARL_
  knowledge.evaluate ("#print ('Testing print to file only\n')");
#else
  knowledge.print ("Testing print to file only\n");
#endif // _MADARA_NO_KARL_

  // print to stderr
  logger::global_logger->clear ();
  logger::global_logger->add_term ();
#ifndef _MADARA_NO_KARL_
  knowledge.evaluate ("#print ('Testing print to stderr only\n')");
#else
  knowledge.print ("Testing print to stderr only\n");
#endif // _MADARA_NO_KARL_

  logger::global_logger->clear ();
  logger::global_logger->add_syslog ();
#ifndef _MADARA_NO_KARL_
  knowledge.evaluate ("#print ('Testing print to syslog only\n')");
#else
  knowledge.print ("Testing print to syslog only\n");
#endif // _MADARA_NO_KARL_

  // print to a file and stderr
  logger::global_logger->clear ();
  logger::global_logger->add_term ();
  logger::global_logger->add_file ("test_logging_to_file.txt");
#ifndef _MADARA_NO_KARL_
  knowledge.evaluate ("#print ('Testing print to stderr and file\n')");
#else
  knowledge.print ("Testing print to stderr and file\n");
#endif // _MADARA_NO_KARL_

  logger::global_logger->clear ();
  logger::global_logger->add_syslog ();
  logger::global_logger->add_term ();
  logger::global_logger->add_file ("test_logging_to_file.txt");
#ifndef _MADARA_NO_KARL_
  knowledge.evaluate ("#print ('Testing print to stderr, syslog and file\n')");
#else
  knowledge.print ("Testing print to stderr, syslog and file\n");
#endif // _MADARA_NO_KARL_

  logger::global_logger->clear ();
  logger::global_logger->add_term ();

  // set the timestamp format to default
  logger::global_logger->set_timestamp_format ();

  knowledge.print ("Testing timestamp format\n");

  knowledge.print ("Sleeping for 5 seconds\n");

  utility::sleep (5.0);

  knowledge.print ("Finished sleeping. Done with test.\n");

  return 0;
}
