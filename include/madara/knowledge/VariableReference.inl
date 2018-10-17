#ifndef _MADARA_VARIABLE_REFERENCE_INL_
#define _MADARA_VARIABLE_REFERENCE_INL_

/**
 * @file VariableReference.inl
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains the inlined functions for the VariableReference class
 */

namespace madara
{
namespace knowledge
{
inline VariableReference::VariableReference(pair_ptr entry) : entry_(entry) {}

inline bool VariableReference::operator==(const VariableReference& rhs) const
{
  return this->entry_ == rhs.entry_;
}

inline bool VariableReference::operator!=(const VariableReference& rhs) const
{
  return !(*this == rhs);
}

inline const char* VariableReference::get_name(void) const
{
  if(!is_valid())
  {
    return nullptr;
  }
  return entry_->first.c_str();
}

inline bool VariableReference::is_valid(void) const
{
  return entry_ != nullptr;
}

inline KnowledgeRecord* VariableReference::get_record_unsafe(void) const
{
  if(!is_valid())
  {
    return nullptr;
  }
  return &entry_->second;
}

inline void VariableReference::assign(pair_ptr entry)
{
  entry_ = entry;
}

inline uint32_t max_quality(const knowledge::VariableReferenceMap& records)
{
  uint32_t max = 0;

  // iterate over the list and return the max
  for(const auto& pair : records)
  {
    KnowledgeRecord* rec = pair.second.get_record_unsafe();
    if(!rec)
    {
      continue;
    }
    max = std::max<uint32_t>(rec->quality, max);
  }
  return max;
}
}
}

#endif  // _MADARA_VARIABLE_REFERENCE_INL_
