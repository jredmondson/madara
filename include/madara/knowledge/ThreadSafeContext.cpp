#include <iostream>
#include <fstream>
#include <sstream>
#include <iterator>
#include <memory>

#include <string.h>

#include <stdio.h>
#include <time.h>
#include "madara/logger/GlobalLogger.h"
#include "madara/utility/DeepIterator.h"
#include "madara/exceptions/MemoryException.h"
#include "madara/exceptions/FileException.h"
#include "madara/exceptions/FilterException.h"
#include "madara/utility/Utility.h"

#include "madara/knowledge/ThreadSafeContext.h"
#include "madara/knowledge/ContextGuard.h"

#include "madara/expression/Interpreter.h"
#include "madara/transport/Transport.h"

#include "madara/knowledge/CheckpointPlayer.h"

namespace madara
{
namespace knowledge
{
// constructor
ThreadSafeContext::ThreadSafeContext()
  :
#ifdef MADARA_CONDITION_MUTEX_CONSTRUCTOR
    changed_(mutex_),
#endif
    clock_(0)
#ifndef _MADARA_NO_KARL_
    ,
    interpreter_(new madara::expression::Interpreter())
#endif  // _MADARA_NO_KARL_
    ,
    logger_(logger::global_logger.get())
{
  expansion_splitters_.push_back("{");
  expansion_splitters_.push_back("}");
}

// destructor
ThreadSafeContext::~ThreadSafeContext(void)
{
#ifndef _MADARA_NO_KARL_
  delete interpreter_;
#endif  // _MADARA_NO_KARL_
}

/**
 * Retrieves a knowledge record from the key. This function is useful
 * for performance reasons and also for using a knowledge::KnowledgeRecord that
 * can be one of multiple types
 **/
KnowledgeRecord* ThreadSafeContext::get_record(
    const std::string& key, const KnowledgeReferenceSettings& settings)
{
  std::string key_actual;
  const std::string* key_ptr;
  MADARA_GUARD_TYPE guard(mutex_);

  if (settings.expand_variables)
  {
    key_actual = expand_statement(key);
    key_ptr = &key_actual;
  }
  else
    key_ptr = &key;

  if (*key_ptr == "")
    return 0;

  // if the variable doesn't exist, hash maps create a record automatically
  // when used in this manner
  return &map_[*key_ptr];
}

VariableReference ThreadSafeContext::get_ref(
    const std::string& key, const KnowledgeReferenceSettings& settings)
{
  std::string key_actual;
  const std::string* key_ptr;
  MADARA_GUARD_TYPE guard(mutex_);

  // expand the key if the user asked for it
  if (settings.expand_variables)
  {
    key_actual = expand_statement(key);
    key_ptr = &key_actual;
  }
  else
    key_ptr = &key;

  if (*key_ptr == "")
  {
    return {};
  }

  auto iter = map_.lower_bound(*key_ptr);
  if (iter == map_.end() || iter->first != *key_ptr)
  {
    iter = map_.emplace_hint(iter, std::piecewise_construct,
        std::forward_as_tuple(*key_ptr), std::forward_as_tuple());
  }

  return &*iter;
}

VariableReference ThreadSafeContext::get_ref(
    const std::string& key, const KnowledgeReferenceSettings& settings) const
{
  std::string key_actual;
  const std::string* key_ptr;
  MADARA_GUARD_TYPE guard(mutex_);

  VariableReference record;

  // expand the key if the user asked for it
  if (settings.expand_variables)
  {
    key_actual = expand_statement(key);
    key_ptr = &key_actual;
  }
  else
    key_ptr = &key;

  if (*key_ptr == "")
  {
    return {};
  }

  KnowledgeMap::const_iterator found = map_.find(*key_ptr);
  return {const_cast<VariableReference::pair_ptr>(&*found)};
}

// set the value of a variable
int ThreadSafeContext::set_xml(const VariableReference& variable,
    const char* value, size_t size, const KnowledgeUpdateSettings& settings)
{
  MADARA_GUARD_TYPE guard(mutex_);
  auto record = variable.get_record_unsafe();

  if (record)
  {
    // check if we have the appropriate write quality
    if (!settings.always_overwrite && record->write_quality < record->quality)
      return -2;

    record->set_xml(value, size);
    record->quality = record->write_quality;
    record->clock = clock_;
    record->set_toi(utility::get_time());

    mark_and_signal(variable, settings);
  }
  else
    return -1;

  return 0;
}

// set the value of a variable
int ThreadSafeContext::set_text(const VariableReference& variable,
    const char* value, size_t size, const KnowledgeUpdateSettings& settings)
{
  MADARA_GUARD_TYPE guard(mutex_);
  auto record = variable.get_record_unsafe();

  if (record)
  {
    // check if we have the appropriate write quality
    if (!settings.always_overwrite && record->write_quality < record->quality)
      return -2;

    record->set_text(value, size);
    record->quality = record->write_quality;
    record->clock = clock_;
    record->set_toi(utility::get_time());

    mark_and_signal(variable, settings);
  }
  else
    return -1;

  return 0;
}

// set the value of a variable
int ThreadSafeContext::set_jpeg(const VariableReference& variable,
    const unsigned char* value, size_t size,
    const KnowledgeUpdateSettings& settings)
{
  MADARA_GUARD_TYPE guard(mutex_);
  auto record = variable.get_record_unsafe();

  if (record)
  {
    // check if we have the appropriate write quality
    if (!settings.always_overwrite && record->write_quality < record->quality)
      return -2;

    record->set_jpeg(value, size);
    record->quality = record->write_quality;
    record->clock = clock_;
    record->set_toi(utility::get_time());

    mark_and_signal(variable, settings);
  }
  else
    return -1;

  return 0;
}

// set the value of a variable
int ThreadSafeContext::set_file(const VariableReference& variable,
    const unsigned char* value, size_t size,
    const KnowledgeUpdateSettings& settings)
{
  MADARA_GUARD_TYPE guard(mutex_);
  auto record = variable.get_record_unsafe();

  if (record)
  {
    // check if we have the appropriate write quality
    if (!settings.always_overwrite && record->write_quality < record->quality)
      return -2;

    record->set_file(value, size);
    record->quality = record->write_quality;
    record->clock = clock_;
    record->set_toi(utility::get_time());

    mark_and_signal(variable, settings);
  }
  else
    return -1;

  return 0;
}

// set the value of a variable
int ThreadSafeContext::read_file(const VariableReference& variable,
    const std::string& filename, const KnowledgeUpdateSettings& settings)
{
  int return_value = 0;
  MADARA_GUARD_TYPE guard(mutex_);
  auto record = variable.get_record_unsafe();

  if (record)
  {
    // check if we have the appropriate write quality
    if (!settings.always_overwrite && record->write_quality < record->quality)
      return -2;

    return_value = record->read_file(filename);
    record->quality = record->write_quality;
    record->clock = clock_;
    record->set_toi(utility::get_time());

    mark_and_signal(variable, settings);
  }
  else
    return return_value = -1;

  return return_value;
}

/// get quality of last update to a variable.
/// @return    quality of the variable
uint32_t ThreadSafeContext::get_quality(
    const std::string& key, const KnowledgeReferenceSettings& settings)
{
  // enter the mutex
  std::string key_actual;
  const std::string* key_ptr;
  MADARA_GUARD_TYPE guard(mutex_);

  if (settings.expand_variables)
  {
    key_actual = expand_statement(key);
    key_ptr = &key_actual;
  }
  else
    key_ptr = &key;

  // find the key in the knowledge base
  KnowledgeMap::iterator found = map_.find(*key_ptr);

  // create the variable if it has never been written to before
  // and update its current value quality to the quality parameter

  if (found != map_.end())
    return map_[*key_ptr].quality;

  // default quality is 0
  return 0;
}

/// get quality of last update to a variable.
/// @return    quality of the variable
uint32_t ThreadSafeContext::get_write_quality(
    const std::string& key, const KnowledgeReferenceSettings& settings)
{
  // enter the mutex
  std::string key_actual;
  const std::string* key_ptr;
  MADARA_GUARD_TYPE guard(mutex_);

  if (settings.expand_variables)
  {
    key_actual = expand_statement(key);
    key_ptr = &key_actual;
  }
  else
    key_ptr = &key;

  // find the key in the knowledge base
  KnowledgeMap::iterator found = map_.find(*key_ptr);

  // create the variable if it has never been written to before
  // and update its current value quality to the quality parameter

  if (found != map_.end())
    return map_[*key_ptr].write_quality;

  // default quality is 0
  return 0;
}

/// Set quality of last update to a variable.
/// @return    quality of the variable after this call
uint32_t ThreadSafeContext::set_quality(const std::string& key,
    uint32_t quality, bool force_update,
    const KnowledgeReferenceSettings& settings)
{
  // enter the mutex
  std::string key_actual;
  const std::string* key_ptr;
  MADARA_GUARD_TYPE guard(mutex_);

  if (settings.expand_variables)
  {
    key_actual = expand_statement(key);
    key_ptr = &key_actual;
  }
  else
    key_ptr = &key;

  // check for null key
  if (*key_ptr == "")
    return 0;

  // find the key in the knowledge base
  KnowledgeMap::iterator found = map_.find(*key_ptr);

  // create the variable if it has never been written to before
  // and update its current value quality to the quality parameter

  if (found == map_.end() || force_update || quality > found->second.quality)
    map_[*key_ptr].quality = quality;

  // return current quality
  return map_[*key_ptr].quality;
}

/// Set quality of this process writing to a variable
void ThreadSafeContext::set_write_quality(const std::string& key,
    uint32_t quality, const KnowledgeReferenceSettings& settings)
{
  // enter the mutex
  std::string key_actual;
  const std::string* key_ptr;
  MADARA_GUARD_TYPE guard(mutex_);

  if (settings.expand_variables)
  {
    key_actual = expand_statement(key);
    key_ptr = &key_actual;
  }
  else
    key_ptr = &key;

  // create the variable if it has never been written to before
  // and update its local process write quality to the quality parameter
  map_[*key_ptr].write_quality = quality;
}

/// Set if the variable value will be different. Always updates clock to
/// highest value
/// @return   1 if the value was changed. 0 if not changed.
///           -1 if null key, -2 if quality not high enough
int ThreadSafeContext::set_if_unequal(const std::string& key,
    KnowledgeRecord::Integer value, uint32_t quality, uint64_t clock,
    const KnowledgeUpdateSettings& settings)
{
  int result = 1;

  // enter the mutex
  std::string key_actual;
  const std::string* key_ptr;
  MADARA_GUARD_TYPE guard(mutex_);

  if (settings.expand_variables)
  {
    key_actual = expand_statement(key);
    key_ptr = &key_actual;
  }
  else
    key_ptr = &key;

  // check for null key
  if (*key_ptr == "")
    return -1;

  // find the key in the knowledge base
  KnowledgeMap::iterator found = map_.find(*key_ptr);

  // if it's found, then compare the value
  if (!settings.always_overwrite && found != map_.end())
  {
    // setup a rhs
    KnowledgeRecord rhs;
    rhs.set_value(value);

    // if we do not have enough quality to update the variable
    // return -2
    if (quality < found->second.quality)
      result = -2;

    // if we have the same quality, but our clock value
    // is less than what we've already seen, then return -3
    else if (quality == found->second.quality && clock < found->second.clock)
      result = -3;

    // check for value already set
    else if (found->second == rhs)
      result = 0;
  }
  else
  {
    auto ret = map_.emplace(std::piecewise_construct,
        std::forward_as_tuple(*key_ptr), std::make_tuple());
    found = ret.first;
  }

  KnowledgeRecord& record = found->second;

  // if we need to update quality, then update it
  if (result != -2 && record.quality != quality)
    record.quality = quality;

  // if we need to update the variable clock, then update it
  if (clock > record.clock)
    record.clock = clock;

  // if we need to update the global clock, then update it
  if (clock > this->clock_)
    this->clock_ = clock;

  if (result == 1)
  {
    // we have a situation where the value needs to be changed
    record.set_value(value);

    record.clock = clock_;
    record.set_toi(utility::get_time());

    mark_and_signal(&*found, settings);
  }

  // value was changed
  return result;
}

/// Set if the variable value will be different. Always updates clock to
/// highest value
/// @return   1 if the value was changed. 0 if not changed.
///           -1 if null key, -2 if quality not high enough
int ThreadSafeContext::set_if_unequal(const std::string& key, double value,
    uint32_t quality, uint64_t clock, const KnowledgeUpdateSettings& settings)
{
  int result = 1;

  // enter the mutex
  std::string key_actual;
  const std::string* key_ptr;
  MADARA_GUARD_TYPE guard(mutex_);

  if (settings.expand_variables)
  {
    key_actual = expand_statement(key);
    key_ptr = &key_actual;
  }
  else
    key_ptr = &key;

  // check for null key
  if (*key_ptr == "")
    return -1;

  // find the key in the knowledge base
  KnowledgeMap::iterator found = map_.find(*key_ptr);

  // if it's found, then compare the value
  if (!settings.always_overwrite && found != map_.end())
  {
    // setup a rhs
    KnowledgeRecord rhs;
    rhs.set_value(value);

    // if we do not have enough quality to update the variable
    // return -2
    if (quality < found->second.quality)
      result = -2;

    // if we have the same quality, but our clock value
    // is less than what we've already seen, then return -3
    else if (quality == found->second.quality && clock < found->second.clock)
      result = -3;

    // check for value already set
    else if (found->second == rhs)
      result = 0;
  }
  else
  {
    auto ret = map_.emplace(std::piecewise_construct,
        std::forward_as_tuple(*key_ptr), std::make_tuple());
    found = ret.first;
  }

  KnowledgeRecord& record = found->second;

  // if we need to update quality, then update it
  if (result != -2 && record.quality != quality)
    record.quality = quality;

  // if we need to update the variable clock, then update it
  if (clock > record.clock)
    record.clock = clock;

  // if we need to update the global clock, then update it
  if (clock > this->clock_)
    this->clock_ = clock;

  if (result == 1)
  {
    // we have a situation where the value needs to be changed
    record.set_value(value);
    record.clock = clock_;
    record.set_toi(utility::get_time());

    mark_and_signal(&*found, settings);
  }

  // value was changed
  return result;
}

/// Set if the variable value will be different. Always updates clock to
/// highest value
/// @return   1 if the value was changed. 0 if not changed.
///           -1 if null key, -2 if quality not high enough
int ThreadSafeContext::set_if_unequal(const std::string& key,
    const std::string& value, uint32_t quality, uint64_t clock,
    const KnowledgeUpdateSettings& settings)
{
  int result = 1;

  // enter the mutex
  std::string key_actual;
  const std::string* key_ptr;
  MADARA_GUARD_TYPE guard(mutex_);

  if (settings.expand_variables)
  {
    key_actual = expand_statement(key);
    key_ptr = &key_actual;
  }
  else
    key_ptr = &key;

  // check for null key
  if (*key_ptr == "")
    return -1;

  // find the key in the knowledge base
  KnowledgeMap::iterator found = map_.find(*key_ptr);

  // if it's found, then compare the value
  if (!settings.always_overwrite && found != map_.end())
  {
    // setup a rhs
    KnowledgeRecord rhs;
    rhs.set_value(value);

    // if we do not have enough quality to update the variable
    // return -2
    if (quality < found->second.quality)
      result = -2;

    // if we have the same quality, but our clock value
    // is less than what we've already seen, then return -3
    else if (quality == found->second.quality && clock < found->second.clock)
      result = -3;

    // check for value already set
    else if (found->second == rhs)
      result = 0;
  }
  else
  {
    auto ret = map_.emplace(std::piecewise_construct,
        std::forward_as_tuple(*key_ptr), std::make_tuple());
    found = ret.first;
  }

  KnowledgeRecord& record = found->second;

  // if we need to update quality, then update it
  if (result != -2 && record.quality != quality)
    record.quality = quality;

  // if we need to update the variable clock, then update it
  if (clock > record.clock)
    record.clock = clock;

  // if we need to update the global clock, then update it
  if (clock > this->clock_)
    this->clock_ = clock;

  if (result == 1)
  {
    // we have a situation where the value needs to be changed
    record.set_value(value);
    record.clock = clock_;
    record.set_toi(utility::get_time());

    // otherwise set the value
    mark_and_signal(&*found, settings);
  }
  // value was changed
  return result;
}

/// Set if the variable value will be different. Always updates clock to
/// highest value
/// @return   1 if the value was changed. 0 if not changed.
///           -1 if null key, -2 if quality not high enough
int ThreadSafeContext::update_record_from_external(const std::string& key,
    const knowledge::KnowledgeRecord& rhs,
    const KnowledgeUpdateSettings& settings)
{
  int result = 1;

  // enter the mutex
  std::string key_actual;
  const std::string* key_ptr;
  MADARA_GUARD_TYPE guard(mutex_);

  if (settings.expand_variables)
  {
    key_actual = expand_statement(key);
    key_ptr = &key_actual;
  }
  else
    key_ptr = &key;

  // check for null key
  if (*key_ptr == "")
    return -1;

  // find the key in the knowledge base
  KnowledgeMap::iterator found = map_.find(*key_ptr);

  // if it's found, then compare the value
  if (!settings.always_overwrite && found != map_.end())
  {
    // if we do not have enough quality to update the variable
    // return -2
    if (rhs.quality < found->second.quality)
      return -2;

    // if we have the same quality, but our clock value
    // is less than what we've already seen, then return -3
    else if (rhs.quality == found->second.quality &&
             rhs.clock < found->second.clock)
      return -3;

    // if we reach this point, then the record is safe to copy
    found->second.set_full(rhs);

    mark_and_signal(&*found, settings);
  }
  else
  {
    // if we reach this point, then we have to create the record
    auto ret = map_.emplace(std::piecewise_construct,
        std::forward_as_tuple(*key_ptr), std::forward_as_tuple(rhs));
    found = ret.first;

    if (!ret.second)
    {
      ret.first->second = rhs;
    }

    mark_and_signal(&*found, settings);
  }

  // if we need to update the global clock, then update it
  if (rhs.clock >= this->clock_)
    this->clock_ = rhs.clock + 1;

  // if (settings.signal_changes)
  //  changed_.MADARA_CONDITION_NOTIFY_ONE ();

  // value was changed
  return result;
}

/// Set if the variable value will be different. Always updates clock to
/// highest value
/// @return   1 if the value was changed. 0 if not changed.
///           -1 if null key, -2 if quality not high enough
int ThreadSafeContext::update_record_from_external(
    const VariableReference& target, const knowledge::KnowledgeRecord& rhs,
    const KnowledgeUpdateSettings& settings)
{
  int result = 1;

  MADARA_GUARD_TYPE guard(mutex_);
  auto record = target.get_record_unsafe();

  // if it's found, then compare the value
  if (!settings.always_overwrite && target.is_valid())
  {
    // if we do not have enough quality to update the variable
    // return -2
    if (rhs.quality < record->quality)
      result = -2;

    // if we have the same quality, but our clock value
    // is less than what we've already seen, then return -3
    else if (rhs.quality == record->quality && rhs.clock < record->clock)
      result = -3;

    // if we reach this point, then the record is safe to copy
    record->set_full(rhs);

    mark_and_signal(target, settings);
  }

  // if we need to update the global clock, then update it
  if (rhs.clock >= this->clock_)
    this->clock_ = rhs.clock + 1;

  // value was changed
  return result;
}

/// Indicate that a status change has occurred. This could be a message
/// from the transport to let the knowledge engine know that new agents
/// are available to send knowledge to.
void ThreadSafeContext::set_changed(void)
{
  changed_.MADARA_CONDITION_NOTIFY_ONE();
}

// print all variables and their values
void ThreadSafeContext::print(unsigned int level) const
{
  MADARA_GUARD_TYPE guard(mutex_);
  for (KnowledgeMap::const_iterator i = map_.begin(); i != map_.end(); ++i)
  {
    if (i->second.exists())
    {
      madara_logger_ptr_log(logger_, (int)level, "%s=%s\n", i->first.c_str(),
          i->second.to_string(", ").c_str());
    }
  }
}

// print all variables and their values
void ThreadSafeContext::to_string(std::string& target,
    const std::string& array_delimiter, const std::string& record_delimiter,
    const std::string& key_val_delimiter) const
{
  MADARA_GUARD_TYPE guard(mutex_);
  std::stringstream buffer;

  bool first = true;

  for (KnowledgeMap::const_iterator i = map_.begin(); i != map_.end(); ++i)
  {
    // separate each record with the record_delimiter
    if (!first)
    {
      buffer << record_delimiter;
    }

    buffer << i->first;

    // separate the key/value pairing with the key_val_delimiter
    buffer << key_val_delimiter;

    if (i->second.is_string_type() || i->second.is_any_type())
    {
      buffer << "'";
    }
    else if (i->second.type() == i->second.DOUBLE_ARRAY ||
             i->second.type() == i->second.INTEGER_ARRAY)
    {
      buffer << "[";
    }

    // use the array_delimiter for the underlying to_string functions
    buffer << i->second.to_string(array_delimiter);

    if (i->second.is_string_type() || i->second.is_any_type())
    {
      buffer << "'";
    }
    else if (i->second.type() == i->second.DOUBLE_ARRAY ||
             i->second.type() == i->second.INTEGER_ARRAY)
    {
      buffer << "]";
    }

    if (first)
      first = false;
  }

  target = buffer.str();
}

/// Expand a string with variable expansions. This is a generic form of
/// the function found in VariableNode, which is optimized to never change
/// keys.
std::string ThreadSafeContext::expand_statement(
    const std::string& statement) const
{
  // enter the mutex
  MADARA_GUARD_TYPE guard(mutex_);

  // vectors for holding parsed tokens and pivot_list
  size_t subcount = 0;
  size_t begin_exp = 0;

  std::stringstream builder;

  // iterate over the input string
  for (std::string::size_type i = 0; i < statement.size(); ++i)
  {
    // if this is an open brace, increase the subcount
    if (statement[i] == '{')
    {
      ++subcount;
      if (subcount == 1)
        begin_exp = i;
    }
    // closed brace should decrease subcount
    else if (statement[i] == '}')
    {
      if (subcount == 1)
      {
        std::string expandable =
            statement.substr(begin_exp + 1, i - begin_exp - 1);
        std::string results = this->expand_statement(expandable);
        builder << this->get(results);
      }
      --subcount;
    }
    // otherwise, if this subcount is 0, then we need to add it to our output
    // we allow anything not in subcount == 0 to be handled through recursion
    else
    {
      if (subcount == 0)
        builder << statement[i];
    }
  }

  // check to see if all brace counts are appropriate
  if (subcount != 0)
  {
    madara_logger_ptr_log(logger_, logger::LOG_ERROR,
        "KARL COMPILE ERROR : Improperly matched braces in %s\n",
        statement.c_str());
  }

  return builder.str();
}

#ifndef _MADARA_NO_KARL_

// defines a function by name
void ThreadSafeContext::define_function(const std::string& name,
    knowledge::KnowledgeRecord (*func)(FunctionArguments&, Variables&),
    const KnowledgeReferenceSettings& settings)
{
  // enter the mutex
  std::string key_actual;
  const std::string* key_ptr;
  MADARA_GUARD_TYPE guard(mutex_);

  if (settings.expand_variables)
  {
    key_actual = expand_statement(name);
    key_ptr = &key_actual;
  }
  else
    key_ptr = &name;

  // check for null key
  if (*key_ptr == "")
    return;

  functions_[*key_ptr] = Function(func);
}

void ThreadSafeContext::define_function(const std::string& name,
    knowledge::KnowledgeRecord (*func)(
        const char* name, FunctionArguments&, Variables&),
    const KnowledgeReferenceSettings& settings)
{
  // enter the mutex
  std::string key_actual;
  const std::string* key_ptr;
  MADARA_GUARD_TYPE guard(mutex_);

  if (settings.expand_variables)
  {
    key_actual = expand_statement(name);
    key_ptr = &key_actual;
  }
  else
    key_ptr = &name;

  // check for null key
  if (*key_ptr == "")
    return;

  functions_[*key_ptr] = Function(func);
}

#ifdef _MADARA_JAVA_
void ThreadSafeContext::define_function(const std::string& name,
    jobject callable, const KnowledgeReferenceSettings& settings)
{
  // enter the mutex
  std::string key_actual;
  const std::string* key_ptr;
  MADARA_GUARD_TYPE guard(mutex_);

  if (settings.expand_variables)
  {
    key_actual = expand_statement(name);
    key_ptr = &key_actual;
  }
  else
    key_ptr = &name;

  // check for null key
  if (*key_ptr == "")
    return;

  functions_[*key_ptr] = Function(callable);
}
#endif

#ifdef _MADARA_PYTHON_CALLBACKS_
void ThreadSafeContext::define_function(const std::string& name,
    boost::python::object callable, const KnowledgeReferenceSettings& settings)
{
  // enter the mutex
  std::string key_actual;
  const std::string* key_ptr;
  MADARA_GUARD_TYPE guard(mutex_);

  if (settings.expand_variables)
  {
    key_actual = expand_statement(name);
    key_ptr = &key_actual;
  }
  else
    key_ptr = &name;

  // check for null key
  if (*key_ptr == "")
    return;

  functions_[*key_ptr] = Function(callable);
}

#endif

void ThreadSafeContext::define_function(const std::string& name,
    const std::string& expression, const KnowledgeReferenceSettings& settings)
{
  CompiledExpression compiled = compile(expression);
  define_function(name, compiled, settings);
}

void ThreadSafeContext::define_function(const std::string& name,
    const CompiledExpression& expression,
    const KnowledgeReferenceSettings& settings)
{
  // enter the mutex
  std::string key_actual;
  const std::string* key_ptr;
  MADARA_GUARD_TYPE guard(mutex_);

  if (settings.expand_variables)
  {
    key_actual = expand_statement(name);
    key_ptr = &key_actual;
  }
  else
    key_ptr = &name;

  // check for null key
  if (*key_ptr == "")
    return;

  functions_[*key_ptr] = Function(expression.expression);
}

Function* ThreadSafeContext::retrieve_function(
    const std::string& name, const KnowledgeReferenceSettings& settings)
{
  // enter the mutex
  std::string key_actual;
  const std::string* key_ptr;
  MADARA_GUARD_TYPE guard(mutex_);

  if (settings.expand_variables)
  {
    key_actual = expand_statement(name);
    key_ptr = &key_actual;
  }
  else
    key_ptr = &name;

  // check for null key
  if (*key_ptr == "")
    return 0;

  return &functions_[*key_ptr];
}

CompiledExpression ThreadSafeContext::compile(const std::string& expression)
{
  madara_logger_ptr_log(logger_, logger::LOG_MINOR,
      "ThreadSafeContext::compile:"
      " compiling %s\n",
      expression.c_str());

  MADARA_GUARD_TYPE guard(mutex_);
  CompiledExpression ce;
  ce.logic = expression;
  ce.expression = interpreter_->interpret(*this, expression);

  return ce;
}

KnowledgeRecord ThreadSafeContext::evaluate(
    CompiledExpression expression, const KnowledgeUpdateSettings& settings)
{
  MADARA_GUARD_TYPE guard(mutex_);
  return expression.expression.evaluate(settings);
}

KnowledgeRecord ThreadSafeContext::evaluate(
    expression::ComponentNode* root, const KnowledgeUpdateSettings& settings)
{
  MADARA_GUARD_TYPE guard(mutex_);
  if (root)
    return root->evaluate(settings);
  else
    return knowledge::KnowledgeRecord(KnowledgeRecord::Integer(0));
}

#endif  // _MADARA_NO_KARL_

size_t ThreadSafeContext::to_vector(const std::string& subject,
    unsigned int start, unsigned int end, std::vector<KnowledgeRecord>& target)
{
  target.clear();

  // enter the mutex
  MADARA_GUARD_TYPE guard(mutex_);

  if (end >= start)
  {
    target.resize(end - start + 1);

    for (unsigned int i = 0; start <= end; ++start, ++i)
    {
      std::stringstream buffer;
      buffer << subject;
      buffer << start;
      target[i] = get(buffer.str());
    }
  }

  return target.size();
}

size_t ThreadSafeContext::to_map(const std::string& expression,
    std::map<std::string, knowledge::KnowledgeRecord>& target)
{
  target.clear();

  std::string subject(expression);
  bool matches_found(false);

  // remove the wildcard and make this into a subject
  if (subject[subject.size() - 1] == '*')
    subject.resize(subject.size() - 1);

  // just in case a string implementation does not inline
  std::string::size_type subject_size = subject.size();
  const char* subject_ptr = subject.c_str();

  // enter the mutex
  MADARA_GUARD_TYPE guard(mutex_);

  // if expression is blank, assume the user wants all variables
  if (expression.size() == 0)
    target = map_;
  else
  {
    for (KnowledgeMap::iterator i = map_.begin(); i != map_.end(); ++i)
    {
      if (i->first.size() >= subject_size)
      {
        int result = strncmp(i->first.c_str(), subject_ptr, subject_size);
        if (result == 0)
        {
          // we have a match, add this to the map
          target[i->first] = i->second;
          matches_found = true;
        }
        else if (matches_found)
        {
          // we have already found matches, and now we're not seeing matches
          break;
        }
      }
    }
  }

  return target.size();
}

void ThreadSafeContext::get_matches(const std::string& prefix,
    const std::string& suffix, VariableReferences& matches)
{
  // get the first thing that either matches the prefix or is just after it
  KnowledgeMap::iterator i = map_.lower_bound(prefix);

  // if we have a valid prefix, then there is more to do
  if (i != map_.end())
  {
    // keep track of the beginning as we're going to iterate twice
    KnowledgeMap::iterator first_match = i;
    KnowledgeMap::iterator after_matches = map_.end();
    VariableReferences::iterator match;

    size_t num_matches = 0;

    size_t prefix_length = prefix.length();

    // Iterate over all of the prefix matches
    while (i != map_.end() && i->first.compare(0, prefix_length, prefix) == 0)
    {
      ++i;
      if (suffix == "" || utility::ends_with(i->first, suffix))
      {
        ++num_matches;
      }
    }

    // save the end point so we can do fewer checks
    after_matches = i;

    // resize the matches to the appropriate size
    matches.resize(num_matches);

    // now, instead of many resizes, we are just going to resize once and set
    i = first_match;
    num_matches = 0;

    match = matches.begin();

    // Reiterate
    while (i != after_matches)
    {
      if (suffix == "" || utility::ends_with(i->first, suffix))
      {
        match->assign(&*i);
        ++match;
      }
      ++i;
    }
  }
  else
  {
    matches.clear();
  }
}

size_t ThreadSafeContext::to_map(const std::string& prefix,
    const std::string& delimiter, const std::string& suffix,
    std::vector<std::string>& next_keys,
    std::map<std::string, knowledge::KnowledgeRecord>& result, bool just_keys)
{
  // clear the user provided maps
  next_keys.clear();
  result.clear();

  // loop tracking for optimizations
  bool matches_found(false);
  std::string last_key("");

  // enter the mutex
  MADARA_GUARD_TYPE guard(mutex_);

  KnowledgeMap::iterator i = map_.begin();

  if (prefix != "")
  {
    i = map_.lower_bound(prefix);
  }

  for (; i != map_.end(); ++i)
  {
    // if the prefix doesn't match
    if (prefix != "" && !utility::begins_with(i->first, prefix))
    {
      // if we had previously matched a prefix, we're done
      if (matches_found)
      {
        break;
      }
    }
    // we have a prefix match
    else
    {
      // set matches found if it hasn't been set previously
      if (!matches_found)
      {
        matches_found = true;
      }

      // if the suffix is provided and doesn't match, continue
      if (suffix != "" && !utility::ends_with(i->first, suffix))
      {
        continue;
      }

      if (!just_keys)
      {
        // the key is safe to add to the master map
        result[i->first] = i->second;
      }

      // determine if there is a next key in the hierarchy
      size_t prefix_end = prefix.length() + delimiter.length();

      std::string current_delimiter =
          i->first.substr(prefix.length(), delimiter.length());

      if (current_delimiter == delimiter && i->first.length() > prefix_end)
      {
        // find the end of the sub key
        size_t key_end = i->first.find(delimiter, prefix_end);

        // if we haven't seen the subkey, add it
        std::string current_key(
            i->first.substr(prefix_end, key_end - prefix_end));
        if (current_key != last_key)
        {
          next_keys.push_back(current_key);
          last_key = current_key;
        }
      }
    }
  }

  return result.size();
}

void ThreadSafeContext::delete_prefix(
    const std::string& prefix, const KnowledgeReferenceSettings&)
{
  // enter the mutex
  MADARA_GUARD_TYPE guard(mutex_);

  std::pair<KnowledgeMap::iterator, KnowledgeMap::iterator> iters(
      get_prefix_range(prefix));

  map_.erase(iters.first, iters.second);

  {
    // check the changed map
    std::pair<VariableReferenceMap::iterator, VariableReferenceMap::iterator>
        changed(changed_map_.lower_bound(prefix.c_str()), changed_map_.end());

    // does our lower bound actually contain the prefix?
    if (prefix.compare(0, prefix.size(), changed.first->first, prefix.size()) ==
        0)
    {
      changed.second = changed.first;

      // until we find an entry that does not begin with prefix, loop
      for (++changed.second; (prefix.compare(0, prefix.size(),
                                  changed.second->first, prefix.size()) == 0) &&
                             changed.second != changed_map_.end();
           ++changed.second)
      {
      }

      changed_map_.erase(changed.first, changed.second);
    }
  }

  {
    // check the local changed map
    std::pair<VariableReferenceMap::iterator, VariableReferenceMap::iterator>
        local_changed(local_changed_map_.lower_bound(prefix.c_str()),
            local_changed_map_.end());

    // does our lower bound actually contain the prefix?
    if (prefix.compare(
            0, prefix.size(), local_changed.first->first, prefix.size()) == 0)
    {
      local_changed.second = local_changed.first;

      // until we find an entry that does not begin with prefix, loop
      for (++local_changed.second;
           (prefix.compare(0, prefix.size(), local_changed.second->first,
                prefix.size()) == 0) &&
           local_changed.second != local_changed_map_.end();
           ++local_changed.second)
      {
      }

      local_changed_map_.erase(local_changed.first, local_changed.second);
    }
  }
}

std::pair<KnowledgeMap::iterator, KnowledgeMap::iterator>
ThreadSafeContext::get_prefix_range(const std::string& prefix)
{
  std::pair<KnowledgeMap::iterator, KnowledgeMap::iterator> ret(
      map_.begin(), map_.end());

  // If prefix is empty string, copy entire map
  if (prefix.size() > 0)
  {
    ssize_t psz = prefix.size();

    // Find first element >= prefix; i.e., first match or first with that prefix
    ret.second = ret.first = map_.lower_bound(prefix);

    // Advance e until it is just past last element with prefix (or at end)
    while (ret.second != map_.end() &&
           ret.second->first.compare(0, psz, prefix) == 0)
      ++ret.second;
  }
  return ret;
}

std::pair<KnowledgeMap::const_iterator, KnowledgeMap::const_iterator>
ThreadSafeContext::get_prefix_range(const std::string& prefix) const
{
  std::pair<KnowledgeMap::const_iterator, KnowledgeMap::const_iterator> ret(
      map_.begin(), map_.end());

  // If prefix is empty string, copy entire map
  if (prefix.size() > 0)
  {
    ssize_t psz = prefix.size();

    // Find first element >= prefix; i.e., first match or first with that prefix
    ret.second = ret.first = map_.lower_bound(prefix);

    // Advance e until it is just past last element with prefix (or at end)
    while (ret.second != map_.end() &&
           ret.second->first.compare(0, psz, prefix) == 0)
      ++ret.second;
  }
  return ret;
}

KnowledgeMap ThreadSafeContext::to_map(const std::string& prefix) const
{
  // enter the mutex
  MADARA_GUARD_TYPE guard(mutex_);

  std::pair<KnowledgeMap::const_iterator, KnowledgeMap::const_iterator> iters(
      get_prefix_range(prefix));

  // RVO should avoid copying this map
  return KnowledgeMap(iters.first, iters.second);
}

KnowledgeMap ThreadSafeContext::to_map_stripped(const std::string& prefix) const
{
  // enter the mutex
  MADARA_GUARD_TYPE guard(mutex_);

  std::pair<KnowledgeMap::const_iterator, KnowledgeMap::const_iterator> iters(
      get_prefix_range(prefix));

  // NRVO should avoid copying this map
  KnowledgeMap ret;
  for (; iters.first != iters.second; ++iters.first)
  {
    ret.emplace_hint(ret.end(), iters.first->first.substr(prefix.size()),
        iters.first->second);
  }
  return ret;
}

void ThreadSafeContext::copy(const ThreadSafeContext& source,
    const KnowledgeRequirements& reqs, const KnowledgeUpdateSettings& settings)
{
  madara_logger_ptr_log(logger_, logger::LOG_MINOR,
      "ThreadSafeContext::copy:"
      " copying a context\n");

  if (reqs.clear_knowledge)
  {
    madara_logger_ptr_log(logger_, logger::LOG_MINOR,
        "ThreadSafeContext::copy:"
        " clearing knowledge in target context\n");

    map_.clear();
  }

  if (reqs.predicates.size() != 0)
  {
    for (auto predicate : reqs.predicates)
    {
      std::pair<KnowledgeMap::const_iterator, KnowledgeMap::const_iterator>
          iters(source.get_prefix_range(predicate.prefix));

      if (predicate.suffix == "")
      {
        madara_logger_ptr_log(logger_, logger::LOG_MINOR,
            "ThreadSafeContext::copy:"
            " matching predicate.prefix=%s\n",
            predicate.prefix.c_str());

        for (; iters.first != iters.second; ++iters.first)
        {
          madara_logger_ptr_log(logger_, logger::LOG_MINOR,
              "ThreadSafeContext::copy:"
              " looking for %s\n",
              iters.first->first.c_str());

          auto where = map_.lower_bound(iters.first->first);

          if (where == map_.end() || where->first != iters.first->first)
          {
            madara_logger_ptr_log(logger_, logger::LOG_MINOR,
                "ThreadSafeContext::copy:"
                " inserting %s\n",
                iters.first->first.c_str());

            where = map_.emplace_hint(
                where, iters.first->first, iters.first->second);
          }
          else
          {
            madara_logger_ptr_log(logger_, logger::LOG_MINOR,
                "ThreadSafeContext::copy:"
                " overwriting %s\n",
                iters.first->first.c_str());

            where->second = iters.first->second;
          }

          mark_modified(iters.first->first, settings);
        }
      }
      else  // we need to match a suffix
      {
        madara_logger_ptr_log(logger_, logger::LOG_MINOR,
            "ThreadSafeContext::copy:"
            " matching predicate.suffix=%s\n",
            predicate.suffix.c_str());

        for (; iters.first != iters.second; ++iters.first)
        {
          if (madara::utility::ends_with(iters.first->first, predicate.suffix))
          {
            madara_logger_ptr_log(logger_, logger::LOG_MINOR,
                "ThreadSafeContext::copy:"
                " looking for %s\n",
                iters.first->first.c_str());

            auto where = map_.lower_bound(iters.first->first);

            if (where == map_.end() || where->first != iters.first->first)
            {
              madara_logger_ptr_log(logger_, logger::LOG_MINOR,
                  "ThreadSafeContext::copy:"
                  " inserting %s\n",
                  iters.first->first.c_str());

              where = map_.emplace_hint(
                  where, iters.first->first, iters.first->second);
            }
            else
            {
              madara_logger_ptr_log(logger_, logger::LOG_MINOR,
                  "ThreadSafeContext::copy:"
                  " overwriting %s\n",
                  iters.first->first.c_str());

              where->second = iters.first->second;
            }

            mark_modified(iters.first->first, settings);
          }  // end suffix match
        }
      }
    }
  }
  // we need to insert everything from source into this
  else
  {
    std::pair<KnowledgeMap::const_iterator, KnowledgeMap::const_iterator> iters(
        source.map_.begin(), source.map_.end());

    for (; iters.first != iters.second; ++iters.first)
    {
      map_.insert(map_.begin(),
          KnowledgeMap::value_type(iters.first->first, iters.first->second));

      mark_modified(iters.first->first, settings);
    }
  }
}

void ThreadSafeContext::copy(const ThreadSafeContext& source,
    const CopySet& copy_set, bool clean_copy,
    const KnowledgeUpdateSettings& settings)
{
  // if we need to clean first, clear the map
  if (clean_copy)
    map_.clear();

  // if the copy set is empty, copy everything
  if (copy_set.size() == 0)
  {
    for (KnowledgeMap::const_iterator i = source.map_.begin();
         i != source.map_.end(); ++i)
    {
      map_[i->first] = (i->second);
      mark_modified(i->first, settings);
    }
  }
  else
  {
    // we have a copy set, so only copy what the user asked for
    for (CopySet::const_iterator key = copy_set.begin(); key != copy_set.end();
         ++key)
    {
      // check source for existence of the current copy set key
      KnowledgeMap::const_iterator i = source.map_.find(key->first);

      // if found, make a copy of the found entry
      if (i != source.map_.end())
      {
        map_[i->first] = (i->second);
        mark_modified(i->first, settings);
      }
    }
  }
}

int64_t ThreadSafeContext::save_context(
    const std::string& filename, const std::string& id) const
{
  CheckpointSettings settings;
  settings.filename = filename;
  settings.originator = id;

  return save_context(settings);
}

int64_t ThreadSafeContext::save_context(
    const CheckpointSettings& settings) const
{
  madara_logger_ptr_log(logger_, logger::LOG_MAJOR,
      "ThreadSafeContext::save_context:"
      " opening file %s\n",
      settings.filename.c_str());

  // int64_t total_written (0);
  FILE* file = fopen(settings.filename.c_str(), "wb");

  FileHeader meta;
  meta.states = 1;
  strncpy(meta.originator, settings.originator.c_str(),
      sizeof(meta.originator) < settings.originator.size() + 1
          ? sizeof(meta.originator)
          : settings.originator.size() + 1);

  transport::MessageHeader checkpoint_header;

  if (file)
  {
    int64_t max_buffer(settings.buffer_size);
    int64_t buffer_remaining(max_buffer);

    madara_logger_ptr_log(logger_, logger::LOG_MINOR,
        "ThreadSafeContext::save_context:"
        " allocating %d byte buffer\n",
        (int)max_buffer);

    utility::ScopedArray<char> buffer = new char[max_buffer];

    char* current = buffer.get_ptr();

    madara_logger_ptr_log(logger_, logger::LOG_MINOR,
        "ThreadSafeContext::save_context:"
        " generating file meta\n");

    meta.size += checkpoint_header.encoded_size();
    checkpoint_header.size = checkpoint_header.encoded_size();

    if (settings.override_timestamp)
    {
      meta.initial_timestamp = settings.initial_timestamp;
      meta.last_timestamp = settings.last_timestamp;
    }

    current = meta.write(current, buffer_remaining);

    if (settings.override_lamport)
    {
      checkpoint_header.clock = settings.initial_lamport_clock;
    }
    else
    {
      checkpoint_header.clock = clock_;
    }

    current = checkpoint_header.write(current, buffer_remaining);

    madara_logger_ptr_log(logger_, logger::LOG_MINOR,
        "ThreadSafeContext::save_context:"
        " writing records\n");

    // lock the context
    MADARA_GUARD_TYPE guard(mutex_);

    for (KnowledgeMap::const_iterator i = map_.begin(); i != map_.end(); ++i)
    {
      if (i->second.exists())
      {
        // check if the prefix is allowed
        if (settings.prefixes.size() > 0)
        {
          madara_logger_ptr_log(logger_, logger::LOG_MINOR,
              "ThreadSafeContext::save_context:"
              " we have %d prefixes to check against.\n",
              (int)settings.prefixes.size());

          bool prefix_found = false;
          for (size_t j = 0; j < settings.prefixes.size() && !prefix_found; ++j)
          {
            madara_logger_ptr_log(logger_, logger::LOG_MINOR,
                "ThreadSafeContext::save_context:"
                " checking record %s against prefix %s.\n",
                i->first.c_str(), settings.prefixes[j].c_str());

            if (madara::utility::begins_with(i->first, settings.prefixes[j]))
            {
              madara_logger_ptr_log(logger_, logger::LOG_MINOR,
                  "ThreadSafeContext::save_context:"
                  " record has the correct prefix.\n");

              prefix_found = true;
            }
          }

          if (!prefix_found)
          {
            madara_logger_ptr_log(logger_, logger::LOG_MINOR,
                "ThreadSafeContext::save_context:"
                " record has the wrong prefix. Rejected.\n");

            continue;
          }
        }

        auto pre_write = current;
        current = i->second.write(current, i->first, buffer_remaining);
        size_t encoded_size = current - pre_write;

        ++checkpoint_header.updates;
        // meta.size += encoded_size;
        checkpoint_header.size += encoded_size;
      }
    }

    // write the final sizes
    current = checkpoint_header.write(
        buffer.get_ptr() + (int)FileHeader::encoded_size(), max_buffer);

    // call decode with any buffer filters
    int total =
        settings.encode(buffer.get_ptr() + (int)FileHeader::encoded_size(),
            (int)checkpoint_header.size, (int)max_buffer);

    if (total < 0)
    {
      throw exceptions::FilterException("ThreadSafeContext::save_context: "
                                        "encode() returned -1 size. Cannot "
                                        "save a negative sized checkpoint.");
    }

    // if (settings.buffer_filters.size () > 0)
    // {
    //   total += (int)filters::BufferFilterHeader::encoded_size ();
    // }

    meta.size = (uint64_t)total;

    current = meta.write(buffer.get_ptr(), max_buffer);

    // update the meta data at the front
    fseek(file, 0, SEEK_SET);

    madara_logger_ptr_log(logger_, logger::LOG_MINOR,
        "ThreadSafeContext::save_context:"
        " encoding with buffer filters: %d:%d bytes written to offset %d.\n",
        (int)meta.size, (int)checkpoint_header.size,
        (int)FileHeader::encoded_size());

    fwrite(buffer.get_ptr(), (size_t)total + (size_t)FileHeader::encoded_size(),
        1, file);

    fclose(file);
  }
  else
  {
    madara_logger_ptr_log(logger_, logger::LOG_MINOR,
        "ThreadSafeContext::save_context:"
        " couldn't open context file: %s.\n",
        settings.filename.c_str());

    return -1;
  }

  return meta.size;
}

int64_t ThreadSafeContext::save_as_karl(const std::string& filename) const
{
  CheckpointSettings settings;
  settings.filename = filename;

  return save_as_karl(settings);
}

int64_t ThreadSafeContext::save_as_karl(
    const CheckpointSettings& settings) const
{
  madara_logger_ptr_log(logger_, logger::LOG_MINOR,
      "ThreadSafeContext::save_as_karl:"
      " opening file %s\n",
      settings.filename.c_str());

  int64_t bytes_written(0);
  std::stringstream buffer;
  std::ofstream file;
  file.open(settings.filename.c_str(), std::ios::binary);

  if (file.is_open())
  {
    // lock the context
    MADARA_GUARD_TYPE guard(mutex_);

    for (KnowledgeMap::const_iterator i = map_.begin(); i != map_.end(); ++i)
    {
      if (i->second.exists())
      {
        // check if the prefix is allowed
        if (settings.prefixes.size() > 0)
        {
          madara_logger_ptr_log(logger_, logger::LOG_MINOR,
              "ThreadSafeContext::save_as_karl:"
              " we have %d prefixes to check against.\n",
              (int)settings.prefixes.size());

          bool prefix_found = false;
          for (size_t j = 0; j < settings.prefixes.size() && !prefix_found; ++j)
          {
            madara_logger_ptr_log(logger_, logger::LOG_MINOR,
                "ThreadSafeContext::save_as_karl:"
                " checking record %s against prefix %s.\n",
                i->first.c_str(), settings.prefixes[j].c_str());

            if (madara::utility::begins_with(i->first, settings.prefixes[j]))
            {
              madara_logger_ptr_log(logger_, logger::LOG_MINOR,
                  "ThreadSafeContext::save_as_karl:"
                  " the record has the correct prefix.\n");

              prefix_found = true;
            }
          }

          if (!prefix_found)
          {
            madara_logger_ptr_log(logger_, logger::LOG_MINOR,
                "ThreadSafeContext::save_as_karl:"
                " the record does not have a correct prefix.\n");

            continue;
          }
        }

        buffer << i->first;
        buffer << "=";

        if (!i->second.is_binary_file_type())
        {
          // record is a non binary file type
          if (i->second.is_string_type())
          {
            // strings require quotation marks
            buffer << "\"";
          }
          else if (i->second.type() ==
                       knowledge::KnowledgeRecord::INTEGER_ARRAY ||
                   i->second.type() == knowledge::KnowledgeRecord::DOUBLE_ARRAY)
          {
            // arrays require brackets
            buffer << "[";
          }

          buffer << i->second;
          if (i->second.is_string_type())
          {
            // strings require quotation marks
            buffer << "\"";
          }
          else if (i->second.type() ==
                       knowledge::KnowledgeRecord::INTEGER_ARRAY ||
                   i->second.type() == knowledge::KnowledgeRecord::DOUBLE_ARRAY)
          {
            // arrays require brackets
            buffer << "]";
          }
        }
        else
        {
          buffer << "#read_file ('";

          std::string path = utility::extract_path(settings.filename);

          if (path == "")
            path = ".";

          path += "/";
          path += i->first;

          if (i->second.type() == knowledge::KnowledgeRecord::IMAGE_JPEG)
          {
            path += ".jpg";
          }
          else
          {
            path += ".dat";
          }

          utility::write_file(
              path, (void*)&(*i->second.file_value_)[0], i->second.size());
          buffer << path;

          buffer << "')";
        }

        buffer << ";\n";
      }
    }

    std::string result = buffer.str();

    if (settings.buffer_filters.size() > 0)
    {
      char* result_copy = new char[settings.buffer_size];
      memcpy(result_copy, result.c_str(), result.size() + 1);

      int size = settings.encode(
        result_copy, (int)result.size(), (int)settings.buffer_size);

      if (size < 0)
      {
        throw exceptions::FilterException(
            "ThreadSafeContext::save_as_karl: "
            "encode() returned -1. Incorrect filter.");
      }

      file.write(result_copy, size);
      bytes_written = (int64_t)size;
    }
    else
    {
      file.write(result.c_str(), result.size());
      bytes_written = (int64_t)result.size();
    }

    file.close();
  }
  else
  {
    madara_logger_ptr_log(logger_, logger::LOG_MINOR,
        "ThreadSafeContext::save_as_karl:"
        " couldn't open karl file: %s.\n",
        settings.filename.c_str());

    return -1;
  }

  return bytes_written;
}

int64_t ThreadSafeContext::save_as_json(const std::string& filename) const
{
  CheckpointSettings settings;
  settings.filename = filename;

  return save_as_json(settings);
}

int64_t ThreadSafeContext::save_as_json(
    const CheckpointSettings& settings) const
{
  madara_logger_ptr_log(logger_, logger::LOG_MINOR,
      "ThreadSafeContext::save_as_json:"
      " opening file %s\n",
      settings.filename.c_str());

  int64_t bytes_written(0);

  std::stringstream buffer;
  std::ofstream file;
  file.open(settings.filename.c_str());

  if (file.is_open())
  {
    // lock the context
    MADARA_GUARD_TYPE guard(mutex_);

    buffer << "{\n";

    for (KnowledgeMap::const_iterator i = map_.begin(); i != map_.end(); ++i)
    {
      if (i->second.exists())
      {
        // check if the prefix is allowed
        if (settings.prefixes.size() > 0)
        {
          madara_logger_ptr_log(logger_, logger::LOG_MINOR,
              "ThreadSafeContext::save_as_json:"
              " we have %d prefixes to check against.\n",
              (int)settings.prefixes.size());

          bool prefix_found = false;
          for (size_t j = 0; j < settings.prefixes.size() && !prefix_found; ++j)
          {
            madara_logger_ptr_log(logger_, logger::LOG_MINOR,
                "ThreadSafeContext::save_as_json:"
                " checking record %s against prefix %s.\n",
                i->first.c_str(), settings.prefixes[j].c_str());

            if (madara::utility::begins_with(i->first, settings.prefixes[j]))
            {
              madara_logger_ptr_log(logger_, logger::LOG_MINOR,
                  "ThreadSafeContext::save_as_json:"
                  " the record has the correct prefix.\n");

              prefix_found = true;
            }
          }

          if (!prefix_found)
          {
            madara_logger_ptr_log(logger_, logger::LOG_MINOR,
                "ThreadSafeContext::save_as_json:"
                " the record does not have a correct prefix.\n");

            continue;
          }
        }

        buffer << "  \"";
        buffer << i->first;
        buffer << "\" : ";

        if (i->second.is_any_type())
        {
          const Any& any = i->second.get_any_cref();
          /*
          const char *tag = any.tag();
          json_oarchive json_out(buffer);
          json_out.setNextName(tag ?
              (std::string("Any<") + tag + ">").c_str() :
              "Any<UKNOWN_ANY_TYPE>");*/
          any.serialize_json(buffer);
        }
        else if (!i->second.is_binary_file_type())
        {
          // record is a non binary file type
          if (i->second.is_string_type())
          {
            // strings require quotation marks
            buffer << "\"";
          }
          else if (i->second.type() ==
                       knowledge::KnowledgeRecord::INTEGER_ARRAY ||
                   i->second.type() == knowledge::KnowledgeRecord::DOUBLE_ARRAY)
          {
            // arrays require brackets
            buffer << "[";
          }

          buffer << i->second;
          if (i->second.is_string_type())
          {
            // strings require quotation marks
            buffer << "\"";
          }
          else if (i->second.type() ==
                       knowledge::KnowledgeRecord::INTEGER_ARRAY ||
                   i->second.type() == knowledge::KnowledgeRecord::DOUBLE_ARRAY)
          {
            // arrays require brackets
            buffer << "]";
          }
        }
        else
        {
          buffer << "#read_file ('";

          std::string path = utility::extract_path(settings.filename);

          if (path == "")
            path = ".";

          path += "/";
          path += i->first;

          if (i->second.type() == knowledge::KnowledgeRecord::IMAGE_JPEG)
          {
            path += ".jpg";
          }
          else
          {
            path += ".dat";
          }

          utility::write_file(
              path, (void*)&(*i->second.file_value_)[0], i->second.size());
          buffer << path;

          buffer << "')";
        }

        KnowledgeMap::const_iterator j(i);

        if (++j != map_.end())
          buffer << ",\n";
      }
    }

    buffer << "\n}\n";

    std::string result = buffer.str();
    file << result;

    bytes_written = (int64_t)result.size();

    file.close();
  }
  else
  {
    madara_logger_ptr_log(logger_, logger::LOG_MINOR,
        "ThreadSafeContext::save_as_json:"
        " couldn't open json file: %s.\n",
        settings.filename.c_str());

    return -1;
  }

  return bytes_written;
}

int64_t ThreadSafeContext::load_context(const std::string& filename,
    std::string& id, const KnowledgeUpdateSettings& settings)
{
  CheckpointSettings checkpoint_settings;
  checkpoint_settings.filename = filename;
  checkpoint_settings.originator = id;

  return load_context(checkpoint_settings, settings);
}

int64_t ThreadSafeContext::load_context(const std::string& filename,
    FileHeader& meta, const KnowledgeUpdateSettings& settings)
{
  madara_logger_ptr_log(logger_, logger::LOG_MAJOR,
      "ThreadSafeContext::load_context:"
      " opening file %s for just header info\n",
      filename.c_str());

  // read the initial FileHeader in
  FILE* file = fopen(filename.c_str(), "rb");

  int64_t total_read(0);

  if (file)
  {
    int64_t max_buffer(102800);
    int64_t buffer_remaining(max_buffer);

    utility::ScopedArray<char> buffer = new char[max_buffer];
    const char* current = buffer.get_ptr();

    madara_logger_ptr_log(logger_, logger::LOG_MINOR,
        "ThreadSafeContext::load_context:"
        " reading file meta data\n");

    total_read = fread(buffer.get_ptr(), 1, max_buffer, file);
    buffer_remaining = (int64_t)total_read;

    if (total_read > FileHeader::encoded_size() &&
        FileHeader::file_header_test(current))
    {
      // if there was something in the file, and it was the right header

      current = meta.read(current, buffer_remaining);

    }  // end if total_read > 0
    else
    {
      madara_logger_ptr_log(logger_, logger::LOG_MINOR,
          "ThreadSafeContext::load_context:"
          " invalid file or wrong version. No contextual change.\n");
    }

    fclose(file);
  }
  else
  {
    madara_logger_ptr_log(logger_, logger::LOG_ALWAYS,
        "ThreadSafeContext::load_context:"
        " could not open file %s for reading. "
        "Check that file exists and that permissions are appropriate.\n",
        filename.c_str());
  }

  CheckpointSettings checkpoint_settings;
  checkpoint_settings.filename = filename;

  return load_context(checkpoint_settings, settings);
}

madara::knowledge::KnowledgeRecord ThreadSafeContext::evaluate_file(
    CheckpointSettings& checkpoint_settings,
    const KnowledgeUpdateSettings& update_settings)
{
  madara_logger_ptr_log(logger_, logger::LOG_MAJOR,
      "ThreadSafeContext::evaluate_file:"
      " opening file %s\n",
      checkpoint_settings.filename.c_str());

#ifndef _MADARA_NO_KARL_

  CompiledExpression expression = compile(file_to_string(checkpoint_settings));

  return evaluate(expression, update_settings);
#else // if KARL has been disabled
  KnowledgeRecord result;
  return result;
#endif
}

std::string ThreadSafeContext::file_to_string(
    CheckpointSettings& checkpoint_settings)
{
  madara_logger_ptr_log(logger_, logger::LOG_MAJOR,
      "ThreadSafeContext::file_to_string:"
      " opening file %s\n",
      checkpoint_settings.filename.c_str());

  FILE* file = fopen(checkpoint_settings.filename.c_str(), "rb");

  int64_t total_read(0);

  if (checkpoint_settings.clear_knowledge)
  {
    this->clear();
  }

  if (file)
  {
    FileHeader meta;
    int64_t max_buffer(checkpoint_settings.buffer_size);

    utility::ScopedArray<char> buffer = new char[max_buffer];

    total_read = fread(buffer.get_ptr(), 1, max_buffer, file);

    madara_logger_ptr_log(logger_, logger::LOG_MAJOR,
        "ThreadSafeContext::file_to_string:"
        " reading file: %d bytes read. Decoding...\n",
        (int)total_read);

    // call decode with any buffer filters
    int size = checkpoint_settings.decode(
        buffer.get_ptr(), (int)total_read, (int)max_buffer);

    if (size < 0)
    {
      throw exceptions::FilterException(
          "ThreadSafeContext::file_to_string: "
          "decode () returned a negative encoding size. Bad filter/encode.");
    }

    madara_logger_ptr_log(logger_, logger::LOG_MINOR,
        "ThreadSafeContext::file_to_string:"
        " decoded %d bytes. Converting to string.\n",
        size);

    std::string script(buffer.get(), (size_t)size);

    madara_logger_ptr_log(logger_, logger::LOG_MINOR,
        "ThreadSafeContext::file_to_string:"
        " reading file: %d bytes read.\n",
        (int)total_read);

    madara_logger_ptr_log(logger_, logger::LOG_DETAILED,
        "ThreadSafeContext::file_to_string:"
        " file_contents: %s.\n",
        script.c_str());

    fclose(file);

    return script;
  }

  return std::string();
}

int64_t ThreadSafeContext::load_context(CheckpointSettings& checkpoint_settings,
    const KnowledgeUpdateSettings& update_settings)
{
  CheckpointReader reader(checkpoint_settings);

  reader.start();

  if (!reader.is_open())
  {
    return 0;
  }

  if (checkpoint_settings.clear_knowledge)
  {
    this->clear();
  }

  for (;;)
  {
    auto cur = reader.next();
    if (cur.first.empty())
    {
      return reader.get_total_read();
    }

    cur.second.clock = clock_;
    update_record_from_external(cur.first, cur.second, update_settings);
  }
}

static uint64_t update_checkpoint_header(logger::Logger* logger_,
    uint64_t clock_, const CheckpointSettings& settings, std::fstream& file,
    FileHeader& meta, transport::MessageHeader& checkpoint_header,
    int64_t& buffer_remaining, utility::ScopedArray<char>& buffer)
{
  char* current = buffer.get_ptr();
  const char* meta_reader = current;

  // read the meta data at the front
  // fseek (file, 0, SEEK_SET);
  file.seekg(0, file.beg);
  // size_t ret = fread (current, meta.encoded_size (), 1, file);

  if (!file.read(buffer.get(), FileHeader::encoded_size()))
  {
    madara_logger_ptr_log(logger_, logger::LOG_ERROR,
        "ThreadSafeContext::save_checkpoint:"
        " failed to read existing file header: size=%d\n",
        (int)meta.encoded_size());

    throw exceptions::MemoryException(
        "ThreadSafeContext::save_checkpoint:"
        "Checkpoint file appears to have been corrupted. Bad header.");
  }

  meta_reader = meta.read(meta_reader, buffer_remaining);

  madara_logger_ptr_log(logger_, logger::LOG_MINOR,
      "ThreadSafeContext::save_checkpoint:"
      " init file meta: size=%d, states=%d\n",
      (int)meta.size, (int)meta.states);

  if (settings.originator != "")
  {
    madara_logger_ptr_log(logger_, logger::LOG_MINOR,
        "ThreadSafeContext::save_checkpoint:"
        " setting file meta id to %s\n",
        settings.originator.c_str());

    strncpy(meta.originator, settings.originator.c_str(),
        sizeof(meta.originator) < settings.originator.size() + 1
            ? sizeof(meta.originator)
            : settings.originator.size() + 1);
  }

  // save the spot where the file ends
  uint64_t checkpoint_start = meta.size + (uint64_t)FileHeader::encoded_size();

  checkpoint_header.size = checkpoint_header.encoded_size();

  madara_logger_ptr_log(logger_, logger::LOG_MINOR,
      "ThreadSafeContext::save_checkpoint:"
      " meta.size=%d, chkpt.header.size=%d \n",
      (int)meta.size, (int)checkpoint_header.size);

  if (settings.override_timestamp)
  {
    meta.initial_timestamp = settings.initial_timestamp;
    meta.last_timestamp = settings.last_timestamp;
  }

  if (settings.override_lamport)
  {
    checkpoint_header.clock = settings.initial_lamport_clock;
  }
  else
  {
    checkpoint_header.clock = clock_;
  }

  return checkpoint_start;
}

static char* init_checkpoint_header(logger::Logger* logger_, uint64_t clock_,
    const CheckpointSettings& settings, FileHeader& meta,
    transport::MessageHeader& checkpoint_header, int64_t& buffer_remaining,
    utility::ScopedArray<char>& buffer)
{
  char* current = buffer.get_ptr();

  madara_logger_ptr_log(logger_, logger::LOG_MINOR,
      "ThreadSafeContext::save_checkpoint:"
      " creating file meta. file.meta.size=%d, state.size=%d\n",
      (int)meta.size, (int)checkpoint_header.encoded_size());

  meta.size += checkpoint_header.encoded_size();
  checkpoint_header.size = checkpoint_header.encoded_size();

  if (settings.override_timestamp)
  {
    meta.initial_timestamp = settings.initial_timestamp;
    meta.last_timestamp = settings.last_timestamp;
  }

  current = meta.write(current, buffer_remaining);

  if (settings.override_lamport)
  {
    checkpoint_header.clock = settings.initial_lamport_clock;
  }
  else
  {
    checkpoint_header.clock = clock_;
  }

  current = checkpoint_header.write(current, buffer_remaining);

  return current;
}

static void checkpoint_write_record(logger::Logger* logger_,
    const std::string& name, const KnowledgeRecord* record,
    const CheckpointSettings& settings,
    transport::MessageHeader& checkpoint_header, char*& current,
    utility::ScopedArray<char>& buffer, int64_t& buffer_remaining)
{
  if (record->exists())
  {
    // check if the prefix is allowed
    if (settings.prefixes.size() > 0)
    {
      madara_logger_ptr_log(logger_, logger::LOG_MINOR,
          "ThreadSafeContext::save_checkpoint:"
          " we have %d prefixes to check against.\n",
          (int)settings.prefixes.size());

      bool prefix_found = false;
      for (size_t j = 0; j < settings.prefixes.size() && !prefix_found; ++j)
      {
        madara_logger_ptr_log(logger_, logger::LOG_MINOR,
            "ThreadSafeContext::save_checkpoint:"
            " checking record %s against prefix %s.\n",
            name.c_str(), settings.prefixes[j].c_str());

        if (madara::utility::begins_with(name, settings.prefixes[j]))
        {
          prefix_found = true;
        }
      }  // end for j->prefixes.size

      if (!prefix_found)
      {
        madara_logger_ptr_log(logger_, logger::LOG_MINOR,
            "ThreadSafeContext::save_checkpoint:"
            " record has the wrong prefix. Rejected.\n");

        return;
      }
    }  // end if prefixes exists

    // get the encoded size of the record for checking buffer boundaries
    int64_t encoded_size = record->get_encoded_size(name);

    madara_logger_ptr_log(logger_, logger::LOG_MINOR,
        "ThreadSafeContext::save_checkpoint:"
        " estimated encoded size of update=%d bytes\n",
        (int)encoded_size);

    if (encoded_size > buffer_remaining)
    {
      throw exceptions::MemoryException(
          "ThreadSafeContext::save_checkpoint: "
          "%d buffer size is not big enough. Partial encoded size "
          "already hit %d bytes. CheckpointSettings.buffer_size is "
          "needs to be larger.");
    }  // end if larger than buffer remaining

    auto pre_write = current;
    try
    {
      current = record->write(current, name, buffer_remaining);
    }
    catch (exceptions::BadAnyAccess& e)
    {
      madara_logger_ptr_log(logger_, logger::LOG_ERROR,
          "ThreadSafeContext::write_record: Caught\n"
          "%s \n"
          "While writing %s\n",
          e.what(), name.c_str());
      throw e;
    }
    encoded_size = current - pre_write;

    ++checkpoint_header.updates;
    checkpoint_header.size += (uint64_t)encoded_size;

    madara_logger_ptr_log(logger_, logger::LOG_MINOR,
        "ThreadSafeContext::save_checkpoint:"
        " chkpt.header.size=%d, current->buffer delta=%d\n",
        (int)checkpoint_header.size, (int)(current - buffer.get_ptr()));
  }  // if record exists
}

namespace
{
class ContextLocalModifiedsLister : public VariablesLister
{
public:
  ContextLocalModifiedsLister(const ThreadSafeContext& context)
    : context_(&context)
  {
  }

