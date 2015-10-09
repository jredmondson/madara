
/**
 * @file deadline_filtering.cpp
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains a simple distributed application that sends messages
 * and enforces deadlines on the receiver side. To induce deadline infractions,
 * the receiver receives a couple of messages and then sleeps for 10s. It keeps
 * track of the number of messages that were received within deadline and
 * out-of-deadline and prints them at the end.
 * 
 * Windows:
 *   cd %MADARA_ROOT%\bin
 *   tutorial_deadline_filtering 0 (in one terminal)
 *   tutorial_deadline_filtering 1 (in a second terminal)
 *   etc.
 *
 * Linux:
 *   cd $MADARA_ROOT/bin
 *   tutorial_deadline_filtering 0 (in one terminal)
 *   tutorial_deadline_filtering 1 (in a second terminal)
 *
 **/

#include <iostream>
#include <string>
#include <sstream>
#include "madara/knowledge_engine/KnowledgeBase.h"
#include "ace/Signal.h"
#include "ace/OS_NS_Thread.h"
#include "madara/transport/TransportContext.h"
#include "madara/logger/GlobalLogger.h"

/**
* Define helpful shortened namespaces that we can refer to later
**/
namespace engine = Madara::KnowledgeEngine;
namespace transport = Madara::Transport;
namespace logger = Madara::Logger;


/**
 * Default settings for our Network Transport. We put them
 * outside of the main function to allow for future expansion
 * with an argument parsing function.
 **/
std::string host ("");
const std::string multicast_address ("239.255.0.1:4150");

/**
 * QosTransportSettings is a subclass of Settings and has a variety of
 * useful tools for ensuring QoS in the distributed context. Specifically,
 * we'll be using the send filters available through the QoSTransportSettings
 * interface.
 **/
transport::QoSTransportSettings settings;

// set a deadline in seconds that will be enforced in the filter
int64_t deadline_in_secs = 10;

/**
 * If real-time performance is desired from filtering calls, all variables
 * should be compiled into variable references for O(1) lookups. Setting
 * or getting variables through the knowledge base or variables interface
 * is an O(log n) lookup without a compiled accessor like variable reference.
 *
 * To initialize a VariableReference, see the get_ref function available
 * through
 * Madara::KnowledgeEngine:Variables            or
 * Madara::KnowledgeEngine::KnowledgeBase
 **/
engine::VariableReference  on_time_messages;

/**
 * variable reference to indicate that publisher should be slowed
 **/
engine::VariableReference  invalid_messages;

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
 * Filter for checking deadlines on sent messages.
 *
 * Filters in MADARA are essentially external function calls that have the
 * opportunity to augment the first argument passed into the function. MADARA
 * will provide other arguments to the filter, to help provide the developer
 * with context concerning the transport layer (which may impact decisions
 * made during filtering).
 *
 * The arguments passed to the filter are the following:
 *
 * args[0] : The knowledge record that the filter is acting upon
 * args[1] : The name of the knowledge record, if applicable ("" if unnamed)
 * args[2] : The type of operation calling the filter (integer valued).
 *         : IDLE_OPERATION (should never see)
 *         : SENDING_OPERATION (transport is about to send the record)
 *         : RECEIVING_OPERATION (transport has received the record and is
 *         :                      about to apply the update)
 *         : REBROADCASTING_OPERATION (transport is about to rebroadcast the
 *                                     record -- only happens if rebroadcast
 *                                     is enabled in Transport Settings)
 * args[3] : Bandwidth used while sending through this transport
 * args[4] : Bandwidth used while receiving from this transport
 * args[5] : Message timestamp (when the message was originally sent)
 * args[6] : Current timestamp (the result of time (NULL))
 * args[7] : domain (partition of knowledge updates)
 * 
 * These args are modifiable in the understanding that future arguments may be
 * communication channels back to the transport to inform some higher level
 * instruction (e.g., discard the entire packet, turn off rebroadcasting, etc.)
 * This is currently unimplemented but planned.
 *
 * Madara::KnowledgeEngine::Variables is a hook into the knowledge base that
 * allows for querying or setting knowledge not available in the args list.
 * Variables can be used for communicating with other filters or the main logic
 * of the agent.
 **/
Madara::KnowledgeRecord
filter_deadlines (
  engine::FunctionArguments & args,
  engine::Variables & vars)
{
  Madara::KnowledgeRecord result;

  if (args.size () >= Madara::Filters::TOTAL_ARGUMENTS)
  {
    /**
     * if the message has arrived within deadline, print success and
     * increment the on time messages
     **/
    if (args[6].to_integer () - args[5].to_integer () < deadline_in_secs)
    {
      /**
       * 
       **/
      vars.print ("Received a message on time.\n", 0);
      vars.inc (on_time_messages);

      if (vars.get (on_time_messages).to_integer () == 2)
      {
        vars.print (
          "Inducing a 10 seconds sleep to force deadline infractions.\n", 0);
        ACE_OS::sleep (10);
      }
    }
    else
    {
      vars.print ("Deadline violated.\n", 0);

      /**
        * You may think you should sleep here, but that would be a bad idea.
        * Sleeping inside of a filter or function will block the knowledge
        * base from operating--including receive operations that may be
        * useful for figuring out what is going on in the network.
        **/
      vars.inc (invalid_messages);
    }
  }

  return result;
}

