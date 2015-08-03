#include "Vector.h"
#include "madara/knowledge_engine/Context_Guard.h"


Madara::Knowledge_Engine::Containers::Vector::Vector (
  const Knowledge_Update_Settings & settings,
  const std::string & delimiter)
: Base_Container ("", settings), context_ (0), delimiter_ (delimiter)
{
}

Madara::Knowledge_Engine::Containers::Vector::Vector (
  const std::string & name,
  Knowledge_Base & knowledge,
  int size,
  bool delete_vars,
  const Knowledge_Update_Settings & settings,
  const std::string & delimiter)
  : Base_Container (name, settings), context_ (&(knowledge.get_context ())),
  delimiter_ (delimiter)
{
  size_ = get_size_ref ();
  resize (size, delete_vars);
}
  
Madara::Knowledge_Engine::Containers::Vector::Vector (
  const std::string & name,
  Variables & knowledge,
  int size,
  bool delete_vars,
  const Knowledge_Update_Settings & settings,
  const std::string & delimiter)
  : Base_Container (name, settings), context_ (knowledge.get_context ()),
  delimiter_ (delimiter)
{
  size_ = get_size_ref ();
  resize (size, delete_vars);
}

Madara::Knowledge_Engine::Containers::Vector::Vector (const Vector & rhs)
  : Base_Container (rhs), context_ (rhs.context_),
  vector_ (rhs.vector_),
  delimiter_ (rhs.delimiter_),
  size_ (rhs.size_)
{

}


Madara::Knowledge_Engine::Containers::Vector::~Vector ()
{
}
  
void
Madara::Knowledge_Engine::Containers::Vector::modify (void)
{
  if (context_ && name_ != "")
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    for (size_t index = 0; index < vector_.size (); ++index)
      context_->mark_modified (vector_[index]);

    context_->mark_modified (size_);
  }
}

std::string
Madara::Knowledge_Engine::Containers::Vector::get_debug_info (void)
{
  std::stringstream result;

  result << "Vector: ";

  if (context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);
    size_t elements = vector_.size ();

    result << this->name_;
    result << " [" << elements << "]";
    result << " = [";

    if (elements > 0)
    {
      result << context_->get (vector_[0]).to_string ();

      for (size_t index = 1; index < elements; ++index)
      {
        result << ", " << context_->get (vector_[index]).to_string ();
      }
    }

    result << "]";
  }

  return result.str ();
}


void
Madara::Knowledge_Engine::Containers::Vector::modify_ (void)
{
  modify ();
}

std::string
Madara::Knowledge_Engine::Containers::Vector::get_debug_info_ (void)
{
  return get_debug_info ();
}

Madara::Knowledge_Engine::Containers::Base_Container *
Madara::Knowledge_Engine::Containers::Vector::clone (void) const
{
  return new Vector (*this);
}

void
Madara::Knowledge_Engine::Containers::Vector::modify (size_t index)
{
  if (context_ && name_ != "" && index < vector_.size ())
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    context_->mark_modified (vector_[index]);
  }
}

void
Madara::Knowledge_Engine::Containers::Vector::operator= (
  const Vector & rhs)
{
  if (this != &rhs)
  {
    Guard guard (mutex_), guard2 (rhs.mutex_);

    this->context_ = rhs.context_;
    this->name_ = rhs.name_;
    this->settings_ = rhs.settings_;
    this->size_ = rhs.size_;
    this->vector_ = rhs.vector_;
  }
}


Madara::Knowledge_Engine::Variable_Reference
Madara::Knowledge_Engine::Containers::Vector::get_size_ref (void)
{
  Variable_Reference ref;
  
  if (context_ && name_ != "")
  {
    Knowledge_Update_Settings keep_local (true);
    std::stringstream buffer;

    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    buffer << name_;
    buffer << delimiter_;
    buffer << "size";

    ref = context_->get_ref (buffer.str (), keep_local);
  }

  return ref;
}

