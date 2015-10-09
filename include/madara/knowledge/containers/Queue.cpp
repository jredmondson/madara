#include <sstream>

#include "Queue.h"
#include "madara/knowledge/ContextGuard.h"

madara::knowledge::containers::Queue::Queue (
  const KnowledgeUpdateSettings & settings)
: context_ (0), settings_ (settings)
{
}
  
madara::knowledge::containers::Queue::Queue (
  const std::string & name,
  KnowledgeBase & knowledge,
  const KnowledgeUpdateSettings & settings)
: context_ (&(knowledge.get_context ())), name_ (name),
  count_ (name + ".count", knowledge, 0),
  head_ (name + ".head", knowledge, 0),
  tail_ (name + ".tail", knowledge, 0),
  queue_ (name, knowledge),
  settings_ (settings) 
{
  resize ();
}
 
madara::knowledge::containers::Queue::Queue (
  const std::string & name,
  Variables & knowledge,
  const KnowledgeUpdateSettings & settings)
: context_ (knowledge.get_context ()), name_ (name),
  count_ (name + ".count", knowledge),
  head_ (name + ".head", knowledge),
  tail_ (name + ".tail", knowledge),
  queue_ (name, knowledge),
  settings_ (settings) 
{
  resize ();
}
 
madara::knowledge::containers::Queue::Queue (
  const std::string & name,
  KnowledgeBase & knowledge,
  int size,
  const KnowledgeUpdateSettings & settings)
: context_ (&(knowledge.get_context ())), name_ (name),
  count_ (name + ".count", knowledge, 0),
  head_ (name + ".head", knowledge, 0),
  tail_ (name + ".tail", knowledge, 0),
  queue_ (name, knowledge, size, false, settings),
  settings_ (settings) 
{
}
 
madara::knowledge::containers::Queue::Queue (
  const std::string & name,
  Variables & knowledge,
  int size,
  const KnowledgeUpdateSettings & settings)
: context_ (knowledge.get_context ()), name_ (name),
  count_ (name + ".count", knowledge, 0),
  head_ (name + ".head", knowledge, 0),
  tail_ (name + ".tail", knowledge, 0),
  queue_ (name, knowledge, size, false, settings),
  settings_ (settings) 
{
}


madara::knowledge::containers::Queue::Queue (const Queue & rhs)
  : context_ (rhs.context_),
    name_ (rhs.name_),
    count_ (rhs.count_),
    head_ (rhs.head_),
    tail_ (rhs.tail_),
    queue_ (rhs.queue_),
    settings_ (rhs.settings_)
{

}


madara::knowledge::containers::Queue::~Queue ()
{

}

void
madara::knowledge::containers::Queue::operator= (const Queue & rhs)
{
  if (this != &rhs)
  {
    Guard guard (mutex_), guard2 (rhs.mutex_);

    this->context_ = rhs.context_;
    this->name_ = rhs.name_;
    this->count_ = rhs.count_;
    this->head_ = rhs.head_;
    this->tail_ = rhs.tail_;
    this->queue_ = rhs.queue_;
    this->settings_ = rhs.settings_;
  }
}


bool
madara::knowledge::containers::Queue::operator== (
  const Queue & value) const
{
  return name_ == value.get_name ();
}
        
bool
madara::knowledge::containers::Queue::operator!= (
  const Queue & value) const
{
  return name_ != value.get_name ();
}

bool
madara::knowledge::containers::Queue::enqueue (
  const KnowledgeRecord & record)
{
  bool result (false);

  if (context_)
  {
    ContextGuard context_guard (*context_);
    Guard guard (mutex_);

    if (count_ < 0)
      count_ = 0;
    
    // avoid double retrieval of count_ during check
    KnowledgeRecord::Integer count = *count_;

    if (context_ && name_ != "" && queue_.size () > 0 && 
        count < (KnowledgeRecord::Integer)queue_.size ())
    {
      // there's no point in signaling inside of a locked context
      if (!settings_.signal_changes)
        settings_.signal_changes = false;
      
      KnowledgeRecord::Integer tail = *tail_;

      context_->set (queue_.vector_[tail], record, settings_);
      tail_ = increment (tail, 1);
      ++count_;
    }
  }

  // now that we no longer have a lock on context, signal
  if (context_)
    context_->signal ();

  return result;
}

