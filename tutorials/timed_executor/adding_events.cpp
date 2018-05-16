
/**
 * @file adding_events.cpp
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains a simple application that demonstrates
 * adding events to a TimedExecutor object
 **/

#include "madara/knowledge/KnowledgeBase.h"
#include "madara/knowledge/TimedExecutor.h"
#include "madara/utility/Utility.h"
#include <iostream>
#include <string>

/**
 * Create a short "engine" alias for referring to Madara::KnowledgeEngine
 **/
namespace knowledge = madara::knowledge;

int main (int, char **)
{
  // Create a knowledge base
  knowledge::KnowledgeBase knowledge;
  
  // let the user know where we are at in the tutorial
  knowledge.print ("\nCreating executor and events.\n");

  // A timed executor is an object that can create threads and execute events
  knowledge::TimedExecutor executor;

  /**
   * The executor takes events as input and executes the contents, which we
   * can fill using the fill_event helper function
   **/
  knowledge::Event new_event;
  
  // let the user know where we are at in the tutorial
  knowledge.print ("Filling the first event.\n");

  /**
   * Fill the event structure with execution details
   **/
  new_event = knowledge::fill_event (
    // attached the knowledge base
    knowledge,

    /**
     * define the logic to execute within the knowledge base. Here, we just
     * update a local counter called executions and print a message saying
     * that we've executed this event.\n"
     **/
     "++.executions ; "
     "#print ('  Executing event {.executions} (Type A)\n')",

    // specify a delay from now before executing the logic (2 seconds)
    2.0,

    /**
     * specify a period between executions (useful if specifying number of
     * executions). Here we specify a 1s delay between executions of the event
     **/
    1.0,

    // specify a number of executions
    20
  );

  // add the event to the timed executor
  executor.add (new_event);
  
  // let the user know where we are at in the tutorial
  knowledge.print ("Launching two threads.\n");

  /**
   * events cannot be executed until you launch threads
   * This action can be done before or after adding events. However,
   * launching threads after adding events could risk missing a target
   * delay time for the event. If you have delayed events that must execute
   * within milliseconds or microseconds of a time epoch, then it's best
   * to launch threads before adding events. Our delay is 2 seconds, so
   * we should be fine.
   **/
  executor.launch_threads (2);

  
  // let the user know where we are at in the tutorial
  knowledge.print ("Filling the second event.\n");

  /**
   * Let's add another event that executes every 2s and adds 3 to the counter
   **/
  new_event = knowledge::fill_event (
    // attached the knowledge base
    knowledge,

    /**
     * define the logic to increment executions by a total of 3 and also
     * to check the number of executions we have and cancel the thread if
     * executions ever gets bigger than 15.
     **/
     "++.executions ;> "
     "#print ('  Executing event {.executions} (Type B)\n') ;> "
     ".executions += 2 ;> "

     /**
      * because of the usage of the choose right operator (;), this is the
      * condition that can be checked after executing the event to determine
      * if the event will be readded to the event queue. Here, the implication
      * returns true if executions is less than or equal to 15. Otherwise,
      * the logic will return false.
      **/
     ".executions <= 15 || #print ('  Canceling Type B event\n')"
     ,

    // specify a delay from now before executing the logic (2 seconds)
    2.0,

    /**
     * specify a period between executions (useful if specifying number of
     * executions). Here we specify a 1s delay between executions of the event
     **/
    1.5,

    // specify a number of executions (-1 means execute until shutdown)
    -1
  );

  // add the event to the timed executor
  executor.add (new_event);
  
  // let the user know where we are at in the tutorial
  knowledge.print ("Waiting for events to increase local counter to 25.\n\n");

  // wait until we have seen enough events for executions to be 25 or more
  knowledge.wait (".executions >= 25");
  
  // let the user know where we are at in the tutorial
  knowledge.print ("\nShutting down threads.\n");

  // shutdown the threads and stop event execution
  executor.shutdown ();
  executor.clear_queue ();

  /**
   * The timed executor object provides an interface for evaluating KaRL logic
   * at set intervals. This powerful feature can be used in a variety of ways
   * and supports all of the features of the regular engine, including function
   * calls and reading or writing files, at intervals specified by the user.
   **/

  return 0;
}