int main (int argc, char * argv[])
{
  // Register a signal handler for Control+C
  ACE_Sig_Action sa ((ACE_SignalHandler) terminate, SIGINT);

  // Setup a multicast transport with the settings mentioned above.
  settings.type = transport::MULTICAST;
  settings.hosts.resize (1);
  settings.hosts[0] = multicast_address;
  settings.id = 0;
  
  // Check command line arguments for a non-zero id
  if (argc >= 2)
  {
    // save the first argument into an integer
    std::stringstream buffer (argv[1]);
    buffer >> settings.id;

    // the 2nd argument is the log level
    if (argc >= 3)
    {
      int log_level;
      std::stringstream reader (argv[2]);
      buffer >> log_level;

      /**
       * Convenience function for setting the verbosity of log messages.
       * Logging level can be from 0 (sparse) to 10 (detailed)
       **/
      logger::global_logger->set_level (log_level);
    }
  }
  
  if (settings.id != 0)
  {
    /**
     * unlike other tutorials that require multiple filters, a deadline
     * filter only needs to be implemented on the receive side or the
     * rebroadcast side. We're not rebroadcasting in this tutorial,
     * so we'll just add a receive filter.
     **/
    settings.add_receive_filter (Madara::KnowledgeRecord::ALL_TYPES,
                              filter_deadlines);
  }

  // Create the knowledge base with the transport settings set for multicast
  engine::KnowledgeBase knowledge (host, settings);
  
  /**
    * Update the knowledge base to include our .id. All variables are zero
    * by default in the knowledge base.
    **/
  knowledge.set (".id", Madara::KnowledgeRecord::Integer (settings.id));

  /**
   * In order for our receive filter to work, we have to first initialize
   * the reference to a variable in the knowledge base. We choose a global
   * variable, so the publisher can have the option to see how many items
   * the other agents are receiving. This allows for the possibilty of the
   * agents to gossip about what should be done (this tutorial does not
   * cover gossip/collaboration).
   **/
  on_time_messages = knowledge.get_ref ("on_time_messages{.id}",
    engine::KnowledgeReferenceSettings (true));

  /**
   * Variable that the publisher uses (id==0) to trigger a sleep statement
   **/
  invalid_messages = knowledge.get_ref ("invalid_messages{.id}");

  /**
   * The publisher (id == 0) keeps track of number of payloads sent
   **/
  engine::VariableReference payloads_sent =
    knowledge.get_ref ("payloads_sent");

  /**
   * Receiver waits until the number of on time messages is greater than 1000
   **/

  std::string receiver_logic = knowledge.expand_statement (
    "on_time_messages{.id} > 1000");
  
  /**
   * Sender only sends enough messages for first receiver to meet quota
   **/

  std::string sender_logic = knowledge.expand_statement (
    "on_time_messages1 > 1000");

  /**
   * Additionally, let's compile the logic into a cached expression tree
   **/

  engine::CompiledExpression compiled_sender_logic =
    knowledge.compile (sender_logic);
  
  engine::CompiledExpression compiled_receiver_logic =
    knowledge.compile (receiver_logic);

  while (!terminated)
  {
    if (settings.id == 0)
    {
      if (knowledge.evaluate (compiled_sender_logic).to_integer () == 0)
      {
        /**
         * We're including a string payload of questionable utility
         **/
        knowledge.set ("payload", "A useless string");

        /**
         * Increment messages sent and send this as meta data to anyone curious
         * about how many payloads have been sent, which they can check against
         * their own .payloads_received if they want to know the number of
         * drops.
         **/
        knowledge.set (payloads_sent,

          // increment the payloads_sent and provide it as an arg to set
          (++knowledge.get (payloads_sent)).to_integer ());
      }
      else
      {
        terminated = true;
      }
      knowledge.print (
        "payloads sent = {payloads_sent}, "
        "payloads received (on id==1) = {on_time_messages1}\n");
    }
    else
    {
      // send updates made in filters;
      knowledge.wait (compiled_receiver_logic);

      terminated = true;

      /**
       * Print the payloads sent versus the ones received. Because this
       * is a very bursty publisher, we should see a very small proportion
       * of payloads received.
       **/
      knowledge.print (
        "payloads sent = {payloads_sent}, "
        "payloads received = {on_time_messages{.id}}\n");
    }

  }
  
 
  /**
   * Print all knowledge at the end of the application run.
   **/
  knowledge.print ();

  return 0;
}
