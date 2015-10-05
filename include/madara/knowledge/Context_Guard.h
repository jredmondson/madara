
#ifndef _MADARA_CONTEXT_GUARD_H_
#define _MADARA_CONTEXT_GUARD_H_

#include "madara/knowledge/Knowledge_Base.h"

/**
 * @file Context_Guard.h
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains the an atomic guard for locking and
 * unlocking the thread safe context
 **/

namespace madara
{
  namespace knowledge
  {
    /**
     * @class Timed_Executor
     * @brief This class executes expression events at specified times
     */
    class MADARA_Export Context_Guard
    {
    public:
      /**
       * Constructor
       **/
      Context_Guard (Knowledge_Base & knowledge);
      
      /**
       * Constructor
       **/
      Context_Guard (Thread_Safe_Context & context);
      
      /**
       * Destructor
       **/
      ~Context_Guard ();

    private:
      
      /**
       * The context that will be locked
       **/
      Thread_Safe_Context & context_;
    };

  }
}


#endif // _MADARA_CONTEXT_GUARD_H_
