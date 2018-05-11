#include "Integer.h"
#include "madara/knowledge/ContextGuard.h"

madara::knowledge::containers::Integer::Integer (
  const KnowledgeUpdateSettings & settings)
: BaseContainer ("", settings), context_ (0)
{
}
  
madara::knowledge::containers::Integer::Integer (
  const std::string & name,
  KnowledgeBase & knowledge,
  const KnowledgeUpdateSettings & settings)
: BaseContainer (name, settings), context_ (&(knowledge.get_context ()))
{
  variable_ = knowledge.get_ref (name, settings_);
}
 
madara::knowledge::containers::Integer::Integer (
  const std::string & name,
  Variables & knowledge,
  const KnowledgeUpdateSettings & settings)
: BaseContainer (name, settings), context_ (knowledge.get_context ())
{
  variable_ = knowledge.get_ref (name, settings_);
}
 
madara::knowledge::containers::Integer::Integer (
  const std::string & name,
  KnowledgeBase & knowledge,
  type value,
  const KnowledgeUpdateSettings & settings)
: BaseContainer (name, settings), context_ (&(knowledge.get_context ()))
{
  variable_ = knowledge.get_ref (name);
  context_->set (variable_, value, settings);
}
 
madara::knowledge::containers::Integer::Integer (
  const std::string & name,
  Variables & knowledge,
  type value,
  const KnowledgeUpdateSettings & settings)
: BaseContainer (name, settings), context_ (knowledge.get_context ())
{
  variable_ = knowledge.get_ref (name);
  context_->set (variable_, value, settings);
}


madara::knowledge::containers::Integer::Integer (const Integer & rhs)
: BaseContainer (rhs), context_ (rhs.context_), variable_ (rhs.variable_)
{

}


madara::knowledge::containers::Integer::~Integer ()
{

}

void
madara::knowledge::containers::Integer::modify_ (void)
{
  modify ();
}

std::string
madara::knowledge::containers::Integer::get_debug_info_ (void)
{
  return get_debug_info ();
}

void
madara::knowledge::containers::Integer::exchange (
  containers::Integer & other)
{
  if (context_ && other.context_)
  {
    ContextGuard context_guard (*context_, std::adopt_lock);
    ContextGuard other_context_guard (*other.context_, std::adopt_lock);
    MADARA_GUARD_TYPE guard (mutex_, std::adopt_lock),
                      guard2 (other.mutex_, std::adopt_lock);


    type temp = *other;
    other = **this;
    *this = temp;
  }
}


bool
madara::knowledge::containers::Integer::is_true_ (void) const
{
  return is_true ();
}

bool
madara::knowledge::containers::Integer::is_false_ (void) const
{
  return is_false ();
}
