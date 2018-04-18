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


inline madara::knowledge::KnowledgeRecord
madara::knowledge::ThreadSafeContext::get (
  const std::string & key,
  const KnowledgeReferenceSettings & settings) const
{
  std::string cur_key;
  knowledge::KnowledgeRecord result;

  if (settings.expand_variables)
  {
    cur_key = expand_statement (key);
  }
  else
  {
    cur_key = key;
  }

  KnowledgeMap::const_iterator found = map_.find (cur_key);

  if (found != map_.end ())
  {
    return found->second;
  }

  return result;
}


inline int
madara::knowledge::ThreadSafeContext::read_file (
  const std::string & key,
  const std::string & filename,
  const KnowledgeUpdateSettings & settings)
{
  VariableReference variable = get_ref (key, settings);
  return read_file (variable, filename, settings);
}

// return the value of a variable
inline madara::knowledge::KnowledgeRecord
madara::knowledge::ThreadSafeContext::get (
  const VariableReference & variable,
  const KnowledgeReferenceSettings &) const
{
  MADARA_GUARD_TYPE guard (mutex_);

  if (variable.record_)
    return *variable.record_;
  else
    return knowledge::KnowledgeRecord ();
}

// return the value of a variable
inline bool
madara::knowledge::ThreadSafeContext::exists (
  const VariableReference & variable,
  const KnowledgeReferenceSettings &) const
{
  MADARA_GUARD_TYPE guard (mutex_);

  if (variable.record_)
    return variable.record_->exists ();
  else
    return false;
}

// return the value of a variable
inline madara::knowledge::KnowledgeRecord
madara::knowledge::ThreadSafeContext::retrieve_index (
  const VariableReference & variable,
  size_t index,
  const KnowledgeReferenceSettings &)
{
  MADARA_GUARD_TYPE guard (mutex_);

  if (variable.record_)
    return variable.record_->retrieve_index (index);
  else
    return knowledge::KnowledgeRecord ();
}

inline madara::knowledge::KnowledgeRecord
madara::knowledge::ThreadSafeContext::retrieve_index (
  const std::string & key,
  size_t index,
  const KnowledgeReferenceSettings & settings)
{
  VariableReference variable = get_ref (key, settings);
  return retrieve_index (variable, index, settings);
}

inline int
madara::knowledge::ThreadSafeContext::set (
  const std::string & key,
  const madara::knowledge::KnowledgeRecord & value,
  const KnowledgeUpdateSettings & settings)
{
  VariableReference variable = get_ref (key, settings);
  return set (variable, value, settings);
}


inline int
madara::knowledge::ThreadSafeContext::set_xml (
  const std::string & key,
  const char * value, size_t size, 
  const KnowledgeUpdateSettings & settings)
{
  VariableReference variable = get_ref (key, settings);
  return set_xml (variable, value, size, settings);
}

inline int
madara::knowledge::ThreadSafeContext::set_text (
  const std::string & key,
  const char * value, size_t size, 
  const KnowledgeUpdateSettings & settings)
{
  VariableReference variable = get_ref (key, settings);
  return set_text (variable, value, size, settings);
}

inline int
madara::knowledge::ThreadSafeContext::set_jpeg (
  const std::string & key,
  const unsigned char * value, size_t size, 
  const KnowledgeUpdateSettings & settings)
{
  VariableReference variable = get_ref (key, settings);
  return set_jpeg (variable, value, size, settings);
}

inline int
madara::knowledge::ThreadSafeContext::set_file (
  const std::string & key,
  const unsigned char * value, size_t size, 
  const KnowledgeUpdateSettings & settings)
{
  VariableReference variable = get_ref (key, settings);
  return set_file (variable, value, size, settings);
}

inline int
madara::knowledge::ThreadSafeContext::set (
  const std::string & key,
  madara::knowledge::KnowledgeRecord::Integer value, 
  const KnowledgeUpdateSettings & settings)
{
  VariableReference variable = get_ref (key, settings);
  return set (variable, value, settings);
}

inline int
madara::knowledge::ThreadSafeContext::set (
  const std::string & key,
  double value, 
  const KnowledgeUpdateSettings & settings)
{
  VariableReference variable = get_ref (key, settings);
  return set (variable, value, settings);
}

