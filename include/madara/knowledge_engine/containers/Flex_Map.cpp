#include "Flex_Map.h"
#include "madara/knowledge_engine/Context_Guard.h"

#include <algorithm>

Madara::Knowledge_Engine::Containers::Flex_Map::Flex_Map (
  const Knowledge_Update_Settings & settings,
  const std::string & delimiter)
  : Base_Container ("", settings), context_ (0), delimiter_ (".")
{
}
  

Madara::Knowledge_Engine::Containers::Flex_Map::Flex_Map (
  const std::string & name,
  Knowledge_Base & knowledge,
  const Knowledge_Update_Settings & settings,
  const std::string & delimiter)
  : Base_Container (name, settings), context_ (&(knowledge.get_context ())),
  delimiter_ (".")
{
}
 
Madara::Knowledge_Engine::Containers::Flex_Map::Flex_Map (
  const std::string & name,
  Variables & knowledge,
  const Knowledge_Update_Settings & settings,
  const std::string & delimiter)
  : Base_Container (name, settings), context_ (knowledge.get_context ()),
  delimiter_ (".")
{
}
       

Madara::Knowledge_Engine::Containers::Flex_Map::Flex_Map (const Flex_Map & rhs)
  : Base_Container (rhs), context_ (rhs.context_),
  variable_ (rhs.variable_),
  delimiter_ (rhs.delimiter_)
{

}

Madara::Knowledge_Engine::Containers::Flex_Map::~Flex_Map ()
{

}

/**
* Checks if the value in the record is not false (0)
* @return  true if value is not false
**/
bool Madara::Knowledge_Engine::Containers::Flex_Map::is_true (void)
{
  Context_Guard context_guard (*context_);
  bool result (false);

  Knowledge_Update_Settings keep_local (true);

  if (!variable_.is_valid ())
  {
    result = context_->get (variable_, keep_local).is_true ();
  }

  return result;
}

/**
* Checks if the value in the record is false (0)
* @return  true if value is false
**/
bool Madara::Knowledge_Engine::Containers::Flex_Map::is_false (void)
{
  Context_Guard context_guard (*context_);
  bool result (true);

  Knowledge_Update_Settings keep_local (true);

  if (!variable_.is_valid ())
  {
    result = context_->get (variable_, keep_local).is_true ();
  }

  return result;
}

void
Madara::Knowledge_Engine::Containers::Flex_Map::modify (void)
{
  if (context_ && name_ != "")
  {
    Context_Guard context_guard (*context_);

    // get all children
    Knowledge_Base knowledge;
    knowledge.facade_for (*context_);
    Map map (name_, knowledge, settings_, delimiter_);

    // modify the children
    map.modify ();

    // modify ourself
    if (variable_.is_valid ())
    {
      knowledge.mark_modified (variable_);
    }
  }
}

std::string
Madara::Knowledge_Engine::Containers::Flex_Map::get_debug_info (void)
{
  std::stringstream result;

  result << "Flex Map: ";

  if (context_ && name_ != "")
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    // get all children
    Knowledge_Base knowledge;
    knowledge.facade_for (*context_);
    Map map (name_, knowledge, settings_, delimiter_);

    result << map.get_debug_info ();
  }

  return result.str ();
}


void
Madara::Knowledge_Engine::Containers::Flex_Map::modify_ (void)
{
  modify ();
}

std::string
Madara::Knowledge_Engine::Containers::Flex_Map::get_debug_info_ (void)
{
  return get_debug_info ();
}

Madara::Knowledge_Engine::Containers::Base_Container *
Madara::Knowledge_Engine::Containers::Flex_Map::clone (void) const
{
  return new Flex_Map (*this);
}

void
Madara::Knowledge_Engine::Containers::Flex_Map::operator= (
  const Flex_Map & rhs)
{
  if (this != &rhs)
  {
    Guard guard (mutex_), guard2 (rhs.mutex_);

    this->context_ = rhs.context_;
    this->name_ = rhs.name_;
    this->settings_ = rhs.settings_;
    this->variable_ = rhs.variable_;
  }
}


