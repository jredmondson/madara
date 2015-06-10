#include "madara/knowledge_engine/Knowledge_Record.h"
#include "madara/knowledge_engine/Variable_Reference.h"
#include "madara/utility/stdint.h"

Madara::Knowledge_Engine::Variable_Reference::Variable_Reference ()
  : record_ (0), name_ (0)
{
}


Madara::Knowledge_Engine::Variable_Reference::Variable_Reference (
  const Variable_Reference & rhs)
  : record_ (rhs.record_), name_ (rhs.name_)
{
}


Madara::Knowledge_Engine::Variable_Reference::~Variable_Reference ()
{
}

void
Madara::Knowledge_Engine::Variable_Reference::operator= (
  const Variable_Reference & rhs)
{
  if (this != &rhs)
  {
    record_ = rhs.record_;
    name_ = rhs.name_;
  }
}

bool
Madara::Knowledge_Engine::Variable_Reference::operator== (
  const Variable_Reference & rhs) const
{
  return this->record_ == rhs.record_;
}

bool
Madara::Knowledge_Engine::Variable_Reference::operator!= (
  const Variable_Reference & rhs) const
{
  return this->record_ != rhs.record_;
}

const char *
Madara::Knowledge_Engine::Variable_Reference::get_name (void)
{
  return name_.get_ptr ();
}

void
Madara::Knowledge_Engine::Variable_Reference::set_name (
  const std::string & name)
{
  // create a new char array and copy over the string
  uint32_t size = uint32_t (name.length () + 1);
  char * temp = new char [size];
  strncpy (temp, name.c_str (), size - 1);
  temp[size - 1] = 0;
  name_ = temp;
}

bool Madara::Knowledge_Engine::Variable_Reference::is_valid (void) const
{
  return record_ != 0;
}