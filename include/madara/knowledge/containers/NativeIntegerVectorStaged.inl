
#ifndef _MADARA_NATIVE_INTEGER_VECTOR_STAGED_INL_
#define _MADARA_NATIVE_INTEGER_VECTOR_STAGED_INL_

#include "NativeIntegerVectorStaged.h"
#include "madara/knowledge/ContextGuard.h"

inline void
madara::knowledge::containers::NativeIntegerVectorStaged::modify (void)
{
  ContextGuard context_guard (*context_);
  if (context_ && name_ != "")
  {
    context_->mark_modified (vector_);
  }
}

inline std::string
madara::knowledge::containers::NativeIntegerVectorStaged::get_debug_info (
  void)
{
  std::stringstream result;

  result << "Native Integer Vector Staged: ";

  if (context_)
  {
    result << this->name_;
    result << " [" << size () << "]";
    result << " = " << value_.to_string ();
  }

  return result.str ();
}

inline std::string
madara::knowledge::containers::NativeIntegerVectorStaged::get_debug_info_ (
  void)
{
  return get_debug_info ();
}

inline void
madara::knowledge::containers::NativeIntegerVectorStaged::operator= (
  const NativeIntegerVectorStaged & rhs)
{
  if (this != &rhs)
  {
    MADARA_GUARD_TYPE guard (mutex_), guard2 (rhs.mutex_);

    this->context_ = rhs.context_;
    this->name_ = rhs.name_;
    this->settings_ = rhs.settings_;
    this->vector_ = rhs.vector_;
    this->value_ = rhs.value_;
    this->has_changed_ = true;
  }
}

inline void
madara::knowledge::containers::NativeIntegerVectorStaged::operator= (
  const std::vector <type> & rhs)
{
  if (!has_changed_)
    has_changed_ = true;

  value_ = knowledge::KnowledgeRecord (rhs);
}

inline void
madara::knowledge::containers::NativeIntegerVectorStaged::push_back (
  type value)
{
  if (!has_changed_)
    has_changed_ = true;

  size_t i = size ();
  resize ((int)i + 1);
  set (i, value);
}

inline void
madara::knowledge::containers::NativeIntegerVectorStaged::resize (
  size_t size)
{
  if (!has_changed_)
    has_changed_ = true;

  value_.resize (size);
}

inline size_t
madara::knowledge::containers::NativeIntegerVectorStaged::size (void) const
{
  return value_.size ();
}

inline void
madara::knowledge::containers::NativeIntegerVectorStaged::set_name (
  const std::string & var_name,
  KnowledgeBase & knowledge, int size,
  bool sync)
{
  if (context_ != &(knowledge.get_context ()) || name_ != var_name)
  {
    context_ = &(knowledge.get_context ());

    ContextGuard context_guard (*context_);
    MADARA_GUARD_TYPE guard (mutex_);

    name_ = var_name;

    vector_ = knowledge.get_ref (var_name, settings_);

    if (size > 0)
    {
      resize (size_t (size));
    }
    else if (sync)
    {
      std::vector <type> values = knowledge.get (vector_).to_integers ();
      value_.set_value (values);
    }
  }
}

inline void
madara::knowledge::containers::NativeIntegerVectorStaged::set_name (
  const std::string & var_name,
  Variables & knowledge, int size,
  bool sync)
{
  if (context_ != knowledge.get_context () || name_ != var_name)
  {
    context_ = knowledge.get_context ();

    ContextGuard context_guard (*context_);
    MADARA_GUARD_TYPE guard (mutex_);

    name_ = var_name;
    
    vector_ = knowledge.get_ref (var_name, settings_);

    if (size > 0)
    {
      resize (size_t (size));
    }
    else if (sync)
    {
      std::vector <type> values = knowledge.get (vector_).to_integers ();
      value_.set_value (values);
    }
  }
}

inline void
madara::knowledge::containers::NativeIntegerVectorStaged::set_name (
  const std::string & var_name,
  ThreadSafeContext & knowledge, int size,
  bool sync)
{
  if (context_ != &knowledge || name_ != var_name)
  {
    context_ = &knowledge;

    ContextGuard context_guard (*context_);
    MADARA_GUARD_TYPE guard (mutex_);

    name_ = var_name;
    
    vector_ = knowledge.get_ref (var_name, settings_);

    if (size > 0)
    {
      resize (size_t (size));
    }
    else if (sync)
    {
      std::vector <type> values = knowledge.get (vector_).to_integers ();
      value_.set_value (values);
    }
  }
}

