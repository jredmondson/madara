#include "Double.h"
#include "madara/knowledge_engine/Context_Guard.h"


Madara::Knowledge_Engine::Containers::Double::Double (
  const Knowledge_Update_Settings & settings)
: Base_Container ("", settings), context_ (0)
{
}
  
Madara::Knowledge_Engine::Containers::Double::Double (
  const std::string & name,
  Knowledge_Base & knowledge,
  const Knowledge_Update_Settings & settings)
: Base_Container (name, settings), context_ (&(knowledge.get_context ()))
{
  variable_ = knowledge.get_ref (name, settings_);
}
 
Madara::Knowledge_Engine::Containers::Double::Double (
  const std::string & name,
  Variables & knowledge,
  const Knowledge_Update_Settings & settings)
: Base_Container (name, settings), context_ (knowledge.get_context ())
{
  variable_ = knowledge.get_ref (name, settings_);
}
 
Madara::Knowledge_Engine::Containers::Double::Double (
  const std::string & name,
  Knowledge_Base & knowledge,
  type value,
  const Knowledge_Update_Settings & settings)
: Base_Container (name, settings), context_ (&(knowledge.get_context ()))
{
  variable_ = knowledge.get_ref (name);
  context_->set (variable_, value, settings);
}
 
Madara::Knowledge_Engine::Containers::Double::Double (
  const std::string & name,
  Variables & knowledge,
  type value,
  const Knowledge_Update_Settings & settings)
: Base_Container (name, settings), context_ (knowledge.get_context ())
{
  variable_ = knowledge.get_ref (name);
  context_->set (variable_, value, settings);
}


Madara::Knowledge_Engine::Containers::Double::Double (const Double & rhs)
  : Base_Container (rhs), context_ (rhs.context_),
    variable_ (rhs.variable_)
{

}


Madara::Knowledge_Engine::Containers::Double::~Double ()
{

}
  
void
Madara::Knowledge_Engine::Containers::Double::modify (void)
{
  Context_Guard context_guard (*context_);
  if (context_ && name_ != "")
  {
    context_->mark_modified (variable_);
  }
}

std::string
Madara::Knowledge_Engine::Containers::Double::get_debug_info (void)
{
  std::stringstream result;

  result << "Double: ";

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
Madara::Knowledge_Engine::Containers::Double::modify_ (void)
{
  modify ();
}

std::string
Madara::Knowledge_Engine::Containers::Double::get_debug_info_ (void)
{
  return get_debug_info ();
}

Madara::Knowledge_Engine::Containers::Base_Container *
Madara::Knowledge_Engine::Containers::Double::clone (void) const
{
  return new Double (*this);
}

void
Madara::Knowledge_Engine::Containers::Double::operator= (const Double & rhs)
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
Madara::Knowledge_Engine::Containers::Double::exchange (
  Containers::Double & other)
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
Madara::Knowledge_Engine::Containers::Double::set_name (
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
Madara::Knowledge_Engine::Containers::Double::set_name (
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

void
Madara::Knowledge_Engine::Containers::Double::set_name (
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

Madara::Knowledge_Engine::Containers::Double::type
Madara::Knowledge_Engine::Containers::Double::operator= (type value)
{
  if (context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);
    context_->set (variable_, value, settings_);
  }

  return value;
}
  
Madara::Knowledge_Engine::Containers::Double::type
Madara::Knowledge_Engine::Containers::Double::operator += (type value)
{
  Madara::Knowledge_Engine::Containers::Double::type result (0);

  if (context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    result = context_->get (variable_, settings_).to_double ();
    result += value;
    context_->set (variable_, result, settings_);
  }

  return result;
}

Madara::Knowledge_Engine::Containers::Double::type
Madara::Knowledge_Engine::Containers::Double::operator -= (type value)
{
  Madara::Knowledge_Engine::Containers::Double::type result (0);

  if (context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    result = context_->get (variable_, settings_).to_double ();
    result -= value;
    context_->set (variable_, result, settings_);
  }

  return result;
}
Madara::Knowledge_Engine::Containers::Double::type
Madara::Knowledge_Engine::Containers::Double::operator++ (void)
{
  if (context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);
    return context_->inc (variable_, settings_).to_double ();
  }
  else
    return 0;
}
 
Madara::Knowledge_Engine::Containers::Double::type
Madara::Knowledge_Engine::Containers::Double::operator-- (void)
{
  if (context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);
    return context_->dec (variable_, settings_).to_double ();
  }
  else
    return 0;
}
 
bool
Madara::Knowledge_Engine::Containers::Double::operator== (type value) const
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
Madara::Knowledge_Engine::Containers::Double::operator != (type value) const
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
Madara::Knowledge_Engine::Containers::Double::operator== (
  const Double & value) const
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
Madara::Knowledge_Engine::Containers::Double::operator != (
  const Double & value) const
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
Madara::Knowledge_Engine::Containers::Double::operator< (type value) const
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
Madara::Knowledge_Engine::Containers::Double::operator<= (type value) const
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
Madara::Knowledge_Engine::Containers::Double::operator> (type value) const
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
Madara::Knowledge_Engine::Containers::Double::operator>= (type value) const
{
  if (context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);
    return context_->get (variable_, settings_) >= value;
  }

  return false;
}

Madara::Knowledge_Engine::Containers::Double::type
Madara::Knowledge_Engine::Containers::Double::operator* (void) const
{
  return to_double ();
}
  
bool
Madara::Knowledge_Engine::Containers::Double::exists (void) const
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
Madara::Knowledge_Engine::Containers::Double::to_record (void) const
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
 
double
Madara::Knowledge_Engine::Containers::Double::to_double (void) const
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
 
Madara::Knowledge_Record::Integer
Madara::Knowledge_Engine::Containers::Double::to_integer (void) const
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
   
std::string
Madara::Knowledge_Engine::Containers::Double::to_string (void) const
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
Madara::Knowledge_Engine::Containers::Double::set_quality (
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

bool
Madara::Knowledge_Engine::Containers::Double::is_true (void) const
{
  bool result (false);

  madara_logger_log (context_->get_logger (), Logger::LOG_MAJOR,
    "Double::is_true: checking for non-zero value\n", (int)result);

  if (context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);
    result = context_->get (variable_, settings_).is_true ();
  }

  madara_logger_log (context_->get_logger (), Logger::LOG_MAJOR,
    "Double::is_true: final result is %d\n", (int)result);

  return result;
}

bool
Madara::Knowledge_Engine::Containers::Double::is_false (void) const
{
  return !is_true ();
}


bool
Madara::Knowledge_Engine::Containers::Double::is_true_ (void) const
{
  return is_true ();
}

bool
Madara::Knowledge_Engine::Containers::Double::is_false_ (void) const
{
  return is_false ();
}
