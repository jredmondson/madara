
/**
 * @file packet_aggregation.cpp
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * One of the features that sets MADARA apart from other key-value stores
 * and NoSQL databases is packet aggregation. MADARA KaRL logics are evaluated
 * atomically and the modified variables are grouped together in a packet that
 * is clock-stamped and sent to other agents in the network. In this tutorial,
 * we showcase what packet aggregation is with a simple tracking application.
 *
 * This tutorial is an extension of the counting_agents tutorial but adds
 * location information and vector
 * 
 * Windows:
 *   cd %MADARA_ROOT%\bin
 *   tutorial_packet_aggregation 0 (in one terminal)
 *   tutorial_packet_aggregation 1 (in a second terminal)
 *   tutorial_packet_aggregation 2 (in a third terminal)
 *   etc.
 *
 * Linux:
 *   cd $MADARA_ROOT/bin
 *   tutorial_packet_aggregation 0 (in one terminal)
 *   tutorial_packet_aggregation 1 (in a second terminal)
 *   tutorial_packet_aggregation 2 (in a third terminal)
 * 
 **/

#include <iostream>
#include <string>
#include <sstream>
#include <time.h>
#include "madara/knowledge_engine/Knowledge_Base.h"
#include "ace/Signal.h"
#include "ace/OS_NS_Thread.h"

/**
 * Default settings for our Network Transport. We put them
 * outside of the main function to allow for future expansion
 * with an argument parsing function.
 **/
std::string host ("");
const std::string multicast_address ("239.255.0.0:4150");
Madara::Transport::Settings settings;

/**
 * To terminate an agent, the user needs to press Control+C. The following
 * function is the signal handler, which we will pass to the OS via ACE
 **/
volatile bool terminated = 0;
extern "C" void terminate (int)
{
  terminated = true;
}

/**
 * Madara function that determines the zone that an agent is in
 **/
Madara::Knowledge_Record
  move (Madara::Knowledge_Engine::Function_Arguments & args,
             Madara::Knowledge_Engine::Variables & variables)
{
  /**
   * Zone 1 = (0,39) -> (39,39)
   * Zone 2 = (40,0) -> (79,39)
   * Zone 3 = (0,40) -> (39,79)
   * Zone 4 = (40,40) -> (79,79)
   **/
  if (args.size () == 1)
  {
    /**
     * Expand the contents of argument 0 into the special variable .0, which
     * is meant to hold the first argument passed to a function
     **/
    variables.set (".0", variables.expand_statement (args[0].to_string ()));
  }
  else
    // Default first argument is the expansion of agent{.id}
    variables.set (".0", variables.expand_statement ("agent{.id}"));

  // 
  Madara::Knowledge_Engine::Compiled_Expression expression = 
    variables.compile (

    // Update the coordinates with the delta
    "{.0}.x += {.0}.delta_x;"
    "{.0}.y += {.0}.delta_y;"

    /**
     * if we've escaped the eastern boundary, stop horizontal movement
     * at the eastern boundary
     **/
     "{.0}.x > 79 => ({.0}.x = 79; {.0}.delta_x = 0);"
    
     /**
     * if we've escaped the western boundary, stop horizontal movement
     * at the western boundary
     **/
    "{.0}.x < 0 => ({.0}.x = 0; {.0}.delta_x = 0);"
    
     /**
     * if we've escaped the northern boundary, stop vertical movement
     * at the northern boundary
     **/
    "{.0}.y < 0 => ({.0}.y = 0; {.0}.delta_y = 0);"
    
     /**
     * if we've escaped the northern boundary, stop vertical movement
     * at the northern boundary
     **/
     "{.0}.y > 79 => ({.0}.y = 79; {.0}.delta_y = 0);"

     /**
      * If we're completely stopped, change deltas
      **/
      "{.0}.delta_y == 0 && {.0}.delta_x == 0 => change_deltas ()"
  );

  variables.evaluate (expression);

  return Madara::Knowledge_Record::Integer (1);
}

/**
 * Madara function that determines the zone that an agent is in
 **/
