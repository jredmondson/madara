
#ifndef _MADARA_KNOWLEDGE_CONTAINERS_CIRCULARBUFFERCONSUMERT_INL_
#define _MADARA_KNOWLEDGE_CONTAINERS_CIRCULARBUFFERCONSUMERT_INL_

#include <sstream>
#include <math.h>

#include "CircularBufferConsumerT.h"
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
inline CircularBufferConsumerT<T>::CircularBufferConsumerT()
  : context_(0), local_index_(-1)
{
}

template<typename T>
inline void CircularBufferConsumerT<T>::check_name(const char* func) const
{
  if (name_ == "")
  {
    throw exceptions::NameException(
        std::string("CircularBufferConsumerT<T>::") + func +
        ": name is empty.");
  }
}

template<typename T>
inline void CircularBufferConsumerT<T>::check_context(const char* func) const
{
  check_name(func);
  if (!context_)
  {
    throw exceptions::ContextException(
        std::string("CircularBufferConsumerT<T>::") + func +
        ": context is not set.");
  }
}

template<typename T>
inline void CircularBufferConsumerT<T>::check_all(const char* func) const
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
    message << "CircularBufferConsumerT<T>::" << func << ": ";
    message << "Invalid access because " << reason << "\n";
    throw exceptions::IndexException(message.str());
  }
}

template<typename T>
inline CircularBufferConsumerT<T>::CircularBufferConsumerT(
    const std::string& name, KnowledgeBase& knowledge)
  : context_(&(knowledge.get_context())), name_(name), local_index_(-1)
{
  check_name(__func__);

  ContextGuard context_guard(knowledge);
  set_name(name, knowledge);
}

template<typename T>
inline CircularBufferConsumerT<T>::CircularBufferConsumerT(
    const std::string& name, Variables& knowledge)
  : context_(knowledge.get_context()), name_(name), local_index_(-1)
{
  check_name(__func__);

  ContextGuard context_guard(*knowledge.get_context());
  set_name(name, knowledge);
}

template<typename T>
inline bool CircularBufferConsumerT<T>::operator==(
    const CircularBufferConsumerT& value) const
{
  return name_ == value.get_name();
}

template<typename T>
inline bool CircularBufferConsumerT<T>::operator!=(
    const CircularBufferConsumerT& value) const
{
  return name_ != value.get_name();
}

template<typename T>
inline madara::knowledge::KnowledgeRecord::Integer
CircularBufferConsumerT<T>::increment(
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
    message << "CircularBufferConsumerT<T>::increment: ";
    message << "Result of " << base << "+" << value << " is " << result;
    message << " which is impossible to access with size " << size() << ".\n";
    throw exceptions::IndexException(message.str());
  }
}

template<typename T>
void CircularBufferConsumerT<T>::consume(T& value) const
{
  check_all(__func__);

  ContextGuard context_guard(*context_);

  KnowledgeRecord::Integer index_diff = *index_ - local_index_;

  if (index_diff > (KnowledgeRecord::Integer)buffer_.size())
  {
    local_index_ = *index_ - (KnowledgeRecord::Integer)buffer_.size();
  }

  KnowledgeRecord::Integer cur = increment(local_index_, 1);

  if (remaining() > 0)
  {
    ++local_index_;
    value = context_->get(buffer_.vector_[(size_t)(cur)]).template to_any<T>();
  }
  else
    throw exceptions::IndexException(
        "CircularBufferConsumerT::consume: no remaining values in buffer");
}

template<typename T>
void CircularBufferConsumerT<T>::consume(T& value, size_t& dropped) const
{
  check_all(__func__);

  ContextGuard context_guard(*context_);

  KnowledgeRecord::Integer index_diff = *index_ - local_index_;

  dropped = get_dropped();

  if (index_diff > (KnowledgeRecord::Integer)buffer_.size())
  {
    local_index_ = *index_ - (KnowledgeRecord::Integer)buffer_.size();
  }

  KnowledgeRecord::Integer cur = increment(local_index_, 1);

  if (remaining() > 0)
  {
    ++local_index_;
    value = context_->get(buffer_.vector_[(size_t)(cur)]).template to_any<T>();
  }
  else
    throw exceptions::IndexException(
        "CircularBufferConsumerT::consume: no remaining values in buffer");
}

template<typename T>
inline size_t CircularBufferConsumerT<T>::get_dropped(void) const
{
  check_all(__func__);

  ContextGuard context_guard(*context_);

  size_t difference = remaining();
  size_t buffer_size = size();

  if (difference > buffer_size)
  {
    return difference - buffer_size;
  }
  else
  {
    return 0;
  }
}