inline int
madara::knowledge::ThreadSafeContext::set (
  const std::string & key,
  const std::string & value, 
  const KnowledgeUpdateSettings & settings)
{
  VariableReference variable = get_ref (key, settings);
  return set (variable, value, settings);
}

inline int
madara::knowledge::ThreadSafeContext::set_index (
  const std::string & key,
  size_t index, knowledge::KnowledgeRecord::Integer value, 
  const KnowledgeUpdateSettings & settings)
{
  VariableReference variable = get_ref (key, settings);
  return set_index (variable, index, value, settings);
}

inline int
madara::knowledge::ThreadSafeContext::set_index (
  const std::string & key,
  size_t index, double value, 
  const KnowledgeUpdateSettings & settings)
{
  VariableReference variable = get_ref (key, settings);
  return set_index (variable, index, value, settings);
}


inline int
madara::knowledge::ThreadSafeContext::set (const std::string & key,
  const madara::knowledge::KnowledgeRecord::Integer * value,
  uint32_t size,
  const KnowledgeUpdateSettings & settings)
{
  VariableReference variable = get_ref (key, settings);
  return set (variable, value, size, settings);
}

inline int
madara::knowledge::ThreadSafeContext::set (const std::string & key,
  const double * value,
  uint32_t size,
  const KnowledgeUpdateSettings & settings)
{
  VariableReference variable = get_ref (key, settings);
  return set (variable, value, size, settings);
}


inline int
madara::knowledge::ThreadSafeContext::set (const std::string & key,
  const std::vector <KnowledgeRecord::Integer> & value,
  const KnowledgeUpdateSettings & settings)
{
  VariableReference variable = get_ref (key, settings);
  return set (variable, value, settings);
}

inline int
madara::knowledge::ThreadSafeContext::set (const std::string & key,
  const std::vector <double> & value,
  const KnowledgeUpdateSettings & settings)
{
  VariableReference variable = get_ref (key, settings);
  return set (variable, value, settings);
}


inline  madara::knowledge::KnowledgeRecord
madara::knowledge::ThreadSafeContext::inc (
  const std::string & key,
  const KnowledgeUpdateSettings & settings)
{
  VariableReference variable = get_ref (key, settings);
  return inc (variable, settings);
}

inline  madara::knowledge::KnowledgeRecord
madara::knowledge::ThreadSafeContext::dec (
  const std::string & key,
  const KnowledgeUpdateSettings & settings)
{
  VariableReference variable = get_ref (key, settings);
  return dec (variable, settings);
}

inline  madara::knowledge::KnowledgeRecord
madara::knowledge::ThreadSafeContext::inc (
  const VariableReference & variable,
  const KnowledgeUpdateSettings & settings)
{
  MADARA_GUARD_TYPE guard (mutex_);
  if (variable.record_)
  {
    // check if we have the appropriate write quality
    if (settings.always_overwrite ||
        variable.record_->write_quality >= variable.record_->quality)
    {
      ++ (*variable.record_);
variable.record_->quality = variable.record_->write_quality;

mark_and_signal (variable.name_.get_ptr (), variable.record_, settings);
    }

    return *variable.record_;
  }

  return knowledge::KnowledgeRecord ();
}

#ifndef _MADARA_NO_KARL_

// return whether or not the key exists
inline bool
madara::knowledge::ThreadSafeContext::delete_expression (
const std::string & expression)
{
  MADARA_GUARD_TYPE guard (mutex_);

  return interpreter_->delete_expression (expression);
}

#endif // _MADARA_NO_KARL_

inline bool
madara::knowledge::ThreadSafeContext::clear (
const std::string & key,
const KnowledgeReferenceSettings & settings)
{
  // enter the mutex
  bool found (false);
  std::string key_actual;
  const std::string * key_ptr;
  MADARA_GUARD_TYPE guard (mutex_);

  if (settings.expand_variables)
  {
    key_actual = expand_statement (key);
    key_ptr = &key_actual;
  }
  else
    key_ptr = &key;

  // find the key and update found with result of find
  KnowledgeMap::iterator record = map_.find (*key_ptr);
  found = record != map_.end ();

  if (found)
  {
    record->second.clear_value ();
  }

  return found;
}

