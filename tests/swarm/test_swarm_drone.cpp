
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <assert.h>
#include <stdlib.h>

#include "madara/knowledge_engine/Knowledge_Base.h"
#include "madara/utility/Log_Macros.h"
#include "ace/Signal.h"

std::string host ("");
const std::string default_multicast ("239.255.0.1:4150");
Madara::Transport::Settings settings;
volatile bool terminated = 0;

// signal handler for someone hitting control+c
extern "C" void terminate (int)
{
  terminated = true;
}


void handle_arguments (int argc, char ** argv)
{
  for (int i = 1; i < argc; ++i)
  {
    std::string arg1 (argv[i]);

    if (arg1 == "-m" || arg1 == "--multicast")
    {
      if (i + 1 < argc)
        settings.hosts[0] = argv[i + 1];

      ++i;
    }
    else if (arg1 == "-o" || arg1 == "--host")
    {
      if (i + 1 < argc)
        host = argv[i + 1];

      ++i;
    }
    else if (arg1 == "-d" || arg1 == "--domain")
    {
      if (i + 1 < argc)
        settings.domains = argv[i + 1];

      ++i;
    }
    else if (arg1 == "-i" || arg1 == "--id")
    {
      if (i + 1 < argc)
      {
        std::stringstream buffer (argv[i + 1]);
        buffer >> settings.id;
      }

      ++i;
    }
    else if (arg1 == "-l" || arg1 == "--level")
    {
      if (i + 1 < argc)
      {
        std::stringstream buffer (argv[i + 1]);
        buffer >> MADARA_debug_level;
      }

      ++i;
    }
    else
    {
      MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_DEBUG, 
        "\nProgram summary for %s:\n\n" \
        "  Run a drone client that responds to a terminal. If running more\n" \
        "  than one drone, make sure to set their ids to different values.\n\n" \
        " [-o|--host hostname]     the hostname of this process (def:localhost)\n" \
        " [-m|--multicast ip:port] the multicast ip to send and listen to\n" \
        " [-d|--domain domain]     the knowledge domain to send and listen to\n" \
        " [-i|--id id]             the id of this agent (should be non-negative)\n" \
        " [-l|--level level]       the logger level (0+, higher is higher detail)\n" \
        "\n",
        argv[0]));
      exit (0);
    }
  }
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
  if (args.size () > 0 && args[0].type () == Madara::Knowledge_Record::INTEGER &&
                          args[0].to_integer () != 0)
    return Madara::Knowledge_Record::Integer (rand () % args[0].to_integer ());
  else
    return Madara::Knowledge_Record::Integer (rand ());
}

/**
 * Madara function to move the drone north
 **/
Madara::Knowledge_Record
  move_north (Madara::Knowledge_Engine::Function_Arguments & args,
             Madara::Knowledge_Engine::Variables & variables)
{
  variables.set ("drone{.id}.pos.y",
    variables.get ("drone{.id}.pos.y").to_integer () + 1);

  return Madara::Knowledge_Record::Integer (1);
}

/**
 * Madara function to move the drone south
 **/
Madara::Knowledge_Record
  move_south (Madara::Knowledge_Engine::Function_Arguments & args,
             Madara::Knowledge_Engine::Variables & variables)
{
  variables.set ("drone{.id}.pos.y",
    variables.get ("drone{.id}.pos.y").to_integer () - 1);

  return Madara::Knowledge_Record::Integer (1);
}

/**
 * Madara function to move the drone east
 **/
Madara::Knowledge_Record
  move_east (Madara::Knowledge_Engine::Function_Arguments & args,
             Madara::Knowledge_Engine::Variables & variables)
{
  variables.set ("drone{.id}.pos.x",
    variables.get ("drone{.id}.pos.x").to_integer () + 1);

  return Madara::Knowledge_Record::Integer (1);
}

/**
 * Madara function to move the drone west
 **/
Madara::Knowledge_Record
  move_west (Madara::Knowledge_Engine::Function_Arguments & args,
             Madara::Knowledge_Engine::Variables & variables)
{
  variables.set ("drone{.id}.pos.x",
    variables.get ("drone{.id}.pos.x").to_integer () - 1);

  return Madara::Knowledge_Record::Integer (1);
}

/**
 * Madara function to move the drone to the origin (phone.pos.x,phone.pos.y)
 **/
Madara::Knowledge_Record
  move_to_origin (Madara::Knowledge_Engine::Function_Arguments & args,
             Madara::Knowledge_Engine::Variables & variables)
{
  variables.set ("drone{.id}.pos.y",
    variables.get ("phone.pos.y").to_integer ());
  variables.set ("drone{.id}.pos.x",
    variables.get ("phone.pos.x").to_integer ());

  return Madara::Knowledge_Record::Integer (1);
}

/**
 * Madara function to use the thermal sensor
 **/
