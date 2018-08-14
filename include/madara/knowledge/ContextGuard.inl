#include "ContextGuard.h"

namespace madara { namespace knowledge {

template<typename... Args>
inline ContextGuard::ContextGuard (
  const KnowledgeBase & knowledge, Args&&... args)
  : guard_ (knowledge.get_context (), std::forward<Args>(args)...)
{
}

template<typename... Args>
inline ContextGuard::ContextGuard (
  const ThreadSafeContext & context, Args&&... args)
  : guard_ (context, std::forward<Args>(args)...)
{
}

} }
