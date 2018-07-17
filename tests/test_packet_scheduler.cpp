
#include "madara/transport/PacketScheduler.h"
#include "madara/knowledge/KnowledgeBase.h"
#include "madara/logger/GlobalLogger.h"

#include <iostream>
#include <string>
#include <sstream>

namespace logger = madara::logger;

int madara_fails = 0;

// command line arguments
int parse_args (int argc, char * argv[]);

void test_probablistic (void)
{
  madara::transport::QoSTransportSettings settings;

  madara::transport::PacketScheduler scheduler (&settings);

  madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_ALWAYS,
    "********************TESTING PROBABLISTIC POLICY*********************\n");

  madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_ALWAYS,
    "****************BEGIN PROBABLISTIC 10%%, BURST=5*****************\n");

  settings.update_drop_rate (.1,
    madara::transport::PACKET_DROP_PROBABLISTIC, 5);

  scheduler.clear ();
  scheduler.reset ();

  const int packet_count = 4000;

  for (int i = 0; i < packet_count; ++i)
  {
    scheduler.add ();
    scheduler.print_status (madara::logger::LOG_DETAILED);
  }
  scheduler.print_status ();

  madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_ALWAYS,
    "  Results were ");

  if (scheduler.get_dropped () > 0 && scheduler.get_dropped () < packet_count &&
      scheduler.get_dropped () + scheduler.get_sent () == packet_count)
  {
    madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_ALWAYS,
      "SUCCESS\n");
  }
  else
  {
    madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_ALWAYS,
      "FAIL\n"); ++madara_fails;
  }

  madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_ALWAYS,
    "********************BEGIN PROBABLISTIC 20%%*********************\n");

  settings.update_drop_rate (.2, madara::transport::PACKET_DROP_PROBABLISTIC);

  scheduler.clear ();
  scheduler.reset ();

  for (int i = 0; i < packet_count; ++i)
  {
    scheduler.add ();
    scheduler.print_status (madara::logger::LOG_DETAILED);
  }
  scheduler.print_status ();

  madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_ALWAYS,
    "  Results were ");

  if (scheduler.get_dropped () > 0 && scheduler.get_dropped () < packet_count &&
      scheduler.get_dropped () + scheduler.get_sent () == packet_count)
  {
    madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_ALWAYS,
      "SUCCESS\n");
  }
  else
  {
    madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_ALWAYS,
      "FAIL\n"); ++madara_fails;
  }

  madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_ALWAYS,
    "********************BEGIN PROBABLISTIC 40%%*********************\n");

  settings.update_drop_rate (.4, madara::transport::PACKET_DROP_PROBABLISTIC);
  
  scheduler.clear ();
  scheduler.reset ();

  for (int i = 0; i < packet_count; ++i)
  {
    scheduler.add ();
    scheduler.print_status (madara::logger::LOG_DETAILED);
  }
  scheduler.print_status ();

  madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_ALWAYS,
    "  Results were ");

  if (scheduler.get_dropped () > 0 && scheduler.get_dropped () < packet_count &&
      scheduler.get_dropped () + scheduler.get_sent () == packet_count)
  {
    madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_ALWAYS,
      "SUCCESS\n");
  }
  else
  {
    madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_ALWAYS,
      "FAIL\n"); ++madara_fails;
  }

  madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_ALWAYS,
    "****************BEGIN PROBABLISTIC 40%%, BURST=3*****************\n");

  settings.update_drop_rate (.4,
    madara::transport::PACKET_DROP_PROBABLISTIC, 3);

  scheduler.clear ();
  scheduler.reset ();
  
  for (int i = 0; i < packet_count; ++i)
  {
    scheduler.add ();
    scheduler.print_status (madara::logger::LOG_DETAILED);
  }
  scheduler.print_status ();

  madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_ALWAYS,
    "  Results were ");
  
  if (scheduler.get_dropped () > 0 && scheduler.get_dropped () < packet_count &&
      scheduler.get_dropped () + scheduler.get_sent () == packet_count)
  {
    madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_ALWAYS,
      "SUCCESS\n");
  }
  else
  {
    madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_ALWAYS,
      "FAIL\n"); ++madara_fails;
  }

  madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_ALWAYS,
    "*****************BEGIN PROBABLISTIC 50%%, BURST=5******************\n");

  settings.update_drop_rate (.5,
    madara::transport::PACKET_DROP_PROBABLISTIC, 5);

  scheduler.clear ();
  scheduler.reset ();
  
  for (int i = 0; i < packet_count; ++i)
  {
    scheduler.add ();
    scheduler.print_status (madara::logger::LOG_DETAILED);
  }
  scheduler.print_status ();

  madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_ALWAYS,
    "  Results were ");

  if (scheduler.get_dropped () > 0 && scheduler.get_dropped () < packet_count &&
      scheduler.get_dropped () + scheduler.get_sent () == packet_count)
  {
    madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_ALWAYS,
      "SUCCESS\n");
  }
  else
  {
    madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_ALWAYS,
      "FAIL\n"); ++madara_fails;
  }


  madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_ALWAYS,
    "*****************BEGIN PROBABLISTIC 80%%******************\n");

  settings.update_drop_rate (.8,
    madara::transport::PACKET_DROP_PROBABLISTIC, 1);

  scheduler.clear ();
  scheduler.reset ();
  
  for (int i = 0; i < packet_count; ++i)
  {
    scheduler.add ();
    scheduler.print_status (madara::logger::LOG_DETAILED);
  }
  scheduler.print_status ();

  madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_ALWAYS,
    "  Results were ");

  if (scheduler.get_dropped () > 0 && scheduler.get_dropped () < packet_count &&
      scheduler.get_dropped () + scheduler.get_sent () == packet_count)
  {
    madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_ALWAYS,
      "SUCCESS\n");
  }
  else
  {
    madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_ALWAYS,
      "FAIL\n"); ++madara_fails;
  }

  
}

