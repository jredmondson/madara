#include "Map.h"
#include "madara/knowledge_engine/Context_Guard.h"

#include <algorithm>

Madara::Knowledge_Engine::Containers::Map::Map (
  const Knowledge_Update_Settings & settings,
  const std::string & delimeter)
: context_ (0), settings_ (settings), delimeter_ (delimeter)
{
}
  

Madara::Knowledge_Engine::Containers::Map::Map (
  const std::string & name,
  Knowledge_Base & knowledge,
  const Knowledge_Update_Settings & settings,
  const std::string & delimeter)
: context_ (&(knowledge.get_context ())), name_ (name), settings_ (settings),
  delimeter_ (delimeter)
{
  std::map <std::string, Knowledge_Record> contents;
  std::string common = name + delimeter_;
  context_->to_map (common, contents);
  
  Knowledge_Update_Settings keep_local (true);
  if (contents.size () > 0)
  {
    for (std::map <std::string, Knowledge_Record>::iterator i =
      contents.begin (); i != contents.end (); ++i)
    {
      map_[i->first.substr (common.size ())] =
        knowledge.get_ref (i->first, keep_local);
    }
  }
}
 
Madara::Knowledge_Engine::Containers::Map::Map (
  const std::string & name,
  Variables & knowledge,
  const Knowledge_Update_Settings & settings,
  const std::string & delimeter)
: context_ (knowledge.get_context ()), name_ (name), settings_ (settings),
  delimeter_ (delimeter)
{
  std::map <std::string, Knowledge_Record> contents;
  std::string common = name + delimeter_;
  context_->to_map (common, contents);
  
  Knowledge_Update_Settings keep_local (true);
  if (contents.size () > 0)
  {
    for (std::map <std::string, Knowledge_Record>::iterator i =
      contents.begin (); i != contents.end (); ++i)
    {
      map_[i->first.substr (common.size ())] =
        knowledge.get_ref (i->first, keep_local);
    }
  }
}
       

Madara::Knowledge_Engine::Containers::Map::Map (const Map & rhs)
: context_ (rhs.context_), name_ (rhs.name_),
  map_ (rhs.map_), settings_ (rhs.settings_),
  delimeter_ (rhs.delimeter_)
{

}

Madara::Knowledge_Engine::Containers::Map::~Map ()
{

}
  
void
Madara::Knowledge_Engine::Containers::Map::modify (void)
{
  Context_Guard context_guard (*context_);
  if (context_ && name_ != "")
  {
    for (Internal_Map::const_iterator index = map_.end ();
         index != map_.end (); ++index)
    {
      context_->mark_modified (index->second);
    }
  }
}
  
void
Madara::Knowledge_Engine::Containers::Map::modify (const std::string & index)
{
  Context_Guard context_guard (*context_);
  if (context_ && name_ != "" && map_.find (index) != map_.end ())
  {
    context_->mark_modified (map_[index]);
  }
}


void
Madara::Knowledge_Engine::Containers::Map::operator= (
  const Map & rhs)
{
  if (this != &rhs)
  {
    Guard guard (mutex_), guard2 (rhs.mutex_);

    this->context_ = rhs.context_;
    this->name_ = rhs.name_;
    this->settings_ = rhs.settings_;
    this->map_ = rhs.map_;
  }
}

Madara::Knowledge_Record
Madara::Knowledge_Engine::Containers::Map::to_record (
  const std::string & key)
{
  Knowledge_Record result;

  if (context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);
    std::stringstream buffer;
    buffer << name_;
    buffer << delimeter_;
    buffer << key;
  
    Knowledge_Update_Settings keep_local (true);
    std::string final_key = buffer.str ();
    std::map <std::string, Variable_Reference>::const_iterator entry =
      map_.find (final_key);

    if (entry == map_.end ())
    {
      Variable_Reference ref = context_->get_ref (final_key, keep_local);
      map_[key] = ref;
      return context_->get (ref, keep_local);
    }

    result = context_->get (entry->second, keep_local);
  }

  return result;
}

Madara::Knowledge_Record
Madara::Knowledge_Engine::Containers::Map::operator[] (
  const std::string & key)
{
  return to_record (key);
}

