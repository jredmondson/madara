#include "Native_Double_Vector.h"
#include "madara/knowledge/Context_Guard.h"
#include "madara/logger/Global_Logger.h"

madara::knowledge::containers::Native_Double_Vector::Native_Double_Vector (
  const Knowledge_Update_Settings & settings)
  : Base_Container ("", settings), context_ (0)
{
  madara_logger_ptr_log (logger::global_logger.get (),
    logger::LOG_MAJOR, "Native_Double_Vector::constructor: new object\n");
}
   
madara::knowledge::containers::Native_Double_Vector::Native_Double_Vector (
  const std::string & name,
  Knowledge_Base & knowledge,
  int size,
  const Knowledge_Update_Settings & settings)
  : Base_Container (name, settings), context_ (&(knowledge.get_context ()))
{
  madara_logger_ptr_log (logger::global_logger.get (),
    logger::LOG_MAJOR,
    "Native_Double_Vector::constructor called for %s[%d]\n",
    name.c_str (), size);

  vector_ = knowledge.get_ref (name, settings_);
  if (size >= 0)
  {
    resize (size);
  }
}
  
madara::knowledge::containers::Native_Double_Vector::Native_Double_Vector (
  const std::string & name,
  Variables & knowledge,
  int size,
  const Knowledge_Update_Settings & settings)
: context_ (knowledge.get_context ())
{
  madara_logger_ptr_log (logger::global_logger.get (),
    logger::LOG_MAJOR,
    "Native_Double_Vector::constructor called for %s[%d]\n",
    name.c_str (), size);

  vector_ = knowledge.get_ref (name, settings);
  if (size >= 0)
  {
    resize (size);
  }
}

madara::knowledge::containers::Native_Double_Vector::Native_Double_Vector (
  const Native_Double_Vector & rhs)
: Base_Container (rhs), context_ (rhs.context_),
  vector_ (rhs.vector_)
{
  madara_logger_ptr_log (logger::global_logger.get (),
    logger::LOG_MAJOR,
    "Native_Double_Vector::copy constructor called on %s\n",
    rhs.name_.c_str ());
}


madara::knowledge::containers::Native_Double_Vector::~Native_Double_Vector ()
{
  madara_logger_ptr_log (logger::global_logger.get (),
    logger::LOG_MAJOR,
    "Native_Double_Vector::destructor called on %s\n",
    this->name_.c_str ());
}
  
void
madara::knowledge::containers::Native_Double_Vector::modify (void)
{
  madara_logger_ptr_log (logger::global_logger.get (),
    logger::LOG_MAJOR,
    "Native_Double_Vector::modify called\n");

  Context_Guard context_guard (*context_);
  if (context_ && name_ != "")
  {
    madara_logger_ptr_log (logger::global_logger.get (),
      logger::LOG_MAJOR,
      "Native_Double_Vector::modify: context is valid. Marking %s.\n",
      this->name_.c_str ());

    context_->mark_modified (vector_);
  }
}

std::string
madara::knowledge::containers::Native_Double_Vector::get_debug_info (
  void)
{
  std::stringstream result;

  result << "Native Double Vector: ";

  if (context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    result << this->name_;
    result << " [" << size () << "]";
    result << " = " << context_->get (vector_).to_string ();
  }

  return result.str ();
}

void
madara::knowledge::containers::Native_Double_Vector::modify_ (void)
{
  modify ();
}

std::string
madara::knowledge::containers::Native_Double_Vector::get_debug_info_ (
  void)
{
  return get_debug_info ();
}

madara::knowledge::containers::Base_Container *
madara::knowledge::containers::Native_Double_Vector::clone (void) const
{
  madara_logger_ptr_log (logger::global_logger.get (),
    logger::LOG_MAJOR,
    "Native_Double_Vector::clone: cloning %s\n",
    this->name_.c_str ());

  return new Native_Double_Vector (*this);
}

void
madara::knowledge::containers::Native_Double_Vector::operator= (
  const Native_Double_Vector & rhs)
{
  if (this != &rhs)
  {
    Guard guard (mutex_), guard2 (rhs.mutex_);

    madara_logger_ptr_log (logger::global_logger.get (),
      logger::LOG_MAJOR,
      "Native_Double_Vector::assignment: %s: copying from %s.\n",
      this->name_.c_str (),
      rhs.name_.c_str ());

    this->context_ = rhs.context_;
    this->name_ = rhs.name_;
    this->settings_ = rhs.settings_;
    this->vector_ = rhs.vector_;
  }
}

void
madara::knowledge::containers::Native_Double_Vector::push_back (
  type value)
{
  if (context_ && name_ != "")
  {
    madara_logger_ptr_log (logger::global_logger.get (),
      logger::LOG_MAJOR,
      "Native_Double_Vector::push_back: %s: valid context, pushing.\n",
      this->name_.c_str ());

    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    size_t i = size ();
    resize ((int)i + 1);
    set (i, value);
  }
}

