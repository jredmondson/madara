#ifndef _MADARA_TRANSPORT_SETTINGS_H_
#define _MADARA_TRANSPORT_SETTINGS_H_

/**
 * @file Transport.h
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains the Transport::Base class, which provides an
 * extensible transport layer for sending knowledge updates in KaRL.
 * To support knowledge updates, only the send_multiassignment method
 * is currently required to be extended as the set, evaluate, and wait
 * methods all call send_multiassignment. For example transport, @see
 * Madara::Transport::Multicast_Transport.
 **/

#include <string>
#include <sstream>
#include <vector>
#include <ostream>
#include "ace/Thread_Mutex.h"
#include "ace/Recursive_Thread_Mutex.h"
#include "ace/Condition_T.h"
#include "ace/Guard_T.h"
#include "madara/utility/Log_Macros.h"
#include "ace/High_Res_Timer.h"

#ifdef _USE_CID_

#include "madara/cid/CID_Settings.h"
#include "madara/cid/CID_Convenience.h"
#include "madara/cid/CID_Heuristic.h"
#include "madara/cid/CID_Genetic.h"
#include "madara/utility/Utility.h"

#endif // _USE_CID_

#include "madara/Lock_Type.h"
#include "madara/knowledge_engine/Knowledge_Record.h"
#include "madara/knowledge_engine/Thread_Safe_Context.h"
#include "madara/expression_tree/Expression_Tree.h"
#include "madara/expression_tree/Interpreter.h"
#include "madara/MADARA_export.h"
#include "madara/transport/Fragmentation.h"

namespace Madara
{
  namespace Transport
  {
    typedef   ACE_High_Res_Timer                   Timer;
    typedef   std::vector<Timer>                   Timers;

#ifdef _USE_CID_
    typedef   Madara::Cid::Settings                Latency_Settings;
#endif

    enum Types {
      NO_TRANSPORT = 0,
      SPLICE       = 1,
      NDDS         = 2,
      TCP          = 3,
      UDP          = 4,
      MULTICAST    = 5,
      BROADCAST    = 6,
    };

    enum Reliabilities {
      BEST_EFFORT = 0,
      RELIABLE = 1
    };

    enum Messages {
      ASSIGN = 0,
      OPERATION = 1,
      MULTIASSIGN = 2,
      LATENCY = 10,
      LATENCY_AGGREGATE = 11,
      LATENCY_SUMMATION = 12,
      VOTE = 20
    };

    /**
     * Holds Transport Settings
     */
    class MADARA_Export Settings
    {
    public:
      // for ease-of-use, typedef the templated guard
      typedef ACE_Guard <MADARA_LOCK_TYPE> Context_Guard;

      /// Default knowledge domain
      #define DEFAULT_DOMAIN      "KaRL"

      typedef std::vector <std::string>    Voters;

      /// Default queue length for event history (must be high for
      /// reliable transport
      static const uint32_t DEFAULT_QUEUE_LENGTH = 5000;

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

#ifdef _USE_CID_

      /**
       * Latency collection is disabled by default
       **/
      #define DEFAULT_LATENCY_ENABLED   false

      /**
       * Set default latency timeout to 10 seconds
       **/
      #define DEFAULT_LATENCY_TIMEOUT   10.0

      /**
       * Set default latency to 200 seconds
       **/
      #define DEFAULT_LATENCY          200000000000

      /**
       * Set default latency to 200 seconds
       **/
      #define DEFAULT_REDEPLOYMENT_PERCENTAGE     0.10

#endif // _USE_CID_

      /// Constructor for this class
      Settings ();

      /// Copy constructor
      Settings (const Settings & settings);

      virtual ~Settings ();

      /// Assignment operator
      void operator= (const Settings & settings);
      
#ifdef _USE_CID_
      /**
       * Resets timers and latencies
       **/
      inline void reset_timers (void)
      {
        Context_Guard guard (mutex);

        for (uint32_t i = 0; i < processes; ++i)
          timers[i].reset ();
        Madara::Cid::reset_latencies (latencies, latency_default);
      }