void
Madara::Knowledge_Engine::Containers::Flex_Map::operator= (
  Knowledge_Record::Integer value)
{
  set (value, settings_);
}

void
Madara::Knowledge_Engine::Containers::Flex_Map::operator= (
int value)
{
  set (Knowledge_Record::Integer (value), settings_);
}

void
Madara::Knowledge_Engine::Containers::Flex_Map::operator= (
  double value)
{
  set (value, settings_);
}

void
Madara::Knowledge_Engine::Containers::Flex_Map::operator= (
  const std::string value)
{
  set (value, settings_);
}

void
Madara::Knowledge_Engine::Containers::Flex_Map::operator= (
  const std::vector <Knowledge_Record::Integer> & value)
{
  set (value, settings_);
}

void
Madara::Knowledge_Engine::Containers::Flex_Map::operator= (
const std::vector <double> & value)
{
  set (value, settings_);
}


Madara::Knowledge_Record
Madara::Knowledge_Engine::Containers::Flex_Map::to_record (void) const
{
  Knowledge_Record result;

  if (context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    Knowledge_Update_Settings keep_local (true);

    if (!variable_.is_valid ())
    {
      this->update_variable ();
    }

    result = context_->get (variable_, settings_);
  }

  return result;
}

Madara::Knowledge_Record::Integer
Madara::Knowledge_Engine::Containers::Flex_Map::to_integer (void) const
{
  return to_record ().to_integer ();
}

double
Madara::Knowledge_Engine::Containers::Flex_Map::to_double (void) const
{
  return to_record ().to_double ();
}

std::string
Madara::Knowledge_Engine::Containers::Flex_Map::to_string (void) const
{
  return to_record ().to_string ();
}


void
Madara::Knowledge_Engine::Containers::Flex_Map::to_container (
  Buffer_Vector & target) const
{
  if (context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    // get all children
    Knowledge_Base knowledge;
    knowledge.facade_for (*context_);

    target.set_delimiter (delimiter_);
    target.set_name (name_, knowledge);
  }
}


void
Madara::Knowledge_Engine::Containers::Flex_Map::to_container (
  Double_Vector & target) const
{
  if (context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    // get all children
    Knowledge_Base knowledge;
    knowledge.facade_for (*context_);

    target.set_delimiter (delimiter_);
    target.set_name (name_, knowledge);
  }
}


void
Madara::Knowledge_Engine::Containers::Flex_Map::to_container (
  Integer_Vector & target) const
{
  if (context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    // get all children
    Knowledge_Base knowledge;
    knowledge.facade_for (*context_);

    target.set_delimiter (delimiter_);
    target.set_name (name_, knowledge);
  }
}

void
Madara::Knowledge_Engine::Containers::Flex_Map::to_container (
  Native_Integer_Vector & target) const
{
  if (context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    // get all children
    Knowledge_Base knowledge;
    knowledge.facade_for (*context_);

    target.set_name (name_, knowledge);
  }
}


void
Madara::Knowledge_Engine::Containers::Flex_Map::to_container (
  String_Vector & target) const
{
  if (context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    // get all children
    Knowledge_Base knowledge;
    knowledge.facade_for (*context_);

    target.set_delimiter (delimiter_);
    target.set_name (name_, knowledge);
  }
}


void
Madara::Knowledge_Engine::Containers::Flex_Map::to_container (
  Double & target) const
{
  if (context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    // get all children
    Knowledge_Base knowledge;
    knowledge.facade_for (*context_);

    target.set_name (name_, knowledge);
  }
}

void
Madara::Knowledge_Engine::Containers::Flex_Map::to_container (
  Native_Double_Vector & target) const
{
  if (context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    // get all children
    Knowledge_Base knowledge;
    knowledge.facade_for (*context_);

    target.set_name (name_, knowledge);
  }
}


