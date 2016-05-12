

#ifndef _MADARA_THREADS_THREADER_H_
#define _MADARA_THREADS_THREADER_H_

/**
 * @file Threader.h
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains a threader class that launches and manages
 * threads for users
 **/

#include <string>
#include <vector>
#include <map>
#include <list>
#include "madara/knowledge/KnowledgeBase.h"
#include "BaseThread.h"
#include "WorkerThread.h"
#include "madara/MADARA_export.h"

#ifdef _MADARA_JAVA_
#include <jni.h>
#include "madara_jni.h"
#endif

namespace madara
{
  namespace threads
  {
    /**
     * Starts threads with first class support of MADARA contexts
     **/
    class MADARA_Export Threader
    {
    public:
      /**
       * Default constructor
       **/
      Threader ();

      /**
       * Constructor
       * @param  data_plane   The data plane for threads to use
       **/
      Threader (knowledge::KnowledgeBase & data_plane);

      /**
       * Destructor
       **/
      ~Threader ();

      /**
      * Modify hertz rate of a thread. This is only useful
      * for periodic threads that are operating at infinite
      * or set hertz rates. If the thread had been started
      * as a run once thread, it cannot be changed to a
      * hertz rate as the thread will be dead. In the latter
      * case, restart the thread at the new hertz rate.
      * @param name   unique thread name for the thread
      * @param hertz  new hertz rate for the periodic thread
      **/
      void change_hertz (const std::string name, double hertz);

      /**
      * Gets the control plane used by threads
      * @return  the knowledge base used by threader for control of threads
      **/
      knowledge::KnowledgeBase * get_control_plane (void);

      /**
       * Gets the data plane used by threads
       * @return  the knowledge base used by threads for data
       **/
      knowledge::KnowledgeBase * get_data_plane (void);

      /**
      * Requests a specific thread to pause
      * @param name    unique thread name for the thread.
      **/
      void pause (const std::string name);

      /**
      * Requests all threads to pause
      **/
      void pause (void);

      /**
      * Requests a specific thread to resume (unpause)
      * @param name    unique thread name for the thread.
      **/
      void resume (const std::string name);

      /**
      * Requests all threads to resume (unpause)
      **/
      void resume (void);

      /**
       * Starts a new thread and executes the provided user
       * thread once. Execution ordering is init -> execute -> cleanup.
       * 
       * <br>&nbsp;<br>The thread will be managed by the
       * threader. Please do not pass the address of a thread
       * on the stack and do not delete this memory yourself
       * or your program will crash.
       * @param name    unique thread name for the thread.
       *                If possible, try to use one word or
       *                words separated by underscores (_)
       * @param thread  user-created thread implementation
       * @param paused  create thread in a paused state.
       **/
      void run (const std::string name, BaseThread * thread,
        bool paused = false);
      
      /**
       * Starts a new thread and executes the provided user
       * thread at a hertz. Execution ordering is init -> *execute -> cleanup.
       * init and cleanup are only called once.
       * 
       * <br>&nbsp;<br>The thread will be managed by the
       * threader. Please do not pass the address of a thread
       * on the stack and do not delete this memory yourself
       * or your program will crash.
       * @param hertz   the intended hertz (frequency) that the thread's
       *                execute should be ran. Hertz is in operations per
       *                second. 0.0 means infinite hertz rate. -1 means
       *                a one shot thread.
       * @param name    unique thread name for the thread.
       *                If possible, try to use one word or
       *                words separated by underscores (_)
       * @param thread  user-created thread implementation
       * @param paused  create thread in a paused state.
       **/
      void run (double hertz, const std::string name, BaseThread * thread,
        bool paused = false);
      
#ifdef _MADARA_JAVA_
      
      /**
       * Starts a new thread and executes the provided user
       * thread once. Execution ordering is init -> execute -> cleanup.
       * 
       * <br>&nbsp;<br>The thread will be managed by the
       * threader. Please do not pass the address of a thread
       * on the stack and do not delete this memory yourself
       * or your program will crash.
       * @param name    unique thread name for the thread.
       *                If possible, try to use one word or
       *                words separated by underscores (_)
       * @param thread  user-created thread implementation
       * @param paused  create thread in a paused state.
       **/
      void run (const std::string name, jobject thread, bool paused = false);
      
      /**
       * Starts a new thread and executes the provided user
       * thread once. Execution ordering is init -> *execute -> cleanup.
       * init and cleanup are only called once.
       * 
       * <br>&nbsp;<br>The thread will be managed by the
       * threader. Please do not pass the address of a thread
       * on the stack and do not delete this memory yourself
       * or your program will crash.
       * @param hertz   the intended hertz (frequency) that the thread's
       *                execute should be ran. Hertz is in operations per
       *                second. 0.0 means infinite hertz rate. -1 means
       *                a one shot thread.
       * @param name    unique thread name for the thread.
       *                If possible, try to use one word or
       *                words separated by underscores (_)
       * @param thread  user-created thread implementation
       * @param paused  create thread in a paused state.
       **/
      void run (double hertz, const std::string name, jobject thread,
        bool paused = false);
      
#endif

      /**
      * Sets the data plane for new threads
      * @param  data_plane   The data plane for threads to use
      **/
      void set_data_plane (knowledge::KnowledgeBase & data_plane);

      /**
      * Requests a specific thread to terminate
      * @param name    unique thread name for the thread.
      **/
      void terminate (const std::string name);

      /**
      * Requests all threads to terminate
      **/
      void terminate (void);

      /**
       * Wait for a specific thread to complete
       * @param name    unique thread name for the thread
       * @param ws      wait settings for specifying period and timeout
       * @return  true if thread was terminated, false if timeout
       **/
      bool wait (const std::string name,
        const knowledge::WaitSettings & ws = knowledge::WaitSettings ());

      /**
       * Wait for all threads to complete
       * @param ws      wait settings for specifying period and timeout
       * @return        true if thread was terminated, false if timeout
       **/
      bool wait (
        const knowledge::WaitSettings & ws = knowledge::WaitSettings ());
      
    private:
      
      /**
       * The data plane used by threads
       **/
      knowledge::KnowledgeBase * data_;

      /**
       * The control plane used by threads for termination
       * and pause information. This has to be on the heap, because
       * each thread gets its own stack!
       **/
      knowledge::KnowledgeBase * control_;

      /**
       * the threads that are still active
       **/
      NamedWorkerThreads threads_;
    };
  }
}

#include "Threader.inl"

#endif  // _MADARA_THREADS_THREADER_H_