      /**
       * Starts all latency timers and resets timers
       **/
      inline void start_all_timers (void)
      {
        Context_Guard guard (mutex);

        reset_timers ();
        
        for (unsigned int i = 0; i < processes; ++i)
          timers[i].start ();
      }

      /**
       * Stops a timer and records latency
       **/
      inline void stop_timer (int index, bool roundtrip = true)
      {
        ACE_hrtime_t measured;
        Context_Guard guard (mutex);

        timers[index].stop ();
        timers[index].elapsed_time (measured);

        latencies.network_latencies[id][index].second = roundtrip ?
          (uint64_t) measured / 2 : (uint64_t) measured;
      }

      /**
       * Allocates latency gathering data structures if necesssary. Called
       * by transports. Users shouldn't need to call this.
       **/
      inline void setup (void)
      {
        Context_Guard guard (mutex);

        if (latency_enabled)
        {
          Madara::Cid::init (processes, latencies);
          timers.resize (processes);

          Madara::Cid::reset_latencies (latencies, latency_default);
        }
      }

      /**
       * Allocates algorithm configurations and results inside of the
       * CID latency settings
       **/
      inline void setup (Madara::Cid::Algorithm_Configs & configs)
      {
        Context_Guard guard (mutex);

        latencies.algorithm_configs = configs;
        latencies.results.resize (configs.size ());
      }

      /**
       * Prints all latencies from this id in the format id -> latency
       **/
      void print_my_latencies (std::ostream & output)
      {
        // we do not use a guard here because we want to do I/O operations
        // outside of the mutex.
        std::stringstream buffer;

        mutex.acquire ();
        Madara::Cid::Identifiers & ids = latencies.ids;
        Madara::Cid::Latency_Vector & current = latencies.network_latencies[id];

        buffer << "Latencies for id = " << id << std::endl;

        // print each id -> latency
        for (uint32_t i = 0; i < processes; ++i)
        {
          buffer << ids[i] << " = " << current[i].second << std::endl;
        }

        mutex.release ();

        output << buffer.str ();
      }

      /**
       * Prints all latencies from this id in the format id -> latency
       **/
      void print_all_latencies (std::ostream & output)
      {
        // we do not use a guard here because we want to do I/O operations
        // outside of the mutex.
        std::stringstream buffer;

        mutex.acquire ();
        Madara::Cid::Identifiers & ids = latencies.ids;

        buffer << "\nAll latencies in the context:\n\n";

        // print each id -> latency
        for (uint32_t i = 0; i < processes; ++i)
        {
          Madara::Cid::Latency_Vector & current = latencies.network_latencies[i];
          for (uint32_t j = 0; j < processes; ++j)
          {
            buffer << ids[i] << " to " << ids[j] << 
                      " = " << current[j].second << std::endl;
          }
        }

        mutex.release ();

        // print the buffer
        output << buffer.str ();
      }

      /**
       * Prints all summations in the format id -> latency
       **/
      void print_all_summations (std::ostream & output)
      {
        // we do not use a guard here because we want to do I/O operations
        // outside of the mutex.
        std::stringstream buffer;

        mutex.acquire ();
        Madara::Cid::Identifiers & ids = latencies.ids;
        Madara::Cid::Summations_Map & summation_map =
          latencies.network_summations;

        buffer << "\nAll summations in the context:\n\n";

        // print each id -> latency
        for (Madara::Cid::Summations_Map::iterator i = summation_map.begin ();
          i != summation_map.end (); ++i)
        {
          buffer << "Degree = " << i->first << std::endl;
          Madara::Cid::Latency_Vector & current = i->second;

          // temporary, since we can't figure out why the summations aren't
          // sorting in the read thread.
          std::sort (current.begin (), current.end (),
            Madara::Cid::Increasing_Latency);

          for (Madara::Cid::Latency_Vector::size_type j = 0;
            j < current.size (); ++j)
          {
            buffer << "  " << current[j].first << " = " << 
                      current[j].second << "\n";
          }
        }

        mutex.release ();

        // print the buffer
        output << buffer.str ();
      }

