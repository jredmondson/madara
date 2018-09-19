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
#include "madara/knowledge/containers/Double.h"
#include "madara/utility/Utility.h"

#include <thread>


namespace madara
{
  namespace threads
  {
    class Threader;

    /**
     * @class WorkerThread
     * @brief A thread that executes BaseThread logic. Users should
     **/
    class WorkerThread
    {
    public:
      /// give access to our status flags to the Threader class
      friend class Threader;

      /**
       * Default constructor
       **/
      WorkerThread () {}

      /**
       * Constructor
       * deprecated: use version that takes control and data by value
       *
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
        double hertz = -1.0)
      : WorkerThread(name, thread, *control, *data, hertz) {}

      /**
       * Constructor
       *
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
        knowledge::KnowledgeBase control,
        knowledge::KnowledgeBase data,
        double hertz = -1.0);

      WorkerThread(
        WorkerThread &&other) = default;
      /**
      * Destructor
      **/
      ~WorkerThread () noexcept;

      /**
       * Assignment operator
       * @param input  thread information to copy
       **/
      //void operator= (const WorkerThread & input);

    protected:
      std::thread me_;

      /**
      * Task loop
      **/
      int svc (void);

      /**
       * Starts the thread, with entry point svc()
       **/
      void run (void);

      /**
       * Changes the frequency given a hertz rate
       * @param  hertz      the new hertz rate
       * @param  current    current time
       * @param  frequency  an updated frequency value
       * @param  next_epoch next time to trigger execution
       * @param  one_shot   if hertz rate is less than 0, true.
       *                    Otherwise, false
       * @param  blaster    if hertz rate is 0, true. Otherwise, false.
       **/
      void change_frequency (double hertz,
        utility::TimeValue & current, utility::Duration & frequency,
        utility::TimeValue & next_epoch,
        bool & one_shot, bool & blaster);

      /// the name of the contained thread
      std::string name_;

      /// the contained thread
      std::unique_ptr<BaseThread> thread_ = nullptr;

      /// the control plane to the knowledge base
      knowledge::KnowledgeBase control_;

      /// the data plane (the knowledge base)
      knowledge::KnowledgeBase data_;

      /**
       * thread safe finished flag that will be sent to the knowledge
       * base on completion of the thread
       **/
      knowledge::containers::Integer finished_;

      /**
       * thread safe start flag that will be sent to the knowledge
       * base on launch of the thread
       **/
      knowledge::containers::Integer started_;

      /**
       * thread safe hertz reference
       **/
      knowledge::containers::Double new_hertz_;

      /**
       * thread safe start flag that will be sent to the knowledge
       * base on launch of the thread
       **/
      knowledge::containers::Integer executions_;

      /**
       * timestamp of the initial svc start
       **/
      knowledge::containers::Integer start_time_;

      /**
       * timestamp of the last svc start
       **/
      knowledge::containers::Integer last_start_time_;

      /**
       * timestamp of the last svc end
       **/
      knowledge::containers::Integer end_time_;

      /**
       * hertz rate for worker thread executions
       **/
      double hertz_ = -1;
    };

    /**
     * Collection of named threads
     **/
    typedef std::map <std::string, std::unique_ptr<WorkerThread>>  NamedWorkerThreads;
  }
}

#include "WorkerThread.inl"

#endif // _MADARA_THREADS_WORKER_THREAD_H_
