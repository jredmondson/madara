
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <assert.h>
#include <stdlib.h>

#include "madara/knowledge_engine/Knowledge_Base.h"
#include "madara/utility/Log_Macros.h"
#include "ace/Signal.h"
#include "ace/High_Res_Timer.h"
#include "ace/OS_NS_Thread.h"

// Ideally set with compiler flags according to drone platform
//#define  USING_PARROT_ARDRONE_2


/**
 * Note that this is not an ordering of tasks but a list of
 * Compiled_Expressions
 **/
#define  NUM_TASKS                     19
#define  GET_CURRENT_POSITION          0
#define  CHECK_CONTROLLER_HEARTBEAT    1
#define  COMPUTE_MOBILE_DRONES         2
#define  COMPUTE_BOUNDING_BOX          3
#define  MOVE_TO_ALTITUDE              4
#define  MOVE_TO_POSITION              5
#define  COMPUTE_VIRTUAL_ID            6
#define  USER_MOVEMENT_DEMANDS         7
#define  COMPUTE_NEW_POSITION_GOAL     8
#define  LAND                          9
#define  TAKEOFF                      10
#define  SHUTDOWN                     11
#define  MAIN_LOGIC                   12
#define  MOVE_NORTH                   13
#define  MOVE_SOUTH                   14
#define  MOVE_EAST                    15
#define  MOVE_WEST                    16
#define  MOVE_TO_ORIGIN               17
#define  UPDATE_STATUS                18

/**
 * Timer defines for keeping high precision clocks of heartbeats
 **/
#define  NUM_TIMERS                    2
#define  CONTROLLER_HEARTBEAT          0

// By default we identify ourselves by the hostname set in our OS
std::string host ("");

// By default, we use the multicast port 239.255.0.1.:4150
const std::string default_multicast ("239.255.0.1:4150");

// Used for updating various transport settings
Madara::Transport::Settings settings;

// Used for shutting down the process (Control+C modifies)
volatile bool terminated = 0;

// Compiled expressions that we expect to be called frequently
Madara::Knowledge_Engine::Compiled_Expression expressions [NUM_TASKS];

// Timers for heartbeats
ACE_High_Res_Timer timers[NUM_TIMERS];

// signal handler for someone hitting control+c
extern "C" void terminate (int)
{
  terminated = true;
}

// Parses command line arguments
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
    return Madara::Knowledge_Record::Integer (rand () % (int)args[0].to_integer ());
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
  return variables.evaluate (expressions[MOVE_NORTH]);
}

/**
 * Madara function to move the drone south
 **/
Madara::Knowledge_Record
  move_south (Madara::Knowledge_Engine::Function_Arguments & args,
             Madara::Knowledge_Engine::Variables & variables)
{
  return variables.evaluate (expressions[MOVE_SOUTH]);
}

/**
 * Madara function to move the drone east
 **/
Madara::Knowledge_Record
  move_east (Madara::Knowledge_Engine::Function_Arguments & args,
             Madara::Knowledge_Engine::Variables & variables)
{
  return variables.evaluate (expressions[MOVE_EAST]);
}

/**
 * Madara function to move the drone west
 **/
Madara::Knowledge_Record
  move_west (Madara::Knowledge_Engine::Function_Arguments & args,
             Madara::Knowledge_Engine::Variables & variables)
{
  return variables.evaluate (expressions[MOVE_WEST]);
}

/**
 * Madara function to move the drone to the origin (phone.pos.x,phone.pos.y)
 **/
Madara::Knowledge_Record
  move_to_origin (Madara::Knowledge_Engine::Function_Arguments & args,
             Madara::Knowledge_Engine::Variables & variables)
{
  return variables.evaluate (expressions[MOVE_TO_ORIGIN]);
}

/**
 * Madara function to use the thermal sensor
 **/
Madara::Knowledge_Record
  sense_thermal (Madara::Knowledge_Engine::Function_Arguments & args,
             Madara::Knowledge_Engine::Variables & variables)
{
#ifdef  USING_PARROT_ARDRONE_2
  // insert code to use thermal sensing from Drone-RK
#endif
  return Madara::Knowledge_Record::Integer (1);
}