void
madara::knowledge::containers::Native_Double_Vector::resize (
  size_t size)
{
  if (context_ && name_ != "")
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    madara_logger_ptr_log (logger::global_logger.get (),
      logger::LOG_MAJOR,
      "Native_Double_Vector::resize: %s: resizing to %d\n",
      this->name_.c_str (),
      (int)size);

    Knowledge_Record value = context_->get (vector_, settings_);

    value.resize (size);

    context_->set (vector_, value, settings_);
  }
}

size_t
madara::knowledge::containers::Native_Double_Vector::size (void) const
{
  Context_Guard context_guard (*context_);
  Guard guard (mutex_);

  return context_->get (vector_, settings_).size ();
}

void
madara::knowledge::containers::Native_Double_Vector::set_name (
  const std::string & var_name,
  Knowledge_Base & knowledge, int size)
{
  if (context_ != &(knowledge.get_context ()) || name_ != var_name)
  {
    context_ = &(knowledge.get_context ());

    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    madara_logger_ptr_log (logger::global_logger.get (),
      logger::LOG_MAJOR,
      "Native_Double_Vector::set_name: setting name to %s\n",
      var_name.c_str ());

    name_ = var_name;

    vector_ = knowledge.get_ref (var_name, settings_);

    if (size > 0)
      resize (size_t (size));
  }
}

void
madara::knowledge::containers::Native_Double_Vector::set_name (
  const std::string & var_name,
  Variables & knowledge, int size)
{
  if (context_ != knowledge.get_context () || name_ != var_name)
  {
    context_ = knowledge.get_context ();

    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    madara_logger_ptr_log (logger::global_logger.get (),
      logger::LOG_MAJOR,
      "Native_Double_Vector::set_name: setting name to %s\n",
      var_name.c_str ());

    name_ = var_name;
    
    vector_ = knowledge.get_ref (var_name, settings_);

    if (size > 0)
      resize (size_t (size));
  }
}

void
madara::knowledge::containers::Native_Double_Vector::set_name (
  const std::string & var_name,
  Thread_Safe_Context & knowledge, int size)
{
  if (context_ != &knowledge || name_ != var_name)
  {
    context_ = &knowledge;

    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    madara_logger_ptr_log (logger::global_logger.get (),
      logger::LOG_MAJOR,
      "Native_Double_Vector::set_name: setting name to %s\n",
      var_name.c_str ());

    name_ = var_name;
    
    vector_ = knowledge.get_ref (var_name, settings_);

    if (size > 0)
      resize (size_t (size));
  }
}

void
madara::knowledge::containers::Native_Double_Vector::exchange (
  Native_Double_Vector & other)
{
  if (context_ && other.context_)
  {
    Context_Guard context_guard (*context_);
    Context_Guard other_context_guard (*other.context_);
    Guard guard (mutex_), guard2 (other.mutex_);

    madara_logger_ptr_log (logger::global_logger.get (),
      logger::LOG_MAJOR,
      "Native_Double_Vector::exchange: %s exchanging with %s\n",
      this->name_.c_str (),
      other.name_.c_str ());

    Knowledge_Record temp (context_->get (other.vector_));

    context_->set (other.vector_, context_->get (vector_), other.settings_);
    context_->set (vector_, temp, settings_);
  }
}

