#include "Typed_Vector.h"
#include "madara/knowledge_engine/Context_Guard.h"


template <class T>
Madara::Knowledge_Engine::Containers::Typed_Vector<T>::Typed_Vector (
  const Knowledge_Update_Settings & settings,
  const std::string & delimiter)
: context_ (0), settings_ (settings), delimiter_ (delimiter)
{
}

template <class T>
Madara::Knowledge_Engine::Containers::Typed_Vector<T>::Typed_Vector (
  const std::string & name,
  Knowledge_Base & knowledge,
  int size,
  bool delete_vars,
  const Knowledge_Update_Settings & settings,
  const std::string & delimiter)
: context_ (&(knowledge.get_context ())), name_ (name), settings_ (true),
  delimiter_ (delimiter)
{
  size_ = get_size_ref ();
  resize (size, delete_vars);
  settings_ = settings;
}

template <class T>
Madara::Knowledge_Engine::Containers::Typed_Vector<T>::Typed_Vector (
  const std::string & name,
  Variables & knowledge,
  int size,
  bool delete_vars,
  const Knowledge_Update_Settings & settings,
  const std::string & delimiter)
: context_ (knowledge.get_context ()), name_ (name), settings_ (true),
  delimiter_ (delimiter)
{
  size_ = get_size_ref ();
  resize (size, delete_vars);
  settings_ = settings;
}

template <class T>
Madara::Knowledge_Engine::Containers::Typed_Vector<T>::Typed_Vector (const Typed_Vector<T> & rhs)
: context_ (rhs.context_),
  name_ (rhs.name_),
  vector_ (rhs.vector_),
  settings_ (rhs.settings_),
  delimiter_ (rhs.delimiter_)
{

}


template <class T>
Madara::Knowledge_Engine::Containers::Typed_Vector<T>::~Typed_Vector ()
{
}

template <class T>
Madara::Knowledge_Engine::Variable_Reference
Madara::Knowledge_Engine::Containers::Typed_Vector<T>::get_size_ref (void)
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

template <class T>
void
Madara::Knowledge_Engine::Containers::Typed_Vector<T>::modify (void)
{
  if (context_ && name_ != "")
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    for (size_t index = 0; index < vector_.size (); ++index)
      vector_[index].modify ();
  }
}

template <class T>
void
Madara::Knowledge_Engine::Containers::Typed_Vector<T>::modify (size_t index)
{
  if (context_ && name_ != "" && index < vector_.size ())
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    vector_[index].modify ();
  }
}

template <class T>
void
Madara::Knowledge_Engine::Containers::Typed_Vector<T>::operator= (
  const Typed_Vector<T> & rhs)
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


template <class T>
Madara::Knowledge_Engine::Variable_Reference
Madara::Knowledge_Engine::Containers::Typed_Vector<T>::get_size_ref (void)
{
  Variable_Reference ref;
  
  if (context_ && name_ != "")
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    Knowledge_Update_Settings keep_local (true);
    std::stringstream buffer;
    buffer << name_;
    buffer << delimiter_;
    buffer << "size";

    ref = context_->get_ref (buffer.str (), keep_local);
  }

  return ref;
}

template <class T>
void Madara::Knowledge_Engine::Containers::Typed_Vector<T>::push_back (
  const T & value)
{
  if (context_ && name_ != "")
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    size_t i = size ();

    resize ((int)i + 1);

    std::stringstream buffer;
    buffer << name_;
    buffer << delimiter_;
    buffer << i;

    Knowledge_Base knowledge;
    knowledge.facade_for (*context_);

    value.set_name (buffer.str (), knowledge);
    vector_[i] = value;
  }
}

template <class T>
void
Madara::Knowledge_Engine::Containers::Typed_Vector<T>::resize (
  int size, bool delete_vars)
{
  if (context_ && name_ != "")
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    Knowledge_Base knowledge;
    knowledge.facade_for (*context_);

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

            vector_[old_size].set_name (buffer.str (), knowledge);
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

template <class T>
size_t
Madara::Knowledge_Engine::Containers::Typed_Vector<T>::size (void) const
{
  Guard guard (mutex_);
  return vector_.size ();
}

template <class T>
std::string
Madara::Knowledge_Engine::Containers::Typed_Vector<T>::get_name (void) const
{
  Guard guard (mutex_);
  return name_;
}

template <class T>
void
Madara::Knowledge_Engine::Containers::Typed_Vector<T>::set_name (
  const std::string & var_name,
  Knowledge_Base & knowledge, int size)
{
  context_ = &(knowledge.get_context ());
  if (context_ != 0 || name_ != var_name)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    name_ = var_name;

    vector_.clear ();

    size_ = get_size_ref ();

    resize (size);
  }
}

template <class T>
void
Madara::Knowledge_Engine::Containers::Typed_Vector<T>::set_name (
  const std::string & var_name,
  Variables & knowledge, int size)
{
  context_ = &(knowledge.get_context ());
  if (context_ != 0 || name_ != var_name)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    name_ = var_name;

    vector_.clear ();
    resize (size);
  }
}

template <class T>
void
Madara::Knowledge_Engine::Containers::Typed_Vector<T>::set_name (
  const std::string & var_name,
  Thread_Safe_Context & knowledge, int size)
{
  context_ = &knowledge);
  if (context_ != 0 || name_ != var_name)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    name_ = var_name;

    vector_.clear ();
    resize (size);
  }
}

template <class T>
void
Madara::Knowledge_Engine::Containers::Typed_Vector<T>::set_delimiter (
const std::string & delimiter)
{
  delimiter_ = delimiter;
  if (context_)
  {
    Context_Guard context_guard (*context_);

    vector_.clear ();
    resize (-1);
  }
}


template <class T>
std::string
Madara::Knowledge_Engine::Containers::Typed_Vector<T>::get_delimiter (void)
{
  Guard guard (mutex_);

  return delimiter_;
}

template <class T>
T
Madara::Knowledge_Engine::Containers::Typed_Vector<T>::operator[] (
  size_t index) const
{
  T result;
  Knowledge_Update_Settings keep_local (true);

  if (index < vector_.size () && context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);
    result = vector_[index];
  }

  return result;
}

template <class T>
T &
Madara::Knowledge_Engine::Containers::Typed_Vector<T>::operator[] (
size_t index) const
{
  if (context_ && index < size ())
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    return vector_[index];
  }
  else
  {
    return undefined_;
  }
}

template <class T>
Madara::Knowledge_Engine::Knowledge_Update_Settings
Madara::Knowledge_Engine::Containers::Typed_Vector<T>::set_settings (
  const Knowledge_Update_Settings & settings)
{
  Guard guard (mutex_);
  
  Knowledge_Update_Settings old_settings = settings_;

  settings_ = settings;

  return old_settings;
}

template <class T>
void
Madara::Knowledge_Engine::Containers::Typed_Vector<T>::set_quality (
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