size_t
Madara::Knowledge_Engine::Containers::Map::size (void) const
{
  Guard guard (mutex_);
  return map_.size ();
}


std::vector <std::string>
Madara::Knowledge_Engine::Containers::Map::sync_keys (void)
{
  std::vector <std::string> additions;

  if (context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    std::map <std::string, Knowledge_Record> contents;
    std::string common = name_ + delimeter_;
    context_->to_map (common, contents);
    Knowledge_Update_Settings keep_local (true);

    for (std::map <std::string, Knowledge_Record>::iterator i =
      contents.begin (); i != contents.end (); ++i)
    {
      std::string key = i->first.substr (common.size ());

      if (map_.find (key) == map_.end ())
      {
        additions.push_back (key);
        map_[key] = context_->get_ref (i->first, keep_local);
      }
    }
  }

  return additions;
}


void
Madara::Knowledge_Engine::Containers::Map::exchange (
  Map & other, bool refresh_keys, bool delete_keys)
{
  if (context_ && other.context_)
  {
    Context_Guard context_guard (*context_);
    Context_Guard other_context_guard (*other.context_);
    Guard guard (mutex_), guard2 (other.mutex_);

    if (refresh_keys)
    {
      other.sync_keys ();
      this->sync_keys ();
    }

    Internal_Map leftover (other.map_);
    Internal_Map::iterator i = map_.begin ();
    while (i != map_.end ())
    {
      // temp = this[i->first]
      Knowledge_Record temp = this->context_->get (i->second, this->settings_);

      // check if the other map has the key
      Internal_Map::iterator other_found = other.map_.find (i->first);

      if (other_found != other.map_.end ())
      {
        // this[i->first] = other[i->first]
        this->context_->set (i->second, 
          other.context_->get (other_found->second, other.settings_),
          this->settings_);

        // other[i->first] = temp
        other.map_ [i->first] = other_found->second;
        other.context_->set (other_found->second, temp, other.settings_);

        // remove item from the items leftover
        leftover.erase (i->first);
      }
      else
      {
        std::stringstream buffer;
        buffer << other.name_;
        buffer << delimeter_;
        buffer << i->first;

        // other[i->first] = temp
        Variable_Reference ref = other.context_->get_ref (
          buffer.str (), other.settings_);
        other.map_[i->first] = ref;
        other.context_->set (ref, temp, other.settings_);
      
        // this[i->first] = zero
        if (delete_keys)
        {
          std::stringstream buffer2;
          buffer2 << this->name_;
          buffer2 << delimeter_;
          buffer2 << i->first;

          this->context_->delete_variable (buffer2.str (), this->settings_);
          this->map_.erase (i++);
        }
        else
        {
          Knowledge_Record zero;
          this->context_->set (i->second, zero, this->settings_);
          ++i;
        }
      }
    }

    // iterate over the elements in the other.map_ that are not in the current map
  
    for (i = leftover.begin (); i != leftover.end (); ++i)
    {
      if (map_.find (i->first) == map_.end ())
      {
        std::stringstream buffer;
        buffer << name_;
        buffer << delimeter_;
        buffer << i->first;

        Variable_Reference ref = context_->get_ref (buffer.str (), settings_);
        this->map_[i->first] = ref;

        // this[i->first] = other[i->first]
        this->context_->set (ref,
          other.context_->get (i->second, other.settings_),
          this->settings_);
      
        // other[i->first] = zero;
        if (delete_keys)
        {
          std::stringstream buffer2;
          buffer2 << other.name_;
          buffer2 << delimeter_;
          buffer2 << i->first;

          other.context_->delete_variable (buffer2.str (), settings_);
          other.map_.erase (i->first);
        }
        else
        {
          Knowledge_Record zero;
          other.context_->set (i->second, zero, other.settings_);
        }
      }
    }
  }
}

void
Madara::Knowledge_Engine::Containers::Map::clear (bool clear_knowledge)
{
  if (clear_knowledge)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    std::vector <std::string> keys;
    this->keys (keys);

    for (size_t i = 0; i < keys.size (); ++i)
      this->erase (keys[i]);
  }
  else
  {
    Guard guard (mutex_);
    map_.clear ();
  }
}

