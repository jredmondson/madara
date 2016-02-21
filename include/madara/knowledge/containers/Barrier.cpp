
#ifndef _MADARA_NO_KARL_

#include <sstream>

#include "Barrier.h"
#include "madara/knowledge/ContextGuard.h"

madara::knowledge::containers::Barrier::Barrier (
  const KnowledgeUpdateSettings & settings)
: BaseContainer ("", settings), context_ (0), id_ (0), participants_ (1)
{
  init_noharm ();
}
  
madara::knowledge::containers::Barrier::Barrier (
  const std::string & name,
  KnowledgeBase & knowledge,
  const KnowledgeUpdateSettings & settings)
: BaseContainer (name, settings), context_ (&(knowledge.get_context ())),
  id_ (0), participants_ (1)
{
  init_noharm ();
  build_var ();
  build_aggregate_barrier ();
}

madara::knowledge::containers::Barrier::Barrier (
  const std::string & name,
  Variables & knowledge,
  const KnowledgeUpdateSettings & settings)
: BaseContainer (name, settings), context_ (knowledge.get_context ()),
  id_ (0), participants_ (1)
{
  init_noharm ();
  build_var ();
  build_aggregate_barrier ();
}
 
madara::knowledge::containers::Barrier::Barrier (
  const std::string & name,
  KnowledgeBase & knowledge,
  int id,
  int participants,
  const KnowledgeUpdateSettings & settings)
: BaseContainer (name, settings), context_ (&(knowledge.get_context ())),
  id_ (id), participants_ (participants)
{
  init_noharm ();
  build_var ();
  build_aggregate_barrier ();
}
 
madara::knowledge::containers::Barrier::Barrier (
  const std::string & name,
  Variables & knowledge,
  int id,
  int participants,
  const KnowledgeUpdateSettings & settings)
: BaseContainer (name, settings), context_ (knowledge.get_context ()),
  id_ (id), participants_ (participants)
{
  init_noharm ();
  build_var ();
  build_aggregate_barrier ();
}


madara::knowledge::containers::Barrier::Barrier (const Barrier & rhs)
: BaseContainer (rhs), context_ (rhs.context_),
    id_ (rhs.id_),
    participants_ (rhs.participants_),
    variable_ (rhs.variable_),
    aggregate_barrier_ (rhs.aggregate_barrier_),
    variable_name_ (rhs.variable_name_)
{

}


madara::knowledge::containers::Barrier::~Barrier ()
{

}

void
madara::knowledge::containers::Barrier::operator= (const Barrier & rhs)
{
  if (this != &rhs)
  {
    MADARA_GUARD_TYPE guard (mutex_), guard2 (rhs.mutex_);

    this->context_ = rhs.context_;
    this->name_ = rhs.name_;
    this->id_ = rhs.id_;
    this->participants_ = rhs.participants_;
    this->settings_ = rhs.settings_;
    this->variable_ = rhs.variable_;
    this->aggregate_barrier_ = rhs.aggregate_barrier_;
    this->variable_name_ = rhs.variable_name_;
  }
}

void
madara::knowledge::containers::Barrier::build_aggregate_barrier (void)
{
  if (context_ && name_ != "")
  {
    ContextGuard context_guard (*context_);
    MADARA_GUARD_TYPE guard (mutex_);

    std::stringstream buffer;
    if (participants_ > 0)
    {
      // barrier logic is that everyone else is at least to your barrier
      buffer << name_;
      buffer << ".0 >= ";
      buffer << name_;
      buffer << ".";
      buffer << id_;

      // add all other barrierer variables
      for (size_t i = 1; i < participants_; ++i)
      {
        buffer << " && ";
        buffer << name_;
        buffer << ".";
        buffer << i;
        buffer << " >= ";
        buffer << name_;
        buffer << ".";
        buffer << id_;
      }
    }

    aggregate_barrier_ = context_->compile (buffer.str ());


    madara_logger_log (context_->get_logger (), logger::LOG_MAJOR,
      "Barrier::build_aggregate_barrier: building barrier string of %s\n",
      buffer.str ().c_str ());
  }
  else if (name_ == "")
  {
    context_->print ("ERROR: Container::Barrier needs a name.\n", 0);
  }
  else if (!context_)
  {
    context_->print ("ERROR: Container::Barrier needs a context.\n", 0);
  }
}

