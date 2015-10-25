
/**
 * @file counting_agents.cpp
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains a simple distributed application that counts the
 * number of agents that come online. To test the application, launch
 * a number of terminal windows (e.g., on Windows, go to Start->Run
 * and type "cmd" to bring up a terminal). Do this a number of times
 * until you have up to 10 terminals open. Now, in each terminal
 * invoke the compiled terminal by changing directory into the bin
 * directory and starting tutorial_counting_agents with an id.
 * 
 * Windows:
 *   cd %MADARA_ROOT%\bin
 *   tutorial_counting_agents 0 (in one terminal)
 *   tutorial_counting_agents 1 (in a second terminal)
 *   tutorial_counting_agents 2 (in a third terminal)
 *   etc.
 *
 * Linux:
 *   cd $MADARA_ROOT/bin
 *   tutorial_counting_agents 0 (in one terminal)
 *   tutorial_counting_agents 1 (in a second terminal)
 *   tutorial_counting_agents 2 (in a third terminal)
 *
 * If you have done this correctly, you should see the number of current
 * agents and max agents change in your terminal window.
 * 
 * This tutorial also allows for setting a domain. A domain is a partition
 * of the network that allows for privacy amongst a clique of agents. To
 * set the domain for one of the agents, simply pass in a 2nd argument
 * that differentiates the partition. For instance,
 *
 * tutorial_counting_agents 0
 * tutorial_counting_agents 0 secret
 *
 * Creates 2 agents, both with id 0, but communicating separately. The
 * first is communicating in the default domain. The second is communicating
 * in a separate domain called "secret". Neither of these will receive
 * updates from the other agents.
 **/

#include <iostream>
#include <string>
#include <sstream>
#include "madara/knowledge/KnowledgeBase.h"
#include "ace/Signal.h"
#include "ace/OS_NS_Thread.h"

/**
 * Default settings for our Network Transport. We put them
 * outside of the main function to allow for future expansion
 * with an argument parsing function.
 **/
std::string host ("");
const std::string multicast_address ("239.255.0.1:4150");
madara::transport::TransportSettings settings;

/**
 * To terminate an agent, the user needs to press Control+C. The following
 * function is the signal handler, which we will pass to the OS via ACE
 **/
volatile bool terminated = 0;
extern "C" void terminate (int)
{
  terminated = true;
}

int main (int argc, char * argv[])
{
  // Register a signal handler for Control+C
  ACE_Sig_Action sa ((ACE_SignalHandler) terminate, SIGINT);

  // Setup a multicast transport with the settings mentioned above.
  settings.type = madara::transport::MULTICAST;
  settings.hosts.resize (1);
  settings.hosts[0] = multicast_address;
  
  // Check for command line argument that changes the domain
  if (argc == 3)
  {
    // change the domain from its default to the one provided by the user
    settings.domains = argv[2];
  }

  // Create the knowledge base with the transport settings set for multicast
  madara::knowledge::KnowledgeBase knowledge (host, settings);
  
  // Check command line arguments for a non-zero id
  if (argc >= 2)
  {
    // save the first argument into an integer
    madara::knowledge::KnowledgeRecord::Integer new_id;
    std::stringstream buffer (argv[1]);
    buffer >> new_id;

    /**
     * Update the knowledge base to include our .id. All variables are zero
     * by default in the knowledge base.
     **/
    knowledge.set (".id", new_id);
  }
  
  
  while (!terminated)
  {
    knowledge.evaluate (
      /**
       * In KaRL, a variable without a period in front of it is meant to be
       * disseminated to every other reasoning agent in the network who is
       * in our domain. The first thing we do in this logic is to tell other
       * processes that we are online.
       **/
      "agent{.id}.online = 1;"

      /**
       * We next set a variable called .cur_agents to 0. We'll use this to
       * count the number of agents online right now.
       **/
      ".cur_agents = 0;"

      /**
       * The second step in this logic is to count the number of online agents.
       * The following code is a for loop that iterates a variable called .i
       * from 0 to 10 (exclusively, meaning we really only go from 0 to 9).
       * In this loop, we count the number of online agents.
       **/
       ".i [0->10) (.cur_agents += agent{.i}.online);"

       /**
        * Another interesting metric to keep track of is the maximum number of
        * agents that were ever online. We do that by exploiting the power of
        * the semicolon operator, which separates expressions and also returns
        * the maximum of the two expressions.
        **/
        ".max_agents = (.max_agents ; .cur_agents);"

        /**
         * Last, let's print out both the current agents and the max agents.
         * Note that we've cut this single print statement into two lines to
         * keep us within 80 column boundaries. We're also showcasing string
         * concatenation within the engine.
         **/
         "#print ('Cur agents online: {.cur_agents}  ' +"
         "        'Max agents online {.max_agents}\n')"
    );

    // Sleep for a second so we limit the number of printouts
    ACE_OS::sleep (1);
  }
  
 
  /**
   * Let everyone know that we are going offline and also do one last
   * agent count and print the results. We'll showcase a second way to
   * print information in an evaluation here. Each evaluate call can
   * be passed settings that dictate how the evaluation takes place.
   * In this case, we set the EvalSettings class to include a post
   * print statement that prints the cur and max agents
   **/
  madara::knowledge::EvalSettings eval_settings;
  eval_settings.post_print_statement = 
    "Cur agents online: {.cur_agents}  Max agents online: {.max_agents}\n";

  /**
   * Evaluate the number of agents one last time before we return to the OS
   **/
  knowledge.evaluate (
"agent{.id}.online = 0; \
.cur_agents = 0; \
.i [0->10) (.cur_agents += agent{.i}.online); \
.max_agents = (.max_agents ; .cur_agents)"
  , eval_settings);

  return 0;
}
