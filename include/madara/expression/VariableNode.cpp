
#ifndef _MADARA_NO_KARL_
#include "madara/expression/Visitor.h"
#include "madara/expression/VariableNode.h"
#include "madara/utility/Utility.h"
#include "VariableExpander.h"
#include "madara/exceptions/UninitializedException.h"

#include <string>
#include <sstream>

madara::expression::VariableNode::VariableNode(
    const std::string& key, madara::knowledge::ThreadSafeContext& context)
  : ComponentNode(context.get_logger()),
    key_(key),
    context_(context),
    key_expansion_necessary_(false)
{
  // this key requires expansion. We do the compilation and error checking here
  // as the key shouldn't change, and this allows us to only have to do this
  // once
  if(key.find("{") != key.npos)
  {
    madara_logger_ptr_log(logger_, logger::LOG_DETAILED,
        "madara::expression::VariableNode: "
        "Variable %s requires variable expansion.\n",
        key.c_str());

    key_expansion_necessary_ = true;
    int num_opens = 0;

    for(size_t i = 0; i < key.size(); ++i)
    {
      if(key[i] == '{')
      {
        markers_.push_back(i);
        ++num_opens;
      }
      else if(key[i] == '}')
      {
        if(num_opens != 0)
        {
          markers_.push_back(i);
          --num_opens;
        }
        else
        {
          std::stringstream buffer;
          buffer << "madara::expression::VariableNode";
          buffer << ": KARL COMPILE ERROR: matching braces not found in ";
          buffer << key << "@" << i << "\n";
          madara_logger_ptr_log(
              logger_, logger::LOG_ERROR, buffer.str().c_str());

          throw exceptions::KarlException(buffer.str());
        }
      }
    }

    if(num_opens > 0)
    {
      std::stringstream buffer;
      buffer << "madara::expression::VariableNode: ";
      buffer << "KARL COMPILE ERROR: more opening braces than closers in ";
      buffer << key << "\n";
      madara_logger_ptr_log(logger_, logger::LOG_ERROR, buffer.str().c_str());

      throw exceptions::KarlException(buffer.str());
    }
    else if(num_opens < 0)
    {
      std::stringstream buffer;
      buffer << "madara::expression::VariableNode: ";
      buffer << "KARL COMPILE ERROR: more closing braces than openers in ";
      buffer << key << "\n";
      madara_logger_ptr_log(logger_, logger::LOG_ERROR, buffer.str().c_str());

      throw exceptions::KarlException(buffer.str());
    }
  }
  // no variable expansion necessary. Create a hard link to the ref_->
  // this will save us lots of clock cycles each variable access or
  // mutation.
  else
  {
    ref_ = context_.get_ref(key);
  }
}

std::string madara::expression::VariableNode::expand_opener(
    size_t opener, size_t& closer) const
{
  size_t i = opener + 1;
  size_t start = markers_[opener] + 1;

  std::stringstream builder;

  madara_logger_ptr_log(logger_, logger::LOG_DETAILED,
      "madara::expression::VariableNode:expand_opener: "
      "key=%s, opener_index=%d, start=%d.\n",
      key_.c_str(), (int)opener);

  for(; i < markers_.size(); ++i)
  {
    if(key_[markers_[i]] == '{')
    {
      // copy before opener (4)
      if(start < markers_[i])
      {
        builder << key_.substr(start, markers_[i] - start);
        madara_logger_ptr_log(logger_, logger::LOG_DETAILED,
            "madara::expression::VariableNode:expand_opener: "
            "%d-%d{: added %d chars.\n",
            (int)start, (int)markers_[i], (int)(markers_[i] - start));
      }

      size_t sub_opener = i;
      builder << expand_opener(i, i);
      madara_logger_ptr_log(logger_, logger::LOG_DETAILED,
          "madara::expression::VariableNode:expand_opener: "
          "get_record(expand_opener()) "
          "expand_opener(%d, %d).\n",
          (int)sub_opener, (int)i);

      // set next start to after [4]{[5]}*[3]
      start = markers_[i] + 1;
    }
    else if(key_[markers_[i]] == '}')
    {
      // main action of this function {[2]}
      closer = i;

      size_t end = markers_[closer];

      builder << *context_.get_record(key_.substr(start, end - start));
      madara_logger_ptr_log(logger_, logger::LOG_DETAILED,
          "madara::expression::VariableNode:expand_opener(%d,%d): "
          "{%d-%d}: added %d chars.\n",
          (int)opener, (int)closer, (int)start, (int)end, (int)(end - start));
      break;
    }
  }

  std::string result = builder.str();

  madara_logger_ptr_log(logger_, logger::LOG_DETAILED,
      "madara::expression::VariableNode:expand_opener(%d,%d): "
      "return %s\n",
      (int)opener, (int)closer, result.c_str());

  return result;
}

