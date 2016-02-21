#include "NativeDoubleVector.h"
#include "madara/knowledge/ContextGuard.h"
#include "madara/logger/GlobalLogger.h"

madara::knowledge::containers::NativeDoubleVector::NativeDoubleVector (
  const KnowledgeUpdateSettings & settings)
  : BaseContainer ("", settings), context_ (0)
{
  madara_logger_ptr_log (logger::global_logger.get (),
    logger::LOG_MAJOR, "NativeDoubleVector::constructor: new object\n");
}
   
madara::knowledge::containers::NativeDoubleVector::NativeDoubleVector (
  const std::string & name,
  KnowledgeBase & knowledge,
  int size,
  const KnowledgeUpdateSettings & settings)
  : BaseContainer (name, settings), context_ (&(knowledge.get_context ()))
{
  madara_logger_ptr_log (logger::global_logger.get (),
    logger::LOG_MAJOR,
    "NativeDoubleVector::constructor called for %s[%d]\n",
    name.c_str (), size);

  vector_ = knowledge.get_ref (name, settings_);
  if (size >= 0)
  {
    resize (size);
  }
}
  
madara::knowledge::containers::NativeDoubleVector::NativeDoubleVector (
  const std::string & name,
  Variables & knowledge,
  int size,
  const KnowledgeUpdateSettings & settings)
: context_ (knowledge.get_context ())
{
  madara_logger_ptr_log (logger::global_logger.get (),
    logger::LOG_MAJOR,
    "NativeDoubleVector::constructor called for %s[%d]\n",
    name.c_str (), size);

  vector_ = knowledge.get_ref (name, settings);
  if (size >= 0)
  {
    resize (size);
  }
}

madara::knowledge::containers::NativeDoubleVector::NativeDoubleVector (
  const NativeDoubleVector & rhs)
: BaseContainer (rhs), context_ (rhs.context_),
  vector_ (rhs.vector_)
{
  madara_logger_ptr_log (logger::global_logger.get (),
    logger::LOG_MAJOR,
    "NativeDoubleVector::copy constructor called on %s\n",
    rhs.name_.c_str ());
}


madara::knowledge::containers::NativeDoubleVector::~NativeDoubleVector ()
{
  madara_logger_ptr_log (logger::global_logger.get (),
    logger::LOG_MAJOR,
    "NativeDoubleVector::destructor called on %s\n",
    this->name_.c_str ());
}
  
void
madara::knowledge::containers::NativeDoubleVector::modify (void)
{
  madara_logger_ptr_log (logger::global_logger.get (),
    logger::LOG_MAJOR,
    "NativeDoubleVector::modify called\n");

  ContextGuard context_guard (*context_);
  if (context_ && name_ != "")
  {
    madara_logger_ptr_log (logger::global_logger.get (),
      logger::LOG_MAJOR,
      "NativeDoubleVector::modify: context is valid. Marking %s.\n",
      this->name_.c_str ());

    context_->mark_modified (vector_);
  }
}

std::string
madara::knowledge::containers::NativeDoubleVector::get_debug_info (
  void)
{
  std::stringstream result;

  result << "Native Double Vector: ";

  if (context_)
  {
    ContextGuard context_guard (*context_);
    MADARA_GUARD_TYPE guard (mutex_);

    result << this->name_;
    result << " [" << size () << "]";
    result << " = " << context_->get (vector_).to_string ();
  }

  return result.str ();
}

void
madara::knowledge::containers::NativeDoubleVector::modify_ (void)
{
  modify ();
}

std::string
madara::knowledge::containers::NativeDoubleVector::get_debug_info_ (
  void)
{
  return get_debug_info ();
}

madara::knowledge::containers::BaseContainer *
madara::knowledge::containers::NativeDoubleVector::clone (void) const
{
  madara_logger_ptr_log (logger::global_logger.get (),
    logger::LOG_MAJOR,
    "NativeDoubleVector::clone: cloning %s\n",
    this->name_.c_str ());

  return new NativeDoubleVector (*this);
}

