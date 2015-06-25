
#ifndef _MADARA_NO_KARL_
#include "madara/expression_tree/Visitor.h"
#include "madara/expression_tree/Variable_Node.h"
#include "madara/utility/Utility.h"


#include <string>
#include <sstream>

Madara::Expression_Tree::Variable_Node::Variable_Node (
  const std::string &key,
  Madara::Knowledge_Engine::Thread_Safe_Context &context)
: Component_Node (context.get_logger ()), key_ (key), record_ (0),
  context_ (context), key_expansion_necessary_ (false)
{
  // this key requires expansion. We do the compilation and error checking here
  // as the key shouldn't change, and this allows us to only have to do this
  // once
  if (key.find ("{") != key.npos)
  {
    madara_logger_ptr_log (logger_, Logger::LOG_DETAILED,
      "Variable %s requires variable expansion.\n",
      key.c_str ());

    unsigned int count = 1;
    key_expansion_necessary_ = true;
    splitters_.push_back ("{");
    splitters_.push_back ("}");

    Utility::tokenizer (key, splitters_, tokens_, pivot_list_);

    if (pivot_list_.size () % 2 != 0)
    {
      madara_logger_ptr_log (logger_, Logger::LOG_EMERGENCY,
        "KARL COMPILE ERROR: matching braces not found in %s\n",
        key.c_str ());

      exit (-1);
    }

    // check for braces that are not properly closed
    std::vector<std::string>::const_iterator pivot = pivot_list_.begin ();
    unsigned int num_opens = 0;
    unsigned int num_closes = 0;

    for (; pivot != pivot_list_.end (); ++pivot)
    {
      if (*pivot == "{")
      {
        ++num_opens;
      }
      else if (*pivot == "}")
      {
        ++num_closes;
      }
    }

    if (num_opens > num_closes)
    {
      madara_logger_ptr_log (logger_, Logger::LOG_EMERGENCY,
        "KARL COMPILE ERROR: " \
        "Variable name has more opening braces than closing in %s\n",
        key.c_str ());

      exit (-1);
    }
    else if (num_closes > num_opens)
    {
      madara_logger_ptr_log (logger_, Logger::LOG_EMERGENCY,
        "KARL COMPILE ERROR: " \
        "Variable name has more closing braces than opening in %s\n",
        key.c_str ());
      exit (-1);
    }
  }
  // no variable expansion necessary. Create a hard link to the record_->
  // this will save us lots of clock cycles each variable access or
  // mutation.
  else
  {
    record_ = context_.get_record (key);
  }
}

Madara::Expression_Tree::Variable_Node::~Variable_Node ()
{
  // do not clean up record_. Let the context clean that up.
}

std::string
Madara::Expression_Tree::Variable_Node::expand_key (void) const
{
  if (key_expansion_necessary_)
  {
    madara_logger_ptr_log (logger_, Logger::LOG_DETAILED,
      "Variable %s requires variable expansion.\n",
      key_.c_str ());

    unsigned int count = 0;

    // add the first token into a string builder
    std::stringstream builder;
    std::vector< std::string>::const_iterator token = tokens_.begin ();
    builder << *token;

    // move the token to the next in the list.
    for (++token, ++count; token != tokens_.end (); ++token, ++count)
    {
      if (*token != "")
      {
        // is the current token a variable to lookup?
        if (count < pivot_list_.size () 
          && pivot_list_[count] == "}")
        {
          builder << context_.get_record (*token)->to_string ();
        }
        else
        {
          builder << *token;
        }
      }
    }

    return builder.str ();
  }
  // if there was no need to expand the key, just return
  // the key
  else
    return key_;
}


void
Madara::Expression_Tree::Variable_Node::accept (Visitor &visitor) const
{
  visitor.visit (*this);
}

Madara::Knowledge_Record
Madara::Expression_Tree::Variable_Node::item () const
{
  if (record_)
    return *record_;
  else
    return context_.get (expand_key ());
}

/// Prune the tree of unnecessary nodes. 
/// Returns evaluation of the node and sets can_change appropriately.
/// if this node can be changed, that means it shouldn't be pruned.
Madara::Knowledge_Record
Madara::Expression_Tree::Variable_Node::prune (bool & can_change)
{
  // a variable is one of very few nodes that can change over time and
  // cannot be pruned
  can_change = true;

  // we could call item(), but since it is virtual, it incurs unnecessary
  // overhead.
  if (record_)
    return *record_;
  else
    return context_.get (expand_key ());
}

/// Evaluates the node and its children. This does not prune any of
/// the expression tree, and is much faster than the prune function
Madara::Knowledge_Record 
Madara::Expression_Tree::Variable_Node::evaluate (
  const Madara::Knowledge_Engine::Knowledge_Update_Settings & settings)
{
  // we could call item(), but since it is virtual, it incurs unnecessary
  // overhead.
  if (record_)
    return *record_;
  else
    return context_.get (expand_key ());
}

const std::string &
Madara::Expression_Tree::Variable_Node::key () const
{
  return key_;
}

int
Madara::Expression_Tree::Variable_Node::set (
  const Madara::Knowledge_Record & value,
  const Madara::Knowledge_Engine::Knowledge_Update_Settings & settings)
{
  if (record_)
  {
    // notice that we assume the context is locked
    // check if we have the appropriate write quality
    if (!settings.always_overwrite && record_->write_quality < record_->quality)
      return -2;

    // cheaper to read than write, so check to see if
    // we actually need to update quality and status
    if (record_->write_quality != record_->quality)
      record_->quality = record_->write_quality;

    *record_ = value;
    
    if (key_[0] != '.' && !settings.treat_globals_as_locals)
    {
      context_.mark_modified (key_, *record_,
        Knowledge_Engine::Knowledge_Reference_Settings (false));
    }
    else if (settings.track_local_changes)
    {
      context_.mark_local_modified (key_, *record_,
        Knowledge_Engine::Knowledge_Reference_Settings (false));
    }
  
    return 0;
  }
  else
  {
    if       (value.type () == Madara::Knowledge_Record::INTEGER)
      return context_.set (expand_key (), value.to_integer ());
    else if  (value.type () == Madara::Knowledge_Record::DOUBLE)
      return context_.set (expand_key (), value.to_double ());
    else if  (value.type () == Madara::Knowledge_Record::STRING)
      return context_.set (expand_key (), value.to_string ());
    else
      return -5;
  }
}