template<typename T>
inline void CircularBufferConsumerT<T>::inspect(
    KnowledgeRecord::Integer position, T& value) const
{
  check_context(__func__);

  ContextGuard context_guard(*context_);

  // If buffer overflowed, update local index to last valid value - 1
  KnowledgeRecord::Integer index_diff = (*index_ - local_index_);
  if (index_diff > (KnowledgeRecord::Integer)buffer_.size())
  {
    local_index_ = *index_ - (KnowledgeRecord::Integer)buffer_.size();
  }

  KnowledgeRecord::Integer requested_index = local_index_ + position;

  if (0 <= requested_index &&
      (*index_ - (KnowledgeRecord::Integer)buffer_.size()) <= requested_index &&
      requested_index <= *index_)
  {
    size_t index =
        (size_t)increment(local_index_, (KnowledgeRecord::Integer)position);

    value = context_->get(buffer_.vector_[index]).template to_any<T>();
  }
  else
  {
    std::stringstream message;
    message << "CircularBufferConsumer::inspect: ";
    message << "Invalid access for relative position " << position
            << " when buffer index is ";
    message << *index_ << " and local index is : " << local_index_
            << " and size is : " << size() << "\n";
    throw exceptions::IndexException(message.str());
  }
}

template<typename T>
inline void CircularBufferConsumerT<T>::inspect(
    KnowledgeRecord::Integer position, size_t count,
    std::vector<T>& values) const
{
  check_context(__func__);

  ContextGuard context_guard(*context_);

  // If buffer overflowed, update local index to last valid value - 1
  KnowledgeRecord::Integer index_diff = (*index_ - local_index_);
  if (index_diff > (KnowledgeRecord::Integer)buffer_.size())
  {
    local_index_ = *index_ - (KnowledgeRecord::Integer)buffer_.size();
  }

  KnowledgeRecord::Integer requested_index = local_index_ + position;

  if (0 <= requested_index &&
      (*index_ - (KnowledgeRecord::Integer)buffer_.size()) <= requested_index &&
      requested_index <= *index_)
  {
    KnowledgeRecord::Integer index =
        increment(local_index_, (KnowledgeRecord::Integer)position);

    for (size_t i = 0; i < count; ++i, index = increment(index, 1))
    {
      values.push_back(buffer_[(size_t)index].template to_any<T>());
    }
  }
  else
  {
    std::stringstream message;
    message << "CircularBufferConsumer::inspect: ";
    message << "Invalid access for relative position " << position
            << " when buffer index is ";
    message << *index_ << " and local index is : " << local_index_
            << " and size is : " << size() << "\n";
    throw exceptions::IndexException(message.str());
  }
}

template<typename T>
inline std::string CircularBufferConsumerT<T>::get_name(void) const
{
  return name_;
}

template<typename T>
inline size_t CircularBufferConsumerT<T>::size(void) const
{
  return buffer_.size();
}

template<typename T>
inline size_t CircularBufferConsumerT<T>::remaining(void) const
{
  check_context(__func__);

  ContextGuard context_guard(*context_);

  return *index_ - local_index_;
}

template<typename T>
inline size_t CircularBufferConsumerT<T>::count(void) const
{
  check_context(__func__);

  ContextGuard context_guard(*context_);

  return std::min((size_t)(*index_ + 1), buffer_.size());
}

template<typename T>
inline void CircularBufferConsumerT<T>::resize(void)
{
  check_context(__func__);

  ContextGuard context_guard(*context_);

  buffer_.resize(-1, false);
}

template<typename T>
inline void CircularBufferConsumerT<T>::resync(void)
{
  if (context_ && name_ != "")
  {
    ContextGuard context_guard(*context_);

    local_index_ = *index_;
  }
  else
  {
    throw exceptions::ContextException(
        "CircularBufferConsumerT<T>::resync: "
        " context is null or name hasn't been set.");
  }
}

template<typename T>
inline void CircularBufferConsumerT<T>::set_name(
    const std::string& name, KnowledgeBase& knowledge)
{
  if (name != "")
  {
    ContextGuard context_guard(knowledge);
    name_ = name;
    context_ = &(knowledge.get_context());
    index_.set_name(name + ".index", knowledge);
    buffer_.set_name(name, knowledge);
    local_index_ = -1;
  }
  else
  {
    throw exceptions::NameException(
        "CircularBufferConsumerT<T>::set_name: empty name provided.");
  }
}

template<typename T>
inline void CircularBufferConsumerT<T>::set_name(
    const std::string& name, Variables& knowledge)
{
  if (name != "")
  {
    ContextGuard context_guard(*knowledge.get_context());
    name_ = name;
    context_ = knowledge.get_context();
    index_.set_name(name + ".index", knowledge);
    buffer_.set_name(name, knowledge);
    local_index_ = -1;
  }
  else
  {
    throw exceptions::NameException(
        "CircularBufferConsumerT<T>::set_name: empty name provided.");
  }
}

