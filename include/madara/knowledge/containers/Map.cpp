#include "Map.h"
#include "madara/knowledge/ContextGuard.h"

#include <algorithm>

madara::knowledge::containers::Map::Map(
    const KnowledgeUpdateSettings& settings, const std::string& delimiter)
  : BaseContainer("", settings), context_(0), delimiter_(delimiter)
{
}

madara::knowledge::containers::Map::Map(const std::string& name,
    KnowledgeBase& knowledge, const KnowledgeUpdateSettings& settings,
    const std::string& delimiter)
  : BaseContainer(name, settings),
    context_(&(knowledge.get_context())),
    delimiter_(delimiter)
{
  std::map<std::string, knowledge::KnowledgeRecord> contents;
  std::string common = name + delimiter_;
  context_->to_map(common, contents);

  KnowledgeUpdateSettings keep_local(true);
  if (contents.size() > 0)
  {
    for (std::map<std::string, knowledge::KnowledgeRecord>::iterator i =
             contents.begin();
         i != contents.end(); ++i)
    {
      map_[i->first.substr(common.size())] =
          knowledge.get_ref(i->first, keep_local);
    }
  }
}

madara::knowledge::containers::Map::Map(const std::string& name,
    Variables& knowledge, const KnowledgeUpdateSettings& settings,
    const std::string& delimiter)
  : BaseContainer(name, settings),
    context_(knowledge.get_context()),
    delimiter_(delimiter)
{
  std::map<std::string, knowledge::KnowledgeRecord> contents;
  std::string common = name + delimiter_;
  context_->to_map(common, contents);

  KnowledgeUpdateSettings keep_local(true);
  if (contents.size() > 0)
  {
    for (std::map<std::string, knowledge::KnowledgeRecord>::iterator i =
             contents.begin();
         i != contents.end(); ++i)
    {
      map_[i->first.substr(common.size())] =
          knowledge.get_ref(i->first, keep_local);
    }
  }
}

madara::knowledge::containers::Map::Map(const Map& rhs)
  : BaseContainer(rhs),
    context_(rhs.context_),
    map_(rhs.map_),
    delimiter_(rhs.delimiter_)
{
}

madara::knowledge::containers::Map::~Map() {}

void madara::knowledge::containers::Map::modify(void)
{
  if (context_ && name_ != "")
  {
    ContextGuard context_guard(*context_);
    MADARA_GUARD_TYPE guard(mutex_);

    for (InternalMap::const_iterator index = map_.begin(); index != map_.end();
         ++index)
    {
      context_->mark_modified(index->second);
    }
  }
}

std::string madara::knowledge::containers::Map::get_debug_info(void)
{
  std::stringstream result;

  result << "Map: ";

  if (context_)
  {
    ContextGuard context_guard(*context_);
    MADARA_GUARD_TYPE guard(mutex_);

    result << this->name_;
    result << " [" << map_.size() << "]";
    result << " = [";

    bool first(true);

    for (InternalMap::const_iterator index = map_.begin(); index != map_.end();
         ++index)
    {
      if (!first)
      {
        result << ", ";
      }
      else
      {
        first = false;
      }

      result << index->first << " => "
             << context_->get(index->second).to_string();
    }

    result << "]";
  }

  return result.str();
}

void madara::knowledge::containers::Map::modify_(void)
{
  modify();
}

std::string madara::knowledge::containers::Map::get_debug_info_(void)
{
  return get_debug_info();
}

madara::knowledge::containers::BaseContainer*
madara::knowledge::containers::Map::clone(void) const
{
  return new Map(*this);
}

void madara::knowledge::containers::Map::modify(const std::string& index)
{
  if (context_ && name_ != "" && map_.find(index) != map_.end())
  {
    ContextGuard context_guard(*context_);
    context_->mark_modified(map_[index]);
  }
}

void madara::knowledge::containers::Map::operator=(const Map& rhs)
{
  if (this != &rhs)
  {
    MADARA_GUARD_TYPE guard(mutex_), guard2(rhs.mutex_);

    this->context_ = rhs.context_;
    this->name_ = rhs.name_;
    this->settings_ = rhs.settings_;
    this->map_ = rhs.map_;
  }
}

