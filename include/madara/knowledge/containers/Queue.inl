
#ifndef _MADARA_CONTAINERS_QUEUE_INL_
#define _MADARA_CONTAINERS_QUEUE_INL_

#include <sstream>

#include "Queue.h"
#include "madara/knowledge/ContextGuard.h"

inline madara::knowledge::containers::Queue::Queue(
    const KnowledgeUpdateSettings& settings)
  : context_(0), settings_(settings)
{
}

inline madara::knowledge::containers::Queue::Queue(const std::string& name,
    KnowledgeBase& knowledge, const KnowledgeUpdateSettings& settings)
  : context_(&(knowledge.get_context())),
    name_(name),
    count_(name + ".count", knowledge, 0),
    head_(name + ".head", knowledge, 0),
    tail_(name + ".tail", knowledge, 0),
    queue_(name, knowledge),
    settings_(settings)
{
  resize();
}

inline madara::knowledge::containers::Queue::Queue(const std::string& name,
    Variables& knowledge, const KnowledgeUpdateSettings& settings)
  : context_(knowledge.get_context()),
    name_(name),
    count_(name + ".count", knowledge),
    head_(name + ".head", knowledge),
    tail_(name + ".tail", knowledge),
    queue_(name, knowledge),
    settings_(settings)
{
  resize();
}

inline madara::knowledge::containers::Queue::Queue(const std::string& name,
    KnowledgeBase& knowledge, int size, const KnowledgeUpdateSettings& settings)
  : context_(&(knowledge.get_context())),
    name_(name),
    count_(name + ".count", knowledge, 0),
    head_(name + ".head", knowledge, 0),
    tail_(name + ".tail", knowledge, 0),
    queue_(name, knowledge, size, false, settings),
    settings_(settings)
{
}

inline madara::knowledge::containers::Queue::Queue(const std::string& name,
    Variables& knowledge, int size, const KnowledgeUpdateSettings& settings)
  : context_(knowledge.get_context()),
    name_(name),
    count_(name + ".count", knowledge, 0),
    head_(name + ".head", knowledge, 0),
    tail_(name + ".tail", knowledge, 0),
    queue_(name, knowledge, size, false, settings),
    settings_(settings)
{
}

inline madara::knowledge::containers::Queue::Queue(const Queue& rhs)
  : context_(rhs.context_),
    name_(rhs.name_),
    count_(rhs.count_),
    head_(rhs.head_),
    tail_(rhs.tail_),
    queue_(rhs.queue_),
    settings_(rhs.settings_)
{
}

inline void madara::knowledge::containers::Queue::operator=(const Queue& rhs)
{
  if (this != &rhs) {
    MADARA_GUARD_TYPE guard(mutex_), guard2(rhs.mutex_);

    this->context_ = rhs.context_;
    this->name_ = rhs.name_;
    this->count_ = rhs.count_;
    this->head_ = rhs.head_;
    this->tail_ = rhs.tail_;
    this->queue_ = rhs.queue_;
    this->settings_ = rhs.settings_;
  }
}

inline bool madara::knowledge::containers::Queue::operator==(
    const Queue& value) const
{
  return name_ == value.get_name();
}

inline bool madara::knowledge::containers::Queue::operator!=(
    const Queue& value) const
{
  return name_ != value.get_name();
}

template<typename... Args>
inline bool madara::knowledge::containers::Queue::emplace(Args&&... args)
{
  return enqueue(KnowledgeRecord(std::forward<Args>(args)...));
}

inline void madara::knowledge::containers::Queue::clear(void)
{
  if (context_ && name_ != "" && count_ > 0) {
    ContextGuard context_guard(*context_);
    MADARA_GUARD_TYPE guard(mutex_);

    count_ = 0;
    head_ = *tail_;
  }
}

inline madara::knowledge::KnowledgeRecord
madara::knowledge::containers::Queue::inspect(size_t position)
{
  madara::knowledge::KnowledgeRecord result;
  if (context_ && name_ != "" && (KnowledgeRecord::Integer)position < *count_) {
    ContextGuard context_guard(*context_);
    MADARA_GUARD_TYPE guard(mutex_);

    size_t index =
        (size_t)increment(*head_, (KnowledgeRecord::Integer)position);

    result = context_->get(queue_.vector_[index], settings_);
  }
  return result;
}

inline std::string madara::knowledge::containers::Queue::get_name(void) const
{
  MADARA_GUARD_TYPE guard(mutex_);
  return name_;
}

inline size_t madara::knowledge::containers::Queue::size(void)
{
  return queue_.size();
}

inline size_t madara::knowledge::containers::Queue::count(void)
{
  return (size_t)(*count_);
}

inline void madara::knowledge::containers::Queue::resize(int size)
{
  if (context_) {
    ContextGuard context_guard(*context_);
    MADARA_GUARD_TYPE guard(mutex_);

    queue_.resize(size, false);

    if (count_ > (KnowledgeRecord::Integer)queue_.size()) {
      count_ = (KnowledgeRecord::Integer)size;
    }
  }
}

inline madara::knowledge::KnowledgeUpdateSettings
madara::knowledge::containers::Queue::set_settings(
    const KnowledgeUpdateSettings& settings)
{
  MADARA_GUARD_TYPE guard(mutex_);

  KnowledgeUpdateSettings old_settings = settings_;

  settings_ = settings;

  return old_settings;
}

inline void madara::knowledge::containers::Queue::set_quality(
    uint32_t quality, const KnowledgeReferenceSettings& settings)
{
  if (context_) {
    ContextGuard context_guard(*context_);
    MADARA_GUARD_TYPE guard(mutex_);
    context_->set_quality(name_, quality, true, settings);
  }
}

#endif  //  _MADARA_CONTAINERS_QUEUE_INL_
