#ifndef _MADARA_TRANSPORT_SETTINGS_H_
#define _MADARA_TRANSPORT_SETTINGS_H_

/**
 * @file TransportSettings.h
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains the transport::Base class, which provides an
 * extensible transport layer for sending knowledge updates in KaRL.
 * To support knowledge updates, only the send_multiassignment method
 * is currently required to be extended as the set, evaluate, and wait
 * methods all call send_multiassignment. For example transport, @see
 * madara::transport::MulticastTransport.
 **/

#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <ostream>

#include "madara/LockType.h"
#include "madara/knowledge/KnowledgeRecord.h"
#include "madara/knowledge/ThreadSafeContext.h"
#include "madara/expression/ExpressionTree.h"
#include "madara/expression/Interpreter.h"
#include "madara/MADARA_export.h"
#include "madara/transport/Fragmentation.h"

namespace madara
{
  namespace transport
  {
    // forward declare for friending
    class Base;

    enum Types {
      NO_TRANSPORT = 0,
      SPLICE       = 1,
      NDDS         = 2,
      TCP          = 3,
      UDP          = 4,
      MULTICAST    = 5,
      BROADCAST    = 6,
      REGISTRY_SERVER = 7,
      REGISTRY_CLIENT = 8,
      ZMQ = 9
    };

    enum Reliabilities {
      BEST_EFFORT = 0,
      RELIABLE = 1
    };

    enum Messages {
      ASSIGN = 0,
      OPERATION = 1,
      MULTIASSIGN = 2,
      REGISTER = 3,
      LATENCY = 10,
      LATENCY_AGGREGATE = 11,
      LATENCY_SUMMATION = 12,
      VOTE = 20
    };

    /**
     * Holds basic transport settings
     */
    class MADARA_Export TransportSettings
    {
    public:
      // allow transport::Base to alter private/protected members
      friend class Base;

      // for ease-of-use, typedef the templated guard
      typedef MADARA_GUARD_TYPE ContextGuard;

      /// Default knowledge domain
      #define DEFAULT_DOMAIN      "KaRL"

      typedef std::vector <std::string>    Voters;

      /// Default queue length for event history (must be high for
      /// reliable transport
      static const uint32_t DEFAULT_QUEUE_LENGTH = 500000;

      /// Default deadline
      static const uint32_t DEFAULT_DEADLINE = 0;

      /// Default transport
      static const uint32_t DEFAULT_TRANSPORT = NO_TRANSPORT;

      /// Default reliability
      static const uint32_t DEFAULT_RELIABILITY = RELIABLE;

      /**
       * Default id in group
       **/
      #define DEFAULT_ID                0

      /**
       * Default number of processes in group
       **/
      #define DEFAULT_PROCESSES         1

      /**
      * Default number of processes in group
      **/
      #define MAXIMUM_RESEND_ATTEMPTS   10

      /// Constructor for this class
      TransportSettings ();

      /// Copy constructor
      TransportSettings (const TransportSettings & settings);

      virtual ~TransportSettings ();

      /// Assignment operator
      void operator= (const TransportSettings & settings);
      
      /**
       * Adds a read domain to the list of domains to read from
       * @param  domain   domain to add to the read list
       **/
      void add_read_domain (const std::string domain);

      /**
      * Clears the list of read domains
      **/
      void clear_read_domains (void);

      /**
      * Retrieves the list of read domains
      * @param  domains   the list to fill with all read domains
      **/
      void get_read_domains (std::vector<std::string> & domains) const;

      /**
      * Checks if a domain is in the domain read list
      * @param  domain   domain to check
      * @return true if the domain exists in the read list
      **/
      bool is_reading_domain (const std::string domain) const;

      /**
      * Returns the number of read domains
      * @return the number of domains in the read list
      **/
      size_t num_read_domains (void) const;

       /**
       * Loads the settings from a binary file
       * @param  filename    the file to load from
       * @param  prefix      prefix for all transports settings
       **/
       virtual void load (const std::string & filename,
         const std::string & prefix = "transport");

