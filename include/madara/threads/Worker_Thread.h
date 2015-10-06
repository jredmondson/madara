#ifndef _MADARA_THREADS_WORKER_THREAD_H_
#define _MADARA_THREADS_WORKER_THREAD_H_

/**
 * @file Worker_Thread.h
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains the Worker_Thread class, which provides a
 * multicast transport for reading knowledge updates in KaRL
 **/

#include <string>
#include <map>

#include "madara/knowledge/Knowledge_Base.h"
#include "Base_Thread.h"

#include "ace/Task.h"
 
namespace madara
{
  namespace threads
  {
    class Threader;

    /**
     * @class Worker_Thread
     * @brief A thread that executes Base_Thread logic. Users should 
     **/
    class Worker_Thread : public ACE_Task<ACE_NULL_SYNCH>
    {
    public:
      /// give access to our status flags to the Threader class
      friend class Threader;

      /**
       * Default constructor
       **/
      Worker_Thread ();
      
      /**
       * Constructor
       * @param    name     the name of the user thread
       * @param    thread   the user thread
       * @param    control  the knowledge base that provides a control plane
       *                    between the data knowledge base and threads
       * @param    data     the knowledge base that provides user data access
       * @param    hertz    the hertz rate to run the thread
       **/
      Worker_Thread (
        const std::string & name,
        Base_Thread * thread,
        knowledge::Knowledge_Base * control,
        knowledge::Knowledge_Base * data,
        double hertz = -1.0);
      
      /**
       * Copy constructor
       * @param input  thread information to copy
       **/
      Worker_Thread (const Worker_Thread & input);

      /**
      * Destructor
      **/
      ~Worker_Thread ();
      
      /**
       * Assignment operator
       * @param input  thread information to copy
       **/
      void operator= (const Worker_Thread & input);

      /**
      * Reads messages from a socket
      **/
      int svc (void);
      
      /**
       * Runs the thread once
       **/
      void run (void);
      
    protected:
      /// the name of the contained thread
      std::string name_;

      /// the contained thread
      Base_Thread * thread_;

      /// the control plane to the knowledge base
      knowledge::Knowledge_Base * control_;

      /// the data plane (the knowledge base)
      knowledge::Knowledge_Base * data_;

      /**
       * thread safe finished flag that will be sent to the knowledge
       * base on completion of the thread
       **/
      madara::knowledge::containers::Integer finished_;
      
      /**
       * thread safe start flag that will be sent to the knowledge
       * base on launch of the thread
       **/
      madara::knowledge::containers::Integer started_;

      /**
       * hertz rate for worker thread executions
       **/
      double hertz_;
    };

    /**
     * Collection of named threads
     **/
    typedef std::map <std::string, Worker_Thread *>  Named_Worker_Threads;
  }
}

#endif // _MADARA_THREADS_WORKER_THREAD_H_
