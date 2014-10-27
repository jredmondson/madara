#ifndef _TEST_FRAMEWORK_H_
#define _TEST_FRAMEWORK_H_

/**
 * @file Knowledge_Base.h
 * @author James Edmondson <james.r.edmondson@vanderbilt.edu>
 *
 * This file contains the Knowledge_Base class
 */

#include "madara/kats/KATS_export.h"
//#include "madara/transport/Transport.h"
#include "madara/knowledge_engine/Knowledge_Base.h"

namespace Madara
{

  namespace KATS
  {
    class KATS_Export Settings : public Madara::Transport::Settings
    {
    public:
      /// Default knowledge domain
      #define DEFAULT_KATS_DOMAIN       "KATS"
      #define DEFAULT_KATS_TRANSPORT    Madara::Transport::SPLICE
      #define DEFAULT_ID                0
      #define DEFAULT_PROCESSES         1
      #define DEFAULT_HOST              "localhost"
      #define DEFAULT_KATS_RELIABILITY  Madara::Transport::RELIABLE

    /**
     * @class Settings
     * @brief Provides a testing configuration to a KATS Test Framework
     */
      Settings ()
        : Madara::Transport::Settings ()
      {
        // set the underlying members
        this->domains = DEFAULT_KATS_DOMAIN;
        this->type = DEFAULT_KATS_TRANSPORT;
        this->reliability = DEFAULT_KATS_RELIABILITY;

        // set our local members
        id = DEFAULT_ID;
        processes = DEFAULT_PROCESSES;
        host = DEFAULT_HOST;
      }

      /// our host name/ip (localhost by default, if multiple hosts
      /// please use actual ips or valid hosts). This is not used for
      /// creating communication channels but is used to identify the
      /// source of data in KaRL
      std::string host;
    };

    /**
     * @class Test_Framework
     * @brief This class provides a distributed testing framework to users
     */
    class KATS_Export Test_Framework
    {
    public:
      /**
       * Constructor
       * @param   config   framework configuration
       **/
      Test_Framework (const Settings & config);

      /**
       * Copy constructor
       **/
      Test_Framework (const Test_Framework & original);

      /**
       * Destructor
       **/
      ~Test_Framework ();

      /**
       * Barriers on all processes until everyone is at the event
       * @param    event_name    name of the event to barrier on
       */
      Madara::Knowledge_Record::Integer barrier (const std::string & event_name);

      /**
       * Attempts to send out all global knowledge
       */
      void sync ();

      /**
       * Syncs on all processes until everyone is at the event and
       * has the latest information. This function does three main operations:
       * 1) barrier on every process synchronizing on this event, 2)
       * marking all globals as modified to current clock, and 3)
       * barrier on every process sending their updates.
       * @param    event_name    name of the sync event to barrier on
       */
      void barriered_sync (const std::string & event_name);

      /**
       * Creates a testing event
       * @param name               the name of this event
       * @param pre_condition      condition that needs to be true 
       *                           for this event to be true
       * @param post_condition     variables to set after the pre_condition
       *                           is satisfied
       * @param fail_condition     condition that would result in
       *                           aborting the test
       * @param continue_condition condition to skip the post_condition and
       *                           event barrier
       * @param barrier_this_event barrier across all processes as part
       *                           of the pre_condition
       * @param close_transport    closes transport after the event
       * @returns          0 if all conditions passed, -1 if fail_condition
       *                   was met
       **/
      int event (const std::string & name, 
        const std::string & pre_condition,
        const std::string & post_condition,
        const std::string & fail_condition = "",
        const std::string & continue_condition = "",
        bool barrier_this_event = false,
        bool close_transport = false
      );

      /**
       * Logs according to a severity level
       * @param level      level of severity
       * @param statement  statement to print (can contain variable expansion
       *                   from the knowledge base)
       **/
      void log (const std::string & statement, unsigned int level = 0);

      
      /**
       * Retrieves a knowledge value
       * @param key                knowledge location
       * @return                   value at knowledge location
       **/
      Madara::Knowledge_Record::Integer get (const ::std::string & key);

      /**
       * Sets a knowledge value to a specified value
       *
       * @param key          knowledge variable location
       * @param value        value to set at location
       * @return             0 if successful, -1 if key is null, and
       *                     -2 if quality isn't high enough
       **/
      int set (const ::std::string & key,
        Madara::Knowledge_Record::Integer value = 1);


      /**
       * Dumps all knowledge to std::err
       * @param level      level of severity
       **/
      void dump (unsigned int level = 0);

    private:
      Madara::Knowledge_Engine::Knowledge_Base knowledge_;
    };

  }
}

// include the inline functions
#include "Test_Framework.inl"

#endif   // _TEST_FRAMEWORK_H_
