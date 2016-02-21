#include "FlexMap.h"
#include "madara/knowledge/ContextGuard.h"

#include <algorithm>

madara::knowledge::containers::FlexMap::FlexMap (
  const KnowledgeUpdateSettings & settings,
  const std::string & delimiter)
  : BaseContainer ("", settings), context_ (0), delimiter_ (".")
{
}
  

madara::knowledge::containers::FlexMap::FlexMap (
  const std::string & name,
  KnowledgeBase & knowledge,
  const KnowledgeUpdateSettings & settings,
  const std::string & delimiter)
  : BaseContainer (name, settings), context_ (&(knowledge.get_context ())),
  delimiter_ (".")
{
}
 
madara::knowledge::containers::FlexMap::FlexMap (
  const std::string & name,
  Variables & knowledge,
  const KnowledgeUpdateSettings & settings,
  const std::string & delimiter)
  : BaseContainer (name, settings), context_ (knowledge.get_context ()),
  delimiter_ (".")
{
}
       

madara::knowledge::containers::FlexMap::FlexMap (const FlexMap & rhs)
  : BaseContainer (rhs), context_ (rhs.context_),
  variable_ (rhs.variable_),
  delimiter_ (rhs.delimiter_)
{

}

madara::knowledge::containers::FlexMap::~FlexMap ()
{

}

/**
* Checks if the value in the record is not false (0)
* @return  true if value is not false
**/
bool madara::knowledge::containers::FlexMap::is_true (void) const
{
  ContextGuard context_guard (*context_);
  bool result (false);

  KnowledgeUpdateSettings keep_local (true);

  madara_logger_log (context_->get_logger (), logger::LOG_MAJOR,
    "Vector::is_true: checking for non-zero value\n");

  if (!variable_.is_valid ())
  {
    result = context_->get (variable_, keep_local).is_true ();
  }

  madara_logger_log (context_->get_logger (), logger::LOG_MAJOR,
    "FlexMap::is_true: final result is %d\n", (int)result);

  return result;
}

/**
* Checks if the value in the record is false (0)
* @return  true if value is false
**/
bool madara::knowledge::containers::FlexMap::is_false (void) const
{
  ContextGuard context_guard (*context_);
  bool result (true);

  KnowledgeUpdateSettings keep_local (true);

  if (!variable_.is_valid ())
  {
    result = context_->get (variable_, keep_local).is_true ();
  }

  return result;
}