void
Madara::Knowledge_Engine::Containers::Map::erase (const std::string & key)
{
  // find the key in the internal map
  Internal_Map::iterator found = map_.find (key);

  // delete the variable if it has been found
  if (found != map_.end ())
  {
    context_->delete_variable (found->second.get_name ());
  }
}

std::string
Madara::Knowledge_Engine::Containers::Map::get_name (void) const
{
  Guard guard (mutex_);
  return name_;
}

void
Madara::Knowledge_Engine::Containers::Map::set_name (
  const std::string & var_name,
  Knowledge_Base & knowledge, bool sync)
{
  if (context_ != &(knowledge.get_context ()) || name_ != var_name)
  {
    context_ = &(knowledge.get_context ());

    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    name_ = var_name;

    // the old map will no longer be appropriate
    clear (false);
    if (sync)
      sync_keys ();
  }
}

void
Madara::Knowledge_Engine::Containers::Map::set_name (
  const std::string & var_name,
  Variables & knowledge, bool sync)
{
  if (context_ != knowledge.get_context () || name_ != var_name)
  {
    context_ = knowledge.get_context ();

    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    name_ = var_name;

    // the old map will no longer be appropriate
    clear (false);
    if (sync)
      sync_keys ();
  }
}

void
Madara::Knowledge_Engine::Containers::Map::set_delimiter (
  const std::string & delimeter,
  bool sync)
{
  delimeter_ = delimeter;

  if (context_)
  {
    // the old map will no longer be appropriate
    clear (false);
    if (sync)
      sync_keys ();
  }
}


std::string
Madara::Knowledge_Engine::Containers::Map::get_delimiter (void)
{
  return delimeter_;
}

bool
Madara::Knowledge_Engine::Containers::Map::exists (
  const std::string & key) const
{
  bool result (false);

  Internal_Map::const_iterator found = map_.find (key);

  if (context_ && found != map_.end ())
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    result = context_->exists (found->second);
  }

  return result;
}

void
Madara::Knowledge_Engine::Containers::Map::keys (
  std::vector <std::string> & curkeys) const
{
  Guard guard (mutex_);
  curkeys.resize (map_.size ());
  unsigned int j = 0;

  for (std::map <std::string, Variable_Reference>::const_iterator i =
         map_.begin ();
       i != map_.end (); ++i, ++j)
  {
    curkeys[j] = i->first;
  }
}



int
Madara::Knowledge_Engine::Containers::Map::read_file (
  const std::string & key,
  const std::string & filename)
{
  int result = -1;

  if (context_ && key != "")
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    std::stringstream buffer;
    buffer << name_;
    buffer << delimeter_;
    buffer << key;
  
    std::string final_key = buffer.str ();
    std::map <std::string, Variable_Reference>::iterator entry =
      map_.find (final_key);

    if (entry == map_.end ())
    {
      Variable_Reference ref = context_->get_ref (final_key, settings_);
      map_[key] = ref;
      result = context_->read_file (ref, filename, settings_);
    }
    else
    {
      result = context_->read_file (entry->second, filename, settings_);
    }
  }
  return result;
}
 
int
Madara::Knowledge_Engine::Containers::Map::read_file (
  const std::string & key,
  const std::string & filename, 
  const Knowledge_Update_Settings & settings)
{
  int result = -1;

  if (context_ && key != "")
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    std::stringstream buffer;
    buffer << name_;
    buffer << delimeter_;
    buffer << key;
  
    std::string final_key = buffer.str ();
    std::map <std::string, Variable_Reference>::iterator entry =
      map_.find (final_key);

    if (entry == map_.end ())
    {
      Variable_Reference ref = context_->get_ref (final_key, settings);
      map_[key] = ref;
      result = context_->read_file (ref, filename, settings);
    }
    else
    {
      result = context_->read_file (entry->second, filename, settings);
    }
  }
  return result;
}
      
