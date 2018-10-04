#include "NativeIntegerVector.h"
#include "madara/knowledge/ContextGuard.h"

madara::knowledge::containers::NativeIntegerVector::NativeIntegerVector(
    const KnowledgeUpdateSettings& settings)
  : BaseContainer("", settings), context_(0)
{
}

madara::knowledge::containers::NativeIntegerVector::NativeIntegerVector(
    const std::string& name, KnowledgeBase& knowledge, int size,
    const KnowledgeUpdateSettings& settings)
  : BaseContainer(name, settings), context_(&(knowledge.get_context()))
{
  vector_ = knowledge.get_ref(name, KnowledgeUpdateSettings(true));
  if (size >= 0)
  {
    resize(size);
  }
}

madara::knowledge::containers::NativeIntegerVector::NativeIntegerVector(
    const std::string& name, Variables& knowledge, int size,
    const KnowledgeUpdateSettings& settings)
  : BaseContainer(name, settings), context_(knowledge.get_context())
{
  vector_ = knowledge.get_ref(name, settings);
  if (size >= 0)
  {
    resize(size);
  }
}

madara::knowledge::containers::NativeIntegerVector::NativeIntegerVector(
    const NativeIntegerVector& rhs)
  : BaseContainer(rhs), context_(rhs.context_), vector_(rhs.vector_)
{
}

madara::knowledge::containers::NativeIntegerVector::~NativeIntegerVector() {}

void madara::knowledge::containers::NativeIntegerVector::modify(void)
{
  ContextGuard context_guard(*context_);
  if (context_ && name_ != "")
  {
    context_->mark_modified(vector_);
  }
}

std::string madara::knowledge::containers::NativeIntegerVector::get_debug_info(
    void)
{
  std::stringstream result;

  result << "Native Integer Vector: ";

  if (context_)
  {
    ContextGuard context_guard(*context_);
    MADARA_GUARD_TYPE guard(mutex_);

    result << this->name_;
    result << " [" << size() << "]";
    result << " = " << context_->get(vector_).to_string();
  }

  return result.str();
}

void madara::knowledge::containers::NativeIntegerVector::modify_(void)
{
  modify();
}

std::string madara::knowledge::containers::NativeIntegerVector::get_debug_info_(
    void)
{
  return get_debug_info();
}

madara::knowledge::containers::BaseContainer*
madara::knowledge::containers::NativeIntegerVector::clone(void) const
{
  return new NativeIntegerVector(*this);
}

void madara::knowledge::containers::NativeIntegerVector::operator=(
    const NativeIntegerVector& rhs)
{
  if (this != &rhs)
  {
    MADARA_GUARD_TYPE guard(mutex_), guard2(rhs.mutex_);

    this->context_ = rhs.context_;
    this->name_ = rhs.name_;
    this->settings_ = rhs.settings_;
    this->vector_ = rhs.vector_;
  }
}

void madara::knowledge::containers::NativeIntegerVector::push_back(type value)
{
  if (context_ && name_ != "")
  {
    ContextGuard context_guard(*context_);
    MADARA_GUARD_TYPE guard(mutex_);

    size_t i = size();
    resize((int)i + 1);
    set(i, value);
  }
}

void madara::knowledge::containers::NativeIntegerVector::resize(size_t size)
{
  if (context_ && name_ != "")
  {
    ContextGuard context_guard(*context_);
    MADARA_GUARD_TYPE guard(mutex_);

    knowledge::KnowledgeRecord value = context_->get(vector_, settings_);

    value.resize(size);

    context_->set(vector_, value, settings_);
  }
}

size_t madara::knowledge::containers::NativeIntegerVector::size(void) const
{
  ContextGuard context_guard(*context_);
  MADARA_GUARD_TYPE guard(mutex_);

  return context_->get(vector_, settings_).size();
}