void
madara::knowledge::containers::FlexMap::modify (void)
{
  if (context_ && name_ != "")
  {
    ContextGuard context_guard (*context_);

    // get all children
    KnowledgeBase knowledge;
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
madara::knowledge::containers::FlexMap::get_debug_info (void)
{
  std::stringstream result;

  result << "Flex Map: ";

  if (context_ && name_ != "")
  {
    ContextGuard context_guard (*context_);
    MADARA_GUARD_TYPE guard (mutex_);

    // get all children
    KnowledgeBase knowledge;
    knowledge.facade_for (*context_);
    Map map (name_, knowledge, settings_, delimiter_);

    result << map.get_debug_info ();
  }

  return result.str ();
}


void
madara::knowledge::containers::FlexMap::modify_ (void)
{
  modify ();
}

std::string
madara::knowledge::containers::FlexMap::get_debug_info_ (void)
{
  return get_debug_info ();
}

madara::knowledge::containers::BaseContainer *
madara::knowledge::containers::FlexMap::clone (void) const
{
  return new FlexMap (*this);
}

void
madara::knowledge::containers::FlexMap::operator= (
  const FlexMap & rhs)
{
  if (this != &rhs)
  {
    MADARA_GUARD_TYPE guard (mutex_), guard2 (rhs.mutex_);

    this->context_ = rhs.context_;
    this->name_ = rhs.name_;
    this->settings_ = rhs.settings_;
    this->variable_ = rhs.variable_;
  }
}


void
madara::knowledge::containers::FlexMap::operator= (
  knowledge::KnowledgeRecord::Integer value)
{
  set (value, settings_);
}

void
madara::knowledge::containers::FlexMap::operator= (
int value)
{
  set (KnowledgeRecord::Integer (value), settings_);
}

void
madara::knowledge::containers::FlexMap::operator= (
  double value)
{
  set (value, settings_);
}

void
madara::knowledge::containers::FlexMap::operator= (
  const std::string value)
{
  set (value, settings_);
}

void
madara::knowledge::containers::FlexMap::operator= (
  const std::vector <KnowledgeRecord::Integer> & value)
{
  set (value, settings_);
}

void
madara::knowledge::containers::FlexMap::operator= (
const std::vector <double> & value)
{
  set (value, settings_);
}


madara::knowledge::KnowledgeRecord
madara::knowledge::containers::FlexMap::to_record (void) const
{
  knowledge::KnowledgeRecord result;

  if (context_)
  {
    ContextGuard context_guard (*context_);
    MADARA_GUARD_TYPE guard (mutex_);

    KnowledgeUpdateSettings keep_local (true);

    if (!variable_.is_valid ())
    {
      this->update_variable ();
    }

    result = context_->get (variable_, settings_);
  }

  return result;
}

madara::knowledge::KnowledgeRecord::Integer
madara::knowledge::containers::FlexMap::to_integer (void) const
{
  return to_record ().to_integer ();
}

double
madara::knowledge::containers::FlexMap::to_double (void) const
{
  return to_record ().to_double ();
}

std::string
madara::knowledge::containers::FlexMap::to_string (void) const
{
  return to_record ().to_string ();
}


void
madara::knowledge::containers::FlexMap::to_container (
  BufferVector & target) const
{
  if (context_)
  {
    ContextGuard context_guard (*context_);
    MADARA_GUARD_TYPE guard (mutex_);

    // get all children
    KnowledgeBase knowledge;
    knowledge.facade_for (*context_);

    target.set_delimiter (delimiter_);
    target.set_name (name_, knowledge);
  }
}


void
madara::knowledge::containers::FlexMap::to_container (
  DoubleVector & target) const
{
  if (context_)
  {
    ContextGuard context_guard (*context_);
    MADARA_GUARD_TYPE guard (mutex_);

    // get all children
    KnowledgeBase knowledge;
    knowledge.facade_for (*context_);

    target.set_delimiter (delimiter_);
    target.set_name (name_, knowledge);
  }
}


void
madara::knowledge::containers::FlexMap::to_container (
  IntegerVector & target) const
{
  if (context_)
  {
    ContextGuard context_guard (*context_);
    MADARA_GUARD_TYPE guard (mutex_);

    // get all children
    KnowledgeBase knowledge;
    knowledge.facade_for (*context_);

    target.set_delimiter (delimiter_);
    target.set_name (name_, knowledge);
  }
}

void
madara::knowledge::containers::FlexMap::to_container (
  NativeIntegerVector & target) const
{
  if (context_)
  {
    ContextGuard context_guard (*context_);
    MADARA_GUARD_TYPE guard (mutex_);

    // get all children
    KnowledgeBase knowledge;
    knowledge.facade_for (*context_);

    target.set_name (name_, knowledge);
  }
}


void
madara::knowledge::containers::FlexMap::to_container (
  StringVector & target) const
{
  if (context_)
  {
    ContextGuard context_guard (*context_);
    MADARA_GUARD_TYPE guard (mutex_);

    // get all children
    KnowledgeBase knowledge;
    knowledge.facade_for (*context_);

    target.set_delimiter (delimiter_);
    target.set_name (name_, knowledge);
  }
}


void
madara::knowledge::containers::FlexMap::to_container (
  Double & target) const
{
  if (context_)
  {
    ContextGuard context_guard (*context_);
    MADARA_GUARD_TYPE guard (mutex_);

    // get all children
    KnowledgeBase knowledge;
    knowledge.facade_for (*context_);

    target.set_name (name_, knowledge);
  }
}

void
madara::knowledge::containers::FlexMap::to_container (
  NativeDoubleVector & target) const
{
  if (context_)
  {
    ContextGuard context_guard (*context_);
    MADARA_GUARD_TYPE guard (mutex_);

    // get all children
    KnowledgeBase knowledge;
    knowledge.facade_for (*context_);

    target.set_name (name_, knowledge);
  }
}


void
madara::knowledge::containers::FlexMap::to_container (
  Integer & target) const
{
  if (context_)
  {
    ContextGuard context_guard (*context_);
    MADARA_GUARD_TYPE guard (mutex_);

    // get all children
    KnowledgeBase knowledge;
    knowledge.facade_for (*context_);

    target.set_name (name_, knowledge);
  }
}


void
madara::knowledge::containers::FlexMap::to_container (
  String & target) const
{
  if (context_)
  {
    ContextGuard context_guard (*context_);
    MADARA_GUARD_TYPE guard (mutex_);

    // get all children
    KnowledgeBase knowledge;
    knowledge.facade_for (*context_);

    target.set_name (name_, knowledge);
  }
}


void
madara::knowledge::containers::FlexMap::to_container (
  Map & target) const
{
  if (context_)
  {
    ContextGuard context_guard (*context_);
    MADARA_GUARD_TYPE guard (mutex_);

    // get all children
    KnowledgeBase knowledge;
    knowledge.facade_for (*context_);

    target.set_delimiter (delimiter_);
    target.set_name (name_, knowledge);
  }
}


madara::knowledge::containers::FlexMap
madara::knowledge::containers::FlexMap::operator[] (
  const std::string & key)
{
  FlexMap map (settings_, delimiter_);

  if (key != "" && context_)
  {
    ContextGuard context_guard (*context_);
    MADARA_GUARD_TYPE guard (mutex_);

    KnowledgeBase knowledge;
    knowledge.facade_for (*context_);
    map.set_name (name_ + delimiter_ + key, knowledge);
  }

  return map;
}

madara::knowledge::containers::FlexMap
madara::knowledge::containers::FlexMap::operator[] (
  size_t index)
{
  FlexMap map (settings_, delimiter_);

  if (context_)
  {
    ContextGuard context_guard (*context_);
    MADARA_GUARD_TYPE guard (mutex_);

    std::stringstream buffer;
    KnowledgeBase knowledge;
    knowledge.facade_for (*context_);

    buffer << name_ << delimiter_ << index;

    map.set_name (buffer.str (), knowledge);
  }

  return map;
}

size_t
madara::knowledge::containers::FlexMap::size (
  bool first_level_keys_only) const
{
  ContextGuard context_guard (*context_);
  MADARA_GUARD_TYPE guard (mutex_);

  size_t result (0);

  if (!first_level_keys_only)
  {
    // get all children
    KnowledgeBase knowledge;
    knowledge.facade_for (*context_);
    Map map (name_, knowledge, settings_, delimiter_);

    result = map.size ();
  }
  else
  {
    // use the context's specialized mapping feature
    std::vector<std::string> next_keys;
    std::map<std::string, madara::knowledge::KnowledgeRecord> all_record_vars;
    context_->to_map (name_, delimiter_, "",
      next_keys, all_record_vars, true);

    result = next_keys.size ();
  }

  return result;
}

void
madara::knowledge::containers::FlexMap::exchange (
  FlexMap & other, bool refresh_keys, bool delete_keys)
{
  if (context_ && other.context_)
  {
    ContextGuard context_guard (*context_);
    ContextGuard other_context_guard (*other.context_);
    MADARA_GUARD_TYPE guard (mutex_), guard2 (other.mutex_);

    // use the Map container, which already has exchange implemented
    KnowledgeBase knowledge;
    knowledge.facade_for (*context_);
    Map this_map (name_, knowledge, settings_, delimiter_);
    Map other_map (other.name_, knowledge, other.settings_, delimiter_);

    this_map.exchange (other_map, refresh_keys, delete_keys);
  }
}

void
madara::knowledge::containers::FlexMap::clear (void)
{
  if (context_)
  {
    ContextGuard context_guard (*context_);
    MADARA_GUARD_TYPE guard (mutex_);

    // use the Map container, which already has clear implemented
    KnowledgeBase knowledge;
    knowledge.facade_for (*context_);
    Map map (name_, knowledge, settings_, delimiter_);

    map.clear ();

    context_->delete_variable (name_, settings_);
  }
}

void
madara::knowledge::containers::FlexMap::erase (
  const std::string & key, bool delete_subkeys)
{
  if (context_)
  {
    ContextGuard context_guard (*context_);
    MADARA_GUARD_TYPE guard (mutex_);

    context_->delete_variable (name_ + delimiter_ + key, settings_);
    
    if (delete_subkeys)
    {
      // use the Map container, which already has clear implemented
      KnowledgeBase knowledge;
      knowledge.facade_for (*context_);
      Map map (name_ + delimiter_ + key, knowledge, settings_, delimiter_);

      map.clear ();
    }
  }
}

void
madara::knowledge::containers::FlexMap::set_name (
  const std::string & var_name,
  KnowledgeBase & knowledge)
{
  if (context_ != &(knowledge.get_context ()) || name_ != var_name)
  {
    context_ = &(knowledge.get_context ());

    ContextGuard context_guard (*context_);
    MADARA_GUARD_TYPE guard (mutex_);

    name_ = var_name;

    if (context_->exists (var_name, settings_))
    {
      KnowledgeUpdateSettings keep_local (true);

      variable_ = context_->get_ref (var_name, keep_local);
    }
    else
    {
      // reset variable reference
      variable_ = VariableReference ();
    }
  }
}

void
madara::knowledge::containers::FlexMap::set_name (
  const std::string & var_name,
  Variables & knowledge)
{
  if (context_ != knowledge.get_context () || name_ != var_name)
  {
    context_ = knowledge.get_context ();

    ContextGuard context_guard (*context_);
    MADARA_GUARD_TYPE guard (mutex_);

    name_ = var_name;

    if (context_->exists (var_name, settings_))
    {
      update_variable ();
    }
    else
    {
      // reset variable reference
      variable_ = VariableReference ();
    }
  }
}

void
madara::knowledge::containers::FlexMap::update_variable (void) const
{
  if (context_ && name_ != "")
  {
    ContextGuard context_guard (*context_);
    MADARA_GUARD_TYPE guard (mutex_);

    KnowledgeUpdateSettings keep_local (true);

    variable_ = context_->get_ref (name_, keep_local);
  }
}

void
madara::knowledge::containers::FlexMap::set_delimiter (
const std::string & delimiter)
{
  delimiter_ = delimiter;
}


std::string
madara::knowledge::containers::FlexMap::get_delimiter (void)
{
  return delimiter_;
}

bool
madara::knowledge::containers::FlexMap::exists (
  const std::string & key, bool first_level_key) const
{
  bool result (false);

  if (context_)
  {
    ContextGuard context_guard (*context_);
    MADARA_GUARD_TYPE guard (mutex_);

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
madara::knowledge::containers::FlexMap::keys (
  std::vector <std::string> & curkeys,
  bool first_level_keys_only) const
{
  ContextGuard context_guard (*context_);
  MADARA_GUARD_TYPE guard (mutex_);

  if (!first_level_keys_only)
  {
    // use the Map container, which already has clear implemented
    KnowledgeBase knowledge;
    knowledge.facade_for (*context_);
    Map map (name_, knowledge, settings_, delimiter_);

    map.keys (curkeys);
  }
  else
  {
    // use the context's specialized mapping feature
    std::vector<std::string> next_keys;
    std::map<std::string, madara::knowledge::KnowledgeRecord> all_record_vars;
    context_->to_map (name_, delimiter_, "",
      curkeys, all_record_vars, true);
  }
}

int
madara::knowledge::containers::FlexMap::read_file (
  const std::string & filename)
{
  int result = -1;

  if (context_ && name_ != "")
  {
    ContextGuard context_guard (*context_);
    MADARA_GUARD_TYPE guard (mutex_);

    if (!variable_.is_valid ())
    {
      update_variable ();
    }

    result = context_->read_file (variable_, filename, settings_);
  }
  return result;
}
 
int
madara::knowledge::containers::FlexMap::read_file (
  
  const std::string & filename, 
  const KnowledgeUpdateSettings & settings)
{
  int result = -1;

  if (context_ && name_ != "")
  {
    ContextGuard context_guard (*context_);
    MADARA_GUARD_TYPE guard (mutex_);

    if (!variable_.is_valid ())
    {
      update_variable ();
    }

    result = context_->read_file (variable_, filename, settings);
  }
  return result;
}
      
int madara::knowledge::containers::FlexMap::set (
  madara::knowledge::KnowledgeRecord::Integer value)
{
  int result = -1;

  if (context_ && name_ != "")
  {
    ContextGuard context_guard (*context_);
    MADARA_GUARD_TYPE guard (mutex_);

    if (!variable_.is_valid ())
    {
      update_variable ();
    }

    result = context_->set (variable_, value, settings_);
  }

  return result;
}

int madara::knowledge::containers::FlexMap::set (
  madara::knowledge::KnowledgeRecord::Integer value, 
  const KnowledgeUpdateSettings & settings)
{
  int result = -1;

  if (context_ && name_ != "")
  {
    ContextGuard context_guard (*context_);
    MADARA_GUARD_TYPE guard (mutex_);

    if (!variable_.is_valid ())
    {
      update_variable ();
    }

    result = context_->set (variable_, value, settings);
  }

  return result;
}

int madara::knowledge::containers::FlexMap::set_index (
  
  size_t index,
  madara::knowledge::KnowledgeRecord::Integer value)
{
  int result = -1;

  if (context_ && name_ != "")
  {
    ContextGuard context_guard (*context_);
    MADARA_GUARD_TYPE guard (mutex_);

    if (!variable_.is_valid ())
    {
      update_variable ();
    }

    result = context_->set_index (variable_, index, value, settings_);
  }

  return result;
}

int madara::knowledge::containers::FlexMap::set_index (
  
  size_t index,
  madara::knowledge::KnowledgeRecord::Integer value,
  const KnowledgeUpdateSettings & settings)
{
  int result = -1;

  if (context_ && name_ != "")
  {
    ContextGuard context_guard (*context_);
    MADARA_GUARD_TYPE guard (mutex_);

    if (!variable_.is_valid ())
    {
      update_variable ();
    }

    result = context_->set_index (variable_, index, value, settings);
  }

  return result;
}


int madara::knowledge::containers::FlexMap::set (
  const madara::knowledge::KnowledgeRecord::Integer * value,
  uint32_t size)
{
  int result = -1;

  if (context_ && name_ != "")
  {
    ContextGuard context_guard (*context_);
    MADARA_GUARD_TYPE guard (mutex_);

    if (!variable_.is_valid ())
    {
      update_variable ();
    }

    result = context_->set (variable_, value, size, settings_);
  }

  return result;
}
 
int madara::knowledge::containers::FlexMap::set (
  const madara::knowledge::KnowledgeRecord::Integer * value,
  uint32_t size,
  const KnowledgeUpdateSettings & settings)
{
  int result = -1;

  if (context_ && name_ != "")
  {
    ContextGuard context_guard (*context_);
    MADARA_GUARD_TYPE guard (mutex_);

    if (!variable_.is_valid ())
    {
      update_variable ();
    }

    result = context_->set (variable_, value, size, settings);
  }

  return result;
}
       
int madara::knowledge::containers::FlexMap::set (
  const std::vector <KnowledgeRecord::Integer> & value)
{
  int result = -1;

  if (context_ && name_ != "")
  {
    ContextGuard context_guard (*context_);
    MADARA_GUARD_TYPE guard (mutex_);

    if (!variable_.is_valid ())
    {
      update_variable ();
    }

    result = context_->set (variable_, value, settings_);
  }

  return result;
}

int madara::knowledge::containers::FlexMap::set (
  const std::vector <KnowledgeRecord::Integer> & value,
  const KnowledgeUpdateSettings & settings)
{
  int result = -1;

  if (context_ && name_ != "")
  {
    ContextGuard context_guard (*context_);
    MADARA_GUARD_TYPE guard (mutex_);

    if (!variable_.is_valid ())
    {
      update_variable ();
    }

    result = context_->set (variable_, value, settings);
  }

  return result;
}
          
int madara::knowledge::containers::FlexMap::set (
  double value)
{
  int result = -1;

  if (context_ && name_ != "")
  {
    ContextGuard context_guard (*context_);
    MADARA_GUARD_TYPE guard (mutex_);

    if (!variable_.is_valid ())
    {
      update_variable ();
    }

    result = context_->set (variable_, value, settings_);
  }

  return result;
}
    
int madara::knowledge::containers::FlexMap::set (
  double value, 
  const KnowledgeUpdateSettings & settings)
{
  int result = -1;

  if (context_ && name_ != "")
  {
    ContextGuard context_guard (*context_);
    MADARA_GUARD_TYPE guard (mutex_);

    if (!variable_.is_valid ())
    {
      update_variable ();
    }

    result = context_->set (variable_, value, settings);
  }

  return result;
}

int madara::knowledge::containers::FlexMap::set_index (
  size_t index,
  double value)
{
  int result = -1;

  if (context_ && name_ != "")
  {
    ContextGuard context_guard (*context_);
    MADARA_GUARD_TYPE guard (mutex_);

    if (!variable_.is_valid ())
    {
      update_variable ();
    }

    result = context_->set_index (variable_, index, value, settings_);
  }

  return result;
}

int madara::knowledge::containers::FlexMap::set_index (
  size_t index,
  double value,
  const KnowledgeUpdateSettings & settings)
{
  int result = -1;

  if (context_ && name_ != "")
  {
    ContextGuard context_guard (*context_);
    MADARA_GUARD_TYPE guard (mutex_);

    if (!variable_.is_valid ())
    {
      update_variable ();
    }

    result = context_->set_index (variable_, index, value, settings);
  }

  return result;
}

int madara::knowledge::containers::FlexMap::set (
  const double * value,
  uint32_t size)
{
  int result = -1;

  if (context_ && name_ != "")
  {
    ContextGuard context_guard (*context_);
    MADARA_GUARD_TYPE guard (mutex_);

    if (!variable_.is_valid ())
    {
      update_variable ();
    }

    result = context_->set (variable_, value, size, settings_);
  }

  return result;
}

int madara::knowledge::containers::FlexMap::set (
  const double * value,
  uint32_t size,
  const KnowledgeUpdateSettings & settings)
{
  int result = -1;

  if (context_ && name_ != "")
  {
    ContextGuard context_guard (*context_);
    MADARA_GUARD_TYPE guard (mutex_);

    if (!variable_.is_valid ())
    {
      update_variable ();
    }

    result = context_->set (variable_, value, size, settings);
  }

  return result;
}
 
int madara::knowledge::containers::FlexMap::set (
  const std::vector <double> & value)
{
  int result = -1;

  if (context_ && name_ != "")
  {
    ContextGuard context_guard (*context_);
    MADARA_GUARD_TYPE guard (mutex_);

    if (!variable_.is_valid ())
    {
      update_variable ();
    }

    result = context_->set (variable_, value, settings_);
  }

  return result;
}
 
int madara::knowledge::containers::FlexMap::set (
  const std::vector <double> & value,
  const KnowledgeUpdateSettings & settings)
{
  int result = -1;

  if (context_ && name_ != "")
  {
    ContextGuard context_guard (*context_);
    MADARA_GUARD_TYPE guard (mutex_);

    if (!variable_.is_valid ())
    {
      update_variable ();
    }

    result = context_->set (variable_, value, settings);
  }

  return result;
}

int madara::knowledge::containers::FlexMap::set (
  const std::string & value)
{
  int result = -1;

  if (context_ && name_ != "")
  {
    ContextGuard context_guard (*context_);
    MADARA_GUARD_TYPE guard (mutex_);

    if (!variable_.is_valid ())
    {
      update_variable ();
    }

    result = context_->set (variable_, value, settings_);
  }

  return result;
}

int madara::knowledge::containers::FlexMap::set (
  const std::string & value, 
  const KnowledgeUpdateSettings & settings)
{
  int result = -1;

  if (context_ && name_ != "")
  {
    ContextGuard context_guard (*context_);
    MADARA_GUARD_TYPE guard (mutex_);

    if (!variable_.is_valid ())
    {
      update_variable ();
    }

    result = context_->set (variable_, value, settings);
  }

  return result;
}

int madara::knowledge::containers::FlexMap::set_file (
  const unsigned char * value, size_t size)
{
  int result = -1;

  if (context_ && name_ != "")
  {
    ContextGuard context_guard (*context_);
    MADARA_GUARD_TYPE guard (mutex_);

    if (!variable_.is_valid ())
    {
      update_variable ();
    }

    result = context_->set_file (variable_, value, size, settings_);
  }

  return result;
}

int madara::knowledge::containers::FlexMap::set_file (
  const unsigned char * value, size_t size, 
  const KnowledgeUpdateSettings & settings)
{
  int result = -1;

  if (context_ && name_ != "")
  {
    ContextGuard context_guard (*context_);
    MADARA_GUARD_TYPE guard (mutex_);

    if (!variable_.is_valid ())
    {
      update_variable ();
    }

    result = context_->set_file (variable_, value, size, settings);
  }

  return result;
}

int madara::knowledge::containers::FlexMap::set_jpeg (
  const unsigned char * value, size_t size)
{
  int result = -1;

  if (context_ && name_ != "")
  {
    ContextGuard context_guard (*context_);
    MADARA_GUARD_TYPE guard (mutex_);

    if (!variable_.is_valid ())
    {
      update_variable ();
    }

    result = context_->set_jpeg (variable_, value, size, settings_);
  }

  return result;
}

int madara::knowledge::containers::FlexMap::set_jpeg (
  const unsigned char * value, size_t size, 
  const KnowledgeUpdateSettings & settings)
{
  int result = -1;

  if (context_ && name_ != "")
  {
    ContextGuard context_guard (*context_);
    MADARA_GUARD_TYPE guard (mutex_);

    if (!variable_.is_valid ())
    {
      update_variable ();
    }

    result = context_->set_jpeg (variable_, value, size, settings);
  }

  return result;
}

void
madara::knowledge::containers::FlexMap::set_quality (
  uint32_t quality,
  const KnowledgeReferenceSettings & settings)
{
  if (context_)
  {
    ContextGuard context_guard (*context_);
    MADARA_GUARD_TYPE guard (mutex_);

    if (!variable_.is_valid ())
    {
      update_variable ();
    }

    context_->set_quality (name_, quality, true, settings);
  }
}

bool
madara::knowledge::containers::FlexMap::is_true_ (void) const
{
  return is_true ();
}

bool
madara::knowledge::containers::FlexMap::is_false_ (void) const
{
  return is_false ();
}
