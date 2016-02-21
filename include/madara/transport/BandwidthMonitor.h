

#ifndef _MADARA_BANDWIDTH_MONITOR_H
#define _MADARA_BANDWIDTH_MONITOR_H

/**
 * @file BandwidthMonitor.h
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains the BandwidthMonitor class, which is intended
 * to monitor bandwidth utilization of a transport
 **/

#include <deque>
#include <time.h>

#include "madara/LockType.h"
#include "madara/utility/stdint.h"
#include "ace/Guard_T.h"
#include "ace/Recursive_Thread_Mutex.h"
#include "madara/MADARA_export.h"

namespace madara
{
  namespace transport
  {
    typedef  std::pair <time_t, uint64_t>   BandwidthRecord;
    typedef  std::deque <BandwidthRecord>  BandwidthMessages;

    /**
     * @class BandwidthMonitor
     * @brief Provides monitoring capability of a transport's bandwidth
     **/

    class MADARA_Export BandwidthMonitor
    {
    public:
      /**
       * Default constructor
       * @param   window_in_secs   Time window to measure bandwidth usage
       **/
      BandwidthMonitor (time_t window_in_secs = 10);
      
      /**
       * Copy constructor
       * @param  rhs   the value to be copied into this class
       **/
      BandwidthMonitor (const BandwidthMonitor & rhs);

      /**
       * Destructor
       **/
      virtual ~BandwidthMonitor ();

      /**
       * Assignment operator
       * @param  rhs   the value to be copied into this class
       **/
      void operator= (const BandwidthMonitor & rhs);
      
      /**
       * Adds a message to the monitor
       * @param   size       the size of the message
       **/
      void add (uint64_t size);

      /**
       * Adds a message to the monitor
       * @param   timestamp  the time the message occured
       * @param   size       the size of the message
       **/
      void add (time_t timestamp, uint64_t size);

      /**
       * Checks send and receive bandwidth against send and receive limits
       * @param  limit  if -1, does not check. If positive, checks
       *                against current usage
       **/
      bool is_bandwidth_violated (int64_t limit);

      /**
       * Sets the window in seconds to measure bandwidth
       * @param   window_in_secs   Time window to measure bandwidth usage
       **/
      void set_window (time_t window_in_secs);

      /**
       * Queries the monitor for the current bandwidth utilization
       * @return   current bandwidth utilization in bytes
       *           over the entire window (this is not bytes/s)
       **/
      uint64_t get_utilization (void);
      
      /**
       * Queries the monitor for the current bandwidth utilization
       * per second over the past window
       * @return   current bandwidth utilization in bytes/s
       **/
      uint64_t get_bytes_per_second (void);

      /**
       * Clears the bandwidth monitor
       **/
      void clear (void);

      /**
       * Prints the number of messages and utilization within the past window
       **/
      void print_utilization (void);

      /**
       * Returns the number of messages in the past window
       * @return  the number of types that have filters
       **/
      size_t get_number_of_messages (void);

    protected:
      /**
       * Updates utilization for most public functions
       * @return   current time
       **/
      time_t update_utilization (void);

      /**
       * Mutex for supporting multithreaded monitor calls
       **/
      mutable MADARA_LOCK_TYPE mutex_;

      /**
       * Map of timestamps to messages
       **/
      BandwidthMessages  messages_;

      /**
       * Utilization
       **/
      uint64_t utilization_;

      /**
       * Time window for useful messages to bandwidth calculations
       **/
      time_t window_;
    };
  }
}

#include "BandwidthMonitor.inl"

#endif   // _MADARA_BANDWIDTH_MONITOR_H