void
madara::knowledge::containers::NativeDoubleVector::operator= (
  const NativeDoubleVector & rhs)
{
  if (this != &rhs)
  {
    MADARA_GUARD_TYPE guard (mutex_), guard2 (rhs.mutex_);

    madara_logger_ptr_log (logger::global_logger.get (),
      logger::LOG_MAJOR,
      "NativeDoubleVector::assignment: %s: copying from %s.\n",
      this->name_.c_str (),
      rhs.name_.c_str ());

    this->context_ = rhs.context_;
    this->name_ = rhs.name_;
    this->settings_ = rhs.settings_;
    this->vector_ = rhs.vector_;
  }
}

void
madara::knowledge::containers::NativeDoubleVector::push_back (
  type value)
{
  if (context_ && name_ != "")
  {
    madara_logger_ptr_log (logger::global_logger.get (),
      logger::LOG_MAJOR,
      "NativeDoubleVector::push_back: %s: valid context, pushing.\n",
      this->name_.c_str ());

    ContextGuard context_guard (*context_);
    MADARA_GUARD_TYPE guard (mutex_);

    size_t i = size ();
    resize ((int)i + 1);
    set (i, value);
  }
}

void
madara::knowledge::containers::NativeDoubleVector::resize (
  size_t size)
{
  if (context_ && name_ != "")
  {
    ContextGuard context_guard (*context_);
    MADARA_GUARD_TYPE guard (mutex_);

    madara_logger_ptr_log (logger::global_logger.get (),
      logger::LOG_MAJOR,
      "NativeDoubleVector::resize: %s: resizing to %d\n",
      this->name_.c_str (),
      (int)size);

    knowledge::KnowledgeRecord value = context_->get (vector_, settings_);

    value.resize (size);

    context_->set (vector_, value, settings_);
  }
}

size_t
madara::knowledge::containers::NativeDoubleVector::size (void) const
{
  ContextGuard context_guard (*context_);
  MADARA_GUARD_TYPE guard (mutex_);

  return context_->get (vector_, settings_).size ();
}

void
madara::knowledge::containers::NativeDoubleVector::set_name (
  const std::string & var_name,
  KnowledgeBase & knowledge, int size)
{
  if (context_ != &(knowledge.get_context ()) || name_ != var_name)
  {
    context_ = &(knowledge.get_context ());

    ContextGuard context_guard (*context_);
    MADARA_GUARD_TYPE guard (mutex_);

    madara_logger_ptr_log (logger::global_logger.get (),
      logger::LOG_MAJOR,
      "NativeDoubleVector::set_name: setting name to %s\n",
      var_name.c_str ());

    name_ = var_name;

    vector_ = knowledge.get_ref (var_name, settings_);

    if (size > 0)
      resize (size_t (size));
  }
}

void
madara::knowledge::containers::NativeDoubleVector::set_name (
  const std::string & var_name,
  Variables & knowledge, int size)
{
  if (context_ != knowledge.get_context () || name_ != var_name)
  {
    context_ = knowledge.get_context ();

    ContextGuard context_guard (*context_);
    MADARA_GUARD_TYPE guard (mutex_);

    madara_logger_ptr_log (logger::global_logger.get (),
      logger::LOG_MAJOR,
      "NativeDoubleVector::set_name: setting name to %s\n",
      var_name.c_str ());

    name_ = var_name;
    
    vector_ = knowledge.get_ref (var_name, settings_);

    if (size > 0)
      resize (size_t (size));
  }
}

void
madara::knowledge::containers::NativeDoubleVector::set_name (
  const std::string & var_name,
  ThreadSafeContext & knowledge, int size)
{
  if (context_ != &knowledge || name_ != var_name)
  {
    context_ = &knowledge;

    ContextGuard context_guard (*context_);
    MADARA_GUARD_TYPE guard (mutex_);

    madara_logger_ptr_log (logger::global_logger.get (),
      logger::LOG_MAJOR,
      "NativeDoubleVector::set_name: setting name to %s\n",
      var_name.c_str ());

    name_ = var_name;
    
    vector_ = knowledge.get_ref (var_name, settings_);

    if (size > 0)
      resize (size_t (size));
  }
}

