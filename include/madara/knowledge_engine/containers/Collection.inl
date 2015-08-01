#ifndef _MADARA_CONTAINERS_COLLECTION_CONTAINER_H
#define _MADARA_CONTAINERS_COLLECTION_CONTAINER_H

#include "Collection.h"
#include "madara/knowledge_engine/Context_Guard.h"

inline void
Madara::Knowledge_Engine::Containers::Collection::add (
  const Integer & container)
{
  Guard guard (mutex_);
  vector_.push_back (container.clone ());
}

inline void
Madara::Knowledge_Engine::Containers::Collection::add (
  const Integer_Vector & container)
{
  Guard guard (mutex_);
  vector_.push_back (container.clone ());
}

inline void
Madara::Knowledge_Engine::Containers::Collection::add (
const Native_Integer_Vector & container)
{
  Guard guard (mutex_);
  vector_.push_back (container.clone ());
}

inline void
Madara::Knowledge_Engine::Containers::Collection::add (
  const Double & container)
{
  Guard guard (mutex_);
  vector_.push_back (container.clone ());
}

inline void
Madara::Knowledge_Engine::Containers::Collection::add (
  const Double_Vector & container)
{
  Guard guard (mutex_);
  vector_.push_back (container.clone ());
}

inline void
Madara::Knowledge_Engine::Containers::Collection::add (
const Native_Double_Vector & container)
{
  Guard guard (mutex_);
  vector_.push_back (container.clone ());
}

inline void
Madara::Knowledge_Engine::Containers::Collection::add (
  const String & container)
{
  Guard guard (mutex_);
  vector_.push_back (container.clone ());
}

inline void
Madara::Knowledge_Engine::Containers::Collection::add (
  const String_Vector & container)
{
  Guard guard (mutex_);
  vector_.push_back (container.clone ());
}

inline void
Madara::Knowledge_Engine::Containers::Collection::add (
const Buffer_Vector & container)
{
  Guard guard (mutex_);
  vector_.push_back (container.clone ());
}

inline void
Madara::Knowledge_Engine::Containers::Collection::add (
  const Map & container)
{
  Guard guard (mutex_);
  vector_.push_back (container.clone ());
}

inline void
Madara::Knowledge_Engine::Containers::Collection::add (
  const Flex_Map & container)
{
  Guard guard (mutex_);
  vector_.push_back (container.clone ());
}

inline void
Madara::Knowledge_Engine::Containers::Collection::add (
  const Vector & container)
{
  Guard guard (mutex_);
  vector_.push_back (container.clone ());
}

inline void
Madara::Knowledge_Engine::Containers::Collection::add (
  const Collection & container)
{
  Guard guard (mutex_);
  vector_.push_back (container.clone ());
}

inline void
Madara::Knowledge_Engine::Containers::Collection::clear (void)
{
  Guard guard (mutex_);

  for (size_t i = 0; i < vector_.size (); ++i)
  {
    delete vector_[i];
  }

  vector_.clear ();
}

inline void
Madara::Knowledge_Engine::Containers::Collection::set_settings (
const Knowledge_Update_Settings & settings)
{
  Guard guard (mutex_);

  for (size_t i = 0; i < vector_.size (); ++i)
  {
    vector_[i]->set_settings (settings);
  }
}

inline size_t
Madara::Knowledge_Engine::Containers::Collection::size (void) const
{
  Guard guard (mutex_);

  return vector_.size ();
}

#endif
