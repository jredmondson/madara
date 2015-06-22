
#include "madara/knowledge_engine/Knowledge_Base.h"
#include "madara/knowledge_engine/Timed_Executor.h"
#include "madara/utility/Utility.h"
#include <stdio.h>
#include <iostream>
#include <string>
#include "madara/logger/Global_Logger.h"

namespace logger = Madara::Logger;

namespace engine = Madara::Knowledge_Engine;
namespace utility = Madara::Utility;

void handle_arguments (int argc, char ** argv)
{
  for (int i = 1; i < argc; ++i)
  {
    std::string arg1 (argv[i]);

    if (arg1 == "-l" || arg1 == "--level")
    {
      if (i + 1 < argc)
      {
        std::stringstream buffer (argv[i + 1]);
        int level;
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
      logger::global_logger->log (logger::LOG_ALWAYS, 
        "\nProgram summary for %s:\n\n" \
        "  Test the timed executor, which executes periodic tasks.\n\n" \
        " [-l|--level level]       the logger level (0+, higher is higher detail)\n" \
        " [-f|--logfile file]      log to a file\n" \
        "\n",
        argv[0]);
      exit (0);
    }
  }
}

void test_timed_executor (void)
{
  engine::Knowledge_Base knowledge;
  engine::Timed_Executor executor;
  engine::Event new_event;

  knowledge.print ("Launching 4 threads\n");
  executor.launch_threads (4);
  
  knowledge.print ("Shutting down threads\n");
  executor.shutdown ();
  
  knowledge.print ("Launching 2 threads\n");
  executor.launch_threads (2);
  
  knowledge.print (
    "Adding a periodic print task with 5 executions, once every 1s\n");
  executor.add (
    engine::fill_event (
    knowledge, "++executions; #print ('  Executed event {executions}.\n')", 0.0, 0.2, 5));

  utility::sleep (4);

  if (knowledge.get ("executions").to_integer () == 5)
    knowledge.print ("SUCCESS. Executed 5 times.\n");
  else
    knowledge.print ("FAIL. Executed {executions} times.\n");


  knowledge.print ();
  knowledge.clear ();
  
  new_event = engine::fill_event (
    knowledge, "++executions < 10; #print ('  Executed event {executions}.\n')", 0.1, 0.1);
  new_event.cancel_on_false = true;

  knowledge.print (
    "\nAdding an indefinite periodic print task, executing once every 1s\n");
  executor.add (new_event);
  
  utility::sleep (4);
  //knowledge.print ();

  if (knowledge.get ("executions").to_integer () == 10)
    knowledge.print ("SUCCESS. Executed 10 times.\n");
  else
    knowledge.print ("FAIL. Executed {executions} times.\n");

  knowledge.clear ();
  
  knowledge.print (
    "\nAdding four separate, ordered events.\n");
  new_event = engine::fill_event (
    knowledge, "executions_4 = ++executions; #print ('  Event 4 executed\n')", 1.0, 0.0, 1);
  executor.add (new_event);
  new_event = engine::fill_event (
    knowledge, "executions_1 = ++executions; #print ('  Event 1 executed\n')", 0.25, 0.0, 1);
  executor.add (new_event);
  new_event = engine::fill_event (
    knowledge, "executions_3 = ++executions; #print ('  Event 3 executed\n')", 0.75, 0.0, 1);
  executor.add (new_event);
  new_event = engine::fill_event (
    knowledge, "executions_2 = ++executions; #print ('  Event 2 executed\n')", 0.50, 0.0, 1);
  executor.add (new_event);
  
  utility::sleep (3);
  
  if (knowledge.get ("executions_1").to_integer () == 1 &&
      knowledge.get ("executions_2").to_integer () == 2 &&
      knowledge.get ("executions_3").to_integer () == 3 &&
      knowledge.get ("executions_4").to_integer () == 4)
  {
    knowledge.print ("SUCCESS. The four events executed in the correct order.\n");
  }
  else
  {
    knowledge.print ("FAIL. The events executed in the incorrect order.\n");
  }

  executor.shutdown ();
  knowledge.clear ();
  executor.launch_threads (3);
  
  knowledge.print (
    "\nAdding eight separate, ordered events.\n");
  new_event = engine::fill_event (
    knowledge, "executions_4 = ++executions; #print ('  Event 4 executed\n')", 0.4, 0.0, 1);
  executor.add (new_event);
  new_event = engine::fill_event (
    knowledge, "executions_1 = ++executions; #print ('  Event 1 executed\n')", 0.1, 0.0, 1);
  executor.add (new_event);
  new_event = engine::fill_event (
    knowledge, "executions_3 = ++executions; #print ('  Event 3 executed\n')", 0.3, 0.0, 1);
  executor.add (new_event);
  new_event = engine::fill_event (
    knowledge, "executions_2 = ++executions; #print ('  Event 2 executed\n')", 0.2, 0.0, 1);
  executor.add (new_event);
  new_event = engine::fill_event (
    knowledge, "executions_8 = ++executions; #print ('  Event 8 executed\n')", 0.8, 0.0, 1);
  executor.add (new_event);
  new_event = engine::fill_event (
    knowledge, "executions_5 = ++executions; #print ('  Event 5 executed\n')", 0.5, 0.0, 1);
  executor.add (new_event);
  new_event = engine::fill_event (
    knowledge, "executions_7 = ++executions; #print ('  Event 7 executed\n')", 0.7, 0.0, 1);
  executor.add (new_event);
  new_event = engine::fill_event (
    knowledge, "executions_6 = ++executions; #print ('  Event 6 executed\n')", 0.6, 0.0, 1);
  executor.add (new_event);
  
  utility::sleep (3);

  if (knowledge.get ("executions_1").to_integer () == 1 &&
      knowledge.get ("executions_2").to_integer () == 2 &&
      knowledge.get ("executions_3").to_integer () == 3 &&
      knowledge.get ("executions_4").to_integer () == 4 &&
      knowledge.get ("executions_5").to_integer () == 5 &&
      knowledge.get ("executions_6").to_integer () == 6 &&
      knowledge.get ("executions_7").to_integer () == 7 &&
      knowledge.get ("executions_8").to_integer () == 8)
  {
    knowledge.print ("SUCCESS. The eight events executed in the correct order.\n");
  }
  else
  {
    knowledge.print ("FAIL. The events executed in the incorrect order.\n");
  }

  executor.shutdown ();
  utility::sleep (1);
}

int main (int argc, char ** argv)
{
  handle_arguments (argc, argv);

  test_timed_executor ();

  return 0;
}
