#include "Integer.h"
#include "madara/knowledge_engine/Context_Guard.h"

Madara::Knowledge_Engine::Containers::Integer::Integer (
  const Knowledge_Update_Settings & settings)
: Base_Container ("", settings), context_ (0)
{
}
  
Madara::Knowledge_Engine::Containers::Integer::Integer (
  const std::string & name,
  Knowledge_Base & knowledge,
  const Knowledge_Update_Settings & settings)
: Base_Container (name, settings), context_ (&(knowledge.get_context ()))
{
  variable_ = knowledge.get_ref (name, settings_);
}
 
Madara::Knowledge_Engine::Containers::Integer::Integer (
  const std::string & name,
  Variables & knowledge,
  const Knowledge_Update_Settings & settings)
: Base_Container (name, settings), context_ (knowledge.get_context ())
{
  variable_ = knowledge.get_ref (name, settings_);
}
 
Madara::Knowledge_Engine::Containers::Integer::Integer (
  const std::string & name,
  Knowledge_Base & knowledge,
  type value,
  const Knowledge_Update_Settings & settings)
: Base_Container (name, settings), context_ (&(knowledge.get_context ()))
{
  variable_ = knowledge.get_ref (name);
  context_->set (variable_, value, settings);
}
 
Madara::Knowledge_Engine::Containers::Integer::Integer (
  const std::string & name,
  Variables & knowledge,
  type value,
  const Knowledge_Update_Settings & settings)
: Base_Container (name, settings), context_ (knowledge.get_context ())
{
  variable_ = knowledge.get_ref (name);
  context_->set (variable_, value, settings);
}


Madara::Knowledge_Engine::Containers::Integer::Integer (const Integer & rhs)
: Base_Container (rhs), context_ (rhs.context_), variable_ (rhs.variable_)
{

}


Madara::Knowledge_Engine::Containers::Integer::~Integer ()
{

}

void
Madara::Knowledge_Engine::Containers::Integer::modify_ (void)
{
  modify ();
}

std::string
Madara::Knowledge_Engine::Containers::Integer::get_debug_info_ (void)
{
  return get_debug_info ();
}

void
Madara::Knowledge_Engine::Containers::Integer::exchange (
  Containers::Integer & other)
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
Madara::Knowledge_Engine::Containers::Integer::is_true_ (void) const
{
  return is_true ();
}

bool
Madara::Knowledge_Engine::Containers::Integer::is_false_ (void) const
{
  return is_false ();
}