madara::knowledge::KnowledgeRecord
madara::knowledge::containers::Map::to_record(const std::string& key)
{
  knowledge::KnowledgeRecord result;

  if (context_)
  {
    ContextGuard context_guard(*context_);
    MADARA_GUARD_TYPE guard(mutex_);
    std::stringstream buffer;
    buffer << name_;
    buffer << delimiter_;
    buffer << key;

    KnowledgeUpdateSettings keep_local(true);
    std::string final_key = buffer.str();
    std::map<std::string, VariableReference>::const_iterator entry =
        map_.find(final_key);

    if (entry == map_.end())
    {
      VariableReference ref = context_->get_ref(final_key, keep_local);
      map_[key] = ref;
      return context_->get(ref, keep_local);
    }

    result = context_->get(entry->second, keep_local);
  }

  return result;
}

madara::knowledge::KnowledgeRecord madara::knowledge::containers::Map::
operator[](const std::string& key)
{
  return to_record(key);
}

size_t madara::knowledge::containers::Map::size(void) const
{
  MADARA_GUARD_TYPE guard(mutex_);
  return map_.size();
}

std::vector<std::string> madara::knowledge::containers::Map::sync_keys(void)
{
  std::vector<std::string> additions;

  if (context_)
  {
    ContextGuard context_guard(*context_);
    MADARA_GUARD_TYPE guard(mutex_);

    clear(false);

    std::map<std::string, knowledge::KnowledgeRecord> contents;
    std::string common = name_ + delimiter_;
    context_->to_map(common, contents);
    KnowledgeUpdateSettings keep_local(true);

    for (std::map<std::string, knowledge::KnowledgeRecord>::iterator i =
             contents.begin();
         i != contents.end(); ++i)
    {
      std::string key = i->first.substr(common.size());

      if (map_.find(key) == map_.end())
      {
        additions.push_back(key);
        map_[key] = context_->get_ref(i->first, keep_local);
      }
    }
  }

  return additions;
}

void madara::knowledge::containers::Map::exchange(
    Map& other, bool refresh_keys, bool delete_keys)
{
  if (context_ && other.context_)
  {
    std::lock(*context_, *other.context_, mutex_, other.mutex_);

    ContextGuard context_guard(*context_, std::adopt_lock);
    ContextGuard other_context_guard(*other.context_, std::adopt_lock);
    MADARA_GUARD_TYPE guard(mutex_, std::adopt_lock),
        guard2(other.mutex_, std::adopt_lock);

    if (refresh_keys)
    {
      other.sync_keys();
      this->sync_keys();
    }

    InternalMap leftover(other.map_);
    InternalMap::iterator i = map_.begin();
    while (i != map_.end())
    {
      // temp = this[i->first]
      knowledge::KnowledgeRecord temp =
          this->context_->get(i->second, this->settings_);

      // check if the other map has the key
      InternalMap::iterator other_found = other.map_.find(i->first);

      if (other_found != other.map_.end())
      {
        // this[i->first] = other[i->first]
        this->context_->set(i->second,
            other.context_->get(other_found->second, other.settings_),
            this->settings_);

        // other[i->first] = temp
        other.map_[i->first] = other_found->second;
        other.context_->set(other_found->second, temp, other.settings_);

        // remove item from the items leftover
        leftover.erase(i->first);
      }
      else
      {
        std::stringstream buffer;
        buffer << other.name_;
        buffer << delimiter_;
        buffer << i->first;

        // other[i->first] = temp
        VariableReference ref =
            other.context_->get_ref(buffer.str(), other.settings_);
        other.map_[i->first] = ref;
        other.context_->set(ref, temp, other.settings_);

        // this[i->first] = zero
        if (delete_keys)
        {
          std::stringstream buffer2;
          buffer2 << this->name_;
          buffer2 << delimiter_;
          buffer2 << i->first;

          this->context_->delete_variable(buffer2.str(), this->settings_);
          this->map_.erase(i++);
        }
        else
        {
          knowledge::KnowledgeRecord zero;
          this->context_->set(i->second, zero, this->settings_);
          ++i;
        }
      }
    }

    // iterate over the elements in the other.map_ that are not in the current
    // map

    for (i = leftover.begin(); i != leftover.end(); ++i)
    {
      if (map_.find(i->first) == map_.end())
      {
        std::stringstream buffer;
        buffer << name_;
        buffer << delimiter_;
        buffer << i->first;

        VariableReference ref = context_->get_ref(buffer.str(), settings_);
        this->map_[i->first] = ref;

        // this[i->first] = other[i->first]
        this->context_->set(ref,
            other.context_->get(i->second, other.settings_), this->settings_);

        // other[i->first] = zero;
        if (delete_keys)
        {
          std::stringstream buffer2;
          buffer2 << other.name_;
          buffer2 << delimiter_;
          buffer2 << i->first;

          other.context_->delete_variable(buffer2.str(), settings_);
          other.map_.erase(i->first);
        }
        else
        {
          knowledge::KnowledgeRecord zero;
          other.context_->set(i->second, zero, other.settings_);
        }
      }
    }
  }
}

