

#ifndef _MADARA_THREADS_WORK_UNIT_H_
#define _MADARA_THREADS_WORK_UNIT_H_

/**
 * @file AggregateFilter.h
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains a filter functor for aggregate records
 **/

#include <string>
#include <vector>
#include <map>
#include <list>
#include "madara/knowledge/KnowledgeRecord.h"
#include "madara/knowledge/Functions.h"
#include "madara/utility/stdint.h"
#include "madara/MADARA_export.h"
#include "madara/knowledge/ExternFunctionVariables.h"

namespace madara
{
  namespace threads
  {
    /**
     * Abstract base class for implementing base threads
     **/
    class WorkUnit
    {
    public:
      /**
       * Default constructor
       **/
      WorkUnit ()
        : priority (0), critical (false)
      {

      }

      /**
       * Initializes thread with MADARA context
       * @param   vars   context for querying current program state
       **/
      virtual void init (knowledge::Variables & vars)
      {

      }

      /**
       * Executes the main thread logic
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

    private:

      /**
       * Task priority
       **/
      int priority;

      /**
       * Criticality. If true, this thread's execution must complete
       **/
      bool critical;
    };
  }
}

#endif  // _MADARA_THREADS_WORK_UNIT_H_