void
Madara::Knowledge_Engine::Containers::Flex_Map::to_container (
  Integer & target) const
{
  if (context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    // get all children
    Knowledge_Base knowledge;
    knowledge.facade_for (*context_);

    target.set_name (name_, knowledge);
  }
}


void
Madara::Knowledge_Engine::Containers::Flex_Map::to_container (
  String & target) const
{
  if (context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    // get all children
    Knowledge_Base knowledge;
    knowledge.facade_for (*context_);

    target.set_name (name_, knowledge);
  }
}


void
Madara::Knowledge_Engine::Containers::Flex_Map::to_container (
  Map & target) const
{
  if (context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    // get all children
    Knowledge_Base knowledge;
    knowledge.facade_for (*context_);

    target.set_delimiter (delimiter_);
    target.set_name (name_, knowledge);
  }
}


Madara::Knowledge_Engine::Containers::Flex_Map
Madara::Knowledge_Engine::Containers::Flex_Map::operator[] (
  const std::string & key)
{
  Flex_Map map (settings_, delimiter_);

  if (key != "" && context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    Knowledge_Base knowledge;
    knowledge.facade_for (*context_);
    map.set_name (name_ + delimiter_ + key, knowledge);
  }

  return map;
}

Madara::Knowledge_Engine::Containers::Flex_Map
Madara::Knowledge_Engine::Containers::Flex_Map::operator[] (
  size_t index)
{
  Flex_Map map (settings_, delimiter_);

  if (context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    std::stringstream buffer;
    Knowledge_Base knowledge;
    knowledge.facade_for (*context_);

    buffer << name_ << delimiter_ << index;

    map.set_name (buffer.str (), knowledge);
  }

  return map;
}

size_t
Madara::Knowledge_Engine::Containers::Flex_Map::size (
  bool first_level_keys_only) const
{
  Context_Guard context_guard (*context_);
  Guard guard (mutex_);

  size_t result (0);

  if (!first_level_keys_only)
  {
    // get all children
    Knowledge_Base knowledge;
    knowledge.facade_for (*context_);
    Map map (name_, knowledge, settings_, delimiter_);

    result = map.size ();
  }
  else
  {
    // use the context's specialized mapping feature
    std::vector<std::string> next_keys;
    std::map<std::string, Madara::Knowledge_Record> all_record_vars;
    context_->to_map (name_, delimiter_, "",
      next_keys, all_record_vars, true);

    result = next_keys.size ();
  }

  return result;
}

void
Madara::Knowledge_Engine::Containers::Flex_Map::exchange (
  Flex_Map & other, bool refresh_keys, bool delete_keys)
{
  if (context_ && other.context_)
  {
    Context_Guard context_guard (*context_);
    Context_Guard other_context_guard (*other.context_);
    Guard guard (mutex_), guard2 (other.mutex_);

    // use the Map container, which already has exchange implemented
    Knowledge_Base knowledge;
    knowledge.facade_for (*context_);
    Map this_map (name_, knowledge, settings_, delimiter_);
    Map other_map (other.name_, knowledge, other.settings_, delimiter_);

    this_map.exchange (other_map, refresh_keys, delete_keys);
  }
}

void
Madara::Knowledge_Engine::Containers::Flex_Map::clear (void)
{
  if (context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    // use the Map container, which already has clear implemented
    Knowledge_Base knowledge;
    knowledge.facade_for (*context_);
    Map map (name_, knowledge, settings_, delimiter_);

    map.clear ();

    context_->delete_variable (name_, settings_);
  }
}

void
Madara::Knowledge_Engine::Containers::Flex_Map::erase (
  const std::string & key, bool delete_subkeys)
{
  if (context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    context_->delete_variable (name_ + delimiter_ + key, settings_);
    
    if (delete_subkeys)
    {
      // use the Map container, which already has clear implemented
      Knowledge_Base knowledge;
      knowledge.facade_for (*context_);
      Map map (name_ + delimiter_ + key, knowledge, settings_, delimiter_);

      map.clear ();
    }
  }
}