int Madara::Knowledge_Engine::Containers::Map::set (const std::string & key,
  Madara::Knowledge_Record::Integer value)
{
  int result = -1;

  if (context_ && key != "")
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);
    std::stringstream buffer;
    buffer << name_;
    buffer << delimeter_;
    buffer << key;

    std::string final_key = buffer.str ();
    std::map <std::string, Variable_Reference>::iterator entry =
      map_.find (final_key);

    if (entry == map_.end ())
    {
      Variable_Reference ref = context_->get_ref (final_key, settings_);
      map_[key] = ref;
      result = context_->set (ref, value, settings_);
    }
    else
    {
      result = context_->set (entry->second, value, settings_);
    }
  }

  return result;
}

int Madara::Knowledge_Engine::Containers::Map::set (const std::string & key,
  Madara::Knowledge_Record::Integer value, 
  const Knowledge_Update_Settings & settings)
{
  int result = -1;

  if (context_ && key != "")
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    std::stringstream buffer;
    buffer << name_;
    buffer << delimeter_;
    buffer << key;

    std::string final_key = buffer.str ();
    std::map <std::string, Variable_Reference>::iterator entry =
      map_.find (final_key);

    if (entry == map_.end ())
    {
      Variable_Reference ref = context_->get_ref (final_key, settings);
      map_[key] = ref;
      result = context_->set (ref, value, settings);
    }
    else
    {
      result = context_->set (entry->second, value, settings);
    }
  }

  return result;
}

int Madara::Knowledge_Engine::Containers::Map::set_index (
  const std::string & key,
  size_t index,
  Madara::Knowledge_Record::Integer value)
{
  int result = -1;

  if (context_ && key != "")
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    std::stringstream buffer;
    buffer << name_;
    buffer << delimeter_;
    buffer << key;

    std::string final_key = buffer.str ();
    std::map <std::string, Variable_Reference>::iterator entry =
      map_.find (final_key);

    if (entry == map_.end ())
    {
      Variable_Reference ref = context_->get_ref (final_key, settings_);
      map_[key] = ref;
      result = context_->set_index (ref, index, value, settings_);
    }
    else
    {
      result = context_->set_index (entry->second, index, value, settings_);
    }
  }

  return result;
}

int Madara::Knowledge_Engine::Containers::Map::set_index (
  const std::string & key,
  size_t index,
  Madara::Knowledge_Record::Integer value,
  const Knowledge_Update_Settings & settings)
{
  int result = -1;

  if (context_ && key != "")
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    std::stringstream buffer;
    buffer << name_;
    buffer << delimeter_;
    buffer << key;

    std::string final_key = buffer.str ();
    std::map <std::string, Variable_Reference>::iterator entry =
      map_.find (final_key);

    if (entry == map_.end ())
    {
      Variable_Reference ref = context_->get_ref (final_key, settings);
      map_[key] = ref;
      result = context_->set_index (ref, index, value, settings);
    }
    else
    {
      result = context_->set_index (entry->second, index, value, settings);
    }
  }

  return result;
}


int Madara::Knowledge_Engine::Containers::Map::set (const std::string & key,
  const Madara::Knowledge_Record::Integer * value,
  uint32_t size)
{
  int result = -1;

  if (context_ && key != "")
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    std::stringstream buffer;
    buffer << name_;
    buffer << delimeter_;
    buffer << key;

    std::string final_key = buffer.str ();
    std::map <std::string, Variable_Reference>::iterator entry =
      map_.find (final_key);

    if (entry == map_.end ())
    {
      Variable_Reference ref = context_->get_ref (final_key, settings_);
      map_[key] = ref;
      result = context_->set (ref, value, size, settings_);
    }
    else
    {
      result = context_->set (entry->second, value, size, settings_);
    }
  }

  return result;
}
 
int Madara::Knowledge_Engine::Containers::Map::set (const std::string & key,
  const Madara::Knowledge_Record::Integer * value,
  uint32_t size,
  const Knowledge_Update_Settings & settings)
{
  int result = -1;

  if (context_ && key != "")
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    std::stringstream buffer;
    buffer << name_;
    buffer << delimeter_;
    buffer << key;

    std::string final_key = buffer.str ();
    std::map <std::string, Variable_Reference>::iterator entry =
      map_.find (final_key);

    if (entry == map_.end ())
    {
      Variable_Reference ref = context_->get_ref (final_key, settings);
      map_[key] = ref;
      result = context_->set (ref, value, size, settings);
    }
    else
    {
      result = context_->set (entry->second, value, size, settings);
    }
  }

  return result;
}
       
