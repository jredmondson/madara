#ifndef _MADARA_KNOWLEDGE_CONTAINERS_STRINGSTAGED_INL_
#define _MADARA_KNOWLEDGE_CONTAINERS_STRINGSTAGED_INL_

#include "StringStaged.h"
#include "madara/knowledge/ContextGuard.h"

inline void madara::knowledge::containers::StringStaged::modify(void)
{
  ContextGuard context_guard(*context_);
  if(context_ && name_ != "")
  {
    context_->mark_modified(variable_);
  }
}

inline std::string madara::knowledge::containers::StringStaged::get_debug_info(
    void)
{
  std::stringstream result;

  result << "StringStaged: ";

  if(context_)
  {
    ContextGuard context_guard(*context_);
    MADARA_GUARD_TYPE guard(mutex_);

    result << this->name_;
    result << " = " << context_->get(variable_).to_string();
  }

  return result.str();
}

inline madara::knowledge::containers::BaseContainer*
madara::knowledge::containers::StringStaged::clone(void) const
{
  return new StringStaged(*this);
}

inline void madara::knowledge::containers::StringStaged::operator=(
    const StringStaged& rhs)
{
  if(this != &rhs)
  {
    MADARA_GUARD_TYPE guard(mutex_), guard2(rhs.mutex_);

    this->has_changed_ = rhs.has_changed_;
    this->value_ = rhs.value_;
    this->context_ = rhs.context_;
    this->name_ = rhs.name_;
    this->settings_ = rhs.settings_;
    this->variable_ = rhs.variable_;
  }
}

inline void madara::knowledge::containers::StringStaged::set_name(
    const std::string& var_name, KnowledgeBase& knowledge, bool sync)
{
  KnowledgeUpdateSettings keep_local(true);
  context_ = &(knowledge.get_context());

  ContextGuard context_guard(*context_);
  MADARA_GUARD_TYPE guard(mutex_);

  name_ = var_name;
  variable_ = context_->get_ref(name_, keep_local);

  if(sync)
  {
    value_ = context_->get(variable_).to_string();
    has_changed_ = false;
  }
}

inline void madara::knowledge::containers::StringStaged::set_name(
    const std::string& var_name, ThreadSafeContext& knowledge, bool sync)
{
  KnowledgeUpdateSettings keep_local(true);
  context_ = &knowledge;

  ContextGuard context_guard(*context_);
  MADARA_GUARD_TYPE guard(mutex_);

  name_ = var_name;
  variable_ = context_->get_ref(name_, keep_local);

  if(sync)
  {
    value_ = context_->get(variable_).to_string();
    has_changed_ = false;
  }
}

inline void madara::knowledge::containers::StringStaged::set_name(
    const std::string& var_name, Variables& knowledge, bool sync)
{
  KnowledgeUpdateSettings keep_local(true);
  context_ = knowledge.get_context();

  ContextGuard context_guard(*context_);
  MADARA_GUARD_TYPE guard(mutex_);

  name_ = var_name;
  variable_ = context_->get_ref(name_, keep_local);

  if(sync)
  {
    value_ = context_->get(variable_).to_string();
    has_changed_ = false;
  }
}

inline madara::knowledge::containers::StringStaged::type
madara::knowledge::containers::StringStaged::operator=(const type& value)
{
  has_changed_ = true;
  return value_ = value;
}

inline bool madara::knowledge::containers::StringStaged::operator==(
    const type& value) const
{
  return value_ == value;
}

inline bool madara::knowledge::containers::StringStaged::operator!=(
    const type& value) const
{
  return value_ != value;
}

inline bool madara::knowledge::containers::StringStaged::operator==(
    const StringStaged& value) const
{
  return value_ == value.value_;
}

inline bool madara::knowledge::containers::StringStaged::operator!=(
    const StringStaged& value) const
{
  return value_ != value.value_;
}

inline bool madara::knowledge::containers::StringStaged::operator<(
    const type& value) const
{
  return value_ < value;
}

inline bool madara::knowledge::containers::StringStaged::operator<=(
    const type& value) const
{
  return value_ <= value;
}

inline bool madara::knowledge::containers::StringStaged::operator>(
    const type& value) const
{
  return value_ > value;
}

inline bool madara::knowledge::containers::StringStaged::operator>=(
    const type& value) const
{
  return value_ >= value;
}

inline madara::knowledge::containers::StringStaged::type
    madara::knowledge::containers::StringStaged::operator*(void)const
{
  return to_string();
}

inline bool madara::knowledge::containers::StringStaged::exists(void) const
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

inline bool madara::knowledge::containers::StringStaged::is_true(void) const
{
  return value_ != "";
}

inline bool madara::knowledge::containers::StringStaged::is_false(void) const
{
  return value_ == "";
}

inline madara::knowledge::KnowledgeRecord
madara::knowledge::containers::StringStaged::to_record(void) const
{
  return KnowledgeRecord(value_);
}

inline madara::knowledge::KnowledgeRecord::Integer
madara::knowledge::containers::StringStaged::to_integer(void) const
{
  KnowledgeRecord value(value_);
  return value.to_integer();
}

inline madara::knowledge::containers::StringStaged::type
madara::knowledge::containers::StringStaged::operator+=(const type& value)
{
  has_changed_ = true;
  value_ += value;

  return value_;
}

inline madara::knowledge::containers::StringStaged::type
madara::knowledge::containers::StringStaged::operator-=(const type& value)
{
  has_changed_ = true;
  value_ += value;

  return value_;
}

inline double madara::knowledge::containers::StringStaged::to_double(void) const
{
  KnowledgeRecord value(value_);
  return value.to_double();
}

inline std::string madara::knowledge::containers::StringStaged::to_string(
    void) const
{
  return value_;
}

inline void madara::knowledge::containers::StringStaged::set_quality(
    uint32_t quality, const KnowledgeReferenceSettings& settings)
{
  if(context_)
  {
    ContextGuard context_guard(*context_);
    MADARA_GUARD_TYPE guard(mutex_);
    context_->set_quality(name_, quality, true, settings);
  }
}

inline void madara::knowledge::containers::StringStaged::read(void)
{
  has_changed_ = false;
  value_ = context_->get(variable_).to_string();
}

inline void madara::knowledge::containers::StringStaged::write(void)
{
  if(has_changed_)
    context_->set(variable_, value_);
}

#endif  // _MADARA_KNOWLEDGE_CONTAINERS_STRINGSTAGED_INL_
