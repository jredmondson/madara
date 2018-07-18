
#ifndef _MADARA_KNOWLEDGE_CONTAINERS_CIRCULARBUFFERSTAMPED_INL_
#define _MADARA_KNOWLEDGE_CONTAINERS_CIRCULARBUFFERSTAMPED_INL_

#include <sstream>
#include <math.h>

#include "CircularBufferStamped.h"
#include "madara/knowledge/ContextGuard.h"
#include "madara/exceptions/IndexException.h"
#include "madara/exceptions/ContextException.h"
#include "madara/exceptions/NameException.h"

namespace madara { namespace knowledge { namespace containers {

inline CircularBufferStamped::CircularBufferStamped (
  const KnowledgeUpdateSettings & settings)
: context_ (0), settings_ (settings)
{
}
  
inline CircularBufferStamped::CircularBufferStamped (
  const std::string & name,
  KnowledgeBase & knowledge,
  const KnowledgeUpdateSettings & settings)
: context_ (&(knowledge.get_context ())),
  settings_ (settings) 
{
  if (name != "")
  {
    ContextGuard context_guard (knowledge);
    set_name (name, knowledge);
  }
  else
  {
    throw exceptions::NameException (
      "CircularBufferStamped::constr: empty name provided.");
  }
}
 
inline CircularBufferStamped::CircularBufferStamped (
  const std::string & name,
  Variables & knowledge,
  const KnowledgeUpdateSettings & settings)
: context_ (knowledge.get_context ()),
  settings_ (settings) 
{
  if (name != "")
  {
    ContextGuard context_guard (*knowledge.get_context ());
    set_name (name, knowledge);
  }
  else
  {
    throw exceptions::NameException (
      "CircularBufferStamped::constr: empty name provided.");
  }
}
 
inline CircularBufferStamped::CircularBufferStamped (
  const std::string & name,
  KnowledgeBase & knowledge,
  int size,
  const KnowledgeUpdateSettings & settings)
: context_ (&(knowledge.get_context ())), name_ (name),
  settings_ (settings) 
{
  if (name != "")
  {
    ContextGuard context_guard (knowledge);
    set_name (name, knowledge);
    if (size >= 0)
    {
      buffer_.resize (size);
    }
  }
  else
  {
    throw exceptions::NameException (
      "CircularBufferStamped::constr: empty name provided.");
  }
}
 
inline CircularBufferStamped::CircularBufferStamped (
  const std::string & name,
  Variables & knowledge,
  int size,
  const KnowledgeUpdateSettings & settings)
: context_ (knowledge.get_context ()), name_ (name),
  settings_ (settings) 
{
  if (name != "")
  {
    ContextGuard context_guard (*knowledge.get_context ());
    set_name (name, knowledge);
    if (size >= 0)
    {
      buffer_.resize (size);
    }
  }
  else
  {
    throw exceptions::NameException (
      "CircularBufferStamped::constr: empty name provided.");
  }
}


inline CircularBufferStamped::CircularBufferStamped (const CircularBufferStamped & rhs)
  : context_ (rhs.context_),
    name_ (rhs.name_),
    index_ (rhs.index_),
    buffer_ (rhs.buffer_),
    settings_ (rhs.settings_)
{

}

inline void
CircularBufferStamped::operator= (const CircularBufferStamped & rhs)
{
  if (this != &rhs)
  {
    this->context_ = rhs.context_;
    this->name_ = rhs.name_;
    this->index_ = rhs.index_;
    this->buffer_ = rhs.buffer_;
    this->settings_ = rhs.settings_;
  }
}


inline bool
CircularBufferStamped::operator== (
  const CircularBufferStamped & value) const
{
  return name_ == value.get_name ();
}
        
inline bool
CircularBufferStamped::operator!= (
  const CircularBufferStamped & value) const
{
  return name_ != value.get_name ();
}

inline void
CircularBufferStamped::clear (void)
{
  if (context_ && name_ != "")
  {
    ContextGuard context_guard (*context_);

    index_ = -1;
    buffer_.resize (0);
  }
  else
  {
    throw exceptions::ContextException ("CircularBufferStamped::resize: "
      " context is null or name hasn't been set.");
  }
}

inline madara::knowledge::KnowledgeRecord::Integer
CircularBufferStamped::increment (
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
    message << "CircularBufferStamped::increment: ";
    message << "Result of " << base << "+" << value << " is " << result;
    message << " which is impossible to access with size " << size () << ".\n";
    throw exceptions::IndexException (message.str ()); 
  }
}

template <typename T>
void CircularBufferStamped::add (const T & value)
{
  Any any_record (value);
  add (KnowledgeRecord (any_record));
}

inline void 
CircularBufferStamped::add (
  const knowledge::KnowledgeRecord & record)
{
  if (context_ && name_ != "" && buffer_.size () > 0)
  {
    ContextGuard context_guard (*context_);

    KnowledgeRecord::Integer index = increment (*index_, 1);

    context_->set (buffer_.vector_[(size_t)index], record, settings_);

    ++index_;
  }
  else
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

    std::stringstream message;
    message << "CircularBufferStamped::add: ";
    message << "Invalid access because " << reason << "\n";
    throw exceptions::IndexException (message.str ()); 
  }
}


template <typename T>
void 
CircularBufferStamped::add (
  const std::vector <T> & records)
{
  for (auto record: records)
  {
    add (record);
  }
}

inline void 
CircularBufferStamped::add (
  const std::vector<KnowledgeRecord> & records)
{
  if (context_ && name_ != "" && buffer_.size () > 0)
  {
    ContextGuard context_guard (*context_);

    KnowledgeRecord::Integer index = *index_;

    for (auto record : records)
    {
      index = increment (index, 1);

      context_->set (buffer_.vector_[(size_t)index], record, settings_);
    }

    index_ += (KnowledgeRecord::Integer)records.size ();
  }
  else
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

    std::stringstream message;
    message << "CircularBufferStamped::add: ";
    message << "Invalid access because " << reason << "\n";
    throw exceptions::IndexException (message.str ()); 
  }
}

