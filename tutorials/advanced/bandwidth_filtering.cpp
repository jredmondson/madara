
/**
 * @file bandwidth_filtering.cpp
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains a simple distributed application that reads images from
 * a file, monitors bandwidth, and filters images from writes if a bandwidth
 * limit is met (until the bandwidth usage becomes acceptable again)
 * 
 * Windows:
 *   cd %MADARA_ROOT%\bin
 *   tutorial_bandwidth_filtering 0 (in one terminal)
 *   tutorial_bandwidth_filtering 1 (in a second terminal)
 *   etc.
 *
 * Linux:
 *   cd $MADARA_ROOT/bin
 *   tutorial_bandwidth_filtering 0 (in one terminal)
 *   tutorial_bandwidth_filtering 1 (in a second terminal)
 *
 **/

#include <iostream>
#include <string>
#include <sstream>
#include "madara/knowledge_engine/Knowledge_Base.h"
#include "ace/Signal.h"
#include "ace/OS_NS_Thread.h"
#include "madara/transport/Transport_Context.h"
#include "madara/logger/Global_Logger.h"

/**
* Define helpful shortened namespaces that we can refer to later
**/
namespace engine = Madara::Knowledge_Engine;
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
 * Qos_Transport_Settings is a subclass of Settings and has a variety of
 * useful tools for ensuring QoS in the distributed context. Specifically,
 * we'll be using the send filters available through the QoS_Transport_Settings
 * interface.
 **/
transport::QoS_Transport_Settings settings;

// set a target sending bandwidth usage of 100000
uint32_t target_sending_bandwidth = 100000;

/**
 * If real-time performance is desired from filtering calls, all variables
 * should be compiled into variable references for O(1) lookups. Setting
 * or getting variables through the knowledge base or variables interface
 * is an O(log n) lookup without a compiled accessor like variable reference.
 *
 * To initialize a Variable_Reference, see the get_ref function available
 * through
 * Madara::Knowledge_Engine:Variables            or
 * Madara::Knowledge_Engine::Knowledge_Base
 **/
engine::Variable_Reference  payloads_received;

/**
 * variable reference to indicate that publisher should be slowed
 **/
engine::Variable_Reference  slow_publisher;

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
 * Filter for dropping images if a target bandwidth is overshot.
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
 * Madara::Knowledge_Engine::Variables is a hook into the knowledge base that
 * allows for querying or setting knowledge not available in the args list.
 * Variables can be used for communicating with other filters or the main logic
 * of the agent.
 **/
Madara::Knowledge_Record
filter_images (
  engine::Function_Arguments & args,
  engine::Variables & vars)
{
  Madara::Knowledge_Record result;

  if (args.size () >= 5)
  {
    /**
     * if the record we're filtering is a file and
     * the used sending bandwidth is greater than the
     * target sending bandwidth
     **/
    if (args[0].is_file_type () && 
        args[3].to_integer () > target_sending_bandwidth)
    {
      /**
       * We're writing the filter this way to be very explicit about the filter
       * decision. A default constructor for a knowledge record creates a
       * Knowledge_Record with its status set to UNCREATED. The transport layer
       * looks for such a record to indicate the record has been discarded. If
       * any valid value is returned, this will be the new value used in
       * constructing a packet or in accepting the value into the system during
       * a receive operation.
       *
       * We could have also changed the above check to args[0].is_image_type ()
       * to only check for images, but this filter is more broadly applicable
       * and may be more useful in your own applications.
       **/
      vars.print ("Bandwidth usage is " + args[3].to_string () + 
          ". Filtering image record...\n", 0);
    }
    else
    {
      /**
       * By default, we don't filter meta data, but if we are using more than
       * twice our target bandwidth, we filter everything. We also block the
       * sender for 5 seconds.
       **/
      if (args[3].to_integer () >= 200000)
      {
        vars.print ("Bandwidth usage is " + args[3].to_string () + 
          ". Filtering record...\n", 0);

        /**
         * You may think you should sleep here, but that would be a bad idea.
         * Sleeping inside of a filter or function will block the knowledge
         * base from operating--including receive operations that may be
         * useful for figuring out what is going on in the network.
         **/
        vars.inc (slow_publisher);
      }
      else
      {
        /**
         * Otherwise, then we keep
         * as-is, regardless of bandwidth usage. This allows the packet to
         * keep important meta data and just lose the extra payload.
         **/
        vars.print ("Bandwidth usage is " + args[3].to_string () + 
          ". Not filtering record...\n", 0);
        result = args[0];
      }
    }
  }

  return result;
}

Madara::Knowledge_Record
update_payloads_received (
  engine::Function_Arguments & args,
  engine::Variables & vars)
{
  if (args.size () > 0)
  {
    if (args[0].is_file_type ())
    {
      /**
       * Update the knowledge base with the fact that we have received another
       * unfiltered payload. Inc increments a variable.
       * 
       * inc is a highly optimized operation, especially when paired with
       * Variable_Reference.
       **/
      vars.inc (payloads_received);
    }

    /**
     * This filter will never remove an update from consideration.
     * Consequently, we always return the first argument unaltered.
     **/
    return args[0];
  }

  /**
   * Because filters are essentially functions that can be used by anyone
   * with a define_function call, we handle the possibility that args is
   * empty, implying someone called in from a MADARA evaluate/wait call
   * with no arguments.
   **/
  else
    return Madara::Knowledge_Record ();
}

