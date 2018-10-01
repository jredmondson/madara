#include "String.h"
#include "madara/knowledge/ContextGuard.h"

madara::knowledge::containers::String::String(
    const KnowledgeUpdateSettings& settings)
  : BaseContainer("", settings), context_(0)
{
}

madara::knowledge::containers::String::String(const std::string& name,
    KnowledgeBase& knowledge, const KnowledgeUpdateSettings& settings)
  : BaseContainer(name, settings), context_(&(knowledge.get_context()))
{
  variable_ = knowledge.get_ref(name, settings_);
  settings_ = settings;
}

madara::knowledge::containers::String::String(const std::string& name,
    Variables& knowledge, const KnowledgeUpdateSettings& settings)
  : BaseContainer(name, settings), context_(knowledge.get_context())
{
  variable_ = knowledge.get_ref(name, settings_);
  settings_ = settings;
}

madara::knowledge::containers::String::String(const std::string& name,
    KnowledgeBase& knowledge, const std::string& value,
    const KnowledgeUpdateSettings& settings)
  : BaseContainer(name, settings), context_(&(knowledge.get_context()))
{
  variable_ = knowledge.get_ref(name);
  knowledge.set(variable_, value);
}

madara::knowledge::containers::String::String(const std::string& name,
    Variables& knowledge, const std::string& value,
    const KnowledgeUpdateSettings& settings)
  : BaseContainer(name, settings), context_(knowledge.get_context())
{
  variable_ = knowledge.get_ref(name);
  knowledge.set(variable_, value);
}

madara::knowledge::containers::String::String(const String& rhs)
  : BaseContainer(rhs), context_(rhs.context_), variable_(rhs.variable_)
{
}

madara::knowledge::containers::String::~String() {}

void madara::knowledge::containers::String::modify(void)
{
  ContextGuard context_guard(*context_);
  if (context_ && name_ != "") {
    context_->mark_modified(variable_);
  }
}

std::string madara::knowledge::containers::String::get_debug_info(void)
{
  std::stringstream result;

  result << "String: ";

  if (context_) {
    ContextGuard context_guard(*context_);
    MADARA_GUARD_TYPE guard(mutex_);

    result << this->name_;
    result << " = " << context_->get(variable_).to_string();
  }

  return result.str();
}

void madara::knowledge::containers::String::modify_(void)
{
  modify();
}

std::string madara::knowledge::containers::String::get_debug_info_(void)
{
  return get_debug_info();
}

madara::knowledge::containers::BaseContainer*
madara::knowledge::containers::String::clone(void) const
{
  return new String(*this);
}

void madara::knowledge::containers::String::operator=(const String& rhs)
{
  if (this != &rhs) {
    MADARA_GUARD_TYPE guard(mutex_), guard2(rhs.mutex_);

    this->context_ = rhs.context_;
    this->name_ = rhs.name_;
    this->settings_ = rhs.settings_;
    this->variable_ = rhs.variable_;
  }
}