void Madara::Knowledge_Engine::Containers::Vector::push_back (
  Knowledge_Record value)
{
  if (context_ && name_ != "")
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    if (!size_.is_valid ())
    {
      size_ = get_size_ref ();
    }

    size_t i = size ();

    resize ((int)i + 1);

    if (value.type () == Knowledge_Record::DOUBLE)
    {
      set (i, value.to_double ());
    }
    else if (value.type () == Knowledge_Record::STRING)
    {
      set (i, value.to_string ());
    }
    else if (value.type () == Knowledge_Record::INTEGER_ARRAY)
    {
      set (i, value.to_integers ());
    }
    else if (value.type () == Knowledge_Record::DOUBLE_ARRAY)
    {
      set (i, value.to_doubles ());
    }
    else if (value.is_binary_file_type ())
    {
      size_t size;
      unsigned char * unmanaged = value.to_unmanaged_buffer (size);
      set (i, (char *)unmanaged, size);
      delete [] unmanaged;
    }
    else
    {
      set (i, value.to_integer ());
    }
  }
}

void
Madara::Knowledge_Engine::Containers::Vector::resize (
  int size, bool delete_vars)
{
  if (context_ && name_ != "")
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    if (!size_.is_valid ())
    {
      size_ = get_size_ref ();
    }

    if (size >= 0)
    {
      size_t old_size = vector_.size ();

      if (old_size != size)
      {
        vector_.resize (size);
        
        context_->set (size_, Knowledge_Record::Integer (size), settings_);

        if ((size_t)size > old_size)
        {
          for (; old_size < (size_t)size; ++old_size)
          {
            std::stringstream buffer;
            buffer << name_;
            buffer << delimiter_;
            buffer << old_size;
            vector_[old_size] = context_->get_ref (buffer.str (), settings_);
          }
        }
        else if (delete_vars)
        {
          for (; (size_t)size < old_size; ++size)
          {
            std::stringstream buffer;
            buffer << name_;
            buffer << delimiter_;
            buffer << size;

            context_->delete_variable (buffer.str (), settings_);
          }
        }
      }
    }
    else
    {
      // dynamically allocate size from the context
      size_t cur_size =
        context_->get (size_, settings_).to_integer ();

      size_t old_size = vector_.size ();

      if (old_size != cur_size)
      {
        vector_.resize (cur_size);

        if (cur_size > old_size)
        {
          for (; old_size < cur_size; ++old_size)
          {
            std::stringstream buffer;
            buffer << name_;
            buffer << delimiter_;
            buffer << old_size;
            vector_[old_size] = context_->get_ref (buffer.str (), settings_);
          }
        }
        else if (delete_vars)
        {
          for (; cur_size < old_size; ++cur_size)
          {
            std::stringstream buffer;
            buffer << name_;
            buffer << delimiter_;
            buffer << cur_size;

            context_->delete_variable (buffer.str (), settings_);
          }
        }
      }
    }
  }
}

size_t
Madara::Knowledge_Engine::Containers::Vector::size (void) const
{
  Guard guard (mutex_);
  return vector_.size ();
}

void
Madara::Knowledge_Engine::Containers::Vector::set_name (
  const std::string & var_name,
  Knowledge_Base & knowledge, int size)
{
  if (context_ != &(knowledge.get_context ()) || name_ != var_name)
  {
    context_ = &(knowledge.get_context ());

    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    name_ = var_name;

    vector_.clear ();

    size_ = get_size_ref ();

    resize (size);
  }
}

void
Madara::Knowledge_Engine::Containers::Vector::set_name (
  const std::string & var_name,
  Variables & knowledge, int size)
{
  if (context_ != knowledge.get_context () || name_ != var_name)
  {
    context_ = knowledge.get_context ();

    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    name_ = var_name;

    vector_.clear ();
    resize (size);
  }
}

void
Madara::Knowledge_Engine::Containers::Vector::set_name (
  const std::string & var_name,
  Thread_Safe_Context & knowledge, int size)
{
  if (context_ != &knowledge || name_ != var_name)
  {
    context_ = &knowledge;

    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    name_ = var_name;

    vector_.clear ();
    resize (size);
  }
}

void
Madara::Knowledge_Engine::Containers::Vector::set_delimiter (
const std::string & delimiter)
{
  delimiter_ = delimiter;
  if (context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    vector_.clear ();
    resize (-1);
  }
}


std::string
Madara::Knowledge_Engine::Containers::Vector::get_delimiter (void)
{
  return delimiter_;
}

