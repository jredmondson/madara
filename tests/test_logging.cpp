
#include "madara/knowledge_engine/Knowledge_Base.h"

int ACE_TMAIN (int argc, ACE_TCHAR * argv[])
{
  Madara::Knowledge_Engine::Knowledge_Base knowledge;
  
 // MADARA_debug_level = 10;

  // print to a file
  knowledge.print ("Printing to file madara_log_file_only.txt\n");
  knowledge.log_to_file ("madara_log_file_only.txt");
  knowledge.evaluate ("#print ('Testing print to file only\n')");

  // print to stderr
  knowledge.log_to_stderr ();
  knowledge.evaluate ("#print ('Testing print to stderr only\n')");
  
  // try printing to system log
  knowledge.log_to_system_log (argv[0]);
  knowledge.evaluate ("#print ('Testing print to syslog only\n')");

  // print to a file and stderr
  knowledge.log_to_stderr ();
  knowledge.log_to_file ("madara_stderr_copy.txt", false);
  knowledge.evaluate ("#print ('Testing print to stderr and file\n')");

  // try printing to the system log, file, and stderr
  knowledge.log_to_system_log (argv[0], false);
  knowledge.log_to_stderr (false);
  knowledge.evaluate ("#print ('Testing print to stderr, syslog and file\n')");

  return 0;
}
