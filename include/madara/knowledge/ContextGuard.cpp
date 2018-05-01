#include "ContextGuard.h"

madara::knowledge::ContextGuard::ContextGuard (
  KnowledgeBase & knowledge)
  : context_ (knowledge.get_context ())
{
  context_.lock ();
}

madara::knowledge::ContextGuard::ContextGuard (
  ThreadSafeContext & context)
  : context_ (context)
{
  context_.lock ();
}

madara::knowledge::ContextGuard::~ContextGuard ()
{
  context_.unlock ();
}
