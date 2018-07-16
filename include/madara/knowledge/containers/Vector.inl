
#ifndef _MADARA_KNOWLEDGE_CONTAINERS_VECTOR_INL_
#define _MADARA_KNOWLEDGE_CONTAINERS_VECTOR_INL_

#include <sstream>

#include "Vector.h"
#include "madara/knowledge/ContextGuard.h"

/**
 * @file Vector.h
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains inline functions of the Vector class
 **/

namespace madara { namespace knowledge { namespace containers {

  inline size_t
  Vector::size (void) const
  {
    MADARA_GUARD_TYPE guard (mutex_);
    return vector_.size ();
  }


  inline void
  Vector::set_name (
    const std::string & var_name,
    KnowledgeBase & knowledge, int size)
  {
    if (context_ != &(knowledge.get_context ()) || name_ != var_name)
    {
      context_ = &(knowledge.get_context ());

      ContextGuard context_guard (*context_);
      MADARA_GUARD_TYPE guard (mutex_);

      name_ = var_name;

      vector_.clear ();

      size_ = get_size_ref ();

      resize (size);
    }
  }

  inline void
  Vector::set_name (
    const std::string & var_name,
    Variables & knowledge, int size)
  {
    if (context_ != knowledge.get_context () || name_ != var_name)
    {
      context_ = knowledge.get_context ();

      ContextGuard context_guard (*context_);
      MADARA_GUARD_TYPE guard (mutex_);

      name_ = var_name;

      vector_.clear ();
      resize (size);
    }
  }

  inline void
  Vector::set_name (
    const std::string & var_name,
    ThreadSafeContext & knowledge, int size)
  {
    if (context_ != &knowledge || name_ != var_name)
    {
      context_ = &knowledge;

      ContextGuard context_guard (*context_);
      MADARA_GUARD_TYPE guard (mutex_);

      name_ = var_name;

      vector_.clear ();
      resize (size);
    }
  }

  inline void
  Vector::set_delimiter (
  const std::string & delimiter)
  {
    delimiter_ = delimiter;
    if (context_)
    {
      ContextGuard context_guard (*context_);
      MADARA_GUARD_TYPE guard (mutex_);

      vector_.clear ();
      resize (-1);
    }
  }

  inline std::string
  Vector::get_delimiter (void)
  {
    return delimiter_;
  }


} // end containers namespace
} // end knowledge namespace
} // end madara namespace

#endif // _MADARA_KNOWLEDGE_CONTAINERS_VECTOR_INL_