// return whether or not the key exists
inline bool
madara::knowledge::ThreadSafeContext::delete_variable (
const std::string & key,
const KnowledgeReferenceSettings & settings)
{
  // enter the mutex
  std::string key_actual;
  bool result (false);

  const std::string * key_ptr;
  MADARA_GUARD_TYPE guard (mutex_);

  if (settings.expand_variables)
  {
    key_actual = expand_statement (key);
    key_ptr = &key_actual;
  }
  else
    key_ptr = &key;

  // erase the map
  result = map_.erase (*key_ptr) == 1;

  // erase any changed or local changed map entries
  changed_map_.erase (*key_ptr);
  local_changed_map_.erase (*key_ptr);

  return result;
}

// return whether or not the key exists
inline bool
madara::knowledge::ThreadSafeContext::exists (
  const std::string & key,
  const KnowledgeReferenceSettings & settings) const
{
  // enter the mutex
  std::string key_actual;
  const std::string * key_ptr;
  MADARA_GUARD_TYPE guard (mutex_);
  
  if (settings.expand_variables)
  {
    key_actual = expand_statement (key);
    key_ptr = &key_actual;
  }
  else
    key_ptr = &key;

  // if key is not null
  if (*key_ptr != "")
  {
    // find the key in the knowledge base
    KnowledgeMap::const_iterator found = map_.find (*key_ptr);

    // if it's found, then return the value
    if (found != map_.end ())
      return found->second.status () != knowledge::KnowledgeRecord::UNCREATED;
  }

  // if no match, return empty (0)
  return false;
}

// Atomically decrement a stored value. Only reason we are inlining this function
// is because it is called by only one function, and we can save a bit of
// execution time via expansion into that function call.
inline madara::knowledge::KnowledgeRecord
madara::knowledge::ThreadSafeContext::dec (
  const VariableReference & variable,
  const KnowledgeUpdateSettings & settings)
{
  MADARA_GUARD_TYPE guard (mutex_);
  if (variable.record_)
  {
    // check if we have the appropriate write quality
    if (settings.always_overwrite ||
        variable.record_->write_quality >= variable.record_->quality)
    {
      -- (*variable.record_);
      variable.record_->quality = variable.record_->write_quality;
  
      mark_and_signal (variable.name_.get_ptr (), variable.record_, settings);
    }

    return *variable.record_;
  }

  return knowledge::KnowledgeRecord ();
}

/// set the lamport clock (updates with lamport clocks lower
/// than our current clock get discarded)
inline uint64_t
madara::knowledge::ThreadSafeContext::set_clock (
  uint64_t clock)
{
  MADARA_GUARD_TYPE guard (mutex_);

  // clock_ is always increasing. We never reset it to a lower clock value
  // user can check return value to see if the clock was set.
  if (clock_ < clock)
    clock_ = clock;

  return clock_;
}

/// set the lamport clock (updates with lamport clocks lower
/// than our current clock get discarded)
inline uint64_t 
madara::knowledge::ThreadSafeContext::set_clock (
  const std::string & key, uint64_t clock,
  const KnowledgeReferenceSettings & settings)
{
  // enter the mutex
  std::string key_actual;
  const std::string * key_ptr;
  MADARA_GUARD_TYPE guard (mutex_);
  
  if (settings.expand_variables)
  {
    key_actual = expand_statement (key);
    key_ptr = &key_actual;
  }
  else
    key_ptr = &key;
  
  // check for null key
  if (*key_ptr == "")
    return 0;

  // create the key if it didn't exist
  knowledge::KnowledgeRecord & record = map_[*key_ptr];

  // check for value already set
  if (record.clock < clock)
  {
    record.clock = clock;

    // try to update the global clock as well
    this->set_clock (clock);
  }

  return record.clock;
}

/// set the lamport clock for a particular variable (updates with 
/// lamport clocks lower than our current clock get discarded)
inline uint64_t
madara::knowledge::ThreadSafeContext::inc_clock (
  const std::string & key,
  const KnowledgeUpdateSettings & settings)
{
  // enter the mutex
  std::string key_actual;
  const std::string * key_ptr;
  MADARA_GUARD_TYPE guard (mutex_);
  
  if (settings.expand_variables)
  {
    key_actual = expand_statement (key);
    key_ptr = &key_actual;
  }
  else
    key_ptr = &key;
  
  // check for null key
  if (*key_ptr == "")
    return 0;

  // create the key if it didn't exist
  knowledge::KnowledgeRecord & record = map_[*key_ptr];

  return record.clock += settings.clock_increment;
}

