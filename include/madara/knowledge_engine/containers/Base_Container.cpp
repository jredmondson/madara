#include "Base_Container.h"
#include "madara/logger/Global_Logger.h"

namespace logger = Madara::Logger;

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

bool Madara::Knowledge_Engine::Containers::Base_Container::modify_if_true (
  Base_Container & conditional)
{
  bool result (false);

  madara_logger_ptr_log (logger::global_logger.get (), logger::LOG_MAJOR,
    "Base_Container::is_true: calling condition.is_true()\n");

  if (conditional.is_true_ ())
  {
    madara_logger_ptr_log (logger::global_logger.get (), logger::LOG_MINOR,
      "Base_Container::is_true: condition.is_true() returned true\n");

    this->modify_ ();
    result = true;
  }

  return result;
}

bool Madara::Knowledge_Engine::Containers::Base_Container::modify_if_false (
  Base_Container & conditional)
{
  bool result (false);

  madara_logger_ptr_log (logger::global_logger.get (), logger::LOG_MAJOR,
    "Base_Container::is_false: calling !condition.is_true()\n");

  if (!conditional.is_true_ ())
  {
    madara_logger_ptr_log (logger::global_logger.get (), logger::LOG_MINOR,
      "Base_Container::is_false: !condition.is_true() returned true\n");
    this->modify_ ();
    result = true;
  }

  return result;
}