void madara::knowledge::containers::Map::clear(bool clear_knowledge)
{
  if (context_ && clear_knowledge)
  {
    ContextGuard context_guard(*context_);
    MADARA_GUARD_TYPE guard(mutex_);

    std::vector<std::string> keys;
    this->keys(keys);

    for (size_t i = 0; i < keys.size(); ++i)
      this->erase(keys[i]);

    map_.clear();
  }

  else if (context_)
  {
    MADARA_GUARD_TYPE guard(mutex_);
    map_.clear();
  }
}

void madara::knowledge::containers::Map::reset(void)
{
  if (context_)
  {
    ContextGuard context_guard(*context_);
    MADARA_GUARD_TYPE guard(mutex_);

    for (auto entry : map_)
      context_->clear(entry.first);

    map_.clear();
  }
}

void madara::knowledge::containers::Map::erase(const std::string& key)
{
  if (context_)
  {
    ContextGuard context_guard(*context_);
    MADARA_GUARD_TYPE guard(mutex_);

    // find the key in the internal map
    InternalMap::iterator found = map_.find(key);

    // delete the variable if it has been found
    if (found != map_.end())
    {
      context_->delete_variable(found->second.get_name());
      map_.erase(found);
    }
  }
}

void madara::knowledge::containers::Map::set_name(
    const std::string& var_name, KnowledgeBase& knowledge, bool sync)
{
  if (context_ != &(knowledge.get_context()) || name_ != var_name)
  {
    context_ = &(knowledge.get_context());

    ContextGuard context_guard(*context_);
    MADARA_GUARD_TYPE guard(mutex_);

    name_ = var_name;

    // the old map will no longer be appropriate
    clear(false);
    if (sync)
      sync_keys();
  }
}

void madara::knowledge::containers::Map::set_name(
    const std::string& var_name, Variables& knowledge, bool sync)
{
  if (context_ != knowledge.get_context() || name_ != var_name)
  {
    context_ = knowledge.get_context();

    ContextGuard context_guard(*context_);
    MADARA_GUARD_TYPE guard(mutex_);

    name_ = var_name;

    // the old map will no longer be appropriate
    clear(false);
    if (sync)
      sync_keys();
  }
}

void madara::knowledge::containers::Map::set_delimiter(
    const std::string& delimiter, bool sync)
{
  delimiter_ = delimiter;

  if (context_)
  {
    // the old map will no longer be appropriate
    clear(false);
    if (sync)
      sync_keys();
  }
}

std::string madara::knowledge::containers::Map::get_delimiter(void)
{
  return delimiter_;
}

bool madara::knowledge::containers::Map::exists(const std::string& key) const
{
  bool result(false);

  InternalMap::const_iterator found = map_.find(key);

  if (context_ && found != map_.end())
  {
    ContextGuard context_guard(*context_);
    MADARA_GUARD_TYPE guard(mutex_);

    result = context_->exists(found->second);
  }

  return result;
}

bool madara::knowledge::containers::Map::has_prefix(
    const std::string& prefix) const
{
  bool result(false);

  InternalMap::const_iterator found = map_.lower_bound(prefix);

  if (found != map_.end())
  {
    if (madara::utility::begins_with(found->first, prefix))
      result = true;
  }

  return result;
}

void madara::knowledge::containers::Map::keys(
    std::vector<std::string>& curkeys) const
{
  MADARA_GUARD_TYPE guard(mutex_);
  curkeys.resize(map_.size());
  unsigned int j = 0;

  for (std::map<std::string, VariableReference>::const_iterator
           i = map_.begin();
       i != map_.end(); ++i, ++j)
  {
    curkeys[j] = i->first;
  }
}