/// increment the process lamport clock
inline uint64_t
madara::knowledge::ThreadSafeContext::inc_clock (
  const KnowledgeUpdateSettings & settings)
{
  MADARA_GUARD_TYPE guard (mutex_);
  return clock_ += settings.clock_increment;
}

/// get the lamport clock (updates with lamport clocks lower
/// than our current clock get discarded)
inline uint64_t
madara::knowledge::ThreadSafeContext::get_clock (void) const
{
  MADARA_GUARD_TYPE guard (mutex_);
  return clock_;
}

inline madara::logger::Logger &
madara::knowledge::ThreadSafeContext::get_logger (void) const
{
  MADARA_GUARD_TYPE guard (mutex_);
  return *logger_;
}

inline void
madara::knowledge::ThreadSafeContext::attach_logger (
  logger::Logger & logger) const
{
  MADARA_GUARD_TYPE guard (mutex_);
  logger_ = &logger;
}

/// get the lamport clock for a particular variable
inline uint64_t
madara::knowledge::ThreadSafeContext::get_clock (
  const std::string & key,
  const KnowledgeReferenceSettings & settings) const
{
  // enter the mutex
  std::string key_actual;
  const std::string * key_ptr;
  MADARA_GUARD_TYPE guard (mutex_);
  
  if (settings.expand_variables)
  {
    key_actual = expand_statement (key);
    key_ptr = &key_actual;
  }
  else
    key_ptr = &key;
  
  // check for null key
  if (*key_ptr == "")
    return 0;

  // find the key in the knowledge base
  KnowledgeMap::const_iterator found = map_.find (*key_ptr);

  // if it's found, then compare the value
  if (found != map_.end ())
  {
    return found->second.clock;
  }
  else
    // key does not exist
    return 0;
}

/// Lock the mutex on this context. Warning: this will cause
/// all operations to block until the unlock call is made.
inline void
madara::knowledge::ThreadSafeContext::lock (void) const
{
  mutex_.MADARA_LOCK_LOCK ();
}

/// Unlock the mutex on this context.
inline void
madara::knowledge::ThreadSafeContext::unlock (void) const
{
  mutex_.MADARA_LOCK_UNLOCK ();
}

/// Print a statement, similar to printf (variable expressions allowed)
/// e.g. input = "MyVar{.id} = {MyVar{.id}}\n";
inline void 
madara::knowledge::ThreadSafeContext::print (
  const std::string & statement, unsigned int level) const
{
  madara_logger_ptr_log (logger_, (int)level,
    this->expand_statement (statement).c_str ());
}

// clear all variables and their values
inline void
madara::knowledge::ThreadSafeContext::clear (bool erase)
{
  // enter the mutex
  MADARA_GUARD_TYPE guard (mutex_);

  if (erase)
  {
    map_.clear ();
  }

  else
  {
    for (madara::knowledge::KnowledgeMap::iterator i = map_.begin ();
      i != map_.end (); ++i)
    {
      i->second.reset_value ();
    }
  }

  changed_map_.clear ();
  changed_.MADARA_CONDITION_NOTIFY_ONE ();
}


/// Make the current thread of execution wait for a change on the
/// context
inline void 
madara::knowledge::ThreadSafeContext::wait_for_change (
  bool extra_release)
{
  // enter the mutex
  MADARA_GUARD_TYPE guard (mutex_);

  // if the caller is relying on a recursive call (e.g. KnowlegeBase::wait),
  // we'll need to call an extra release for this to work. Otherwise, the 
  // context would remain locked to the calling thread - even though it will
  // now be put to sleep
  if (extra_release)
    mutex_.MADARA_LOCK_UNLOCK ();

  changed_.wait (mutex_);

  //if (extra_release)
  //  mutex_.MADARA_LOCK_LOCK ();
}

inline void
madara::knowledge::ThreadSafeContext::mark_to_send (
  const std::string & key,
  const KnowledgeUpdateSettings & settings
  )
{
  VariableReference ref = get_ref (key, settings);
  if (ref.is_valid ())
    mark_to_send (ref, settings);
}

