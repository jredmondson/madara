
#ifndef _MADARA_KNOWLEDGE_CONTAINERS_NATIVECIRCULARBUFFERCONSUMER_INL_
#define _MADARA_KNOWLEDGE_CONTAINERS_NATIVECIRCULARBUFFERCONSUMER_INL_

#include <sstream>
#include <math.h>

#include "NativeCircularBufferConsumer.h"
#include "madara/knowledge/ContextGuard.h"
#include "madara/exceptions/IndexException.h"
#include "madara/exceptions/ContextException.h"
#include "madara/exceptions/NameException.h"

namespace madara { namespace knowledge { namespace containers {

inline NativeCircularBufferConsumer::NativeCircularBufferConsumer ()
: context_ (0), local_index_ (-1)
{
}

inline void NativeCircularBufferConsumer::check_name (const char *func,
    const char *name)
{
  if (name == nullptr || name[0] == '\0') {
    throw exceptions::NameException (
      std::string("NativeCircularBufferConsumer::") + func + ": name is empty.");
  }
}

inline void NativeCircularBufferConsumer::check_context (const char *func) const
{
  if (!context_)
  {
    throw exceptions::ContextException (
      std::string("NativeCircularBufferConsumer::") + func + ": context is not set.");
  }
  if (!ref_.is_valid())
  {
    throw exceptions::ContextException (
      std::string("NativeCircularBufferConsumer::") + func + ": underlying record is not set.");
  }
}

inline NativeCircularBufferConsumer::NativeCircularBufferConsumer (
  const std::string & name,
  KnowledgeBase & knowledge)
: context_ (&(knowledge.get_context ())), local_index_ (-1UL)
{
  check_name (__func__, name.c_str());

  ContextGuard context_guard (knowledge);
  set_name (name, knowledge);
}
 
inline NativeCircularBufferConsumer::NativeCircularBufferConsumer (
  const std::string & name,
  Variables & knowledge)
: context_ (knowledge.get_context ()), local_index_ (-1UL)
{
  check_name (__func__, name.c_str());

  ContextGuard context_guard (*knowledge.get_context ());
  set_name (name, knowledge);
}

inline bool
NativeCircularBufferConsumer::operator== (
  const NativeCircularBufferConsumer & value) const
{
  return context_ == value.context_ && get_name () == value.get_name ();
}

inline bool
NativeCircularBufferConsumer::operator!= (
  const NativeCircularBufferConsumer & value) const
{
  return !operator==(value);
}

template <typename T> void
NativeCircularBufferConsumer::consume (T & value) const
{
  check_context (__func__);

  ContextGuard context_guard (*context_);

  if (remaining () > 0)
    value = consume ().to_any <T> ();
  else
    throw exceptions::IndexException ("NativeCircularBufferConsumer::consume<T>: "
      "attempted consume on empty consumer buffer");
}

template <typename T> void
NativeCircularBufferConsumer::consume (T & value, size_t & dropped) const
{
  check_context (__func__);

  ContextGuard context_guard (*context_);

  if (remaining () > 0)
    value = consume (dropped).to_any <T> ();
  else
    throw exceptions::IndexException ("NativeCircularBufferConsumer::consume<T>: "
      "attempted consume on empty consumer buffer");
}

inline madara::knowledge::KnowledgeRecord
NativeCircularBufferConsumer::consume (void) const
{
  size_t dropped;
  return consume(dropped);
}

inline madara::knowledge::KnowledgeRecord
NativeCircularBufferConsumer::consume (size_t & dropped) const
{
  check_context (__func__);

  ContextGuard context_guard (*context_);

  KnowledgeRecord &rec = *ref_.get_record_unsafe();
  size_t newest_index = rec.get_history_newest_index();
  size_t oldest_index = rec.get_history_oldest_index();

  if (local_index_ > newest_index) {
    return KnowledgeRecord();
  }

  if (local_index_ < oldest_index) {
    dropped = oldest_index - local_index_;
    local_index_ = oldest_index;
  } else {
    dropped = 0;
  }

  KnowledgeRecord ret;
  rec.get_history_range(&ret, local_index_, 1);
  ++local_index_;
  return ret;
}


template <typename T> void
NativeCircularBufferConsumer::consume_earliest (size_t count,
  std::vector <T> & values) const
{
  uint64_t latest_toi = 0;
  // iterate over the returned records
  for (auto record : consume_earliest (count))
  {
    // add them to the values if valid
    if (record.is_valid() && record.toi >= latest_toi)
    {
      latest_toi = record.toi;
      values.push_back (record.to_any <T> ());
    }
  }
}

inline std::vector <KnowledgeRecord>
NativeCircularBufferConsumer::consume_earliest (size_t count) const
{
  check_context (__func__);

  ContextGuard context_guard (*context_);
  KnowledgeRecord &rec = *ref_.get_record_unsafe();

  KnowledgeRecord ret;
  std::vector <KnowledgeRecord> retvec;
  for ( uint idx=0; idx < count ; ++idx )
  {
    rec.get_history_range(&ret, local_index_, idx);
    retvec.push_back(ret);
  }

  if ( count > size() )
    local_index_ += count;
  else
    //Placeholder for the alternate case.
    local_index_ = local_index_;

  return retvec;
}

template <typename T> void
NativeCircularBufferConsumer::inspect (KnowledgeRecord::Integer position, T & value) const
{
  value = inspect (position).to_any <T> ();
}

inline madara::knowledge::KnowledgeRecord
NativeCircularBufferConsumer::inspect (KnowledgeRecord::Integer position) const
{
  check_context (__func__);

  ContextGuard context_guard (*context_);
  KnowledgeRecord &rec = *ref_.get_record_unsafe();
  //size_t newest_index = rec.get_history_newest_index();
  size_t oldest_index = rec.get_history_oldest_index();

  // If buffer overflowed, update local index to last valid value - 1
  KnowledgeRecord::Integer index_diff = (oldest_index - local_index_);
  if (index_diff > (KnowledgeRecord::Integer)rec.get_history_size() )
  {
    local_index_ = oldest_index - rec.get_history_size();
  }

  KnowledgeRecord::Integer requested_index = local_index_ + position;

  if (0 <= requested_index &&
      (KnowledgeRecord::Integer)(oldest_index - rec.get_history_size()) <= requested_index &&
      (size_t)requested_index <= oldest_index)
  {
    size_t index = (size_t) local_index_+position;
    KnowledgeRecord ret;
    ret = rec.get_history(index);
  }
  else
  {
    std::stringstream message;
    message << "NativeCircularBufferConsumer::inspect: ";
    message << "Invalid access for relative position " << position << " when buffer index is ";
    message << oldest_index << " and local index is : " << local_index_ << " and size is : "
      << rec.get_history_size() << "\n";
    throw exceptions::IndexException (message.str ()); 
  }

}


template <typename T> void
NativeCircularBufferConsumer::inspect (KnowledgeRecord::Integer position,
                                       size_t count, std::vector <T> & values) const
{
  uint64_t latest_toi = 0;

  // iterate over the returned records
  for (auto record : inspect (position, count))
  {
    // add them to the values if valid and increasing toi
    if (record.is_valid() && record.toi >= latest_toi)
    {
      latest_toi = record.toi;
      values.push_back (record.to_any <T> ());
    }
  }
}

inline std::vector <KnowledgeRecord>
NativeCircularBufferConsumer::inspect (KnowledgeRecord::Integer position,size_t count) const
{
  check_context (__func__);

  ContextGuard context_guard (*context_);

  // If buffer overflowed, update local index to last valid value - 1
  KnowledgeRecord &rec = *ref_.get_record_unsafe();
  size_t oldest_index = rec.get_history_oldest_index();
  size_t newest_index = rec.get_history_newest_index();

  KnowledgeRecord::Integer index_diff = (oldest_index - local_index_);
  if (index_diff > (KnowledgeRecord::Integer)rec.get_history_size ())
  {
    local_index_ = oldest_index - rec.get_history_size ();
  }

  KnowledgeRecord::Integer requested_index = local_index_ + position;

  if (0 <= requested_index &&
    (oldest_index - (KnowledgeRecord::Integer)rec.get_history_size ()) <= requested_index &&
    requested_index <= oldest_index)
  {
    std::vector <KnowledgeRecord> retvec;
    KnowledgeRecord::Integer index = local_index_ + position;
    for ( uint idx=0; idx < count ; ++idx )
    {
      KnowledgeRecord &rec = *ref_.get_record_unsafe();
      KnowledgeRecord ret;
      rec.get_history(&ret,local_index_,idx);
      retvec.push_back(ret);
    }

    return retvec;
  }
  else
  {
    std::stringstream message;
    message << "NativeCircularBufferConsumer::inspect: ";
    message << "Invalid access for relative position " << position << " when buffer index is ";
    message << oldest_index << " and local index is : " << local_index_ <<
      " and size is : " << rec.get_history_size() << "\n";
    throw exceptions::IndexException (message.str ()); 
  }
}


inline std::string
NativeCircularBufferConsumer::get_name (void) const
{
  return ref_.get_name();
}

inline size_t
NativeCircularBufferConsumer::size (void) const
{
  check_context (__func__);

  ContextGuard context_guard (*context_);

  KnowledgeRecord &rec = *ref_.get_record_unsafe();
  return rec.get_history_capacity ();
}

inline size_t
NativeCircularBufferConsumer::remaining (void) const
{
  check_context (__func__);

  ContextGuard context_guard (*context_);

  KnowledgeRecord &rec = *ref_.get_record_unsafe();
  size_t newest_index = rec.get_history_newest_index();
  if (local_index_ > newest_index) {
    return 0;
  }
  return newest_index + 1 - local_index_;
}

inline size_t
NativeCircularBufferConsumer::count (void) const
{
  check_context (__func__);

  ContextGuard context_guard (*context_);

  KnowledgeRecord &rec = *ref_.get_record_unsafe();
  return rec.get_history_size ();
}

inline void
NativeCircularBufferConsumer::set_name (
  const std::string & name, ThreadSafeContext & context)
{
  check_name(__func__, name.c_str());

  ContextGuard context_guard (context);

  context_ = &context;
  ref_ = context_->get_ref(name);
  local_index_ = 0;
}

inline void
NativeCircularBufferConsumer::set_name (
  const std::string & name, KnowledgeBase & knowledge)
{
  set_name (name, knowledge.get_context ());
}

inline void
NativeCircularBufferConsumer::set_name (const std::string & name,
  Variables & knowledge)
{
  set_name (name, *knowledge.get_context ());
}


inline void
NativeCircularBufferConsumer::set_index (size_t index)
{
  check_context (__func__);

  ContextGuard context_guard (*context_);

  local_index_ = index;
}

inline size_t
NativeCircularBufferConsumer::get_dropped (void) const
{
  check_context (__func__);

  ContextGuard context_guard (*context_);

  KnowledgeRecord &rec = *ref_.get_record_unsafe();
  size_t oldest_index = rec.get_history_oldest_index();
  if (local_index_ < oldest_index) {
    return oldest_index - local_index_;
  }
  return 0;
}

inline KnowledgeRecord
NativeCircularBufferConsumer::get_record (void) const
{
  check_context (__func__);

  ContextGuard context_guard (*context_);

  return *ref_.get_record_unsafe();
}


} // end containers namespace
} // end knowledge namespace
} // end madara namespace

#endif //  _MADARA_KNOWLEDGE_CONTAINERS_NATIVECIRCULARBUFFERCONSUMER_INL_
