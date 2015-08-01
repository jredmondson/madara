#include "Base_Container.h"

Madara::Knowledge_Engine::Containers::Base_Container::Base_Container (
  const std::string & name, const Knowledge_Update_Settings & settings)
  : name_ (name), settings_ (settings)
{
}


Madara::Knowledge_Engine::Containers::Base_Container::Base_Container (
  const Base_Container & rhs)
: name_ (rhs.name_),
  settings_ (rhs.settings_)
{

}


Madara::Knowledge_Engine::Containers::Base_Container::~Base_Container ()
{

}

void
Madara::Knowledge_Engine::Containers::Base_Container::set_settings (
const Knowledge_Update_Settings & settings)
{
  Guard guard (mutex_);

  settings_ = settings;
}