int madara::knowledge::containers::Map::read_file(
    const std::string& key, const std::string& filename)
{
  int result = -1;

  if (context_ && key != "")
  {
    ContextGuard context_guard(*context_);
    MADARA_GUARD_TYPE guard(mutex_);

    std::stringstream buffer;
    buffer << name_;
    buffer << delimiter_;
    buffer << key;

    std::string final_key = buffer.str();
    std::map<std::string, VariableReference>::iterator entry =
        map_.find(final_key);

    if (entry == map_.end())
    {
      VariableReference ref = context_->get_ref(final_key, settings_);
      map_[key] = ref;
      result = context_->read_file(ref, filename, settings_);
    }
    else
    {
      result = context_->read_file(entry->second, filename, settings_);
    }
  }
  return result;
}

int madara::knowledge::containers::Map::read_file(const std::string& key,
    const std::string& filename, const KnowledgeUpdateSettings& settings)
{
  int result = -1;

  if (context_ && key != "")
  {
    ContextGuard context_guard(*context_);
    MADARA_GUARD_TYPE guard(mutex_);

    std::stringstream buffer;
    buffer << name_;
    buffer << delimiter_;
    buffer << key;

    std::string final_key = buffer.str();
    std::map<std::string, VariableReference>::iterator entry =
        map_.find(final_key);

    if (entry == map_.end())
    {
      VariableReference ref = context_->get_ref(final_key, settings);
      map_[key] = ref;
      result = context_->read_file(ref, filename, settings);
    }
    else
    {
      result = context_->read_file(entry->second, filename, settings);
    }
  }
  return result;
}

int madara::knowledge::containers::Map::set(
    const std::string& key, madara::knowledge::KnowledgeRecord::Integer value)
{
  int result = -1;

  if (context_ && key != "")
  {
    ContextGuard context_guard(*context_);
    MADARA_GUARD_TYPE guard(mutex_);
    std::stringstream buffer;
    buffer << name_;
    buffer << delimiter_;
    buffer << key;

    std::string final_key = buffer.str();
    std::map<std::string, VariableReference>::iterator entry =
        map_.find(final_key);

    if (entry == map_.end())
    {
      VariableReference ref = context_->get_ref(final_key, settings_);
      map_[key] = ref;
      result = context_->set(ref, value, settings_);
    }
    else
    {
      result = context_->set(entry->second, value, settings_);
    }
  }

  return result;
}

int madara::knowledge::containers::Map::set(const std::string& key,
    madara::knowledge::KnowledgeRecord::Integer value,
    const KnowledgeUpdateSettings& settings)
{
  int result = -1;

  if (context_ && key != "")
  {
    ContextGuard context_guard(*context_);
    MADARA_GUARD_TYPE guard(mutex_);

    std::stringstream buffer;
    buffer << name_;
    buffer << delimiter_;
    buffer << key;

    std::string final_key = buffer.str();
    std::map<std::string, VariableReference>::iterator entry =
        map_.find(final_key);

    if (entry == map_.end())
    {
      VariableReference ref = context_->get_ref(final_key, settings);
      map_[key] = ref;
      result = context_->set(ref, value, settings);
    }
    else
    {
      result = context_->set(entry->second, value, settings);
    }
  }

  return result;
}

int madara::knowledge::containers::Map::set(const std::string& key,
    const KnowledgeRecord& value, const KnowledgeUpdateSettings& settings)
{
  int result = -1;

  if (context_ && key != "")
  {
    ContextGuard context_guard(*context_);
    MADARA_GUARD_TYPE guard(mutex_);

    std::stringstream buffer;
    buffer << name_;
    buffer << delimiter_;
    buffer << key;

    std::string final_key = buffer.str();
    std::map<std::string, VariableReference>::iterator entry =
        map_.find(final_key);

    if (entry == map_.end())
    {
      VariableReference ref = context_->get_ref(final_key, settings);
      map_[key] = ref;
      result = context_->set(ref, value, settings);
    }
    else
    {
      result = context_->set(entry->second, value, settings);
    }
  }

  return result;
}

int madara::knowledge::containers::Map::set_index(const std::string& key,
    size_t index, madara::knowledge::KnowledgeRecord::Integer value)
{
  int result = -1;

  if (context_ && key != "")
  {
    ContextGuard context_guard(*context_);
    MADARA_GUARD_TYPE guard(mutex_);

    std::stringstream buffer;
    buffer << name_;
    buffer << delimiter_;
    buffer << key;

    std::string final_key = buffer.str();
    std::map<std::string, VariableReference>::iterator entry =
        map_.find(final_key);

    if (entry == map_.end())
    {
      VariableReference ref = context_->get_ref(final_key, settings_);
      map_[key] = ref;
      result = context_->set_index(ref, index, value, settings_);
    }
    else
    {
      result = context_->set_index(entry->second, index, value, settings_);
    }
  }

  return result;
}