template <typename T> void
CircularBufferStamped::get (T & value) const
{
  value = get ().to_any <T> ();
}

inline madara::knowledge::KnowledgeRecord
CircularBufferStamped::get (void) const
{
  if (context_ && name_ != "" && buffer_.size () > 0)
  {
    ContextGuard context_guard (*context_);

    KnowledgeRecord::Integer index = *index_;
    index = increment (index, 0);

    if (count () > 0)
      return context_->get (buffer_.vector_[(size_t)index], settings_);
    else
      return KnowledgeRecord ();
  }
  else
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

    std::stringstream message;
    message << "CircularBufferStamped::get: ";
    message << "Invalid access because " << reason << "\n";
    throw exceptions::IndexException (message.str ()); 
  }
}

template <typename T> void
CircularBufferStamped::inspect (KnowledgeRecord::Integer position, T & value) const
{
  value = inspect (position).to_any <T> ();
}

inline madara::knowledge::KnowledgeRecord
CircularBufferStamped::inspect (
  KnowledgeRecord::Integer position) const
{
  if (context_ && name_ != "")
  {
    ContextGuard context_guard (*context_);

    KnowledgeRecord::Integer inserted = (KnowledgeRecord::Integer)count ();

    if ((position <= 0 && -position < inserted) ||
        (position > 0 && inserted == (KnowledgeRecord::Integer)size () &&
         position < inserted))
    {
      size_t index = (size_t)increment (
        *index_, (KnowledgeRecord::Integer)position);
      
      return context_->get (buffer_.vector_[index], settings_);
    }
    else
    {
      std::stringstream message;
      message << "CircularBufferStamped::inspect: ";
      message << "Invalid access for " << position << " element when count is ";
      message << inserted << "\n";
      throw exceptions::IndexException (message.str ()); 
    }
  }
  else
  {
    throw exceptions::ContextException ("CircularBufferStamped::inspect: "
      " context is null or name hasn't been set.");
  }
}

