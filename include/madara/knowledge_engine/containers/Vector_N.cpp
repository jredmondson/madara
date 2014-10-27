#include "Vector_N.h"
#include "madara/knowledge_engine/Context_Guard.h"


Madara::Knowledge_Engine::Containers::Vector_N::Vector_N (
  const Knowledge_Update_Settings & settings)
: context_ (0), settings_ (settings) 
{
}
 
Madara::Knowledge_Engine::Containers::Vector_N::Vector_N (
  const std::string & name,
  Knowledge_Base & knowledge,
  const Knowledge_Update_Settings & settings)
: context_ (&(knowledge.get_context ())), name_ (name), settings_ (settings) 
{
}
  
Madara::Knowledge_Engine::Containers::Vector_N::Vector_N (
  const std::string & name,
  Variables & knowledge,
  const Knowledge_Update_Settings & settings)
: context_ (knowledge.get_context ()), name_ (name), settings_ (settings) 
{
}
      

Madara::Knowledge_Engine::Containers::Vector_N::Vector_N (const Vector_N & rhs)
  : context_ (rhs.context_),
    name_ (rhs.name_),
    settings_ (rhs.settings_)
{

}


Madara::Knowledge_Engine::Containers::Vector_N::~Vector_N ()
{

}


void
Madara::Knowledge_Engine::Containers::Vector_N::operator= (
  const Vector_N & rhs)
{
  if (this != &rhs)
  {
    Guard guard (mutex_), guard2 (rhs.mutex_);

    this->context_ = rhs.context_;
    this->name_ = rhs.name_;
    this->settings_ = rhs.settings_;
  }
}

std::string
Madara::Knowledge_Engine::Containers::Vector_N::get_name (void) const
{
  Guard guard (mutex_);
  return name_;
}

void
Madara::Knowledge_Engine::Containers::Vector_N::set_name (
  const std::string & var_name,
  Knowledge_Base & knowledge)
{
  Guard guard (mutex_);
  context_ = &(knowledge.get_context ());
  name_ = var_name;
}

void
Madara::Knowledge_Engine::Containers::Vector_N::set_name (
  const std::string & var_name,
  Variables & knowledge)
{
  Guard guard (mutex_);
  context_ = knowledge.get_context ();
  name_ = var_name;
}

Madara::Knowledge_Record
Madara::Knowledge_Engine::Containers::Vector_N::operator[] (const Index & index) const
{
  Knowledge_Record result;
  Knowledge_Update_Settings keep_local (true);

  if (index.size () > 0 && context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);
    std::stringstream buffer;
    buffer << name_;
    
    for (unsigned int i = 0; i < index.size (); ++i)
    {
      buffer << ".";
      buffer << index[i];
    }

    result = context_->get (buffer.str (), keep_local);
  }

  return result;
}
  
Madara::Knowledge_Record
Madara::Knowledge_Engine::Containers::Vector_N::operator () (
  unsigned int x) const
{
  Knowledge_Record result;

  if (context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    std::stringstream buffer;
    buffer << name_;
    
    buffer << ".";
    buffer << x;

    result = context_->get (buffer.str (), settings_);
  }

  return result;
}

Madara::Knowledge_Record
Madara::Knowledge_Engine::Containers::Vector_N::operator () (
  unsigned int x, unsigned int y) const
{
  Knowledge_Record result;
  Knowledge_Update_Settings keep_local (true);

  if (context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);
    std::stringstream buffer;
    buffer << name_;
    
    buffer << ".";
    buffer << x;
    buffer << ".";
    buffer << y;

    result = context_->get (buffer.str (), keep_local);
  }

  return result;
}
  
Madara::Knowledge_Record
Madara::Knowledge_Engine::Containers::Vector_N::operator () (
  unsigned int x, unsigned int y, unsigned int z) const
{
  Knowledge_Record result;
  Knowledge_Update_Settings keep_local (true);

  if (context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    std::stringstream buffer;
    buffer << name_;
    
    buffer << ".";
    buffer << x;
    buffer << ".";
    buffer << y;
    buffer << ".";
    buffer << z;

    result = context_->get (buffer.str (), keep_local);
  }

  return result;
}
      