      /**
       * Prints all algorithm results
       **/
      void print_all_results (std::ostream & output)
      {
        // we do not use a guard here because we want to do I/O operations
        // outside of the mutex.
        std::stringstream buffer;

        mutex.acquire ();
        Madara::Cid::Identifiers & ids = latencies.ids;
        Madara::Cid::Algorithm_Configs & configs = latencies.algorithm_configs;
        Madara::Cid::Algorithm_Results & results = latencies.results;

        buffer << "\nAll redeployment algorithm results in the context:\n\n";

        // print each id -> latency
        for (Madara::Cid::Algorithm_Configs::size_type i = 0;
             i < configs.size (); ++i)
        {
          if (     results[i].algorithm == Madara::Cid::CID)
            buffer << "CID,";
          else if (results[i].algorithm == Madara::Cid::BCID)
            buffer << "BCID,";
          else if (results[i].algorithm == Madara::Cid::BCID_GGA)
            buffer << "BCID-GGA (" << configs[i].time << "),";
          else if (results[i].algorithm == Madara::Cid::BCID_BGA)
            buffer << "BCID-BGA (" << configs[i].time << "),";
          else if (results[i].algorithm == Madara::Cid::CID_BGA)
            buffer << "CID-BGA (" << configs[i].time << "),";
          else if (results[i].algorithm == Madara::Cid::CID_GGA)
            buffer << "CID-GGA (" << configs[i].time << "),";

          buffer << results[i].latency << "\n  ";
          buffer << results[i].deployment << "\n";
        }

        mutex.release ();

        buffer << "\n";

        // print the buffer
        output << buffer.str ();
      }

      /**
       * Aggregates all latencies from this id in the format id=latency;
       * @return  aggregation of all latencies to this id
       **/
      std::string aggregate_latencies (void)
      {
        std::stringstream buffer;
        Context_Guard guard (mutex);
        Madara::Cid::Latency_Vector & current = latencies.network_latencies[id];

        // print each id -> latency
        for (uint32_t i = 0; i < processes; ++i)
        {
          buffer << i << "=" << current[i].second << ";";
        }

        return buffer.str ();
      }

      /**
       * Un-aggregates all latencies from this id in the format id=latency;
       * @param     source        the id of the process to update
       * @param     aggregation   the aggregation of latencies
       **/
      void unaggregate_latencies (uint32_t source, 
        const std::string & aggregation)
      {
        std::stringstream stream (aggregation);
        Context_Guard guard (mutex);
        Madara::Cid::Latency_Vector & current = latencies.network_latencies[source];

        // key symbol value symbol
        // 0 = 15     or 24 = 13847169741, for instance
        char symbol;
        unsigned int key;
        uint64_t value;

        for (unsigned int i = 0; !stream.eof (); ++i)
        {
          stream >> key >> symbol >> value >> symbol;

          // make a quick check to see if these values are indeed useful
          if (i < processes)
          {
            current[i].first = key;
            current[i].second = value;
          }
        }
      }

      /**
       * Packs all latency data into summations based on workflow degrees
       **/
      std::string pack_summations (void)
      {
        Context_Guard guard (mutex);

        return Madara::Cid::prepare_summations (id, latencies);
      }

      /**
       * Unpacks all latency summaries from this id in the format id=latency;
       * @param     source        the id of the process to update
       * @param     summations   the aggregation of latencies
       **/
      void unpack_summations (uint32_t source, 
        const std::string & summations)
      {
        std::stringstream stream (summations);
        Context_Guard guard (mutex);
        Madara::Cid::Latency_Vector & current =
          latencies.network_latencies[source];
        Madara::Cid::Summations_Map & summations_map =
          latencies.network_summations;

        // key symbol value symbol
        // 0 = 15     or 24 = 13847169741, for instance
        char symbol;
        unsigned int key;
        uint64_t value;

        while (!stream.eof ())
        {
          stream >> key >> symbol >> value >> symbol;

          Madara::Cid::Latency_Vector & cur_summation = summations_map[key];

          if (cur_summation.size () == processes)
          {
            // if the array had already been created, sort by Id
            std::sort (cur_summation.begin (), cur_summation.end (),
              Madara::Cid::Increasing_Id);
          }
          else
          {
            // if the size of this entry is 0, create a new processes-length
            // summation for this degree, and set each element to the
            // corresponding id of that process (essentially sorting by Id)
            cur_summation.resize (processes);
            for (unsigned int i = 0; i < processes; ++i)
              cur_summation[i].first = i;
          }

          // make a quick check to see if these values are indeed useful
          if (source < processes && cur_summation[source].second != value)
          {
            cur_summation[source].second = value;
          }
        }
      }