madara::KnowledgeRecord
madara::knowledge::containers::Queue::dequeue (bool wait)
{
  madara::KnowledgeRecord result;
  
  if (context_ && name_ != "")
  {
    ContextGuard context_guard (*context_);
    Guard guard (mutex_);
    
    if (wait)
    {
      while (count_ <= 0)
        context_->wait_for_change (true);
    }

    if (count_ > 0)
    {
      KnowledgeRecord::Integer head = *head_;

      result = context_->get (queue_.vector_[head], settings_);
    
      head_ = increment (head, 1);
      --count_;
    }
  }
  
  // now that we no longer have a lock on context, signal
  if (context_)
    context_->signal ();

  return result;
}

void
madara::knowledge::containers::Queue::clear (void)
{
  if (context_ && name_ != "" && count_ > 0)
  {
    ContextGuard context_guard (*context_);
    Guard guard (mutex_);

    count_ = 0;
    head_ = *tail_;
  }
}

madara::KnowledgeRecord
madara::knowledge::containers::Queue::inspect (size_t position)
{
  madara::KnowledgeRecord result;
  if (context_ && name_ != "" &&
      (KnowledgeRecord::Integer)position < *count_)
  {
    ContextGuard context_guard (*context_);
    Guard guard (mutex_);

    size_t index = (size_t)increment (
      *head_, (KnowledgeRecord::Integer)position);
    
    result = context_->get (queue_.vector_[index], settings_);
  }
  return result;
}

std::string
madara::knowledge::containers::Queue::get_name (void) const
{
  Guard guard (mutex_);
  return name_;
}

void
madara::knowledge::containers::Queue::set_name (
  const std::string & var_name,
  KnowledgeBase & knowledge)
{
  KnowledgeUpdateSettings keep_local (true);
  context_ = &(knowledge.get_context ());

  ContextGuard context_guard (*context_);
  Guard guard (mutex_);

  name_ = var_name;
  
  this->count_.set_name (var_name + ".count", knowledge);
  this->head_.set_name (var_name + ".head", knowledge);
  this->tail_.set_name (var_name + ".tail", knowledge);
  this->queue_.set_name (var_name, knowledge);
}

void
madara::knowledge::containers::Queue::set_name (
  const std::string & var_name,
  Variables & knowledge)
{
  KnowledgeUpdateSettings keep_local (true);
  context_ = knowledge.get_context ();

  ContextGuard context_guard (*context_);
  Guard guard (mutex_);

  name_ = var_name;
  
  this->count_.set_name (var_name + ".count", knowledge);
  this->head_.set_name (var_name + ".head", knowledge);
  this->tail_.set_name (var_name + ".tail", knowledge);
  this->queue_.set_name (var_name, knowledge);
}

size_t
madara::knowledge::containers::Queue::size (void)
{
  return queue_.size ();
}

size_t
madara::knowledge::containers::Queue::count (void)
{
  return (size_t)(*count_);
}

void
madara::knowledge::containers::Queue::resize (int size)
{
  if (context_)
  {
    ContextGuard context_guard (*context_);
    Guard guard (mutex_);

    queue_.resize (size, false);

    if (count_ > (KnowledgeRecord::Integer)queue_.size ())
    {
      count_ = (KnowledgeRecord::Integer)size;
    }
  }
}

madara::knowledge::KnowledgeUpdateSettings
madara::knowledge::containers::Queue::set_settings (
  const KnowledgeUpdateSettings & settings)
{
  Guard guard (mutex_);
  
  KnowledgeUpdateSettings old_settings = settings_;

  settings_ = settings;

  return old_settings;
}

void
madara::knowledge::containers::Queue::set_quality (
  uint32_t quality,
  const KnowledgeReferenceSettings & settings)
{
  if (context_)
  {
    ContextGuard context_guard (*context_);
    Guard guard (mutex_);
    context_->set_quality (name_, quality, true, settings);
  }
}