Madara::Knowledge_Record
  get_zone (Madara::Knowledge_Engine::Function_Arguments & args,
             Madara::Knowledge_Engine::Variables & variables)
{
  /**
   * Zone 1 = (0,39) -> (39,39)
   * Zone 2 = (40,0) -> (79,39)
   * Zone 3 = (0,40) -> (39,79)
   * Zone 4 = (40,40) -> (79,79)
   **/
  if (args.size () == 1)
  {
    /**
     * Expand the contents of argument 0 into the special variable .0, which
     * is meant to hold the first argument passed to a function
     **/
    variables.set (".0", variables.expand_statement (args[0].to_string ()));
  }
  else
    // Default first argument is the expansion of agent{.id}
    variables.set (".0", variables.expand_statement ("agent{.id}"));

  // 
  Madara::Knowledge_Engine::Compiled_Expression expression = 
    variables.compile (
    "("
        // if x <= 39, then we're either in zone 1 or zone 3
        "{.0}.x <= 39 =>"
          "("
              // if y <= 39, then we're in zone 1
              "({.0}.y <= 39 => {.0}.zone = 'Zone 1')"
              "||"
              // otherwise, we're in zone 3
              "({.0}.zone = 'Zone 3')"
          ")"
    ")"
    "||"
    "("
        // if x > 39, then it's either in zone 2 or zone 4
        "{.0}.y <= 39 => {.0}.zone = 'Zone 2'"
    ")"
    "||"
    "("
        // if x > 39, then it's either in zone 2 or zone 4
        "{.0}.zone = 'Zone 4'"
    ")"
  );

  variables.evaluate (expression);

  return variables.get ("{.0}.zone");
}

/**
 * Madara function that randomly changes agent direction
 **/
Madara::Knowledge_Record
  change_deltas (Madara::Knowledge_Engine::Function_Arguments & args,
             Madara::Knowledge_Engine::Variables & variables)
{
  if (args.size () == 1)
  {
    /**
     * Save the first argument to .change_deltas_id, a temporary variable.
     * Note that we could set this in a KaRL evaluation as well by saying
     * .change_deltas_id = .0, but we'd have to do this before calling
     * any user functions (such as rand (5) below). Remember that .0 is
     * overwritten on every user function call that has an argument.
     **/
    variables.set (".change_deltas_id",
      variables.expand_statement (args[0].to_string ()));
  }
  else
    // Default first argument is the expansion of agent{.id}
    variables.set (".change_deltas_id",
    variables.expand_statement ("agent{.id}"));

  // 
  Madara::Knowledge_Engine::Compiled_Expression expression = 
    variables.compile (

    // set random vector information (up to 5 x or y coordinates per second)
    "{.change_deltas_id}.delta_y = rand (5);"
    "{.change_deltas_id}.delta_x = rand (5);"
    
    // allow for negative vectors of movement
    "rand (2) => {.change_deltas_id}.delta_x = -{.change_deltas_id}.delta_x;"
    "rand (2) => {.change_deltas_id}.delta_y = -{.change_deltas_id}.delta_y;"
  );

  variables.evaluate (expression);

  return Madara::Knowledge_Record::Integer (1);
}

/**
 * Madara function that returns a random number. No argument means to generate a
 * random number up to the limits of the stdlib.h implementation's limits. An
 * integer argument means to generate a random up to the specified arg limit.
 **/
Madara::Knowledge_Record
  rand_int (Madara::Knowledge_Engine::Function_Arguments & args,
             Madara::Knowledge_Engine::Variables & variables)
{
  // if the args list is greater than zero, is an integer, and is not 0
  if (args.size () > 0 && 
    args[0].type () == Madara::Knowledge_Record::INTEGER &&
    args[0].to_integer () != 0)
    // then call rand () with a modulus up to that integer argument
    return Madara::Knowledge_Record::Integer (rand () % (int)args[0].to_integer ());
  // otherwise, just return rand ()
  else
    return Madara::Knowledge_Record::Integer (rand ());
}


