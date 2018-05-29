
#ifndef _MADARA_NO_KARL_

#include "madara/expression/Visitor.h"
#include "madara/expression/CompositeArrayReference.h"
#include "madara/utility/Utility.h"
#include "VariableExpander.h"


#include <string>
#include <sstream>

madara::expression::CompositeArrayReference::CompositeArrayReference (
  const std::string & key, ComponentNode * index,
  madara::knowledge::ThreadSafeContext &context)
: CompositeUnaryNode (context.get_logger (), index),
  context_ (context),
  key_ (key),
  key_expansion_necessary_ (false)
{
  VariableExpander expander;
  ref_ = expander.expand (key, "CompositeArrayReference", context, logger_,
    key_expansion_necessary_, splitters_, tokens_, pivot_list_);
}

std::string
madara::expression::CompositeArrayReference::expand_key (void) const
{
  if (key_expansion_necessary_)
  {
    madara_logger_ptr_log (logger_, logger::LOG_DETAILED,
      "Variable %s requires variable expansion\n",
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
madara::expression::CompositeArrayReference::accept (Visitor &visitor) const
{
  visitor.visit (*this);
}

madara::knowledge::KnowledgeRecord
madara::expression::CompositeArrayReference::item () const
{
  size_t index = right_->item ().to_integer ();

  if (ref_.is_valid())
  {
    return ref_.get_record_unsafe()->retrieve_index (index);
  }
  else
    return context_.get (expand_key ()).retrieve_index (index);
}

/// Prune the tree of unnecessary nodes. 
/// Returns evaluation of the node and sets can_change appropriately.
/// if this node can be changed, that means it shouldn't be pruned.
madara::knowledge::KnowledgeRecord
madara::expression::CompositeArrayReference::prune (bool & can_change)
{
  // a variable is one of very few nodes that can change over time and
  // cannot be pruned
  can_change = true;

  std::string key = expand_key ();

  if (key == "nan")
  {
    madara_logger_ptr_log (logger_, logger::LOG_DETAILED,
      "CompositeArrayReference::prune: "
      "KARL COMPILE ERROR:"
      "Reserved word 'nan' used as an array reference.\n",
      key_.c_str ());

    //return new LeafNode (context_.get_logger (), NAN);
    throw KarlException ("madara::expression::ComponentNode: "
      "KARL COMPILE ERROR: "
      "Reserved word 'nan' used as an array reference.");
  }
  else if (key == "inf")
  {
    madara_logger_ptr_log (logger_, logger::LOG_DETAILED,
      "CompositeArrayReference::prune: "
      "KARL COMPILE ERROR:"
      "Reserved word 'inf' used as an array reference.\n",
      key_.c_str ());

    //return new LeafNode (context_.get_logger (), NAN);
    throw KarlException ("madara::expression::ComponentNode: "
      "KARL COMPILE ERROR: "
      "Reserved word 'inf' used as an array reference.");
  }

  // we could call item(), but since it is virtual, it incurs unnecessary
  // overhead.
  if (ref_.is_valid())
    return *ref_.get_record_unsafe();
  else
    return context_.get (expand_key ());
}

/// Evaluates the node and its children. This does not prune any of
/// the expression tree, and is much faster than the prune function
madara::knowledge::KnowledgeRecord 
madara::expression::CompositeArrayReference::evaluate (
  const madara::knowledge::KnowledgeUpdateSettings & settings)
{
  size_t index = right_->evaluate (settings).to_integer ();

  if (ref_.is_valid())
  {
    return ref_.get_record_unsafe()->retrieve_index (index);
  }
  else
    return context_.get (expand_key ()).retrieve_index (index);
}

const std::string &
madara::expression::CompositeArrayReference::key () const
{
  return key_;
}


/// Sets the value stored in the node.
madara::knowledge::KnowledgeRecord
madara::expression::CompositeArrayReference::dec (
  const madara::knowledge::KnowledgeUpdateSettings & settings)
{
  size_t index = size_t (right_->evaluate (settings).to_integer ());

  if (ref_.is_valid())
  {
    auto record = ref_.get_record_unsafe();

    // notice that we assume the context is locked
    // check if we have the appropriate write quality
    if (!settings.always_overwrite && record->write_quality < record->quality)
      return context_.retrieve_index (expand_key (), index, settings);

    // cheaper to read than write, so check to see if
    // we actually need to update quality and status
    if (record->write_quality != record->quality)
      record->quality = record->write_quality;

    knowledge::KnowledgeRecord result (record->dec_index (index));

    context_.mark_and_signal (ref_);

    return result;
  }
  else
  {
    knowledge::KnowledgeRecord result (
      context_.retrieve_index (expand_key (), index, settings) - knowledge::KnowledgeRecord (1));

    if (result.type () == knowledge::KnowledgeRecord::INTEGER)
      context_.set_index (expand_key (), index, result.to_integer (), settings);
    else if (result.type () == knowledge::KnowledgeRecord::DOUBLE)
      context_.set_index (expand_key (), index, result.to_double (), settings);

    return result;
  }
}
      
/// Sets the value stored in the node.
madara::knowledge::KnowledgeRecord
madara::expression::CompositeArrayReference::inc (
  const madara::knowledge::KnowledgeUpdateSettings & settings)
{
  size_t index = size_t (right_->evaluate (settings).to_integer ());

  if (ref_.is_valid())
  {
    auto record = ref_.get_record_unsafe();

    // notice that we assume the context is locked
    // check if we have the appropriate write quality
    if (!settings.always_overwrite && record->write_quality < record->quality)
      return context_.retrieve_index (expand_key (), index, settings);

    // cheaper to read than write, so check to see if
    // we actually need to update quality and status
    if (record->write_quality != record->quality)
      record->quality = record->write_quality;

    knowledge::KnowledgeRecord result (record->inc_index (index));

    context_.mark_and_signal (ref_);

    return result;
  }
  else
  {
    knowledge::KnowledgeRecord result = 
      context_.retrieve_index (expand_key (), index, settings)
        + knowledge::KnowledgeRecord (1);

    if (result.type () == knowledge::KnowledgeRecord::INTEGER)
      context_.set_index (expand_key (), index, result.to_integer (), settings);
    else if (result.type () == knowledge::KnowledgeRecord::DOUBLE)
      context_.set_index (expand_key (), index, result.to_double (), settings);

    return result;
  }
}

int
madara::expression::CompositeArrayReference::set (
  const madara::knowledge::KnowledgeRecord & value,
  const madara::knowledge::KnowledgeUpdateSettings & settings)
{
  int return_value = 0;

  if (value.type () == knowledge::KnowledgeRecord::INTEGER)
    return_value = set (value.to_integer (), settings);
  else if (value.type () == knowledge::KnowledgeRecord::DOUBLE)
    return_value = set (value.to_double (), settings);

  return return_value;
}

int
madara::expression::CompositeArrayReference::set (const madara::knowledge::KnowledgeRecord::Integer & value,
  const madara::knowledge::KnowledgeUpdateSettings & settings)
{
  size_t index = size_t (right_->evaluate (settings).to_integer ());

  if (ref_.is_valid())
  {
    auto record = ref_.get_record_unsafe();

    // notice that we assume the context is locked
    // check if we have the appropriate write quality
    if (!settings.always_overwrite && record->write_quality < record->quality)
      return -2;

    // cheaper to read than write, so check to see if
    // we actually need to update quality and status
    if (record->write_quality != record->quality)
      record->quality = record->write_quality;

    record->set_index (index, value);

    context_.mark_and_signal (ref_);

    return 0;
  }
  else
    return context_.set_index (expand_key (), index, value, settings);
}

int
madara::expression::CompositeArrayReference::set (double value,
  const madara::knowledge::KnowledgeUpdateSettings & settings)
{
  size_t index = size_t (right_->evaluate (settings).to_integer ());

  if (ref_.is_valid())
  {
    auto record = ref_.get_record_unsafe();

    // notice that we assume the context is locked
    // check if we have the appropriate write quality
    if (!settings.always_overwrite && record->write_quality < record->quality)
      return -2;

    // cheaper to read than write, so check to see if
    // we actually need to update quality and status
    if (record->write_quality != record->quality)
      record->quality = record->write_quality;
    
    record->set_index (index, value);

    context_.mark_and_signal (ref_);
  
    return 0;
  }
  else
    return context_.set_index (expand_key (), index, value, settings);
}

#endif // _MADARA_NO_KARL_