void
Madara::Knowledge_Engine::Containers::Vector::exchange (
  Vector & other, bool refresh_keys, bool delete_keys)
{
  if (context_ && other.context_)
  {
    Context_Guard context_guard (*context_);
    Context_Guard other_context_guard (*other.context_);
    Guard guard (mutex_), guard2 (other.mutex_);

    if (refresh_keys)
    {
      other.resize ();
      this->resize ();
    }

    size_t other_size = other.vector_.size ();
    size_t this_size = this->vector_.size ();

    for (size_t i = 0; i < this_size; ++i)
    {
      // temp = this[i];
      Knowledge_Record temp = context_->get (this->vector_[i], settings_);
    
      if (i < other_size)
      {
        // this[i] = other[i];
        context_->set (this->vector_[i],
          context_->get (other.vector_[i], other.settings_),
          settings_);

        // other[i] = temp;
        other.context_->set (other.vector_[i], temp, other.settings_);
      }
      else
      {
        if (delete_keys)
        {
          std::stringstream buffer;
          buffer << this->name_;
          buffer << delimiter_;
          buffer << i;
          this->context_->delete_variable (buffer.str (), other.settings_);
        }
        else
        {
          Knowledge_Record zero;
          this->context_->set (this->vector_[i], zero, this->settings_);
        }

        {
          std::stringstream buffer;
          buffer << other.name_;
          buffer << delimiter_;
          buffer << i;

          // other[i] = temp;
          other.context_->set (buffer.str (), temp, other.settings_);
        }
      }

    }

    // copy the other vector's elements to this vector's location
    for (size_t i = this_size; i < other_size; ++i)
    {
      std::stringstream buffer;
      buffer << this->name_;
      buffer << delimiter_;
      buffer << i;
      context_->set (buffer.str (),
        other.context_->get (other.vector_[i], other.settings_), this->settings_);
    }

    // set the size appropriately
    this->context_->set (this->size_,
      Knowledge_Record::Integer (other_size), this->settings_);
    other.context_->set (other.size_,
      Knowledge_Record::Integer (this_size), other.settings_);

    if (refresh_keys)
    {
      this->resize (-1, true);
      other.resize (-1, true);
    }
  }
}

void
Madara::Knowledge_Engine::Containers::Vector::transfer_to (Vector & other)
{
  if (context_ && other.context_)
  {
    Context_Guard context_guard (*context_);
    Context_Guard other_context_guard (*other.context_);
    Guard guard (mutex_);
    Guard guard2 (other.mutex_);

    size_t other_size = other.vector_.size ();
    size_t this_size = this->vector_.size ();

    size_t size = other_size + this_size;
    other.resize ((int)size);

    for (size_t i = 0, j = other_size; i < this_size; ++i, ++j)
    {
      other.context_->set (other.vector_[j], (*this)[i], other.settings_);
    }

    this->resize (0, true);
  }
}

void
Madara::Knowledge_Engine::Containers::Vector::copy_to (
  Knowledge_Vector & target) const
{
  if (context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    target.resize (vector_.size ());
    
    for (size_t i = 0; i < vector_.size (); ++i)
    {
      target[i].deep_copy ((*this)[i]);
    }
  }
}

Madara::Knowledge_Record
Madara::Knowledge_Engine::Containers::Vector::operator[] (
  size_t index) const
{
  Knowledge_Record result;
  Knowledge_Update_Settings keep_local (true);

  if (index < vector_.size () && context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);
    result = context_->get (vector_[index], keep_local);
  }

  return result;
}

Madara::Knowledge_Record
Madara::Knowledge_Engine::Containers::Vector::to_record (
  size_t index) const
{
  Knowledge_Record result;

  if (index < vector_.size () && context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);
    result = context_->get (vector_[index], settings_);
  }

  return result;
}

bool
Madara::Knowledge_Engine::Containers::Vector::exists (
  size_t index) const
{
  bool result (false);

  if (index < vector_.size () && context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);
    result = context_->exists (vector_[index]);
  }

  return result;
}

int
Madara::Knowledge_Engine::Containers::Vector::read_file (
  size_t index,
  const std::string & filename)
{
  int result = -1;

  if (index < vector_.size () && context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);
    result = context_->read_file (vector_[index], filename, settings_);
  }

  return result;
}

int
Madara::Knowledge_Engine::Containers::Vector::read_file (
  size_t index,
  const std::string & filename, 
  const Knowledge_Update_Settings & settings)
{
  int result = -1;

  if (index < vector_.size () && context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);
    result = context_->read_file (vector_[index], filename, settings);
  }

  return result;
}