      /**
       * Reads a user provided dataflow that the Madara framework
       * should be approximating
       **/
      void read_dataflow (const std::string & filename)
      {
        Context_Guard guard (mutex);
        latencies.network_summations.clear ();
        Madara::Cid::read_deployment (latencies,
          Madara::Utility::clean_dir_name (filename));
      }

      void read_solution (const std::string & source,
        const std::string & my_host_port)
      {
        Context_Guard guard (mutex);
        unsigned int id;
        std::string host_port;

        std::vector <std::string> tokens;
        std::vector <std::string> splitters;
        std::vector <std::string> pivot_list;

        tokens.resize (2);
        tokens[0] = ";";
        tokens[1] = "=";

        Madara::Utility::tokenizer (source, splitters, tokens, pivot_list);

        for (std::vector <std::string>::size_type i = 0;
          i + 1 < tokens.size (); i += 2)
        {
          std::stringstream buffer (tokens[i]);
          buffer >> id;

          latencies.ids[id] = tokens[i + 1];
        }
      }

      /**
       * Run CID
       * @param    index     index into the Algorithm Results array
       **/
      void run_cid (unsigned int index)
      {
        Context_Guard guard (mutex);

        latencies.solution_lookup.clear ();
        Madara::Cid::reset_solution (latencies);

        Madara::Cid::approximate (latencies);
        
        latencies.results[index].algorithm = Madara::Cid::CID;
        latencies.results[index].deployment = Madara::Cid::stringify_solution (latencies);
        latencies.results[index].latency =
          Madara::Cid::calculate_latency (latencies);
      }

      /**
       * Run Blind CID
       * @param    index     index into the Algorithm Results array
       **/
      void run_bcid (unsigned int index)
      {
        Context_Guard guard (mutex);

        latencies.solution_lookup.clear ();
        Madara::Cid::reset_solution (latencies);

        Madara::Cid::fill_by_highest_degree (latencies);
        
        latencies.results[index].algorithm = Madara::Cid::BCID;
        latencies.results[index].deployment =
          Madara::Cid::stringify_solution (latencies);
        latencies.results[index].latency =
          Madara::Cid::calculate_latency (latencies);
      }

      /**
       * Run BCID BGA
       * @param    index     index into the Algorithm Results array
       **/
      void run_bcid_bga (unsigned int index)
      {
        Context_Guard guard (mutex);

        latencies.solution_lookup.clear ();
        Madara::Cid::reset_solution (latencies);

        Madara::Cid::fill_by_highest_degree (latencies);
        Madara::Cid::ga_naive (latencies, 
          latencies.algorithm_configs[index].time);

        latencies.results[index].algorithm = Madara::Cid::BCID_BGA;
        latencies.results[index].deployment =
          Madara::Cid::stringify_solution (latencies);
        latencies.results[index].latency =
          Madara::Cid::calculate_latency (latencies);
      }

      /**
       * Run BCID GGA
       * @param    index     index into the Algorithm Results array
       **/
      void run_bcid_gga (unsigned int index)
      {
        Context_Guard guard (mutex);

        latencies.solution_lookup.clear ();
        Madara::Cid::reset_solution (latencies);

        Madara::Cid::fill_by_highest_degree (latencies);
        Madara::Cid::ga_degree (latencies, 
          latencies.algorithm_configs[index].time);

        latencies.results[index].algorithm = Madara::Cid::BCID_GGA;
        latencies.results[index].deployment =
          Madara::Cid::stringify_solution (latencies);
        latencies.results[index].latency =
          Madara::Cid::calculate_latency (latencies);
      }