inline void
madara::knowledge::ThreadSafeContext::mark_to_send (
  const VariableReference & ref,
  const KnowledgeUpdateSettings & settings
  )
{
  MADARA_GUARD_TYPE guard (mutex_);
  mark_to_send_unsafe (std::string (ref.get_name ()), *ref.record_, settings);
}

inline void
madara::knowledge::ThreadSafeContext::mark_to_send_unsafe (
  const std::string & key, madara::knowledge::KnowledgeRecord & record,
  const KnowledgeUpdateSettings & 
  )
{
  changed_map_[key] = &record;

  if (record.status () != madara::knowledge::KnowledgeRecord::MODIFIED)
    record.set_modified ();
}

inline void
madara::knowledge::ThreadSafeContext::mark_to_checkpoint (
  const std::string & key,
  const KnowledgeUpdateSettings & settings
  )
{
  VariableReference ref = get_ref (key, settings);
  if (ref.is_valid ())
    mark_to_checkpoint (ref, settings);
}

inline void
madara::knowledge::ThreadSafeContext::mark_to_checkpoint (
  const VariableReference & ref,
  const KnowledgeUpdateSettings & settings
  )
{
  MADARA_GUARD_TYPE guard (mutex_);
  mark_to_checkpoint_unsafe (std::string (ref.get_name ()),
                            *ref.record_, settings);
}

inline void
madara::knowledge::ThreadSafeContext::mark_to_checkpoint_unsafe (
  const std::string & key, madara::knowledge::KnowledgeRecord & record,
  const KnowledgeUpdateSettings & 
  )
{
  local_changed_map_[key] = &record;

  if (record.status () != madara::knowledge::KnowledgeRecord::MODIFIED)
    record.set_modified ();
}

inline void
madara::knowledge::ThreadSafeContext::mark_and_signal (
  const char * name, knowledge::KnowledgeRecord * record,
  const KnowledgeUpdateSettings & settings)
{
  // otherwise set the value
  if (name[0] != '.' || settings.treat_locals_as_globals)
  {
    if (!settings.treat_globals_as_locals)
    {
      mark_to_send_unsafe (name, *record);
    }
    else if (settings.track_local_changes)
    {
      mark_to_checkpoint_unsafe (name, *record);
    }
  }
  else if (settings.track_local_changes)
  {
    mark_to_checkpoint_unsafe (name, *record);
  }

  if (settings.signal_changes)
    changed_.MADARA_CONDITION_NOTIFY_ALL ();
}

inline void
madara::knowledge::ThreadSafeContext::mark_modified (
  const std::string & key,
  const KnowledgeUpdateSettings & settings
  )
{
  VariableReference ref = get_ref (key, settings);
  if (ref.is_valid ())
    mark_modified (ref, settings);
}

inline void
madara::knowledge::ThreadSafeContext::mark_modified (
  const VariableReference & ref,
  const KnowledgeUpdateSettings & settings
  )
{
  MADARA_GUARD_TYPE guard (mutex_);
  mark_and_signal (ref.get_name (), ref.record_, settings);
}

inline std::string
madara::knowledge::ThreadSafeContext::debug_modifieds (void) const
{
  MADARA_GUARD_TYPE guard (mutex_);
  std::stringstream result;

  result << changed_map_.size () << " modifications ready to send:\n";

  for (KnowledgeRecords::const_iterator i = changed_map_.begin ();
    i != changed_map_.end (); ++i)
  {
    if (i->second->is_binary_file_type ())
    {
      result << "File: ";
    }
    else if (i->second->type () == knowledge::KnowledgeRecord::DOUBLE)
    {
      result << "Double: ";
    }
    else if (i->second->type () == knowledge::KnowledgeRecord::DOUBLE_ARRAY)
    {
      result << "Double array: ";
    }
    else if (i->second->type () == knowledge::KnowledgeRecord::INTEGER)
    {
      result << "Integer: ";
    }
    else if (i->second->type () == knowledge::KnowledgeRecord::INTEGER_ARRAY)
    {
      result << "Integer array: ";
    }
    else if (i->second->is_string_type ())
    {
      result << "String: ";
    }
    else
    {
      result << "Unknown: ";
    }

    result << i->first << " = " << i->second->to_string () << "\n";
  }

  return result.str ();
}


