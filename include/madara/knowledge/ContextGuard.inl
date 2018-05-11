#include "ContextGuard.h"

namespace madara { namespace knowledge {

template<typename... Args>
inline ContextGuard::ContextGuard (
  KnowledgeBase & knowledge, Args&&... args)
  : context_ (knowledge.get_context ()),
    guard_ (context_, std::forward<Args>(args)...)
{
}

template<typename... Args>
inline ContextGuard::ContextGuard (
  ThreadSafeContext & context, Args&&... args)
  : context_ (context),
    guard_ (context_, std::forward<Args>(args)...)
{
}

} }
