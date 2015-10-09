
#ifndef _MADARA_TIMED_EXECUTOR_H_
#define _MADARA_TIMED_EXECUTOR_H_

#include <vector>
#include <map>
#include <string>
#include <utility>
#include <queue>
#include "madara/LockType.h"
#include "madara/knowledge/KnowledgeBase.h"
#include "madara/knowledge/EvalSettings.h"
#include "madara/expression/ComponentNode.h"

#include "ace/High_Res_Timer.h"
#include "ace/Barrier.h"
#include "ace/Mutex.h"
#include "ace/Thread_Mutex.h"

/**
 * @file TimedExecutor.h
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains C++ objects that facilitate timed event
 * execution.
 **/

namespace madara
{
  namespace knowledge
  {
    /**
     * @class Event
     * @brief Expression container for timed event
     */
    struct Event
    {
      /// delay before first execution
      ACE_Time_Value delay;

      /// time between executions in seconds
      ACE_Time_Value period;
      
#ifndef _MADARA_NO_KARL_
      /// expression to be executed (rooted Tree)
      expression::ComponentNode * root;
#endif // _MADARA_NO_KARL_

      /**
       * knowledge base for executing the expression
       * (should be same as compiled)
       **/
      KnowledgeBase * knowledge;

      /// executions so far
      unsigned int executions;

      /// intended executions
      int intended_executions;

      /// cancel on false return value
      bool cancel_on_false;
    };

    /// forward declare the struct
    struct TimedEventThreadInfo;

    /// forward declare the event threads
    class TimedEventThread;

    typedef std::pair <ACE_Time_Value, Event *> TimedEvent;

    /**
     * @class TimedExecutor
     * @brief This class executes expression events at specified times
     */
    class MADARA_Export TimedExecutor
    {
    public:
      /**
       * Default constructor
       **/
      TimedExecutor ();
      
      /**
       * Destructor
       **/
      ~TimedExecutor ();

      /**
       * Adds a timed event to the queue
       * @param  new_event   new event to add to the queue
       **/
      void add (const TimedEvent & new_event);
      
      /**
       * Adds an event to the queue
       * @param  new_event   new event to add to the queue.
       **/
      void add (const Event & new_event);

      /**
       * Removes a timed event from the queue
       * @param  cur_event a placeholder for the current highest priority event
       * @return time until next event time out (useful when the cur_event
       *         has a zeroed event (the 2nd part of the cur_event pair)
       **/
      ACE_Time_Value remove (TimedEvent & cur_event);

      /**
       * Informs threads of a shutdown and waits for threads to exit
       **/
      void shutdown (void);

      /**
       * Checks if the thread pool has been shutdown
       * @return true if shutdown has been called. False otherwise.
       **/
      bool is_shutdown (void);

      /**
       * Returns the time 
       **/
      ACE_Time_Value time_until_next (void);
      
      /**
       * Launches threads
       * @param threads  number of threads
       **/
      void launch_threads (unsigned int threads);

      /**
       * Enters the barrier
       **/
      void enter_barrier (void);
      
      /**
       * Clears the event queue. Note that this only clears the
       * queue. If there are events currently being processed in timed
       * event threads, they may be added to the queue once they are 
       * completed. Consequently, clear_queue is usually only completely
       * effective if shutdown has been called previously.
       **/
      void clear_queue (void);

      /**
       * Returns the number of threads
       * @return the number of threads currently running
       **/
      KnowledgeRecord::Integer num_threads (void);

    private:
      

      /// guard for access and changes
      typedef ACE_Guard<MADARA_LOCK_TYPE> Guard;
      
      /// priority queue typedef
      typedef std::priority_queue <TimedEvent, std::vector <TimedEvent> >
        PriorityQueue;

      /**
       * Locks the context
       **/
      void lock (void);

      /**
       * Unlocks the event queue
       **/
      void unlock (void);

      /**
       * Mutex for local changes
       **/
      mutable MADARA_LOCK_TYPE mutex_;
      
      /**
       * Event queue
       **/
      PriorityQueue events_;

      /**
       * Vector of thread info
       **/
      TimedEventThreadInfo * thread_info_;

      /**
       * Timed Event Threads
       **/
      TimedEventThread ** threads_;

      /**
       * Number of thread
       **/
      KnowledgeRecord::Integer num_threads_;

      /**
       * For barrier and other information
       **/
      KnowledgeBase control_plane_;

      /**
       * Reference to the number of threads in the control plane
       **/
      VariableReference threads_ref_;

      /**
       * Reference to queue size in the control plane
       **/
      VariableReference queue_size_;

      /**
       * Reference to terminated condition within control plane
       **/
      VariableReference terminated_;
    };

    /**
     * A struct that is passed to Timed Event Thread Executors
     * to control job handling, shutdowns, etc.
     **/
    struct TimedEventThreadInfo
    {
      /**
       * pointer to TimedExecutor caller, which provides thread-safe callbacks
       * for shutdown information
       **/
      TimedExecutor * executor;
      
      /**
       * For barrier and other information
       **/
      KnowledgeBase * control_plane;

      /**
       * thread identifier
       **/
      unsigned int id;
    };

    /**
     * Fills an event struct. It is highly recommended that you use this
     * function.
     * @param knowledge  the knowledge base to reference and execute in
     * @param expression KaRL expression to execute
     * @param delay      time, in seconds, before executing
     * @param period     time, in seconds, between executions
     * @param executions number of executions. -1 means infinite executions
     *                   until a shutdown is called on the timed executor
     **/
    MADARA_Export Event fill_event (
      KnowledgeBase & knowledge,
      const std::string & expression,
      double delay = 0.0, double period = 0.0, int executions = -1);
  }
}

/**
  * Comparison for higher event priorities
  * @param lhs  left hand side of the check
  * @param rhs  right hand side of the check
  * @return true if lhs timer is greater than rhs, false otherwise.
  **/
bool operator< (
  const madara::knowledge::TimedEvent & lhs,
  const madara::knowledge::TimedEvent & rhs);



#endif // _MADARA_TIMED_EXECUTOR_H_
