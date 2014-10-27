
#include "madara/transport/Packet_Scheduler.h"
#include "madara/knowledge_engine/Knowledge_Base.h"
#include "madara/utility/Log_Macros.h"
#include "ace/High_Res_Timer.h"
#include "ace/OS.h"

#include <iostream>
#include <string>
#include <sstream>

// command line arguments
int parse_args (int argc, ACE_TCHAR * argv[]);

void test_probablistic (void)
{
  Madara::Transport::QoS_Transport_Settings settings;

  Madara::Transport::Packet_Scheduler scheduler (&settings);
  
  MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_DEBUG, 
    "********************TESTING PROBABLISTIC POLICY*********************\n"));
  
  MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_DEBUG, 
    "****************BEGIN PROBABLISTIC 10%%, BURST=5*****************\n"));

  settings.update_drop_rate (.1,
    Madara::Transport::PACKET_DROP_PROBABLISTIC, 5);

  scheduler.clear ();
  scheduler.reset ();
  
  for (int i = 0; i < 400; ++i)
  {
    scheduler.add ();
    scheduler.print_status (MADARA_LOG_DETAILED_TRACE);
  }
  scheduler.print_status ();
  
  MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_DEBUG, 
    "  Results were "));

  if (scheduler.get_dropped () > 20 && scheduler.get_dropped () < 60)
    MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_DEBUG, 
      "SUCCESS\n"));
  else
    MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_DEBUG, 
      "FAIL\n"));

  MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_DEBUG, 
    "********************BEGIN PROBABLISTIC 20%%*********************\n"));
  
  settings.update_drop_rate (.2, Madara::Transport::PACKET_DROP_PROBABLISTIC);

  scheduler.clear ();
  scheduler.reset ();

  for (int i = 0; i < 400; ++i)
  {
    scheduler.add ();
    scheduler.print_status (MADARA_LOG_DETAILED_TRACE);
  }
  scheduler.print_status ();
  
  MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_DEBUG, 
    "  Results were "));

  if (scheduler.get_dropped () > 60 && scheduler.get_dropped () < 100)
    MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_DEBUG, 
      "SUCCESS\n"));
  else
    MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_DEBUG, 
      "FAIL\n"));


  MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_DEBUG, 
    "********************BEGIN PROBABLISTIC 40%%*********************\n"));

  settings.update_drop_rate (.4, Madara::Transport::PACKET_DROP_PROBABLISTIC);
  
  scheduler.clear ();
  scheduler.reset ();

  for (int i = 0; i < 400; ++i)
  {
    scheduler.add ();
    scheduler.print_status (MADARA_LOG_DETAILED_TRACE);
  }
  scheduler.print_status ();
  
  MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_DEBUG, 
    "  Results were "));

  if (scheduler.get_dropped () > 140 && scheduler.get_dropped () < 180)
    MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_DEBUG, 
      "SUCCESS\n"));
  else
    MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_DEBUG, 
      "FAIL\n"));

  
  MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_DEBUG, 
    "****************BEGIN PROBABLISTIC 40%%, BURST=3*****************\n"));

  settings.update_drop_rate (.4,
    Madara::Transport::PACKET_DROP_PROBABLISTIC, 3);

  scheduler.clear ();
  scheduler.reset ();
  
  for (int i = 0; i < 400; ++i)
  {
    scheduler.add ();
    scheduler.print_status (MADARA_LOG_DETAILED_TRACE);
  }
  scheduler.print_status ();
  
  MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_DEBUG, 
    "  Results were "));
  
  if (scheduler.get_dropped () > 140 && scheduler.get_dropped () < 180)
    MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_DEBUG, 
      "SUCCESS\n"));
  else
    MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_DEBUG, 
      "FAIL\n"));

  
  MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_DEBUG, 
    "*****************BEGIN PROBABLISTIC 50%%, BURST=5******************\n"));

  settings.update_drop_rate (.5,
    Madara::Transport::PACKET_DROP_PROBABLISTIC, 5);

  scheduler.clear ();
  scheduler.reset ();
  
  for (int i = 0; i < 400; ++i)
  {
    scheduler.add ();
    scheduler.print_status (MADARA_LOG_DETAILED_TRACE);
  }
  scheduler.print_status ();
  
  MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_DEBUG, 
    "  Results were "));

  if (scheduler.get_dropped () > 180 && scheduler.get_dropped () < 220)
    MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_DEBUG, 
      "SUCCESS\n"));
  else
    MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_DEBUG, 
      "FAIL\n"));

  
  MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_DEBUG, 
    "*****************BEGIN PROBABLISTIC 80%%******************\n"));

  settings.update_drop_rate (.8,
    Madara::Transport::PACKET_DROP_PROBABLISTIC, 1);

  scheduler.clear ();
  scheduler.reset ();
  
  for (int i = 0; i < 400; ++i)
  {
    scheduler.add ();
    scheduler.print_status (MADARA_LOG_DETAILED_TRACE);
  }
  scheduler.print_status ();
  
  MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_DEBUG, 
    "  Results were "));

  if (scheduler.get_dropped () > 300 && scheduler.get_dropped () < 340)
    MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_DEBUG, 
      "SUCCESS\n"));
  else
    MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_DEBUG, 
      "FAIL\n"));

  
}

