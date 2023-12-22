
#ifndef _MADARA_NO_KARL_

#include <sstream>

#include "Counter.h"
#include "madara/knowledge/ContextGuard.h"

madara::knowledge::containers::Counter::Counter(
    const KnowledgeUpdateSettings& settings)
  : BaseContainer("", settings), context_(0), id_(0), counters_(1)
{
  init_noharm();
}

madara::knowledge::containers::Counter::Counter(const std::string& name,
    KnowledgeBase& knowledge, const KnowledgeUpdateSettings& settings)
  : BaseContainer(name, settings),
    context_(&(knowledge.get_context())),
    id_(0),
    counters_(1)
{
  init_noharm();
  build_var();
  build_aggregate_count();
}

madara::knowledge::containers::Counter::Counter(const std::string& name,
    Variables& knowledge, const KnowledgeUpdateSettings& settings)
  : BaseContainer(name, settings),
    context_(knowledge.get_context()),
    id_(0),
    counters_(1)
{
  init_noharm();
  build_var();
  build_aggregate_count();
}

madara::knowledge::containers::Counter::Counter(const std::string& name,
    KnowledgeBase& knowledge, int id, int counters, type value,
    const KnowledgeUpdateSettings& settings)
  : BaseContainer(name, settings),
    context_(&(knowledge.get_context())),
    id_(id),
    counters_(counters)
{
  init_noharm();
  build_var();
  build_aggregate_count();
  context_->set(variable_, value, settings);
}

madara::knowledge::containers::Counter::Counter(const std::string& name,
    Variables& knowledge, int id, int counters, type value,
    const KnowledgeUpdateSettings& settings)
  : BaseContainer(name, settings),
    context_(knowledge.get_context()),
    id_(id),
    counters_(counters)
{
  init_noharm();
  build_var();
  build_aggregate_count();
  context_->set(variable_, value, settings);
}

madara::knowledge::containers::Counter::Counter(const Counter& rhs)
  : BaseContainer(rhs),
    context_(rhs.context_),
    variable_(rhs.variable_),
    id_(rhs.id_),
    counters_(rhs.counters_),
    aggregate_count_(rhs.aggregate_count_)
{
}

madara::knowledge::containers::Counter::~Counter() {}

void madara::knowledge::containers::Counter::operator=(const Counter& rhs)
{
  if (this != &rhs)
  {
    MADARA_GUARD_TYPE guard(mutex_), guard2(rhs.mutex_);

    this->context_ = rhs.context_;
    this->name_ = rhs.name_;
    this->id_ = rhs.id_;
    this->counters_ = rhs.counters_;
    this->settings_ = rhs.settings_;
    this->variable_ = rhs.variable_;
    this->aggregate_count_ = rhs.aggregate_count_;
  }
}

void madara::knowledge::containers::Counter::build_aggregate_count(void)
{
  if (context_ && name_ != "")
  {
    ContextGuard context_guard(*context_);
    MADARA_GUARD_TYPE guard(mutex_);

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

    aggregate_count_ = context_->compile(buffer.str());
  }
  else if (name_ == "")
  {
    context_->print("ERROR: containers::Counter needs a name.\n", 0);
  }
  else if (!context_)
  {
    logger::global_logger->log(
      logger::LOG_ERROR, "ERROR: containers::Counter needs a context.\n");
  }
}

void madara::knowledge::containers::Counter::build_var(void)
{
  if (context_ && name_ != "")
  {
    ContextGuard context_guard(*context_);
    MADARA_GUARD_TYPE guard(mutex_);

    std::stringstream buffer;

    buffer << name_;
    buffer << ".";
    buffer << id_;

    variable_ = context_->get_ref(buffer.str(), no_harm);
  }
  else if (name_ == "")
  {
    context_->print("ERROR: Container::Counter needs a name.\n", 0);
  }
  else if (!context_)
  {
    context_->print("ERROR: Container::Counter needs a context.\n", 0);
  }
}

void madara::knowledge::containers::Counter::init_noharm(void)
{
  no_harm.always_overwrite = false;
  no_harm.delay_sending_modifieds = true;
  no_harm.expand_variables = false;
  no_harm.signal_changes = false;
  no_harm.track_local_changes = false;
  no_harm.treat_globals_as_locals = true;
}

