
#ifndef _MADARA_KNOWLEDGE_CONTAINERS_CIRCULARBUFFER_INL_
#define _MADARA_KNOWLEDGE_CONTAINERS_CIRCULARBUFFER_INL_

#include <sstream>
#include <math.h>

#include "CircularBuffer.h"
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
inline CircularBuffer::CircularBuffer(const KnowledgeUpdateSettings& settings)
  : context_(0), settings_(settings)
{
}

inline void CircularBuffer::check_name(const char* func) const
{
  if (name_ == "")
  {
    throw exceptions::NameException(
        std::string("CircularBuffer::") + func + ": name is empty.");
  }
}

inline void CircularBuffer::check_context(const char* func) const
{
  check_name(func);
  if (!context_)
  {
    throw exceptions::ContextException(
        std::string("CircularBuffer::") + func + ": context is not set.");
  }
}

inline void CircularBuffer::check_all(const char* func) const
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
    message << "CircularBuffer::" << func << ": ";
    message << "Invalid access because " << reason << "\n";
    throw exceptions::IndexException(message.str());
  }
}

inline CircularBuffer::CircularBuffer(const std::string& name,
    KnowledgeBase& knowledge, const KnowledgeUpdateSettings& settings)
  : context_(&(knowledge.get_context())), name_(name), settings_(settings)
{
  check_name(__func__);

  ContextGuard context_guard(knowledge);
  set_name(name, knowledge);
}

inline CircularBuffer::CircularBuffer(const std::string& name,
    Variables& knowledge, const KnowledgeUpdateSettings& settings)
  : context_(knowledge.get_context()), name_(name), settings_(settings)
{
  check_name(__func__);

  ContextGuard context_guard(*knowledge.get_context());
  set_name(name, knowledge);
}

inline CircularBuffer::CircularBuffer(const std::string& name,
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

inline CircularBuffer::CircularBuffer(const std::string& name,
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

inline bool CircularBuffer::operator==(const CircularBuffer& value) const
{
  return name_ == value.get_name();
}

inline bool CircularBuffer::operator!=(const CircularBuffer& value) const
{
  return name_ != value.get_name();
}

inline void CircularBuffer::clear(void)
{
  check_all(__func__);

  ContextGuard context_guard(*context_);

  index_ = -1;
  buffer_.resize(0);
}

inline madara::knowledge::KnowledgeRecord::Integer CircularBuffer::increment(
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
    message << "CircularBuffer::increment: ";
    message << "Result of " << base << "+" << value << " is " << result;
    message << " which is impossible to access with size " << size() << ".\n";
    throw exceptions::IndexException(message.str());
  }
}

inline void CircularBuffer::add(const knowledge::KnowledgeRecord& record)
{
  check_all(__func__);

  ContextGuard context_guard(*context_);

  KnowledgeRecord::Integer index = increment(*index_, 1);

  context_->set(buffer_.vector_[(size_t)index], record, settings_);

  context_->mark_modified(size_ref_);

  ++index_;
}

inline void CircularBuffer::add(const std::vector<KnowledgeRecord>& records)
{
  check_all(__func__);

  ContextGuard context_guard(*context_);

  KnowledgeRecord::Integer index = *index_;

  for (auto record : records)
  {
    index = increment(index, 1);

    context_->set(buffer_.vector_[(size_t)index], record, settings_);
  }

  context_->mark_modified(size_ref_);

  index_ += (KnowledgeRecord::Integer)records.size();
}

inline madara::knowledge::KnowledgeRecord CircularBuffer::get(void) const
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

inline madara::knowledge::KnowledgeRecord CircularBuffer::inspect(
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
    message << "CircularBuffer::inspect: ";
    message << "Invalid access for " << position << " element when count is ";
    message << inserted << "\n";
    throw exceptions::IndexException(message.str());
  }
}

inline std::vector<KnowledgeRecord> CircularBuffer::inspect(
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
    message << "CircularBuffer::inspect: ";
    message << "Invalid access for " << position << " element when count is ";
    message << inserted << "\n";
    throw exceptions::IndexException(message.str());
  }
}

inline std::string CircularBuffer::get_name(void) const
{
  return name_;
}

inline size_t CircularBuffer::size(void) const
{
  return buffer_.size();
}

inline size_t CircularBuffer::count(void) const
{
  check_context(__func__);

  ContextGuard context_guard(*context_);

  return std::min((size_t)(*index_ + 1), buffer_.size());
}

inline void CircularBuffer::resize(int size)
{
  check_context(__func__);

  ContextGuard context_guard(*context_);

  buffer_.resize(size, false);

  context_->mark_modified(size_ref_);
}

inline madara::knowledge::KnowledgeUpdateSettings CircularBuffer::set_settings(
    const KnowledgeUpdateSettings& settings)
{
  KnowledgeUpdateSettings old_settings = settings_;

  settings_ = settings;

  return old_settings;
}

inline void CircularBuffer::set_quality(
    uint32_t quality, const KnowledgeReferenceSettings& settings)
{
  check_context(__func__);

  ContextGuard context_guard(*context_);
  context_->set_quality(name_, quality, true, settings);
}

inline void CircularBuffer::set_name(
    const std::string& name, KnowledgeBase& knowledge)
{
  if (name != "")
  {
    ContextGuard context_guard(knowledge);
    name_ = name;
    context_ = &(knowledge.get_context());
    index_.set_name(name + ".index", knowledge);
    buffer_.set_name(name, knowledge);
    size_ref_ = buffer_.get_size_ref();
    if (buffer_.size() == 0 && index_ != -1)
    {
      index_ = -1;
    }
  }
  else
  {
    throw exceptions::NameException(
        "CircularBuffer::set_name: empty name provided.");
  }
}

inline void CircularBuffer::set_name(
    const std::string& name, Variables& knowledge)
{
  if (name != "")
  {
    ContextGuard context_guard(*knowledge.get_context());
    name_ = name;
    context_ = knowledge.get_context();
    index_.set_name(name + ".index", knowledge);
    buffer_.set_name(name, knowledge);
    size_ref_ = buffer_.get_size_ref();
    if (buffer_.size() == 0 && index_ != -1)
    {
      index_ = -1;
    }
  }
  else
  {
    throw exceptions::NameException(
        "CircularBuffer::set_name: empty name provided.");
  }
}

inline void CircularBuffer::set_index(KnowledgeRecord::Integer index)
{
  check_context(__func__);
  index_ = index;
}

inline std::vector<KnowledgeRecord> CircularBuffer::get_latest(
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

inline std::vector<KnowledgeRecord> CircularBuffer::get_earliest(
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

}  // end containers namespace
}  // end knowledge namespace
}  // end madara namespace

#endif  //  _MADARA_KNOWLEDGE_CONTAINERS_CIRCULARBUFFER_INL_
