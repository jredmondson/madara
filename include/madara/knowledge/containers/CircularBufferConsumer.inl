
#ifndef _MADARA_KNOWLEDGE_CONTAINERS_CIRCULARBUFFERCONSUMER_INL_
#define _MADARA_KNOWLEDGE_CONTAINERS_CIRCULARBUFFERCONSUMER_INL_

#include <sstream>
#include <math.h>

#include "CircularBufferConsumer.h"
#include "madara/knowledge/ContextGuard.h"
#include "madara/exceptions/IndexException.h"
#include "madara/exceptions/ContextException.h"
#include "madara/exceptions/NameException.h"

namespace madara { namespace knowledge { namespace containers {

inline CircularBufferConsumer::CircularBufferConsumer ()
: context_ (0), local_index_ (-1)
{
}

inline void CircularBufferConsumer::check_name (const char *func) const
{
  if (name_ == "") {
    throw exceptions::NameException (
      std::string("CircularBufferConsumer::") + func + ": name is empty.");
  }
}

inline void CircularBufferConsumer::check_context (const char *func) const
{
  check_name (func);
  if (!context_)
  {
    throw exceptions::ContextException (
      std::string("CircularBufferConsumer::") + func + ": context is not set.");
  }
}

inline void CircularBufferConsumer::check_all (const char *func) const
{
  std::string reason = "";
  if (context_ == 0)
  {
    reason = "context has not been set";
  }

  if (name_ == "")
  {
    if (reason.size () > 0)
    {
      reason += " and ";
    }
    reason = "name has not been set";
  }

  if (buffer_.size () == 0)
  {
    if (reason.size () > 0)
    {
      reason += " and ";
    }
    reason = "size == 0";
  }

  if (reason != "") {
    std::stringstream message;
    message << "CircularBufferConsumer::" << func << ": ";
    message << "Invalid access because " << reason << "\n";
    throw exceptions::IndexException (message.str ()); 
  }
}

inline CircularBufferConsumer::CircularBufferConsumer (
  const std::string & name,
  KnowledgeBase & knowledge)
: context_ (&(knowledge.get_context ())), name_ (name),
  local_index_ (-1)
{
  check_name (__func__);

  ContextGuard context_guard (knowledge);
  set_name (name, knowledge);
}
 
inline CircularBufferConsumer::CircularBufferConsumer (
  const std::string & name,
  Variables & knowledge)
: context_ (knowledge.get_context ()), name_ (name),
  local_index_ (-1)
{
  check_name (__func__);

  ContextGuard context_guard (*knowledge.get_context ());
  set_name (name, knowledge);
}

inline bool
CircularBufferConsumer::operator== (
  const CircularBufferConsumer & value) const
{
  return name_ == value.get_name ();
}

inline bool
CircularBufferConsumer::operator!= (
  const CircularBufferConsumer & value) const
{
  return name_ != value.get_name ();
}

inline madara::knowledge::KnowledgeRecord::Integer
CircularBufferConsumer::increment (
  KnowledgeRecord::Integer base, KnowledgeRecord::Integer value) const
{
  KnowledgeRecord::Integer result = base + value;
  if (buffer_.size () > 0 && base + value >= 0)
  {
    return (result) % (KnowledgeRecord::Integer)buffer_.size ();
  }
  else if (buffer_.size () > 0)
  {
    return (KnowledgeRecord::Integer)buffer_.size () + result;
  }
  else
  {
    std::stringstream message;
    message << "CircularBufferConsumer::increment: ";
    message << "Result of " << base << "+" << value << " is " << result;
    message << " which is impossible to access with size " << size () << ".\n";
    throw exceptions::IndexException (message.str ()); 
  }
}

template <typename T> void
CircularBufferConsumer::consume (T & value) const
{
  check_context (__func__);

  ContextGuard context_guard (*context_);

  if (remaining () > 0)
    value = consume ().to_any <T> ();
  else
    throw exceptions::IndexException ("CircularBufferConsumer::consume<T>: "
      "attempted consume on empty consumer buffer");
}

inline madara::knowledge::KnowledgeRecord
CircularBufferConsumer::consume (void) const
{
  check_all (__func__);

  ContextGuard context_guard (*context_);

  KnowledgeRecord::Integer index_diff = *index_ - local_index_;

  if (index_diff > (KnowledgeRecord::Integer)buffer_.size ())
  {
    local_index_ = *index_ - (KnowledgeRecord::Integer)buffer_.size ();
  }

  KnowledgeRecord::Integer cur = increment (local_index_, 1);

  if (remaining () > 0)
  {
    ++local_index_;
    return context_->get (buffer_.vector_[(size_t)(cur)]);
  }
  else return KnowledgeRecord ();
}

inline madara::knowledge::KnowledgeRecord
CircularBufferConsumer::consume (size_t & dropped) const
{
  check_all (__func__);

  ContextGuard context_guard (*context_);

  KnowledgeRecord::Integer index_diff = *index_ - local_index_;

  dropped = get_dropped ();

  if (index_diff > (KnowledgeRecord::Integer)buffer_.size ())
  {
    local_index_ = *index_ - (KnowledgeRecord::Integer)buffer_.size ();
  }

  KnowledgeRecord::Integer cur = increment (local_index_, 1);

  if (remaining () > 0)
  {
    ++local_index_;
    return context_->get (buffer_.vector_[(size_t)(cur)]);
  }
  else return KnowledgeRecord ();
}

template <typename T> void
CircularBufferConsumer::inspect (
  KnowledgeRecord::Integer position, T & value) const
{
  value = inspect (position).to_any <T> ();
}

inline madara::knowledge::KnowledgeRecord
CircularBufferConsumer::inspect (
  KnowledgeRecord::Integer position) const
{
  check_context (__func__);

  ContextGuard context_guard (*context_);

  // If buffer overflowed, update local index to last valid value - 1
  KnowledgeRecord::Integer index_diff = (*index_ - local_index_);
  if (index_diff > (KnowledgeRecord::Integer)buffer_.size ())
  {
    local_index_ = *index_ - (KnowledgeRecord::Integer)buffer_.size ();
  }

  KnowledgeRecord::Integer requested_index = local_index_ + position;

  if (0 <= requested_index &&
      (*index_ - (KnowledgeRecord::Integer)buffer_.size ()) <= requested_index &&
      requested_index <= *index_)
  {
    size_t index = (size_t)increment (
      local_index_, (KnowledgeRecord::Integer)position);

    return context_->get (buffer_.vector_[index]);
  }
  else
  {
    std::stringstream message;
    message << "CircularBufferConsumer::inspect: ";
    message << "Invalid access for " << position << " element when buffer index is ";
    message << *index_ << "\n";
    throw exceptions::IndexException (message.str ()); 
  }
}

template <typename T> void
CircularBufferConsumer::inspect (KnowledgeRecord::Integer position,
  size_t count, std::vector <T> & values) const
{
  // iterate over the returned records
  for (auto record : inspect (position, count))
  {
    // add them to the values
    values.push_back (record.to_any <T> ());
  }
}

inline std::vector <KnowledgeRecord>
CircularBufferConsumer::inspect (
  KnowledgeRecord::Integer position,
  size_t count) const
{
  check_context (__func__);

  ContextGuard context_guard (*context_);

  // If buffer overflowed, update local index to last valid value - 1
  KnowledgeRecord::Integer index_diff = (*index_ - local_index_);
  if (index_diff > (KnowledgeRecord::Integer)buffer_.size ())
  {
    local_index_ = *index_ - (KnowledgeRecord::Integer)buffer_.size ();
  }

  KnowledgeRecord::Integer requested_index = local_index_ + position;

  if (0 <= requested_index &&
      (*index_ - (KnowledgeRecord::Integer)buffer_.size ()) <= requested_index &&
      requested_index <= *index_)
  {
    KnowledgeRecord::Integer index = increment (
      local_index_, (KnowledgeRecord::Integer)position);

    std::vector <KnowledgeRecord> result;

    for (size_t i = 0; i < count; ++i, index = increment (index, 1))
    {
      result.push_back (buffer_[(size_t)index]);
    }

    return result;
  }
  else
  {
    std::stringstream message;
    message << "CircularBufferConsumer::inspect: ";
    message << "Invalid access for " << position << " element when buffer index is ";
    message << *index_ << "\n";
    throw exceptions::IndexException (message.str ()); 
  }
}

inline std::string
CircularBufferConsumer::get_name (void) const
{
  return name_;
}

inline size_t
CircularBufferConsumer::size (void) const
{
  return buffer_.size ();
}

inline size_t
CircularBufferConsumer::remaining (void) const
{
  check_context (__func__);

  ContextGuard context_guard (*context_);

  return *index_ - local_index_;
}

inline size_t
CircularBufferConsumer::count (void) const
{
  check_context (__func__);

  ContextGuard context_guard (*context_);

  return std::min ((size_t)(*index_ + 1), buffer_.size ());
}

inline void
CircularBufferConsumer::resize (void)
{
  check_context (__func__);

  ContextGuard context_guard (*context_);

  buffer_.resize (-1, false);
}

inline void
CircularBufferConsumer::resync (void)
{
  if (context_ && name_ != "")
  {
    ContextGuard context_guard (*context_);

    local_index_ = *index_;
  }
  else
  {
    throw exceptions::ContextException ("CircularBufferConsumer::resync: "
      " context is null or name hasn't been set.");
  }
}

inline void
CircularBufferConsumer::set_name (
  const std::string & name, KnowledgeBase & knowledge)
{
  if (name != "")
  {
    ContextGuard context_guard (knowledge);
    name_ = name;
    context_ = &(knowledge.get_context ());
    index_.set_name (name + ".index", knowledge);
    buffer_.set_name (name, knowledge);
    local_index_ = -1;
  }
  else
  {
    throw exceptions::NameException (
      "CircularBufferConsumer::set_name: empty name provided.");
  }
}

inline void
CircularBufferConsumer::set_name (const std::string & name,
  Variables & knowledge)
{
  if (name != "")
  {
    ContextGuard context_guard (*knowledge.get_context ());
    name_ = name;
    context_ = knowledge.get_context ();
    index_.set_name (name + ".index", knowledge);
    buffer_.set_name (name, knowledge);
    local_index_ = -1;
  }
  else
  {
    throw exceptions::NameException (
      "CircularBufferConsumer::set_name: empty name provided.");
  }
}


inline void
CircularBufferConsumer::set_index (KnowledgeRecord::Integer index)
{
  check_context (__func__);

  local_index_ = index;
}

template <typename T> void
CircularBufferConsumer::consume_latest (size_t count,
  std::vector <T> & values) const
{
  // iterate over the returned records
  for (auto record : consume_latest (count))
  {
    // add them to the values
    values.push_back (record.to_any <T> ());
  }
}

template <typename T> void
CircularBufferConsumer::consume_latest (size_t count,
  std::vector <T> & values, size_t & dropped) const
{
  dropped = get_dropped ();

  // iterate over the returned records
  for (auto record : consume_latest (count))
  {
    // add them to the values
    values.push_back (record.to_any <T> ());
  }
}

inline std::vector <KnowledgeRecord>
CircularBufferConsumer::consume_latest (size_t count) const
{
  check_all (__func__);

  ContextGuard context_guard (*context_);

  std::vector <KnowledgeRecord> result;

  KnowledgeRecord::Integer cur = *index_ % buffer_.size ();

  count = std::min (count, (size_t)(*index_ - local_index_));

  for (size_t i = 0; i < count; ++i, cur = increment (cur, -1))
  {
    result.push_back (buffer_[(size_t)cur]);
  }

  // note the difference here is that reading the latest will change index
  local_index_ = *index_;

  return result;
}

inline std::vector <KnowledgeRecord>
CircularBufferConsumer::consume_latest (size_t count, size_t & dropped) const
{
  check_all (__func__);

  ContextGuard context_guard (*context_);

  std::vector <KnowledgeRecord> result;

  dropped = get_dropped ();

  KnowledgeRecord::Integer cur = *index_ % buffer_.size ();

  count = std::min (count, (size_t)(*index_ - local_index_));

  for (size_t i = 0; i < count; ++i, cur = increment (cur, -1))
  {
    result.push_back (buffer_[(size_t)cur]);
  }

  // note the difference here is that reading the latest will change index
  local_index_ = *index_;

  return result;
}

template <typename T> void
CircularBufferConsumer::consume_earliest (size_t count,
  std::vector <T> & values) const
{
  // iterate over the returned records
  for (auto record : consume_earliest (count))
  {
    // add them to the values
    values.push_back (record.to_any <T> ());
  }
}

template <typename T> void
CircularBufferConsumer::consume_earliest (size_t count,
  std::vector <T> & values, size_t & dropped) const
{
  dropped = get_dropped ();

  // iterate over the returned records
  for (auto record : consume_earliest (count))
  {
    // add them to the values
    values.push_back (record.to_any <T> ());
  }
}

inline std::vector <KnowledgeRecord>
CircularBufferConsumer::consume_earliest (size_t count) const
{
  check_all (__func__);

  ContextGuard context_guard (*context_);
  std::vector <KnowledgeRecord> result;

  KnowledgeRecord::Integer index_diff = (*index_ - local_index_);

  // If buffer overflowed, update local index to last valid value - 1
  if (index_diff > (KnowledgeRecord::Integer)buffer_.size ())
  {
    local_index_ = *index_ - (KnowledgeRecord::Integer)buffer_.size ();
    index_diff = (KnowledgeRecord::Integer)buffer_.size ();
  }

  count = std::min (count, (size_t)index_diff);

  // start is either 0 or index_ + 1
  KnowledgeRecord::Integer cur =
    index_diff < (KnowledgeRecord::Integer)buffer_.size () ? 
    increment (local_index_, 1) :
    increment (*index_, -(KnowledgeRecord::Integer)(buffer_.size ()) + 1);

  for (size_t i = 0; i < count; ++i, cur = increment (cur, 1))
  {
    result.push_back (buffer_[(size_t)cur]);
  }

  local_index_ += (KnowledgeRecord::Integer)result.size ();

  return result;
}

inline std::vector <KnowledgeRecord>
CircularBufferConsumer::consume_earliest (
  size_t count, size_t & dropped) const
{
  check_all (__func__);

  ContextGuard context_guard (*context_);
  std::vector <KnowledgeRecord> result;

  KnowledgeRecord::Integer index_diff = (*index_ - local_index_);

  dropped = get_dropped ();

  count = std::min (count, (size_t)index_diff);

  // start is either 0 or index_ + 1
  KnowledgeRecord::Integer cur =
    index_diff < (KnowledgeRecord::Integer)buffer_.size () ? 
    increment (local_index_, 1) :
    increment (*index_, -(KnowledgeRecord::Integer)(buffer_.size ()) + 1);

  for (size_t i = 0; i < count; ++i, cur = increment (cur, 1))
  {
    result.push_back (buffer_[(size_t)cur]);
  }

  local_index_ += (KnowledgeRecord::Integer)result.size ();

  return result;
}

inline size_t
CircularBufferConsumer::get_dropped (void) const
{
  check_all (__func__);

  ContextGuard context_guard (*context_);

  size_t difference = remaining ();
  size_t buffer_size = size ();

  if (difference > buffer_size)
  {
    return difference - buffer_size;
  } 
  else
  {
    return 0;
  }
}

template <typename T> void
CircularBufferConsumer::peek_latest (size_t count,
  std::vector <T> & values) const
{
  // iterate over the returned records
  for (auto record : peek_latest (count))
  {
    // add them to the values
    values.push_back (record.to_any <T> ());
  }
}

inline std::vector <KnowledgeRecord>
CircularBufferConsumer::peek_latest (size_t count) const
{
  check_name (__func__);

  ContextGuard context_guard (*context_);

  count = std::min (count, this->count ());

  std::vector <KnowledgeRecord> result;

  KnowledgeRecord::Integer cur = *index_ % buffer_.size ();

  for (size_t i = 0; i < count; ++i, cur = increment (cur, -1))
  {
    result.push_back (buffer_[(size_t)cur]);
  }

  return result;
}

template <typename T> void
CircularBufferConsumer::peek_latest (T & value) const
{
  check_all (__func__);

  ContextGuard context_guard (*context_);

  if (count () > 0)
    value = peek_latest ().to_any <T> ();
  else
    throw exceptions::IndexException ("CircularBufferConsumer::peek_latest<T>: "
      "attempted consume on empty consumer buffer");
}

inline madara::knowledge::KnowledgeRecord
CircularBufferConsumer::peek_latest (void) const
{
  check_all (__func__);

  ContextGuard context_guard (*context_);

  KnowledgeRecord::Integer index = *index_;
  index = increment (index, 0);

  if (count () > 0)
    return context_->get (buffer_.vector_[(size_t)index]);
  else
    return KnowledgeRecord ();
}

} // end containers namespace
} // end knowledge namespace
} // end madara namespace

#endif //  _MADARA_KNOWLEDGE_CONTAINERS_CIRCULARBUFFERCONSUMER_INL_