int madara::knowledge::containers::Map::set_index(const std::string& key,
    size_t index, madara::knowledge::KnowledgeRecord::Integer value,
    const KnowledgeUpdateSettings& settings)
{
  int result = -1;

  if (context_ && key != "")
  {
    ContextGuard context_guard(*context_);
    MADARA_GUARD_TYPE guard(mutex_);

    std::stringstream buffer;
    buffer << name_;
    buffer << delimiter_;
    buffer << key;

    std::string final_key = buffer.str();
    std::map<std::string, VariableReference>::iterator entry =
        map_.find(final_key);

    if (entry == map_.end())
    {
      VariableReference ref = context_->get_ref(final_key, settings);
      map_[key] = ref;
      result = context_->set_index(ref, index, value, settings);
    }
    else
    {
      result = context_->set_index(entry->second, index, value, settings);
    }
  }

  return result;
}

int madara::knowledge::containers::Map::set(const std::string& key,
    const madara::knowledge::KnowledgeRecord::Integer* value, uint32_t size)
{
  int result = -1;

  if (context_ && key != "")
  {
    ContextGuard context_guard(*context_);
    MADARA_GUARD_TYPE guard(mutex_);

    std::stringstream buffer;
    buffer << name_;
    buffer << delimiter_;
    buffer << key;

    std::string final_key = buffer.str();
    std::map<std::string, VariableReference>::iterator entry =
        map_.find(final_key);

    if (entry == map_.end())
    {
      VariableReference ref = context_->get_ref(final_key, settings_);
      map_[key] = ref;
      result = context_->set(ref, value, size, settings_);
    }
    else
    {
      result = context_->set(entry->second, value, size, settings_);
    }
  }

  return result;
}

int madara::knowledge::containers::Map::set(const std::string& key,
    const madara::knowledge::KnowledgeRecord::Integer* value, uint32_t size,
    const KnowledgeUpdateSettings& settings)
{
  int result = -1;

  if (context_ && key != "")
  {
    ContextGuard context_guard(*context_);
    MADARA_GUARD_TYPE guard(mutex_);

    std::stringstream buffer;
    buffer << name_;
    buffer << delimiter_;
    buffer << key;

    std::string final_key = buffer.str();
    std::map<std::string, VariableReference>::iterator entry =
        map_.find(final_key);

    if (entry == map_.end())
    {
      VariableReference ref = context_->get_ref(final_key, settings);
      map_[key] = ref;
      result = context_->set(ref, value, size, settings);
    }
    else
    {
      result = context_->set(entry->second, value, size, settings);
    }
  }

  return result;
}

int madara::knowledge::containers::Map::set(
    const std::string& key, const std::vector<KnowledgeRecord::Integer>& value)
{
  int result = -1;

  if (context_ && key != "")
  {
    ContextGuard context_guard(*context_);
    MADARA_GUARD_TYPE guard(mutex_);

    std::stringstream buffer;
    buffer << name_;
    buffer << delimiter_;
    buffer << key;

    std::string final_key = buffer.str();
    std::map<std::string, VariableReference>::iterator entry =
        map_.find(final_key);

    if (entry == map_.end())
    {
      VariableReference ref = context_->get_ref(final_key, settings_);
      map_[key] = ref;
      result = context_->set(ref, value, settings_);
    }
    else
    {
      result = context_->set(entry->second, value, settings_);
    }
  }

  return result;
}

int madara::knowledge::containers::Map::set(const std::string& key,
    const std::vector<KnowledgeRecord::Integer>& value,
    const KnowledgeUpdateSettings& settings)
{
  int result = -1;

  if (context_ && key != "")
  {
    ContextGuard context_guard(*context_);
    MADARA_GUARD_TYPE guard(mutex_);

    std::stringstream buffer;
    buffer << name_;
    buffer << delimiter_;
    buffer << key;

    std::string final_key = buffer.str();
    std::map<std::string, VariableReference>::iterator entry =
        map_.find(final_key);

    if (entry == map_.end())
    {
      VariableReference ref = context_->get_ref(final_key, settings);
      map_[key] = ref;
      return context_->set(ref, value, settings);
    }
    else
    {
      result = context_->set(entry->second, value, settings);
    }
  }

  return result;
}