template <typename T> void
CircularBufferStamped::inspect (KnowledgeRecord::Integer position,
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
CircularBufferStamped::inspect (
  KnowledgeRecord::Integer position, size_t count) const
{
  if (context_ && name_ != "")
  {
    ContextGuard context_guard (*context_);

    KnowledgeRecord::Integer inserted =
      (KnowledgeRecord::Integer)this->count ();

    if ((position <= 0 && -position < inserted) ||
        (position > 0 && inserted == (KnowledgeRecord::Integer)size () &&
         position < inserted))
    {
      KnowledgeRecord::Integer index = increment (
        *index_, (KnowledgeRecord::Integer)position);
      
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
      message << "CircularBufferStamped::inspect: ";
      message << "Invalid access for " << position << " element when count is ";
      message << inserted << "\n";
      throw exceptions::IndexException (message.str ()); 
    }
  }
  else
  {
    throw exceptions::ContextException ("CircularBufferStamped::inspect: "
      " context is null or name hasn't been set.");
  }
}

inline std::string
CircularBufferStamped::get_name (void) const
{
  return name_;
}

inline size_t
CircularBufferStamped::size (void) const
{
  return buffer_.size ();
}

inline size_t
CircularBufferStamped::count (void) const
{
  if (context_ && name_ != "")
  {
    ContextGuard context_guard (*context_);

    return std::min ((size_t)(*index_ + 1), buffer_.size ());
  }
  else
  {
    throw exceptions::ContextException ("CircularBufferStamped::count: "
      " context is null or name hasn't been set.");
  }
}

inline void
CircularBufferStamped::resize (int size)
{
  if (context_ && name_ != "")
  {
    ContextGuard context_guard (*context_);

    buffer_.resize (size, false);
  }
  else
  {
    throw exceptions::ContextException ("CircularBufferStamped::resize: "
      " context is null or name hasn't been set.");
  }
}

inline madara::knowledge::KnowledgeUpdateSettings
CircularBufferStamped::set_settings (
  const KnowledgeUpdateSettings & settings)
{ 
  KnowledgeUpdateSettings old_settings = settings_;

  settings_ = settings;

  return old_settings;
}

inline void
CircularBufferStamped::set_quality (
  uint32_t quality,
  const KnowledgeReferenceSettings & settings)
{
  if (context_)
  {
    ContextGuard context_guard (*context_);
    context_->set_quality (name_, quality, true, settings);
  }
  else
  {
    throw exceptions::ContextException (
      "CircularBufferStamped::set_quality: context is null");
  }
}


inline void
CircularBufferStamped::set_name (
  const std::string & name, KnowledgeBase & knowledge)
{
  if (name != "")
  {
    ContextGuard context_guard (knowledge);
    name_ = name;
    context_ = &(knowledge.get_context ());
    index_.set_name (name + ".index", knowledge);
    buffer_.set_name (name, knowledge);
    if (buffer_.size () == 0 && index_ != -1)
    {
      index_ = -1;
    }
  }
  else
  {
    throw exceptions::NameException (
      "CircularBufferStamped::set_name: empty name provided.");
  }
}      

inline void
CircularBufferStamped::set_name (const std::string & name,
  Variables & knowledge)
{
  if (name != "")
  {
    ContextGuard context_guard (*knowledge.get_context ());
    name_ = name;
    context_ = knowledge.get_context ();
    index_.set_name (name + ".index", knowledge);
    buffer_.set_name (name, knowledge);
    if (buffer_.size () == 0 && index_ != -1)
    {
      index_ = -1;
    }
  }
  else
  {
    throw exceptions::NameException (
      "CircularBufferStamped::set_name: empty name provided.");
  }
}


inline void
CircularBufferStamped::set_index (KnowledgeRecord::Integer index)
{
  if (name_ != "" && context_ != 0)
  {
    index_ = index;
  }
  else
  {
    throw exceptions::ContextException ("CircularBufferStamped::set_index: "
      " context is null or name hasn't been set.");
  }
}

template <typename T> void
CircularBufferStamped::get_latest (size_t count, std::vector <T> & values) const
{
  // iterate over the returned records
  for (auto record : get_latest (count))
  {
    // add them to the values
    values.push_back (record.to_any <T> ());
  }
}

inline std::vector <KnowledgeRecord>
CircularBufferStamped::get_latest (size_t count) const
{
  if (context_ && name_ != "" && buffer_.size () > 0)
  {
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
  else
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

    std::stringstream message;
    message << "CircularBufferStamped::get_latest: ";
    message << "Invalid access because " << reason << "\n";
    throw exceptions::IndexException (message.str ()); 
  }
}      

template <typename T> void
CircularBufferStamped::get_earliest (size_t count, std::vector <T> & values) const
{
  // iterate over the returned records
  for (auto record : get_earliest (count))
  {
    // add them to the values
    values.push_back (record.to_any <T> ());
  }
}

inline std::vector <KnowledgeRecord>
CircularBufferStamped::get_earliest (size_t count) const
{
  if (context_ && name_ != "" && buffer_.size () > 0)
  {
    ContextGuard context_guard (*context_);
    std::vector <KnowledgeRecord> result;

    count = std::min (count, this->count ());

    // start is either 0 or index_ + 1
    KnowledgeRecord::Integer cur = this->count () < buffer_.size () ? 
      0 : increment (*index_, 1);

    for (size_t i = 0; i < count; ++i, cur = increment (cur, 1))
    {
      result.push_back (buffer_[(size_t)cur]);
    }

    return result;
  }
  else
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

    std::stringstream message;
    message << "CircularBufferStamped::get_earliest: ";
    message << "Invalid access because " << reason << "\n";
    throw exceptions::IndexException (message.str ()); 
  }
}

} // end containers namespace
} // end knowledge namespace
} // end madara namespace

#endif //  _MADARA_KNOWLEDGE_CONTAINERS_CIRCULARBUFFERSTAMPED_INL_