/**
 * Madara function to shutdown the agent
 **/
Madara::Knowledge_Record
  shutdown (Madara::Knowledge_Engine::Function_Arguments & args,
             Madara::Knowledge_Engine::Variables & variables)
{
  terminated = true;

#ifdef  USING_PARROT_ARDRONE_2
  // insert code to land a drone
#endif

  return variables.evaluate (expressions[SHUTDOWN]);
}

/**
 * Madara function to do absolutely nothing
 **/
Madara::Knowledge_Record
  no_op (Madara::Knowledge_Engine::Function_Arguments & args,
             Madara::Knowledge_Engine::Variables & variables)
{
  return Madara::Knowledge_Record::Integer (0);
}

/**
 * Madara function to move to a altitude
 **/
Madara::Knowledge_Record
  move_to_altitude (Madara::Knowledge_Engine::Function_Arguments & args,
             Madara::Knowledge_Engine::Variables & variables)
{
#ifdef  USING_PARROT_ARDRONE_2
  // insert code to move to an altitude
#endif
  return variables.evaluate (expressions[MOVE_TO_ALTITUDE]);
}

/**
 * Madara function to move to a position
 **/
Madara::Knowledge_Record
  move_to_position (Madara::Knowledge_Engine::Function_Arguments & args,
             Madara::Knowledge_Engine::Variables & variables)
{
#ifdef  USING_PARROT_ARDRONE_2
  // insert code to move to a position
#endif
  return variables.evaluate (expressions[MOVE_TO_POSITION]);
}

/**
 * Madara function to take off the ground
 **/
Madara::Knowledge_Record
  takeoff (Madara::Knowledge_Engine::Function_Arguments & args,
             Madara::Knowledge_Engine::Variables & variables)
{
#ifdef  USING_PARROT_ARDRONE_2
  // insert code to land a drone
#endif
  return variables.evaluate (expressions[TAKEOFF]);
}


/**
 * Madara function to land the agent
 **/
Madara::Knowledge_Record
  land (Madara::Knowledge_Engine::Function_Arguments & args,
             Madara::Knowledge_Engine::Variables & variables)
{
#ifdef  USING_PARROT_ARDRONE_2
  // insert code to land a drone
#endif
  return variables.evaluate (expressions[LAND]);
}



/**
 * Madara function to get the current position of the drone
 **/
Madara::Knowledge_Record
  get_current_position (Madara::Knowledge_Engine::Function_Arguments & args,
             Madara::Knowledge_Engine::Variables & variables)
{
#ifdef  USING_PARROT_ARDRONE_2
  // insert code to return a GPS location from Drone-RK
#else
  return "{drone{.id}.pos.x},{drone{.id}.pos.y},{drone{.id}.pos.z}";
#endif
}

/**
 * Madara function to check and reset the controller heartbeat
 **/
Madara::Knowledge_Record
  check_controller_heartbeat (Madara::Knowledge_Engine::Function_Arguments & args,
             Madara::Knowledge_Engine::Variables & variables)
{
  return variables.evaluate (expressions[CHECK_CONTROLLER_HEARTBEAT],
    Madara::Knowledge_Engine::Knowledge_Update_Settings (true));
}

/**
 * Madara function to compute the bounding box for drone search operation
 **/
Madara::Knowledge_Record
  compute_bounding_box (Madara::Knowledge_Engine::Function_Arguments & args,
             Madara::Knowledge_Engine::Variables & variables)
{
  return variables.evaluate (expressions[COMPUTE_BOUNDING_BOX]);
}

/**
 * Madara function to update position and mobility status
 **/
Madara::Knowledge_Record
  update_status (Madara::Knowledge_Engine::Function_Arguments & args,
             Madara::Knowledge_Engine::Variables & variables)
{
  return variables.evaluate (expressions[UPDATE_STATUS]);
}

/**
 * Madara function to compute the number of mobile drones still available
 **/
Madara::Knowledge_Record
  compute_mobile_drones (Madara::Knowledge_Engine::Function_Arguments & args,
             Madara::Knowledge_Engine::Variables & variables)
{
  return variables.evaluate (expressions[COMPUTE_MOBILE_DRONES]);
}

