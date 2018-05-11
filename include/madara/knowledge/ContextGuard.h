
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
       *
       * Arguments past first forwarded to underlying guard type
       **/
      template<typename... Args>
      ContextGuard (KnowledgeBase & knowledge, Args&&... args);

      /**
       * Constructor
       *
       * Arguments past first forwarded to underlying guard type
       **/
      template<typename... Args>
      ContextGuard (ThreadSafeContext & context, Args&&... args);

    private:
      /**
       * The context that will be locked
       **/
      ThreadSafeContext & context_;

      std::lock_guard<ThreadSafeContext> guard_;
    };

  }
}

#include "ContextGuard.inl"

#endif // _MADARA_CONTEXT_GUARD_H_
