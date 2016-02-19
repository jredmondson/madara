#include <iostream>
#include <fstream>
#include <sstream>
#include <iterator>

#include <string.h>

#include "madara/utility/Utility.h"

#include "madara/knowledge/ThreadSafeContext.h"

#include "madara/expression/Interpreter.h"
#include "madara/knowledge/FileHeader.h"
#include "madara/transport/Transport.h"
#include <stdio.h>
#include <time.h>
#include "madara/logger/GlobalLogger.h"
#include "madara/utility/DeepIterator.h"


// constructor
madara::knowledge::ThreadSafeContext::ThreadSafeContext ()
  : changed_ (mutex_), clock_ (0)
#ifndef _MADARA_NO_KARL_
,
  interpreter_ (new madara::expression::Interpreter ())
#endif // _MADARA_NO_KARL_
  , logger_ (logger::global_logger.get ())
{
  expansion_splitters_.push_back ("{");
  expansion_splitters_.push_back ("}");
}

// destructor
madara::knowledge::ThreadSafeContext::~ThreadSafeContext (void)
{
#ifndef _MADARA_NO_KARL_
  delete interpreter_;
#endif // _MADARA_NO_KARL_

}

/**
 * Retrieves a knowledge record from the key. This function is useful
 * for performance reasons and also for using a knowledge::KnowledgeRecord that
 * can be one of multiple types
 **/