int Madara::Knowledge_Engine::Containers::Map::set (const std::string & key,
  const std::vector <Knowledge_Record::Integer> & value)
{
  int result = -1;

  if (context_ && key != "")
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    std::stringstream buffer;
    buffer << name_;
    buffer << delimeter_;
    buffer << key;

    std::string final_key = buffer.str ();
    std::map <std::string, Variable_Reference>::iterator entry =
      map_.find (final_key);

    if (entry == map_.end ())
    {
      Variable_Reference ref = context_->get_ref (final_key, settings_);
      map_[key] = ref;
      result = context_->set (ref, value, settings_);
    }
    else
    {
      result = context_->set (entry->second, value, settings_);
    }
  }

  return result;
}

int Madara::Knowledge_Engine::Containers::Map::set (const std::string & key,
  const std::vector <Knowledge_Record::Integer> & value,
  const Knowledge_Update_Settings & settings)
{
  int result = -1;

  if (context_ && key != "")
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    std::stringstream buffer;
    buffer << name_;
    buffer << delimeter_;
    buffer << key;

    std::string final_key = buffer.str ();
    std::map <std::string, Variable_Reference>::iterator entry =
      map_.find (final_key);

    if (entry == map_.end ())
    {
      Variable_Reference ref = context_->get_ref (final_key, settings);
      map_[key] = ref;
      return context_->set (ref, value, settings);
    }
    else
    {
      result = context_->set (entry->second, value, settings);
    }
  }

  return result;
}
          
int Madara::Knowledge_Engine::Containers::Map::set (const std::string & key,
  double value)
{
  int result = -1;

  if (context_ && key != "")
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    std::stringstream buffer;
    buffer << name_;
    buffer << delimeter_;
    buffer << key;

    std::string final_key = buffer.str ();
    std::map <std::string, Variable_Reference>::iterator entry =
      map_.find (final_key);

    if (entry == map_.end ())
    {
      Variable_Reference ref = context_->get_ref (final_key, settings_);
      map_[key] = ref;
      result = context_->set (ref, value, settings_);
    }
    else
    {
      result = context_->set (entry->second, value, settings_);
    }
  }

  return result;
}
    
int Madara::Knowledge_Engine::Containers::Map::set (const std::string & key,
  double value, 
  const Knowledge_Update_Settings & settings)
{
  int result = -1;

  if (context_ && key != "")
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    std::stringstream buffer;
    buffer << name_;
    buffer << delimeter_;
    buffer << key;

    std::string final_key = buffer.str ();
    std::map <std::string, Variable_Reference>::iterator entry =
      map_.find (final_key);

    if (entry == map_.end ())
    {
      Variable_Reference ref = context_->get_ref (final_key, settings);
      map_[key] = ref;
      result = context_->set (ref, value, settings);
    }
    else
    {
      result = context_->set (entry->second, value, settings);
    }
  }

  return result;
}

int Madara::Knowledge_Engine::Containers::Map::set_index (const std::string & key,
  size_t index,
  double value)
{
  int result = -1;

  if (context_ && key != "")
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    std::stringstream buffer;
    buffer << name_;
    buffer << delimeter_;
    buffer << key;

    std::string final_key = buffer.str ();
    std::map <std::string, Variable_Reference>::iterator entry =
      map_.find (final_key);

    if (entry == map_.end ())
    {
      Variable_Reference ref = context_->get_ref (final_key, settings_);
      map_[key] = ref;
      result = context_->set_index (ref, index, value, settings_);
    }
    else
    {
      result = context_->set_index (entry->second, index, value, settings_);
    }
  }

  return result;
}

int Madara::Knowledge_Engine::Containers::Map::set_index (const std::string & key,
  size_t index,
  double value,
  const Knowledge_Update_Settings & settings)
{
  int result = -1;

  if (context_ && key != "")
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    std::stringstream buffer;
    buffer << name_;
    buffer << delimeter_;
    buffer << key;

    std::string final_key = buffer.str ();
    std::map <std::string, Variable_Reference>::iterator entry =
      map_.find (final_key);

    if (entry == map_.end ())
    {
      Variable_Reference ref = context_->get_ref (final_key, settings);
      map_[key] = ref;
      result = context_->set_index (ref, index, value, settings);
    }
    else
    {
      result = context_->set_index (entry->second, index, value, settings);
    }
  }

  return result;
}

