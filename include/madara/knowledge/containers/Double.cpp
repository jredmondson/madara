#include "Double.h"
#include "madara/knowledge/ContextGuard.h"

madara::knowledge::containers::Double::Double(
    const KnowledgeUpdateSettings& settings)
  : BaseContainer("", settings), context_(0)
{
}

madara::knowledge::containers::Double::Double(const std::string& name,
    KnowledgeBase& knowledge, const KnowledgeUpdateSettings& settings)
  : BaseContainer(name, settings), context_(&(knowledge.get_context()))
{
  variable_ = knowledge.get_ref(name, settings_);
}

madara::knowledge::containers::Double::Double(const std::string& name,
    Variables& knowledge, const KnowledgeUpdateSettings& settings)
  : BaseContainer(name, settings), context_(knowledge.get_context())
{
  variable_ = knowledge.get_ref(name, settings_);
}

madara::knowledge::containers::Double::Double(const std::string& name,
    KnowledgeBase& knowledge, type value,
    const KnowledgeUpdateSettings& settings)
  : BaseContainer(name, settings), context_(&(knowledge.get_context()))
{
  variable_ = knowledge.get_ref(name);
  context_->set(variable_, value, settings);
}

madara::knowledge::containers::Double::Double(const std::string& name,
    Variables& knowledge, type value, const KnowledgeUpdateSettings& settings)
  : BaseContainer(name, settings), context_(knowledge.get_context())
{
  variable_ = knowledge.get_ref(name);
  context_->set(variable_, value, settings);
}

madara::knowledge::containers::Double::Double(const Double& rhs)
  : BaseContainer(rhs), context_(rhs.context_), variable_(rhs.variable_)
{
}

madara::knowledge::containers::Double::~Double() {}

void madara::knowledge::containers::Double::modify(void)
{
  ContextGuard context_guard(*context_);
  if(context_ && name_ != "")
  {
    context_->mark_modified(variable_);
  }
}

std::string madara::knowledge::containers::Double::get_debug_info(void)
{
  std::stringstream result;

  result << "Double: ";

  if(context_)
  {
    ContextGuard context_guard(*context_);
    MADARA_GUARD_TYPE guard(mutex_);

    result << this->name_;
    result << " = " << context_->get(variable_).to_string();
  }

  return result.str();
}

void madara::knowledge::containers::Double::modify_(void)
{
  modify();
}

std::string madara::knowledge::containers::Double::get_debug_info_(void)
{
  return get_debug_info();
}

madara::knowledge::containers::BaseContainer*
madara::knowledge::containers::Double::clone(void) const
{
  return new Double(*this);
}

void madara::knowledge::containers::Double::operator=(const Double& rhs)
{
  if(this != &rhs)
  {
    MADARA_GUARD_TYPE guard(mutex_), guard2(rhs.mutex_);

    this->context_ = rhs.context_;
    this->name_ = rhs.name_;
    this->settings_ = rhs.settings_;
    this->variable_ = rhs.variable_;
  }
}

void madara::knowledge::containers::Double::exchange(containers::Double& other)
{
  if(context_ && other.context_)
  {
    std::lock(*context_, *other.context_, mutex_, other.mutex_);

    ContextGuard context_guard(*context_, std::adopt_lock);
    ContextGuard other_context_guard(*other.context_, std::adopt_lock);
    MADARA_GUARD_TYPE guard(mutex_, std::adopt_lock),
        guard2(other.mutex_, std::adopt_lock);

    type temp = *other;
    other = **this;
    *this = temp;
  }
}

void madara::knowledge::containers::Double::set_name(
    const std::string& var_name, KnowledgeBase& knowledge)
{
  KnowledgeUpdateSettings keep_local(true);
  context_ = &(knowledge.get_context());

  ContextGuard context_guard(*context_);
  MADARA_GUARD_TYPE guard(mutex_);

  name_ = var_name;
  variable_ = context_->get_ref(name_, keep_local);
}