/**
 * Madara function to reset the controller heartbeat timer
 **/
Madara::Knowledge_Record
  controller_timer_reset (Madara::Knowledge_Engine::Function_Arguments & args,
             Madara::Knowledge_Engine::Variables & variables)
{
  variables.print ("Controller heartbeat has been reset.\n", 0);
  timers[CONTROLLER_HEARTBEAT].start ();
  return Madara::Knowledge_Record::Integer (1);
}
  
/**
 * Madara function to check if the controller timer is expired
 **/
Madara::Knowledge_Record
  controller_timer_is_expired (Madara::Knowledge_Engine::Function_Arguments & args,
             Madara::Knowledge_Engine::Variables & variables)
{
  variables.print (
    "Controller heartbeat timer is expired is being checked.\n", 0);
  ACE_hrtime_t elapsed;
  ACE_hrtime_t max_time;

  // set the max time to the seconds contained in controller.max_heartbeat
  max_time = 1000000000;
  max_time *= (ACE_hrtime_t)
    variables.get ("controller.max_heartbeat").to_integer ();

  // stop the timer and take the elapsed time
  timers[CONTROLLER_HEARTBEAT].stop ();
  timers[CONTROLLER_HEARTBEAT].elapsed_time (elapsed);

  variables.set (".heartbeat_latency",
    (Madara::Knowledge_Record::Integer)elapsed);

  if (elapsed > max_time)
    return Madara::Knowledge_Record::Integer (1);
  else
    return Madara::Knowledge_Record::Integer (0);
}
 
/**
 * Convenience function for defining all external functions. We only
 * need to call this once from the main function.
 **/
void define_external_functions (
  Madara::Knowledge_Engine::Knowledge_Base & knowledge)
{
  // define control functions
  knowledge.define_function ("rand", rand_int);
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

  // define tasks functions
  knowledge.define_function ("get_current_position",
    get_current_position);
  knowledge.define_function ("check_controller_heartbeat",
    check_controller_heartbeat);
  knowledge.define_function ("compute_mobile_drones",
    compute_mobile_drones);
  knowledge.define_function ("compute_bounding_box",
    compute_bounding_box);
  knowledge.define_function ("move_to_altitude",
    move_to_altitude);
  knowledge.define_function ("move_to_position",
    move_to_position);
  knowledge.define_function ("controller.timer_reset",
    controller_timer_reset);
  knowledge.define_function ("controller.timer_is_expired",
    controller_timer_is_expired);
  knowledge.define_function ("update_status",
    update_status);
  
}

/**
 * Convenience function for compiling all expressions into
 * Compiled_Expressions in a global store. This is needed to
 * work with the evaluate calls inside of the external functions.
 * Compiling expressions once globally saves us microseconds of time
 * every single function call.
 **/