inline void
madara::knowledge::containers::NativeIntegerVectorStaged::exchange (
  NativeIntegerVectorStaged & other)
{
  knowledge::KnowledgeRecord temp (other.value_);

  other.value_ = value_;
  value_ = temp;

  if (!has_changed_)
    has_changed_ = true;

  if (!other.has_changed_)
    other.has_changed_ = true;
}

inline void
madara::knowledge::containers::NativeIntegerVectorStaged::transfer_to (
  NativeIntegerVectorStaged & other)
{
  size_t other_size = other.size ();
  size_t this_size = this->size ();

  if (this_size > 0)
  {
    size_t size = other_size + this_size;
    other.resize ((int)size);

    knowledge::KnowledgeRecord & rhs (other.value_);
    knowledge::KnowledgeRecord & lhs (value_);

    rhs.set_index (size - 1, lhs.retrieve_index (this_size - 1).to_double ());

    for (size_t i = 0, j = other_size; i < this_size - 1; ++i, ++j)
    {
      rhs.set_index (j, lhs.retrieve_index (i).to_double ());
    }

    lhs.reset_value ();

    if (!has_changed_)
      has_changed_ = true;
  }
}

inline void
madara::knowledge::containers::NativeIntegerVectorStaged::copy_to (
  KnowledgeVector & target) const
{
  target.resize (size ());

  for (size_t i = 0; i < target.size (); ++i)
  {
    target[i] = value_.retrieve_index (i);
  }
}

inline madara::knowledge::containers::NativeIntegerVectorStaged::type
madara::knowledge::containers::NativeIntegerVectorStaged::operator[] (
  size_t index) const
{
  knowledge::KnowledgeRecord result;

  result = result.retrieve_index (index);

  return result.to_integer ();
}

inline void
madara::knowledge::containers::NativeIntegerVectorStaged::set (
  size_t index,
  type value)
{
  if (!has_changed_)
    has_changed_ = true;

  value_.set_index (index, value);
}

inline void
madara::knowledge::containers::NativeIntegerVectorStaged::set (
  const std::vector <type> & value)
{
  if (!has_changed_)
    has_changed_ = true;

  value_ = knowledge::KnowledgeRecord (value);
}
 
inline void
madara::knowledge::containers::NativeIntegerVectorStaged::set_quality (
  size_t /*index*/,
  uint32_t quality,
  const KnowledgeReferenceSettings & settings)
{
  if (context_)
  {
    ContextGuard context_guard (*context_);
    MADARA_GUARD_TYPE guard (mutex_);
    context_->set_quality (name_, quality, true, settings);
  }
}

inline madara::knowledge::KnowledgeRecord
madara::knowledge::containers::NativeIntegerVectorStaged::to_record (
  size_t index) const
{
  madara::knowledge::KnowledgeRecord result;
  result = value_.retrieve_index (index);
  return result;
}

inline madara::knowledge::KnowledgeRecord
madara::knowledge::containers::NativeIntegerVectorStaged::to_record (
  void) const
{
  return value_;
}

inline bool
madara::knowledge::containers::NativeIntegerVectorStaged::is_true (void) const
{
  bool result (false);

  madara_logger_log (context_->get_logger (), logger::LOG_MAJOR,
    "NativeIntegerVectorStaged::is_true: checking for non-zero value\n");

  result = value_.is_true ();

  madara_logger_log (context_->get_logger (), logger::LOG_MAJOR,
    "NativeIntegerVectorStaged::is_true: final result is %d\n",
    (int)result);

  return result;
}

inline bool
madara::knowledge::containers::NativeIntegerVectorStaged::is_false (void) const
{
  return !is_true ();
}

inline void
madara::knowledge::containers::NativeIntegerVectorStaged::read (void)
{
  has_changed_ = false;
  value_ = context_->get (vector_);
}

inline void
madara::knowledge::containers::NativeIntegerVectorStaged::write (void)
{
  if (has_changed_)
    context_->set (vector_, value_);
}


#endif // _MADARA_NATIVE_INTEGER_VECTOR_STAGED_INL_
