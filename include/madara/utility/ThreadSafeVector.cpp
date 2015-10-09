/* -*- C++ -*- */
#ifndef _MADARA_THREADSAFE_VECTOR_CPP_
#define _MADARA_THREADSAFE_VECTOR_CPP_

#include "madara/utility/ThreadSafeVector.h"

template <typename T>
madara::utility::ThreadSafeVector<T>::ThreadSafeVector (void)
{
}

template <typename T>
madara::utility::ThreadSafeVector<T>::ThreadSafeVector (
  const ThreadSafeVector &rhs)
{
  Guard rhs_guard (rhs.mutex_);
  vector_ = rhs.vector_;
}

template <typename T>
madara::utility::ThreadSafeVector<T>::ThreadSafeVector (
  const std::vector<T> &rhs)
{
  vector_ = rhs;
}

template <typename T>
madara::utility::ThreadSafeVector<T>::~ThreadSafeVector (void)
{
  clear ();
}

template <typename T>
void
  madara::utility::ThreadSafeVector<T>::operator= (const ThreadSafeVector& rhs)
{
  Guard guard (mutex_);
  Guard rhs_guard (rhs.mutex_);

  if (this != &rhs)
  {
    vector_ = rhs.vector_;
  }
}

template <typename T>
void
  madara::utility::ThreadSafeVector<T>::operator= (const std::vector<T> & rhs)
{
  Guard guard (mutex_);

  if (this->vector_ != &rhs)
  {
    vector_ = rhs;
  }
}

template <typename T>
T & madara::utility::ThreadSafeVector<T>::operator[] (size_t index)
{
  Guard guard (mutex_);
  return vector_[index];
}

template <typename T>
const T & madara::utility::ThreadSafeVector<T>::operator[] (size_t index) const
{
  Guard guard (mutex_);
  return vector_[index];
}

template <typename T>
void madara::utility::ThreadSafeVector<T>::push_back (T & value)
{
  Guard guard (mutex_);
  vector_.push_back (value);
}

template <typename T>
const T & madara::utility::ThreadSafeVector<T>::back (void) const
{
  Guard guard (mutex_);
  return vector_.back ();
}

template <typename T>
T madara::utility::ThreadSafeVector<T>::pop_back (void)
{
  Guard guard (mutex_);
  T result (vector_.back ());
  
  vector_.pop_back ();
  return result;
}

template <typename T>
size_t madara::utility::ThreadSafeVector<T>::erase (size_t index)
{
  Guard guard (mutex_);

  if (index < vector_.size ())
    vector_.erase (vector_.begin () + index);

  return vector_.size ();
}

template <typename T>
T & madara::utility::ThreadSafeVector<T>::back (void)
{
  Guard guard (mutex_);
  return vector_.back ();
}

template <typename T>
void madara::utility::ThreadSafeVector<T>::resize (size_t new_size) const
{
  Guard guard (mutex_);
  vector_.resize (new_size);
}

template <typename T>
void madara::utility::ThreadSafeVector<T>::reserve (size_t new_size) const
{
  Guard guard (mutex_);
  vector_.reserve (new_size);
}

template <typename T>
size_t madara::utility::ThreadSafeVector<T>::size (void) const
{
  Guard guard (mutex_);
  return vector_.size ();
}

template <typename T>
size_t madara::utility::ThreadSafeVector<T>::max_size (void) const
{
  Guard guard (mutex_);
  return vector_.max_size ();
}

template <typename T>
void madara::utility::ThreadSafeVector<T>::clear (void)
{
  Guard guard (mutex_);
  vector_.clear ();
}

template <typename T>
void madara::utility::ThreadSafeVector<T>::lock (void) const
{
  mutex_.acquire ();
}

template <typename T>
void madara::utility::ThreadSafeVector<T>::acquire (void) const
{
  mutex_.acquire ();
}

template <typename T>
void madara::utility::ThreadSafeVector<T>::unlock (void) const
{
  mutex_.release ();
}

template <typename T>
void madara::utility::ThreadSafeVector<T>::release (void) const
{
  mutex_.release ();
}

#endif /* _MADARA_THREADSAFE_VECTOR_CPP_ */