int main (int argc, char * argv[])
{
  // Register a signal handler for Control+C
  ACE_Sig_Action sa ((ACE_SignalHandler) terminate, SIGINT);

  // Setup a multicast transport with the settings mentioned above.
  settings.type = Madara::Transport::MULTICAST;
  settings.hosts.resize (1);
  settings.hosts[0] = multicast_address;

  // Create the knowledge base with the transport settings set for multicast
  Madara::Knowledge_Engine::Knowledge_Base knowledge (host, settings);
  
  // Check command line arguments for a non-zero id
  if (argc == 2)
  {
    // save the first argument into an integer
    Madara::Knowledge_Record::Integer new_id;
    std::stringstream buffer (argv[1]);
    buffer >> new_id;

    /**
     * Update the knowledge base to include our .id. All variables are zero
     * by default in the knowledge base.
     **/
    knowledge.set (".id", new_id);
  }

  /**
   * Each agent in this tutorial is moving across a grid with four zones
   *
   * Grid (80x80)
   * ==========================
   * ||  Zone 1  ||  Zone 2  ||
   * ==========================
   * ||  Zone 3  ||  Zone 4  ||
   * ==========================
   *
   * Zone 1 = (0,39) -> (39,39)
   * Zone 2 = (40,0) -> (79,39)
   * Zone 3 = (0,40) -> (39,79)
   * Zone 4 = (40,40) -> (79,79)
   *
   * Because we will likely call this logic several times in different places,
   * we define a function called get_zone above and define it to MADARA
   * function call called "get_zone" in the code below. We also define a
   * function called "rand" that maps to the rand_int function above. This
   * function will give us access to a random integer generator for location
   * information.
   **/

  knowledge.define_function ("rand", rand_int);
  knowledge.define_function ("get_zone", get_zone);
  knowledge.define_function ("move", move);
  knowledge.define_function ("change_deltas", change_deltas);
  
  // set the random seed to be the number of seconds since January 1st, 1970
  srand ((unsigned int)time (NULL));

  // do a couple of rands to get the seed away from standard seconds
  rand ();
  rand ();

  /**
   * Initialize the agent location, online status and zone information. All of
   * this information is aggregated into a single packet, atomically, so other
   * agents cannot update this agent's information and cause an invalid state
   * to be recognized within the agent reasoning network
   **/
  knowledge.evaluate (
    "agent{.id}.online = 1;"

    // set random x,y coordinates within the grid
    "agent{.id}.x = rand (80);"
    "agent{.id}.y = rand (80);"

    // set random vector information (up to 5 x or y coordinates per second)
    "change_deltas ('agent{.id}');"

    "get_zone ('agent{.id}');"
  );

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
         "#print ("
"'Agent {.id} of {.cur_agents} online ({.max_agents} max online):\n' +"
"'  Position: {agent{.id}.x},{agent{.id}.y}\n' +"
"'  Zone: {agent{.id}.zone}\n' +"
"'  Vector: {agent{.id}.delta_x},{agent{.id}.delta_y}\n');"
    );

    // Sleep for a second so we limit the number of printouts and movements
    ACE_OS::sleep (1);

    // Move our position around the grid according to our x/y deltas
    knowledge.evaluate (
      "move ();"
      "get_zone ('agent{.id}');"
    );
  }
  
 
  /**
   * Let everyone know that we are going offline and also do one last
   * agent count and print the results. We'll showcase a second way to
   * print information in an evaluation here. Each evaluate call can
   * be passed settings that dictate how the evaluation takes place.
   * In this case, we set the Eval_Settings class to include a post
   * print statement that prints the cur and max agents
   **/
  Madara::Knowledge_Engine::Eval_Settings eval_settings;
  eval_settings.post_print_statement = 
"Agent {.id} of {.cur_agents} online ({.max_agents} max online):\n"
"  Position: {agent{.id}.x},{agent{.id}.y}\n"
"  Zone: {agent{.id}.zone}\n"
"  Vector: {agent{.id}.delta_x},{agent{.id}.delta_y}\n";

  /**
   * Evaluate the number of agents one last time before we return to the OS
   **/
  knowledge.evaluate (
"agent{.id}.online = 0; \
.cur_agents = 0; \
.i [0->10) (.cur_agents += agent{.i}.online); \
.max_agents = (.max_agents ; .cur_agents)"
  , eval_settings);
  
  knowledge.print ();
  return 0;
}