void madara::knowledge::containers::Double::set_name(
    const std::string& var_name, Variables& knowledge)
{
  KnowledgeUpdateSettings keep_local(true);
  context_ = knowledge.get_context();

  ContextGuard context_guard(*context_);
  MADARA_GUARD_TYPE guard(mutex_);

  name_ = var_name;
  variable_ = context_->get_ref(name_, keep_local);
}

void madara::knowledge::containers::Double::set_name(
    const std::string& var_name, ThreadSafeContext& knowledge)
{
  KnowledgeUpdateSettings keep_local(true);
  context_ = &knowledge;

  ContextGuard context_guard(*context_);
  MADARA_GUARD_TYPE guard(mutex_);

  name_ = var_name;
  variable_ = context_->get_ref(name_, keep_local);
}

madara::knowledge::containers::Double::type
madara::knowledge::containers::Double::operator=(type value)
{
  if(context_)
  {
    ContextGuard context_guard(*context_);
    MADARA_GUARD_TYPE guard(mutex_);
    context_->set(variable_, value, settings_);
  }

  return value;
}

madara::knowledge::containers::Double::type
madara::knowledge::containers::Double::operator+=(type value)
{
  madara::knowledge::containers::Double::type result(0);

  if(context_)
  {
    ContextGuard context_guard(*context_);
    MADARA_GUARD_TYPE guard(mutex_);

    result = context_->get(variable_, settings_).to_double();
    result += value;
    context_->set(variable_, result, settings_);
  }

  return result;
}

madara::knowledge::containers::Double::type
madara::knowledge::containers::Double::operator-=(type value)
{
  madara::knowledge::containers::Double::type result(0);

  if(context_)
  {
    ContextGuard context_guard(*context_);
    MADARA_GUARD_TYPE guard(mutex_);

    result = context_->get(variable_, settings_).to_double();
    result -= value;
    context_->set(variable_, result, settings_);
  }

  return result;
}
madara::knowledge::containers::Double::type
    madara::knowledge::containers::Double::operator++(void)
{
  if(context_)
  {
    ContextGuard context_guard(*context_);
    MADARA_GUARD_TYPE guard(mutex_);
    return context_->inc(variable_, settings_).to_double();
  }
  else
    return 0;
}

madara::knowledge::containers::Double::type
    madara::knowledge::containers::Double::operator--(void)
{
  if(context_)
  {
    ContextGuard context_guard(*context_);
    MADARA_GUARD_TYPE guard(mutex_);
    return context_->dec(variable_, settings_).to_double();
  }
  else
    return 0;
}

bool madara::knowledge::containers::Double::operator==(type value) const
{
  if(context_)
  {
    ContextGuard context_guard(*context_);
    MADARA_GUARD_TYPE guard(mutex_);
    return context_->get(variable_, settings_) == value;
  }

  return false;
}

bool madara::knowledge::containers::Double::operator!=(type value) const
{
  if(context_)
  {
    ContextGuard context_guard(*context_);
    MADARA_GUARD_TYPE guard(mutex_);
    return context_->get(variable_, settings_).to_double() != value;
  }

  return true;
}

bool madara::knowledge::containers::Double::operator==(
    const Double& value) const
{
  if(context_)
  {
    ContextGuard context_guard(*context_);
    MADARA_GUARD_TYPE guard(mutex_);
    return context_->get(variable_, settings_) ==
           value.context_->get(value.variable_, value.settings_);
  }

  return false;
}

bool madara::knowledge::containers::Double::operator!=(
    const Double& value) const
{
  if(context_)
  {
    ContextGuard context_guard(*context_);
    MADARA_GUARD_TYPE guard(mutex_);
    return context_->get(variable_, settings_) !=
           value.context_->get(value.variable_, value.settings_);
  }

  return true;
}