int Madara::Knowledge_Engine::Containers::Map::set (const std::string & key,
  const double * value,
  uint32_t size)
{
  int result = -1;

  if (context_ && key != "")
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    std::stringstream buffer;
    buffer << name_;
    buffer << delimeter_;
    buffer << key;

    std::string final_key = buffer.str ();
    std::map <std::string, Variable_Reference>::iterator entry =
      map_.find (final_key);

    if (entry == map_.end ())
    {
      Variable_Reference ref = context_->get_ref (final_key, settings_);
      map_[key] = ref;
      result = context_->set (ref, value, size, settings_);
    }
    else
    {
      result = context_->set (entry->second, value, size, settings_);
    }
  }

  return result;
}

int Madara::Knowledge_Engine::Containers::Map::set (const std::string & key,
  const double * value,
  uint32_t size,
  const Knowledge_Update_Settings & settings)
{
  int result = -1;

  if (context_ && key != "")
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    std::stringstream buffer;
    buffer << name_;
    buffer << delimeter_;
    buffer << key;

    std::string final_key = buffer.str ();
    std::map <std::string, Variable_Reference>::iterator entry =
      map_.find (final_key);

    if (entry == map_.end ())
    {
      Variable_Reference ref = context_->get_ref (final_key, settings);
      map_[key] = ref;
      result = context_->set (ref, value, size, settings);
    }
  
    result = context_->set (entry->second, value, size, settings);
  }

  return result;
}
 
int Madara::Knowledge_Engine::Containers::Map::set (const std::string & key,
  const std::vector <double> & value)
{
  int result = -1;

  if (context_ && key != "")
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    std::stringstream buffer;
    buffer << name_;
    buffer << delimeter_;
    buffer << key;

    std::string final_key = buffer.str ();
    std::map <std::string, Variable_Reference>::iterator entry =
      map_.find (final_key);

    if (entry == map_.end ())
    {
      Variable_Reference ref = context_->get_ref (final_key, settings_);
      map_[key] = ref;
      result = context_->set (ref, value, settings_);
    }
    else
    {
      result = context_->set (entry->second, value, settings_);
    }
  }

  return result;
}
 
int Madara::Knowledge_Engine::Containers::Map::set (const std::string & key,
  const std::vector <double> & value,
  const Knowledge_Update_Settings & settings)
{
  int result = -1;

  if (context_ && key != "")
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    std::stringstream buffer;
    buffer << name_;
    buffer << delimeter_;
    buffer << key;

    std::string final_key = buffer.str ();
    std::map <std::string, Variable_Reference>::iterator entry =
      map_.find (final_key);

    if (entry == map_.end ())
    {
      Variable_Reference ref = context_->get_ref (final_key, settings);
      map_[key] = ref;
      result = context_->set (ref, value, settings);
    }
    else
    {
      result = context_->set (entry->second, value, settings);
    }
  }

  return result;
}

int Madara::Knowledge_Engine::Containers::Map::set (const std::string & key,
  const std::string & value)
{
  int result = -1;

  if (context_ && key != "")
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    std::stringstream buffer;
    buffer << name_;
    buffer << delimeter_;
    buffer << key;

    std::string final_key = buffer.str ();
    std::map <std::string, Variable_Reference>::iterator entry =
      map_.find (final_key);

    if (entry == map_.end ())
    {
      Variable_Reference ref = context_->get_ref (final_key, settings_);
      map_[key] = ref;
      result = context_->set (ref, value, settings_);
    }
    else
    {
      result = context_->set (entry->second, value, settings_);
    }
  }

  return result;
}