madara::knowledge::KnowledgeRecord *
madara::knowledge::ThreadSafeContext::get_record (
  const std::string & key,
  const KnowledgeReferenceSettings & settings)
{
  std::string key_actual;
  const std::string * key_ptr;
  Guard guard (mutex_);

  if (settings.expand_variables)
  {
    key_actual = expand_statement (key);
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


madara::knowledge::VariableReference
madara::knowledge::ThreadSafeContext::get_ref (
  const std::string & key,
  const KnowledgeReferenceSettings & settings)
{
  std::string key_actual;
  const std::string * key_ptr;
  Guard guard (mutex_);

  VariableReference record;

  // expand the key if the user asked for it
  if (settings.expand_variables)
  {
    key_actual = expand_statement (key);
    key_ptr = &key_actual;
  }
  else
    key_ptr = &key;
  
  // set name to the expanded key, for debugging purposes
  record.set_name (*key_ptr);

  // if the key is possible, create a reference to the record
  if (*key_ptr != "")
  {
    record.record_ = &map_[*key_ptr];
  }
  return record;
}


madara::knowledge::VariableReference
madara::knowledge::ThreadSafeContext::get_ref (
const std::string & key,
const KnowledgeReferenceSettings & settings) const
{
  std::string key_actual;
  const std::string * key_ptr;
  Guard guard (mutex_);

  VariableReference record;

  // expand the key if the user asked for it
  if (settings.expand_variables)
  {
    key_actual = expand_statement (key);
    key_ptr = &key_actual;
  }
  else
    key_ptr = &key;

  // set name to the expanded key, for debugging purposes
  record.set_name (*key_ptr);

  // if the key is possible, create a reference to the record
  if (*key_ptr != "")
  {
    KnowledgeMap::const_iterator found = map_.find (*key_ptr);
    record.record_ = (KnowledgeRecord *)&found->second;
  }
  return record;
}



// set the value of a variable
int
madara::knowledge::ThreadSafeContext::set (
  const VariableReference & variable,
  madara::knowledge::KnowledgeRecord::Integer value,
  const KnowledgeUpdateSettings & settings)
{
  Guard guard (mutex_);
  if (variable.record_)
  {
    // check if we have the appropriate write quality
    if (!settings.always_overwrite &&
        variable.record_->write_quality < variable.record_->quality)
      return -2;

    variable.record_->set_value (value);
    variable.record_->quality = variable.record_->write_quality;
  
    mark_and_signal (variable.name_.get_ptr (), variable.record_, settings);
  }
  else
    return -1;

  return 0;
}

// set the value of a variable
int
madara::knowledge::ThreadSafeContext::set (
  const VariableReference & variable,
  const madara::knowledge::KnowledgeRecord & value,
  const KnowledgeUpdateSettings & settings)
{
  Guard guard (mutex_);
  if (variable.record_)
  {
    // check if we have the appropriate write quality
    if (!settings.always_overwrite &&
        variable.record_->write_quality < variable.record_->quality)
      return -2;

    // copy size and type from the record value
    variable.record_->size_ = value.size_;
    variable.record_->type_ = value.type_;

    if (value.status_ != knowledge::KnowledgeRecord::UNCREATED)
    {
      variable.record_->deep_copy (value);
    }
    else
    {
      variable.record_->status_ = knowledge::KnowledgeRecord::UNCREATED;
      variable.record_->int_value_ = 0;
    }
    
    variable.record_->quality = variable.record_->write_quality;
  
    mark_and_signal (variable.name_.get_ptr (), variable.record_, settings);
  }
  else
    return -1;

  return 0;
}


// set the value of an array index
int
madara::knowledge::ThreadSafeContext::set_index (
  const VariableReference & variable, size_t index,
  madara::knowledge::KnowledgeRecord::Integer value,
  const KnowledgeUpdateSettings & settings)
{
  Guard guard (mutex_);
  if (variable.record_)
  {
    // check if we have the appropriate write quality
    if (!settings.always_overwrite &&
        variable.record_->write_quality < variable.record_->quality)
      return -2;

    variable.record_->set_index (index, value);
    variable.record_->quality = variable.record_->write_quality;
  
    mark_and_signal (variable.name_.get_ptr (), variable.record_, settings);
  }
  else
    return -1;

  return 0;
}

// set the value of a variable
int
madara::knowledge::ThreadSafeContext::set (
  const VariableReference & variable,
  const knowledge::KnowledgeRecord::Integer * value,
  uint32_t size,
  const KnowledgeUpdateSettings & settings)
{
  Guard guard (mutex_);
  if (variable.record_)
  {
    // check if we have the appropriate write quality
    if (!settings.always_overwrite &&
        variable.record_->write_quality < variable.record_->quality)
      return -2;

    variable.record_->set_value (value, size);
    variable.record_->quality = variable.record_->write_quality;
  
    mark_and_signal (variable.name_.get_ptr (), variable.record_, settings);
  }
  else
    return -1;

  return 0;
}

// set the value of a variable
int
madara::knowledge::ThreadSafeContext::set (
  const VariableReference & variable,
  const std::vector <KnowledgeRecord::Integer> & value,
  const KnowledgeUpdateSettings & settings)
{
  Guard guard (mutex_);
  if (variable.record_)
  {
    // check if we have the appropriate write quality
    if (!settings.always_overwrite &&
        variable.record_->write_quality < variable.record_->quality)
      return -2;

    variable.record_->set_value (value);
    variable.record_->quality = variable.record_->write_quality;
  
    mark_and_signal (variable.name_.get_ptr (), variable.record_, settings);
  }
  else
    return -1;

  return 0;
}

// set the value of a variable
int
madara::knowledge::ThreadSafeContext::set (
  const VariableReference & variable,
  double value,
  const KnowledgeUpdateSettings & settings)
{
  Guard guard (mutex_);
  if (variable.record_)
  {
    // check if we have the appropriate write quality
    if (!settings.always_overwrite &&
        variable.record_->write_quality < variable.record_->quality)
      return -2;

    variable.record_->set_value (value);
    variable.record_->quality = variable.record_->write_quality;
  
    mark_and_signal (variable.name_.get_ptr (), variable.record_, settings);
  }
  else
    return -1;

  return 0;
}

// set the value of a variable
int
madara::knowledge::ThreadSafeContext::set_index (
  const VariableReference & variable, size_t index,
  double value,
  const KnowledgeUpdateSettings & settings)
{
  Guard guard (mutex_);
  if (variable.record_)
  {
    // check if we have the appropriate write quality
    if (!settings.always_overwrite &&
        variable.record_->write_quality < variable.record_->quality)
      return -2;

    variable.record_->set_index (index, value);
    variable.record_->quality = variable.record_->write_quality;
  
    mark_and_signal (variable.name_.get_ptr (), variable.record_, settings);
  }
  else
    return -1;

  return 0;
}

// set the value of a variable
int
madara::knowledge::ThreadSafeContext::set (
  const VariableReference & variable,
  const double * value,
  uint32_t size,
  const KnowledgeUpdateSettings & settings)
{
  Guard guard (mutex_);
  if (variable.record_)
  {
    // check if we have the appropriate write quality
    if (!settings.always_overwrite &&
        variable.record_->write_quality < variable.record_->quality)
      return -2;

    variable.record_->set_value (value, size);
    variable.record_->quality = variable.record_->write_quality;
  
    mark_and_signal (variable.name_.get_ptr (), variable.record_, settings);
  }
  else
    return -1;

  return 0;
}

// set the value of a variable
int
madara::knowledge::ThreadSafeContext::set (
  const VariableReference & variable,
  const std::vector <double> & value,
  const KnowledgeUpdateSettings & settings)
{
  Guard guard (mutex_);
  if (variable.record_)
  {
    // check if we have the appropriate write quality
    if (!settings.always_overwrite &&
        variable.record_->write_quality < variable.record_->quality)
      return -2;

    variable.record_->set_value (value);
    variable.record_->quality = variable.record_->write_quality;
  
    mark_and_signal (variable.name_.get_ptr (), variable.record_, settings);
  }
  else
    return -1;

  return 0;
}

// set the value of a variable
int
madara::knowledge::ThreadSafeContext::set (
  const VariableReference & variable,
  const std::string & value,
  const KnowledgeUpdateSettings & settings)
{
  Guard guard (mutex_);
  if (variable.record_)
  {
    // check if we have the appropriate write quality
    if (!settings.always_overwrite &&
        variable.record_->write_quality < variable.record_->quality)
      return -2;

    variable.record_->set_value (value);
    variable.record_->quality = variable.record_->write_quality;
  
    mark_and_signal (variable.name_.get_ptr (), variable.record_, settings);
  }
  else
    return -1;

  return 0;
}

// set the value of a variable
int
madara::knowledge::ThreadSafeContext::set_xml (
  const VariableReference & variable,
  const char * value, size_t size,
  const KnowledgeUpdateSettings & settings)
{
  
  Guard guard (mutex_);
  if (variable.record_)
  {
    // check if we have the appropriate write quality
    if (!settings.always_overwrite &&
        variable.record_->write_quality < variable.record_->quality)
      return -2;

    variable.record_->set_xml (value, size);
    variable.record_->quality = variable.record_->write_quality;
  
    mark_and_signal (variable.name_.get_ptr (), variable.record_, settings);
  }
  else
    return -1;

  return 0;
}

// set the value of a variable
int
madara::knowledge::ThreadSafeContext::set_text (
  const VariableReference & variable,
  const char * value, size_t size,
  const KnowledgeUpdateSettings & settings)
{
  Guard guard (mutex_);
  if (variable.record_)
  {
    // check if we have the appropriate write quality
    if (!settings.always_overwrite &&
        variable.record_->write_quality < variable.record_->quality)
      return -2;

    variable.record_->set_text (value, size);
    variable.record_->quality = variable.record_->write_quality;
  
    mark_and_signal (variable.name_.get_ptr (), variable.record_, settings);
  }
  else
    return -1;

  return 0;
}

// set the value of a variable
int
madara::knowledge::ThreadSafeContext::set_jpeg (
  const VariableReference & variable,
  const unsigned char * value, size_t size,
  const KnowledgeUpdateSettings & settings)
{
  Guard guard (mutex_);
  if (variable.record_)
  {
    // check if we have the appropriate write quality
    if (!settings.always_overwrite &&
        variable.record_->write_quality < variable.record_->quality)
      return -2;

    variable.record_->set_jpeg (value, size);
    variable.record_->quality = variable.record_->write_quality;
  
    mark_and_signal (variable.name_.get_ptr (), variable.record_, settings);
  }
  else
    return -1;

  return 0;
}

// set the value of a variable
int
madara::knowledge::ThreadSafeContext::set_file (
  const VariableReference & variable,
  const unsigned char * value, size_t size,
  const KnowledgeUpdateSettings & settings)
{
  Guard guard (mutex_);
  if (variable.record_)
  {
    // check if we have the appropriate write quality
    if (!settings.always_overwrite &&
        variable.record_->write_quality < variable.record_->quality)
      return -2;

    variable.record_->set_file (value, size);
    variable.record_->quality = variable.record_->write_quality;
  
    mark_and_signal (variable.name_.get_ptr (), variable.record_, settings);
  }
  else
    return -1;

  return 0;
}

// set the value of a variable
int
madara::knowledge::ThreadSafeContext::read_file (
  const VariableReference & variable,
  const std::string & filename,
  const KnowledgeUpdateSettings & settings)
{
  int return_value = 0;
  Guard guard (mutex_);
  if (variable.record_)
  {
    // check if we have the appropriate write quality
    if (!settings.always_overwrite &&
        variable.record_->write_quality < variable.record_->quality)
      return -2;

    return_value = variable.record_->read_file (filename);
    variable.record_->quality = variable.record_->write_quality;
  
    mark_and_signal (variable.name_.get_ptr (), variable.record_, settings);
  }
  else
    return return_value = -1;

  return return_value;
}

/// get quality of last update to a variable.
/// @return    quality of the variable 
uint32_t 
madara::knowledge::ThreadSafeContext::get_quality (
  const std::string & key,
  const KnowledgeReferenceSettings & settings)
{
  // enter the mutex
  std::string key_actual;
  const std::string * key_ptr;
  Guard guard (mutex_);
  
  if (settings.expand_variables)
  {
    key_actual = expand_statement (key);
    key_ptr = &key_actual;
  }
  else
    key_ptr = &key;
  
  // find the key in the knowledge base
  KnowledgeMap::iterator found = map_.find (*key_ptr);

  // create the variable if it has never been written to before
  // and update its current value quality to the quality parameter

  if (found != map_.end ())
    return map_[*key_ptr].quality;

  // default quality is 0
  return 0;
}

/// get quality of last update to a variable.
/// @return    quality of the variable 
uint32_t 
madara::knowledge::ThreadSafeContext::get_write_quality (
  const std::string & key,
  const KnowledgeReferenceSettings & settings)
{
  // enter the mutex
  std::string key_actual;
  const std::string * key_ptr;
  Guard guard (mutex_);
  
  if (settings.expand_variables)
  {
    key_actual = expand_statement (key);
    key_ptr = &key_actual;
  }
  else
    key_ptr = &key;

  // find the key in the knowledge base
  KnowledgeMap::iterator found = map_.find (*key_ptr);

  // create the variable if it has never been written to before
  // and update its current value quality to the quality parameter

  if (found != map_.end ())
    return map_[*key_ptr].write_quality;

  // default quality is 0
  return 0;
}

/// Set quality of last update to a variable.
/// @return    quality of the variable after this call
uint32_t 
madara::knowledge::ThreadSafeContext::set_quality (
  const std::string & key, uint32_t quality,
                           bool force_update,
  const KnowledgeReferenceSettings & settings)
{
  // enter the mutex
  std::string key_actual;
  const std::string * key_ptr;
  Guard guard (mutex_);
  
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
  KnowledgeMap::iterator found = map_.find (*key_ptr);

  // create the variable if it has never been written to before
  // and update its current value quality to the quality parameter

  if (found == map_.end () || force_update || quality > found->second.quality)
    map_[*key_ptr].quality = quality;

  // return current quality
  return map_[*key_ptr].quality;
}
      
/// Set quality of this process writing to a variable
void 
madara::knowledge::ThreadSafeContext::set_write_quality (
  const std::string & key, uint32_t quality,
  const KnowledgeReferenceSettings & settings)
{
  // enter the mutex
  std::string key_actual;
  const std::string * key_ptr;
  Guard guard (mutex_);
  
  if (settings.expand_variables)
  {
    key_actual = expand_statement (key);
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
int
madara::knowledge::ThreadSafeContext::set_if_unequal (
  const std::string & key, madara::knowledge::KnowledgeRecord::Integer value,
  uint32_t quality, uint64_t clock,
  const KnowledgeUpdateSettings & settings)
{
  int result = 1;

  // enter the mutex
  std::string key_actual;
  const std::string * key_ptr;
  Guard guard (mutex_);
  
  if (settings.expand_variables)
  {
    key_actual = expand_statement (key);
    key_ptr = &key_actual;
  }
  else
    key_ptr = &key;
  
  // check for null key
  if (*key_ptr == "")
    return -1;
  
  // find the key in the knowledge base
  KnowledgeMap::iterator found = map_.find (*key_ptr);

  // if it's found, then compare the value
  if (!settings.always_overwrite && found != map_.end ())
  {
    // setup a rhs
    madara::knowledge::KnowledgeRecord rhs;
    rhs.set_value (value);

    // if we do not have enough quality to update the variable
    // return -2
    if (quality < found->second.quality)
      result = -2;

    // if we have the same quality, but our clock value
    // is less than what we've already seen, then return -3
    else if (quality == found->second.quality && 
             clock < found->second.clock)
      result = -3;

    // check for value already set
    else if (found->second == rhs)
      result = 0;
  }

  madara::knowledge::KnowledgeRecord & record = map_[*key_ptr];

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
    record.set_value (value);
  
    mark_and_signal (key_ptr->c_str (), &record, settings);
  }

  // value was changed
  return result;
}

/// Set if the variable value will be different. Always updates clock to
/// highest value
/// @return   1 if the value was changed. 0 if not changed. 
///           -1 if null key, -2 if quality not high enough
int
madara::knowledge::ThreadSafeContext::set_if_unequal (
  const std::string & key, double value,
  uint32_t quality, uint64_t clock,
  const KnowledgeUpdateSettings & settings)
{
  int result = 1;

  // enter the mutex
  std::string key_actual;
  const std::string * key_ptr;
  Guard guard (mutex_);
  
  if (settings.expand_variables)
  {
    key_actual = expand_statement (key);
    key_ptr = &key_actual;
  }
  else
    key_ptr = &key;
  
  // check for null key
  if (*key_ptr == "")
    return -1;
  
  // find the key in the knowledge base
  KnowledgeMap::iterator found = map_.find (*key_ptr);

  // if it's found, then compare the value
  if (!settings.always_overwrite && found != map_.end ())
  {
    // setup a rhs
    madara::knowledge::KnowledgeRecord rhs;
    rhs.set_value (value);

    // if we do not have enough quality to update the variable
    // return -2
    if (quality < found->second.quality)
      result = -2;

    // if we have the same quality, but our clock value
    // is less than what we've already seen, then return -3
    else if (quality == found->second.quality && 
             clock < found->second.clock)
      result = -3;

    // check for value already set
    else if (found->second == rhs)
      result = 0;
  }

  madara::knowledge::KnowledgeRecord & record = map_[*key_ptr];

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
    record.set_value (value);
  
    mark_and_signal (key_ptr->c_str (), &record, settings);
  }

  // value was changed
  return result;
}

/// Set if the variable value will be different. Always updates clock to
/// highest value
/// @return   1 if the value was changed. 0 if not changed. 
///           -1 if null key, -2 if quality not high enough
int
madara::knowledge::ThreadSafeContext::set_if_unequal (
  const std::string & key, const std::string & value,
  uint32_t quality, uint64_t clock,
  const KnowledgeUpdateSettings & settings)
{
  int result = 1;

  // enter the mutex
  std::string key_actual;
  const std::string * key_ptr;
  Guard guard (mutex_);
  
  if (settings.expand_variables)
  {
    key_actual = expand_statement (key);
    key_ptr = &key_actual;
  }
  else
    key_ptr = &key;
  
  // check for null key
  if (*key_ptr == "")
    return -1;
  
  // find the key in the knowledge base
  KnowledgeMap::iterator found = map_.find (*key_ptr);

  // if it's found, then compare the value
  if (!settings.always_overwrite && found != map_.end ())
  {
    // setup a rhs
    madara::knowledge::KnowledgeRecord rhs;
    rhs.set_value (value);

    // if we do not have enough quality to update the variable
    // return -2
    if (quality < found->second.quality)
      result = -2;

    // if we have the same quality, but our clock value
    // is less than what we've already seen, then return -3
    else if (quality == found->second.quality && 
             clock < found->second.clock)
      result = -3;

    // check for value already set
    else if (found->second == rhs)
      result = 0;
  }

  madara::knowledge::KnowledgeRecord & record = map_[*key_ptr];

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
    record.set_value (value);
  
    // otherwise set the value
    mark_and_signal (key_ptr->c_str (), &record, settings);
  }
  // value was changed
  return result;
}


/// Set if the variable value will be different. Always updates clock to
/// highest value
/// @return   1 if the value was changed. 0 if not changed. 
///           -1 if null key, -2 if quality not high enough
int
madara::knowledge::ThreadSafeContext::update_record_from_external (
  const std::string & key, const knowledge::KnowledgeRecord & rhs,
  const KnowledgeUpdateSettings & settings)
{
  int result = 1;

  // enter the mutex
  std::string key_actual;
  const std::string * key_ptr;
  Guard guard (mutex_);
  
  if (settings.expand_variables)
  {
    key_actual = expand_statement (key);
    key_ptr = &key_actual;
  }
  else
    key_ptr = &key;
  
  // check for null key
  if (*key_ptr == "")
    return -1;
  
  // find the key in the knowledge base
  KnowledgeMap::iterator found = map_.find (*key_ptr);

  // if it's found, then compare the value
  if (!settings.always_overwrite && found != map_.end ())
  {
    // if we do not have enough quality to update the variable
    // return -2
    if (rhs.quality < found->second.quality)
      result = -2;

    // if we have the same quality, but our clock value
    // is less than what we've already seen, then return -3
    else if (rhs.quality == found->second.quality && 
             rhs.clock < found->second.clock)
      result = -3;

    // if we reach this point, then the record is safe to copy
    found->second.deep_copy (rhs);

    knowledge::KnowledgeRecord & current_value = found->second;

    mark_and_signal (key_ptr->c_str (), &current_value, settings);
  }
  else
  {
    // if we reach this point, then we have to create the record
    knowledge::KnowledgeRecord & current_value = map_[*key_ptr];
    current_value.deep_copy (rhs);
    
    mark_and_signal (key_ptr->c_str (), &current_value, settings);
  }
  
  // if we need to update the global clock, then update it
  if (rhs.clock >= this->clock_)
    this->clock_ = rhs.clock + 1;
  
  //if (settings.signal_changes)
  //  changed_.signal ();

  // value was changed
  return result;
}

/// Indicate that a status change has occurred. This could be a message
/// from the transport to let the knowledge engine know that new agents
/// are available to send knowledge to.
void 
madara::knowledge::ThreadSafeContext::set_changed (void)
{
  changed_.signal ();
}

// print all variables and their values
void
madara::knowledge::ThreadSafeContext::print (
  unsigned int level) const
{
  Guard guard (mutex_);
  for (madara::knowledge::KnowledgeMap::const_iterator i = map_.begin ();
       i != map_.end (); 
       ++i)
  {
    if (i->second.exists ())
    {
      madara_logger_ptr_log (logger_, (int)level, "%s=%s\n",
        i->first.c_str (), i->second.to_string (", ").c_str ());
    }
  }
}

// print all variables and their values
void
madara::knowledge::ThreadSafeContext::to_string (
  std::string & target,
  const std::string & array_delimiter,
  const std::string & record_delimiter,
  const std::string & key_val_delimiter) const
{
  Guard guard (mutex_);
  std::stringstream buffer;

  bool first = true;

  for (madara::knowledge::KnowledgeMap::const_iterator i = map_.begin ();
       i != map_.end (); 
       ++i)
  {
    // separate each record with the record_delimiter
    if (!first)
    {
      buffer << record_delimiter;
    }

    buffer << i->first;

    // separate the key/value pairing with the key_val_delimiter
    buffer << key_val_delimiter;

    if (i->second.is_string_type ())
    {
      buffer << "'";
    }
    else if (i->second.type () == i->second.DOUBLE_ARRAY ||
      i->second.type () == i->second.INTEGER_ARRAY)
    {
      buffer << "[";
    }

    // use the array_delimiter for the underlying to_string functions
    buffer << i->second.to_string (array_delimiter);
    
    if (i->second.is_string_type ())
    {
      buffer << "'";
    }
    else if (i->second.type () == i->second.DOUBLE_ARRAY ||
      i->second.type () == i->second.INTEGER_ARRAY)
    {
      buffer << "]";
    }

    if (first)
      first = false;
  }

  target = buffer.str ();
}

/// Expand a string with variable expansions. This is a generic form of
/// the function found in VariableNode, which is optimized to never change
/// keys.
std::string 
madara::knowledge::ThreadSafeContext::expand_statement (
  const std::string & statement) const
{
  // enter the mutex
  Guard guard (mutex_);

  // vectors for holding parsed tokens and pivot_list
  size_t subcount = 0;
  size_t begin_exp = 0;

  std::stringstream builder;

  // iterate over the input string
  for (std::string::size_type i = 0; i < statement.size (); ++i)
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
          statement.substr (begin_exp + 1, i - begin_exp - 1);
        std::string results = this->expand_statement (expandable);
        builder << this->get (results);
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
    madara_logger_ptr_log (logger_, logger::LOG_EMERGENCY,
      "KARL COMPILE ERROR : Improperly matched braces in %s\n",
      statement.c_str ());

    exit (-1);
  }

  return builder.str ();
}

