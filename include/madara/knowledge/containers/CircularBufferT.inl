
#ifndef _MADARA_KNOWLEDGE_CONTAINERS_CIRCULARBUFFERT_INL_
#define _MADARA_KNOWLEDGE_CONTAINERS_CIRCULARBUFFERT_INL_

#include <sstream>
#include <math.h>

#include "CircularBufferT.h"
#include "madara/knowledge/ContextGuard.h"
#include "madara/exceptions/IndexException.h"
#include "madara/exceptions/ContextException.h"
#include "madara/exceptions/NameException.h"

namespace madara
{
namespace knowledge
{
namespace containers
{
template<typename T>
inline CircularBufferT<T>::CircularBufferT(
    const KnowledgeUpdateSettings& settings)
  : context_(0), settings_(settings)
{
}

template<typename T>
inline void CircularBufferT<T>::check_name(const char* func) const
{
  if (name_ == "")
  {
    throw exceptions::NameException(
        std::string("CircularBufferT<T>::") + func + ": name is empty.");
  }
}

template<typename T>
inline void CircularBufferT<T>::check_context(const char* func) const
{
  check_name(func);
  if (!context_)
  {
    throw exceptions::ContextException(
        std::string("CircularBufferT<T>::") + func + ": context is not set.");
  }
}

template<typename T>
inline void CircularBufferT<T>::check_all(const char* func) const
{
  std::string reason = "";
  if (context_ == 0)
  {
    reason = "context has not been set";
  }

  if (name_ == "")
  {
    if (reason.size() > 0)
    {
      reason += " and ";
    }
    reason = "name has not been set";
  }

  if (buffer_.size() == 0)
  {
    if (reason.size() > 0)
    {
      reason += " and ";
    }
    reason = "size == 0";
  }

  if (reason != "")
  {
    std::stringstream message;
    message << "CircularBufferT<T>::" << func << ": ";
    message << "Invalid access because " << reason << "\n";
    throw exceptions::IndexException(message.str());
  }
}

template<typename T>
inline CircularBufferT<T>::CircularBufferT(const std::string& name,
    KnowledgeBase& knowledge, const KnowledgeUpdateSettings& settings)
  : context_(&(knowledge.get_context())), name_(name), settings_(settings)
{
  check_name(__func__);

  ContextGuard context_guard(knowledge);
  set_name(name, knowledge);
}

template<typename T>
inline CircularBufferT<T>::CircularBufferT(const std::string& name,
    Variables& knowledge, const KnowledgeUpdateSettings& settings)
  : context_(knowledge.get_context()), name_(name), settings_(settings)
{
  check_name(__func__);

  ContextGuard context_guard(*knowledge.get_context());
  set_name(name, knowledge);
}

template<typename T>
inline CircularBufferT<T>::CircularBufferT(const std::string& name,
    KnowledgeBase& knowledge, int size, const KnowledgeUpdateSettings& settings)
  : context_(&(knowledge.get_context())), name_(name), settings_(settings)
{
  check_name(__func__);

  ContextGuard context_guard(knowledge);
  set_name(name, knowledge);
  if (size >= 0)
  {
    buffer_.resize(size);
  }
}

template<typename T>
inline CircularBufferT<T>::CircularBufferT(const std::string& name,
    Variables& knowledge, int size, const KnowledgeUpdateSettings& settings)
  : context_(knowledge.get_context()), name_(name), settings_(settings)
{
  check_name(__func__);

  ContextGuard context_guard(*knowledge.get_context());
  set_name(name, knowledge);
  if (size >= 0)
  {
    buffer_.resize(size);
  }
}

template<typename T>
inline bool CircularBufferT<T>::operator==(const CircularBufferT& value) const
{
  return name_ == value.get_name();
}

template<typename T>
inline bool CircularBufferT<T>::operator!=(const CircularBufferT& value) const
{
  return name_ != value.get_name();
}

template<typename T>
inline void CircularBufferT<T>::clear(void)
{
  check_all(__func__);

  ContextGuard context_guard(*context_);

  index_ = -1;
  buffer_.resize(0);
}

template<typename T>
inline madara::knowledge::KnowledgeRecord::Integer
CircularBufferT<T>::increment(
    KnowledgeRecord::Integer base, KnowledgeRecord::Integer value) const
{
  KnowledgeRecord::Integer result = base + value;
  if (buffer_.size() > 0 && base + value >= 0)
  {
    return (result) % (KnowledgeRecord::Integer)buffer_.size();
  }
  else if (buffer_.size() > 0)
  {
    return (KnowledgeRecord::Integer)buffer_.size() + result;
  }
  else
  {
    std::stringstream message;
    message << "CircularBufferT<T>::increment: ";
    message << "Result of " << base << "+" << value << " is " << result;
    message << " which is impossible to access with size " << size() << ".\n";
    throw exceptions::IndexException(message.str());
  }
}

template<typename T>
inline void CircularBufferT<T>::add(const T& value)
{
  Any any_record(value);
  add(KnowledgeRecord(any_record));
}

template<typename T>
inline void CircularBufferT<T>::add(const knowledge::KnowledgeRecord& record)
{
  check_all(__func__);

  ContextGuard context_guard(*context_);

  KnowledgeRecord::Integer index = increment(*index_, 1);

  context_->set(buffer_.vector_[(size_t)index], record, settings_);

  ++index_;
}

template<typename T>
inline void CircularBufferT<T>::add(const std::vector<T>& records)
{
  for (auto record : records)
  {
    add(record);
  }
}

template<typename T>
inline void CircularBufferT<T>::add(const std::vector<KnowledgeRecord>& records)
{
  check_all(__func__);

  ContextGuard context_guard(*context_);

  KnowledgeRecord::Integer index = *index_;

  for (auto record : records)
  {
    index = increment(index, 1);

    context_->set(buffer_.vector_[(size_t)index], record, settings_);
  }

  index_ += (KnowledgeRecord::Integer)records.size();
}

template<typename T>
inline madara::knowledge::KnowledgeRecord CircularBufferT<T>::get(void) const
{
  check_all(__func__);

  ContextGuard context_guard(*context_);

  KnowledgeRecord::Integer index = *index_;
  index = increment(index, 0);

  if (count() > 0)
    return context_->get(buffer_.vector_[(size_t)index], settings_);
  else
    return KnowledgeRecord();
}

template<typename T>
inline void CircularBufferT<T>::get(T& value) const
{
  value = this->get().template to_any<T>();
}

template<typename T>
inline madara::knowledge::KnowledgeRecord CircularBufferT<T>::inspect(
    KnowledgeRecord::Integer position) const
{
  check_context(__func__);
  ContextGuard context_guard(*context_);

  KnowledgeRecord::Integer inserted = (KnowledgeRecord::Integer)count();

  if ((position <= 0 && -position < inserted) ||
      (position > 0 && inserted == (KnowledgeRecord::Integer)size() &&
          position < inserted))
  {
    size_t index =
        (size_t)increment(*index_, (KnowledgeRecord::Integer)position);

    return context_->get(buffer_.vector_[index], settings_);
  }
  else
  {
    std::stringstream message;
    message << "CircularBufferT<T>::inspect: ";
    message << "Invalid access for " << position << " element when count is ";
    message << inserted << "\n";
    throw exceptions::IndexException(message.str());
  }
}

template<typename T>
inline void CircularBufferT<T>::inspect(
    KnowledgeRecord::Integer position, T& value) const
{
  value = inspect(position).template to_any<T>();
}

template<typename T>
inline std::vector<KnowledgeRecord> CircularBufferT<T>::inspect(
    KnowledgeRecord::Integer position, size_t count) const
{
  check_context(__func__);

  ContextGuard context_guard(*context_);

  KnowledgeRecord::Integer inserted = (KnowledgeRecord::Integer)this->count();

  if ((position <= 0 && -position < inserted) ||
      (position > 0 && inserted == (KnowledgeRecord::Integer)size() &&
          position < inserted))
  {
    KnowledgeRecord::Integer index =
        increment(*index_, (KnowledgeRecord::Integer)position);

    std::vector<KnowledgeRecord> result;

    for (size_t i = 0; i < count; ++i, index = increment(index, 1))
    {
      result.push_back(buffer_[(size_t)index]);
    }

    return result;
  }
  else
  {
    std::stringstream message;
    message << "CircularBufferT<T>::inspect: ";
    message << "Invalid access for " << position << " element when count is ";
    message << inserted << "\n";
    throw exceptions::IndexException(message.str());
  }
}

template<typename T>
inline void CircularBufferT<T>::inspect(KnowledgeRecord::Integer position,
    size_t count, std::vector<T>& values) const
{
  // iterate over the returned records
  for (auto record : inspect(position, count))
  {
    // add them to the values
    values.push_back(record.template to_any<T>());
  }
}

template<typename T>
inline std::string CircularBufferT<T>::get_name(void) const
{
  return name_;
}

template<typename T>
inline size_t CircularBufferT<T>::size(void) const
{
  return buffer_.size();
}

template<typename T>
inline size_t CircularBufferT<T>::count(void) const
{
  check_context(__func__);

  ContextGuard context_guard(*context_);

  return std::min((size_t)(*index_ + 1), buffer_.size());
}

template<typename T>
inline void CircularBufferT<T>::resize(int size)
{
  check_context(__func__);

  ContextGuard context_guard(*context_);

  buffer_.resize(size, false);
}

template<typename T>
inline madara::knowledge::KnowledgeUpdateSettings
CircularBufferT<T>::set_settings(const KnowledgeUpdateSettings& settings)
{
  KnowledgeUpdateSettings old_settings = settings_;

  settings_ = settings;

  return old_settings;
}

template<typename T>
inline void CircularBufferT<T>::set_quality(
    uint32_t quality, const KnowledgeReferenceSettings& settings)
{
  check_context(__func__);

  ContextGuard context_guard(*context_);
  context_->set_quality(name_, quality, true, settings);
}

template<typename T>
inline void CircularBufferT<T>::set_name(
    const std::string& name, KnowledgeBase& knowledge)
{
  if (name != "")
  {
    ContextGuard context_guard(knowledge);
    name_ = name;
    context_ = &(knowledge.get_context());
    index_.set_name(name + ".index", knowledge);
    buffer_.set_name(name, knowledge);
    if (buffer_.size() == 0 && index_ != -1)
    {
      index_ = -1;
    }
  }
  else
  {
    throw exceptions::NameException(
        "CircularBufferT<T>::set_name: empty name provided.");
  }
}

template<typename T>
inline void CircularBufferT<T>::set_name(
    const std::string& name, Variables& knowledge)
{
  if (name != "")
  {
    ContextGuard context_guard(*knowledge.get_context());
    name_ = name;
    context_ = knowledge.get_context();
    index_.set_name(name + ".index", knowledge);
    buffer_.set_name(name, knowledge);
    if (buffer_.size() == 0 && index_ != -1)
    {
      index_ = -1;
    }
  }
  else
  {
    throw exceptions::NameException(
        "CircularBufferT<T>::set_name: empty name provided.");
  }
}

template<typename T>
inline void CircularBufferT<T>::set_index(KnowledgeRecord::Integer index)
{
  check_context(__func__);
  index_ = index;
}

template<typename T>
inline std::vector<KnowledgeRecord> CircularBufferT<T>::get_latest(
    size_t count) const
{
  check_all(__func__);

  ContextGuard context_guard(*context_);

  count = std::min(count, this->count());

  std::vector<KnowledgeRecord> result;

  KnowledgeRecord::Integer cur = *index_ % buffer_.size();

  for (size_t i = 0; i < count; ++i, cur = increment(cur, -1))
  {
    result.push_back(buffer_[(size_t)cur]);
  }

  return result;
}

template<typename T>
inline void CircularBufferT<T>::get_latest(
    size_t count, std::vector<T>& values) const
{
  // iterate over the returned records
  for (auto record : get_latest(count))
  {
    // add them to the values
    values.push_back(record.template to_any<T>());
  }
}

template<typename T>
inline std::vector<KnowledgeRecord> CircularBufferT<T>::get_earliest(
    size_t count) const
{
  check_all(__func__);

  ContextGuard context_guard(*context_);
  std::vector<KnowledgeRecord> result;

  count = std::min(count, this->count());

  // start is either 0 or index_ + 1
  KnowledgeRecord::Integer cur =
      this->count() < buffer_.size() ? 0 : increment(*index_, 1);

  for (size_t i = 0; i < count; ++i, cur = increment(cur, 1))
  {
    result.push_back(buffer_[(size_t)cur]);
  }

  return result;
}

template<typename T>
inline void CircularBufferT<T>::get_earliest(
    size_t count, std::vector<T>& values) const
{
  // iterate over the returned records
  for (auto record : get_earliest(count))
  {
    // add them to the values
    values.push_back(record.template to_any<T>());
  }
}

}  // end containers namespace
}  // end knowledge namespace
}  // end madara namespace

#endif  //  _MADARA_KNOWLEDGE_CONTAINERS_CIRCULARBUFFERT_INL_
