#ifndef _MADARA_CONTAINERS_COLLECTION_CONTAINER_H
#define _MADARA_CONTAINERS_COLLECTION_CONTAINER_H

#include "Collection.h"
#include "madara/knowledge/ContextGuard.h"

inline void madara::knowledge::containers::Collection::add(
    const BaseContainer& container)
{
  MADARA_GUARD_TYPE guard(mutex_);
  vector_.push_back(container.clone());
}

inline void madara::knowledge::containers::Collection::clear(void)
{
  MADARA_GUARD_TYPE guard(mutex_);

  for (size_t i = 0; i < vector_.size(); ++i)
  {
    delete vector_[i];
  }

  vector_.clear();
}

inline void madara::knowledge::containers::Collection::set_settings(
    const KnowledgeUpdateSettings& settings)
{
  MADARA_GUARD_TYPE guard(mutex_);

  for (size_t i = 0; i < vector_.size(); ++i)
  {
    vector_[i]->set_settings(settings);
  }
}

inline size_t madara::knowledge::containers::Collection::size(void) const
{
  MADARA_GUARD_TYPE guard(mutex_);

  return vector_.size();
}

#endif