template<typename T>
inline void CircularBufferConsumerT<T>::set_index(
    KnowledgeRecord::Integer index)
{
  check_context(__func__);

  local_index_ = index;
}

template<typename T>
void CircularBufferConsumerT<T>::consume_latest(
    size_t count, std::vector<T>& values) const
{
  check_all(__func__);

  ContextGuard context_guard(*context_);

  KnowledgeRecord::Integer cur = *index_ % buffer_.size();

  count = std::min(count, (size_t)(*index_ - local_index_));

  for (size_t i = 0; i < count; ++i, cur = increment(cur, -1))
  {
    values.push_back(buffer_[(size_t)cur].template to_any<T>());
  }

  // note the difference here is that reading the latest will change index
  local_index_ = *index_;
}

template<typename T>
void CircularBufferConsumerT<T>::consume_latest(
    size_t count, std::vector<T>& values, size_t& dropped) const
{
  check_all(__func__);

  ContextGuard context_guard(*context_);

  dropped = get_dropped();

  KnowledgeRecord::Integer cur = *index_ % buffer_.size();

  count = std::min(count, (size_t)(*index_ - local_index_));

  for (size_t i = 0; i < count; ++i, cur = increment(cur, -1))
  {
    values.push_back(buffer_[(size_t)cur].template to_any<T>());
  }

  // note the difference here is that reading the latest will change index
  local_index_ = *index_;
}

template<typename T>
void CircularBufferConsumerT<T>::consume_earliest(
    size_t count, std::vector<T>& values) const
{
  check_all(__func__);

  ContextGuard context_guard(*context_);

  KnowledgeRecord::Integer index_diff = (*index_ - local_index_);

  // If buffer overflowed, update local index to last valid value - 1
  if (index_diff > (KnowledgeRecord::Integer)buffer_.size())
  {
    local_index_ = *index_ - (KnowledgeRecord::Integer)buffer_.size();
    index_diff = (KnowledgeRecord::Integer)buffer_.size();
  }

  count = std::min(count, (size_t)index_diff);

  // start is either 0 or index_ + 1
  KnowledgeRecord::Integer cur =
      index_diff < (KnowledgeRecord::Integer)buffer_.size()
          ? increment(local_index_, 1)
          : increment(*index_, -(KnowledgeRecord::Integer)(buffer_.size()) + 1);

  for (size_t i = 0; i < count; ++i, cur = increment(cur, 1))
  {
    values.push_back(buffer_[(size_t)cur].template to_any<T>());
  }

  local_index_ += (KnowledgeRecord::Integer)values.size();
}

template<typename T>
void CircularBufferConsumerT<T>::consume_earliest(
    size_t count, std::vector<T>& values, size_t& dropped) const
{
  check_all(__func__);

  ContextGuard context_guard(*context_);

  KnowledgeRecord::Integer index_diff = (*index_ - local_index_);

  dropped = get_dropped();

  count = std::min(count, (size_t)index_diff);

  // start is either 0 or index_ + 1
  KnowledgeRecord::Integer cur =
      index_diff < (KnowledgeRecord::Integer)buffer_.size()
          ? increment(local_index_, 1)
          : increment(*index_, -(KnowledgeRecord::Integer)(buffer_.size()) + 1);

  for (size_t i = 0; i < count; ++i, cur = increment(cur, 1))
  {
    values.push_back(buffer_[(size_t)cur].template to_any<T>());
  }

  local_index_ += (KnowledgeRecord::Integer)values.size();
}

template<typename T>
void CircularBufferConsumerT<T>::peek_latest(
    size_t count, std::vector<T>& values) const
{
  check_name(__func__);

  ContextGuard context_guard(*context_);

  count = std::min(count, this->count());

  KnowledgeRecord::Integer cur = *index_ % buffer_.size();

  for (size_t i = 0; i < count; ++i, cur = increment(cur, -1))
  {
    values.push_back(buffer_[(size_t)cur].template to_any<T>());
  }
}

template<typename T>
void CircularBufferConsumerT<T>::peek_latest(T& value) const
{
  check_all(__func__);

  ContextGuard context_guard(*context_);

  KnowledgeRecord::Integer index = *index_;
  index = increment(index, 0);

  if (count() > 0)
  {
    value = context_->get(buffer_.vector_[(size_t)index]).template to_any<T>();
  }
  else
    throw exceptions::IndexException(
        "CircularBufferConsumerT::peek_latest: no values in buffer");
}

}  // end containers namespace
}  // end knowledge namespace
}  // end madara namespace

#endif  //  _MADARA_KNOWLEDGE_CONTAINERS_CIRCULARBUFFERCONSUMERT_INL_
