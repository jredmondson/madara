
#include <sstream>

#include "Barrier.h"
#include "madara/knowledge/ContextGuard.h"

madara::knowledge::containers::Barrier::Barrier(
    const KnowledgeUpdateSettings& settings)
  : BaseContainer("", settings), context_(0), id_(0), participants_(1),
    last_failed_check_(0)
{
}

madara::knowledge::containers::Barrier::Barrier(const std::string& name,
    KnowledgeBase& knowledge, const KnowledgeUpdateSettings& settings)
  : BaseContainer(name, settings),
    context_(&(knowledge.get_context())),
    id_(0),
    participants_(1),
    last_failed_check_(0)
{
  build_aggregate_barrier();
}

madara::knowledge::containers::Barrier::Barrier(const std::string& name,
    Variables& knowledge, const KnowledgeUpdateSettings& settings)
  : BaseContainer(name, settings),
    context_(knowledge.get_context()),
    id_(0),
    participants_(1),
    last_failed_check_(0)
{
  build_aggregate_barrier();
}

madara::knowledge::containers::Barrier::Barrier(const std::string& name,
    KnowledgeBase& knowledge, int id, int participants,
    const KnowledgeUpdateSettings& settings)
  : BaseContainer(name, settings),
    context_(&(knowledge.get_context())),
    id_(id),
    participants_(participants),
    last_failed_check_(0)
{
  build_aggregate_barrier();
}

madara::knowledge::containers::Barrier::Barrier(const std::string& name,
    Variables& knowledge, int id, int participants,
    const KnowledgeUpdateSettings& settings)
  : BaseContainer(name, settings),
    context_(knowledge.get_context()),
    id_(id),
    participants_(participants),
    last_failed_check_(0)
{
  build_aggregate_barrier();
}

madara::knowledge::containers::Barrier::Barrier(const Barrier& rhs)
  : BaseContainer(rhs),
    context_(rhs.context_),
    barrier_(rhs.barrier_),
    id_(rhs.id_),
    participants_(rhs.participants_),
    last_failed_check_(rhs.last_failed_check_)
{
}

madara::knowledge::containers::Barrier::~Barrier() {}

void madara::knowledge::containers::Barrier::operator=(const Barrier& rhs)
{
  if (this != &rhs)
  {
    MADARA_GUARD_TYPE guard(mutex_), guard2(rhs.mutex_);

    this->context_ = rhs.context_;
    this->name_ = rhs.name_;
    this->id_ = rhs.id_;
    this->participants_ = rhs.participants_;
    this->settings_ = rhs.settings_;
    this->barrier_ = rhs.barrier_;
    last_failed_check_ = rhs.last_failed_check_;
  }
}

void madara::knowledge::containers::Barrier::build_aggregate_barrier(void)
{
  if (context_ && name_ != "")
  {
    ContextGuard context_guard(*context_);
    MADARA_GUARD_TYPE guard(mutex_);

    barrier_.resize(participants_);

    // add all other barrierer variables
    for (size_t i = 0; i < participants_; ++i)
    {
      std::stringstream buffer;
      buffer << name_;
      buffer << ".";
      buffer << i;

      barrier_[i].set_name(buffer.str(), *context_);
    }

    barrier_[id_] = 0;
    barrier_[id_].write();

    madara_logger_log(context_->get_logger(), logger::LOG_MAJOR,
        "Barrier::build_aggregate_barrier: built %d member barrier\n",
        (int)participants_);
  }
  else if (name_ == "")
  {
    context_->print("ERROR: Container::Barrier needs a name.\n", 0);
  }
  else if (!context_)
  {
    context_->print("ERROR: Container::Barrier needs a context.\n", 0);
  }
}

size_t madara::knowledge::containers::Barrier::get_id(void) const
{
  MADARA_GUARD_TYPE guard(mutex_);
  return id_;
}

size_t madara::knowledge::containers::Barrier::get_participants(void) const
{
  MADARA_GUARD_TYPE guard(mutex_);
  return participants_;
}

void madara::knowledge::containers::Barrier::set_name(
    const std::string& var_name, KnowledgeBase& knowledge, int id,
    int participants)
{
  KnowledgeUpdateSettings keep_local(true);
  context_ = &(knowledge.get_context());

  ContextGuard context_guard(*context_);
  MADARA_GUARD_TYPE guard(mutex_);

  name_ = var_name;
  id_ = id;
  participants_ = participants;
  last_failed_check_ = 0;

  this->build_aggregate_barrier();
}

void madara::knowledge::containers::Barrier::set_name(
    const std::string& var_name, Variables& knowledge, int id, int participants)
{
  KnowledgeUpdateSettings keep_local(true);
  context_ = knowledge.get_context();

  ContextGuard context_guard(*context_);
  MADARA_GUARD_TYPE guard(mutex_);

  name_ = var_name;
  id_ = id;
  participants_ = participants;
  last_failed_check_ = 0;

  this->build_aggregate_barrier();
}

void madara::knowledge::containers::Barrier::set_name(
    const std::string& var_name, ThreadSafeContext& knowledge, int id,
    int participants)
{
  KnowledgeUpdateSettings keep_local(true);
  context_ = &knowledge;

  ContextGuard context_guard(*context_);
  MADARA_GUARD_TYPE guard(mutex_);

  name_ = var_name;
  id_ = id;
  participants_ = participants;
  last_failed_check_ = 0;

  this->build_aggregate_barrier();
}

