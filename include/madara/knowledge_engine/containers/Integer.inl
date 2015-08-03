#include "Integer.h"
#include "madara/knowledge_engine/Context_Guard.h"

inline void
Madara::Knowledge_Engine::Containers::Integer::modify (void)
{
  Context_Guard context_guard (*context_);
  if (context_ && name_ != "")
  {
    context_->mark_modified (variable_);
  }
}

inline std::string
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

inline Madara::Knowledge_Engine::Containers::Base_Container *
Madara::Knowledge_Engine::Containers::Integer::clone (void) const
{
  return new Integer (*this);
}

inline void
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

inline void
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

inline void
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

inline void
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

inline Madara::Knowledge_Engine::Containers::Integer::type
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

inline bool
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

inline bool
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

inline bool
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

inline bool
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

inline bool
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

inline bool
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

inline bool
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

inline bool
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

inline Madara::Knowledge_Engine::Containers::Integer::type
Madara::Knowledge_Engine::Containers::Integer::operator* (void) const
{
  return to_integer ();
}

inline bool
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

inline bool
Madara::Knowledge_Engine::Containers::Integer::is_true (void) const
{
  bool result (false);

  madara_logger_log (context_->get_logger (), Logger::LOG_MAJOR,
    "Integer::is_true: checking for non-zero value\n");

  if (context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);
    result = context_->get (variable_, settings_).is_true ();
  }

  madara_logger_log (context_->get_logger (), Logger::LOG_MAJOR,
    "Integer::is_true: final result is %d\n", (int)result);

  return result;
}

inline bool
Madara::Knowledge_Engine::Containers::Integer::is_false (void) const
{
  bool result (true);

  madara_logger_log (context_->get_logger (), Logger::LOG_MAJOR,
    "Integer::is_false: checking for zero value\n", (int)result);

  if (context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);
    result = context_->get (variable_, settings_).is_false ();
  }

  madara_logger_log (context_->get_logger (), Logger::LOG_MAJOR,
    "Integer::is_false: final result is %d\n", (int)result);

  return result;
}

inline Madara::Knowledge_Record
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
 
inline Madara::Knowledge_Record::Integer
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

inline Madara::Knowledge_Engine::Containers::Integer::type
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

inline Madara::Knowledge_Engine::Containers::Integer::type
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

inline Madara::Knowledge_Engine::Containers::Integer::type
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
 
inline Madara::Knowledge_Engine::Containers::Integer::type
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
 
inline double
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
   
inline std::string
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

inline void
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
