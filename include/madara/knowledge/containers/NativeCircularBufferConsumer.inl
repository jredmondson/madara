
#ifndef _MADARA_KNOWLEDGE_CONTAINERS_NATIVECIRCULARBUFFERCONSUMER_INL_
#define _MADARA_KNOWLEDGE_CONTAINERS_NATIVECIRCULARBUFFERCONSUMER_INL_

#include <sstream>
#include <math.h>

#include "NativeCircularBufferConsumer.h"
#include "madara/knowledge/ContextGuard.h"
#include "madara/exceptions/IndexException.h"
#include "madara/exceptions/ContextException.h"
#include "madara/exceptions/NameException.h"

namespace madara
{
namespace knowledge
{
namespace containers
{
inline NativeCircularBufferConsumer::NativeCircularBufferConsumer()
  : context_(0), local_index_(-1)
{
}

inline void NativeCircularBufferConsumer::check_name(
    const char* func, const char* name)
{
  if (name == nullptr || name[0] == '\0')
  {
    throw exceptions::NameException(
        std::string("NativeCircularBufferConsumer::") + func +
        ": name is empty.");
  }
}

inline void NativeCircularBufferConsumer::check_context(const char* func) const
{
  if (!context_)
  {
    throw exceptions::ContextException(
        std::string("NativeCircularBufferConsumer::") + func +
        ": context is not set.");
  }

  if (!ref_.is_valid())
  {
    throw exceptions::ContextException(
        std::string("NativeCircularBufferConsumer::") + func +
        ": underlying record is not set.");
  }
}

inline NativeCircularBufferConsumer::NativeCircularBufferConsumer(
    const std::string& name, KnowledgeBase& knowledge)
  : context_(&(knowledge.get_context())), local_index_(0UL - 1)
{
  check_name(__func__, name.c_str());

  ContextGuard context_guard(knowledge);

  set_name(name, knowledge);
}

inline NativeCircularBufferConsumer::NativeCircularBufferConsumer(
    const std::string& name, Variables& knowledge)
  : context_(knowledge.get_context()), local_index_(0UL - 1)
{
  check_name(__func__, name.c_str());

  ContextGuard context_guard(*knowledge.get_context());

  set_name(name, knowledge);
}

inline bool NativeCircularBufferConsumer::operator==(
    const NativeCircularBufferConsumer& value) const
{
  return context_ == value.context_ && get_name() == value.get_name();
}

inline bool NativeCircularBufferConsumer::operator!=(
    const NativeCircularBufferConsumer& value) const
{
  return !operator==(value);
}

template<typename T>
void NativeCircularBufferConsumer::consume(T& value, size_t& dropped) const
{
  value = consume(dropped).to_any<T>();
}

template<typename T>
void NativeCircularBufferConsumer::peek_latest(
    size_t count, std::vector<T>& values) const
{
  // iterate over the returned records
  for (auto record : peek_latest(count))
  {
    // add them to the values
    if (record.is_valid())
      values.push_back(record.to_any<T>());
  }
}

inline std::vector<KnowledgeRecord> NativeCircularBufferConsumer::peek_latest(
    size_t count) const
{
  ContextGuard context_guard(*context_);

  KnowledgeRecord& rec = *ref_.get_record_unsafe();

  size_t newest_index = rec.get_history_newest_index();

  if (local_index_ + count > newest_index + 1)
  {
    count = (newest_index + 1) - local_index_;
  }

  std::vector<KnowledgeRecord> ret_vec = rec.get_newest(count);

  std::reverse(ret_vec.begin(), ret_vec.end());

  return ret_vec;
}

inline madara::knowledge::KnowledgeRecord
NativeCircularBufferConsumer::peek_latest(void) const
{
  ContextGuard context_guard(*context_);

  KnowledgeRecord& rec = *ref_.get_record_unsafe();

  size_t newest_index = rec.get_history_newest_index();

  if (local_index_ >= newest_index + 1)
  {
    return KnowledgeRecord();
  }

  return rec.get_newest();
}

template<typename T>
void NativeCircularBufferConsumer::consume_latest(
    size_t count, std::vector<T>& values) const
{
  // iterate over the returned records
  for (auto record : consume_latest(count))
  {
    // add them to the values
    if (record.is_valid())
      values.push_back(record.to_any<T>());
  }
}

template<typename T>
void NativeCircularBufferConsumer::consume_latest(
    size_t count, std::vector<T>& values, size_t& dropped) const
{
  dropped = get_dropped();

  // iterate over the returned records
  for (auto record : consume_latest(count))
  {
    // add them to the values
    if (record.is_valid())
      values.push_back(record.to_any<T>());
  }
}

inline std::vector<KnowledgeRecord>
NativeCircularBufferConsumer::consume_latest(size_t count) const
{
  ContextGuard context_guard(*context_);

  KnowledgeRecord& rec = *ref_.get_record_unsafe();

  size_t newest_index = rec.get_history_newest_index();

  std::vector<KnowledgeRecord> result;

  if (local_index_ + count > newest_index + 1)
  {
    count = (newest_index + 1) - local_index_;
  }

  local_index_ = newest_index + 1;

  std::vector<KnowledgeRecord> ret_vec = rec.get_newest(count);

  std::reverse(ret_vec.begin(), ret_vec.end());

  return ret_vec;
}

inline madara::knowledge::KnowledgeRecord
NativeCircularBufferConsumer::consume_latest(void) const
{
  ContextGuard context_guard(*context_);

  KnowledgeRecord& rec = *ref_.get_record_unsafe();

  size_t newest_index = rec.get_history_newest_index();

  KnowledgeRecord ret;

  if (local_index_ < newest_index + 1)
  {
    ret = rec.get_newest();
  }

  local_index_ = newest_index + 1;

  return ret;
}

inline std::vector<KnowledgeRecord>
NativeCircularBufferConsumer::consume_latest(
    size_t count, size_t& dropped) const
{
  ContextGuard context_guard(*context_);

  KnowledgeRecord& rec = *ref_.get_record_unsafe();

  size_t newest_index = rec.get_history_newest_index();

  dropped = get_dropped();

  if (local_index_ + count > newest_index)
  {
    count = newest_index - local_index_;
  }

  local_index_ = newest_index;

  return rec.get_newest(count);
}

inline madara::knowledge::KnowledgeRecord NativeCircularBufferConsumer::consume(
    void) const
{
  size_t dropped = 0;

  return consume(dropped);
}

inline madara::knowledge::KnowledgeRecord NativeCircularBufferConsumer::consume(
    size_t& dropped) const
{
  ContextGuard context_guard(*context_);

  KnowledgeRecord& rec = *ref_.get_record_unsafe();

  size_t oldest_index = rec.get_history_oldest_index();

  if (remaining() == 0)
  {
    return KnowledgeRecord();
  }

  if (local_index_ < oldest_index)
  {
    dropped = oldest_index - local_index_;
    local_index_ = oldest_index;
  }
  else
  {
    dropped = 0;
  }

  KnowledgeRecord ret;

  rec.get_history_range(&ret, local_index_, 1);
  ++local_index_;

  return ret;
}

template<typename T>
void NativeCircularBufferConsumer::consume_many(
    size_t count, std::vector<T>& values) const
{
  for (auto record : consume_many(count))
  {
    values.push_back(record.to_any<T>());
  }
}

inline std::vector<KnowledgeRecord> NativeCircularBufferConsumer::consume_many(
    size_t count, size_t& dropped) const
{
  check_context(__func__);

  ContextGuard context_guard(*context_);

  dropped = get_dropped();

  std::vector<KnowledgeRecord> ret_vec;
  for (size_t consume_counter = 0; consume_counter < count; ++consume_counter)
  {
    ret_vec.emplace_back(consume());
  }

  return ret_vec;
}

inline std::vector<KnowledgeRecord> NativeCircularBufferConsumer::consume_many(
    size_t count) const
{
  check_context(__func__);

  ContextGuard context_guard(*context_);

  std::vector<KnowledgeRecord> ret_vec;
  for (size_t consume_counter = 0; consume_counter < count; ++consume_counter)
  {
    ret_vec.emplace_back(consume());
  }

  return ret_vec;
}

template<typename T>
void NativeCircularBufferConsumer::inspect(
    KnowledgeRecord::Integer position, T& value) const
{
  value = inspect(position).to_any<T>();
}

inline madara::knowledge::KnowledgeRecord NativeCircularBufferConsumer::inspect(
    KnowledgeRecord::Integer position) const
{
  check_context(__func__);

  ContextGuard context_guard(*context_);

  KnowledgeRecord& rec = *ref_.get_record_unsafe();

  KnowledgeRecord ret;
  rec.get_history_range(&ret, local_index_ + position, 1);

  return ret;
}

template<typename T>
void NativeCircularBufferConsumer::inspect(KnowledgeRecord::Integer position,
    size_t count, std::vector<T>& values) const
{
  // iterate over the returned records
  for (auto record : inspect(position, count))
  {
    values.push_back(record.to_any<T>());
  }
}

inline std::vector<KnowledgeRecord> NativeCircularBufferConsumer::inspect(
    KnowledgeRecord::Integer position, size_t count) const
{
  check_context(__func__);

  ContextGuard context_guard(*context_);

  KnowledgeRecord& rec = *ref_.get_record_unsafe();

  size_t oldest_index = rec.get_history_oldest_index();

  if (local_index_ < oldest_index)
  {
    // Messages were dropped
    local_index_ = oldest_index;
  }

  KnowledgeRecord ret;
  std::vector<KnowledgeRecord> ret_vec;
  for (size_t inspect_counter = 0; inspect_counter < count; ++inspect_counter)
  {
    if (rec.get_history_range(
            &ret, local_index_ + position + inspect_counter, 1))
    {
      ret_vec.emplace_back(std::move(ret));
    }
  }

  return ret_vec;
}

inline std::string NativeCircularBufferConsumer::get_name(void) const
{
  return ref_.get_name();
}

inline size_t NativeCircularBufferConsumer::size(void) const
{
  check_context(__func__);

  ContextGuard context_guard(*context_);

  KnowledgeRecord& rec = *ref_.get_record_unsafe();

  return rec.get_history_capacity();
}

inline size_t NativeCircularBufferConsumer::remaining(void) const
{
  check_context(__func__);

  ContextGuard context_guard(*context_);

  KnowledgeRecord& rec = *ref_.get_record_unsafe();

  size_t newest_index = rec.get_history_newest_index();

  if (local_index_ > newest_index)
  {
    return 0;
  }

  return newest_index + 1 - local_index_;
}

inline size_t NativeCircularBufferConsumer::count(void) const
{
  check_context(__func__);

  ContextGuard context_guard(*context_);

  KnowledgeRecord& rec = *ref_.get_record_unsafe();
  return rec.get_history_size();
}

inline void NativeCircularBufferConsumer::set_name(
    const std::string& name, ThreadSafeContext& context)
{
  check_name(__func__, name.c_str());

  ContextGuard context_guard(context);

  context_ = &context;
  ref_ = context_->get_ref(name);
  local_index_ = 0;
}

inline void NativeCircularBufferConsumer::set_name(
    const std::string& name, KnowledgeBase& knowledge)
{
  set_name(name, knowledge.get_context());
}

inline void NativeCircularBufferConsumer::set_name(
    const std::string& name, Variables& knowledge)
{
  set_name(name, *knowledge.get_context());
}

inline void NativeCircularBufferConsumer::set_index(size_t index)
{
  check_context(__func__);

  ContextGuard context_guard(*context_);

  local_index_ = index;
}

inline size_t NativeCircularBufferConsumer::get_dropped(void) const
{
  check_context(__func__);

  ContextGuard context_guard(*context_);

  KnowledgeRecord& rec = *ref_.get_record_unsafe();

  size_t oldest_index = rec.get_history_oldest_index();

  if (local_index_ < oldest_index)
  {
    return oldest_index - local_index_;
  }

  return 0;
}

inline KnowledgeRecord NativeCircularBufferConsumer::get_record(void) const
{
  check_context(__func__);

  ContextGuard context_guard(*context_);

  return *ref_.get_record_unsafe();
}

}  // end containers namespace
}  // end knowledge namespace
}  // end madara namespace

#endif  //  _MADARA_KNOWLEDGE_CONTAINERS_NATIVECIRCULARBUFFERCONSUMER_INL_
