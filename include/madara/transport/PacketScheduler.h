

#ifndef _MADARA_PACKET_SCHEDULER_H_
#define _MADARA_PACKET_SCHEDULER_H_

/**
 * @file PacketScheduler.h
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains the PacketScheduler class, which is intended
 * to enforce user-requested packet drop rates
 **/

#include <deque>
#include <queue>
#include <time.h>

#include "madara/LockType.h"
#include "madara/utility/stdint.h"
#include "ace/Guard_T.h"
#include "ace/Recursive_Thread_Mutex.h"
#include "madara/MADARA_export.h"
#include "madara/transport/QoSTransportSettings.h"

class StrideTask;

namespace madara
{
  namespace transport
  {
    /**
     * @class PacketScheduler
     * @brief Provides scheduler for dropping packets
     **/

    class MADARA_Export PacketScheduler
    {
    public:
      /**
       * Default constructor
       **/
      PacketScheduler (const QoSTransportSettings * settings = 0);
      
      /**
       * Copy constructor
       * @param  rhs   the value to be copied into this class
       **/
      PacketScheduler (const PacketScheduler & rhs);

      /**
       * Destructor
       **/
      virtual ~PacketScheduler ();

      /**
       * Assignment operator
       * @param  rhs   the value to be copied into this class
       **/
      void operator= (const PacketScheduler & rhs);
      
      /**
       * Adds a message to the monitor
       * @return   false if the packet should be dropped, true otherwise
       **/
      bool add (void);

      /**
       * Attaches settings
       * @param   settings   Settings to attach to this scheduler
       **/
      void attach (const QoSTransportSettings * settings);

      /**
       * Queries the monitor for the current dropped messages
       * @return   current bandwidth utilization in bytes
       *           over the entire window (this is not bytes/s)
       **/
      uint64_t get_dropped (void);
      
      /**
       * Queries the monitor for the current sent messages
       * per second over the past window
       * @return   current bandwidth utilization in bytes/s
       **/
      uint64_t get_sent (void);


      /**
       * Clears the packet scheduler
       **/
      void clear (void);

      /**
       * Resets the packet scheduler to current settings
       **/
      void reset (void);

      /**
       * Prints the number of status of the packet scheduler
       * @param   log_level   the log level to print status at
       * @param   prefix      the prefix of the calling function to allow
       *                      for more descriptive and relevant status
       **/
      void print_status (unsigned int log_level = 0,
        const char * prefix = "PacketScheduler");

    protected:
      /**
       * Convenience typedef for the ACE_Guard
       **/
      typedef ACE_Guard<MADARA_LOCK_TYPE> SchedulerGuard;

      /**
       * Mutex for supporting multithreaded monitor calls
       **/
      mutable MADARA_LOCK_TYPE mutex_;
      
      /**
       * Transport settings
       **/
      const QoSTransportSettings * settings_;

      /**
       * Number of sent messages
       **/
      uint64_t  sent_messages_;

      /**
       * Number of dropped messages
       **/
      uint64_t  dropped_messages_;

      /**
       * Consecutive dropped messages
       **/
      uint64_t  consecutive_drops_;

      /**
       * queue used by stride scheduling algorithm
       **/
      std::priority_queue <StrideTask, std::vector <StrideTask> > queue_;
    };
  }
}

#endif   // _MADARA_PACKET_SCHEDULER_H_
