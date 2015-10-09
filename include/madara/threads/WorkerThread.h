#ifndef _MADARA_THREADS_WORKER_THREAD_H_
#define _MADARA_THREADS_WORKER_THREAD_H_

/**
 * @file WorkerThread.h
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains the WorkerThread class, which provides a
 * multicast transport for reading knowledge updates in KaRL
 **/

#include <string>
#include <map>

#include "madara/knowledge/KnowledgeBase.h"
#include "BaseThread.h"

#include "ace/Task.h"
 
namespace madara
{
  namespace threads
  {
    class Threader;

    /**
     * @class WorkerThread
     * @brief A thread that executes BaseThread logic. Users should 
     **/
    class WorkerThread : public ACE_Task<ACE_NULL_SYNCH>
    {
    public:
      /// give access to our status flags to the Threader class
      friend class Threader;

      /**
       * Default constructor
       **/
      WorkerThread ();
      
      /**
       * Constructor
       * @param    name     the name of the user thread
       * @param    thread   the user thread
       * @param    control  the knowledge base that provides a control plane
       *                    between the data knowledge base and threads
       * @param    data     the knowledge base that provides user data access
       * @param    hertz    the hertz rate to run the thread
       **/
      WorkerThread (
        const std::string & name,
        BaseThread * thread,
        knowledge::KnowledgeBase * control,
        knowledge::KnowledgeBase * data,
        double hertz = -1.0);
      
      /**
       * Copy constructor
       * @param input  thread information to copy
       **/
      WorkerThread (const WorkerThread & input);

      /**
      * Destructor
      **/
      ~WorkerThread ();
      
      /**
       * Assignment operator
       * @param input  thread information to copy
       **/
      void operator= (const WorkerThread & input);

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
      BaseThread * thread_;

      /// the control plane to the knowledge base
      knowledge::KnowledgeBase * control_;

      /// the data plane (the knowledge base)
      knowledge::KnowledgeBase * data_;

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
    typedef std::map <std::string, WorkerThread *>  NamedWorkerThreads;
  }
}

#endif // _MADARA_THREADS_WORKER_THREAD_H_
