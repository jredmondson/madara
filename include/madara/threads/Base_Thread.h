

#ifndef _MADARA_THREADS_BASE_THREAD_H_
#define _MADARA_THREADS_BASE_THREAD_H_

/**
 * @file Base_Thread.h
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains a base thread implementation for
 * use when running MADARA threads
 **/

#include <string>
#include <vector>
#include <map>
#include <list>
#include "madara/knowledge_engine/Knowledge_Record.h"
#include "madara/knowledge_engine/Functions.h"
#include "madara/utility/stdint.h"
#include "madara/MADARA_export.h"
#include "madara/knowledge_engine/Knowledge_Base.h"
#include "madara/knowledge_engine/containers/Integer.h"

namespace Madara
{
  namespace Threads
  {
    /**
     * Forward declare some classes
     **/
    class Worker_Thread;
    class Threader;

    /**
     * Abstract base class for implementing threads
     **/
    class Base_Thread
    {
    public:
      /**
       * Worker threads are MADARA thread entities that manage
       * user-defined threads
       **/
      friend class Worker_Thread;
      friend class Threader;

      /**
       * Initializes thread with a MADARA context
       **/
      virtual void init (Knowledge_Engine::Knowledge_Base &)
      {
      }

      /**
       * Executes the main thread logic. In a Base_Thread, the
       * execute is called once. Afterwards, cleanup is called.
       **/
      virtual void run (void) = 0;
      
      /**
       * Cleans up any thread residue (usually instances created in init).
       * It's important to note that the only things that would need to
       * be cleaned up are generally ports opened, memory initialized with
       * new/malloc, etc. Anything MADARA related is generally cleaned up
       * for you.
       **/
      virtual void cleanup (void)
      {
      }

    protected:
      
      /**
       * Initializes the Java thread implementation's control plane variables
       * @param name the unique name for the thread
       * @param control the control plane between Threader and threads
       **/
      virtual void init_control_vars (
        Knowledge_Engine::Knowledge_Base & control)
      {
        terminated.set_name (name + ".terminated", control);
        paused.set_name (name + ".paused", control);
      }

      /**
       * The unique name of your thread
       **/
      std::string name;

      /**
       * thread safe terminated flag that may be set by the Threader
       * base
       **/
      Madara::Knowledge_Engine::Containers::Integer terminated;

      /**
       * thread safe paused flag that may be set by the Threader
       **/
      Madara::Knowledge_Engine::Containers::Integer paused;
    };
  }
}

#endif  // _MADARA_THREADS_BASE_THREAD_H_
