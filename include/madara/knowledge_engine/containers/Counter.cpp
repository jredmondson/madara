
#ifndef _MADARA_NO_KARL_

#include <sstream>

#include "Counter.h"
#include "madara/knowledge_engine/Context_Guard.h"

Madara::Knowledge_Engine::Containers::Counter::Counter (
  const Knowledge_Update_Settings & settings)
: Base_Container ("", settings), context_ (0), id_ (0), counters_ (1)
{
  init_noharm ();
}
  
Madara::Knowledge_Engine::Containers::Counter::Counter (
  const std::string & name,
  Knowledge_Base & knowledge,
  const Knowledge_Update_Settings & settings)
: Base_Container (name, settings), context_ (&(knowledge.get_context ())),
  id_ (0), counters_ (1)
{
  init_noharm ();
  build_var ();
  build_aggregate_count ();
}
 
Madara::Knowledge_Engine::Containers::Counter::Counter (
  const std::string & name,
  Variables & knowledge,
  const Knowledge_Update_Settings & settings)
: Base_Container (name, settings), context_ (knowledge.get_context ()),
  id_ (0), counters_ (1)
{
  init_noharm ();
  build_var ();
  build_aggregate_count ();
}
 
Madara::Knowledge_Engine::Containers::Counter::Counter (
  const std::string & name,
  Knowledge_Base & knowledge,
  int id,
  int counters,
  type value,
  const Knowledge_Update_Settings & settings)
: Base_Container (name, settings), context_ (&(knowledge.get_context ())),
  id_ (id), counters_ (counters)
{
  init_noharm ();
  build_var ();
  build_aggregate_count ();
  context_->set (variable_, value, settings);
}
 
Madara::Knowledge_Engine::Containers::Counter::Counter (
  const std::string & name,
  Variables & knowledge,
  int id,
  int counters,
  type value,
  const Knowledge_Update_Settings & settings)
: Base_Container (name, settings), context_ (knowledge.get_context ()),
  id_ (id), counters_ (counters)
{
  init_noharm ();
  build_var ();
  build_aggregate_count ();
  context_->set (variable_, value, settings);
}


Madara::Knowledge_Engine::Containers::Counter::Counter (const Counter & rhs)
: Base_Container (rhs), context_ (rhs.context_),
    id_ (rhs.id_),
    counters_ (rhs.counters_),
    variable_ (rhs.variable_),
    aggregate_count_ (rhs.aggregate_count_)
{

}


Madara::Knowledge_Engine::Containers::Counter::~Counter ()
{

}

void
Madara::Knowledge_Engine::Containers::Counter::operator= (const Counter & rhs)
{
  if (this != &rhs)
  {
    Guard guard (mutex_), guard2 (rhs.mutex_);

    this->context_ = rhs.context_;
    this->name_ = rhs.name_;
    this->id_ = rhs.id_;
    this->counters_ = rhs.counters_;
    this->settings_ = rhs.settings_;
    this->variable_ = rhs.variable_;
    this->aggregate_count_ = rhs.aggregate_count_;
  }
}

void
Madara::Knowledge_Engine::Containers::Counter::build_aggregate_count (void)
{
  if (context_ && name_ != "")
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    std::stringstream buffer;
    if (counters_ > 0)
    {
      // add the first counter variable
      buffer << name_;
      buffer << ".0";

      // add all other counter variables
      for (int i = 1; i < counters_; ++i)
      {
        buffer << "+";
        buffer << name_;
        buffer << ".";
        buffer << i;
      }
    }

    aggregate_count_ = context_->compile (buffer.str ());
  }
  else if (name_ == "")
  {
    context_->print ("ERROR: Container::Counter needs a name.\n", 0);
  }
  else if (!context_)
  {
    context_->print ("ERROR: Container::Counter needs a context.\n", 0);
  }
}

void
Madara::Knowledge_Engine::Containers::Counter::build_var (void)
{
  if (context_ && name_ != "")
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    std::stringstream buffer;

    buffer << name_;
    buffer << ".";
    buffer << id_;

    variable_ = context_->get_ref (buffer.str (), no_harm);
  }
  else if (name_ == "")
  {
    context_->print ("ERROR: Container::Counter needs a name.\n", 0);
  }
  else if (!context_)
  {
    context_->print ("ERROR: Container::Counter needs a context.\n", 0);
  }
}

void
Madara::Knowledge_Engine::Containers::Counter::init_noharm (void)
{
  no_harm.always_overwrite = false;
  no_harm.delay_sending_modifieds = true;
  no_harm.expand_variables = false;
  no_harm.signal_changes = false;
  no_harm.track_local_changes = false;
  no_harm.treat_globals_as_locals = true;
}
  
void
Madara::Knowledge_Engine::Containers::Counter::modify (void)
{
  Context_Guard context_guard (*context_);
  if (context_ && name_ != "")
  {
    context_->mark_modified (variable_);
  }
}

std::string
Madara::Knowledge_Engine::Containers::Counter::get_debug_info (void)
{
  std::stringstream result;

  result << "Counter: ";

  if (context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    result << this->name_;
    result << " = " << context_->get (variable_).to_string ();
    result << " (total: " << get_count () << ")";
  }

  return result.str ();
}

void
Madara::Knowledge_Engine::Containers::Counter::modify_ (void)
{
  modify ();
}

std::string
Madara::Knowledge_Engine::Containers::Counter::get_debug_info_ (void)
{
  return get_debug_info ();
}

