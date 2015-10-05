#include "Integer.h"
#include "madara/knowledge/Context_Guard.h"

madara::knowledge::containers::Integer::Integer (
  const Knowledge_Update_Settings & settings)
: Base_Container ("", settings), context_ (0)
{
}
  
madara::knowledge::containers::Integer::Integer (
  const std::string & name,
  Knowledge_Base & knowledge,
  const Knowledge_Update_Settings & settings)
: Base_Container (name, settings), context_ (&(knowledge.get_context ()))
{
  variable_ = knowledge.get_ref (name, settings_);
}
 
madara::knowledge::containers::Integer::Integer (
  const std::string & name,
  Variables & knowledge,
  const Knowledge_Update_Settings & settings)
: Base_Container (name, settings), context_ (knowledge.get_context ())
{
  variable_ = knowledge.get_ref (name, settings_);
}
 
madara::knowledge::containers::Integer::Integer (
  const std::string & name,
  Knowledge_Base & knowledge,
  type value,
  const Knowledge_Update_Settings & settings)
: Base_Container (name, settings), context_ (&(knowledge.get_context ()))
{
  variable_ = knowledge.get_ref (name);
  context_->set (variable_, value, settings);
}
 
madara::knowledge::containers::Integer::Integer (
  const std::string & name,
  Variables & knowledge,
  type value,
  const Knowledge_Update_Settings & settings)
: Base_Container (name, settings), context_ (knowledge.get_context ())
{
  variable_ = knowledge.get_ref (name);
  context_->set (variable_, value, settings);
}


madara::knowledge::containers::Integer::Integer (const Integer & rhs)
: Base_Container (rhs), context_ (rhs.context_), variable_ (rhs.variable_)
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
    Context_Guard context_guard (*context_);
    Context_Guard other_context_guard (*other.context_);
    Guard guard (mutex_), guard2 (other.mutex_);

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