int main (int argc, char * argv[])
{
  // Register a signal handler for Control+C
  ACE_Sig_Action sa ((ACE_SignalHandler) terminate, SIGINT);

  // Setup a multicast transport with the settings mentioned above.
  settings.type = transport::MULTICAST;
  settings.hosts.resize (1);
  settings.hosts[0] = multicast_address;

  /**
   * The default queue length is not large enough to accomodate images.
   * If we want to send images/files, then we need to set the queue
   * length to something appropriate. Here, we allow for 500k messages.
   **/

  settings.queue_length = 100000;
  
  /**
   * add_send_filter allows us to set filters for multiple types at
   * the same time. Types can be logically or'd together to form the
   * first parameter. Here, we are using a predefined aggregation of
   * types that specifies IMAGE_JPEG | TEXT_FILE | XML | UNKNOWN_FILE_TYPE
   * (collectively known as the FILE_TYPES aggregation) along with
   * all primitive types (INTEGER | DOUBLE | etc.)
   *
   * Any time the send_data method is called on this transport for this
   * agent, the send filter will run the file record through the filter_images
   * filter that we've specified earlier in this file.
   **/
  settings.add_send_filter (Madara::Knowledge_Record::ALL_TYPES,
                            filter_images);


  settings.add_receive_filter (Madara::Knowledge_Record::ALL_FILE_TYPES,
                            update_payloads_received);

  // Create the knowledge base with the transport settings set for multicast
  engine::Knowledge_Base knowledge (host, settings);
  
  // Check command line arguments for a non-zero id
  if (argc >= 2)
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
  
  /**
   * In order for our receive filter to work, we have to first initialize
   * the reference to a variable in the knowledge base. We choose a global
   * variable, so the publisher can have the option to see how many items
   * the other agents are receiving. This allows for the possibilty of the
   * agents to gossip about what should be done (this tutorial does not
   * cover gossip/collaboration).
   **/
  payloads_received = knowledge.get_ref ("payloads_received{.id}",
    engine::Knowledge_Reference_Settings (true));

  /**
   * Variable that the publisher uses (id==0) to trigger a sleep statement
   **/
  slow_publisher = knowledge.get_ref (".slow_publisher");

  /**
   * Many useful utility functions can be found in madara/utility/Utility.h.
   * The expand_envs function takes a string and expands any environment
   * variables it finds, denoted by $ and enclosed parentheses. Here we
   * look for one of the 50k images we have included in the repo for testing.
   **/
  std::string filename =
    Madara::Utility::expand_envs (
      "$(MADARA_ROOT)/tests/images/manaus_hotel_225x375.jpg");

  /**
   * To showcase how selective the filtering is, we also send some
   * meta data with the payload which should not be filtered out,
   * regardless of bandwidth utilization.
   **/
  engine::Variable_Reference payloads_sent =
    knowledge.get_ref ("payloads_sent");

  /**
   * Variable expansion can be very useful and quick to use, but it's orders
   * of magnitudes slower than not using variable expansion. To make our
   * evaluate function as fast as possible, we expand the logic first to
   * make payloads_received{.id} into payloads_received1, if .id==1
   **/

  std::string logic = knowledge.expand_statement (
    ".last_received != payloads_received{.id} => "
    "               .last_received = payloads_received{.id}");

  /**
   * Additionally, let's compile the logic into a cached expression tree
   **/

  engine::Compiled_Expression ce =
    knowledge.compile (logic);

  while (!terminated)
  {
    if (knowledge.get (".id").to_integer () == 0)
    {
      if (knowledge.get (slow_publisher).to_integer () == 0)
      {
        /**
         * Read file takes a variable name and a file name to be read from.
         * Consequently, the following reads an image from our tests directory
         * into the payload variable, which will then be transmitted to other
         * agents. This is the basic loop of an image streaming application.
         *
         * The "true" we pass to the eval settings specifies that we want to
         * delay sending the payload until we also set the meta data
         * (payloads_sent) later in the loop.
         **/
        knowledge.read_file ("payload", filename,
          engine::Eval_Settings (true));

        /**
         * Increment payloads sent and send this as meta data to anyone curious
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
        knowledge.print ("Bandwidth limit hit. Stopping publisher for 10s.\n");
        ACE_OS::sleep (10);
        knowledge.set (slow_publisher, Madara::Knowledge_Record::Integer (0));
      }
      knowledge.print (
        "payloads sent = {payloads_sent}, "
        "payloads received (on id==1) = {payloads_received1}\n");
    }
    else
    {
      // send updates made in filters;
      knowledge.evaluate (logic);

      // Sleep for a second so we limit the number of printouts
      ACE_OS::sleep (1);
      /**
       * Print the payloads sent versus the ones received. Because this
       * is a very bursty publisher, we should see a very small proportion
       * of payloads received.
       **/
      knowledge.print (
        "payloads sent = {payloads_sent}, "
        "payloads received = {payloads_received{.id}}\n");
    }

  }
  
 
  /**
   * Print all knowledge at the end of the application run.
   **/
  knowledge.print ();

  return 0;
}