Madara::Knowledge_Record
Madara::Knowledge_Engine::Containers::Vector_N::operator () (
  unsigned int i1, unsigned int i2, unsigned int i3, unsigned int i4) const
{
  Knowledge_Record result;
  Knowledge_Update_Settings keep_local (true);

  if (context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    std::stringstream buffer;
    buffer << name_;
    
    buffer << ".";
    buffer << i1;
    buffer << ".";
    buffer << i2;
    buffer << ".";
    buffer << i3;
    buffer << ".";
    buffer << i4;

    result = context_->get (buffer.str (), keep_local);
  }

  return result;
}

int
Madara::Knowledge_Engine::Containers::Vector_N::read_file (
  const Index & index, 
  const std::string & filename)
{
  int result = -1;
  
  if (index.size () > 0 && context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    std::stringstream buffer;
    buffer << name_;
    
    for (unsigned int i = 0; i < index.size (); ++i)
    {
      buffer << ".";
      buffer << index[i];
    }

    result = context_->read_file (buffer.str (), filename, settings_);
  }

  return result;
}
  
int
Madara::Knowledge_Engine::Containers::Vector_N::read_file (
  const Index & index, 
  const std::string & filename, 
  const Knowledge_Update_Settings & settings)
{
  int result = -1;
  
  if (index.size () > 0 && context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    std::stringstream buffer;
    buffer << name_;
    
    for (unsigned int i = 0; i < index.size (); ++i)
    {
      buffer << ".";
      buffer << index[i];
    }

    result = context_->read_file (buffer.str (), filename, settings);
  }

  return result;
}

int
Madara::Knowledge_Engine::Containers::Vector_N::set_file (
  const Index & index,
  const unsigned char * value, size_t size)
{
  int result = -1;
  
  if (index.size () > 0 && context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    std::stringstream buffer;
    buffer << name_;
    
    for (unsigned int i = 0; i < index.size (); ++i)
    {
      buffer << ".";
      buffer << index[i];
    }

    result = context_->set_file (buffer.str (), value, size, settings_);
  }
  
  return result;
}

int
Madara::Knowledge_Engine::Containers::Vector_N::set_file (
  const Index & index,
  const unsigned char * value, size_t size, 
  const Knowledge_Update_Settings & settings)
{
  int result = -1;
  
  if (index.size () > 0 && context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    std::stringstream buffer;
    buffer << name_;
    
    for (unsigned int i = 0; i < index.size (); ++i)
    {
      buffer << ".";
      buffer << index[i];
    }

    result = context_->set_file (buffer.str (), value, size, settings);
  }
  
  return result;
}

int
Madara::Knowledge_Engine::Containers::Vector_N::set_jpeg (
  const Index & index,
  const unsigned char * value, size_t size)
{
  int result = -1;
  
  if (index.size () > 0 && context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    std::stringstream buffer;
    buffer << name_;
    
    for (unsigned int i = 0; i < index.size (); ++i)
    {
      buffer << ".";
      buffer << index[i];
    }

    result = context_->set_jpeg (buffer.str (), value, size, settings_);
  }
  
  return result;
}
  
int
Madara::Knowledge_Engine::Containers::Vector_N::set_jpeg (
  const Index & index,
  const unsigned char * value, size_t size, 
  const Knowledge_Update_Settings & settings)
{
  int result = -1;
  
  if (index.size () > 0 && context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    std::stringstream buffer;
    buffer << name_;
    
    for (unsigned int i = 0; i < index.size (); ++i)
    {
      buffer << ".";
      buffer << index[i];
    }

    result = context_->set_jpeg (buffer.str (), value, size, settings);
  }
  
  return result;
}

int
Madara::Knowledge_Engine::Containers::Vector_N::set (
  const Index & index,
  Knowledge_Record::Integer value)
{
  int result = -1;
  
  if (index.size () > 0 && context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    std::stringstream buffer;
    buffer << name_;
    
    for (unsigned int i = 0; i < index.size (); ++i)
    {
      buffer << ".";
      buffer << index[i];
    }

    result = context_->set (buffer.str (), value, settings_);
  }
  
  return result;
}

int
Madara::Knowledge_Engine::Containers::Vector_N::set (
  const Index & index,
  Knowledge_Record::Integer value, 
  const Knowledge_Update_Settings & settings)
{
  int result = -1;
  
  if (index.size () > 0 && context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    std::stringstream buffer;
    buffer << name_;
    
    for (unsigned int i = 0; i < index.size (); ++i)
    {
      buffer << ".";
      buffer << index[i];
    }

    result = context_->set (buffer.str (), value, settings);
  }
  
  return result;
}

