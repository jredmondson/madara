#include "StringStaged.h"
#include "madara/knowledge/ContextGuard.h"

madara::knowledge::containers::StringStaged::StringStaged(
    const KnowledgeUpdateSettings& settings)
  : BaseContainer("", settings), context_(0), has_changed_(false)
{
}

madara::knowledge::containers::StringStaged::StringStaged(
    const std::string& name, KnowledgeBase& knowledge,
    const KnowledgeUpdateSettings& settings)
  : BaseContainer(name, settings),
    context_(&(knowledge.get_context())),
    has_changed_(false)
{
  variable_ = knowledge.get_ref(name, settings_);
  value_ = knowledge.get(variable_).to_string();
}

madara::knowledge::containers::StringStaged::StringStaged(
    const std::string& name, Variables& knowledge,
    const KnowledgeUpdateSettings& settings)
  : BaseContainer(name, settings),
    context_(knowledge.get_context()),
    has_changed_(false)
{
  variable_ = knowledge.get_ref(name, settings_);
  value_ = knowledge.get(variable_).to_string();
}

madara::knowledge::containers::StringStaged::StringStaged(
    const std::string& name, KnowledgeBase& knowledge, const type& value,
    const KnowledgeUpdateSettings& settings)
  : BaseContainer(name, settings),
    context_(&(knowledge.get_context())),
    value_(value),
    has_changed_(false)
{
  variable_ = knowledge.get_ref(name);
  context_->set(variable_, value, settings);
}

madara::knowledge::containers::StringStaged::StringStaged(
    const std::string& name, Variables& knowledge, const type& value,
    const KnowledgeUpdateSettings& settings)
  : BaseContainer(name, settings),
    context_(knowledge.get_context()),
    value_(value),
    has_changed_(false)
{
  variable_ = knowledge.get_ref(name);
  context_->set(variable_, value, settings);
}

madara::knowledge::containers::StringStaged::StringStaged(
    const StringStaged& rhs)
  : BaseContainer(rhs),
    context_(rhs.context_),
    variable_(rhs.variable_),
    value_(rhs.value_)
{
}

madara::knowledge::containers::StringStaged::~StringStaged()
{
  write();
}

void madara::knowledge::containers::StringStaged::modify_(void)
{
  modify();
}

std::string madara::knowledge::containers::StringStaged::get_debug_info_(void)
{
  return get_debug_info();
}

void madara::knowledge::containers::StringStaged::exchange(
    containers::StringStaged& other)
{
  if (context_ && other.context_) {
    std::lock(*context_, *other.context_, mutex_, other.mutex_);

    ContextGuard context_guard(*context_, std::adopt_lock);
    ContextGuard other_context_guard(*other.context_, std::adopt_lock);
    MADARA_GUARD_TYPE guard(mutex_, std::adopt_lock),
        guard2(other.mutex_, std::adopt_lock);

    type temp = *other;
    other = **this;
    *this = temp;
  }
}

bool madara::knowledge::containers::StringStaged::is_true_(void) const
{
  return is_true();
}

bool madara::knowledge::containers::StringStaged::is_false_(void) const
{
  return is_false();
}