void madara::knowledge::containers::Counter::modify(void)
{
  ContextGuard context_guard(*context_);
  if (context_ && name_ != "")
  {
    context_->mark_modified(variable_);
  }
}

std::string madara::knowledge::containers::Counter::get_debug_info(void)
{
  std::stringstream result;

  result << "Counter: ";

  if (context_)
  {
    ContextGuard context_guard(*context_);
    MADARA_GUARD_TYPE guard(mutex_);

    result << this->name_;
    result << " = " << context_->get(variable_).to_string();
    result << " (total: " << get_count() << ")";
  }

  return result.str();
}

void madara::knowledge::containers::Counter::modify_(void)
{
  modify();
}

std::string madara::knowledge::containers::Counter::get_debug_info_(void)
{
  return get_debug_info();
}

madara::knowledge::containers::BaseContainer*
madara::knowledge::containers::Counter::clone(void) const
{
  return new Counter(*this);
}

int madara::knowledge::containers::Counter::get_id(void) const
{
  MADARA_GUARD_TYPE guard(mutex_);
  return id_;
}

int madara::knowledge::containers::Counter::get_counters(void) const
{
  MADARA_GUARD_TYPE guard(mutex_);
  return counters_;
}

void madara::knowledge::containers::Counter::set_name(
    const std::string& var_name, KnowledgeBase& knowledge)
{
  KnowledgeUpdateSettings keep_local(true);
  context_ = &(knowledge.get_context());

  ContextGuard context_guard(*context_);
  MADARA_GUARD_TYPE guard(mutex_);

  name_ = var_name;

  this->build_var();
  this->build_aggregate_count();
}

void madara::knowledge::containers::Counter::set_name(
    const std::string& var_name, Variables& knowledge)
{
  KnowledgeUpdateSettings keep_local(true);
  context_ = knowledge.get_context();

  ContextGuard context_guard(*context_);
  MADARA_GUARD_TYPE guard(mutex_);

  name_ = var_name;

  this->build_var();
  this->build_aggregate_count();
}

void madara::knowledge::containers::Counter::resize(int id, int counters)
{
  if (context_)
  {
    ContextGuard context_guard(*context_);
    MADARA_GUARD_TYPE guard(mutex_);

    id_ = id;
    counters_ = counters;

    this->build_var();
    this->build_aggregate_count();
  }
}

madara::knowledge::containers::Counter::type
madara::knowledge::containers::Counter::operator=(type value)
{
  if (context_)
  {
    ContextGuard context_guard(*context_);
    MADARA_GUARD_TYPE guard(mutex_);
    context_->set(variable_, value, settings_);
  }

  return value;
}

bool madara::knowledge::containers::Counter::operator==(type value) const
{
  if (context_)
  {
    ContextGuard context_guard(*context_);
    MADARA_GUARD_TYPE guard(mutex_);
    return get_count() == value;
  }

  return false;
}

bool madara::knowledge::containers::Counter::operator!=(type value) const
{
  if (context_)
  {
    ContextGuard context_guard(*context_);
    MADARA_GUARD_TYPE guard(mutex_);
    return get_count() != value;
  }

  return true;
}

bool madara::knowledge::containers::Counter::operator==(
    const Counter& value) const
{
  if (context_)
  {
    ContextGuard context_guard(*context_);
    MADARA_GUARD_TYPE guard(mutex_);
    return get_count() == value.get_count();
  }

  return false;
}

bool madara::knowledge::containers::Counter::operator!=(
    const Counter& value) const
{
  if (context_)
  {
    ContextGuard context_guard(*context_);
    MADARA_GUARD_TYPE guard(mutex_);
    return get_count() != value.get_count();
  }

  return true;
}

bool madara::knowledge::containers::Counter::operator<(type value) const
{
  if (context_)
  {
    ContextGuard context_guard(*context_);
    MADARA_GUARD_TYPE guard(mutex_);
    return get_count() < value;
  }

  return false;
}

bool madara::knowledge::containers::Counter::operator<=(type value) const
{
  if (context_)
  {
    ContextGuard context_guard(*context_);
    MADARA_GUARD_TYPE guard(mutex_);
    return get_count() <= value;
  }

  return false;
}