void test_deterministic (void)
{
  Madara::Transport::QoS_Transport_Settings settings;

  Madara::Transport::Packet_Scheduler scheduler (&settings);
  
  MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_DEBUG, 
    "*******************TESTING DETERMINISTIC POLICY********************\n"));
  
  MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_DEBUG, 
    "****************BEGIN DETERMINISTIC 10%%, BURST=6*****************\n"));

  settings.update_drop_rate (.1,
    Madara::Transport::PACKET_DROP_DETERMINISTIC, 6);

  scheduler.clear ();
  scheduler.reset ();
  
  for (int i = 0; i < 400; ++i)
  {
    scheduler.add ();
    scheduler.print_status (MADARA_LOG_DETAILED_TRACE);
  }
  scheduler.print_status ();
  
  MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_DEBUG, 
    "  Results were "));

  if (scheduler.get_dropped () > 20 && scheduler.get_dropped () < 60)
    MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_DEBUG, 
      "SUCCESS\n"));
  else
    MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_DEBUG, 
      "FAIL\n"));

  MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_DEBUG, 
    "*****************BEGIN DETERMINISTIC 20%%******************\n"));
  
  settings.update_drop_rate (.2, Madara::Transport::PACKET_DROP_DETERMINISTIC);

  scheduler.clear ();
  scheduler.reset ();

  for (int i = 0; i < 400; ++i)
  {
    scheduler.add ();
    scheduler.print_status (MADARA_LOG_DETAILED_TRACE);
  }
  scheduler.print_status ();
  
  MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_DEBUG, 
    "  Results were "));

  if (scheduler.get_dropped () == 80)
    MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_DEBUG, 
      "SUCCESS\n"));
  else
    MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_DEBUG, 
      "FAIL\n"));
  
  MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_DEBUG, 
    "*****************BEGIN DETERMINISTIC 30%%******************\n"));

  settings.update_drop_rate (.3,
    Madara::Transport::PACKET_DROP_DETERMINISTIC, 1);

  scheduler.clear ();
  scheduler.reset ();
  
  for (int i = 0; i < 400; ++i)
  {
    scheduler.add ();
    scheduler.print_status (MADARA_LOG_DETAILED_TRACE);
  }
  scheduler.print_status ();
  
  MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_DEBUG, 
    "  Results were "));

  if (scheduler.get_dropped () == 120)
    MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_DEBUG, 
      "SUCCESS\n"));
  else
    MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_DEBUG, 
      "FAIL\n"));

  
  MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_DEBUG, 
    "********************BEGIN DETERMINISTIC 40%%*********************\n"));

  settings.update_drop_rate (.4,
    Madara::Transport::PACKET_DROP_DETERMINISTIC);

  scheduler.clear ();
  
  for (int i = 0; i < 400; ++i)
  {
    scheduler.add ();
    scheduler.print_status (MADARA_LOG_DETAILED_TRACE);
  }
  scheduler.print_status ();
  
  MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_DEBUG, 
    "  Results were "));

  if (scheduler.get_dropped () == 160)
    MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_DEBUG, 
      "SUCCESS\n"));
  else
    MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_DEBUG, 
      "FAIL\n"));

  
  MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_DEBUG, 
    "*****************BEGIN DETERMINISTIC 40%%, BURST=3******************\n"));

  settings.update_drop_rate (.4,
    Madara::Transport::PACKET_DROP_DETERMINISTIC, 3);

  scheduler.clear ();
  scheduler.reset ();
  
  for (int i = 0; i < 400; ++i)
  {
    scheduler.add ();
    scheduler.print_status (MADARA_LOG_DETAILED_TRACE);
  }
  scheduler.print_status ();
  
  MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_DEBUG, 
    "  Results were "));

  if (scheduler.get_dropped () > 140 && scheduler.get_dropped () < 180)
    MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_DEBUG, 
      "SUCCESS\n"));
  else
    MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_DEBUG, 
      "FAIL\n"));

  
  MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_DEBUG, 
    "*****************BEGIN DETERMINISTIC 50%%, BURST=3******************\n"));

  settings.update_drop_rate (.5,
    Madara::Transport::PACKET_DROP_DETERMINISTIC, 3);

  scheduler.clear ();
  scheduler.reset ();
  
  for (int i = 0; i < 400; ++i)
  {
    scheduler.add ();
    scheduler.print_status (MADARA_LOG_DETAILED_TRACE);
  }
  scheduler.print_status ();
  
  MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_DEBUG, 
    "  Results were "));
  
  if (scheduler.get_dropped () > 180 && scheduler.get_dropped () < 220)
    MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_DEBUG, 
      "SUCCESS\n"));
  else
    MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_DEBUG, 
      "FAIL\n"));

  
  MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_DEBUG, 
    "*****************BEGIN DETERMINISTIC 50%%, BURST=10******************\n"));

  settings.update_drop_rate (.5,
    Madara::Transport::PACKET_DROP_DETERMINISTIC, 10);

  scheduler.clear ();
  scheduler.reset ();
  
  for (int i = 0; i < 400; ++i)
  {
    scheduler.add ();
    scheduler.print_status (MADARA_LOG_DETAILED_TRACE);
  }
  scheduler.print_status ();
  
  MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_DEBUG, 
    "  Results were "));
  
  if (scheduler.get_dropped () > 180 && scheduler.get_dropped () < 220)
    MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_DEBUG, 
      "SUCCESS\n"));
  else
    MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_DEBUG, 
      "FAIL\n"));

  
  MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_DEBUG, 
    "*****************BEGIN DETERMINISTIC 80%%******************\n"));

  settings.update_drop_rate (.8,
    Madara::Transport::PACKET_DROP_DETERMINISTIC, 1);

  scheduler.clear ();
  scheduler.reset ();
  
  for (int i = 0; i < 400; ++i)
  {
    scheduler.add ();
    scheduler.print_status (MADARA_LOG_DETAILED_TRACE);
  }
  scheduler.print_status ();
  
  MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_DEBUG, 
    "  Results were "));

  if (scheduler.get_dropped () == 320)
    MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_DEBUG, 
      "SUCCESS\n"));
  else
    MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_DEBUG, 
      "FAIL\n"));

}

int ACE_TMAIN (int argc, ACE_TCHAR * argv[])
{
  parse_args (argc, argv);

  test_probablistic ();
  test_deterministic ();

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
        std::stringstream buffer (argv[i + 1]);
        buffer >> MADARA_debug_level;
      }

      ++i;
    }
    else if (arg1 == "-f" || arg1 == "--logfile")
    {
      if (i + 1 < argc)
      {
        Madara::Knowledge_Engine::Knowledge_Base::log_to_file (argv[i + 1]);
      }

      ++i;
    }
    else
    {
      MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_DEBUG, 
        "\nProgram summary for %s:\n\n" \
"This test checks the functionality of the Bandwidth Monitoring class\n"
        " [-l|--level level]       the logger level (0+, higher is higher detail)\n" \
        " [-f|--logfile file]      log to a file\n" \
        "\n",
        argv[0]));
      exit (0);
    }
  }

  return 0;
}