int Madara::Knowledge_Engine::Containers::Map::set (const std::string & key,
  const std::string & value, 
  const Knowledge_Update_Settings & settings)
{
  int result = -1;

  if (context_ && key != "")
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    std::stringstream buffer;
    buffer << name_;
    buffer << delimeter_;
    buffer << key;

    std::string final_key = buffer.str ();
    std::map <std::string, Variable_Reference>::iterator entry =
      map_.find (final_key);

    if (entry == map_.end ())
    {
      Variable_Reference ref = context_->get_ref (final_key, settings);
      map_[key] = ref;
      result = context_->set (ref, value, settings);
    }
    else
    {
      result = context_->set (entry->second, value, settings);
    }
  }

  return result;
}

int Madara::Knowledge_Engine::Containers::Map::set_file (const std::string & key,
  const unsigned char * value, size_t size)
{
  int result = -1;

  if (context_ && key != "")
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    std::stringstream buffer;
    buffer << name_;
    buffer << delimeter_;
    buffer << key;

    std::string final_key = buffer.str ();
    std::map <std::string, Variable_Reference>::iterator entry =
      map_.find (final_key);

    if (entry == map_.end ())
    {
      Variable_Reference ref = context_->get_ref (final_key, settings_);
      map_[key] = ref;
      result = context_->set_file (ref, value, size, settings_);
    }
    else
    {
      result = context_->set_file (entry->second, value, size, settings_);
    }
  }

  return result;
}

int Madara::Knowledge_Engine::Containers::Map::set_file (const std::string & key,
  const unsigned char * value, size_t size, 
  const Knowledge_Update_Settings & settings)
{
  int result = -1;

  if (context_ && key != "")
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    std::stringstream buffer;
    buffer << name_;
    buffer << delimeter_;
    buffer << key;

    std::string final_key = buffer.str ();
    std::map <std::string, Variable_Reference>::iterator entry =
      map_.find (final_key);

    if (entry == map_.end ())
    {
      Variable_Reference ref = context_->get_ref (final_key, settings);
      map_[key] = ref;
      result = context_->set_file (ref, value, size, settings);
    }
  
    result = context_->set_file (entry->second, value, size, settings);
  }

  return result;
}

int Madara::Knowledge_Engine::Containers::Map::set_jpeg (const std::string & key,
  const unsigned char * value, size_t size)
{
  int result = -1;

  if (context_ && key != "")
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    std::stringstream buffer;
    buffer << name_;
    buffer << delimeter_;
    buffer << key;

    std::string final_key = buffer.str ();
    std::map <std::string, Variable_Reference>::iterator entry =
      map_.find (final_key);

    if (entry == map_.end ())
    {
      Variable_Reference ref = context_->get_ref (final_key, settings_);
      map_[key] = ref;
      result = context_->set_jpeg (ref, value, size, settings_);
    }
    else
    {
      result = context_->set_jpeg (entry->second, value, size, settings_);
    }
  }

  return result;
}

int Madara::Knowledge_Engine::Containers::Map::set_jpeg (const std::string & key,
  const unsigned char * value, size_t size, 
  const Knowledge_Update_Settings & settings)
{
  int result = -1;

  if (context_ && key != "")
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    std::stringstream buffer;
    buffer << name_;
    buffer << delimeter_;
    buffer << key;

    std::string final_key = buffer.str ();
    std::map <std::string, Variable_Reference>::iterator entry =
      map_.find (final_key);

    if (entry == map_.end ())
    {
      Variable_Reference ref = context_->get_ref (final_key, settings);
      map_[key] = ref;
      result = context_->set_jpeg (ref, value, size, settings);
    }
    else
    {
      result = context_->set_jpeg (entry->second, value, size, settings);
    }
  }

  return result;
}

Madara::Knowledge_Engine::Knowledge_Update_Settings
Madara::Knowledge_Engine::Containers::Map::set_settings (
  const Knowledge_Update_Settings & settings)
{
  Guard guard (mutex_);
  
  Knowledge_Update_Settings old_settings = settings_;

  settings_ = settings;

  return old_settings;
}

void Madara::Knowledge_Engine::Containers::Map::set_quality (
  const std::string & key,
  uint32_t quality,
  const Knowledge_Reference_Settings & settings)
{
  if (context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    std::stringstream buffer;
    buffer << name_;
    buffer << delimeter_;
    buffer << key;

    context_->set_quality (buffer.str (), quality, true, settings);
  }
}