#ifndef _MADARA_NO_KARL_

// defines a function by name
void
madara::knowledge::ThreadSafeContext::define_function (
  const std::string & name,
    knowledge::KnowledgeRecord (*func) (FunctionArguments &, Variables &),
        const KnowledgeReferenceSettings & settings)
{
  // enter the mutex
  std::string key_actual;
  const std::string * key_ptr;
  Guard guard (mutex_);
  
  if (settings.expand_variables)
  {
    key_actual = expand_statement (name);
    key_ptr = &key_actual;
  }
  else
    key_ptr = &name;
  
  // check for null key
  if (*key_ptr == "")
    return;
  
  functions_[*key_ptr] = Function (func);
}

void
madara::knowledge::ThreadSafeContext::define_function (
  const std::string & name,
    knowledge::KnowledgeRecord (*func) (const char * name, FunctionArguments &, Variables &),
        const KnowledgeReferenceSettings & settings)
{
  // enter the mutex
  std::string key_actual;
  const std::string * key_ptr;
  Guard guard (mutex_);
  
  if (settings.expand_variables)
  {
    key_actual = expand_statement (name);
    key_ptr = &key_actual;
  }
  else
    key_ptr = &name;
  
  // check for null key
  if (*key_ptr == "")
    return;
  
  functions_[*key_ptr] = Function (func);
}