void
madara::knowledge::containers::NativeDoubleVector::exchange (
  NativeDoubleVector & other)
{
  if (context_ && other.context_)
  {
    ContextGuard context_guard (*context_);
    ContextGuard other_context_guard (*other.context_);
    MADARA_GUARD_TYPE guard (mutex_), guard2 (other.mutex_);

    madara_logger_ptr_log (logger::global_logger.get (),
      logger::LOG_MAJOR,
      "NativeDoubleVector::exchange: %s exchanging with %s\n",
      this->name_.c_str (),
      other.name_.c_str ());

    knowledge::KnowledgeRecord temp (context_->get (other.vector_));

    context_->set (other.vector_, context_->get (vector_), other.settings_);
    context_->set (vector_, temp, settings_);
  }
}

void
madara::knowledge::containers::NativeDoubleVector::transfer_to (
  NativeDoubleVector & other)
{
  if (context_ && other.context_)
  {
    ContextGuard context_guard (*context_);
    ContextGuard other_context_guard (*other.context_);
    MADARA_GUARD_TYPE guard (mutex_);
    MADARA_GUARD_TYPE guard2 (other.mutex_);

    madara_logger_ptr_log (logger::global_logger.get (),
      logger::LOG_MAJOR,
      "NativeDoubleVector::transfer_to: %s transfering to %s\n",
      this->name_.c_str (),
      other.name_.c_str ());

    size_t other_size = other.size ();
    size_t this_size = this->size ();

    if (this_size > 0)
    {
      madara_logger_ptr_log (logger::global_logger.get (),
        logger::LOG_MINOR,
        "NativeDoubleVector::transfer_to: %s has elements\n",
        this->name_.c_str ());

      size_t size = other_size + this_size;
      other.resize ((int)size);

      knowledge::KnowledgeRecord rhs (other.context_->get (other.vector_));
      knowledge::KnowledgeRecord lhs (context_->get (vector_));

      rhs.set_index (size - 1, lhs.retrieve_index (this_size - 1).to_double ());

      for (size_t i = 0, j = other_size; i < this_size - 1; ++i, ++j)
      {
        rhs.set_index (other_size, lhs.retrieve_index (i).to_double ());
      }

      other.context_->set (other.vector_, rhs, other.settings_);

      this->resize (0);
    }
  }
}

void
madara::knowledge::containers::NativeDoubleVector::copy_to (
  KnowledgeVector & target) const
{
  if (context_)
  {
    ContextGuard context_guard (*context_);
    MADARA_GUARD_TYPE guard (mutex_);

    madara_logger_ptr_log (logger::global_logger.get (),
      logger::LOG_MAJOR,
      "NativeDoubleVector::copy_to: %s: copying elements to vector\n",
      this->name_.c_str ());

    target.resize (size ());

    for (size_t i = 0; i < target.size (); ++i)
    {
      target[i].deep_copy ((*this)[i]);
    }
  }
}

madara::knowledge::containers::NativeDoubleVector::type
madara::knowledge::containers::NativeDoubleVector::operator[] (
  size_t index) const
{
  knowledge::KnowledgeRecord result;

  if (context_)
  {
    ContextGuard context_guard (*context_);
    MADARA_GUARD_TYPE guard (mutex_);

    madara_logger_ptr_log (logger::global_logger.get (),
      logger::LOG_MAJOR,
      "NativeDoubleVector[]: %s: retrieving element from container\n",
      this->name_.c_str ());

    result = context_->get (vector_, settings_);

    if (index < result.size ())
      result = result.retrieve_index (index);
  }

  return result.to_double ();
}

int
madara::knowledge::containers::NativeDoubleVector::set (
  size_t index,
  type value)
{
  int result = -1;

  if (context_)
  {
    ContextGuard context_guard (*context_);
    MADARA_GUARD_TYPE guard (mutex_);

    madara_logger_ptr_log (logger::global_logger.get (),
      logger::LOG_MAJOR,
      "NativeDoubleVector::set: %s: setting element [%d] to %f\n",
      this->name_.c_str (),
      (int)index, value);

    result = context_->set_index (vector_, index, value, settings_);
  }
  
  return result;
}

