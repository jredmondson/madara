#include "Collection.h"

Madara::Knowledge_Engine::Containers::Collection::Collection ()
: Base_Container ()
{
}


Madara::Knowledge_Engine::Containers::Collection::Collection (
  const Collection & rhs)
: Base_Container ()
{
  vector_.resize (rhs.vector_.size ());
  for (size_t i = 0; i < rhs.vector_.size (); ++i)
  {
    vector_[i] = rhs.vector_[i]->clone ();
  }
}


Madara::Knowledge_Engine::Containers::Collection::~Collection ()
{
  clear ();
}

void
Madara::Knowledge_Engine::Containers::Collection::modify (void)
{
  Guard guard (mutex_);

  for (size_t i = 0; i < vector_.size (); ++i)
  {
    vector_[i]->modify_ ();
  }
}

std::string
Madara::Knowledge_Engine::Containers::Collection::get_debug_info (void)
{
  Guard guard (mutex_);

  std::stringstream result;

  for (size_t i = 0; i < vector_.size (); ++i)
  {
    result << vector_[i]->get_debug_info_ () << "\n";
  }

  return result.str ();
}

void
Madara::Knowledge_Engine::Containers::Collection::modify_ (void)
{
  modify ();
}

std::string
Madara::Knowledge_Engine::Containers::Collection::get_debug_info_ (void)
{
  return get_debug_info ();
}

Madara::Knowledge_Engine::Containers::Base_Container *
Madara::Knowledge_Engine::Containers::Collection::clone (void) const
{
  return new Collection (*this);
}

bool
Madara::Knowledge_Engine::Containers::Collection::is_true (void) const
{
  bool result (true);

  if (context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    for (size_t i = 0; i < vector_.size (); ++i)
    {
      if (vector_[i]->is_false_ ())
      {
        result = false;
        break;
      }
    }
  }

  return result;
}

bool
Madara::Knowledge_Engine::Containers::Collection::is_false (void) const
{
  return !is_true ();
}


bool
Madara::Knowledge_Engine::Containers::Collection::is_true_ (void) const
{
  return is_true ();
}

bool
Madara::Knowledge_Engine::Containers::Collection::is_false_ (void) const
{
  return is_false ();
}