       /**
       * Loads the settings from a text file
       * @param  filename    the file to load from
       * @param  prefix      prefix for all transports settings
       **/
       virtual void load_text (const std::string & filename,
         const std::string & prefix = "transport");

       /**
       * Saves the settings from a binary file
       * @param  filename    the file to load from
       * @param  prefix      prefix for all transports settings
       **/
       virtual void save (const std::string & filename,
         const std::string & prefix = "transport") const;

       /**
       * Saves the settings from a text file
       * @param  filename    the file to load from
       * @param  prefix      prefix for all transports settings
       **/
       virtual void save_text (const std::string & filename,
         const std::string & prefix = "transport") const;

      /// All class members are accessible to users for easy setup

      /**
       * We only write to one domain
       **/
      std::string write_domain;

      /// the number of read threads to start
      uint32_t read_threads;

      /// Length of the buffer used to store history of events
      uint32_t queue_length;

      /// Type of transport. See madara::transport::Types for options
      uint32_t type;

      /// Maximum allowed fragment size for partitioning large messages
      uint32_t max_fragment_size;

      /// Maximum number of attempts to resend if transport is busy
      int resend_attempts;

      /**
       * Indicates queue length for holding clock-keyed fragments. Note
       * that this does not limit the number of fragments--only how many
       * clock values we want to queue for defragmentation. So, if you
       * have a fragment_queue_length of 3, and your last three received
       * fragmented clock values were 1=4GB, 2=4GB, 3=4GB, then you could
       * have 12GB, regardless of max_fragment_size.
       **/
      uint32_t fragment_queue_length;

      /// Reliability required of the transport. 
      /// See madara::transport::Reliabilities for options
      uint32_t reliability;

      /// the id of this process. May be useful for latency gathering
      /// or testing purposes
      uint32_t id;

      /// number of processes expected in the network (best to overestimate
      /// if building latency tables
      uint32_t processes;

      /// logic to be evaluated after every successful update
      std::string on_data_received_logic;
      
      /// delay launching transports
      bool delay_launch;

      /// prevent MADARA from exiting on fatal errors and invalid state
      bool never_exit;

      /// send the reduced message header (clock, size, updates, KaRL id)
      bool send_reduced_message_header;

      /// map of fragments received by originator
      mutable OriginatorFragmentMap fragment_map;

      /// time to sleep between sends and rebroadcasts
      double slack_time;

      /**
       * number of valid messages allowed to be received per second. This
       * value can be -1 or 0.0 to go as fast as possible
       **/
      double read_thread_hertz;

      /**
       * Host information for transports that require it. The format of these
       * is transport specific, but for UDP, you might have "localhost:1234"
       * for a host named localhost and a port of 1234. See the specific
       * transport for more information.
       **/
      std::vector <std::string> hosts;

      /**
       * if true, never send over transport
       **/
      bool no_sending;
      
      /**
       * if true, never receive over transport
       **/
      bool no_receiving;

    private:

      /**
      * Any acceptable read domain is added here
      **/
      std::map <std::string, int> read_domains_;
    };

    inline std::string
    type_name (const TransportSettings & settings)
    {
      std::string name = "none";

      switch (settings.type)
      {
      case 0:
        name = "None";
        break;
      case 1:
        name = "PrismTech DDS";
        break;
      case 2:
        name = "RTI DDS";
        break;
      case 3:
        name = "TCP (unsupported)";
        break;
      case 4:
        name = "UDP Unicast";
        break;
      case 5:
        name = "UDP Multicast";
        break;
      case 6:
        name = "UDP Broadcast";
        break;
      case 7:
        name = "UDP P2P Registry Server";
        break;
      case 8:
        name = "UDP P2P Registry Client";
        break;
      case 9:
        name = "ZeroMQ Pub/Sub";
        break;
      }

      return name;
    }
  }
}

#include "TransportSettings.inl"

#endif // _MADARA_TRANSPORT_SETTINGS_H_
