
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
#include "madara/knowledge/KnowledgeBase.h"
#include "madara/transport/TransportContext.h"
#include "madara/logger/GlobalLogger.h"
#include "madara/utility/Utility.h"

/**
* Define helpful shortened namespaces that we can refer to later
**/
namespace knowledge = madara::knowledge;
namespace transport = madara::transport;
namespace logger = madara::logger;

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

/**
 * If real-time performance is desired from filtering calls, all variables
 * should be compiled into variable references for O(1) lookups. Setting
 * or getting variables through the knowledge base or variables interface
 * is an O(log n) lookup without a compiled accessor like variable reference.
 *
 * To initialize a VariableReference, see the get_ref function available
 * through
 * Madara::KnowledgeEngine:Variables            or
 * madara::knowledge::KnowledgeBase
 **/
knowledge::VariableReference  payloads_received;

/**
 * variable reference to indicate that publisher should be slowed
 **/
knowledge::VariableReference  slow_publisher;

madara::knowledge::KnowledgeRecord
update_payloads_received (
  knowledge::FunctionArguments & args,
  knowledge::Variables & vars)
{
  if(args.size () > 0)
  {
    if(args[0].is_file_type ())
    {
      /**
       * Update the knowledge base with the fact that we have received another
       * unfiltered payload. Inc increments a variable.
       * 
       * inc is a highly optimized operation, especially when paired with
       * VariableReference.
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
    return madara::knowledge::KnowledgeRecord ();
}

int main (int argc, char * argv[])
{
  // Setup a multicast transport with the settings mentioned above.
  settings.type = transport::MULTICAST;
  settings.hosts.push_back (multicast_address);

  /**
   * The default queue length is not large enough to accomodate images.
   * If we want to send images/files, then we need to set the queue
   * length to something appropriate. Here, we allow for 500k messages.
   **/

  settings.queue_length = 100000;
  
  // set the send bandwidth limit to 100,000 bytes per second.
  settings.set_send_bandwidth_limit (100000);
  

  settings.add_receive_filter (madara::knowledge::KnowledgeRecord::ALL_FILE_TYPES,
                            update_payloads_received);

  // Create the knowledge base with the transport settings set for multicast
  knowledge::KnowledgeBase knowledge (host, settings);
  
  // Check command line arguments for a non-zero id
  if(argc >= 2)
  {
    // save the first argument into an integer
    madara::knowledge::KnowledgeRecord::Integer new_id;
    std::stringstream buffer (argv[1]);
    buffer >> new_id;

    /**
     * Update the knowledge base to include our .id. All variables are zero
     * by default in the knowledge base.
     **/
    knowledge.set (".id", new_id,
      madara::knowledge::EvalSettings::SEND);

    // the 2nd argument is the log level
    if(argc >= 3)
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
    knowledge::KnowledgeReferenceSettings (true));

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
    madara::utility::expand_envs (
      "$(MADARA_ROOT)/tests/images/manaus_hotel_225x375.jpg");

  /**
   * To showcase how selective the filtering is, we also send some
   * meta data with the payload which should not be filtered out,
   * regardless of bandwidth utilization.
   **/
  knowledge::VariableReference payloads_sent =
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

  knowledge::CompiledExpression ce =
    knowledge.compile (logic);

  while(1)
  {
    if(knowledge.get (".id").to_integer () == 0)
    {
      if(knowledge.get (slow_publisher).to_integer () == 0)
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
          knowledge::EvalSettings (true));

        /**
         * Increment payloads sent and send this as meta data to anyone curious
         * about how many payloads have been sent, which they can check against
         * their own .payloads_received if they want to know the number of
         * drops.
         **/
        knowledge.set (payloads_sent,

          // increment the payloads_sent and provide it as an arg to set
          (++knowledge.get (payloads_sent)).to_integer (),
          madara::knowledge::EvalSettings::SEND);
      }
      else
      {
        knowledge.print ("Bandwidth limit hit. Stopping publisher for 10s.\n");
        madara::utility::sleep (10);
        knowledge.set (slow_publisher,
          madara::knowledge::KnowledgeRecord::Integer (0),
          madara::knowledge::EvalSettings::SEND);
      }
      knowledge.print (
        "payloads sent = {payloads_sent}, "
        "payloads received (on id==1) = {payloads_received1}\n");
    }
    else
    {
      // send updates made in filters;
      knowledge.evaluate (logic,
        madara::knowledge::EvalSettings::SEND);

      // Sleep for a second so we limit the number of printouts
      madara::utility::sleep (1);
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
