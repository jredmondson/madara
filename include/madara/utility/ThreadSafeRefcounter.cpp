/* -*- C++ -*- */
#ifndef _MADARA_THREADSAFE_REFCOUNTER_CPP_
#define _MADARA_THREADSAFE_REFCOUNTER_CPP_

#include "madara/utility/ThreadSafeRefcounter.h"

/// default Ctor
template <typename T>
madara::utility::ThreadSafeRefcounter<T>::ThreadSafeRefcounter (void)
  : ptr_ (0)
{
}

/// Ctor with refcounting functionality
template <typename T>
madara::utility::ThreadSafeRefcounter<T>::ThreadSafeRefcounter (T *ptr,
  bool increase_count, bool manage)
  : ptr_ (new Shim (ptr, manage))
{
  if (increase_count)
    increment ();
}

  /// copy Ctor
template <typename T>
madara::utility::ThreadSafeRefcounter<T>::ThreadSafeRefcounter (
  const ThreadSafeRefcounter &rhs)
  : ptr_ (rhs.ptr_)
{
  increment ();
}

  /// Dtor will delete pointer if refcount becomes 0
template <typename T>
madara::utility::ThreadSafeRefcounter<T>::~ThreadSafeRefcounter (void)
{
  decrement ();
}

/// assignment operator for times when you don't want the reference
/// increased for incoming ptr.
template <typename T>
void 
madara::utility::ThreadSafeRefcounter<T>::operator= (T *ptr)
{
  decrement ();
  ptr_ = new Shim (ptr);
}

  /// assignment operator
template <typename T>
void
madara::utility::ThreadSafeRefcounter<T>::operator= (const ThreadSafeRefcounter& rhs)
{
  if (this != &rhs)
  {
    decrement ();
    ptr_ = rhs.ptr_;
    increment ();
  }
}

/// get the underlying pointer
template <typename T>
T * 
madara::utility::ThreadSafeRefcounter<T>::get_ptr (void)
{
  T * result (0);
  
  if (ptr_)
  {
    result = ptr_->t_;
  }

  return result;
}

/// get the underlying pointer
template <typename T>
const T *
madara::utility::ThreadSafeRefcounter<T>::get_ptr (void) const
{
  const T * result (0);

  if (ptr_)
  {
    result = ptr_->t_;
  }

  return result;
}

/// get the underlying pointer
template <typename T>
bool
madara::utility::ThreadSafeRefcounter<T>::is_valid (void) const
{
  return ptr_ && ptr_->refcount_ > 0;
}

/// get the underlying pointer
template <typename T>
T *
madara::utility::ThreadSafeRefcounter<T>::get (void)
{
  T * result (0);

  if (ptr_)
  {
    result = ptr_->t_;
  }

  return result;
}

/// get the underlying pointer
template <typename T>
const T *
madara::utility::ThreadSafeRefcounter<T>::get (void) const
{
  const T * result (0);

  if (ptr_)
  {
    result = ptr_->t_;
  }

  return result;
}

/// dereference operator
template <typename T>
T & 
madara::utility::ThreadSafeRefcounter<T>::operator* (void)
{
  return *ptr_->t_;
}

/// dereference operator
template <typename T>
const 
T &
madara::utility::ThreadSafeRefcounter<T>::operator* (void) const
{
  return *ptr_->t_;
}

/// mimic pointer dereferencing
template <typename T>
T *
madara::utility::ThreadSafeRefcounter<T>::operator-> (void)
{
  T * result (0);

  if (ptr_)
  {
    result = ptr_->t_;
  }

  return result;
}

/// mimic pointer dereferencing
template <typename T>
const T *
madara::utility::ThreadSafeRefcounter<T>::operator-> (void) const
{
  const T * result (0);

  if (ptr_)
  {
    result = ptr_->t_;
  }

  return result;
}

/// implementation of the increment operation
template <typename T>
void 
madara::utility::ThreadSafeRefcounter<T>::increment (void)
{
  if (ptr_ && ptr_->manage_)
  {
    ++ptr_->refcount_;
  }
}

  /// implementation of the decrement operation
template <typename T>
void 
madara::utility::ThreadSafeRefcounter<T>::decrement (void)
{
  if (ptr_&& ptr_->manage_)
  {
    --ptr_->refcount_;
    if (ptr_->refcount_ <= 0)
    {
      delete ptr_;
      ptr_ = 0;
    }
  }
}

template <typename T>
madara::utility::ThreadSafeRefcounter<T>::Shim::Shim (T *t, bool manage)
  : t_ (t), refcount_ (1), manage_ (manage)
{
}

template <typename T>
madara::utility::ThreadSafeRefcounter<T>::Shim::~Shim (void) 
{
  if (manage_)
    delete t_; 
}

#endif /* _MADARA_THREADSAFE_REFCOUNTER_CPP_ */