void compile_expressions (
  Madara::Knowledge_Engine::Knowledge_Base & knowledge)
{
  expressions[CHECK_CONTROLLER_HEARTBEAT] = knowledge.compile (
    // if we've received a controller heartbeat
    "(controller.heartbeat => "
    "("
      // reset the heartbeat
      "controller.heartbeat = 0;"
      "controller.timer_reset ()"
    "))"
    "||"
    "controller.timer_is_expired () => land ()"
  );
  
  expressions[COMPUTE_BOUNDING_BOX] = knowledge.compile (
    // to be filled in
    ""
  );
  
  expressions[COMPUTE_MOBILE_DRONES] = knowledge.compile (
    // set mobile drones to 0 and disregard its return (choose right) 
    ".mobile_drones = 0 ;>"
    // for loop of .i = 0 until max_drones
    ".i[controller.max_drones]"
    "("
      // if drone{.i} is mobile, increment .mobile_drones
      "drone{.i}.mobile => ++.mobile_drones"
    ")"
  );
  
  
  expressions[MOVE_NORTH] = knowledge.compile (
    // if we've received a controller heartbeat
    "drone{.id}.pos.y = drone{.id}.pos.y + 1"
  );
  
  expressions[MOVE_SOUTH] = knowledge.compile (
    // if we've received a controller heartbeat
    "drone{.id}.pos.y = drone{.id}.pos.y - 1"
  );
  
  expressions[MOVE_EAST] = knowledge.compile (
    // if we've received a controller heartbeat
    "drone{.id}.pos.x = drone{.id}.pos.x + 1"
  );
  
  expressions[MOVE_WEST] = knowledge.compile (
    // if we've received a controller heartbeat
    "drone{.id}.pos.y = drone{.id}.pos.y - 1"
  );
  
  expressions[MOVE_TO_ORIGIN] = knowledge.compile (
    // if we've received a controller heartbeat
    "drone{.id}.pos.y = controller.pos.y;"
    "drone{.id}.pos.x = controller.pos.x"
  );
  

  
  expressions[LAND] = knowledge.compile (
    // if we've received a controller heartbeat
    "drone{.id}.landed = 1; drone{.id}.mobile = 0"
  );
  
  expressions[TAKEOFF] = knowledge.compile (
    // if we've received a controller heartbeat
    "drone{.id}.landed = 0; drone{.id}.mobile = 1"
  );
  
  expressions[SHUTDOWN] = knowledge.compile (
    // if we've received a controller heartbeat
    "drone{.id}.landed = 0; drone{.id}.mobile = 0; drone{.id}.shutdown = 1"
  );
  
  expressions[MAIN_LOGIC] = knowledge.compile (
    "update_status ();"
    "sense_thermals ();"
    "get_current_position ();"
    "check_controller_heartbeat ();"
    "compute_mobile_drones ();"
    "compute_bounding_box ();"
    "move_to_altitude ();"
    "move_to_position ();"
  );
  
  expressions[UPDATE_STATUS] = knowledge.compile (
    "drone{.id}.mobile=drone{.id}.mobile;"
    "drone{.id}.pos.x=drone{.id}.pos.x;"
    "drone{.id}.pos.y=drone{.id}.pos.y;"
    "drone{.id}.pos.z=drone{.id}.pos.z;"
  );

}


int main (int argc, char ** argv)
{
  // signal handler for clean exit
  ACE_Sig_Action sa ((ACE_SignalHandler) terminate, SIGINT);

  settings.hosts.resize (1);
  settings.hosts[0] = default_multicast;
  settings.type = Madara::Transport::MULTICAST;
  handle_arguments (argc, argv);

  Madara::Knowledge_Engine::Knowledge_Base knowledge (host, settings);

  // define external functions for the knowlege base to map calls to
  define_external_functions (knowledge);

  // compile all expressions into a global store, which is accessible
  // from our external functions
  compile_expressions (knowledge);

  // start the controller heartbeat
  timers[CONTROLLER_HEARTBEAT].start ();

  knowledge.set (".id", (Madara::Knowledge_Record::Integer) settings.id);

  // the controller can overwrite these values later if desired
  knowledge.set ("controller.max_heartbeat",
    15.0, Madara::Knowledge_Engine::Eval_Settings (true, true));
  knowledge.set ("controller.max_drones",
    Madara::Knowledge_Record::Integer (20),
    Madara::Knowledge_Engine::Eval_Settings (true, true));
  
  srand ((unsigned int)settings.id);

  // set our position to a random x,y,z
  knowledge.evaluate (
    "drone{.id}.pos.x = rand (8);"
    "drone{.id}.pos.y = rand (8);"
    "drone{.id}.pos.z = rand (40);"
    "drone{.id}.mobile = 1;"
    "controller.timer_reset ()"
    );


  Madara::Knowledge_Engine::Eval_Settings eval_settings;
  eval_settings.pre_print_statement = 
    "Mobile drones: {.mobile_drones}, Heartbeat latency: {.heartbeat_latency}\n"
  ;
  
  eval_settings.post_print_statement = 
    "Current drone{.id} position: {drone{.id}.pos.x},{drone{.id}.pos.y}\n"
   ;

  // until the user presses ctrl+c in this terminal, check for input
  while (!terminated)
  {
    knowledge.evaluate (expressions[MAIN_LOGIC], eval_settings);
    ACE_OS::sleep (1);
  }

  knowledge.print ();

  return 0;
}