void madara::knowledge::containers::NativeIntegerVector::set_name(
    const std::string& var_name, KnowledgeBase& knowledge, int size)
{
  if (context_ != &(knowledge.get_context()) || name_ != var_name)
  {
    context_ = &(knowledge.get_context());

    ContextGuard context_guard(*context_);
    MADARA_GUARD_TYPE guard(mutex_);

    name_ = var_name;

    vector_ = knowledge.get_ref(var_name, settings_);

    if (size > 0)
      resize(size_t(size));
  }
}

void madara::knowledge::containers::NativeIntegerVector::set_name(
    const std::string& var_name, Variables& knowledge, int size)
{
  if (context_ != knowledge.get_context() || name_ != var_name)
  {
    context_ = knowledge.get_context();

    ContextGuard context_guard(*context_);
    MADARA_GUARD_TYPE guard(mutex_);

    name_ = var_name;

    vector_ = knowledge.get_ref(var_name, settings_);

    if (size > 0)
      resize(size_t(size));
  }
}

void madara::knowledge::containers::NativeIntegerVector::set_name(
    const std::string& var_name, ThreadSafeContext& knowledge, int size)
{
  if (context_ != &knowledge || name_ != var_name)
  {
    context_ = &knowledge;

    ContextGuard context_guard(*context_);
    MADARA_GUARD_TYPE guard(mutex_);

    name_ = var_name;

    vector_ = knowledge.get_ref(var_name, settings_);

    if (size > 0)
      resize(size_t(size));
  }
}

void madara::knowledge::containers::NativeIntegerVector::exchange(
    NativeIntegerVector& other)
{
  if (context_ && other.context_)
  {
    std::lock(*context_, *other.context_, mutex_, other.mutex_);

    ContextGuard context_guard(*context_, std::adopt_lock);
    ContextGuard other_context_guard(*other.context_, std::adopt_lock);
    MADARA_GUARD_TYPE guard(mutex_, std::adopt_lock),
        guard2(other.mutex_, std::adopt_lock);

    knowledge::KnowledgeRecord temp(context_->get(other.vector_));

    context_->set(other.vector_, context_->get(vector_), other.settings_);
    context_->set(vector_, temp, settings_);
  }
}

void madara::knowledge::containers::NativeIntegerVector::transfer_to(
    NativeIntegerVector& other)
{
  if (context_ && other.context_)
  {
    std::lock(*context_, *other.context_, mutex_, other.mutex_);

    ContextGuard context_guard(*context_, std::adopt_lock);
    ContextGuard other_context_guard(*other.context_, std::adopt_lock);
    MADARA_GUARD_TYPE guard(mutex_, std::adopt_lock),
        guard2(other.mutex_, std::adopt_lock);

    size_t other_size = other.size();
    size_t this_size = this->size();

    if (this_size > 0)
    {
      size_t size = other_size + this_size;
      other.resize((int)size);

      knowledge::KnowledgeRecord rhs(other.context_->get(other.vector_));
      knowledge::KnowledgeRecord lhs(context_->get(vector_));

      rhs.set_index(size - 1, lhs.retrieve_index(this_size - 1).to_double());

      for (size_t i = 0, j = other_size; i < this_size - 1; ++i, ++j)
      {
        rhs.set_index(other_size, lhs.retrieve_index(i).to_double());
      }

      other.context_->set(other.vector_, rhs, other.settings_);

      this->resize(0);
    }
  }
}

void madara::knowledge::containers::NativeIntegerVector::copy_to(
    KnowledgeVector& target) const
{
  if (context_)
  {
    ContextGuard context_guard(*context_);
    MADARA_GUARD_TYPE guard(mutex_);

    target.resize(size());

    for (size_t i = 0; i < target.size(); ++i)
    {
      target[i] = knowledge::KnowledgeRecord((*this)[i]);
    }
  }
}

