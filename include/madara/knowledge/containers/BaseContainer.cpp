#include "BaseContainer.h"
#include "madara/logger/GlobalLogger.h"

namespace logger = madara::logger;

madara::knowledge::containers::BaseContainer::BaseContainer(
    const std::string& name, const KnowledgeUpdateSettings& settings)
  : name_(name), settings_(settings)
{
}

madara::knowledge::containers::BaseContainer::BaseContainer(
    const BaseContainer& rhs)
  : name_(rhs.name_), settings_(rhs.settings_)
{
}

madara::knowledge::containers::BaseContainer::~BaseContainer() {}

void madara::knowledge::containers::BaseContainer::set_settings(
    const KnowledgeUpdateSettings& settings)
{
  MADARA_GUARD_TYPE guard(mutex_);

  settings_ = settings;
}

madara::knowledge::KnowledgeUpdateSettings
madara::knowledge::containers::BaseContainer::get_settings(void)
{
  MADARA_GUARD_TYPE guard(mutex_);

  return settings_;
}

bool madara::knowledge::containers::BaseContainer::modify_if_true(
    BaseContainer& conditional)
{
  bool result(false);

  madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_MAJOR,
      "BaseContainer::is_true: calling condition.is_true()\n");

  if(conditional.is_true_())
  {
    madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_MINOR,
        "BaseContainer::is_true: condition.is_true() returned true\n");

    this->modify_();
    result = true;
  }

  return result;
}

bool madara::knowledge::containers::BaseContainer::modify_if_false(
    BaseContainer& conditional)
{
  bool result(false);

  madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_MAJOR,
      "BaseContainer::is_false: calling !condition.is_true()\n");

  if(!conditional.is_true_())
  {
    madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_MINOR,
        "BaseContainer::is_false: !condition.is_true() returned true\n");
    this->modify_();
    result = true;
  }

  return result;
}