void
madara::knowledge::containers::Barrier::build_var (void)
{
  if (context_ && name_ != "")
  {
    ContextGuard context_guard (*context_);
    MADARA_GUARD_TYPE guard (mutex_);

    std::stringstream buffer;

    buffer << name_;
    buffer << ".";
    buffer << id_;

    variable_name_ = buffer.str ();

    madara_logger_log (context_->get_logger (), logger::LOG_MAJOR,
      "Barrier::build_var: settings variable reference to %s\n",
      variable_name_.c_str ());

    variable_ = context_->get_ref (buffer.str (), no_harm);
  }
  else if (name_ == "")
  {
    context_->print ("ERROR: Container::Barrier needs a name.\n", 0);
  }
  else if (!context_)
  {
    context_->print ("ERROR: Container::Barrier needs a context.\n", 0);
  }
}

void
madara::knowledge::containers::Barrier::init_noharm (void)
{
  no_harm.always_overwrite = false;
  no_harm.delay_sending_modifieds = true;
  no_harm.expand_variables = false;
  no_harm.signal_changes = false;
  no_harm.track_local_changes = false;
  no_harm.treat_globals_as_locals = true;
}

size_t
madara::knowledge::containers::Barrier::get_id (void) const
{
  MADARA_GUARD_TYPE guard (mutex_);
  return id_;
}

size_t
madara::knowledge::containers::Barrier::get_participants (void) const
{
  MADARA_GUARD_TYPE guard (mutex_);
  return participants_;
}

void
madara::knowledge::containers::Barrier::set_name (
  const std::string & var_name,
  KnowledgeBase & knowledge,
  int id,
  int participants)
{
  KnowledgeUpdateSettings keep_local (true);
  context_ = &(knowledge.get_context ());

  ContextGuard context_guard (*context_);
  MADARA_GUARD_TYPE guard (mutex_);

  name_ = var_name;
  id_ = id;
  participants_ = participants;

  this->build_var ();
  this->build_aggregate_barrier ();
}

void
madara::knowledge::containers::Barrier::set_name (
  const std::string & var_name,
  Variables & knowledge,
  int id,
  int participants)
{
  KnowledgeUpdateSettings keep_local (true);
  context_ = knowledge.get_context ();

  ContextGuard context_guard (*context_);
  MADARA_GUARD_TYPE guard (mutex_);

  name_ = var_name;
  id_ = id;
  participants_ = participants;

  this->build_var ();
  this->build_aggregate_barrier ();
}

void
madara::knowledge::containers::Barrier::set_name (
  const std::string & var_name,
  ThreadSafeContext & knowledge,
  int id,
  int participants)
{
  KnowledgeUpdateSettings keep_local (true);
  context_ = &knowledge;

  ContextGuard context_guard (*context_);
  MADARA_GUARD_TYPE guard (mutex_);

  name_ = var_name;
  id_ = id;
  participants_ = participants;

  this->build_var ();
  this->build_aggregate_barrier ();
}

void
madara::knowledge::containers::Barrier::resize (size_t id, size_t participants)
{
  if (context_)
  {
    ContextGuard context_guard (*context_);
    MADARA_GUARD_TYPE guard (mutex_);

    id_ = id;
    participants_ = participants;

    this->build_var ();
    this->build_aggregate_barrier ();
  }
}

madara::knowledge::containers::Barrier::type
madara::knowledge::containers::Barrier::operator= (type value)
{
  if (context_)
  {
    ContextGuard context_guard (*context_);
    MADARA_GUARD_TYPE guard (mutex_);
    context_->set (variable_, value, settings_);
  }

  return value;
}

madara::knowledge::KnowledgeRecord
madara::knowledge::containers::Barrier::to_record (void) const
{
  madara::knowledge::KnowledgeRecord result;
  
  if (context_)
  {
    ContextGuard context_guard (*context_);
    MADARA_GUARD_TYPE guard (mutex_);
    result = context_->get (variable_, no_harm);
  }
  
  return result;
}
 
madara::knowledge::KnowledgeRecord::Integer
madara::knowledge::containers::Barrier::to_integer (void) const
{
  madara::knowledge::KnowledgeRecord::Integer result (0);
  
  if (context_)
  {
    ContextGuard context_guard (*context_);
    MADARA_GUARD_TYPE guard (mutex_);
    result = context_->get (variable_, no_harm).to_integer ();
  }

  return result;
}


