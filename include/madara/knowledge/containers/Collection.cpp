#include "Collection.h"

madara::knowledge::containers::Collection::Collection() : BaseContainer() {}

madara::knowledge::containers::Collection::Collection(const Collection& rhs)
  : BaseContainer()
{
  vector_.resize(rhs.vector_.size());
  for (size_t i = 0; i < rhs.vector_.size(); ++i)
  {
    vector_[i] = rhs.vector_[i]->clone();
  }
}

madara::knowledge::containers::Collection::~Collection()
{
  clear();
}

void madara::knowledge::containers::Collection::modify(void)
{
  MADARA_GUARD_TYPE guard(mutex_);

  for (size_t i = 0; i < vector_.size(); ++i)
  {
    vector_[i]->modify_();
  }
}

std::string madara::knowledge::containers::Collection::get_debug_info(void)
{
  MADARA_GUARD_TYPE guard(mutex_);

  std::stringstream result;

  for (size_t i = 0; i < vector_.size(); ++i)
  {
    result << vector_[i]->get_debug_info_() << "\n";
  }

  return result.str();
}

void madara::knowledge::containers::Collection::modify_(void)
{
  modify();
}

std::string madara::knowledge::containers::Collection::get_debug_info_(void)
{
  return get_debug_info();
}

madara::knowledge::containers::BaseContainer*
madara::knowledge::containers::Collection::clone(void) const
{
  return new Collection(*this);
}

bool madara::knowledge::containers::Collection::is_true(void) const
{
  bool result(true);

  madara_logger_log(context_->get_logger(), logger::LOG_MAJOR,
      "Collection::is_true: checking all containers for truth\n");

  if (context_)
  {
    ContextGuard context_guard(*context_);
    MADARA_GUARD_TYPE guard(mutex_);

    for (size_t i = 0; i < vector_.size(); ++i)
    {
      if (vector_[i]->is_false_())
      {
        result = false;
        break;
      }
    }
  }

  madara_logger_log(context_->get_logger(), logger::LOG_MAJOR,
      "Collection::is_true: final result is %d\n", (int)result);

  return result;
}

bool madara::knowledge::containers::Collection::is_false(void) const
{
  return !is_true();
}

bool madara::knowledge::containers::Collection::is_true_(void) const
{
  return is_true();
}

bool madara::knowledge::containers::Collection::is_false_(void) const
{
  return is_false();
}
