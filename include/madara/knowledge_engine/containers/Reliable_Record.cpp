#include "Reliable_Record.h"
#include "madara/transport/Fragmentation.h"
#include "madara/knowledge_engine/Context_Guard.h"

#define FRAGMENT_LIMIT 50000

Madara::Knowledge_Engine::Containers::Reliable_Record::Reliable_Record (
  const Knowledge_Update_Settings & settings)
: context_ (0),
  id_ (0), processes_ (2),
  settings_ (settings)
{
}
   

Madara::Knowledge_Engine::Containers::Reliable_Record::Reliable_Record (
  const std::string & name,
  Knowledge_Base & knowledge,
  const Knowledge_Update_Settings & settings)
  : context_ (&(knowledge.get_context ())), name_ (name),
  id_ (0), processes_ (2), settings_ (settings)
{
  source_ = knowledge.get_ref (name, settings);
  fragments_.set_name (name_ + ".frags", *context_);
  sync ();
}

Madara::Knowledge_Engine::Containers::Reliable_Record::Reliable_Record (
  const Reliable_Record & rhs)
  : context_ (rhs.context_),
    name_ (rhs.name_),
    id_ (rhs.id_), processes_ (rhs.processes_),
    fragments_ (rhs.fragments_), acks_ (rhs.acks_),
    settings_ (rhs.settings_)
{

}


Madara::Knowledge_Engine::Containers::Reliable_Record::~Reliable_Record ()
{

}

void
Madara::Knowledge_Engine::Containers::Reliable_Record::operator= (const Reliable_Record & rhs)
{
  if (this != &rhs)
  {
    Guard guard (mutex_), guard2 (rhs.mutex_);

    this->context_ = rhs.context_;
    this->name_ = rhs.name_;
    this->id_ = rhs.id_;
    this->processes_ = rhs.processes_;
    this->fragments_ = rhs.fragments_;
    this->acks_ = rhs.acks_;
    this->settings_ = rhs.settings_;
  }
}

std::string
Madara::Knowledge_Engine::Containers::Reliable_Record::get_name (void) const
{
  Guard guard (mutex_);
  return name_;
}

void
Madara::Knowledge_Engine::Containers::Reliable_Record::set_name (
  const std::string & var_name,
  Knowledge_Base & knowledge)
{
  context_ = &(knowledge.get_context ());

  Context_Guard context_guard (knowledge);
  Guard guard (mutex_);

  name_ = var_name;
  source_ = knowledge.get_ref (name_, settings_);
  fragments_.set_name (name_ + ".frags", *context_);

  sync ();
}

bool
Madara::Knowledge_Engine::Containers::Reliable_Record::operator== (
  const Reliable_Record & value) const
{
  Guard guard (mutex_);
  
  return source_ == source_;
}

bool
Madara::Knowledge_Engine::Containers::Reliable_Record::operator != (
  const Reliable_Record & value) const
{
  Guard guard (mutex_);

  return source_ != source_;
}

size_t
Madara::Knowledge_Engine::Containers::Reliable_Record::get_id (void)
{
  Guard guard (mutex_);

  return id_;
}

size_t
Madara::Knowledge_Engine::Containers::Reliable_Record::get_processes (void)
{
  Guard guard (mutex_);

  return processes_;
}

bool
Madara::Knowledge_Engine::Containers::Reliable_Record::is_done (void) const
{
  bool result (true);
  if (context_ && name_ != "")
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    for (; current_fragment_ < fragments_.size (); ++current_fragment_)
    {
      const Integer_Vector & acks = acks_[current_fragment_];

      for (; current_ack_ < acks.size (); ++current_ack_)
      {
        // if we do not have an acknowledgement, keep track of this position
        if (acks[current_ack_] == 0)
        {
          result = false;
          break;
        }
      }

      // if we broke out of inner loop, break out of outer loop 
      if (!result)
        break;
    }
  }
  return result;
}

void
Madara::Knowledge_Engine::Containers::Reliable_Record::next (void)
{
  if (context_ && name_ != "")
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    // check to see if we're done. If not, modify the next fragment
    if (!is_done ())
      fragments_.modify (current_fragment_);
  }
}

void
Madara::Knowledge_Engine::Containers::Reliable_Record::sync (void)
{
  if (context_ && name_ != "")
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    Knowledge_Record current_source = context_->get (source_);

    if (current_source.get_encoded_size (name_) < FRAGMENT_LIMIT)
    {
      fragments_.resize (1, true);

      fragments_.set (0, current_source);
    }
    else
    {

      uint32_t element_size = 1;
      int32_t type = current_source.type ();

      if (type == Knowledge_Record::INTEGER_ARRAY)
      {
        element_size = sizeof (Knowledge_Record::Integer);
      }
      else if (type == Knowledge_Record::DOUBLE_ARRAY)
      {
        element_size = sizeof (double);
      }

      uint32_t actual_size = (current_source.size () * element_size);

      uint32_t num_fragments (actual_size / FRAGMENT_LIMIT);
      uint32_t elements_per_fragment (FRAGMENT_LIMIT / element_size);

      if (actual_size % FRAGMENT_LIMIT != 0)
        ++num_fragments;

      fragments_.resize ((int)num_fragments, true);
      
      unsigned int first = 0;
      unsigned int last = first + elements_per_fragment - 1;

      for (size_t i = 0; i < fragments_.size (); ++i)
      {
        fragments_.set ((unsigned int)i, current_source.fragment (first, last));

        first = last + 1;
        last = first + elements_per_fragment;

        acks_.resize (num_fragments);

        for (size_t j = 0; j < processes_; ++j)
        {
          std::stringstream buffer;
          buffer << name_;
          buffer << ".frags.";
          buffer << i;
          buffer << ".";
          buffer << "acks";
          acks_[i].set_name (buffer.str (), *context_, (int)processes_);
        }
      }
    }

    current_fragment_ = 0;
    current_ack_ = 0;
  }
}

bool
  Madara::Knowledge_Engine::Containers::Reliable_Record::merge (void)
{
  bool result (false);

  fragments_.resize (-1, false);



  return result;
}

Madara::Knowledge_Engine::Knowledge_Update_Settings
Madara::Knowledge_Engine::Containers::Reliable_Record::set_settings (
  const Knowledge_Update_Settings & settings)
{
  Guard guard (mutex_);
  
  Knowledge_Update_Settings old_settings = settings_;

  settings_ = settings;

  return old_settings;
}

void
Madara::Knowledge_Engine::Containers::Reliable_Record::set_quality (
  uint32_t quality,
  const Knowledge_Reference_Settings & settings)
{
  Guard guard (mutex_);

  if (context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);
    context_->set_quality (name_, quality, true, settings);
  }
}

void
Madara::Knowledge_Engine::Containers::Reliable_Record::resize (size_t id, size_t participants)
{
  if (context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    id_ = id;
    processes_ = participants;

    sync ();
  }
}