int madara::knowledge::containers::Map::set(
    const std::string& key, double value)
{
  int result = -1;

  if (context_ && key != "")
  {
    ContextGuard context_guard(*context_);
    MADARA_GUARD_TYPE guard(mutex_);

    std::stringstream buffer;
    buffer << name_;
    buffer << delimiter_;
    buffer << key;

    std::string final_key = buffer.str();
    std::map<std::string, VariableReference>::iterator entry =
        map_.find(final_key);

    if (entry == map_.end())
    {
      VariableReference ref = context_->get_ref(final_key, settings_);
      map_[key] = ref;
      result = context_->set(ref, value, settings_);
    }
    else
    {
      result = context_->set(entry->second, value, settings_);
    }
  }

  return result;
}

int madara::knowledge::containers::Map::set(const std::string& key,
    double value, const KnowledgeUpdateSettings& settings)
{
  int result = -1;

  if (context_ && key != "")
  {
    ContextGuard context_guard(*context_);
    MADARA_GUARD_TYPE guard(mutex_);

    std::stringstream buffer;
    buffer << name_;
    buffer << delimiter_;
    buffer << key;

    std::string final_key = buffer.str();
    std::map<std::string, VariableReference>::iterator entry =
        map_.find(final_key);

    if (entry == map_.end())
    {
      VariableReference ref = context_->get_ref(final_key, settings);
      map_[key] = ref;
      result = context_->set(ref, value, settings);
    }
    else
    {
      result = context_->set(entry->second, value, settings);
    }
  }

  return result;
}

int madara::knowledge::containers::Map::set_index(
    const std::string& key, size_t index, double value)
{
  int result = -1;

  if (context_ && key != "")
  {
    ContextGuard context_guard(*context_);
    MADARA_GUARD_TYPE guard(mutex_);

    std::stringstream buffer;
    buffer << name_;
    buffer << delimiter_;
    buffer << key;

    std::string final_key = buffer.str();
    std::map<std::string, VariableReference>::iterator entry =
        map_.find(final_key);

    if (entry == map_.end())
    {
      VariableReference ref = context_->get_ref(final_key, settings_);
      map_[key] = ref;
      result = context_->set_index(ref, index, value, settings_);
    }
    else
    {
      result = context_->set_index(entry->second, index, value, settings_);
    }
  }

  return result;
}

int madara::knowledge::containers::Map::set_index(const std::string& key,
    size_t index, double value, const KnowledgeUpdateSettings& settings)
{
  int result = -1;

  if (context_ && key != "")
  {
    ContextGuard context_guard(*context_);
    MADARA_GUARD_TYPE guard(mutex_);

    std::stringstream buffer;
    buffer << name_;
    buffer << delimiter_;
    buffer << key;

    std::string final_key = buffer.str();
    std::map<std::string, VariableReference>::iterator entry =
        map_.find(final_key);

    if (entry == map_.end())
    {
      VariableReference ref = context_->get_ref(final_key, settings);
      map_[key] = ref;
      result = context_->set_index(ref, index, value, settings);
    }
    else
    {
      result = context_->set_index(entry->second, index, value, settings);
    }
  }

  return result;
}

int madara::knowledge::containers::Map::set(
    const std::string& key, const double* value, uint32_t size)
{
  int result = -1;

  if (context_ && key != "")
  {
    ContextGuard context_guard(*context_);
    MADARA_GUARD_TYPE guard(mutex_);

    std::stringstream buffer;
    buffer << name_;
    buffer << delimiter_;
    buffer << key;

    std::string final_key = buffer.str();
    std::map<std::string, VariableReference>::iterator entry =
        map_.find(final_key);

    if (entry == map_.end())
    {
      VariableReference ref = context_->get_ref(final_key, settings_);
      map_[key] = ref;
      result = context_->set(ref, value, size, settings_);
    }
    else
    {
      result = context_->set(entry->second, value, size, settings_);
    }
  }

  return result;
}