bool madara::knowledge::containers::Double::operator<(type value) const
{
  if(context_)
  {
    ContextGuard context_guard(*context_);
    MADARA_GUARD_TYPE guard(mutex_);
    return context_->get(variable_, settings_).to_double() < value;
  }

  return false;
}

bool madara::knowledge::containers::Double::operator<=(type value) const
{
  if(context_)
  {
    ContextGuard context_guard(*context_);
    MADARA_GUARD_TYPE guard(mutex_);
    return context_->get(variable_, settings_).to_double() <= value;
  }

  return false;
}

bool madara::knowledge::containers::Double::operator>(type value) const
{
  if(context_)
  {
    ContextGuard context_guard(*context_);
    MADARA_GUARD_TYPE guard(mutex_);
    return context_->get(variable_, settings_).to_double() > value;
  }

  return false;
}

bool madara::knowledge::containers::Double::operator>=(type value) const
{
  if(context_)
  {
    ContextGuard context_guard(*context_);
    MADARA_GUARD_TYPE guard(mutex_);
    return context_->get(variable_, settings_).to_double() >= value;
  }

  return false;
}

madara::knowledge::containers::Double::type
    madara::knowledge::containers::Double::operator*(void)const
{
  return to_double();
}

bool madara::knowledge::containers::Double::exists(void) const
{
  bool result(false);

  if(context_)
  {
    ContextGuard context_guard(*context_);
    MADARA_GUARD_TYPE guard(mutex_);
    result = context_->exists(variable_);
  }

  return result;
}

madara::knowledge::KnowledgeRecord
madara::knowledge::containers::Double::to_record(void) const
{
  madara::knowledge::KnowledgeRecord result;

  if(context_)
  {
    ContextGuard context_guard(*context_);
    MADARA_GUARD_TYPE guard(mutex_);
    result = context_->get(variable_, settings_);
  }

  return result;
}

double madara::knowledge::containers::Double::to_double(void) const
{
  if(context_)
  {
    ContextGuard context_guard(*context_);
    MADARA_GUARD_TYPE guard(mutex_);
    return context_->get(variable_, settings_).to_double();
  }
  else
    return 0.0;
}

madara::knowledge::KnowledgeRecord::Integer
madara::knowledge::containers::Double::to_integer(void) const
{
  if(context_)
  {
    ContextGuard context_guard(*context_);
    MADARA_GUARD_TYPE guard(mutex_);
    return context_->get(variable_, settings_).to_integer();
  }
  else
    return 0;
}

std::string madara::knowledge::containers::Double::to_string(void) const
{
  if(context_)
  {
    ContextGuard context_guard(*context_);
    MADARA_GUARD_TYPE guard(mutex_);
    return context_->get(variable_, settings_).to_string();
  }
  else
    return "";
}

void madara::knowledge::containers::Double::set_quality(
    uint32_t quality, const KnowledgeReferenceSettings& settings)
{
  if(context_)
  {
    ContextGuard context_guard(*context_);
    MADARA_GUARD_TYPE guard(mutex_);
    context_->set_quality(name_, quality, true, settings);
  }
}

bool madara::knowledge::containers::Double::is_true(void) const
{
  bool result(false);

  madara_logger_log(context_->get_logger(), logger::LOG_MAJOR,
      "Double::is_true: checking for non-zero value\n", (int)result);

  if(context_)
  {
    ContextGuard context_guard(*context_);
    MADARA_GUARD_TYPE guard(mutex_);
    result = context_->get(variable_, settings_).is_true();
  }

  madara_logger_log(context_->get_logger(), logger::LOG_MAJOR,
      "Double::is_true: final result is %d\n", (int)result);

  return result;
}

bool madara::knowledge::containers::Double::is_false(void) const
{
  return !is_true();
}

bool madara::knowledge::containers::Double::is_true_(void) const
{
  return is_true();
}

bool madara::knowledge::containers::Double::is_false_(void) const
{
  return is_false();
}