#ifdef _MADARA_JAVA_
void
madara::knowledge::ThreadSafeContext::define_function (const std::string & name,
  jobject callable,
  const KnowledgeReferenceSettings & settings)
{
  // enter the mutex
  std::string key_actual;
  const std::string * key_ptr;
  Guard guard (mutex_);
  
  if (settings.expand_variables)
  {
    key_actual = expand_statement (name);
    key_ptr = &key_actual;
  }
  else
    key_ptr = &name;
  
  // check for null key
  if (*key_ptr == "")
    return;
  
  functions_[*key_ptr] = Function (callable);
}
#endif

#ifdef _MADARA_PYTHON_CALLBACKS_
void
madara::knowledge::ThreadSafeContext::define_function (const std::string & name,
  boost::python::object callable,
  const KnowledgeReferenceSettings & settings)
{
  // enter the mutex
  std::string key_actual;
  const std::string * key_ptr;
  Guard guard (mutex_);
  
  if (settings.expand_variables)
  {
    key_actual = expand_statement (name);
    key_ptr = &key_actual;
  }
  else
    key_ptr = &name;
  
  // check for null key
  if (*key_ptr == "")
    return;
  
  functions_[*key_ptr] = Function (callable);
}
 
#endif

void
madara::knowledge::ThreadSafeContext::define_function (const std::string & name,
  const std::string & expression,
  const KnowledgeReferenceSettings & settings)
{
  CompiledExpression compiled = compile (expression);
  define_function (name, compiled, settings);
}
     
