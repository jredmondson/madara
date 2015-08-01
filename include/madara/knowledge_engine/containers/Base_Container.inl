#include "Base_Container.h"
#include "madara/knowledge_engine/Context_Guard.h"

inline std::string
Madara::Knowledge_Engine::Containers::Base_Container::get_name (void) const
{
  Guard guard (mutex_);
  return name_;
}
