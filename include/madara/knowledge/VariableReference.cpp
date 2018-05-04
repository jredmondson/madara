#include "madara/knowledge/KnowledgeRecord.h"
#include "madara/knowledge/VariableReference.h"
#include "madara/utility/stdint.h"

madara::knowledge::VariableReference::VariableReference ()
  : name_ (0), record_ (0)
{
}


madara::knowledge::VariableReference::VariableReference (
  const VariableReference & rhs)
  : name_ (rhs.name_), record_ (rhs.record_)
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