void test_deterministic (void)
{
  madara::transport::QoSTransportSettings settings;

  madara::transport::PacketScheduler scheduler (&settings);

  madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_ALWAYS,
    "*******************TESTING DETERMINISTIC POLICY********************\n");

  madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_ALWAYS,
    "****************BEGIN DETERMINISTIC 10%%, BURST=6*****************\n");

  settings.update_drop_rate (.1,
    madara::transport::PACKET_DROP_DETERMINISTIC, 6);

  scheduler.clear ();
  scheduler.reset ();
  
  for (int i = 0; i < 400; ++i)
  {
    scheduler.add ();
    scheduler.print_status (madara::logger::LOG_DETAILED);
  }
  scheduler.print_status ();

  madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_ALWAYS,
    "  Results were ");

  if (scheduler.get_dropped () > 20 && scheduler.get_dropped () < 60)
  {
    madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_ALWAYS,
      "SUCCESS\n");
  }
  else
  {
    madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_ALWAYS,
      "FAIL\n"); ++madara_fails;
  }

  madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_ALWAYS,
    "*****************BEGIN DETERMINISTIC 20%%******************\n");
  
  settings.update_drop_rate (.2, madara::transport::PACKET_DROP_DETERMINISTIC);

  scheduler.clear ();
  scheduler.reset ();

  for (int i = 0; i < 400; ++i)
  {
    scheduler.add ();
    scheduler.print_status (madara::logger::LOG_DETAILED);
  }
  scheduler.print_status ();

  madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_ALWAYS,
    "  Results were ");

  if (scheduler.get_dropped () == 80)
  {
    madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_ALWAYS,
      "SUCCESS\n");
  }
  else
  {
    madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_ALWAYS,
      "FAIL\n"); ++madara_fails;
  }

  madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_ALWAYS,
    "*****************BEGIN DETERMINISTIC 30 %%******************\n");

  settings.update_drop_rate (.3,
    madara::transport::PACKET_DROP_DETERMINISTIC, 1);

  scheduler.clear ();
  scheduler.reset ();
  
  for (int i = 0; i < 400; ++i)
  {
    scheduler.add ();
    scheduler.print_status (madara::logger::LOG_DETAILED);
  }
  scheduler.print_status ();

  madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_ALWAYS,
    "  Results were ");

  if (scheduler.get_dropped () == 120)
  {
    madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_ALWAYS,
      "SUCCESS\n");
  }
  else
  {
    madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_ALWAYS,
      "FAIL\n"); ++madara_fails;
  }

  madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_ALWAYS,
    "********************BEGIN DETERMINISTIC 40%%*********************\n");

  settings.update_drop_rate (.4,
    madara::transport::PACKET_DROP_DETERMINISTIC);

  scheduler.clear ();
  
  for (int i = 0; i < 400; ++i)
  {
    scheduler.add ();
    scheduler.print_status (madara::logger::LOG_DETAILED);
  }
  scheduler.print_status ();

  madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_ALWAYS,
    "  Results were ");

  if (scheduler.get_dropped () == 160)
  {
    madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_ALWAYS,
      "SUCCESS\n");
  }
  else
  {
    madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_ALWAYS,
      "FAIL\n"); ++madara_fails;
  }

  madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_ALWAYS,
    "*****************BEGIN DETERMINISTIC 40%%, BURST=3******************\n");

  settings.update_drop_rate (.4,
    madara::transport::PACKET_DROP_DETERMINISTIC, 3);

  scheduler.clear ();
  scheduler.reset ();
  
  for (int i = 0; i < 400; ++i)
  {
    scheduler.add ();
    scheduler.print_status (madara::logger::LOG_DETAILED);
  }
  scheduler.print_status ();

  madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_ALWAYS,
    "  Results were ");

  if (scheduler.get_dropped () > 140 && scheduler.get_dropped () < 180)
  {
    madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_ALWAYS,
      "SUCCESS\n");
  }
  else
  {
    madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_ALWAYS,
      "FAIL\n"); ++madara_fails;
  }

  madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_ALWAYS,
    "*****************BEGIN DETERMINISTIC 50%%, BURST=3******************\n");

  settings.update_drop_rate (.5,
    madara::transport::PACKET_DROP_DETERMINISTIC, 3);

  scheduler.clear ();
  scheduler.reset ();
  
  for (int i = 0; i < 400; ++i)
  {
    scheduler.add ();
    scheduler.print_status (madara::logger::LOG_DETAILED);
  }
  scheduler.print_status ();

  madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_ALWAYS,
    "  Results were ");
  
  if (scheduler.get_dropped () > 180 && scheduler.get_dropped () < 220)
  {
    madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_ALWAYS,
      "SUCCESS\n");
  }
  else
  {
    madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_ALWAYS,
      "FAIL\n"); ++madara_fails;
  }

  madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_ALWAYS,
    "*****************BEGIN DETERMINISTIC 50%%, BURST=10******************\n");

  settings.update_drop_rate (.5,
    madara::transport::PACKET_DROP_DETERMINISTIC, 10);

  scheduler.clear ();
  scheduler.reset ();
  
  for (int i = 0; i < 400; ++i)
  {
    scheduler.add ();
    scheduler.print_status (madara::logger::LOG_DETAILED);
  }
  scheduler.print_status ();

  madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_ALWAYS,
    "  Results were ");
  
  if (scheduler.get_dropped () > 140 && scheduler.get_dropped () < 220)
  {
    madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_ALWAYS,
      "SUCCESS\n");
  }
  else
  {
    madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_ALWAYS,
      "FAIL\n"); ++madara_fails;
  }

  madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_ALWAYS,
    "*****************BEGIN DETERMINISTIC 80%%******************\n");

  settings.update_drop_rate (.8,
    madara::transport::PACKET_DROP_DETERMINISTIC, 1);

  scheduler.clear ();
  scheduler.reset ();
  
  for (int i = 0; i < 400; ++i)
  {
    scheduler.add ();
    scheduler.print_status (madara::logger::LOG_DETAILED);
  }
  scheduler.print_status ();

  madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_ALWAYS,
    "  Results were ");

  if (scheduler.get_dropped () == 320)
  {
    madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_ALWAYS,
      "SUCCESS\n");
  }
  else
  {
    madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_ALWAYS,
      "FAIL\n"); ++madara_fails;
  }

}

int main (int argc, char * argv[])
{
  parse_args (argc, argv);

  test_probablistic ();
  test_deterministic ();

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


int parse_args (int argc, char * argv[])
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
    else if (arg1 == "-f" || arg1 == "--logfile")
    {
      if (i + 1 < argc)
      {
        logger::global_logger->add_file (argv[i + 1]);
      }

      ++i;
    }
    else
    {
      madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_EMERGENCY,
        "\nProgram summary for %s:\n\n" \
        "This test checks the functionality of the Bandwidth Monitoring class\n"
        " [-l|--level level]       the logger level (0+, higher is higher detail)\n" \
        " [-f|--logfile file]      log to a file\n" \
        "\n",
        argv[0]);

      exit (0);
    }
  }

  return 0;
}