int madara::knowledge::containers::Map::set(const std::string& key,
    const double* value, uint32_t size, const KnowledgeUpdateSettings& settings)
{
  int result = -1;

  if (context_ && key != "")
  {
    ContextGuard context_guard(*context_);
    MADARA_GUARD_TYPE guard(mutex_);

    std::stringstream buffer;
    buffer << name_;
    buffer << delimiter_;
    buffer << key;

    std::string final_key = buffer.str();
    std::map<std::string, VariableReference>::iterator entry =
        map_.find(final_key);

    if (entry == map_.end())
    {
      VariableReference ref = context_->get_ref(final_key, settings);
      map_[key] = ref;
      result = context_->set(ref, value, size, settings);
    }

    result = context_->set(entry->second, value, size, settings);
  }

  return result;
}

int madara::knowledge::containers::Map::set(
    const std::string& key, const std::vector<double>& value)
{
  int result = -1;

  if (context_ && key != "")
  {
    ContextGuard context_guard(*context_);
    MADARA_GUARD_TYPE guard(mutex_);

    std::stringstream buffer;
    buffer << name_;
    buffer << delimiter_;
    buffer << key;

    std::string final_key = buffer.str();
    std::map<std::string, VariableReference>::iterator entry =
        map_.find(final_key);

    if (entry == map_.end())
    {
      VariableReference ref = context_->get_ref(final_key, settings_);
      map_[key] = ref;
      result = context_->set(ref, value, settings_);
    }
    else
    {
      result = context_->set(entry->second, value, settings_);
    }
  }

  return result;
}

int madara::knowledge::containers::Map::set(const std::string& key,
    const std::vector<double>& value, const KnowledgeUpdateSettings& settings)
{
  int result = -1;

  if (context_ && key != "")
  {
    ContextGuard context_guard(*context_);
    MADARA_GUARD_TYPE guard(mutex_);

    std::stringstream buffer;
    buffer << name_;
    buffer << delimiter_;
    buffer << key;

    std::string final_key = buffer.str();
    std::map<std::string, VariableReference>::iterator entry =
        map_.find(final_key);

    if (entry == map_.end())
    {
      VariableReference ref = context_->get_ref(final_key, settings);
      map_[key] = ref;
      result = context_->set(ref, value, settings);
    }
    else
    {
      result = context_->set(entry->second, value, settings);
    }
  }

  return result;
}

int madara::knowledge::containers::Map::set(
    const std::string& key, const std::string& value)
{
  int result = -1;

  if (context_ && key != "")
  {
    ContextGuard context_guard(*context_);
    MADARA_GUARD_TYPE guard(mutex_);

    std::stringstream buffer;
    buffer << name_;
    buffer << delimiter_;
    buffer << key;

    std::string final_key = buffer.str();
    std::map<std::string, VariableReference>::iterator entry =
        map_.find(final_key);

    if (entry == map_.end())
    {
      VariableReference ref = context_->get_ref(final_key, settings_);
      map_[key] = ref;
      result = context_->set(ref, value, settings_);
    }
    else
    {
      result = context_->set(entry->second, value, settings_);
    }
  }

  return result;
}

int madara::knowledge::containers::Map::set(const std::string& key,
    const std::string& value, const KnowledgeUpdateSettings& settings)
{
  int result = -1;

  if (context_ && key != "")
  {
    ContextGuard context_guard(*context_);
    MADARA_GUARD_TYPE guard(mutex_);

    std::stringstream buffer;
    buffer << name_;
    buffer << delimiter_;
    buffer << key;

    std::string final_key = buffer.str();
    std::map<std::string, VariableReference>::iterator entry =
        map_.find(final_key);

    if (entry == map_.end())
    {
      VariableReference ref = context_->get_ref(final_key, settings);
      map_[key] = ref;
      result = context_->set(ref, value, settings);
    }
    else
    {
      result = context_->set(entry->second, value, settings);
    }
  }

  return result;
}

int madara::knowledge::containers::Map::set_file(
    const std::string& key, const unsigned char* value, size_t size)
{
  int result = -1;

  if (context_ && key != "")
  {
    ContextGuard context_guard(*context_);
    MADARA_GUARD_TYPE guard(mutex_);

    std::stringstream buffer;
    buffer << name_;
    buffer << delimiter_;
    buffer << key;

    std::string final_key = buffer.str();
    std::map<std::string, VariableReference>::iterator entry =
        map_.find(final_key);

    if (entry == map_.end())
    {
      VariableReference ref = context_->get_ref(final_key, settings_);
      map_[key] = ref;
      result = context_->set_file(ref, value, size, settings_);
    }
    else
    {
      result = context_->set_file(entry->second, value, size, settings_);
    }
  }

  return result;
}

