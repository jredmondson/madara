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
Madara::Knowledge_Engine::Containers::Integer::modify (void)
{
  Context_Guard context_guard (*context_);
  if (context_ && name_ != "")
  {
    context_->mark_modified (variable_);
  }
}

std::string
Madara::Knowledge_Engine::Containers::Integer::get_debug_info (void)
{
  std::stringstream result;

  result << "Integer: ";

  if (context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    result << this->name_;
    result << " = " << context_->get (variable_).to_string ();
  }

  return result.str ();
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

Madara::Knowledge_Engine::Containers::Base_Container *
Madara::Knowledge_Engine::Containers::Integer::clone (void) const
{
  return new Integer (*this);
}

void
Madara::Knowledge_Engine::Containers::Integer::operator= (const Integer & rhs)
{
  if (this != &rhs)
  {
    Guard guard (mutex_), guard2 (rhs.mutex_);

    this->context_ = rhs.context_;
    this->name_ = rhs.name_;
    this->settings_ = rhs.settings_;
    this->variable_ = rhs.variable_;
  }
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

void
Madara::Knowledge_Engine::Containers::Integer::set_name (
  const std::string & var_name,
  Knowledge_Base & knowledge)
{
  Knowledge_Update_Settings keep_local (true);
  context_ = &(knowledge.get_context ());

  Context_Guard context_guard (*context_);
  Guard guard (mutex_);

  name_ = var_name;
  variable_ = context_->get_ref (name_, keep_local);
}

void
Madara::Knowledge_Engine::Containers::Integer::set_name (
  const std::string & var_name,
  Thread_Safe_Context & knowledge)
{
  Knowledge_Update_Settings keep_local (true);
  context_ = &knowledge;

  Context_Guard context_guard (*context_);
  Guard guard (mutex_);

  name_ = var_name;
  variable_ = context_->get_ref (name_, keep_local);
}

void
Madara::Knowledge_Engine::Containers::Integer::set_name (
  const std::string & var_name,
  Variables & knowledge)
{
  Knowledge_Update_Settings keep_local (true);
  context_ = knowledge.get_context ();

  Context_Guard context_guard (*context_);
  Guard guard (mutex_);

  name_ = var_name;
  variable_ = context_->get_ref (name_, keep_local);
}

Madara::Knowledge_Engine::Containers::Integer::type
Madara::Knowledge_Engine::Containers::Integer::operator= (type value)
{
  if (context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);
    context_->set (variable_, value, settings_);
  }

  return value;
}

bool
Madara::Knowledge_Engine::Containers::Integer::operator== (type value) const
{
  if (context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);
    return context_->get (variable_, settings_) == value;
  }

  return false;
}

bool
Madara::Knowledge_Engine::Containers::Integer::operator != (type value) const
{
  if (context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);
    return context_->get (variable_, settings_) != value;
  }

  return true;
}

bool
Madara::Knowledge_Engine::Containers::Integer::operator== (
  const Integer & value) const
{
  if (context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);
    return
      context_->get (variable_, settings_) ==
        value.context_->get (value.variable_, value.settings_);
  }

  return false;
}

bool
Madara::Knowledge_Engine::Containers::Integer::operator != (
  const Integer & value) const
{
  if (context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);
    return
      context_->get (variable_, settings_) !=
        value.context_->get (value.variable_, value.settings_);
  }

  return true;
}

bool
Madara::Knowledge_Engine::Containers::Integer::operator< (type value) const
{
  if (context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);
    return context_->get (variable_, settings_) < value;
  }

  return false;
}

bool
Madara::Knowledge_Engine::Containers::Integer::operator<= (type value) const
{
  if (context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);
    return context_->get (variable_, settings_) <= value;
  }

  return false;
}

bool
Madara::Knowledge_Engine::Containers::Integer::operator> (type value) const
{
  if (context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);
    return context_->get (variable_, settings_) > value;
  }

  return false;
}

bool
Madara::Knowledge_Engine::Containers::Integer::operator>= (type value) const
{
  if (context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);
    return context_->get (variable_, settings_) >= value;
  }

  return false;
}

Madara::Knowledge_Engine::Containers::Integer::type
Madara::Knowledge_Engine::Containers::Integer::operator* (void) const
{
  return to_integer ();
}

bool
Madara::Knowledge_Engine::Containers::Integer::exists (void) const
{
  bool result (false);

  if (context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);
    result = context_->exists (variable_);
  }

  return result;
}

Madara::Knowledge_Record
Madara::Knowledge_Engine::Containers::Integer::to_record (void) const
{
  Madara::Knowledge_Record result;
  
  if (context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);
    result = context_->get (variable_, settings_);
  }
  
  return result;
}
 
Madara::Knowledge_Record::Integer
Madara::Knowledge_Engine::Containers::Integer::to_integer (void) const
{
  if (context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);
    return context_->get (variable_, settings_).to_integer ();
  }
  else
    return 0;
}

Madara::Knowledge_Engine::Containers::Integer::type
Madara::Knowledge_Engine::Containers::Integer::operator += (type value)
{
  Madara::Knowledge_Engine::Containers::Integer::type result (0);

  if (context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    result = context_->get (variable_, settings_).to_integer ();
    result += value;
    context_->set (variable_, result, settings_);
  }

  return result;
}

Madara::Knowledge_Engine::Containers::Integer::type
Madara::Knowledge_Engine::Containers::Integer::operator -= (type value)
{
  Madara::Knowledge_Engine::Containers::Integer::type result (0);

  if (context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    result = context_->get (variable_, settings_).to_integer ();
    result -= value;
    context_->set (variable_, result, settings_);
  }

  return result;
}
Madara::Knowledge_Engine::Containers::Integer::type
Madara::Knowledge_Engine::Containers::Integer::operator++ (void)
{
  if (context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);
    return context_->inc (variable_, settings_).to_integer ();
  }
  else
    return 0;
}
 
Madara::Knowledge_Engine::Containers::Integer::type
Madara::Knowledge_Engine::Containers::Integer::operator-- (void)
{
  if (context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);
    return context_->dec (variable_, settings_).to_integer ();
  }
  else
    return 0;
}
 
double
Madara::Knowledge_Engine::Containers::Integer::to_double (void) const
{
  if (context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);
    return context_->get (variable_, settings_).to_double ();
  }
  else
    return 0.0;
}
   
std::string
Madara::Knowledge_Engine::Containers::Integer::to_string (void) const
{
  if (context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);
    return context_->get (variable_, settings_).to_string ();
  }
  else
    return "";
}

void
Madara::Knowledge_Engine::Containers::Integer::set_quality (
  uint32_t quality,
  const Knowledge_Reference_Settings & settings)
{
  if (context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);
    context_->set_quality (name_, quality, true, settings);
  }
}