int
Madara::Knowledge_Engine::Containers::Vector_N::set_index (
  const Index & index,
  size_t sub_index,
  Knowledge_Record::Integer value)
{
  int result = -1;
  
  if (index.size () > 0 && context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    std::stringstream buffer;
    buffer << name_;
    
    for (unsigned int i = 0; i < index.size (); ++i)
    {
      buffer << ".";
      buffer << index[i];
    }

    result = context_->set_index (buffer.str (), sub_index, value, settings_);
  }
  
  return result;
}

int
Madara::Knowledge_Engine::Containers::Vector_N::set_index (
  const Index & index,
  size_t sub_index,
  Knowledge_Record::Integer value,
  const Knowledge_Update_Settings & settings)
{
  int result = -1;
  
  if (index.size () > 0 && context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    std::stringstream buffer;
    buffer << name_;
    
    for (unsigned int i = 0; i < index.size (); ++i)
    {
      buffer << ".";
      buffer << index[i];
    }

    result = context_->set_index (buffer.str (), sub_index, value, settings);
  }
  
  return result;
}

int
Madara::Knowledge_Engine::Containers::Vector_N::set (
  const Index & index,
  const Madara::Knowledge_Record::Integer * value,
  uint32_t size)
{
  int result = -1;
  
  if (index.size () > 0 && context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    std::stringstream buffer;
    buffer << name_;
    
    for (unsigned int i = 0; i < index.size (); ++i)
    {
      buffer << ".";
      buffer << index[i];
    }

    result = context_->set (buffer.str (), value, size, settings_);
  }
  
  return result;
}

int
Madara::Knowledge_Engine::Containers::Vector_N::set (
  const Index & index,
  const Madara::Knowledge_Record::Integer * value,
  uint32_t size,
  const Knowledge_Update_Settings & settings)
{
  int result = -1;
  
  if (index.size () > 0 && context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    std::stringstream buffer;
    buffer << name_;
    
    for (unsigned int i = 0; i < index.size (); ++i)
    {
      buffer << ".";
      buffer << index[i];
    }

    result = context_->set (buffer.str (), value, size, settings);
  }
  
  return result;
}

int
Madara::Knowledge_Engine::Containers::Vector_N::set (
  const Index & index,
  const std::vector <Knowledge_Record::Integer> & value)
{
  int result = -1;
  
  if (index.size () > 0 && context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    std::stringstream buffer;
    buffer << name_;
    
    for (unsigned int i = 0; i < index.size (); ++i)
    {
      buffer << ".";
      buffer << index[i];
    }

    result = context_->set (buffer.str (), value, settings_);
  }
  
  return result;
}

int
Madara::Knowledge_Engine::Containers::Vector_N::set (
  const Index & index,
  const std::vector <Knowledge_Record::Integer> & value,
  const Knowledge_Update_Settings & settings)
{
  int result = -1;
  
  if (index.size () > 0 && context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    std::stringstream buffer;
    buffer << name_;
    
    for (unsigned int i = 0; i < index.size (); ++i)
    {
      buffer << ".";
      buffer << index[i];
    }

    result = context_->set (buffer.str (), value, settings);
  }
  
  return result;
}

int
Madara::Knowledge_Engine::Containers::Vector_N::set (
  const Index & index,
  double value)
{
  int result = -1;
  
  if (index.size () > 0 && context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    std::stringstream buffer;
    buffer << name_;
    
    for (unsigned int i = 0; i < index.size (); ++i)
    {
      buffer << ".";
      buffer << index[i];
    }

    result = context_->set (buffer.str (), value, settings_);
  }
  
  return result;
}

int
Madara::Knowledge_Engine::Containers::Vector_N::set (
  const Index & index,
  double value, 
  const Knowledge_Update_Settings & settings)
{
  int result = -1;
  
  if (index.size () > 0 && context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    std::stringstream buffer;
    buffer << name_;
    
    for (unsigned int i = 0; i < index.size (); ++i)
    {
      buffer << ".";
      buffer << index[i];
    }

    result = context_->set (buffer.str (), value, settings);
  }
  
  return result;
}

int
Madara::Knowledge_Engine::Containers::Vector_N::set_index (
  const Index & index,
  size_t sub_index,
  double value)
{
  int result = -1;
  
  if (index.size () > 0 && context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    std::stringstream buffer;
    buffer << name_;
    
    for (unsigned int i = 0; i < index.size (); ++i)
    {
      buffer << ".";
      buffer << index[i];
    }

    result = context_->set_index (buffer.str (), sub_index, value, settings_);
  }
  
  return result;
}

