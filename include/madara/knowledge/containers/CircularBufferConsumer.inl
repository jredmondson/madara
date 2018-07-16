
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
  
inline CircularBufferConsumer::CircularBufferConsumer (
  const std::string & name,
  KnowledgeBase & knowledge)
: context_ (&(knowledge.get_context ())), name_ (name),
  local_index_ (-1)
{
  if (name != "")
  {
    ContextGuard context_guard (knowledge);
    set_name (name, knowledge);
  }
  else
  {
    throw exceptions::NameException (
      "CircularBufferConsumer::constr: empty name provided.");
  }
}
 
inline CircularBufferConsumer::CircularBufferConsumer (
  const std::string & name,
  Variables & knowledge)
: context_ (knowledge.get_context ()), name_ (name),
  local_index_ (-1)
{
  if (name != "")
  {
    ContextGuard context_guard (*knowledge.get_context ());
    set_name (name, knowledge);
  }
  else
  {
    throw exceptions::NameException (
      "CircularBufferConsumer::constr: empty name provided.");
  }
}

inline CircularBufferConsumer::CircularBufferConsumer (const CircularBufferConsumer & rhs)
  : context_ (rhs.context_),
    name_ (rhs.name_),
    index_ (rhs.index_),
    local_index_ (rhs.local_index_),
    buffer_ (rhs.buffer_)
{

}

inline void
CircularBufferConsumer::operator= (const CircularBufferConsumer & rhs)
{
  if (this != &rhs)
  {
    this->context_ = rhs.context_;
    this->name_ = rhs.name_;
    this->index_ = rhs.index_;
    this->buffer_ = rhs.buffer_;
    this->local_index_ = rhs.local_index_;
  }
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

inline madara::knowledge::KnowledgeRecord
CircularBufferConsumer::consume (void) const
{
  if (context_ && name_ != "" && buffer_.size () > 0)
  {
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
    message << "CircularBufferConsumer::get: ";
    message << "Invalid access because " << reason << "\n";
    throw exceptions::IndexException (message.str ()); 
  }
}

inline madara::knowledge::KnowledgeRecord
CircularBufferConsumer::inspect (
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
        local_index_, (KnowledgeRecord::Integer)position);
      
      return context_->get (buffer_.vector_[index]);
    }
    else
    {
      std::stringstream message;
      message << "CircularBufferConsumer::inspect: ";
      message << "Invalid access for " << position << " element when count is ";
      message << inserted << "\n";
      throw exceptions::IndexException (message.str ()); 
    }
  }
  else
  {
    throw exceptions::ContextException ("CircularBufferConsumer::inspect: "
      " context is null or name hasn't been set.");
  }
}

inline std::vector <KnowledgeRecord>
CircularBufferConsumer::inspect (
  KnowledgeRecord::Integer position,
  size_t count) const
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
      message << "Invalid access for " << position << " element when count is ";
      message << inserted << "\n";
      throw exceptions::IndexException (message.str ()); 
    }
  }
  else
  {
    throw exceptions::ContextException ("CircularBufferConsumer::inspect: "
      " context is null or name hasn't been set.");
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
  if (context_ && name_ != "")
  {
    ContextGuard context_guard (*context_);

    return *index_ - local_index_;
  }
  else
  {
    throw exceptions::ContextException ("CircularBufferConsumer::remaining: "
      " context is null or name hasn't been set.");
  }
}

inline size_t
CircularBufferConsumer::count (void) const
{
  if (context_ && name_ != "")
  {
    ContextGuard context_guard (*context_);

    return std::min ((size_t)(*index_ + 1), buffer_.size ());
  }
  else
  {
    throw exceptions::ContextException ("CircularBufferConsumer::count: "
      " context is null or name hasn't been set.");
  }
}

inline void
CircularBufferConsumer::resize (void)
{
  if (context_ && name_ != "")
  {
    ContextGuard context_guard (*context_);

    buffer_.resize (-1, false);
  }
  else
  {
    throw exceptions::ContextException ("CircularBufferConsumer::resize: "
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
  if (name_ != "" && context_ != 0)
  {
    local_index_ = index;
  }
  else
  {
    throw exceptions::ContextException ("CircularBufferConsumer::set_index: "
      " context is null or name hasn't been set.");
  }
}

inline std::vector <KnowledgeRecord>
CircularBufferConsumer::consume_latest (size_t count) const
{
  if (context_ && name_ != "" && buffer_.size () > 0)
  {
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
    message << "CircularBufferConsumer::consume_latest: ";
    message << "Invalid access because " << reason << "\n";
    throw exceptions::IndexException (message.str ()); 
  }
}      

inline std::vector <KnowledgeRecord>
CircularBufferConsumer::consume_earliest (size_t count) const
{
  if (context_ && name_ != "" && buffer_.size () > 0)
  {
    ContextGuard context_guard (*context_);
    std::vector <KnowledgeRecord> result;

    KnowledgeRecord::Integer index_diff = (*index_ - local_index_);

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
    message << "CircularBufferConsumer::consume_earliest: ";
    message << "Invalid access because " << reason << "\n";
    throw exceptions::IndexException (message.str ()); 
  }
}


inline std::vector <KnowledgeRecord>
CircularBufferConsumer::peak_latest (size_t count) const
{
  if (context_ && name_ != "")
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
    message << "CircularBuffer::get_latest: ";
    message << "Invalid access because " << reason << "\n";
    throw exceptions::IndexException (message.str ()); 
  }
}      

inline madara::knowledge::KnowledgeRecord
CircularBufferConsumer::peak_latest (void) const
{
  if (context_ && name_ != "" && buffer_.size () > 0)
  {
    ContextGuard context_guard (*context_);

    KnowledgeRecord::Integer index = *index_;
    index = increment (index, 0);

    if (count () > 0)
      return context_->get (buffer_.vector_[(size_t)index]);
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
    message << "CircularBuffer::get: ";
    message << "Invalid access because " << reason << "\n";
    throw exceptions::IndexException (message.str ()); 
  }
}


} // end containers namespace
} // end knowledge namespace
} // end madara namespace

#endif //  _MADARA_KNOWLEDGE_CONTAINERS_CIRCULARBUFFERCONSUMER_INL_