Madara::Knowledge_Engine::Containers::Base_Container *
Madara::Knowledge_Engine::Containers::Counter::clone (void) const
{
  return new Counter (*this);
}

int
Madara::Knowledge_Engine::Containers::Counter::get_id (void) const
{
  Guard guard (mutex_);
  return id_;
}

int
Madara::Knowledge_Engine::Containers::Counter::get_counters (void) const
{
  Guard guard (mutex_);
  return counters_;
}

void
Madara::Knowledge_Engine::Containers::Counter::set_name (
  const std::string & var_name,
  Knowledge_Base & knowledge)
{
  Knowledge_Update_Settings keep_local (true);
  context_ = &(knowledge.get_context ());

  Context_Guard context_guard (*context_);
  Guard guard (mutex_);

  name_ = var_name;

  this->build_var ();
  this->build_aggregate_count ();
}

void
Madara::Knowledge_Engine::Containers::Counter::set_name (
  const std::string & var_name,
  Variables & knowledge)
{
  Knowledge_Update_Settings keep_local (true);
  context_ = knowledge.get_context ();

  Context_Guard context_guard (*context_);
  Guard guard (mutex_);

  name_ = var_name;

  this->build_var ();
  this->build_aggregate_count ();
}

void
Madara::Knowledge_Engine::Containers::Counter::resize (int id, int counters)
{
  if (context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    id_ = id;
    counters_ = counters;

    this->build_var ();
    this->build_aggregate_count ();
  }
}

Madara::Knowledge_Engine::Containers::Counter::type
Madara::Knowledge_Engine::Containers::Counter::operator= (type value)
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
Madara::Knowledge_Engine::Containers::Counter::operator== (type value) const
{
  if (context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);
    return get_count () == value;
  }

  return false;
}

bool
Madara::Knowledge_Engine::Containers::Counter::operator != (type value) const
{
  if (context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);
    return get_count () != value;
  }

  return true;
}

bool
Madara::Knowledge_Engine::Containers::Counter::operator== (
  const Counter & value) const
{
  if (context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);
    return get_count () == value.get_count ();
  }

  return false;
}

bool
Madara::Knowledge_Engine::Containers::Counter::operator != (
  const Counter & value) const
{
  if (context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);
    return get_count () != value.get_count ();
  }

  return true;
}

bool
Madara::Knowledge_Engine::Containers::Counter::operator< (type value) const
{
  if (context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);
    return get_count () < value;
  }

  return false;
}

bool
Madara::Knowledge_Engine::Containers::Counter::operator<= (type value) const
{
  if (context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);
    return get_count () <= value;
  }

  return false;
}

bool
Madara::Knowledge_Engine::Containers::Counter::operator> (type value) const
{
  if (context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);
    return get_count () > value;
  }

  return false;
}

bool
Madara::Knowledge_Engine::Containers::Counter::operator>= (type value) const
{
  if (context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);
    return get_count () >= value;
  }

  return false;
}

Madara::Knowledge_Engine::Containers::Counter::type
Madara::Knowledge_Engine::Containers::Counter::operator* (void) const
{
  return to_integer ();
}

Madara::Knowledge_Record
Madara::Knowledge_Engine::Containers::Counter::to_record (void) const
{
  Madara::Knowledge_Record result;
  
  if (context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);
    result = get_count_record ();
  }
  
  return result;
}
 
Madara::Knowledge_Record::Integer
Madara::Knowledge_Engine::Containers::Counter::to_integer (void) const
{
  Madara::Knowledge_Record::Integer result (0);
  
  if (context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);
    result = get_count ();
  }

  return result;
}

void
Madara::Knowledge_Engine::Containers::Counter::operator += (type value)
{

  if (context_)
  {
    type current (0);
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    current = context_->get (variable_, settings_).to_integer ();
    current += value;
    context_->set (variable_, current, settings_);
  }
}

void
Madara::Knowledge_Engine::Containers::Counter::operator -= (type value)
{
  if (context_)
  {
    type current (0);
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    current = context_->get (variable_, settings_).to_integer ();
    current -= value;
    context_->set (variable_, current, settings_);
  }
}

void
Madara::Knowledge_Engine::Containers::Counter::operator++ (void)
{
  if (context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);
    context_->inc (variable_, settings_);
  }
}
 
void
Madara::Knowledge_Engine::Containers::Counter::operator-- (void)
{
  if (context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);
    context_->dec (variable_, settings_);
  }
}
 
double
Madara::Knowledge_Engine::Containers::Counter::to_double (void) const
{
  double result (0.0);

  if (context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);
    result = get_count_double ();
  }

  return result;
}
   
std::string
Madara::Knowledge_Engine::Containers::Counter::to_string (void) const
{
  std::string result;

  if (context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);
    result = get_count_string ();
  }

  return result;
}

void
Madara::Knowledge_Engine::Containers::Counter::set_quality (
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
Madara::Knowledge_Engine::Containers::Counter::is_true (void) const
{
  bool result (false);

  if (context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);
    result = context_->get (variable_, settings_).is_true ();
  }

  return result;
}

bool
Madara::Knowledge_Engine::Containers::Counter::is_false (void) const
{
  return !is_true ();
}


bool
Madara::Knowledge_Engine::Containers::Counter::is_true_ (void) const
{
  return is_true ();
}

bool
Madara::Knowledge_Engine::Containers::Counter::is_false_ (void) const
{
  return is_false ();
}


#endif // _MADARA_NO_KARL_