void
Madara::Knowledge_Engine::Containers::Flex_Map::set_name (
  const std::string & var_name,
  Knowledge_Base & knowledge)
{
  if (context_ != &(knowledge.get_context ()) || name_ != var_name)
  {
    context_ = &(knowledge.get_context ());

    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    name_ = var_name;

    if (context_->exists (var_name, settings_))
    {
      Knowledge_Update_Settings keep_local (true);

      variable_ = context_->get_ref (var_name, keep_local);
    }
    else
    {
      // reset variable reference
      variable_ = Variable_Reference ();
    }
  }
}

void
Madara::Knowledge_Engine::Containers::Flex_Map::set_name (
  const std::string & var_name,
  Variables & knowledge)
{
  if (context_ != knowledge.get_context () || name_ != var_name)
  {
    context_ = knowledge.get_context ();

    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    name_ = var_name;

    if (context_->exists (var_name, settings_))
    {
      update_variable ();
    }
    else
    {
      // reset variable reference
      variable_ = Variable_Reference ();
    }
  }
}

void
Madara::Knowledge_Engine::Containers::Flex_Map::update_variable (void) const
{
  if (context_ && name_ != "")
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    Knowledge_Update_Settings keep_local (true);

    variable_ = context_->get_ref (name_, keep_local);
  }
}

void
Madara::Knowledge_Engine::Containers::Flex_Map::set_delimiter (
const std::string & delimiter)
{
  delimiter_ = delimiter;
}


std::string
Madara::Knowledge_Engine::Containers::Flex_Map::get_delimiter (void)
{
  return delimiter_;
}

bool
Madara::Knowledge_Engine::Containers::Flex_Map::exists (
  const std::string & key, bool first_level_key) const
{
  bool result (false);

  if (context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    // check if the key exists
    std::string exact_key = name_ + delimiter_ + key;
    result = context_->exists (exact_key);

    if (!result && first_level_key)
    {
      std::vector <std::string> curkeys;
      keys (curkeys);

      std::vector <std::string>::iterator found;

      // look for the key in the sub keys
      found = std::find (curkeys.begin (), curkeys.end (), key);

      result = found != curkeys.end ();
    }
  }

  return result;
}

void
Madara::Knowledge_Engine::Containers::Flex_Map::keys (
  std::vector <std::string> & curkeys,
  bool first_level_keys_only) const
{
  Context_Guard context_guard (*context_);
  Guard guard (mutex_);

  if (!first_level_keys_only)
  {
    // use the Map container, which already has clear implemented
    Knowledge_Base knowledge;
    knowledge.facade_for (*context_);
    Map map (name_, knowledge, settings_, delimiter_);

    map.keys (curkeys);
  }
  else
  {
    // use the context's specialized mapping feature
    std::vector<std::string> next_keys;
    std::map<std::string, Madara::Knowledge_Record> all_record_vars;
    context_->to_map (name_, delimiter_, "",
      curkeys, all_record_vars, true);
  }
}

int
Madara::Knowledge_Engine::Containers::Flex_Map::read_file (
  const std::string & filename)
{
  int result = -1;

  if (context_ && name_ != "")
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    if (!variable_.is_valid ())
    {
      update_variable ();
    }

    result = context_->read_file (variable_, filename, settings_);
  }
  return result;
}
 
int
Madara::Knowledge_Engine::Containers::Flex_Map::read_file (
  
  const std::string & filename, 
  const Knowledge_Update_Settings & settings)
{
  int result = -1;

  if (context_ && name_ != "")
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    if (!variable_.is_valid ())
    {
      update_variable ();
    }

    result = context_->read_file (variable_, filename, settings);
  }
  return result;
}
      
int Madara::Knowledge_Engine::Containers::Flex_Map::set (
  Madara::Knowledge_Record::Integer value)
{
  int result = -1;

  if (context_ && name_ != "")
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    if (!variable_.is_valid ())
    {
      update_variable ();
    }

    result = context_->set (variable_, value, settings_);
  }

  return result;
}