  void start(const CheckpointSettings& settings) override
  {
    guard_.reset(new ContextGuard(*context_));
    map_ = context_->get_local_modified();
    iter_ = map_.begin();

    if (settings.reset_checkpoint)
    {
      clear_modifieds_ = true;
    }
  }

  std::pair<const char*, const KnowledgeRecord*> next() override
  {
    std::pair<const char*, const KnowledgeRecord*> ret{nullptr, nullptr};

    if (iter_ == map_.end())
    {
      guard_.reset();
      if (clear_modifieds_)
      {
        context_->reset_checkpoint();
      }
      return ret;
    }

    ret.first = iter_->first;
    ret.second = iter_->second.get_record_unsafe();

    ++iter_;

    return ret;
  }

private:
  const ThreadSafeContext* context_;
  std::unique_ptr<ContextGuard> guard_;
  VariableReferenceMap map_;
  VariableReferenceMap::iterator iter_;
  bool clear_modifieds_ = false;
};
}

static void checkpoint_write_records(const ThreadSafeContext& context,
    logger::Logger* logger_, const CheckpointSettings& settings,
    transport::MessageHeader& checkpoint_header, char*& current,
    utility::ScopedArray<char>& buffer, int64_t& buffer_remaining)
{
  ContextLocalModifiedsLister default_lister(context);

  VariablesLister* lister = settings.variables_lister;

  if (!lister)
  {
    lister = &default_lister;
  }

  lister->start(settings);
  for (;;)
  {
    auto e = lister->next();
    if (e.second == nullptr)
    {
      break;
    }

    auto record = e.second;

    checkpoint_write_record(logger_, e.first, record, settings,
        checkpoint_header, current, buffer, buffer_remaining);
  }
}

static void checkpoint_do_incremental(const ThreadSafeContext& context,
    logger::Logger* logger_, uint64_t clock_,
    const CheckpointSettings& settings, std::fstream& file, FileHeader& meta,
    transport::MessageHeader& checkpoint_header)
{
  int64_t total_written(0);

  int64_t max_buffer(settings.buffer_size);
  int64_t buffer_remaining(max_buffer);
  utility::ScopedArray<char> buffer = new char[max_buffer];

  uint64_t checkpoint_start = update_checkpoint_header(logger_, clock_,
      settings, file, meta, checkpoint_header, buffer_remaining, buffer);

  if (settings.variables_lister != nullptr ||
      context.get_local_modified().size() != 0)
  {
    // skip over the checkpoint header. We'll write this later with the records

    madara_logger_ptr_log(logger_, logger::LOG_MINOR,
        "ThreadSafeContext::save_checkpoint:"
        " fseek set to %d\n",
        (int)(checkpoint_start));

    // set the file pointer to the checkpoint header start
    // fseek (file, (long)checkpoint_start, SEEK_SET);
    file.seekp(checkpoint_start);

    // start updates just past the checkpoint header's buffer location
    char* current = checkpoint_header.write(buffer.get_ptr(), buffer_remaining);

    madara_logger_ptr_log(logger_, logger::LOG_MINOR,
        "ThreadSafeContext::save_checkpoint:"
        " chkpt.header.size=%d, current->buffer delta=%d\n",
        (int)checkpoint_header.encoded_size(),
        (int)(current - buffer.get_ptr()));

    checkpoint_write_records(context, logger_, settings, checkpoint_header,
        current, buffer, buffer_remaining);

    ++meta.states;

    madara_logger_ptr_log(logger_, logger::LOG_MINOR,
        "ThreadSafeContext::save_checkpoint:"
        " writing final data to checkpoint for state #%d\n",
        (int)meta.states);

    madara_logger_ptr_log(logger_, logger::LOG_MINOR,
        "ThreadSafeContext::save_checkpoint:"
        " chkpt.header: size=%d, updates=%d\n",
        (int)checkpoint_header.size, (int)checkpoint_header.updates);

    int total_encoded = 0;

    if (buffer_remaining != max_buffer)
    {
      total_written = (int64_t)(current - buffer.get_ptr());

      madara_logger_ptr_log(logger_, logger::LOG_MINOR,
          "ThreadSafeContext::save_checkpoint:"
          " encoding %d bytes in checkpoint\n",
          (int)total_written);

      current = checkpoint_header.write(buffer.get_ptr(), buffer_remaining);

      // call decode with any buffer filters
      total_encoded = settings.encode(
          buffer.get_ptr(), (int)total_written, (int)max_buffer);

      if (total_encoded < 0)
      {
        throw exceptions::FilterException(
            "ThreadSafeContext::save_context: "
            "encode () returned a negative encoding size. Bad filter/encode.");
      }

      madara_logger_ptr_log(logger_, logger::LOG_MINOR,
          "ThreadSafeContext::save_checkpoint:"
          " encoded %d bytes in buffer. Writing to disk at offset %d bytes\n",
          (int)total_encoded, (int)checkpoint_start);

      // if (settings.buffer_filters.size () > 0)
      // {
      //   total_encoded += (int)filters::BufferFilterHeader::encoded_size ();
      // }

      file.write(buffer.get_ptr(), total_encoded);
      // fwrite (buffer.get_ptr (), (size_t)(total_encoded), 1, file);

      meta.size += (uint64_t)total_encoded;

      madara_logger_ptr_log(logger_, logger::LOG_MINOR,
          "ThreadSafeContext::save_checkpoint:"
          " meta.size updated to %d bytes\n",
          total_encoded, (int)meta.size);
    }

    buffer_remaining = max_buffer;
    // fseek (file, (long)checkpoint_start, SEEK_SET);
    file.seekp(0, file.beg);

    madara_logger_ptr_log(logger_, logger::LOG_MINOR,
        "ThreadSafeContext::save_checkpoint:"
        " new file meta: size=%d, states=%d, "
        " lastchkpt.start=%d, lastchkpt.size=%d, encoded=%d\n",
        (int)meta.size, (int)meta.states, (int)checkpoint_start,
        (int)checkpoint_header.size, (int)total_encoded);

    // update the meta data at the front
    // fseek (file, 0, SEEK_SET);

    madara_logger_ptr_log(logger_, logger::LOG_MINOR,
        "ThreadSafeContext::save_checkpoint:"
        " updating file meta data at beginning in the file\n");
    buffer_remaining = max_buffer;
    current = meta.write(buffer.get_ptr(), buffer_remaining);

    // fwrite (buffer.get_ptr (), current - buffer.get_ptr (), 1, file);
    file.write(buffer.get(), FileHeader::encoded_size());

  }  // if there are local checkpointing records

  // fclose (file);
  file.close();
}

static void checkpoint_do_initial(const ThreadSafeContext& context,
    logger::Logger* logger_, uint64_t clock_,
    const CheckpointSettings& settings, std::fstream& file, FileHeader& meta,
    transport::MessageHeader& checkpoint_header)
{
  int file_header_size = (int)FileHeader::encoded_size();

  int64_t max_buffer(settings.buffer_size);
  int64_t buffer_remaining(max_buffer);
  utility::ScopedArray<char> buffer = new char[max_buffer];

  char* current = init_checkpoint_header(logger_, clock_, settings, meta,
      checkpoint_header, buffer_remaining, buffer);

  madara_logger_ptr_log(logger_, logger::LOG_MINOR,
      "ThreadSafeContext::save_checkpoint:"
      " writing diff records\n");

  checkpoint_write_records(context, logger_, settings, checkpoint_header,
      current, buffer, buffer_remaining);

  char* final_position = current;
  int full_buffer = final_position - buffer.get_ptr();

  madara_logger_ptr_log(logger_, logger::LOG_MINOR,
      "ThreadSafeContext::save_checkpoint:"
      " final_position indicates a buffer of %d bytes,"
      " encode buffer is %d bytes\n",
      full_buffer, full_buffer - file_header_size);

  // write the final sizes for the checkpoint at [108]
  current =
      checkpoint_header.write(buffer.get_ptr() + file_header_size, max_buffer);

  // call decode with any buffer filters on [108]
  int total =
      settings.encode(buffer.get_ptr() + (int)FileHeader::encoded_size(),
          (int)(full_buffer - file_header_size), (int)max_buffer);

  if (total < 0)
  {
    throw exceptions::FilterException(
        "ThreadSafeContext::save_checkpoint: "
        "encode () returned a negative encoding size. Bad filter/encode.");
  }

  meta.size = (uint64_t)total;

  current = meta.write(buffer.get_ptr(), max_buffer);

  // update the meta data at the front
  // fseek (file, 0, SEEK_SET);
  file.seekp(0, file.beg);

  madara_logger_ptr_log(logger_, logger::LOG_MINOR,
      "ThreadSafeContext::save_checkpoint:"
      " encoding with buffer filters: meta.size=%d, chkpt.size=%d, "
      "encoded=%d, chkpt.offset=%d.\n",
      (int)meta.size, (int)checkpoint_header.size, (int)total,
      file_header_size);

  // fwrite (buffer.get_ptr (),
  //   (size_t)total + (size_t)FileHeader::encoded_size (), 1, file);
  file.write(buffer.get(), (size_t)total + (size_t)file_header_size);

  madara_logger_ptr_log(logger_, logger::LOG_MINOR,
      "ThreadSafeContext::save_checkpoint:"
      " wrote: %d bytes to file from beginning.\n",
      (int)total + (int)FileHeader::encoded_size());

  // fclose (file);
  file.close();
}

int64_t ThreadSafeContext::save_checkpoint(
    const CheckpointSettings& settings) const
{
  madara_logger_ptr_log(logger_, logger::LOG_MAJOR,
      "ThreadSafeContext::save_checkpoint:"
      " opening file %s\n",
      settings.filename.c_str());

  // FILE * file = fopen (settings.filename.c_str (), "rb+");
  std::fstream file(
      settings.filename, std::ios::in | std::ios::out | std::ios::binary);

  FileHeader meta;
  transport::MessageHeader checkpoint_header;
  // int chkpt_header_size = (int)checkpoint_header.encoded_size ();

  if (file)
  {
    checkpoint_do_incremental(
        *this, logger_, clock_, settings, file, meta, checkpoint_header);
  }  // if file is opened
  else
  {
    madara_logger_ptr_log(logger_, logger::LOG_MINOR,
        "ThreadSafeContext::save_checkpoint:"
        " checkpoint doesn't exist. Creating.\n");

    file.open(settings.filename, std::ios::out | std::ios::binary);

    meta.states = 1;
    strncpy(meta.originator, settings.originator.c_str(),
        sizeof(meta.originator) < settings.originator.size() + 1
            ? sizeof(meta.originator)
            : settings.originator.size() + 1);

    // if the new file creation for wb was unsuccessful
    if (!file)
    {
      madara_logger_ptr_log(logger_, logger::LOG_MINOR,
          "ThreadSafeContext::save_checkpoint:"
          " couldn't create checkpoint file: %s.\n",
          settings.filename.c_str());

      return -1;
    }

    checkpoint_do_initial(
        *this, logger_, clock_, settings, file, meta, checkpoint_header);
  }  // end if we need to create a new file

  return checkpoint_header.size;
}

int64_t ThreadSafeContext::save_checkpoint(
    const std::string& filename, const std::string& id) const
{
  CheckpointSettings settings;
  settings.filename = filename;
  settings.originator = id;

  return save_checkpoint(settings);
}
}
}