void
madara::knowledge::ThreadSafeContext::define_function (const std::string & name,
  const CompiledExpression & expression,
  const KnowledgeReferenceSettings & settings)
{
  // enter the mutex
  std::string key_actual;
  const std::string * key_ptr;
  Guard guard (mutex_);
  
  if (settings.expand_variables)
  {
    key_actual = expand_statement (name);
    key_ptr = &key_actual;
  }
  else
    key_ptr = &name;
  
  // check for null key
  if (*key_ptr == "")
    return;
  
  functions_[*key_ptr] = Function (expression.expression);
}


madara::knowledge::Function *
madara::knowledge::ThreadSafeContext::retrieve_function (
  const std::string & name,
  const KnowledgeReferenceSettings & settings)
{
  // enter the mutex
  std::string key_actual;
  const std::string * key_ptr;
  Guard guard (mutex_);
  
  if (settings.expand_variables)
  {
    key_actual = expand_statement (name);
    key_ptr = &key_actual;
  }
  else
    key_ptr = &name;
  
  // check for null key
  if (*key_ptr == "")
    return 0;
  
  return &functions_[*key_ptr];
}


madara::knowledge::CompiledExpression
madara::knowledge::ThreadSafeContext::compile (
  const std::string & expression)
{
  madara_logger_ptr_log (logger_, logger::LOG_MINOR,
    "ThreadSafeContext::compile:" \
    " compiling %s\n", expression.c_str ());
  
  Guard guard (mutex_);
  CompiledExpression ce;
  ce.logic = expression;
  ce.expression = interpreter_->interpret (*this, expression);

  return ce;
}

madara::knowledge::KnowledgeRecord
madara::knowledge::ThreadSafeContext::evaluate (
  CompiledExpression expression,
  const KnowledgeUpdateSettings & settings)
{
  Guard guard (mutex_);
  return expression.expression.evaluate (settings);
}

madara::knowledge::KnowledgeRecord
madara::knowledge::ThreadSafeContext::evaluate (
  expression::ComponentNode * root,
  const KnowledgeUpdateSettings & settings)
{
  Guard guard (mutex_);
  if (root)
    return root->evaluate (settings);
  else
    return knowledge::KnowledgeRecord (KnowledgeRecord::Integer (0));
}

#endif // _MADARA_NO_KARL_

size_t
  madara::knowledge::ThreadSafeContext::to_vector (
  const std::string & subject,
  unsigned int start,
  unsigned int end,
  std::vector <KnowledgeRecord> & target)
{
  target.clear ();
  
  // enter the mutex
  Guard guard (mutex_);

  if (end >= start)
  {
    target.resize (end - start + 1);

    for (unsigned int i = 0; start <= end; ++start, ++i)
    {
      std::stringstream buffer;
      buffer << subject;
      buffer << start;
      target[i].deep_copy (get (buffer.str ()));
    }
  }

  return target.size ();
}