int
Madara::Knowledge_Engine::Containers::Vector::set_file (
  size_t index,
  const unsigned char * value, size_t size)
{
  int result = -1;
  
  if (index < vector_.size () && context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);
    result = context_->set_file (vector_[index], value, size, settings_);
  }

  return result;
}
  
int
Madara::Knowledge_Engine::Containers::Vector::set_file (
  size_t index,
  const unsigned char * value, size_t size, 
  const Knowledge_Update_Settings & settings)
{
  int result = -1;
  
  if (index < vector_.size () && context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);
    result = context_->set_file (vector_[index], value, size, settings);
  }

  return result;
}

int
Madara::Knowledge_Engine::Containers::Vector::set_jpeg (
  size_t index,
  const unsigned char * value, size_t size)
{
  int result = -1;
  
  if (index < vector_.size () && context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);
    result = context_->set_jpeg (vector_[index], value, size, settings_);
  }
  
  return result;
}
  
int
Madara::Knowledge_Engine::Containers::Vector::set_jpeg (
  size_t index,
  const unsigned char * value, size_t size, 
  const Knowledge_Update_Settings & settings)
{
  int result = -1;
  
  if (index < vector_.size () && context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);
    result = context_->set_jpeg (vector_[index], value, size, settings);
  }
  return result;
}

int
Madara::Knowledge_Engine::Containers::Vector::set (
  size_t index,
  Knowledge_Record::Integer value)
{
  int result = -1;
  
  if (index < vector_.size () && context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);
    result = context_->set (vector_[index], value, settings_);
  }

  return result;
}

int
Madara::Knowledge_Engine::Containers::Vector::set (
  size_t index,
  Knowledge_Record::Integer value, 
  const Knowledge_Update_Settings & settings)
{
  int result = -1;
  
  if (index < vector_.size () && context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);
    result = context_->set (vector_[index], value, settings);
  }

  return result;
}

int
Madara::Knowledge_Engine::Containers::Vector::set_index (
  size_t index,
  size_t sub_index,
  Knowledge_Record::Integer value)
{
  int result = -1;
  
  if (index < vector_.size () && context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);
    result = context_->set_index (vector_[index], sub_index, value, settings_);
  }

  return result;
}

int
Madara::Knowledge_Engine::Containers::Vector::set_index (
  size_t index,
  size_t sub_index,
  Knowledge_Record::Integer value,
  const Knowledge_Update_Settings & settings)
{
  int result = -1;
  
  if (index < vector_.size () && context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);
    result = context_->set_index (vector_[index], sub_index, value, settings);
  }

  return result;
}

int
Madara::Knowledge_Engine::Containers::Vector::set (
  size_t index,
  const Madara::Knowledge_Record::Integer * value,
  uint32_t size)
{
  int result = -1;
  
  if (index < vector_.size () && context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);
    result = context_->set (vector_[index], value, size, settings_);
  }

  return result;
}

int
Madara::Knowledge_Engine::Containers::Vector::set (
  size_t index,
  const Madara::Knowledge_Record::Integer * value,
  uint32_t size,
  const Knowledge_Update_Settings & settings)
{
  int result = -1;
  
  if (index < vector_.size () && context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);
    result = context_->set (vector_[index], value, size, settings);
  }
  
  return result;
}

int
Madara::Knowledge_Engine::Containers::Vector::set (
  size_t index,
  const std::vector <Knowledge_Record::Integer> & value)
{
  int result = -1;
  
  if (index < vector_.size () && context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);
    result = context_->set (vector_[index], value, settings_);
  }

  return result;
}
 
int
Madara::Knowledge_Engine::Containers::Vector::set (
  size_t index,
  const std::vector <Knowledge_Record::Integer> & value,
  const Knowledge_Update_Settings & settings)
{
  int result = -1;
  
  if (index < vector_.size () && context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);
    result = context_->set (vector_[index], value, settings);
  }

  return result;
}

int
Madara::Knowledge_Engine::Containers::Vector::set (
  size_t index,
  double value)
{
  int result = -1;
  
  if (index < vector_.size () && context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);
    result = context_->set (vector_[index], value, settings_);
  }

  return result;
}

int
Madara::Knowledge_Engine::Containers::Vector::set (
  size_t index,
  double value, 
  const Knowledge_Update_Settings & settings)
{
  int result = -1;
  
  if (index < vector_.size () && context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);
    result = context_->set (vector_[index], value, settings);
  }

  return result;
}

