#include <sstream>

#include "Queue.h"
#include "madara/knowledge/ContextGuard.h"

bool madara::knowledge::containers::Queue::enqueue(
    const knowledge::KnowledgeRecord& record)
{
  bool result(false);

  if (context_)
  {
    ContextGuard context_guard(*context_);
    MADARA_GUARD_TYPE guard(mutex_);

    if (count_ < 0)
      count_ = 0;

    // avoid double retrieval of count_ during check
    knowledge::KnowledgeRecord::Integer count = *count_;

    if (context_ && name_ != "" && queue_.size() > 0 &&
        count < (KnowledgeRecord::Integer)queue_.size())
    {
      // there's no point in signaling inside of a locked context
      if (!settings_.signal_changes)
        settings_.signal_changes = false;

      knowledge::KnowledgeRecord::Integer tail = *tail_;

      context_->set(queue_.vector_[tail], record, settings_);
      tail_ = increment(tail, 1);
      ++count_;
    }
  }

  // now that we no longer have a lock on context, signal
  if (context_)
    context_->signal();

  return result;
}

madara::knowledge::KnowledgeRecord
madara::knowledge::containers::Queue::dequeue(bool wait)
{
  madara::knowledge::KnowledgeRecord result;

  if (context_ && name_ != "")
  {
    ContextGuard context_guard(*context_);
    MADARA_GUARD_TYPE guard(mutex_);

    if (wait)
    {
      while (count_ <= 0)
        context_->wait_for_change(true);
    }

    if (count_ > 0)
    {
      knowledge::KnowledgeRecord::Integer head = *head_;

      result = context_->get(queue_.vector_[head], settings_);

      head_ = increment(head, 1);
      --count_;
    }
  }

  // now that we no longer have a lock on context, signal
  if (context_)
    context_->signal();

  return result;
}

void madara::knowledge::containers::Queue::set_name(
    const std::string& var_name, KnowledgeBase& knowledge)
{
  KnowledgeUpdateSettings keep_local(true);
  context_ = &(knowledge.get_context());

  ContextGuard context_guard(*context_);
  MADARA_GUARD_TYPE guard(mutex_);

  name_ = var_name;

  this->count_.set_name(var_name + ".count", knowledge);
  this->head_.set_name(var_name + ".head", knowledge);
  this->tail_.set_name(var_name + ".tail", knowledge);
  this->queue_.set_name(var_name, knowledge);
}

void madara::knowledge::containers::Queue::set_name(
    const std::string& var_name, Variables& knowledge)
{
  KnowledgeUpdateSettings keep_local(true);
  context_ = knowledge.get_context();

  ContextGuard context_guard(*context_);
  MADARA_GUARD_TYPE guard(mutex_);

  name_ = var_name;

  this->count_.set_name(var_name + ".count", knowledge);
  this->head_.set_name(var_name + ".head", knowledge);
  this->tail_.set_name(var_name + ".tail", knowledge);
  this->queue_.set_name(var_name, knowledge);
}