/// Return list of variables that have been modified
inline const madara::knowledge::KnowledgeRecords &
madara::knowledge::ThreadSafeContext::get_modifieds (void) const
{
  MADARA_GUARD_TYPE guard (mutex_);

  return changed_map_;
}

inline madara::knowledge::VariableReferences
madara::knowledge::ThreadSafeContext::save_modifieds (void) const
{
  MADARA_GUARD_TYPE guard (mutex_);

  VariableReferences snapshot;
  snapshot.resize (changed_map_.size ());
  int cur = 0;
  
  madara_logger_ptr_log (logger_, logger::LOG_MAJOR,
    "ThreadSafeContext::save_modifieds:" \
    " changed_map.size=%d, snapshot.size=%d\n",
    (int)changed_map_.size (), (int)snapshot.size ());
  
  for (KnowledgeRecords::const_iterator i = changed_map_.begin ();
    i != changed_map_.end (); ++i, ++cur)
  {
    madara_logger_ptr_log (logger_, logger::LOG_MINOR,
      "ThreadSafeContext::save_modifieds:" \
      " snapshot[%d].name=%s\n",
      cur, i->first.c_str ());
  
    snapshot[cur].set_name (i->first);
    snapshot[cur].record_ = i->second;
  }

  return snapshot;
}

inline void
madara::knowledge::ThreadSafeContext::add_modifieds (
const VariableReferences & modifieds) const
{
  MADARA_GUARD_TYPE guard (mutex_);

  for (VariableReferences::const_iterator i = modifieds.begin ();
    i != modifieds.end (); ++i)
  {
    changed_map_.insert (
      KnowledgeRecords::value_type (i->name_.get_ptr (), i->record_));
  }
}

/// Return list of variables that have been modified
inline const madara::knowledge::KnowledgeRecords &
madara::knowledge::ThreadSafeContext::get_local_modified (void) const
{
  MADARA_GUARD_TYPE guard (mutex_);

  return local_changed_map_;
}

/// Reset all variables to unmodified
inline void 
madara::knowledge::ThreadSafeContext::reset_modified (void)
{
  MADARA_GUARD_TYPE guard (mutex_);

  changed_map_.clear ();
  local_changed_map_.clear ();
}

/// Changes all global variables to modified at current time
inline void 
madara::knowledge::ThreadSafeContext::apply_modified (void)
{
  MADARA_GUARD_TYPE guard (mutex_);

  // each synchronization counts as an event, since this is a
  // pretty important networking event

  //++this->clock_;

  for (madara::knowledge::KnowledgeMap::iterator i = map_.begin ();
       i != map_.end (); 
       ++i)
  {
    if (i->first.size () > 0 && i->first[0] != '.')
    {
      // local or global doesn't matter. Clock and modification
      // aren't really a part of local variable checking anyway
      //i->second.status = madara::knowledge::KnowledgeRecord::MODIFIED;

      if (i->second.status () != knowledge::KnowledgeRecord::UNCREATED)
        mark_and_signal (i->first.c_str (), &i->second,
                         KnowledgeUpdateSettings ());
      else
        i->second.set_value (KnowledgeRecord::Integer (0));

      //i->second.clock = this->clock_;
    }
  }
}

/// Reset a variable to unmodified
inline void 
madara::knowledge::ThreadSafeContext::reset_modified (
  const std::string & variable)
{
  MADARA_GUARD_TYPE guard (mutex_);

  changed_map_.erase (variable);
}


/// Signal the condition that it can wake up someone else on changed data.
inline void
madara::knowledge::ThreadSafeContext::signal (bool lock) const
{
  if (lock)
  {
    MADARA_GUARD_TYPE guard (mutex_);
    changed_.MADARA_CONDITION_NOTIFY_ONE ();
  }
  else
    changed_.MADARA_CONDITION_NOTIFY_ONE ();
}

inline void
madara::knowledge::ThreadSafeContext::add_logger (
  const std::string & filename)
{
  logger_->add_file (filename);
}

inline int
madara::knowledge::ThreadSafeContext::get_log_level (void)
{
  return logger_->get_level ();
}

inline void
madara::knowledge::ThreadSafeContext::set_log_level (int level)
{
  logger_->set_level (level);
}



#endif // _MADARA_THREADSAFECONTEXT_INL_