int
Madara::Knowledge_Engine::Containers::Vector::set_index (
  size_t index,
  size_t sub_index,
  double value)
{
  int result = -1;
  
  if (index < vector_.size () && context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);
    result = context_->set_index (vector_[index], sub_index, value, settings_);
  }

  return result;
}

int
Madara::Knowledge_Engine::Containers::Vector::set_index (
  size_t index,
  size_t sub_index,
  double value,
  const Knowledge_Update_Settings & settings)
{
  int result = -1;
  
  if (index < vector_.size () && context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);
    result = context_->set_index (vector_[index], sub_index, value, settings);
  }

  return result;
}

int
Madara::Knowledge_Engine::Containers::Vector::set (
  size_t index,
  const double * value,
  uint32_t size)
{
  int result = -1;
  
  if (index < vector_.size () && context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);
    result = context_->set (vector_[index], value, size, settings_);
  }

  return result;
}

int
Madara::Knowledge_Engine::Containers::Vector::set (
  size_t index,
  const double * value,
  uint32_t size,
  const Knowledge_Update_Settings & settings)
{
  int result = -1;
  
  if (index < vector_.size () && context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);
    result = context_->set (vector_[index], value, size, settings);
  }

  return result;
}

int
Madara::Knowledge_Engine::Containers::Vector::set (
  size_t index,
  const std::vector <double> & value)
{
  int result = -1;
  
  if (index < vector_.size () && context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);
    result = context_->set (vector_[index], value, settings_);
  }

  return result;
}

int
Madara::Knowledge_Engine::Containers::Vector::set (
  size_t index,
  const std::vector <double> & value,
  const Knowledge_Update_Settings & settings)
{
  int result = -1;
  
  if (index < vector_.size () && context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);
    result = context_->set (vector_[index], value, settings);
  }

  return result;
}

int
Madara::Knowledge_Engine::Containers::Vector::set (
  size_t index,
  const std::string & value)
{
  int result = -1;
  
  if (index < vector_.size () && context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);
    result = context_->set (vector_[index], value, settings_);
  }

  return result;
}

int
Madara::Knowledge_Engine::Containers::Vector::set (
  size_t index,
  const std::string & value, 
  const Knowledge_Update_Settings & settings)
{
  int result = -1;
  
  if (index < vector_.size () && context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);
    result = context_->set (vector_[index], value, settings);
  }

  return result;
}

void
Madara::Knowledge_Engine::Containers::Vector::set_quality (
  size_t index,
  uint32_t quality,
  const Knowledge_Reference_Settings & settings)
{
  if (index < vector_.size () && context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);
    context_->set_quality (vector_[index].get_name (), quality,
      true, settings);
  }
}


bool
Madara::Knowledge_Engine::Containers::Vector::is_true (void) const
{
  bool result (false);

  madara_logger_log (context_->get_logger (), Logger::LOG_MAJOR,
    "Vector::is_true: Checking for truth\n");

  if (context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    result = true;

    madara_logger_log (context_->get_logger (), Logger::LOG_MINOR,
      "Vector::is_true: context was not null. Result changed to %d\n",
      (int)result);

    for (size_t index = 0; index < vector_.size (); ++index)
    {

      madara_logger_log (context_->get_logger (), Logger::LOG_DETAILED,
        "Vector::is_true: checking index %d, is_false of %d. \n",
        (int)result, (int)context_->get (vector_[index]).is_false ());

      if (context_->get (vector_[index]).is_false ())
      {
        madara_logger_log (context_->get_logger (), Logger::LOG_MAJOR,
          "Vector::is_true: result is false, breaking\n");

        result = false;
        break;
      }
    }

    if (vector_.size () == 0)
      result = false;
  }

  madara_logger_log (context_->get_logger (), Logger::LOG_MAJOR,
    "Vector::is_true: final result is %d\n", (int)result);

  return result;
}

bool
Madara::Knowledge_Engine::Containers::Vector::is_false (void) const
{
  return !is_true ();
}


bool
Madara::Knowledge_Engine::Containers::Vector::is_true_ (void) const
{
  return is_true ();
}

bool
Madara::Knowledge_Engine::Containers::Vector::is_false_ (void) const
{
  return is_false ();
}