std::string madara::expression::VariableNode::expand_key(void) const
{
  if(key_expansion_necessary_)
  {
    madara_logger_ptr_log(logger_, logger::LOG_DETAILED,
        "madara::expression::VariableNode:expand_key: "
        "Variable %s requires variable expansion"
        " (%d markers).\n",
        key_.c_str(), (int)markers_.size());

    size_t i = 0;
    size_t start = 0;

    // add the first token into a string builder
    std::stringstream builder;

    for(; i < markers_.size(); ++i)
    {
      if(key_[markers_[i]] == '{')
      {
        // copy characters between expansions [1]{
        if(start < markers_[i])
        {
          builder << key_.substr(start, markers_[i] - start);
          madara_logger_ptr_log(logger_, logger::LOG_DETAILED,
              "madara::expression::VariableNode:expand_key: "
              "%d-%d{: added %d chars.\n",
              (int)start, (int)markers_[i], (int)(markers_[i] - start));
        }

        size_t opener = i;
        builder << expand_opener(i, i);
        madara_logger_ptr_log(logger_, logger::LOG_DETAILED,
            "madara::expression::VariableNode:expand_key: "
            "adding results of expand_opener(%d, %d).\n",
            (int)opener, (int)i);

        start = markers_[i] + 1;
      }
    }

    // handle characters trailing }[3]
    if(start < key_.size() && key_.size() - start > 0)
    {
      madara_logger_ptr_log(logger_, logger::LOG_DETAILED,
          "madara::expression::VariableNode:expand_key: "
          "substr add from index %d, %d chars.\n",
          (int)start, (int)key_.size() - start);
      builder << key_.substr(start, key_.size() - start);
    }

    std::string result = builder.str();

    madara_logger_ptr_log(logger_, logger::LOG_DETAILED,
        "madara::expression::VariableNode:expand_key: "
        "return %s\n",
        result.c_str());

    return result;
  }
  // if there was no need to expand the key, just return
  // the key
  else
    return key_;
}

void madara::expression::VariableNode::accept(Visitor& visitor) const
{
  visitor.visit(*this);
}

madara::knowledge::KnowledgeRecord madara::expression::VariableNode::item()
    const
{
  if(ref_.is_valid())
    return *ref_.get_record_unsafe();
  else
    return context_.get(expand_key());
}

/// Prune the tree of unnecessary nodes.
/// Returns evaluation of the node and sets can_change appropriately.
/// if this node can be changed, that means it shouldn't be pruned.
madara::knowledge::KnowledgeRecord madara::expression::VariableNode::prune(
    bool& can_change)
{
  // a variable is one of very few nodes that can change over time and
  // cannot be pruned
  can_change = true;

  // we could call item(), but since it is virtual, it incurs unnecessary
  // overhead.
  if(ref_.is_valid())
    return *ref_.get_record_unsafe();
  else
    return context_.get(expand_key());
}

/// Evaluates the node and its children.
madara::knowledge::KnowledgeRecord madara::expression::VariableNode::evaluate(
    const madara::knowledge::KnowledgeUpdateSettings& settings)
{
  madara_logger_ptr_log(logger_, logger::LOG_TRACE,
      "madara::expression::VariableNode::evaluate: "
      "Returning variable %s.\n",
      key_.c_str());

  if(ref_.is_valid())
  {
    auto ret = ref_.get_record_unsafe();

    if(settings.exception_on_unitialized && !ret->exists ())
    {
      std::stringstream buffer;
      buffer << "madara::expression::VariableNode::evaluate: ";
      buffer << "ERROR: settings do not allow reads of unset vars and ";
      buffer << ref_.get_name() << " is uninitialized";
      throw exceptions::UninitializedException (buffer.str ());
    }

    return *ret;
  }
  else
  {
    auto ret = context_.get(expand_key(), settings);

    if(settings.exception_on_unitialized && !ret.exists ())
    {
      std::stringstream buffer;
      buffer << "madara::expression::VariableNode::evaluate: ";
      buffer << "ERROR: settings do not allow reads of unset vars and ";
      buffer << ref_.get_name() << " is uninitialized";
      throw exceptions::UninitializedException (buffer.str ());
    }

    return ret;
  }
}

