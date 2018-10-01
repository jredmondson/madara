#ifndef _MADARA_THREADSAFECONTEXT_INL_
#define _MADARA_THREADSAFECONTEXT_INL_

/**
 * @file ThreadSafeContext.h
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains inline functions for a thread safe context
 */

#include "madara/expression/Interpreter.h"

#include <sstream>

namespace madara
{
namespace knowledge
{
inline int ThreadSafeContext::read_file(const std::string& key,
    const std::string& filename, const KnowledgeUpdateSettings& settings)
{
  VariableReference variable = get_ref(key, settings);
  return read_file(variable, filename, settings);
}

inline KnowledgeRecord ThreadSafeContext::get(
    const std::string& key, const KnowledgeReferenceSettings& settings) const
{
  const KnowledgeRecord* ret = with(key, settings);
  if (ret) {
    if (ret->has_history()) {
      return ret->get_newest();
    } else {
      return *ret;
    }
  }
  return KnowledgeRecord();
}

inline KnowledgeRecord ThreadSafeContext::get(const VariableReference& variable,
    const KnowledgeReferenceSettings& settings) const
{
  const KnowledgeRecord* ret = with(variable, settings);
  if (ret) {
    if (ret->has_history()) {
      return ret->get_newest();
    } else {
      return *ret;
    }
  }
  return KnowledgeRecord();
}

inline KnowledgeRecord ThreadSafeContext::get_actual(
    const std::string& key, const KnowledgeReferenceSettings& settings) const
{
  const KnowledgeRecord* ret = with(key, settings);
  if (ret) {
    return *ret;
  }
  return KnowledgeRecord();
}

inline KnowledgeRecord ThreadSafeContext::get_actual(
    const VariableReference& variable,
    const KnowledgeReferenceSettings& settings) const
{
  const KnowledgeRecord* ret = with(variable, settings);
  if (ret) {
    return *ret;
  }
  return KnowledgeRecord();
}

inline KnowledgeRecord* ThreadSafeContext::with(
    const std::string& key, const KnowledgeReferenceSettings& settings)
{
  KnowledgeMap::iterator found;

  MADARA_GUARD_TYPE guard(mutex_);

  if (settings.expand_variables) {
    std::string cur_key = expand_statement(key);
    found = map_.find(cur_key);
  } else {
    found = map_.find(key);
  }

  if (found != map_.end()) {
    return &found->second;
  }

  return nullptr;
}

// return the value of a variable
inline KnowledgeRecord* ThreadSafeContext::with(
    const VariableReference& variable, const KnowledgeReferenceSettings&)
{
  MADARA_GUARD_TYPE guard(mutex_);

  return variable.get_record_unsafe();
}

inline const KnowledgeRecord* ThreadSafeContext::with(
    const std::string& key, const KnowledgeReferenceSettings& settings) const
{
  KnowledgeMap::const_iterator found;

  MADARA_GUARD_TYPE guard(mutex_);

  if (settings.expand_variables) {
    std::string cur_key = expand_statement(key);
    found = map_.find(cur_key);
  } else {
    found = map_.find(key);
  }

  if (found != map_.end()) {
    return &found->second;
  }

  return nullptr;
}

// return the value of a variable
inline const KnowledgeRecord* ThreadSafeContext::with(
    const VariableReference& variable, const KnowledgeReferenceSettings&) const
{
  MADARA_GUARD_TYPE guard(mutex_);

  return variable.get_record_unsafe();
}

// return the value of a variable
inline bool ThreadSafeContext::exists(
    const VariableReference& variable, const KnowledgeReferenceSettings&) const
{
  MADARA_GUARD_TYPE guard(mutex_);

  auto record = variable.get_record_unsafe();
  return record && record->exists();
}

// return the value of a variable
inline KnowledgeRecord ThreadSafeContext::retrieve_index(
    const VariableReference& variable, size_t index,
    const KnowledgeReferenceSettings&)
{
  MADARA_GUARD_TYPE guard(mutex_);

  auto record = variable.get_record_unsafe();
  if (record)
    return record->retrieve_index(index);
  else
    return knowledge::KnowledgeRecord();
}

inline KnowledgeRecord ThreadSafeContext::retrieve_index(const std::string& key,
    size_t index, const KnowledgeReferenceSettings& settings)
{
  VariableReference variable = get_ref(key, settings);
  return retrieve_index(variable, index, settings);
}

template<typename T>
inline int ThreadSafeContext::set(
    const std::string& key, T&& value, const KnowledgeUpdateSettings& settings)
{
  VariableReference variable = get_ref(key, settings);
  return set(variable, std::forward<T>(value), settings);
}

template<typename T>
inline int ThreadSafeContext::set(const VariableReference& variable, T&& value,
    const KnowledgeUpdateSettings& settings)
{
  MADARA_GUARD_TYPE guard(mutex_);

  if (variable.is_valid())
    return set_unsafe(variable, std::forward<T>(value), settings);
  else
    return -1;
}

template<typename T>
inline int ThreadSafeContext::set_any(
    const std::string& key, T&& value, const KnowledgeUpdateSettings& settings)
{
  VariableReference variable = get_ref(key, settings);
  return set_any(variable, std::forward<T>(value), settings);
}

template<typename T>
inline int ThreadSafeContext::set_any(const VariableReference& variable,
    T&& value, const KnowledgeUpdateSettings& settings)
{
  MADARA_GUARD_TYPE guard(mutex_);

  if (variable.is_valid())
    return emplace_any_unsafe(
        variable, settings, type<decay_<T>>{}, std::forward<T>(value));
  else
    return -1;
}

template<typename T>
inline int ThreadSafeContext::set(const std::string& key, const T* value,
    uint32_t size, const KnowledgeUpdateSettings& settings)
{
  VariableReference variable = get_ref(key, settings);
  return set(variable, value, size, settings);
}

// set the value of a variable
template<typename T>
inline int ThreadSafeContext::set(const VariableReference& variable,
    const T* value, uint32_t size, const KnowledgeUpdateSettings& settings)
{
  MADARA_GUARD_TYPE guard(mutex_);
  if (variable.is_valid()) {
    return set_unsafe_impl(variable, settings, value, size);
  } else
    return -1;
}

template<typename... Args>
inline int ThreadSafeContext::set_unsafe_impl(const VariableReference& variable,
    const KnowledgeUpdateSettings& settings, Args&&... args)
{
  auto record = variable.get_record_unsafe();

  // check if we have the appropriate write quality
  if (!settings.always_overwrite && record->write_quality < record->quality)
    return -2;
  else
    record->quality = 0;

  record->set_value(std::forward<Args>(args)...);
  record->quality = record->write_quality;
  record->clock = clock_;
  record->set_toi(utility::get_time());

  return 0;
}

template<typename T>
inline int ThreadSafeContext::set_unsafe(const VariableReference& variable,
    T&& value, const KnowledgeUpdateSettings& settings)
{
  int ret = set_unsafe_impl(variable, settings, std::forward<T>(value));

  if (ret == 0)
    mark_and_signal(variable, settings);

  return ret;
}

template<typename... Args>
inline int ThreadSafeContext::emplace_any(const std::string& key,
    const KnowledgeUpdateSettings& settings, Args&&... args)
{
  VariableReference variable = get_ref(key, settings);
  return emplace_any(variable, settings, std::forward<Args>(args)...);
}

template<typename... Args>
inline int ThreadSafeContext::emplace_any(const VariableReference& variable,
    const KnowledgeUpdateSettings& settings, Args&&... args)
{
  MADARA_GUARD_TYPE guard(mutex_);

  if (variable.is_valid())
    return emplace_any_unsafe(variable, settings, std::forward<Args>(args)...);
  else
    return -1;
}

template<typename... Args>
inline int ThreadSafeContext::emplace_any_unsafe_impl(
    const VariableReference& variable, const KnowledgeUpdateSettings& settings,
    Args&&... args)
{
  auto record = variable.get_record_unsafe();

  // check if we have the appropriate write quality
  if (!settings.always_overwrite && record->write_quality < record->quality)
    return -2;
  else
    record->quality = 0;

  record->emplace_any(std::forward<Args>(args)...);
  record->quality = record->write_quality;
  record->clock = clock_;
  record->set_toi(utility::get_time());

  return 0;
}

template<typename... Args>
inline int ThreadSafeContext::emplace_any_unsafe(
    const VariableReference& variable, const KnowledgeUpdateSettings& settings,
    Args&&... args)
{
  int ret =
      emplace_any_unsafe_impl(variable, settings, std::forward<Args>(args)...);

  if (ret == 0)
    mark_and_signal(variable, settings);

  return ret;
}

template<typename T>
// set the value of a variable
inline int ThreadSafeContext::set_unsafe(const VariableReference& variable,
    const T* value, size_t size, const KnowledgeUpdateSettings& settings)
{
  int ret = set_unsafe_impl(variable, settings, value, size);

  if (ret == 0)
    mark_and_signal(variable, settings);

  return ret;
}

inline int ThreadSafeContext::set_xml(const std::string& key, const char* value,
    size_t size, const KnowledgeUpdateSettings& settings)
{
  VariableReference variable = get_ref(key, settings);
  return set_xml(variable, value, size, settings);
}

inline int ThreadSafeContext::set_text(const std::string& key,
    const char* value, size_t size, const KnowledgeUpdateSettings& settings)
{
  VariableReference variable = get_ref(key, settings);
  return set_text(variable, value, size, settings);
}

inline int ThreadSafeContext::set_jpeg(const std::string& key,
    const unsigned char* value, size_t size,
    const KnowledgeUpdateSettings& settings)
{
  VariableReference variable = get_ref(key, settings);
  return set_jpeg(variable, value, size, settings);
}

inline int ThreadSafeContext::set_file(const std::string& key,
    const unsigned char* value, size_t size,
    const KnowledgeUpdateSettings& settings)
{
  VariableReference variable = get_ref(key, settings);
  return set_file(variable, value, size, settings);
}

template<typename T>
inline int ThreadSafeContext::set_index(const std::string& key, size_t index,
    T&& value, const KnowledgeUpdateSettings& settings)
{
  VariableReference variable = get_ref(key, settings);
  return set_index(variable, index, std::forward<T>(value), settings);
}

template<typename T>
inline int ThreadSafeContext::set_index(const VariableReference& variable,
    size_t index, T&& value, const KnowledgeUpdateSettings& settings)
{
  MADARA_GUARD_TYPE guard(mutex_);
  if (variable.is_valid())
    return set_index_unsafe(variable, index, std::forward<T>(value), settings);
  else
    return -1;
}

template<typename T>
inline int ThreadSafeContext::set_index_unsafe_impl(
    const VariableReference& variable, size_t index, T&& value,
    const KnowledgeUpdateSettings& settings)
{
  auto record = variable.get_record_unsafe();
  // check if we have the appropriate write quality
  if (!settings.always_overwrite && record->write_quality < record->quality)
    return -2;
  else
    record->quality = 0;

  record->set_index(index, std::forward<T>(value));
  record->quality = record->write_quality;
  record->clock = clock_;
  record->set_toi(utility::get_time());

  return 0;
}

template<typename T>
inline int ThreadSafeContext::set_index_unsafe(
    const VariableReference& variable, size_t index, T&& value,
    const KnowledgeUpdateSettings& settings)
{
  int ret =
      set_index_unsafe_impl(variable, index, std::forward<T>(value), settings);

  if (ret == 0)
    mark_and_signal(variable, settings);

  return ret;
}

inline KnowledgeRecord ThreadSafeContext::inc(
    const std::string& key, const KnowledgeUpdateSettings& settings)
{
  VariableReference variable = get_ref(key, settings);
  return inc(variable, settings);
}

inline KnowledgeRecord ThreadSafeContext::dec(
    const std::string& key, const KnowledgeUpdateSettings& settings)
{
  VariableReference variable = get_ref(key, settings);
  return dec(variable, settings);
}

inline KnowledgeRecord ThreadSafeContext::inc(
    const VariableReference& variable, const KnowledgeUpdateSettings& settings)
{
  MADARA_GUARD_TYPE guard(mutex_);
  auto record = variable.get_record_unsafe();
  if (record) {
    // check if we have the appropriate write quality
    if (settings.always_overwrite || record->write_quality >= record->quality) {
      ++(*record);
      record->quality = record->write_quality;
      record->clock = clock_;
      record->set_toi(utility::get_time());
      mark_and_signal(variable, settings);
    }

    return *record;
  }

  return knowledge::KnowledgeRecord();
}

#ifndef _MADARA_NO_KARL_

// return whether or not the key exists
inline bool ThreadSafeContext::delete_expression(const std::string& expression)
{
  MADARA_GUARD_TYPE guard(mutex_);

  return interpreter_->delete_expression(expression);
}

#endif  // _MADARA_NO_KARL_

inline bool ThreadSafeContext::clear(
    const std::string& key, const KnowledgeReferenceSettings& settings)
{
  // enter the mutex
  bool found(false);
  std::string key_actual;
  const std::string* key_ptr;
  MADARA_GUARD_TYPE guard(mutex_);

  if (settings.expand_variables) {
    key_actual = expand_statement(key);
    key_ptr = &key_actual;
  } else
    key_ptr = &key;

  // find the key and update found with result of find
  KnowledgeMap::iterator record = map_.find(*key_ptr);
  found = record != map_.end();

  if (found) {
    record->second.clear_value();
  }

  return found;
}

inline bool ThreadSafeContext::clear(const VariableReference& variable)
{
  if (variable.is_valid()) {
    MADARA_GUARD_TYPE guard(mutex_);

    // erase any changed or local changed map entries
    // changed_map_.erase (variable.entry_->first.c_str ());
    // local_changed_map_.erase (variable.entry_->first.c_str ());

    variable.entry_->second.clear_value();

    return true;
  } else {
    return false;
  }
}

// return whether or not the key exists
inline bool ThreadSafeContext::delete_variable(
    const std::string& key, const KnowledgeReferenceSettings& settings)
{
  // enter the mutex
  std::string key_actual;
  bool result(false);

  const std::string* key_ptr;
  MADARA_GUARD_TYPE guard(mutex_);

  if (settings.expand_variables) {
    key_actual = expand_statement(key);
    key_ptr = &key_actual;
  } else
    key_ptr = &key;

  // erase any changed or local changed map entries
  changed_map_.erase(key_ptr->c_str());
  local_changed_map_.erase(key_ptr->c_str());

  // erase the map
  result = map_.erase(*key_ptr) == 1;

  return result;
}

// return whether or not the key exists
inline bool ThreadSafeContext::delete_variable(
    const VariableReference& var, const KnowledgeReferenceSettings&)
{
  // enter the mutex
  MADARA_GUARD_TYPE guard(mutex_);

  // erase any changed or local changed map entries
  changed_map_.erase(var.entry_->first.c_str());
  local_changed_map_.erase(var.entry_->first.c_str());

  // erase the map
  return map_.erase(var.entry_->first.c_str()) == 1;
}

inline void ThreadSafeContext::delete_variables(KnowledgeMap::iterator begin,
    KnowledgeMap::iterator end, const KnowledgeReferenceSettings&)
{
  for (auto cur = begin; cur != end; ++cur) {
    changed_map_.erase(cur->first.c_str());
    local_changed_map_.erase(cur->first.c_str());
  }
  map_.erase(begin, end);
}

// return whether or not the key exists
inline bool ThreadSafeContext::exists(
    const std::string& key, const KnowledgeReferenceSettings& settings) const
{
  // enter the mutex
  std::string key_actual;
  const std::string* key_ptr;
  MADARA_GUARD_TYPE guard(mutex_);

  if (settings.expand_variables) {
    key_actual = expand_statement(key);
    key_ptr = &key_actual;
  } else
    key_ptr = &key;

  // if key is not null
  if (*key_ptr != "") {
    // find the key in the knowledge base
    KnowledgeMap::const_iterator found = map_.find(*key_ptr);

    // if it's found, then return the value
    if (found != map_.end())
      return found->second.status() != knowledge::KnowledgeRecord::UNCREATED;
  }

  // if no match, return empty (0)
  return false;
}

// Atomically decrement a stored value. Only reason we are inlining this
// function is because it is called by only one function, and we can save a bit
// of execution time via expansion into that function call.
inline KnowledgeRecord ThreadSafeContext::dec(
    const VariableReference& variable, const KnowledgeUpdateSettings& settings)
{
  MADARA_GUARD_TYPE guard(mutex_);
  auto record = variable.get_record_unsafe();
  if (record) {
    // check if we have the appropriate write quality
    if (settings.always_overwrite || record->write_quality >= record->quality) {
      --(*record);
      record->quality = record->write_quality;
      record->clock = clock_;
      record->set_toi(utility::get_time());

      mark_and_signal(variable, settings);
    }

    return *record;
  }

  return knowledge::KnowledgeRecord();
}

/// set the lamport clock (updates with lamport clocks lower
/// than our current clock get discarded)
inline uint64_t ThreadSafeContext::set_clock(uint64_t clock)
{
  MADARA_GUARD_TYPE guard(mutex_);

  // clock_ is always increasing. We never reset it to a lower clock value
  // user can check return value to see if the clock was set.
  if (clock_ < clock)
    clock_ = clock;

  return clock_;
}

/// set the lamport clock (updates with lamport clocks lower
/// than our current clock get discarded)
inline uint64_t ThreadSafeContext::set_clock(const std::string& key,
    uint64_t clock, const KnowledgeReferenceSettings& settings)
{
  // enter the mutex
  std::string key_actual;
  const std::string* key_ptr;
  MADARA_GUARD_TYPE guard(mutex_);

  if (settings.expand_variables) {
    key_actual = expand_statement(key);
    key_ptr = &key_actual;
  } else
    key_ptr = &key;

  // check for null key
  if (*key_ptr == "")
    return 0;

  // create the key if it didn't exist
  knowledge::KnowledgeRecord& record = map_[*key_ptr];

  // check for value already set
  if (record.clock < clock) {
    record.clock = clock;

    // try to update the global clock as well
    this->set_clock(clock);
  }

  return record.clock;
}

/// set the lamport clock for a particular variable (updates with
/// lamport clocks lower than our current clock get discarded)
inline uint64_t ThreadSafeContext::inc_clock(
    const std::string& key, const KnowledgeUpdateSettings& settings)
{
  // enter the mutex
  std::string key_actual;
  const std::string* key_ptr;
  MADARA_GUARD_TYPE guard(mutex_);

  if (settings.expand_variables) {
    key_actual = expand_statement(key);
    key_ptr = &key_actual;
  } else
    key_ptr = &key;

  // check for null key
  if (*key_ptr == "")
    return 0;

  // create the key if it didn't exist
  knowledge::KnowledgeRecord& record = map_[*key_ptr];

  return record.clock += settings.clock_increment;
}

/// increment the process lamport clock
inline uint64_t ThreadSafeContext::inc_clock(
    const KnowledgeUpdateSettings& settings)
{
  MADARA_GUARD_TYPE guard(mutex_);
  return clock_ += settings.clock_increment;
}

/// get the lamport clock (updates with lamport clocks lower
/// than our current clock get discarded)
inline uint64_t ThreadSafeContext::get_clock(void) const
{
  MADARA_GUARD_TYPE guard(mutex_);
  return clock_;
}

inline madara::logger::Logger& ThreadSafeContext::get_logger(void) const
{
  MADARA_GUARD_TYPE guard(mutex_);
  return *logger_;
}

inline void ThreadSafeContext::attach_logger(logger::Logger& logger) const
{
  MADARA_GUARD_TYPE guard(mutex_);
  logger_ = &logger;
}

/// get the lamport clock for a particular variable
inline uint64_t ThreadSafeContext::get_clock(
    const std::string& key, const KnowledgeReferenceSettings& settings) const
{
  // enter the mutex
  std::string key_actual;
  const std::string* key_ptr;
  MADARA_GUARD_TYPE guard(mutex_);

  if (settings.expand_variables) {
    key_actual = expand_statement(key);
    key_ptr = &key_actual;
  } else
    key_ptr = &key;

  // check for null key
  if (*key_ptr == "")
    return 0;

  // find the key in the knowledge base
  KnowledgeMap::const_iterator found = map_.find(*key_ptr);

  // if it's found, then compare the value
  if (found != map_.end()) {
    return found->second.clock;
  } else
    // key does not exist
    return 0;
}

/// Lock the mutex on this context. Warning: this will cause
/// all operations to block until the unlock call is made.
inline void ThreadSafeContext::lock(void) const
{
  mutex_.lock();
}

inline bool ThreadSafeContext::try_lock(void) const
{
  return mutex_.try_lock();
}

/// Unlock the mutex on this context.
inline void ThreadSafeContext::unlock(void) const
{
  mutex_.unlock();
}

/// Print a statement, similar to printf (variable expressions allowed)
/// e.g. input = "MyVar{.id} = {MyVar{.id}}\n";
inline void ThreadSafeContext::print(
    const std::string& statement, unsigned int level) const
{
  madara_logger_ptr_log(
      logger_, (int)level, this->expand_statement(statement).c_str());
}

// clear all variables and their values
inline void ThreadSafeContext::clear(bool erase)
{
  // enter the mutex
  MADARA_GUARD_TYPE guard(mutex_);

  changed_map_.clear();
  local_changed_map_.clear();

  if (erase) {
    map_.clear();
  } else {
    for (KnowledgeMap::iterator i = map_.begin(); i != map_.end(); ++i) {
      i->second.reset_value();
    }
  }

  changed_.MADARA_CONDITION_NOTIFY_ONE();
}

/// Make the current thread of execution wait for a change on the
/// context
inline void ThreadSafeContext::wait_for_change(bool extra_release)
{
  // enter the mutex
  MADARA_GUARD_TYPE guard(mutex_);

  // if the caller is relying on a recursive call (e.g. KnowlegeBase::wait),
  // we'll need to call an extra release for this to work. Otherwise, the
  // context would remain locked to the calling thread - even though it will
  // now be put to sleep
  if (extra_release)
    mutex_.MADARA_LOCK_UNLOCK();

  changed_.wait(mutex_);

  // if (extra_release)
  //  mutex_.MADARA_LOCK_LOCK ();
}

inline void ThreadSafeContext::mark_to_send(
    const std::string& key, const KnowledgeUpdateSettings& settings)
{
  VariableReference ref = get_ref(key, settings);
  if (ref.is_valid())
    mark_to_send(ref, settings);
}

inline void ThreadSafeContext::mark_to_send(
    const VariableReference& ref, const KnowledgeUpdateSettings& settings)
{
  MADARA_GUARD_TYPE guard(mutex_);
  if (ref.is_valid()) {
    mark_to_send_unsafe(ref, settings);
  }
}

inline void ThreadSafeContext::mark_to_send_unsafe(
    VariableReference ref, const KnowledgeUpdateSettings&)
{
  changed_map_[ref.get_name()] = std::move(ref);

  KnowledgeRecord& record = *ref.get_record_unsafe();
  if (record.status() != KnowledgeRecord::MODIFIED)
    record.set_modified();
}

inline void ThreadSafeContext::mark_to_checkpoint(
    const std::string& key, const KnowledgeUpdateSettings& settings)
{
  VariableReference ref = get_ref(key, settings);
  if (ref.is_valid())
    mark_to_checkpoint(ref, settings);
}

inline void ThreadSafeContext::mark_to_checkpoint(
    const VariableReference& ref, const KnowledgeUpdateSettings& settings)
{
  MADARA_GUARD_TYPE guard(mutex_);
  if (ref.is_valid()) {
    mark_to_checkpoint_unsafe(ref, settings);
  }
}

inline void ThreadSafeContext::mark_to_checkpoint_unsafe(
    VariableReference ref, const KnowledgeUpdateSettings&)
{
  local_changed_map_[ref.get_name()] = std::move(ref);

  KnowledgeRecord& record = *ref.get_record_unsafe();
  if (record.status() != KnowledgeRecord::MODIFIED)
    record.set_modified();
}

inline void ThreadSafeContext::mark_and_signal(
    VariableReference ref, const KnowledgeUpdateSettings& settings)
{
  // otherwise set the value
  if (ref.get_name()[0] != '.' || settings.treat_locals_as_globals) {
    if (!settings.treat_globals_as_locals) {
      mark_to_send_unsafe(std::move(ref));
    }
  }

  // track local changes now checkpoints all state, not just local
  if (settings.track_local_changes) {
    mark_to_checkpoint_unsafe(std::move(ref));
  }

  if (settings.stream_changes && streamer_ != nullptr) {
    streamer_->enqueue(ref.get_name(), *ref.get_record_unsafe());
  }

  if (settings.signal_changes)
    changed_.MADARA_CONDITION_NOTIFY_ALL();
}

inline void ThreadSafeContext::mark_modified(
    const std::string& key, const KnowledgeUpdateSettings& settings)
{
  VariableReference ref = get_ref(key, settings);
  if (ref.is_valid())
    mark_modified(ref, settings);
}

inline void ThreadSafeContext::mark_modified(
    const VariableReference& ref, const KnowledgeUpdateSettings& settings)
{
  MADARA_GUARD_TYPE guard(mutex_);
  mark_and_signal(ref, settings);
}

inline std::string ThreadSafeContext::debug_modifieds(void) const
{
  MADARA_GUARD_TYPE guard(mutex_);
  std::stringstream result;

  result << changed_map_.size() << " modifications ready to send:\n";

  for (auto& entry : changed_map_) {
    KnowledgeRecord& record = *entry.second.get_record_unsafe();
    if (record.is_binary_file_type()) {
      result << "File: ";
    } else if (record.type() == knowledge::KnowledgeRecord::DOUBLE) {
      result << "Double: ";
    } else if (record.type() == knowledge::KnowledgeRecord::DOUBLE_ARRAY) {
      result << "Double array: ";
    } else if (record.type() == knowledge::KnowledgeRecord::INTEGER) {
      result << "Integer: ";
    } else if (record.type() == knowledge::KnowledgeRecord::INTEGER_ARRAY) {
      result << "Integer array: ";
    } else if (record.is_string_type()) {
      result << "String: ";
    } else {
      result << "Unknown: ";
    }

    result << entry.first << " = " << record.to_string() << "\n";
  }

  return result.str();
}

/// Return list of variables that have been modified
inline const VariableReferenceMap& ThreadSafeContext::get_modifieds(void) const
{
  MADARA_GUARD_TYPE guard(mutex_);

  return changed_map_;
}

inline VariableReferences ThreadSafeContext::save_modifieds(void) const
{
  MADARA_GUARD_TYPE guard(mutex_);

  VariableReferences snapshot;
  snapshot.reserve(changed_map_.size());
  int cur = 0;

  madara_logger_ptr_log(logger_, logger::LOG_MAJOR,
      "ThreadSafeContext::save_modifieds:"
      " changed_map.size=%d, snapshot.size=%d\n",
      (int)changed_map_.size(), (int)snapshot.size());

  for (auto& entry : changed_map_) {
    madara_logger_ptr_log(logger_, logger::LOG_MINOR,
        "ThreadSafeContext::save_modifieds:"
        " snapshot[%d].name=%s\n",
        cur, entry.first);

    snapshot.emplace_back(entry.second);
  }

  return snapshot;
}

inline void ThreadSafeContext::add_modifieds(
    const VariableReferences& modifieds) const
{
  MADARA_GUARD_TYPE guard(mutex_);

  for (auto& entry : modifieds) {
    changed_map_.insert(std::make_pair(entry.get_name(), entry));
  }
}

/// Return list of variables that have been modified
inline const VariableReferenceMap& ThreadSafeContext::get_local_modified(
    void) const
{
  MADARA_GUARD_TYPE guard(mutex_);

  return local_changed_map_;
}

/// Reset all variables to unmodified
inline void ThreadSafeContext::reset_modified(void)
{
  MADARA_GUARD_TYPE guard(mutex_);

  changed_map_.clear();
}

/// Changes all global variables to modified at current time
inline void ThreadSafeContext::apply_modified(void)
{
  MADARA_GUARD_TYPE guard(mutex_);

  // each synchronization counts as an event, since this is a
  // pretty important networking event

  //++this->clock_;

  for (KnowledgeMap::iterator i = map_.begin(); i != map_.end(); ++i)
    for (auto& entry : map_) {
      if (entry.first.size() > 0 && entry.first[0] != '.') {
        // local or global doesn't matter. Clock and modification
        // aren't really a part of local variable checking anyway
        // i->second.status = KnowledgeRecord::MODIFIED;

        if (entry.second.status() != knowledge::KnowledgeRecord::UNCREATED)
          mark_and_signal(&entry, KnowledgeUpdateSettings());
        else
          entry.second.set_value(KnowledgeRecord::Integer(0));

        // i->second.clock = this->clock_;
      }
    }
}

/// Reset a variable to unmodified
inline void ThreadSafeContext::reset_modified(const std::string& variable)
{
  MADARA_GUARD_TYPE guard(mutex_);

  changed_map_.erase(variable.c_str());
}

inline void ThreadSafeContext::reset_checkpoint(void) const
{
  MADARA_GUARD_TYPE guard(mutex_);

  local_changed_map_.clear();
}

/// Signal the condition that it can wake up someone else on changed data.
inline void ThreadSafeContext::signal(bool lock) const
{
  if (lock) {
    MADARA_GUARD_TYPE guard(mutex_);
    changed_.MADARA_CONDITION_NOTIFY_ONE();
  } else
    changed_.MADARA_CONDITION_NOTIFY_ONE();
}

inline void ThreadSafeContext::add_logger(const std::string& filename)
{
  logger_->add_file(filename);
}

inline int ThreadSafeContext::get_log_level(void)
{
  return logger_->get_level();
}

inline void ThreadSafeContext::set_log_level(int level)
{
  logger_->set_level(level);
}
}
}

#endif  // _MADARA_THREADSAFECONTEXT_INL_
