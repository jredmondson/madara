
/**
 * @file creating_custom_transports.cpp
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains an example custom user transport that
 * can be attached to a knowledge base
 **/

#include "madara/knowledge_engine/Knowledge_Base.h"
#include "Custom_Transport.h"
#include <iostream>
#include <string>
#include "ace/Signal.h"
#include "ace/OS_NS_Thread.h"

/**
 * Default settings for our Network Transport. We put them
 * outside of the main function to allow for future expansion
 * with an argument parsing function.
 **/
std::string host ("");
const std::string multicast_address ("239.255.0.1:4150");
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

int main (int argc, char * argv[])
{
  // Register a signal handler for Control+C
  ACE_Sig_Action sa ((ACE_SignalHandler) terminate, SIGINT);
  
  // Setup a multicast transport with the settings mentioned above.
  settings.type = Madara::Transport::MULTICAST;
  settings.hosts.resize (1);
  settings.hosts[0] = multicast_address;
  
  Madara::Knowledge_Engine::Knowledge_Base knowledge;

  Custom_Transport * transport = new Custom_Transport (knowledge.get_id (),
    knowledge.get_context (), settings, true);

  knowledge.attach_transport (transport);

  while (!terminated)
  {
    knowledge.evaluate ("agent11.online=1; agent12.online=1");

    knowledge.print ();

    // Sleep for a second so we limit the number of printouts
    ACE_OS::sleep (1);
  }

  return 0;
}