void madara::knowledge::containers::Barrier::resize(
    size_t id, size_t participants)
{
  if (context_)
  {
    ContextGuard context_guard(*context_);
    MADARA_GUARD_TYPE guard(mutex_);

    id_ = id;
    participants_ = participants;
    last_failed_check_ = 0;

    this->build_aggregate_barrier();
  }
}

madara::knowledge::containers::Barrier::type
madara::knowledge::containers::Barrier::operator=(type value)
{
  if (context_)
  {
    ContextGuard context_guard(*context_);
    MADARA_GUARD_TYPE guard(mutex_);
    // context_->set(variable_, value, settings_);
    barrier_[id_] = value;

    last_failed_check_ = 0;
  }

  return value;
}

madara::knowledge::KnowledgeRecord
madara::knowledge::containers::Barrier::to_record(void) const
{
  madara::knowledge::KnowledgeRecord result;

  if (context_)
  {
    result = *barrier_[id_];
  }

  return result;
}

madara::knowledge::KnowledgeRecord::Integer
madara::knowledge::containers::Barrier::to_integer(void) const
{
  madara::knowledge::KnowledgeRecord::Integer result(0);

  if (context_)
  {
    result = *barrier_[id_];
  }

  return result;
}

void madara::knowledge::containers::Barrier::next(void)
{
  if (context_)
  {
    ++barrier_[id_];
    last_failed_check_ = 0;
    barrier_[id_].write();
  }
}

bool madara::knowledge::containers::Barrier::is_done(void)
{
  bool result = false;

  if (context_ && name_ != "")
  {
    ContextGuard context_guard(*context_);
    MADARA_GUARD_TYPE guard(mutex_);

    madara_logger_log(context_->get_logger(), logger::LOG_MAJOR,
        "Barrier::is_done: checking barrier result for done\n");

    result = barrier_result() == 1;

    if (!result)
    {
      madara_logger_log(context_->get_logger(), logger::LOG_MAJOR,
          "Barrier::is_true: barrier is not true, remarking barrier "
          "variable\n");

      barrier_[id_].write();

      context_->print(logger::LOG_DETAILED);
    }
    else
    {
      madara_logger_log(context_->get_logger(), logger::LOG_MAJOR,
          "Barrier::is_true: barrier is true\n");
    }
  }

  return result;
}

void madara::knowledge::containers::Barrier::set(type value)
{
  if (context_ && name_ != "")
  {
    barrier_[id_] = value;
  }
}

void madara::knowledge::containers::Barrier::modify(void)
{
  if (context_ && name_ != "")
  {
    barrier_[id_].write();
  }
}

std::string madara::knowledge::containers::Barrier::get_debug_info(void)
{
  std::stringstream result;

  result << "Barrier: ";

  // if at all possible, do not try to touch the KB with any locks
  if (context_)
  {
    // note that because the variables are staged, we should only
    for (size_t i = 0; i < participants_; ++i)
    {
      // we could speed this up by creating unsafe versions of functions like
      // get name that do not lock the vars local mutex, but this should be
      // reasonable
      result << barrier_[i].get_name();
      result << "={";
      result << *barrier_[i];
      result << "}\n";
    }
  }

  return result.str();
}

void madara::knowledge::containers::Barrier::modify_(void)
{
  modify();
}

std::string madara::knowledge::containers::Barrier::get_debug_info_(void)
{
  return get_debug_info();
}

madara::knowledge::containers::BaseContainer*
madara::knowledge::containers::Barrier::clone(void) const
{
  return new Barrier(*this);
}

double madara::knowledge::containers::Barrier::to_double(void) const
{
  double result(0.0);

  if (context_)
  {
    result = barrier_[id_].to_double();
  }

  return result;
}

std::string madara::knowledge::containers::Barrier::to_string(void) const
{
  std::string result;

  if (context_)
  {
    result = barrier_[id_].to_string();
  }

  return result;
}

void madara::knowledge::containers::Barrier::set_quality(
    uint32_t quality, const KnowledgeReferenceSettings& settings)
{
  if (context_)
  {
    barrier_[id_].set_quality(quality, settings);
  }
}

bool madara::knowledge::containers::Barrier::is_true(void) const
{
  bool result(false);

  madara_logger_log(context_->get_logger(), logger::LOG_MAJOR,
      "Barrier::is_true: checking barrier result for truth\n");

  if (context_)
  {
    // this could be dangerous in that if someone changes barrier name in
    // the middle of this, we're not thread safe. However, this is an
    // optimization decision that is extremely performant in comparison.
    // and it would be weird if someone wanted to change the barrier name
    result = barrier_result() == 1;
  }

  madara_logger_log(context_->get_logger(), logger::LOG_MAJOR,
      "Barrier::is_true: final result is %d\n", (int)result);

  return result;
}

bool madara::knowledge::containers::Barrier::is_false(void) const
{
  return !is_true();
}

bool madara::knowledge::containers::Barrier::is_true_(void) const
{
  return is_true();
}

bool madara::knowledge::containers::Barrier::is_false_(void) const
{
  return is_false();
}
