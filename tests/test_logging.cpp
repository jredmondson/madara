
#include "madara/knowledge_engine/Knowledge_Base.h"

int ACE_TMAIN (int argc, ACE_TCHAR ** argv)
{
  Madara::Knowledge_Engine::Knowledge_Base knowledge;
  
 // MADARA_debug_level = 10;

  // print to a file
  knowledge.print ("Printing to file madara_log_file_only.txt\n");
  knowledge.log_to_file ("madara_log_file_only.txt");

#ifndef _MADARA_NO_KARL_
  knowledge.evaluate ("#print ('Testing print to file only\n')");
#else
  knowledge.print ("Testing print to file only\n");
#endif // _MADARA_NO_KARL_

  // print to stderr
  knowledge.log_to_stderr ();
#ifndef _MADARA_NO_KARL_
  knowledge.evaluate ("#print ('Testing print to stderr only\n')");
#else
  knowledge.print ("Testing print to stderr only\n");
#endif // _MADARA_NO_KARL_
  
  if (argc > 1)
  {
    // try printing to the system log, file, and stderr
    knowledge.log_to_system_log (argv[0], false);
  }

#ifndef _MADARA_NO_KARL_
  knowledge.evaluate ("#print ('Testing print to syslog only\n')");
#else
  knowledge.print ("Testing print to syslog only\n");
#endif // _MADARA_NO_KARL_

  // print to a file and stderr
  knowledge.log_to_stderr ();
  knowledge.log_to_file ("madara_stderr_copy.txt", false);
#ifndef _MADARA_NO_KARL_
  knowledge.evaluate ("#print ('Testing print to stderr and file\n')");
#else
  knowledge.print ("Testing print to stderr and file\n");
#endif // _MADARA_NO_KARL_

  if (argc > 1)
  {
    // try printing to the system log, file, and stderr
    knowledge.log_to_system_log (argv[0], false);
  }

  knowledge.log_to_stderr (false);
#ifndef _MADARA_NO_KARL_
  knowledge.evaluate ("#print ('Testing print to stderr, syslog and file\n')");
#else
  knowledge.print ("Testing print to stderr, syslog and file\n");
#endif // _MADARA_NO_KARL_

  return 0;
}