void
madara::knowledge::containers::Native_Double_Vector::transfer_to (
  Native_Double_Vector & other)
{
  if (context_ && other.context_)
  {
    Context_Guard context_guard (*context_);
    Context_Guard other_context_guard (*other.context_);
    Guard guard (mutex_);
    Guard guard2 (other.mutex_);

    madara_logger_ptr_log (logger::global_logger.get (),
      logger::LOG_MAJOR,
      "Native_Double_Vector::transfer_to: %s transfering to %s\n",
      this->name_.c_str (),
      other.name_.c_str ());

    size_t other_size = other.size ();
    size_t this_size = this->size ();

    if (this_size > 0)
    {
      madara_logger_ptr_log (logger::global_logger.get (),
        logger::LOG_MINOR,
        "Native_Double_Vector::transfer_to: %s has elements\n",
        this->name_.c_str ());

      size_t size = other_size + this_size;
      other.resize ((int)size);

      Knowledge_Record rhs (other.context_->get (other.vector_));
      Knowledge_Record lhs (context_->get (vector_));

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
madara::knowledge::containers::Native_Double_Vector::copy_to (
  Knowledge_Vector & target) const
{
  if (context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    madara_logger_ptr_log (logger::global_logger.get (),
      logger::LOG_MAJOR,
      "Native_Double_Vector::copy_to: %s: copying elements to vector\n",
      this->name_.c_str ());

    target.resize (size ());

    for (size_t i = 0; i < target.size (); ++i)
    {
      target[i].deep_copy ((*this)[i]);
    }
  }
}

madara::knowledge::containers::Native_Double_Vector::type
madara::knowledge::containers::Native_Double_Vector::operator[] (
  size_t index) const
{
  Knowledge_Record result;

  if (context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    madara_logger_ptr_log (logger::global_logger.get (),
      logger::LOG_MAJOR,
      "Native_Double_Vector[]: %s: retrieving element from container\n",
      this->name_.c_str ());

    result = context_->get (vector_, settings_);

    if (index < result.size ())
      result = result.retrieve_index (index);
  }

  return result.to_double ();
}

int
madara::knowledge::containers::Native_Double_Vector::set (
  size_t index,
  type value)
{
  int result = -1;

  if (context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    madara_logger_ptr_log (logger::global_logger.get (),
      logger::LOG_MAJOR,
      "Native_Double_Vector::set: %s: setting element [%d] to %f\n",
      this->name_.c_str (),
      (int)index, value);

    result = context_->set_index (vector_, index, value, settings_);
  }
  
  return result;
}

int
madara::knowledge::containers::Native_Double_Vector::set (
  size_t index,
  type value, 
  const Knowledge_Update_Settings & settings)
{
  int result = -1;

  if (context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    madara_logger_ptr_log (logger::global_logger.get (),
      logger::LOG_MAJOR,
      "Native_Double_Vector::set: %s: setting element [%d] "
      "to %f with custom settings\n",
      this->name_.c_str (),
      (int)index, value);

    result = context_->set_index (vector_, index, value, settings);
  }
  
  return result;
}

int
madara::knowledge::containers::Native_Double_Vector::set (
  const std::vector <type> & value)
{
  int result = -1;
  
  if (context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    madara_logger_ptr_log (logger::global_logger.get (),
      logger::LOG_MAJOR,
      "Native_Double_Vector::set: %s: setting all elements\n",
      this->name_.c_str ());

    context_->set (vector_, value, settings_);
  }
  
  return result;
}
 
int
madara::knowledge::containers::Native_Double_Vector::set (
  const std::vector <type> & value,
  const Knowledge_Update_Settings & settings)
{
  int result = -1;
  
  if (context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    madara_logger_ptr_log (logger::global_logger.get (),
      logger::LOG_MAJOR,
      "Native_Double_Vector::set: %s: setting all elements "
      "with custom settings\n",
      this->name_.c_str ());

    context_->set (vector_, value, settings);
  }
  
  return result;
}     

void
madara::knowledge::containers::Native_Double_Vector::set_quality (
  size_t index,
  uint32_t quality,
  const Knowledge_Reference_Settings & settings)
{
  if (context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    madara_logger_ptr_log (logger::global_logger.get (),
      logger::LOG_MAJOR,
      "Native_Double_Vector::set: %s: setting quality of knowledge\n",
      this->name_.c_str ());

    context_->set_quality (name_, quality, true, settings);
  }
}

madara::Knowledge_Record
madara::knowledge::containers::Native_Double_Vector::to_record (
  size_t index) const
{
  madara_logger_ptr_log (logger::global_logger.get (),
    logger::LOG_MAJOR,
    "Native_Double_Vector::to_record: %s: retrieving record\n",
    this->name_.c_str ());

  madara::Knowledge_Record result = context_->get (this->vector_, settings_);

  result = result.retrieve_index (index);
  return result;
}

madara::Knowledge_Record
madara::knowledge::containers::Native_Double_Vector::to_record (
  void) const
{
  madara_logger_ptr_log (logger::global_logger.get (),
    logger::LOG_MAJOR,
    "Native_Double_Vector::to_record: %s: retrieving record\n",
    this->name_.c_str ());

  return context_->get (this->vector_, settings_);
}

bool
madara::knowledge::containers::Native_Double_Vector::is_true (void) const
{
  bool result (false);

  madara_logger_log (context_->get_logger (), logger::LOG_MAJOR,
    "Native_Double_Vector::is_true: %s: Checking for non-zero value\n",
    this->name_.c_str ());

  if (context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);
    result = context_->get (vector_, settings_).is_true ();
  }

  madara_logger_log (context_->get_logger (), logger::LOG_MAJOR,
    "Native_Double_Vector::is_true: %s: final result is %d\n",
    this->name_.c_str (),
    (int)result);

  return result;
}

bool
madara::knowledge::containers::Native_Double_Vector::is_false (void) const
{
  return !is_true ();
}


bool
madara::knowledge::containers::Native_Double_Vector::is_true_ (void) const
{
  return is_true ();
}

bool
madara::knowledge::containers::Native_Double_Vector::is_false_ (void) const
{
  return is_false ();
}