int
Madara::Knowledge_Engine::Containers::Vector_N::set_index (
  const Index & index,
  size_t sub_index,
  double value,
  const Knowledge_Update_Settings & settings)
{
  int result = -1;
  
  if (index.size () > 0 && context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    std::stringstream buffer;
    buffer << name_;
    
    for (unsigned int i = 0; i < index.size (); ++i)
    {
      buffer << ".";
      buffer << index[i];
    }

    result = context_->set_index (buffer.str (), sub_index, value, settings);
  }
  
  return result;
}

int
Madara::Knowledge_Engine::Containers::Vector_N::set (
  const Index & index,
  const double * value,
  uint32_t size)
{
  int result = -1;
  
  if (index.size () > 0 && context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    std::stringstream buffer;
    buffer << name_;
    
    for (unsigned int i = 0; i < index.size (); ++i)
    {
      buffer << ".";
      buffer << index[i];
    }

    result = context_->set (buffer.str (), value, size, settings_);
  }
  
  return result;
}

int
Madara::Knowledge_Engine::Containers::Vector_N::set (
  const Index & index,
  const double * value,
  uint32_t size,
  const Knowledge_Update_Settings & settings)
{
  int result = -1;
  
  if (index.size () > 0 && context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    std::stringstream buffer;
    buffer << name_;
    
    for (unsigned int i = 0; i < index.size (); ++i)
    {
      buffer << ".";
      buffer << index[i];
    }

    result = context_->set (buffer.str (), value, size, settings);
  }
  
  return result;
}

int
Madara::Knowledge_Engine::Containers::Vector_N::set (
  const Index & index,
  const std::vector <double> & value)
{
  int result = -1;
  
  if (index.size () > 0 && context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    std::stringstream buffer;
    buffer << name_;
    
    for (unsigned int i = 0; i < index.size (); ++i)
    {
      buffer << ".";
      buffer << index[i];
    }

    result = context_->set (buffer.str (), value, settings_);
  }
  
  return result;
}

int
Madara::Knowledge_Engine::Containers::Vector_N::set (
  const Index & index,
  const std::vector <double> & value,
  const Knowledge_Update_Settings & settings)
{
  int result = -1;
  
  if (index.size () > 0 && context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    std::stringstream buffer;
    buffer << name_;
    
    for (unsigned int i = 0; i < index.size (); ++i)
    {
      buffer << ".";
      buffer << index[i];
    }

    result = context_->set (buffer.str (), value, settings);
  }
  
  return result;
}

int
Madara::Knowledge_Engine::Containers::Vector_N::set (
  const Index & index,
  const std::string & value)
{
  int result = -1;
  
  if (index.size () > 0 && context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    std::stringstream buffer;
    buffer << name_;
    
    for (unsigned int i = 0; i < index.size (); ++i)
    {
      buffer << ".";
      buffer << index[i];
    }

    result = context_->set (buffer.str (), value, settings_);
  }
  
  return result;
}

int
Madara::Knowledge_Engine::Containers::Vector_N::set (
  const Index & index,
  const std::string & value, 
  const Knowledge_Update_Settings & settings)
{
  int result = -1;
  
  if (index.size () > 0 && context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    std::stringstream buffer;
    buffer << name_;
    
    for (unsigned int i = 0; i < index.size (); ++i)
    {
      buffer << ".";
      buffer << index[i];
    }

    result = context_->set (buffer.str (), value, settings);
  }
  
  return result;
}

Madara::Knowledge_Engine::Knowledge_Update_Settings
Madara::Knowledge_Engine::Containers::Vector_N::set_settings (
  const Knowledge_Update_Settings & settings)
{
  Guard guard (mutex_);
  
  Knowledge_Update_Settings old_settings = settings_;

  settings_ = settings;

  return old_settings;
}

void
Madara::Knowledge_Engine::Containers::Vector_N::set_quality (
  const Index & index,
  uint32_t quality,
  const Knowledge_Reference_Settings & settings)
{
  if (index.size () > 0 && context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    std::stringstream buffer;
    buffer << name_;
    
    for (unsigned int i = 0; i < index.size (); ++i)
    {
      buffer << ".";
      buffer << index[i];
    }

    context_->set_quality (buffer.str (), quality, true, settings);
  }
}