int madara::knowledge::containers::Map::set_file(const std::string& key,
    const unsigned char* value, size_t size,
    const KnowledgeUpdateSettings& settings)
{
  int result = -1;

  if (context_ && key != "")
  {
    ContextGuard context_guard(*context_);
    MADARA_GUARD_TYPE guard(mutex_);

    std::stringstream buffer;
    buffer << name_;
    buffer << delimiter_;
    buffer << key;

    std::string final_key = buffer.str();
    std::map<std::string, VariableReference>::iterator entry =
        map_.find(final_key);

    if (entry == map_.end())
    {
      VariableReference ref = context_->get_ref(final_key, settings);
      map_[key] = ref;
      result = context_->set_file(ref, value, size, settings);
    }

    result = context_->set_file(entry->second, value, size, settings);
  }

  return result;
}

int madara::knowledge::containers::Map::set_jpeg(
    const std::string& key, const unsigned char* value, size_t size)
{
  int result = -1;

  if (context_ && key != "")
  {
    ContextGuard context_guard(*context_);
    MADARA_GUARD_TYPE guard(mutex_);

    std::stringstream buffer;
    buffer << name_;
    buffer << delimiter_;
    buffer << key;

    std::string final_key = buffer.str();
    std::map<std::string, VariableReference>::iterator entry =
        map_.find(final_key);

    if (entry == map_.end())
    {
      VariableReference ref = context_->get_ref(final_key, settings_);
      map_[key] = ref;
      result = context_->set_jpeg(ref, value, size, settings_);
    }
    else
    {
      result = context_->set_jpeg(entry->second, value, size, settings_);
    }
  }

  return result;
}

int madara::knowledge::containers::Map::set_jpeg(const std::string& key,
    const unsigned char* value, size_t size,
    const KnowledgeUpdateSettings& settings)
{
  int result = -1;

  if (context_ && key != "")
  {
    ContextGuard context_guard(*context_);
    MADARA_GUARD_TYPE guard(mutex_);

    std::stringstream buffer;
    buffer << name_;
    buffer << delimiter_;
    buffer << key;

    std::string final_key = buffer.str();
    std::map<std::string, VariableReference>::iterator entry =
        map_.find(final_key);

    if (entry == map_.end())
    {
      VariableReference ref = context_->get_ref(final_key, settings);
      map_[key] = ref;
      result = context_->set_jpeg(ref, value, size, settings);
    }
    else
    {
      result = context_->set_jpeg(entry->second, value, size, settings);
    }
  }

  return result;
}

void madara::knowledge::containers::Map::set_quality(const std::string& key,
    uint32_t quality, const KnowledgeReferenceSettings& settings)
{
  if (context_)
  {
    ContextGuard context_guard(*context_);
    MADARA_GUARD_TYPE guard(mutex_);

    std::stringstream buffer;
    buffer << name_;
    buffer << delimiter_;
    buffer << key;

    context_->set_quality(buffer.str(), quality, true, settings);
  }
}

bool madara::knowledge::containers::Map::is_true(void) const
{
  bool result(false);

  madara_logger_log(context_->get_logger(), logger::LOG_MAJOR,
      "Map::is_true: checking for truth in all elements\n");

  if (context_ && name_ != "")
  {
    ContextGuard context_guard(*context_);
    MADARA_GUARD_TYPE guard(mutex_);

    result = true;

    for (InternalMap::const_iterator index = map_.begin(); index != map_.end();
         ++index)
    {
      madara_logger_log(context_->get_logger(), logger::LOG_DETAILED,
          "Map::is_true: checking index, is_false of %d. \n",
          (int)context_->get(index->second).is_false());

      if (context_->get(index->second).is_false())
      {
        madara_logger_log(context_->get_logger(), logger::LOG_MAJOR,
            "Map::is_true: result is false, breaking\n");

        result = false;
        break;
      }
    }

    if (map_.size() == 0)
      result = false;
  }

  madara_logger_log(context_->get_logger(), logger::LOG_MAJOR,
      "Map::is_true: final result is %d\n", (int)result);

  return result;
}

bool madara::knowledge::containers::Map::is_false(void) const
{
  return !is_true();
}

bool madara::knowledge::containers::Map::is_true_(void) const
{
  return is_true();
}

bool madara::knowledge::containers::Map::is_false_(void) const
{
  return is_false();
}