size_t
  madara::knowledge::ThreadSafeContext::to_map (
  const std::string & expression,
  std::map <std::string, knowledge::KnowledgeRecord> & target)
{
  target.clear ();
  
  std::string subject (expression);
  bool matches_found (false);

  // remove the wildcard and make this into a subject
  if (subject[subject.size () - 1] == '*')
    subject.resize (subject.size () - 1);

  // just in case a string implementation does not inline
  std::string::size_type subject_size = subject.size ();
  const char * subject_ptr = subject.c_str ();

  // enter the mutex
  Guard guard (mutex_);

  // if expression is blank, assume the user wants all variables
  if (expression.size () == 0)
    target = map_;
  else
  {
    for (KnowledgeMap::iterator i = map_.begin ();
      i != map_.end (); ++i)
    {
      if (i->first.size () >= subject_size)
      {
        int result = strncmp (i->first.c_str (), subject_ptr, subject_size);
        if (result == 0)
        {
          // we have a match, add this to the map
          target[i->first].deep_copy (i->second);
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


  return target.size ();
}


size_t 
madara::knowledge::ThreadSafeContext::to_map (
  const std::string & prefix,
  const std::string & delimiter,
  const std::string & suffix,
  std::vector <std::string> & next_keys,
  std::map <std::string, knowledge::KnowledgeRecord> & result,
  bool just_keys)
{
  // clear the user provided maps
  next_keys.clear ();
  result.clear ();

  // loop tracking for optimizations
  bool matches_found (false);
  std::string last_key ("");

  // enter the mutex
  Guard guard (mutex_);

  for (KnowledgeMap::iterator i = map_.begin ();
    i != map_.end (); ++i)
  {
    // if the prefix doesn't match
    if (prefix != "" && !utility::begins_with (i->first, prefix))
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
      if (suffix != "" && !utility::ends_with (i->first, suffix))
      {
        continue;
      }

      if (!just_keys)
      {
        // the key is safe to add to the master map
        result[i->first].deep_copy (i->second);
      }

      // determine if there is a next key in the hierarchy
      size_t prefix_end = prefix.length () + delimiter.length ();

      std::string current_delimiter = i->first.substr (prefix.length (), delimiter.length ());

      if (current_delimiter == delimiter && i->first.length () > prefix_end)
      {
        // find the end of the sub key
        size_t key_end = i->first.find (delimiter, prefix_end);
        
        // if we haven't seen the subkey, add it
        std::string current_key (
          i->first.substr (prefix_end, key_end - prefix_end));
        if (current_key != last_key)
        {
          next_keys.push_back (current_key);
          last_key = current_key;
        }
      }
    }
  }



  return result.size ();
}

std::pair<madara::knowledge::KnowledgeMap::iterator,
          madara::knowledge::KnowledgeMap::iterator>
madara::knowledge::ThreadSafeContext::get_prefix_range(
  const std::string &prefix) 
{
  std::pair<KnowledgeMap::iterator, KnowledgeMap::iterator>
    ret(map_.begin(), map_.end());

  // If prefix is empty string, copy entire map
  if(prefix.size() > 0)
  {
    ssize_t psz = prefix.size();

    // Find first element >= prefix; i.e., first match or first with that prefix
    ret.second = ret.first = map_.lower_bound(prefix);

    // Advance e until it is just past last element with prefix (or at end)
    while(ret.second != map_.end() &&
        ret.second->first.compare(0, psz, prefix) == 0)
      ++ret.second;
  }
  return ret;
}

std::pair<madara::knowledge::KnowledgeMap::const_iterator,
          madara::knowledge::KnowledgeMap::const_iterator>
madara::knowledge::ThreadSafeContext::get_prefix_range(
  const std::string &prefix) const
{
  std::pair<KnowledgeMap::const_iterator, KnowledgeMap::const_iterator>
    ret(map_.begin(), map_.end());

  // If prefix is empty string, copy entire map
  if(prefix.size() > 0)
  {
    ssize_t psz = prefix.size();

    // Find first element >= prefix; i.e., first match or first with that prefix
    ret.second = ret.first = map_.lower_bound(prefix);

    // Advance e until it is just past last element with prefix (or at end)
    while(ret.second != map_.end() &&
        ret.second->first.compare(0, psz, prefix) == 0)
      ++ret.second;
  }
  return ret;
}

madara::knowledge::KnowledgeMap
madara::knowledge::ThreadSafeContext::to_map (
  const std::string &prefix) const
{
  // enter the mutex
  Guard guard (mutex_);

  std::pair<KnowledgeMap::const_iterator, KnowledgeMap::const_iterator>
    iters(get_prefix_range(prefix));

  // RVO should avoid copying this map
  return KnowledgeMap(deep_iterate(iters.first), deep_iterate(iters.second));
}

madara::knowledge::KnowledgeMap
madara::knowledge::ThreadSafeContext::to_map_stripped (
  const std::string &prefix) const
{
  // enter the mutex
  Guard guard (mutex_);

  std::pair<KnowledgeMap::const_iterator, KnowledgeMap::const_iterator>
    iters(get_prefix_range(prefix));

  // NRVO should avoid copying this map
  KnowledgeMap ret;
#ifndef USE_CPP11
  KnowledgeMap::iterator hint = ret.begin();
#endif
  for(;iters.first != iters.second; ++iters.first)
  {
#ifdef USE_CPP11
    ret.emplace_hint(ret.end(), iters.first->first.substr(prefix.size()),
                     iters.first->second.deep_copy());
#else
    // Before C++11, hint works if it is the value _before_ the one to be
    // inserted, so we have to keep track of it.
    hint = ret.insert(hint, KnowledgeMap::value_type(
                               iters.first->first.substr(prefix.size()),
                               iters.first->second.deep_copy()));
#endif
  }
  return ret;
}

void
madara::knowledge::ThreadSafeContext::copy (
  const ThreadSafeContext & source,
  const CopySet & copy_set,
  bool clean_copy)
{
  // if we need to clean first, clear the map
  if (clean_copy)
    map_.clear ();
  
  // if the copy set is empty, copy everything
  if (copy_set.size () == 0)
  {
    for (KnowledgeMap::const_iterator i = source.map_.begin ();
         i != source.map_.end (); ++i)
    {
      // deep copy to prevent issues with ref counting in threads
      map_[i->first].deep_copy (i->second);
    }
  }
  else
  {
    // we have a copy set, so only copy what the user asked for
    for (CopySet::const_iterator key = copy_set.begin ();
         key != copy_set.end (); ++key)
    {
      // check source for existence of the current copy set key
      KnowledgeMap::const_iterator i = source.map_.find (key->first);

      // if found, make a deep copy of the found entry
      if (i != source.map_.end ())
      {
        map_[i->first].deep_copy (i->second);
      }
    }
  }
}

int64_t
madara::knowledge::ThreadSafeContext::save_context (
  const std::string & filename,
  const std::string & id) const
{
  madara_logger_ptr_log (logger_, logger::LOG_MAJOR,
    "ThreadSafeContext::save_context:" \
    " opening file %s\n", filename.c_str ());
  
  int64_t total_written (0);
  FILE * file = fopen (filename.c_str (), "wb");
  
  FileHeader meta;
  meta.states = 1;
  strncpy (meta.originator, id.c_str (),
    sizeof (meta.originator) < id.size () + 1 ?
    sizeof (meta.originator) : id.size () + 1);

  transport::MessageHeader checkpoint_header;

  if (file)
  {
    int64_t max_buffer (102800);
    int64_t buffer_remaining (max_buffer);
    utility::ScopedArray <char> buffer = new char [max_buffer];

    char * current = buffer.get_ptr ();

    madara_logger_ptr_log (logger_, logger::LOG_MINOR,
      "ThreadSafeContext::save_context:" \
      " generating file meta\n");
  
    meta.size += checkpoint_header.encoded_size ();
    checkpoint_header.size = checkpoint_header.encoded_size ();

    current = meta.write (current, buffer_remaining);
    current = checkpoint_header.write (current, buffer_remaining);

    madara_logger_ptr_log (logger_, logger::LOG_MINOR,
      "ThreadSafeContext::save_context:" \
      " writing records\n");
  
    // lock the context
    Guard guard (mutex_);

    for (KnowledgeMap::const_iterator i = map_.begin ();
         i != map_.end (); ++i)
    {
      if (i->second.exists ())
      {
        // get the encoded size of the record for checking buffer boundaries
        int64_t encoded_size = i->second.get_encoded_size (i->first);
        ++checkpoint_header.updates;
        meta.size += encoded_size;
        checkpoint_header.size += encoded_size;

        if (encoded_size > buffer_remaining)
        {
          /**
           * if the record is larger than the buffer we have remaining, then
           * write the buffer to the file
           **/
          current = buffer.get_ptr ();
          fwrite (current,
            (size_t)(max_buffer - buffer_remaining), 1, file);
          total_written += (int64_t)(max_buffer - buffer_remaining);
          buffer_remaining = max_buffer;

          if (encoded_size > max_buffer)
          {
            /**
             * If the record is larger than the buffer, then we must allocate a
             * buffer large enough to write to it.
             **/
            buffer = new char[encoded_size];
            max_buffer = encoded_size;
            buffer_remaining = max_buffer;
            current = buffer.get_ptr ();
          } // end if larger than buffer
        } // end if larger than buffer remaining

        current = i->second.write (current, i->first, buffer_remaining);
      }
    }
  
    if (buffer_remaining != max_buffer)
    {
      fwrite (buffer.get_ptr (),
        (size_t) (max_buffer - buffer_remaining), 1, file);
      total_written += (int64_t) (max_buffer - buffer_remaining);
    }

    // update the meta data at the front
    fseek (file, 0, SEEK_SET);

    current = buffer.get_ptr ();
    buffer_remaining = max_buffer;

    current = meta.write (current, buffer_remaining);
    current = checkpoint_header.write (current, buffer_remaining);

    fwrite (buffer.get_ptr (), current - buffer.get_ptr (), 1, file);

    fclose (file);
  }

  return meta.size;
}


int64_t
madara::knowledge::ThreadSafeContext::save_as_karl (
const std::string & filename) const
{
  madara_logger_ptr_log (logger_, logger::LOG_MINOR,
    "ThreadSafeContext::save_as_karl:" \
    " opening file %s\n", filename.c_str ());

  int64_t bytes_written (0);
  std::stringstream buffer;
  std::ofstream file;
  file.open (filename.c_str ());

  if (file.is_open ())
  {
    // lock the context
    Guard guard (mutex_);

    for (KnowledgeMap::const_iterator i = map_.begin ();
      i != map_.end (); ++i)
    {
      if (i->second.exists ())
      {
        buffer << i->first;
        buffer << "=";

        if (!i->second.is_binary_file_type ())
        {
          // record is a non binary file type
          if (i->second.is_string_type ())
          {
            // strings require quotation marks
            buffer << "\"";
          }
          else if (i->second.type () == knowledge::KnowledgeRecord::INTEGER_ARRAY ||
            i->second.type () == knowledge::KnowledgeRecord::DOUBLE_ARRAY)
          {
            // arrays require brackets
            buffer << "[";
          }

          buffer << i->second;
          if (i->second.is_string_type ())
          {
            // strings require quotation marks
            buffer << "\"";
          }
          else if (i->second.type () == knowledge::KnowledgeRecord::INTEGER_ARRAY ||
            i->second.type () == knowledge::KnowledgeRecord::DOUBLE_ARRAY)
          {
            // arrays require brackets
            buffer << "]";
          }
        }
        else
        {
          buffer << "#read_file ('";

          std::string path = utility::extract_path (filename);

          if (path == "")
            path = ".";

          path += "/";
          path += i->first;

          if (i->second.type () == knowledge::KnowledgeRecord::IMAGE_JPEG)
          {
            path += ".jpg";
          }
          else
          {
            path += ".dat";
          }

          utility::write_file (path,
            (void *)i->second.file_value_.get_ptr (), i->second.size ());
          buffer << path;


          buffer << "')";
        }

        buffer << ";\n";
      }
    }

    std::string result = buffer.str ();
    file << result;

    bytes_written = (int64_t) result.size ();

    file.close ();
  }

  return bytes_written;
}


int64_t
madara::knowledge::ThreadSafeContext::load_context (
  const std::string & filename, std::string & id,
  const KnowledgeUpdateSettings & settings)
{
  madara_logger_ptr_log (logger_, logger::LOG_MAJOR,
    "ThreadSafeContext::load_context:" \
    " opening file %s\n", filename.c_str ());
  
  // using ACE for writing to the destination file
  FILE * file = fopen (filename.c_str (), "rb");

  int64_t total_read (0);

  if (file)
  {
    int64_t max_buffer (102800);
    int64_t buffer_remaining (max_buffer);
    FileHeader meta;

    utility::ScopedArray <char> buffer = new char [max_buffer];
    const char * current = buffer.get_ptr ();

    madara_logger_ptr_log (logger_, logger::LOG_MINOR,
      "ThreadSafeContext::load_context:" \
      " reading file meta data\n");
  
    total_read = fread (buffer.get_ptr (),
      1, max_buffer, file);
    buffer_remaining = (int64_t)total_read;
    
    if (total_read > FileHeader::encoded_size () &&
        FileHeader::file_header_test (current))
    {
      // if there was something in the file, and it was the right header

      current = meta.read (current, buffer_remaining);
      id = meta.originator;

      /**
       * check that there is more than one state and that the rest of
       * the file is sufficient to at least be a message header (what
       * we use as a checkpoint header
       **/
      if (meta.states > 0)
      {
        for (uint64_t state = 0; state < meta.states; ++state)
        {
          if (buffer_remaining > (int64_t)
             transport::MessageHeader::static_encoded_size ())
          {
            transport::MessageHeader checkpoint_header;

            current = checkpoint_header.read (current, buffer_remaining);

            /**
             * What we read into the checkpoint_header will dictate our
             * max_buffer. We want to make this checkpoint_header size into
             * something reasonable.
             **/
            if (checkpoint_header.size > (uint64_t) buffer_remaining)
            {
              /**
               * create a new array and copy the remaining elements
               * from buffer_remaining
               **/
              utility::ScopedArray <char> new_buffer =
                new char [checkpoint_header.size];
              memcpy (new_buffer.get_ptr (), current,
                (size_t)buffer_remaining);

              // read the rest of checkpoint into new buffer
              total_read += fread (new_buffer.get_ptr () + buffer_remaining, 1,
                checkpoint_header.size
                  - (uint64_t)buffer_remaining
                  - checkpoint_header.encoded_size (), file);

              // update other variables
              max_buffer = checkpoint_header.size;
              buffer_remaining = checkpoint_header.size
                - checkpoint_header.encoded_size ();
              current = new_buffer.get_ptr ();
              buffer = new_buffer;
            } // end if allocation is needed

            for (uint32_t update = 0;
                 update < checkpoint_header.updates; ++update)
            {
              std::string key;
              knowledge::KnowledgeRecord record;
              current = record.read (current, key, buffer_remaining);
              update_record_from_external (key, record, settings);
            }

          } // end if enough buffer for reading a message header

          if (buffer_remaining == 0  && (uint64_t) total_read < meta.size)
          {
            buffer_remaining = max_buffer;
            current = buffer.get_ptr ();
            total_read += fread (buffer.get_ptr (), 1, buffer_remaining, file);
          }
        } // end for loop of states
      }
    } // end if total_read > 0
    else
    {
      madara_logger_ptr_log (logger_, logger::LOG_MINOR,
        "ThreadSafeContext::load_context:" \
        " invalid file. No contextual change.\n");
    }

    fclose (file);
  }

  return total_read;
}


int64_t
madara::knowledge::ThreadSafeContext::save_checkpoint (
  const std::string & filename,
  const std::string & id) const
{
  madara_logger_ptr_log (logger_, logger::LOG_MAJOR,
    "ThreadSafeContext::save_checkpoint:" \
    " opening file %s\n", filename.c_str ());
  
  int64_t total_written (0);
  FILE * file = fopen (filename.c_str (), "rb+");
  
  FileHeader meta;
  transport::MessageHeader checkpoint_header;

  if (file)
  {
    int64_t max_buffer (102800);
    int64_t buffer_remaining (max_buffer);
    utility::ScopedArray <char> buffer = new char [max_buffer];

    char * current = buffer.get_ptr ();
    const char * meta_reader = current;
  
    // read the meta data at the front
    fseek (file, 0, SEEK_SET);
    fread (current, meta.encoded_size (), 1, file);

    meta_reader = meta.read (meta_reader, buffer_remaining);
    
    if (id != "")
    {
      madara_logger_ptr_log (logger_, logger::LOG_MINOR,
        "ThreadSafeContext::save_checkpoint:" \
        " setting file meta id to %s\n",
        id.c_str ());
  
      strncpy (meta.originator, id.c_str (),
        sizeof (meta.originator) < id.size () + 1 ?
        sizeof (meta.originator) : id.size () + 1);
    }

    // save the spot where the file ends
    uint64_t checkpoint_start = meta.size;

    madara_logger_ptr_log (logger_, logger::LOG_MINOR,
      "ThreadSafeContext::save_checkpoint:" \
      " generating file meta\n");
  
    meta.size += checkpoint_header.encoded_size ();
    checkpoint_header.size = 0;

    // lock the context
    Guard guard (mutex_);

    const knowledge::KnowledgeRecords & records = this->get_modifieds ();    
    const knowledge::KnowledgeRecords & local_records = this->get_local_modified ();

    if (records.size () + local_records.size () != 0)
    {
      checkpoint_header.size = checkpoint_header.encoded_size ();

      // set the file pointer to the end of the file
      fseek (file, checkpoint_start, SEEK_SET);
      current = checkpoint_header.write (current, buffer_remaining);

      madara_logger_ptr_log (logger_, logger::LOG_MINOR,
        "ThreadSafeContext::save_checkpoint:" \
        " writing records\n");
  
      for (KnowledgeRecords::const_iterator i = records.begin ();
           i != records.end (); ++i)
      {
        if (i->second->exists ())
        {
          // get the encoded size of the record for checking buffer boundaries
          int64_t encoded_size = i->second->get_encoded_size (i->first);
          ++checkpoint_header.updates;
          meta.size += encoded_size;
          checkpoint_header.size += encoded_size;

          if (encoded_size > buffer_remaining)
          {
            /**
             * if the record is larger than the buffer we have remaining, then
             * write the buffer to the file
             **/
            current = buffer.get_ptr ();
            fwrite (current,
              (size_t)(max_buffer - buffer_remaining), 1, file);
            total_written += (int64_t)(max_buffer - buffer_remaining);
            buffer_remaining = max_buffer;

            madara_logger_ptr_log (logger_, logger::LOG_MINOR,
              "ThreadSafeContext::save_checkpoint:" \
              " encoded_size larger than remaining buffer. Flushing\n");

            if (encoded_size > max_buffer)
            {
              /**
               * If the record is larger than the buffer, then we must allocate a
               * buffer large enough to write to it.
               **/
              buffer = new char[encoded_size];
              max_buffer = encoded_size;
              buffer_remaining = max_buffer;
              current = buffer.get_ptr ();

              madara_logger_ptr_log (logger_, logger::LOG_MINOR,
                "ThreadSafeContext::save_checkpoint:" \
                " encoded_size larger than entire buffer. Reallocating\n");
            } // end if larger than buffer
          } // end if larger than buffer remaining

          current = i->second->write (current, i->first, buffer_remaining);
        }
      } // end records loop

      for (KnowledgeRecords::const_iterator i = local_records.begin ();
           i != local_records.end (); ++i)
      {
        if (i->second->exists ())
        {
          // get the encoded size of the record for checking buffer boundaries
          int64_t encoded_size = i->second->get_encoded_size (i->first);
          ++checkpoint_header.updates;
          meta.size += encoded_size;
          checkpoint_header.size += encoded_size;

          if (encoded_size > buffer_remaining)
          {
            /**
             * if the record is larger than the buffer we have remaining, then
             * write the buffer to the file
             **/
            current = buffer.get_ptr ();
            fwrite (current,
              (size_t)(max_buffer - buffer_remaining), 1, file);
            total_written += (int64_t)(max_buffer - buffer_remaining);
            buffer_remaining = max_buffer;

            madara_logger_ptr_log (logger_, logger::LOG_MINOR,
              "ThreadSafeContext::save_checkpoint:" \
              " encoded_size larger than remaining buffer. Flushing\n");

            if (encoded_size > max_buffer)
            {
              /**
               * If the record is larger than the buffer, then we must allocate a
               * buffer large enough to write to it.
               **/
              buffer = new char[encoded_size];
              max_buffer = encoded_size;
              buffer_remaining = max_buffer;
              current = buffer.get_ptr ();

              madara_logger_ptr_log (logger_, logger::LOG_MINOR,
                "ThreadSafeContext::save_checkpoint:" \
                " encoded_size larger than entire buffer. Reallocating\n");
            } // end if larger than buffer
          } // end if larger than buffer remaining

          current = i->second->write (current, i->first, buffer_remaining);
        }
      }

      if (buffer_remaining != max_buffer)
      {
        fwrite (buffer.get_ptr (),
          (size_t) (max_buffer - buffer_remaining), 1, file);
        total_written += (size_t) (max_buffer - buffer_remaining);
      }

      madara_logger_ptr_log (logger_, logger::LOG_MINOR,
        "ThreadSafeContext::save_checkpoint:" \
        " updating file meta data\n");
  
      // update the meta data at the front
      fseek (file, 0, SEEK_SET);

      current = buffer.get_ptr ();
      buffer_remaining = max_buffer;
      ++meta.states;

      current = meta.write (current, buffer_remaining);

      fwrite (buffer.get_ptr (), current - buffer.get_ptr (), 1, file);

      madara_logger_ptr_log (logger_, logger::LOG_MINOR,
        "ThreadSafeContext::save_checkpoint:" \
        " updating checkpoint meta data\n");
  
      // update the checkpoint meta data
      fseek (file, checkpoint_start, SEEK_SET);
      
      current = buffer.get_ptr ();
      buffer_remaining = max_buffer;

      current = checkpoint_header.write (current, buffer_remaining);

      fwrite (buffer.get_ptr (), current - buffer.get_ptr (), 1, file);
    }

    fclose (file);
  }

  return checkpoint_header.size;
}