void
madara::knowledge::containers::Barrier::next (void)
{
  if (context_)
  {
    ContextGuard context_guard (*context_);
    MADARA_GUARD_TYPE guard (mutex_);
    context_->inc (variable_, settings_);
  }
}
 
bool
madara::knowledge::containers::Barrier::is_done (void)
{
  bool result = false;

  if (context_ && name_ != "")
  {
    ContextGuard context_guard (*context_);
    MADARA_GUARD_TYPE guard (mutex_);

    madara_logger_log (context_->get_logger (), logger::LOG_MAJOR,
      "Barrier::is_done: checking barrier result for done\n");

    result = barrier_result () == 1;

    if (!result)
    {
      madara_logger_log (context_->get_logger (), logger::LOG_MAJOR,
        "Barrier::is_true: barrier is not true, remarking barrier variable\n");

      context_->mark_modified (variable_);

      context_->print (logger::LOG_DETAILED);
    }
    else
    {
      madara_logger_log (context_->get_logger (), logger::LOG_MAJOR,
        "Barrier::is_true: barrier is true\n");
    }
  }

  return result;
}

void
madara::knowledge::containers::Barrier::set (type value)
{
  ContextGuard context_guard (*context_);
  if (context_ && name_ != "")
  {
    context_->set (variable_, value, settings_);
  }
}

void
madara::knowledge::containers::Barrier::modify (void)
{
  ContextGuard context_guard (*context_);
  if (context_ && name_ != "")
  {
    context_->mark_modified (variable_);
  }
}

std::string
madara::knowledge::containers::Barrier::get_debug_info (void)
{
  std::stringstream result;

  result << "Barrier: ";

  if (context_)
  {
    ContextGuard context_guard (*context_);
    MADARA_GUARD_TYPE guard (mutex_);

    result << this->name_;
    result << " = " << context_->get (variable_).to_string ();
  }

  return result.str ();
}

void
madara::knowledge::containers::Barrier::modify_ (void)
{
  modify ();
}

std::string
madara::knowledge::containers::Barrier::get_debug_info_ (void)
{
  return get_debug_info ();
}

madara::knowledge::containers::BaseContainer *
madara::knowledge::containers::Barrier::clone (void) const
{
  return new Barrier (*this);
}

double
madara::knowledge::containers::Barrier::to_double (void) const
{
  double result (0.0);

  if (context_)
  {
    ContextGuard context_guard (*context_);
    MADARA_GUARD_TYPE guard (mutex_);
    result = context_->get (variable_, no_harm).to_double ();
  }

  return result;
}
   
std::string
madara::knowledge::containers::Barrier::to_string (void) const
{
  std::string result;

  if (context_)
  {
    ContextGuard context_guard (*context_);
    MADARA_GUARD_TYPE guard (mutex_);
    result = context_->get (variable_, no_harm).to_string ();
  }

  return result;
}

void
madara::knowledge::containers::Barrier::set_quality (
  uint32_t quality,
  const KnowledgeReferenceSettings & settings)
{
  if (context_)
  {
    ContextGuard context_guard (*context_);
    MADARA_GUARD_TYPE guard (mutex_);
    context_->set_quality (name_, quality, true, settings);
  }
}

bool
madara::knowledge::containers::Barrier::is_true (void) const
{
  bool result (false);

  madara_logger_log (context_->get_logger (), logger::LOG_MAJOR,
    "Barrier::is_true: checking barrier result for truth\n");

  if (context_)
  {
    ContextGuard context_guard (*context_);
    MADARA_GUARD_TYPE guard (mutex_);
    result = barrier_result () == 1;
  }

  madara_logger_log (context_->get_logger (), logger::LOG_MAJOR,
    "Barrier::is_true: final result is %d\n", (int)result);

  return result;
}

bool
madara::knowledge::containers::Barrier::is_false (void) const
{
  return !is_true ();
}


bool
madara::knowledge::containers::Barrier::is_true_ (void) const
{
  return is_true ();
}

bool
madara::knowledge::containers::Barrier::is_false_ (void) const
{
  return is_false ();
}


#endif // _MADARA_NO_KARL_