void madara::knowledge::containers::String::exchange(String& other)
{
  if (context_ && other.context_) {
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

void madara::knowledge::containers::String::set_name(
    const std::string& var_name, KnowledgeBase& knowledge)
{
  KnowledgeUpdateSettings keep_local(true);
  context_ = &(knowledge.get_context());

  ContextGuard context_guard(*context_);
  MADARA_GUARD_TYPE guard(mutex_);

  name_ = var_name;
  variable_ = context_->get_ref(name_, keep_local);
}

void madara::knowledge::containers::String::set_name(
    const std::string& var_name, Variables& knowledge)
{
  KnowledgeUpdateSettings keep_local(true);
  context_ = knowledge.get_context();

  ContextGuard context_guard(*context_);
  MADARA_GUARD_TYPE guard(mutex_);

  name_ = var_name;
  variable_ = context_->get_ref(name_, keep_local);
}

void madara::knowledge::containers::String::set_name(
    const std::string& var_name, ThreadSafeContext& knowledge)
{
  KnowledgeUpdateSettings keep_local(true);
  context_ = &knowledge;

  ContextGuard context_guard(*context_);
  MADARA_GUARD_TYPE guard(mutex_);

  name_ = var_name;
  variable_ = context_->get_ref(name_, keep_local);
}

madara::knowledge::containers::String::type
madara::knowledge::containers::String::operator=(const type& value)
{
  if (context_) {
    ContextGuard context_guard(*context_);
    MADARA_GUARD_TYPE guard(mutex_);
    context_->set(variable_, value, settings_);
  }

  return value;
}

madara::knowledge::containers::String::type
madara::knowledge::containers::String::operator+=(const type& value)
{
  madara::knowledge::containers::String::type result;

  if (context_) {
    ContextGuard context_guard(*context_);
    MADARA_GUARD_TYPE guard(mutex_);

    result = context_->get(variable_, settings_).to_string();
    result += value;
    context_->set(variable_, result, settings_);
  }

  return result;
}

bool madara::knowledge::containers::String::operator==(const type& value) const
{
  if (context_) {
    ContextGuard context_guard(*context_);
    MADARA_GUARD_TYPE guard(mutex_);
    return context_->get(variable_, settings_) == value;
  }

  return false;
}

bool madara::knowledge::containers::String::operator!=(const type& value) const
{
  if (context_) {
    ContextGuard context_guard(*context_);
    MADARA_GUARD_TYPE guard(mutex_);
    return context_->get(variable_, settings_).to_string() != value;
  }

  return true;
}

bool madara::knowledge::containers::String::operator==(
    const String& value) const
{
  if (context_) {
    ContextGuard context_guard(*context_);
    MADARA_GUARD_TYPE guard(mutex_);
    return context_->get(variable_, settings_) ==
           value.context_->get(value.variable_, value.settings_);
  }

  return false;
}

bool madara::knowledge::containers::String::operator!=(
    const String& value) const
{
  if (context_) {
    ContextGuard context_guard(*context_);
    MADARA_GUARD_TYPE guard(mutex_);
    return context_->get(variable_, settings_) !=
           value.context_->get(value.variable_, value.settings_);
  }

  return true;
}

bool madara::knowledge::containers::String::operator<(const type& value) const
{
  if (context_) {
    ContextGuard context_guard(*context_);
    MADARA_GUARD_TYPE guard(mutex_);
    return context_->get(variable_, settings_).to_string() < value;
  }

  return false;
}

bool madara::knowledge::containers::String::operator<=(const type& value) const
{
  if (context_) {
    ContextGuard context_guard(*context_);
    MADARA_GUARD_TYPE guard(mutex_);
    return context_->get(variable_, settings_).to_string() <= value;
  }

  return false;
}

bool madara::knowledge::containers::String::operator>(const type& value) const
{
  if (context_) {
    ContextGuard context_guard(*context_);
    MADARA_GUARD_TYPE guard(mutex_);
    return context_->get(variable_, settings_).to_string() > value;
  }

  return false;
}

bool madara::knowledge::containers::String::operator>=(const type& value) const
{
  if (context_) {
    ContextGuard context_guard(*context_);
    MADARA_GUARD_TYPE guard(mutex_);
    return context_->get(variable_, settings_).to_string() >= value;
  }

  return false;
}

madara::knowledge::containers::String::type
    madara::knowledge::containers::String::operator*(void)const
{
  return to_string();
}

bool madara::knowledge::containers::String::exists(void) const
{
  bool result(false);

  if (context_) {
    ContextGuard context_guard(*context_);
    MADARA_GUARD_TYPE guard(mutex_);
    result = context_->exists(variable_);
  }

  return result;
}

madara::knowledge::KnowledgeRecord
madara::knowledge::containers::String::to_record(void) const
{
  madara::knowledge::KnowledgeRecord result;

  if (context_) {
    ContextGuard context_guard(*context_);
    MADARA_GUARD_TYPE guard(mutex_);
    result = context_->get(variable_, settings_);
  }

  return result;
}

std::string madara::knowledge::containers::String::to_string(void) const
{
  if (context_) {
    ContextGuard context_guard(*context_);
    MADARA_GUARD_TYPE guard(mutex_);
    return context_->get(variable_, settings_).to_string();
  } else
    return "";
}

double madara::knowledge::containers::String::to_double(void) const
{
  if (context_) {
    ContextGuard context_guard(*context_);
    MADARA_GUARD_TYPE guard(mutex_);
    return context_->get(variable_, settings_).to_double();
  } else
    return 0.0;
}

madara::knowledge::KnowledgeRecord::Integer
madara::knowledge::containers::String::to_integer(void) const
{
  if (context_) {
    ContextGuard context_guard(*context_);
    MADARA_GUARD_TYPE guard(mutex_);
    return context_->get(variable_, settings_).to_integer();
  } else
    return 0;
}

void madara::knowledge::containers::String::set_quality(
    uint32_t quality, const KnowledgeReferenceSettings& settings)
{
  if (context_) {
    ContextGuard context_guard(*context_);
    MADARA_GUARD_TYPE guard(mutex_);
    context_->set_quality(name_, quality, true, settings);
  }
}

bool madara::knowledge::containers::String::is_true(void) const
{
  bool result(false);

  madara_logger_log(context_->get_logger(), logger::LOG_MAJOR,
      "String::is_true: checking for non-zero value\n");

  if (context_) {
    ContextGuard context_guard(*context_);
    MADARA_GUARD_TYPE guard(mutex_);
    result = context_->get(variable_, settings_).is_true();
  }

  madara_logger_log(context_->get_logger(), logger::LOG_MAJOR,
      "String::is_true: final result is %d\n", (int)result);

  return result;
}

bool madara::knowledge::containers::String::is_false(void) const
{
  return !is_true();
}

bool madara::knowledge::containers::String::is_true_(void) const
{
  return is_true();
}

bool madara::knowledge::containers::String::is_false_(void) const
{
  return is_false();
}