madara::knowledge::containers::NativeIntegerVector::type
    madara::knowledge::containers::NativeIntegerVector::operator[](
        size_t index) const
{
  knowledge::KnowledgeRecord result;

  if (context_)
  {
    ContextGuard context_guard(*context_);
    MADARA_GUARD_TYPE guard(mutex_);
    result = context_->get(vector_, settings_);

    if (index < result.size())
      result = result.retrieve_index(index);
  }

  return result.to_integer();
}

int madara::knowledge::containers::NativeIntegerVector::set(
    size_t index, type value)
{
  int result = -1;

  if (context_)
  {
    ContextGuard context_guard(*context_);
    MADARA_GUARD_TYPE guard(mutex_);
    result = context_->set_index(vector_, index, value, settings_);
  }

  return result;
}

int madara::knowledge::containers::NativeIntegerVector::set(
    size_t index, type value, const KnowledgeUpdateSettings& settings)
{
  int result = -1;

  if (context_)
  {
    ContextGuard context_guard(*context_);
    MADARA_GUARD_TYPE guard(mutex_);
    result = context_->set_index(vector_, index, value, settings);
  }

  return result;
}

int madara::knowledge::containers::NativeIntegerVector::set(
    const std::vector<type>& value)
{
  int result = -1;

  if (context_)
  {
    ContextGuard context_guard(*context_);
    MADARA_GUARD_TYPE guard(mutex_);
    context_->set(vector_, value, settings_);
  }

  return result;
}

int madara::knowledge::containers::NativeIntegerVector::set(
    const std::vector<type>& value, const KnowledgeUpdateSettings& settings)
{
  int result = -1;

  if (context_)
  {
    ContextGuard context_guard(*context_);
    MADARA_GUARD_TYPE guard(mutex_);
    context_->set(vector_, value, settings);
  }

  return result;
}

void madara::knowledge::containers::NativeIntegerVector::set_quality(
    size_t /*index*/, uint32_t quality,
    const KnowledgeReferenceSettings& settings)
{
  if (context_)
  {
    ContextGuard context_guard(*context_);
    MADARA_GUARD_TYPE guard(mutex_);
    context_->set_quality(name_, quality, true, settings);
  }
}

bool madara::knowledge::containers::NativeIntegerVector::exists(void) const
{
  bool result(false);

  if (context_)
  {
    ContextGuard context_guard(*context_);
    MADARA_GUARD_TYPE guard(mutex_);
    result = context_->exists(vector_);
  }

  return result;
}

std::vector<madara::knowledge::KnowledgeRecord::Integer>
madara::knowledge::containers::NativeIntegerVector::to_integers(void) const
{
  return context_->get(this->vector_, settings_).to_integers();
}

madara::knowledge::KnowledgeRecord
madara::knowledge::containers::NativeIntegerVector::to_record(
    size_t index) const
{
  madara::knowledge::KnowledgeRecord result =
      context_->get(this->vector_, settings_);
  result = result.retrieve_index(index);
  return result;
}

madara::knowledge::KnowledgeRecord
madara::knowledge::containers::NativeIntegerVector::to_record(void) const
{
  return context_->get(this->vector_, settings_);
}

bool madara::knowledge::containers::NativeIntegerVector::is_true(void) const
{
  bool result(false);

  madara_logger_log(context_->get_logger(), logger::LOG_MAJOR,
      "NativeIntegerVector::is_true: checking for non-zero value\n");

  if (context_)
  {
    ContextGuard context_guard(*context_);
    MADARA_GUARD_TYPE guard(mutex_);
    result = context_->get(vector_, settings_).is_true();
  }

  madara_logger_log(context_->get_logger(), logger::LOG_MAJOR,
      "NativeIntegerVector::is_true: final result is %d\n", (int)result);

  return result;
}

bool madara::knowledge::containers::NativeIntegerVector::is_false(void) const
{
  return !is_true();
}

bool madara::knowledge::containers::NativeIntegerVector::is_true_(void) const
{
  return is_true();
}

bool madara::knowledge::containers::NativeIntegerVector::is_false_(void) const
{
  return is_false();
}