bool madara::knowledge::containers::Counter::operator>(type value) const
{
  if (context_)
  {
    ContextGuard context_guard(*context_);
    MADARA_GUARD_TYPE guard(mutex_);
    return get_count() > value;
  }

  return false;
}

bool madara::knowledge::containers::Counter::operator>=(type value) const
{
  if (context_)
  {
    ContextGuard context_guard(*context_);
    MADARA_GUARD_TYPE guard(mutex_);
    return get_count() >= value;
  }

  return false;
}

madara::knowledge::containers::Counter::type
    madara::knowledge::containers::Counter::operator*(void)const
{
  return to_integer();
}

madara::knowledge::KnowledgeRecord
madara::knowledge::containers::Counter::to_record(void) const
{
  madara::knowledge::KnowledgeRecord result;

  if (context_)
  {
    ContextGuard context_guard(*context_);
    MADARA_GUARD_TYPE guard(mutex_);
    result = get_count_record();
  }

  return result;
}

madara::knowledge::KnowledgeRecord::Integer
madara::knowledge::containers::Counter::to_integer(void) const
{
  madara::knowledge::KnowledgeRecord::Integer result(0);

  if (context_)
  {
    ContextGuard context_guard(*context_);
    MADARA_GUARD_TYPE guard(mutex_);
    result = get_count();
  }

  return result;
}

void madara::knowledge::containers::Counter::operator+=(type value)
{
  if (context_)
  {
    type current(0);
    ContextGuard context_guard(*context_);
    MADARA_GUARD_TYPE guard(mutex_);

    current = context_->get(variable_, settings_).to_integer();
    current += value;
    context_->set(variable_, current, settings_);
  }
}

void madara::knowledge::containers::Counter::operator-=(type value)
{
  if (context_)
  {
    type current(0);
    ContextGuard context_guard(*context_);
    MADARA_GUARD_TYPE guard(mutex_);

    current = context_->get(variable_, settings_).to_integer();
    current -= value;
    context_->set(variable_, current, settings_);
  }
}

void madara::knowledge::containers::Counter::operator++(void)
{
  if (context_)
  {
    ContextGuard context_guard(*context_);
    MADARA_GUARD_TYPE guard(mutex_);
    context_->inc(variable_, settings_);
  }
}

void madara::knowledge::containers::Counter::operator--(void)
{
  if (context_)
  {
    ContextGuard context_guard(*context_);
    MADARA_GUARD_TYPE guard(mutex_);
    context_->dec(variable_, settings_);
  }
}

double madara::knowledge::containers::Counter::to_double(void) const
{
  double result(0.0);

  if (context_)
  {
    ContextGuard context_guard(*context_);
    MADARA_GUARD_TYPE guard(mutex_);
    result = get_count_double();
  }

  return result;
}

std::string madara::knowledge::containers::Counter::to_string(void) const
{
  std::string result;

  if (context_)
  {
    ContextGuard context_guard(*context_);
    MADARA_GUARD_TYPE guard(mutex_);
    result = get_count_string();
  }

  return result;
}

void madara::knowledge::containers::Counter::set_quality(
    uint32_t quality, const KnowledgeReferenceSettings& settings)
{
  if (context_)
  {
    ContextGuard context_guard(*context_);
    MADARA_GUARD_TYPE guard(mutex_);
    context_->set_quality(name_, quality, true, settings);
  }
}

bool madara::knowledge::containers::Counter::is_true(void) const
{
  bool result(false);

  madara_logger_log(context_->get_logger(), logger::LOG_MAJOR,
      "Counter::is_true: checking local counter for non-zero\n");

  if (context_)
  {
    ContextGuard context_guard(*context_);
    MADARA_GUARD_TYPE guard(mutex_);
    result = context_->get(variable_, settings_).is_true();
  }

  madara_logger_log(context_->get_logger(), logger::LOG_MAJOR,
      "Counter::is_true: final result is %d\n", (int)result);

  return result;
}

bool madara::knowledge::containers::Counter::is_false(void) const
{
  return !is_true();
}

bool madara::knowledge::containers::Counter::is_true_(void) const
{
  return is_true();
}

bool madara::knowledge::containers::Counter::is_false_(void) const
{
  return is_false();
}

#endif  // _MADARA_NO_KARL_
