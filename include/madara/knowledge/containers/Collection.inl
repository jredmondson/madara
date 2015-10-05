#ifndef _MADARA_CONTAINERS_COLLECTION_CONTAINER_H
#define _MADARA_CONTAINERS_COLLECTION_CONTAINER_H

#include "Collection.h"
#include "madara/knowledge/Context_Guard.h"

inline void
madara::knowledge::containers::Collection::add (
const Barrier & container)
{
  Guard guard (mutex_);
  vector_.push_back (container.clone ());
}

inline void
madara::knowledge::containers::Collection::add (
const Counter & container)
{
  Guard guard (mutex_);
  vector_.push_back (container.clone ());
}

inline void
madara::knowledge::containers::Collection::add (
  const Integer & container)
{
  Guard guard (mutex_);
  vector_.push_back (container.clone ());
}

inline void
madara::knowledge::containers::Collection::add (
  const Integer_Vector & container)
{
  Guard guard (mutex_);
  vector_.push_back (container.clone ());
}

inline void
madara::knowledge::containers::Collection::add (
const Native_Integer_Vector & container)
{
  Guard guard (mutex_);
  vector_.push_back (container.clone ());
}

inline void
madara::knowledge::containers::Collection::add (
  const Double & container)
{
  Guard guard (mutex_);
  vector_.push_back (container.clone ());
}

inline void
madara::knowledge::containers::Collection::add (
  const Double_Vector & container)
{
  Guard guard (mutex_);
  vector_.push_back (container.clone ());
}

inline void
madara::knowledge::containers::Collection::add (
const Native_Double_Vector & container)
{
  Guard guard (mutex_);
  vector_.push_back (container.clone ());
}

inline void
madara::knowledge::containers::Collection::add (
  const String & container)
{
  Guard guard (mutex_);
  vector_.push_back (container.clone ());
}

inline void
madara::knowledge::containers::Collection::add (
  const String_Vector & container)
{
  Guard guard (mutex_);
  vector_.push_back (container.clone ());
}

inline void
madara::knowledge::containers::Collection::add (
const Buffer_Vector & container)
{
  Guard guard (mutex_);
  vector_.push_back (container.clone ());
}

inline void
madara::knowledge::containers::Collection::add (
  const Map & container)
{
  Guard guard (mutex_);
  vector_.push_back (container.clone ());
}

inline void
madara::knowledge::containers::Collection::add (
  const Flex_Map & container)
{
  Guard guard (mutex_);
  vector_.push_back (container.clone ());
}

inline void
madara::knowledge::containers::Collection::add (
  const Vector & container)
{
  Guard guard (mutex_);
  vector_.push_back (container.clone ());
}

inline void
madara::knowledge::containers::Collection::add (
  const Collection & container)
{
  Guard guard (mutex_);
  vector_.push_back (container.clone ());
}

inline void
madara::knowledge::containers::Collection::clear (void)
{
  Guard guard (mutex_);

  for (size_t i = 0; i < vector_.size (); ++i)
  {
    delete vector_[i];
  }

  vector_.clear ();
}

inline void
madara::knowledge::containers::Collection::set_settings (
const Knowledge_Update_Settings & settings)
{
  Guard guard (mutex_);

  for (size_t i = 0; i < vector_.size (); ++i)
  {
    vector_[i]->set_settings (settings);
  }
}

inline size_t
madara::knowledge::containers::Collection::size (void) const
{
  Guard guard (mutex_);

  return vector_.size ();
}

#endif
