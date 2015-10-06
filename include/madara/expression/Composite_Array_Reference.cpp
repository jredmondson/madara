
#ifndef _MADARA_NO_KARL_

#include "madara/expression/Visitor.h"
#include "madara/expression/Composite_Array_Reference.h"
#include "madara/utility/Utility.h"


#include <string>
#include <sstream>

madara::expression::Composite_Array_Reference::Composite_Array_Reference (
  const std::string & key, Component_Node * index,
  madara::knowledge::Thread_Safe_Context &context)
: Composite_Unary_Node (context.get_logger (), index),
  key_ (key), record_ (0),
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
        "KARL COMPILE ERROR: Array name has "
        "more opening braces than closing in %s\n",
        key.c_str ());

      exit (-1);
    }
    else if (num_closes > num_opens)
    {
      madara_logger_ptr_log (logger_, logger::LOG_EMERGENCY,
        "KARL COMPILE ERROR: Array name has "
        "more closing braces than opening in %s\n",
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

madara::expression::Composite_Array_Reference::~Composite_Array_Reference ()
{
  // do not clean up record_. Let the context clean that up.
}

std::string
madara::expression::Composite_Array_Reference::expand_key (void) const
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
madara::expression::Composite_Array_Reference::accept (Visitor &visitor) const
{
  visitor.visit (*this);
}

madara::Knowledge_Record
madara::expression::Composite_Array_Reference::item () const
{
  size_t index = right_->item ().to_integer ();
  
  if (record_)
  {
    return record_->retrieve_index (index);
  }
  else
    return context_.get (expand_key ()).retrieve_index (index);
}

/// Prune the tree of unnecessary nodes. 
/// Returns evaluation of the node and sets can_change appropriately.
/// if this node can be changed, that means it shouldn't be pruned.
madara::Knowledge_Record
madara::expression::Composite_Array_Reference::prune (bool & can_change)
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
madara::Knowledge_Record 
madara::expression::Composite_Array_Reference::evaluate (
  const madara::knowledge::Knowledge_Update_Settings & settings)
{
  size_t index = right_->evaluate (settings).to_integer ();

  if (record_)
  {
    return record_->retrieve_index (index);
  }
  else
    return context_.get (expand_key ()).retrieve_index (index);
}

const std::string &
madara::expression::Composite_Array_Reference::key () const
{
  return key_;
}


/// Sets the value stored in the node.
madara::Knowledge_Record
madara::expression::Composite_Array_Reference::dec (
  const madara::knowledge::Knowledge_Update_Settings & settings)
{
  size_t index = size_t (right_->evaluate (settings).to_integer ());

  if (record_)
  {
    // notice that we assume the context is locked
    // check if we have the appropriate write quality
    if (!settings.always_overwrite && record_->write_quality < record_->quality)
      return context_.retrieve_index (expand_key (), index, settings);

    // cheaper to read than write, so check to see if
    // we actually need to update quality and status
    if (record_->write_quality != record_->quality)
      record_->quality = record_->write_quality;

    Knowledge_Record result (record_->dec_index (index));

    if (key_[0] != '.' && !settings.treat_globals_as_locals)
    {
      context_.mark_modified (key_, *record_,
        knowledge::Knowledge_Reference_Settings (false));
    }

    return result;
  }
  else
  {
    Knowledge_Record result = 
      context_.retrieve_index (expand_key (), index, settings) - Knowledge_Record::Integer (1);

    if (result.type () == Knowledge_Record::INTEGER)
      context_.set_index (expand_key (), index, result.to_integer (), settings);
    else if (result.type () == Knowledge_Record::DOUBLE)
      context_.set_index (expand_key (), index, result.to_double (), settings);

    return result;
  }
}
      
/// Sets the value stored in the node.
madara::Knowledge_Record
madara::expression::Composite_Array_Reference::inc (
  const madara::knowledge::Knowledge_Update_Settings & settings)
{
  size_t index = size_t (right_->evaluate (settings).to_integer ());

  if (record_)
  {
    // notice that we assume the context is locked
    // check if we have the appropriate write quality
    if (!settings.always_overwrite && record_->write_quality < record_->quality)
      return context_.retrieve_index (expand_key (), index, settings);

    // cheaper to read than write, so check to see if
    // we actually need to update quality and status
    if (record_->write_quality != record_->quality)
      record_->quality = record_->write_quality;

    Knowledge_Record result (record_->inc_index (index));

    if (key_[0] != '.' && !settings.treat_globals_as_locals)
    {
      context_.mark_modified (key_, *record_,
        knowledge::Knowledge_Reference_Settings (false));
    }

    return result;
  }
  else
  {
    Knowledge_Record result = 
      context_.retrieve_index (expand_key (), index, settings)
        + Knowledge_Record::Integer (1);

    if (result.type () == Knowledge_Record::INTEGER)
      context_.set_index (expand_key (), index, result.to_integer (), settings);
    else if (result.type () == Knowledge_Record::DOUBLE)
      context_.set_index (expand_key (), index, result.to_double (), settings);

    return result;
  }
}

int
madara::expression::Composite_Array_Reference::set (
  const madara::Knowledge_Record & value,
  const madara::knowledge::Knowledge_Update_Settings & settings)
{
  int return_value = 0;

  if (value.type () == Knowledge_Record::INTEGER)
    return_value = set (value.to_integer (), settings);
  else if (value.type () == Knowledge_Record::DOUBLE)
    return_value = set (value.to_double (), settings);

  return return_value;
}

int
madara::expression::Composite_Array_Reference::set (const madara::Knowledge_Record::Integer & value,
  const madara::knowledge::Knowledge_Update_Settings & settings)
{
  size_t index = size_t (right_->evaluate (settings).to_integer ());

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

    record_->set_index (index, value);

    if (key_[0] != '.' && !settings.treat_globals_as_locals)
    {
      context_.mark_modified (key_, *record_,
        knowledge::Knowledge_Reference_Settings (false));
    }
  
    return 0;
  }
  else
    return context_.set_index (expand_key (), index, value, settings);
}

int
madara::expression::Composite_Array_Reference::set (double value,
  const madara::knowledge::Knowledge_Update_Settings & settings)
{
  size_t index = size_t (right_->evaluate (settings).to_integer ());

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
    
    record_->set_index (index, value);

    if (key_[0] != '.' && !settings.treat_globals_as_locals)
    {
      context_.mark_modified (key_, *record_,
        knowledge::Knowledge_Reference_Settings (false));
    }
  
    return 0;
  }
  else
    return context_.set_index (expand_key (), index, value, settings);
}

#endif // _MADARA_NO_KARL_
