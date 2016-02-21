#include "BaseContainer.h"
#include "madara/knowledge/ContextGuard.h"

inline std::string
madara::knowledge::containers::BaseContainer::get_name (void) const
{
  MADARA_GUARD_TYPE guard (mutex_);
  return name_;
}