      /**
       * Run CID BGA
       * @param    index     index into the Algorithm Results array
       **/
      void run_cid_bga (unsigned int index)
      {
        Context_Guard guard (mutex);

        latencies.solution_lookup.clear ();
        Madara::Cid::reset_solution (latencies);

        Madara::Cid::approximate (latencies);
        Madara::Cid::ga_naive(latencies, 
          latencies.algorithm_configs[index].time);

        latencies.results[index].algorithm = Madara::Cid::CID_BGA;
        latencies.results[index].deployment =
          Madara::Cid::stringify_solution (latencies);
        latencies.results[index].latency =
          Madara::Cid::calculate_latency (latencies);
      }

      /**
       * Run CID GGA
       * @param    index     index into the Algorithm Results array
       **/
      void run_cid_gga (unsigned int index)
      {
        Context_Guard guard (mutex);

        latencies.solution_lookup.clear ();
        Madara::Cid::reset_solution (latencies);

        Madara::Cid::approximate (latencies);
        Madara::Cid::ga_degree(latencies, 
          latencies.algorithm_configs[index].time);

        latencies.results[index].algorithm = Madara::Cid::CID_GGA;
        latencies.results[index].deployment =
          Madara::Cid::stringify_solution (latencies);
        latencies.results[index].latency =
          Madara::Cid::calculate_latency (latencies);
      }

      /**
       * Run an algorithm
       * @param    index     index into the Algorithm Config array
       **/
      void run (unsigned int index)
      {
        int algorithm = latencies.algorithm_configs[index].algorithm;
        if (     algorithm == Madara::Cid::CID)
          run_cid (index);
        else if (algorithm == Madara::Cid::BCID)
          run_bcid (index);
        else if (algorithm == Madara::Cid::BCID_GGA)
          run_bcid_gga (index);
        else if (algorithm == Madara::Cid::BCID_BGA)
          run_bcid_bga (index);
        else if (algorithm == Madara::Cid::CID_BGA)
          run_cid_bga (index);
        else if (algorithm == Madara::Cid::CID_GGA)
          run_cid_gga (index);
      }

      /**
       * Runs all algorithms
       **/
      void run_all (void)
      {
        for (unsigned int i = 0; i < latencies.algorithm_configs.size (); ++i)
          run (i);

        std::sort (latencies.results.begin (), latencies.results.end (),
          Madara::Cid::Increasing_Algorithm_Latency);
      }

#endif //  _USE_CID_

      /// All class members are accessible to users for easy setup

      /// Domains should be separated by commas
      std::string domains;

      /// the number of read threads to start
      uint32_t read_threads;

      /// Length of the buffer used to store history of events
      uint32_t queue_length;

      /// Deadline for sessions with any other reasoning entity
      uint32_t deadline;

      /// Type of transport. See Madara::Transport::Types for options
      uint32_t type;

      /// Maximum allowed fragment size for partitioning large messages
      uint32_t max_fragment_size;

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
      /// See Madara::Transport::Reliabilities for options
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
      mutable Originator_Fragment_Map fragment_map;

      /// time to sleep between sends and rebroadcasts
      double slack_time;

      /**
       * number of valid messages allowed to be received per second. This
       * value can be -1 or 0.0 to go as fast as possible
       **/
      double read_thread_hertz;

#ifdef _USE_CID_
      /// should we try to gather latencies?
      bool latency_enabled;

      /// maximum time allowed before starting another latency round
      double latency_timeout;

      /// default value for latency times in nanoseconds. This is the value
      /// recorded if a process is unreachable.
      uint64_t latency_default;

      /// mutex used for mutating latencies or timers
      MADARA_LOCK_TYPE mutex;

      /// latency information
      Latency_Settings latencies;

      /// timers used to establish latency
      Timers timers;

      /// number of responses received so far
      uint32_t num_responses;

      /// number of summations received so far
      uint32_t num_summations;

      /// number of voters
      uint32_t num_voters;

      /// number of votes received
      uint32_t num_votes_received;

      /// percentage allowed to be off before forced redeployment
      double  redeployment_percentage_allowed;

      /// a list of voters
      Voters voters;
#endif  // _USE_CID_

      /**
       * Host information for transports that require it. The format of these
       * is transport specific, but for UDP, you might have "localhost:1234"
       * for a host named localhost and a port of 1234. See the specific
       * transport for more information.
       **/
      std::vector <std::string> hosts;
    };
  }
}

#endif // _MADARA_TRANSPORT_SETTINGS_H_