int
madara::knowledge::containers::NativeDoubleVector::set (
  size_t index,
  type value, 
  const KnowledgeUpdateSettings & settings)
{
  int result = -1;

  if (context_)
  {
    ContextGuard context_guard (*context_);
    MADARA_GUARD_TYPE guard (mutex_);

    madara_logger_ptr_log (logger::global_logger.get (),
      logger::LOG_MAJOR,
      "NativeDoubleVector::set: %s: setting element [%d] "
      "to %f with custom settings\n",
      this->name_.c_str (),
      (int)index, value);

    result = context_->set_index (vector_, index, value, settings);
  }
  
  return result;
}

int
madara::knowledge::containers::NativeDoubleVector::set (
  const std::vector <type> & value)
{
  int result = -1;
  
  if (context_)
  {
    ContextGuard context_guard (*context_);
    MADARA_GUARD_TYPE guard (mutex_);

    madara_logger_ptr_log (logger::global_logger.get (),
      logger::LOG_MAJOR,
      "NativeDoubleVector::set: %s: setting all elements\n",
      this->name_.c_str ());

    context_->set (vector_, value, settings_);
  }
  
  return result;
}
 
int
madara::knowledge::containers::NativeDoubleVector::set (
  const std::vector <type> & value,
  const KnowledgeUpdateSettings & settings)
{
  int result = -1;
  
  if (context_)
  {
    ContextGuard context_guard (*context_);
    MADARA_GUARD_TYPE guard (mutex_);

    madara_logger_ptr_log (logger::global_logger.get (),
      logger::LOG_MAJOR,
      "NativeDoubleVector::set: %s: setting all elements "
      "with custom settings\n",
      this->name_.c_str ());

    context_->set (vector_, value, settings);
  }
  
  return result;
}     

void
madara::knowledge::containers::NativeDoubleVector::set_quality (
  size_t index,
  uint32_t quality,
  const KnowledgeReferenceSettings & settings)
{
  if (context_)
  {
    ContextGuard context_guard (*context_);
    MADARA_GUARD_TYPE guard (mutex_);

    madara_logger_ptr_log (logger::global_logger.get (),
      logger::LOG_MAJOR,
      "NativeDoubleVector::set: %s: setting quality of knowledge\n",
      this->name_.c_str ());

    context_->set_quality (name_, quality, true, settings);
  }
}

madara::knowledge::KnowledgeRecord
madara::knowledge::containers::NativeDoubleVector::to_record (
  size_t index) const
{
  madara_logger_ptr_log (logger::global_logger.get (),
    logger::LOG_MAJOR,
    "NativeDoubleVector::to_record: %s: retrieving record\n",
    this->name_.c_str ());

  madara::knowledge::KnowledgeRecord result = context_->get (this->vector_, settings_);

  result = result.retrieve_index (index);
  return result;
}

madara::knowledge::KnowledgeRecord
madara::knowledge::containers::NativeDoubleVector::to_record (
  void) const
{
  madara_logger_ptr_log (logger::global_logger.get (),
    logger::LOG_MAJOR,
    "NativeDoubleVector::to_record: %s: retrieving record\n",
    this->name_.c_str ());

  return context_->get (this->vector_, settings_);
}

bool
madara::knowledge::containers::NativeDoubleVector::is_true (void) const
{
  bool result (false);

  madara_logger_log (context_->get_logger (), logger::LOG_MAJOR,
    "NativeDoubleVector::is_true: %s: Checking for non-zero value\n",
    this->name_.c_str ());

  if (context_)
  {
    ContextGuard context_guard (*context_);
    MADARA_GUARD_TYPE guard (mutex_);
    result = context_->get (vector_, settings_).is_true ();
  }

  madara_logger_log (context_->get_logger (), logger::LOG_MAJOR,
    "NativeDoubleVector::is_true: %s: final result is %d\n",
    this->name_.c_str (),
    (int)result);

  return result;
}

bool
madara::knowledge::containers::NativeDoubleVector::is_false (void) const
{
  return !is_true ();
}


bool
madara::knowledge::containers::NativeDoubleVector::is_true_ (void) const
{
  return is_true ();
}

bool
madara::knowledge::containers::NativeDoubleVector::is_false_ (void) const
{
  return is_false ();
}
