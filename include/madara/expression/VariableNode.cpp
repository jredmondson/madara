
#ifndef _MADARA_NO_KARL_
#include "madara/expression/Visitor.h"
#include "madara/expression/VariableNode.h"
#include "madara/utility/Utility.h"


#include <string>
#include <sstream>

madara::expression::VariableNode::VariableNode (
  const std::string &key,
  madara::knowledge::ThreadSafeContext &context)
: ComponentNode (context.get_logger ()), key_ (key), record_ (0),
  context_ (context), key_expansion_necessary_ (false)
{
  // this key requires expansion. We do the compilation and error checking here
  // as the key shouldn't change, and this allows us to only have to do this
  // once
  if (key.find ("{") != key.npos)
  {
    madara_logger_ptr_log (logger_, logger::LOG_DETAILED,
      "Variable %s requires variable expansion.\n",
      key.c_str ());

    unsigned int count = 1;
    key_expansion_necessary_ = true;
    splitters_.push_back ("{");
    splitters_.push_back ("}");

    utility::tokenizer (key, splitters_, tokens_, pivot_list_);

    if (pivot_list_.size () % 2 != 0)
    {
      madara_logger_ptr_log (logger_, logger::LOG_EMERGENCY,
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
      madara_logger_ptr_log (logger_, logger::LOG_EMERGENCY,
        "KARL COMPILE ERROR: " \
        "Variable name has more opening braces than closing in %s\n",
        key.c_str ());

      exit (-1);
    }
    else if (num_closes > num_opens)
    {
      madara_logger_ptr_log (logger_, logger::LOG_EMERGENCY,
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

madara::expression::VariableNode::~VariableNode ()
{
  // do not clean up record_. Let the context clean that up.
}

std::string
madara::expression::VariableNode::expand_key (void) const
{
  if (key_expansion_necessary_)
  {
    madara_logger_ptr_log (logger_, logger::LOG_DETAILED,
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
madara::expression::VariableNode::accept (Visitor &visitor) const
{
  visitor.visit (*this);
}

madara::knowledge::KnowledgeRecord
madara::expression::VariableNode::item () const
{
  if (record_)
    return *record_;
  else
    return context_.get (expand_key ());
}

/// Prune the tree of unnecessary nodes. 
/// Returns evaluation of the node and sets can_change appropriately.
/// if this node can be changed, that means it shouldn't be pruned.
madara::knowledge::KnowledgeRecord
madara::expression::VariableNode::prune (bool & can_change)
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
madara::knowledge::KnowledgeRecord 
madara::expression::VariableNode::evaluate (
  const madara::knowledge::KnowledgeUpdateSettings & settings)
{
  // we could call item(), but since it is virtual, it incurs unnecessary
  // overhead.
  if (record_)
    return *record_;
  else
    return context_.get (expand_key ());
}

const std::string &
madara::expression::VariableNode::key () const
{
  return key_;
}

int
madara::expression::VariableNode::set (
  const madara::knowledge::KnowledgeRecord & value,
  const madara::knowledge::KnowledgeUpdateSettings & settings)
{
  int result = 0;

  knowledge::KnowledgeRecord * record = record_;

  madara_logger_ptr_log (logger_, logger::LOG_MINOR,
    "VariableNode::set: "
    "Attempting to set variable %s to a KnowledgeRecord parameter (%s).\n",
    key_.c_str (), value.to_string ().c_str ());

  if (!record)
  {
    record = context_.get_record (this->expand_key ());
  }

  if (record)
  {
    // notice that we assume the context is locked
    // check if we have the appropriate write quality
    if (!settings.always_overwrite && record->write_quality < record->quality)
    {
      result = -2;
    }
    else
    {
      // cheaper to read than write, so check to see if
      // we actually need to update quality and status
      if (record->write_quality != record->quality)
        record->quality = record->write_quality;

      madara_logger_ptr_log (logger_, logger::LOG_MINOR,
        "VariableNode::set: "
        "Setting variable %s with KnowledgeRecord assignment operator.\n",
        key_.c_str ());

      *record = value;

      context_.mark_and_signal (key_.c_str (), record);
    }
  }

  return result;
}

int
madara::expression::VariableNode::set (const madara::knowledge::KnowledgeRecord::Integer & value,
  const madara::knowledge::KnowledgeUpdateSettings & settings)
{
  madara_logger_ptr_log (logger_, logger::LOG_MINOR,
    "VariableNode::set: "
    "Attempting to set variable %s to an Integer parameter (%d).\n",
    key_.c_str (), (int)value);

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

    context_.mark_and_signal (key_.c_str(), record_);
  
    return 0;
  }
  else
    return context_.set (expand_key (), value, settings);
}

int
madara::expression::VariableNode::set (double value,
  const madara::knowledge::KnowledgeUpdateSettings & settings)
{
  madara_logger_ptr_log (logger_, logger::LOG_MINOR,
    "VariableNode::set: "
    "Attempting to set variable %s to a double parameter (%f).\n",
    key_.c_str (), value);

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

    context_.mark_and_signal (key_.c_str(), record_);
  
    return 0;
  }
  else
    return context_.set (expand_key (), value, settings);
}

int
madara::expression::VariableNode::set (const std::string & value,
  const madara::knowledge::KnowledgeUpdateSettings & settings)
{
  madara_logger_ptr_log (logger_, logger::LOG_MINOR,
    "VariableNode::set: "
    "Attempting to set variable %s to a string parameter (%s).\n",
    key_.c_str (), value.c_str ());

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

    context_.mark_and_signal (key_.c_str(), record_);
  
    return 0;
  }
  else
    return context_.set (expand_key (), value, settings);
}

madara::knowledge::KnowledgeRecord 
madara::expression::VariableNode::dec (
  const madara::knowledge::KnowledgeUpdateSettings & settings)
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

    context_.mark_and_signal (key_.c_str(), record_);
  
    return *record_;
  }
  else
    return context_.dec (expand_key (), settings);
}

madara::knowledge::KnowledgeRecord 
madara::expression::VariableNode::inc (
  const madara::knowledge::KnowledgeUpdateSettings & settings)
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

    context_.mark_and_signal (key_.c_str(), record_);
  
    return *record_;
  }
  else
    return context_.inc (expand_key (), settings);
}

#endif // _MADARA_NO_KARL_
