
#include "madara/knowledge_engine/Knowledge_Base.h"
#include "madara/logger/Global_Logger.h"

namespace logger = Madara::Logger;

int ACE_TMAIN (int argc, ACE_TCHAR ** argv)
{
  Madara::Knowledge_Engine::Knowledge_Base knowledge;
  
 // MADARA_debug_level = 10;

  // print to a file
  knowledge.print ("Printing to file madara_log_file_only.txt\n");
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

  return 0;
}