Madara::Knowledge_Record
  sense_thermal (Madara::Knowledge_Engine::Function_Arguments & args,
             Madara::Knowledge_Engine::Variables & variables)
{
  return Madara::Knowledge_Record::Integer (1);
}

/**
 * Madara function to use the thermal sensor
 **/
Madara::Knowledge_Record
  shutdown (Madara::Knowledge_Engine::Function_Arguments & args,
             Madara::Knowledge_Engine::Variables & variables)
{
  terminated = true;
  return Madara::Knowledge_Record::Integer (1);
}

/**
 * Madara function to use the thermal sensor
 **/
Madara::Knowledge_Record
  takeoff (Madara::Knowledge_Engine::Function_Arguments & args,
             Madara::Knowledge_Engine::Variables & variables)
{
  return Madara::Knowledge_Record::Integer (1);
}


/**
 * Madara function to use the thermal sensor
 **/
Madara::Knowledge_Record
  land (Madara::Knowledge_Engine::Function_Arguments & args,
             Madara::Knowledge_Engine::Variables & variables)
{
  return Madara::Knowledge_Record::Integer (1);
}



int main (int argc, char ** argv)
{
  // signal handler for clean exit
  ACE_Sig_Action sa ((ACE_SignalHandler) terminate, SIGINT);

  settings.hosts.resize (1);
  settings.hosts[0] = default_multicast;
  handle_arguments (argc, argv);
  
#ifndef _MADARA_NO_KARL_
  Madara::Knowledge_Engine::Knowledge_Base knowledge (host, settings);

  // define control functions
  knowledge.define_function ("shutdown", shutdown);
  knowledge.define_function ("takeoff", takeoff);
  knowledge.define_function ("land", land);
  
  // define movement functions
  knowledge.define_function ("move_east", move_east);
  knowledge.define_function ("move_west", move_west);
  knowledge.define_function ("move_north", move_north);
  knowledge.define_function ("move_south", move_south);
  knowledge.define_function ("move_to_origin", move_to_origin);
  
  // define sensor functions
  knowledge.define_function ("sense_thermal", sense_thermal);


  knowledge.set (".id", (Madara::Knowledge_Record::Integer) settings.id);
  
  srand ((unsigned int)settings.id);

  // set our position to a random x,y
  knowledge.evaluate (
    "drone{.id}.pos.x = rand (8);"
    "drone{.id}.pos.y = rand (8);"
    "drone{.id}.operational = 1"
    );
  
  Madara::Knowledge_Engine::Eval_Settings eval_settings;
  eval_settings.pre_print_statement = knowledge.expand_statement (
    "Processing movement instructions:\n"
    "  move_east:  {move_east}  drone{.id}.move_east:  {drone{.id}.move_east}\n"
    "  move_west:  {move_west}  drone{.id}.move_west:  {drone{.id}.move_west}\n"
    "  move_north: {move_north} drone{.id}.move_north: {drone{.id}.move_north}\n"
    "  move_south: {move_south} drone{.id}.move_south: {drone{.id}.move_south}\n"
    "  move_to_origin: {move_to_origin} drone{.id}.move_to_origin: {drone{.id}.move_to_origin}\n"
    );
  
  eval_settings.post_print_statement = knowledge.expand_statement (
    "Current drone{.id} position: {drone{.id}.pos.x},{drone{.id}.pos.y}\n"
    );

  std::string movement_logic_text = knowledge.expand_statement (
    "("
      // detect whether or not the user wants us to move
      "("
         "move_east || drone{.id}.move_east =>"
           "(move_east (); move_east = drone{.id}.move_east = 0)"
         ";"
         "move_west || drone{.id}.move_west =>"
           "(move_west (); move_west = drone{.id}.move_west = 0)"
         ";"
         "move_north || drone{.id}.move_north =>"
           "(move_north (); move_north = drone{.id}.move_north = 0)"
         ";"
         "move_south || drone{.id}.move_south"
           "(move_south (); move_south = drone{.id}.move_south = 0)"
         ";"
      ")"
      // or check if the user wants us to return to the origin
      "||"
      "(move_to_origin || drone{.id}.move_to_origin => move_to_origin ())"
    ")"
    ";"
    "sense_thermal ()"
    ";"
    "shutdown || drone{.id}.shutdown => shutdown ()"
    ";"
    "land || drone{.id}.land => land ()"
    ";"
    "takeoff || drone{.id}.takeoff => takeoff ()"
  );

  Madara::Knowledge_Engine::Compiled_Expression movement_logic =
    knowledge.compile (movement_logic_text);

  // until the user presses ctrl+c in this terminal, check for input
  while (!terminated)
  {
    knowledge.evaluate (movement_logic, eval_settings);
  }

  knowledge.print ();
  
#else
  std::cout << "This test is disabled due to karl feature being disabled.\n";
#endif
  return 0;
}