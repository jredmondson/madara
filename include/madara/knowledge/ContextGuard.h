
#ifndef _MADARA_CONTEXT_GUARD_H_
#define _MADARA_CONTEXT_GUARD_H_

#include "madara/knowledge/KnowledgeBase.h"

/**
 * @file ContextGuard.h
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
     * @class ContextGuard
     * @brief A thread-safe guard for a context or knowledge base
     */
    class MADARA_Export ContextGuard
    {
    public:
      /**
       * Constructor
       **/
      ContextGuard (KnowledgeBase & knowledge);
      
      /**
       * Constructor
       **/
      ContextGuard (ThreadSafeContext & context);
      
      /**
       * Destructor
       **/
      ~ContextGuard ();

    private:
      
      /**
       * The context that will be locked
       **/
      ThreadSafeContext & context_;
    };

  }
}


#endif // _MADARA_CONTEXT_GUARD_H_
