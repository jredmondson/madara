#include "Integer.h"
#include "madara/knowledge/Context_Guard.h"

inline void
madara::knowledge::containers::Integer::modify (void)
{
  Context_Guard context_guard (*context_);
  if (context_ && name_ != "")
  {
    context_->mark_modified (variable_);
  }
}

inline std::string
madara::knowledge::containers::Integer::get_debug_info (void)
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

inline madara::knowledge::containers::Base_Container *
madara::knowledge::containers::Integer::clone (void) const
{
  return new Integer (*this);
}

inline void
madara::knowledge::containers::Integer::operator= (const Integer & rhs)
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
madara::knowledge::containers::Integer::set_name (
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
madara::knowledge::containers::Integer::set_name (
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
madara::knowledge::containers::Integer::set_name (
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

inline madara::knowledge::containers::Integer::type
madara::knowledge::containers::Integer::operator= (type value)
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
madara::knowledge::containers::Integer::operator== (type value) const
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
madara::knowledge::containers::Integer::operator != (type value) const
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
madara::knowledge::containers::Integer::operator== (
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
madara::knowledge::containers::Integer::operator != (
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
madara::knowledge::containers::Integer::operator< (type value) const
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
madara::knowledge::containers::Integer::operator<= (type value) const
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
madara::knowledge::containers::Integer::operator> (type value) const
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
madara::knowledge::containers::Integer::operator>= (type value) const
{
  if (context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);
    return context_->get (variable_, settings_) >= value;
  }

  return false;
}

inline madara::knowledge::containers::Integer::type
madara::knowledge::containers::Integer::operator* (void) const
{
  return to_integer ();
}

inline bool
madara::knowledge::containers::Integer::exists (void) const
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
madara::knowledge::containers::Integer::is_true (void) const
{
  bool result (false);

  madara_logger_log (context_->get_logger (), logger::LOG_MAJOR,
    "Integer::is_true: checking for non-zero value\n");

  if (context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);
    result = context_->get (variable_, settings_).is_true ();
  }

  madara_logger_log (context_->get_logger (), logger::LOG_MAJOR,
    "Integer::is_true: final result is %d\n", (int)result);

  return result;
}

inline bool
madara::knowledge::containers::Integer::is_false (void) const
{
  bool result (true);

  madara_logger_log (context_->get_logger (), logger::LOG_MAJOR,
    "Integer::is_false: checking for zero value\n", (int)result);

  if (context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);
    result = context_->get (variable_, settings_).is_false ();
  }

  madara_logger_log (context_->get_logger (), logger::LOG_MAJOR,
    "Integer::is_false: final result is %d\n", (int)result);

  return result;
}

inline madara::Knowledge_Record
madara::knowledge::containers::Integer::to_record (void) const
{
  madara::Knowledge_Record result;
  
  if (context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);
    result = context_->get (variable_, settings_);
  }
  
  return result;
}
 
inline madara::Knowledge_Record::Integer
madara::knowledge::containers::Integer::to_integer (void) const
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

inline madara::knowledge::containers::Integer::type
madara::knowledge::containers::Integer::operator += (type value)
{
  madara::knowledge::containers::Integer::type result (0);

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

inline madara::knowledge::containers::Integer::type
madara::knowledge::containers::Integer::operator -= (type value)
{
  madara::knowledge::containers::Integer::type result (0);

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

inline madara::knowledge::containers::Integer::type
madara::knowledge::containers::Integer::operator++ (void)
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
 
inline madara::knowledge::containers::Integer::type
madara::knowledge::containers::Integer::operator-- (void)
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
madara::knowledge::containers::Integer::to_double (void) const
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
madara::knowledge::containers::Integer::to_string (void) const
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
madara::knowledge::containers::Integer::set_quality (
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