int Madara::Knowledge_Engine::Containers::Flex_Map::set (
  Madara::Knowledge_Record::Integer value, 
  const Knowledge_Update_Settings & settings)
{
  int result = -1;

  if (context_ && name_ != "")
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    if (!variable_.is_valid ())
    {
      update_variable ();
    }

    result = context_->set (variable_, value, settings);
  }

  return result;
}

int Madara::Knowledge_Engine::Containers::Flex_Map::set_index (
  
  size_t index,
  Madara::Knowledge_Record::Integer value)
{
  int result = -1;

  if (context_ && name_ != "")
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    if (!variable_.is_valid ())
    {
      update_variable ();
    }

    result = context_->set_index (variable_, index, value, settings_);
  }

  return result;
}

int Madara::Knowledge_Engine::Containers::Flex_Map::set_index (
  
  size_t index,
  Madara::Knowledge_Record::Integer value,
  const Knowledge_Update_Settings & settings)
{
  int result = -1;

  if (context_ && name_ != "")
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    if (!variable_.is_valid ())
    {
      update_variable ();
    }

    result = context_->set_index (variable_, index, value, settings);
  }

  return result;
}


int Madara::Knowledge_Engine::Containers::Flex_Map::set (
  const Madara::Knowledge_Record::Integer * value,
  uint32_t size)
{
  int result = -1;

  if (context_ && name_ != "")
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    if (!variable_.is_valid ())
    {
      update_variable ();
    }

    result = context_->set (variable_, value, size, settings_);
  }

  return result;
}
 
int Madara::Knowledge_Engine::Containers::Flex_Map::set (
  const Madara::Knowledge_Record::Integer * value,
  uint32_t size,
  const Knowledge_Update_Settings & settings)
{
  int result = -1;

  if (context_ && name_ != "")
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    if (!variable_.is_valid ())
    {
      update_variable ();
    }

    result = context_->set (variable_, value, size, settings);
  }

  return result;
}
       
int Madara::Knowledge_Engine::Containers::Flex_Map::set (
  const std::vector <Knowledge_Record::Integer> & value)
{
  int result = -1;

  if (context_ && name_ != "")
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    if (!variable_.is_valid ())
    {
      update_variable ();
    }

    result = context_->set (variable_, value, settings_);
  }

  return result;
}

int Madara::Knowledge_Engine::Containers::Flex_Map::set (
  const std::vector <Knowledge_Record::Integer> & value,
  const Knowledge_Update_Settings & settings)
{
  int result = -1;

  if (context_ && name_ != "")
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    if (!variable_.is_valid ())
    {
      update_variable ();
    }

    result = context_->set (variable_, value, settings);
  }

  return result;
}
          
int Madara::Knowledge_Engine::Containers::Flex_Map::set (
  double value)
{
  int result = -1;

  if (context_ && name_ != "")
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    if (!variable_.is_valid ())
    {
      update_variable ();
    }

    result = context_->set (variable_, value, settings_);
  }

  return result;
}
    
int Madara::Knowledge_Engine::Containers::Flex_Map::set (
  double value, 
  const Knowledge_Update_Settings & settings)
{
  int result = -1;

  if (context_ && name_ != "")
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    if (!variable_.is_valid ())
    {
      update_variable ();
    }

    result = context_->set (variable_, value, settings);
  }

  return result;
}

int Madara::Knowledge_Engine::Containers::Flex_Map::set_index (
  size_t index,
  double value)
{
  int result = -1;

  if (context_ && name_ != "")
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    if (!variable_.is_valid ())
    {
      update_variable ();
    }

    result = context_->set_index (variable_, index, value, settings_);
  }

  return result;
}

int Madara::Knowledge_Engine::Containers::Flex_Map::set_index (
  size_t index,
  double value,
  const Knowledge_Update_Settings & settings)
{
  int result = -1;

  if (context_ && name_ != "")
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    if (!variable_.is_valid ())
    {
      update_variable ();
    }

    result = context_->set_index (variable_, index, value, settings);
  }

  return result;
}