const std::string& madara::expression::VariableNode::key() const
{
  return key_;
}

int madara::expression::VariableNode::set(
    const madara::knowledge::KnowledgeRecord& value,
    const madara::knowledge::KnowledgeUpdateSettings& settings)
{
  int result = 0;

  knowledge::VariableReference ref = ref_;

  madara_logger_ptr_log(logger_, logger::LOG_MINOR,
      "madara::expression::VariableNode::set: "
      "Attempting to set variable %s to a KnowledgeRecord parameter (%s).\n",
      key_.c_str(), value.to_string().c_str());

  if(!ref.is_valid())
  {
    ref = context_.get_ref(key_, settings);
  }

  if(ref.is_valid())
  {
    auto record = ref.get_record_unsafe();

    // notice that we assume the context is locked
    // check if we have the appropriate write quality
    if(!settings.always_overwrite && record->write_quality < record->quality)
    {
      result = -2;
    }
    else
    {
      // cheaper to read than write, so check to see if
      // we actually need to update quality and status
      if(record->write_quality != record->quality)
        record->quality = record->write_quality;

      madara_logger_ptr_log(logger_, logger::LOG_MINOR,
          "madara::expression::VariableNode::set: "
          "Setting variable %s with KnowledgeRecord assignment operator.\n",
          key_.c_str());

      *record = value;

      context_.mark_and_signal(ref);
    }
  }

  return result;
}

int madara::expression::VariableNode::set(
    const madara::knowledge::KnowledgeRecord::Integer& value,
    const madara::knowledge::KnowledgeUpdateSettings& settings)
{
  madara_logger_ptr_log(logger_, logger::LOG_MINOR,
      "madara::expression::VariableNode::set: "
      "Attempting to set variable %s to an Integer parameter (%d).\n",
      key_.c_str(), (int)value);

  return VariableNode::set(knowledge::KnowledgeRecord(value), settings);
}

int madara::expression::VariableNode::set(
    double value, const madara::knowledge::KnowledgeUpdateSettings& settings)
{
  madara_logger_ptr_log(logger_, logger::LOG_MINOR,
      "madara::expression::VariableNode::set: "
      "Attempting to set variable %s to a double parameter (%f).\n",
      key_.c_str(), value);

  return VariableNode::set(knowledge::KnowledgeRecord(value), settings);
}

int madara::expression::VariableNode::set(const std::string& value,
    const madara::knowledge::KnowledgeUpdateSettings& settings)
{
  madara_logger_ptr_log(logger_, logger::LOG_MINOR,
      "madara::expression::VariableNode::set: "
      "Attempting to set variable %s to a string parameter (%s).\n",
      key_.c_str(), value.c_str());

  return VariableNode::set(knowledge::KnowledgeRecord(value), settings);
}

madara::knowledge::KnowledgeRecord madara::expression::VariableNode::dec(
    const madara::knowledge::KnowledgeUpdateSettings& settings)
{
  if(ref_.is_valid())
  {
    auto record = ref_.get_record_unsafe();

    // notice that we assume the context is locked
    // check if we have the appropriate write quality
    if(!settings.always_overwrite && record->write_quality < record->quality)
      return *record;

    // cheaper to read than write, so check to see if
    // we actually need to update quality and status
    if(record->write_quality != record->quality)
      record->quality = record->write_quality;

    --(*record);

    std::string expanded_key(expand_key());
    context_.mark_and_signal(ref_);

    return *record;
  }
  else
    return context_.dec(expand_key(), settings);
}

madara::knowledge::KnowledgeRecord madara::expression::VariableNode::inc(
    const madara::knowledge::KnowledgeUpdateSettings& settings)
{
  if(ref_.is_valid())
  {
    auto record = ref_.get_record_unsafe();

    // notice that we assume the context is locked
    // check if we have the appropriate write quality
    if(!settings.always_overwrite && record->write_quality < record->quality)
      return *record;

    // cheaper to read than write, so check to see if
    // we actually need to update quality and status
    if(record->write_quality != record->quality)
      record->quality = record->write_quality;

    ++(*record);

    std::string expanded_key(expand_key());
    context_.mark_and_signal(ref_);

    return *record;
  }
  else
    return context_.inc(expand_key(), settings);
}

#endif  // _MADARA_NO_KARL_
