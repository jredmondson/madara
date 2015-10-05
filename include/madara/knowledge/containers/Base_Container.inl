#include "Base_Container.h"
#include "madara/knowledge/Context_Guard.h"

inline std::string
madara::knowledge::containers::Base_Container::get_name (void) const
{
  Guard guard (mutex_);
  return name_;
}