int Madara::Knowledge_Engine::Containers::Flex_Map::set (
  const double * value,
  uint32_t size)
{
  int result = -1;

  if (context_ && name_ != "")
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    if (!variable_.is_valid ())
    {
      update_variable ();
    }

    result = context_->set (variable_, value, size, settings_);
  }

  return result;
}

int Madara::Knowledge_Engine::Containers::Flex_Map::set (
  const double * value,
  uint32_t size,
  const Knowledge_Update_Settings & settings)
{
  int result = -1;

  if (context_ && name_ != "")
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    if (!variable_.is_valid ())
    {
      update_variable ();
    }

    result = context_->set (variable_, value, size, settings);
  }

  return result;
}
 
int Madara::Knowledge_Engine::Containers::Flex_Map::set (
  const std::vector <double> & value)
{
  int result = -1;

  if (context_ && name_ != "")
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    if (!variable_.is_valid ())
    {
      update_variable ();
    }

    result = context_->set (variable_, value, settings_);
  }

  return result;
}
 
int Madara::Knowledge_Engine::Containers::Flex_Map::set (
  const std::vector <double> & value,
  const Knowledge_Update_Settings & settings)
{
  int result = -1;

  if (context_ && name_ != "")
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    if (!variable_.is_valid ())
    {
      update_variable ();
    }

    result = context_->set (variable_, value, settings);
  }

  return result;
}

int Madara::Knowledge_Engine::Containers::Flex_Map::set (
  const std::string & value)
{
  int result = -1;

  if (context_ && name_ != "")
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    if (!variable_.is_valid ())
    {
      update_variable ();
    }

    result = context_->set (variable_, value, settings_);
  }

  return result;
}

int Madara::Knowledge_Engine::Containers::Flex_Map::set (
  const std::string & value, 
  const Knowledge_Update_Settings & settings)
{
  int result = -1;

  if (context_ && name_ != "")
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    if (!variable_.is_valid ())
    {
      update_variable ();
    }

    result = context_->set (variable_, value, settings);
  }

  return result;
}

int Madara::Knowledge_Engine::Containers::Flex_Map::set_file (
  const unsigned char * value, size_t size)
{
  int result = -1;

  if (context_ && name_ != "")
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    if (!variable_.is_valid ())
    {
      update_variable ();
    }

    result = context_->set_file (variable_, value, size, settings_);
  }

  return result;
}

int Madara::Knowledge_Engine::Containers::Flex_Map::set_file (
  const unsigned char * value, size_t size, 
  const Knowledge_Update_Settings & settings)
{
  int result = -1;

  if (context_ && name_ != "")
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    if (!variable_.is_valid ())
    {
      update_variable ();
    }

    result = context_->set_file (variable_, value, size, settings);
  }

  return result;
}

int Madara::Knowledge_Engine::Containers::Flex_Map::set_jpeg (
  const unsigned char * value, size_t size)
{
  int result = -1;

  if (context_ && name_ != "")
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    if (!variable_.is_valid ())
    {
      update_variable ();
    }

    result = context_->set_jpeg (variable_, value, size, settings_);
  }

  return result;
}

int Madara::Knowledge_Engine::Containers::Flex_Map::set_jpeg (
  const unsigned char * value, size_t size, 
  const Knowledge_Update_Settings & settings)
{
  int result = -1;

  if (context_ && name_ != "")
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    if (!variable_.is_valid ())
    {
      update_variable ();
    }

    result = context_->set_jpeg (variable_, value, size, settings);
  }

  return result;
}

void
Madara::Knowledge_Engine::Containers::Flex_Map::set_quality (
  uint32_t quality,
  const Knowledge_Reference_Settings & settings)
{
  if (context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    if (!variable_.is_valid ())
    {
      update_variable ();
    }

    context_->set_quality (name_, quality, true, settings);
  }
}
