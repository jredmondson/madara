#include "madara/knowledge/KnowledgeRecord.h"
#include "madara/knowledge/VariableReference.h"
#include "madara/utility/stdint.h"

madara::knowledge::VariableReference::VariableReference ()
  : record_ (0), name_ (0)
{
}


madara::knowledge::VariableReference::VariableReference (
  const VariableReference & rhs)
  : record_ (rhs.record_), name_ (rhs.name_)
{
}

madara::knowledge::VariableReference::~VariableReference ()
{
}

void
madara::knowledge::VariableReference::operator= (
  const VariableReference & rhs)
{
  if (this != &rhs)
  {
    record_ = rhs.record_;
    name_ = rhs.name_;
  }
}

bool
madara::knowledge::VariableReference::operator== (
  const VariableReference & rhs) const
{
  return this->record_ == rhs.record_;
}

bool
madara::knowledge::VariableReference::operator!= (
  const VariableReference & rhs) const
{
  return this->record_ != rhs.record_;
}

const char *
madara::knowledge::VariableReference::get_name (void) const
{
  return name_.get_ptr ();
}

void
madara::knowledge::VariableReference::set_name (
  const std::string & name)
{
  // create a new char array and copy over the string
  uint32_t size = uint32_t (name.length () + 1);
  char * temp = new char [size];
  strncpy (temp, name.c_str (), size - 1);
  temp[size - 1] = 0;
  name_ = temp;
}

bool madara::knowledge::VariableReference::is_valid (void) const
{
  return record_ != 0;
}