int
Madara::Expression_Tree::Variable_Node::set (const Madara::Knowledge_Record::Integer & value,
  const Madara::Knowledge_Engine::Knowledge_Update_Settings & settings)
{
  if (record_)
  {
    // notice that we assume the context is locked
    // check if we have the appropriate write quality
    if (!settings.always_overwrite && record_->write_quality < record_->quality)
      return -2;

    // cheaper to read than write, so check to see if
    // we actually need to update quality and status
    if (record_->write_quality != record_->quality)
      record_->quality = record_->write_quality;

    record_->set_value (value);

    if (key_[0] != '.' && !settings.treat_globals_as_locals)
    {
      context_.mark_modified (key_, *record_,
        Knowledge_Engine::Knowledge_Reference_Settings (false));
    }
    else if (settings.track_local_changes)
    {
      context_.mark_local_modified (key_, *record_,
        Knowledge_Engine::Knowledge_Reference_Settings (false));
    }
  
    return 0;
  }
  else
    return context_.set (expand_key (), value, settings);
}

int
Madara::Expression_Tree::Variable_Node::set (double value,
  const Madara::Knowledge_Engine::Knowledge_Update_Settings & settings)
{
  if (record_)
  {
    // notice that we assume the context is locked
    // check if we have the appropriate write quality
    if (!settings.always_overwrite && record_->write_quality < record_->quality)
      return -2;

    // cheaper to read than write, so check to see if
    // we actually need to update quality and status
    if (record_->write_quality != record_->quality)
      record_->quality = record_->write_quality;

    record_->set_value (value);

    if (key_[0] != '.' && !settings.treat_globals_as_locals)
    {
      context_.mark_modified (key_, *record_,
        Knowledge_Engine::Knowledge_Reference_Settings (false));
    }
    else if (settings.track_local_changes)
    {
      context_.mark_local_modified (key_, *record_,
        Knowledge_Engine::Knowledge_Reference_Settings (false));
    }
  
    return 0;
  }
  else
    return context_.set (expand_key (), value, settings);
}

int
Madara::Expression_Tree::Variable_Node::set (const std::string & value,
  const Madara::Knowledge_Engine::Knowledge_Update_Settings & settings)
{
  if (record_)
  {
    // notice that we assume the context is locked
    // check if we have the appropriate write quality
    if (!settings.always_overwrite && record_->write_quality < record_->quality)
      return -2;

    // cheaper to read than write, so check to see if
    // we actually need to update quality and status
    if (record_->write_quality != record_->quality)
      record_->quality = record_->write_quality;

    record_->set_value (value);

    if (key_[0] != '.' && !settings.treat_globals_as_locals)
    {
      context_.mark_modified (key_, *record_,
        Knowledge_Engine::Knowledge_Reference_Settings (false));
    }
    else if (settings.track_local_changes)
    {
      context_.mark_local_modified (key_, *record_,
        Knowledge_Engine::Knowledge_Reference_Settings (false));
    }
  
    return 0;
  }
  else
    return context_.set (expand_key (), value, settings);
}

Madara::Knowledge_Record 
Madara::Expression_Tree::Variable_Node::dec (
  const Madara::Knowledge_Engine::Knowledge_Update_Settings & settings)
{
  if (record_)
  {
    // notice that we assume the context is locked
    // check if we have the appropriate write quality
    if (!settings.always_overwrite && record_->write_quality < record_->quality)
      return *record_;

    // cheaper to read than write, so check to see if
    // we actually need to update quality and status
    if (record_->write_quality != record_->quality)
      record_->quality = record_->write_quality;

    --(*record_);

    if (key_[0] != '.' && !settings.treat_globals_as_locals)
    {
      context_.mark_modified (key_, *record_,
        Knowledge_Engine::Knowledge_Reference_Settings (false));
    }
    else if (settings.track_local_changes)
    {
      context_.mark_local_modified (key_, *record_,
        Knowledge_Engine::Knowledge_Reference_Settings (false));
    }
  
    return *record_;
  }
  else
    return context_.dec (expand_key (), settings);
}

Madara::Knowledge_Record 
Madara::Expression_Tree::Variable_Node::inc (
  const Madara::Knowledge_Engine::Knowledge_Update_Settings & settings)
{
  if (record_)
  {
    // notice that we assume the context is locked
    // check if we have the appropriate write quality
    if (!settings.always_overwrite && record_->write_quality < record_->quality)
      return *record_;

    // cheaper to read than write, so check to see if
    // we actually need to update quality and status
    if (record_->write_quality != record_->quality)
      record_->quality = record_->write_quality;

    ++(*record_);

    if (key_[0] != '.' && !settings.treat_globals_as_locals)
    {
      context_.mark_modified (key_, *record_,
        Knowledge_Engine::Knowledge_Reference_Settings (false));
    }
    else if (settings.track_local_changes)
    {
      context_.mark_local_modified (key_, *record_,
        Knowledge_Engine::Knowledge_Reference_Settings (false));
    }
  
    return *record_;
  }
  else
    return context_.inc (expand_key (), settings);
}

#endif // _MADARA_NO_KARL_
