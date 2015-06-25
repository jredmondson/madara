
#include "madara/transport/Bandwidth_Monitor.h"

#include "ace/High_Res_Timer.h"
#include "ace/OS.h"

#include <iostream>
#include <string>
#include <sstream>

#include "madara/logger/Global_Logger.h"

namespace logger = Madara::Logger;

// command line arguments
int parse_args (int argc, ACE_TCHAR * argv[]);

int ACE_TMAIN (int argc, ACE_TCHAR * argv[])
{
  parse_args (argc, argv);

  Madara::Transport::Bandwidth_Monitor monitor;
  
  std::cerr << "Adding ten 150 byte messages to bandwidth monitor...\n";
  std::cerr << "And sleeping for 10s...\n\n";

  for (int i = 0; i < 10; ++i)
  {
    monitor.add (150);
    ACE_OS::sleep (1);
  }
  
  std::cerr << "Initial bandwidth utilization...\n";
  monitor.print_utilization ();
  if (monitor.get_bytes_per_second () >= 100)
    std::cerr << "Bandwidth check results in SUCCESS\n\n";
  else
    std::cerr << "Bandwidth check results in FAIL\n\n";


  
  std::cerr << "Sleeping for 5 seconds...\n";
  ACE_OS::sleep (5);
  
  std::cerr << "New bandwidth utilization...\n";
  monitor.print_utilization ();
  if (monitor.get_bytes_per_second () >= 60)
    std::cerr << "Bandwidth check results in SUCCESS\n\n";
  else
    std::cerr << "Bandwidth check results in FAIL\n\n";
  
  std::cerr << "Sleeping for 3 seconds...\n";
  ACE_OS::sleep (3);
  
  std::cerr << "New bandwidth utilization...\n";
  monitor.print_utilization ();
  if (monitor.get_bytes_per_second () >= 15)
    std::cerr << "Bandwidth check results in SUCCESS\n\n";
  else
    std::cerr << "Bandwidth check results in FAIL\n\n";
  
  monitor.clear ();
  
  std::cerr << "Setting window to 30s...\n";

  monitor.set_window (30);

  
  std::cerr << "Adding 100 1000 byte messages to bandwidth monitor...\n";
  std::cerr << "And sleeping for 10s...\n\n";
  
  for (int i = 0; i < 100; ++i)
  {
    monitor.add (1000);
  }
  
  ACE_OS::sleep (10);

  
  std::cerr << "New bandwidth utilization...\n";
  monitor.print_utilization ();
  if (monitor.get_bytes_per_second () >= 3000)
    std::cerr << "Bandwidth check results in SUCCESS\n\n";
  else
    std::cerr << "Bandwidth check results in FAIL\n\n";
  

  return 0;
}


int parse_args (int argc, ACE_TCHAR * argv[])
{
  for (int i = 1; i < argc; ++i)
  {
    std::string arg1 (argv[i]);

    if (arg1 == "-l" || arg1 == "--level")
    {
      if (i + 1 < argc)
      {
        int level;
        std::stringstream buffer (argv[i + 1]);
        buffer >> level;

        logger::global_logger->set_level (level);
      }

      ++i;
    }
    else
    {
      madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_ALWAYS,
"\nProgram summary for %s:\n\n" \
"This test checks the functionality of the Bandwidth Monitoring class\n"
" [-l|--level level]       the logger level (0+, higher is higher detail)\n" \
"\n",
        argv[0]);
      exit (0);
    }
  }

  return 0;
}

