/* -*- C++ -*- */
#ifndef _SCOPED_ARRAY_CPP_
#define _SCOPED_ARRAY_CPP_

#include "madara/utility/ScopedArray.h"

/// default Ctor
template <typename T>
madara::utility::ScopedArray<T>::ScopedArray (void)
  : ptr_ (0)
{
}

/// Ctor with refcounting functionality
template <typename T>
madara::utility::ScopedArray<T>::ScopedArray (T *ptr, bool increase_count)
  : ptr_ (ptr ? new Shim (ptr) : 0)
{
  if (increase_count)
    increment ();
}

  /// copy Ctor
template <typename T>
madara::utility::ScopedArray<T>::ScopedArray (const ScopedArray &rhs)
  : ptr_ (rhs.ptr_)
{
  increment ();
}

  /// Dtor will delete pointer if refcount becomes 0
template <typename T>
madara::utility::ScopedArray<T>::~ScopedArray (void)
{
  decrement ();
}

/// assignment operator for times when you don't want the reference
/// increased for incoming ptr.
template <typename T>
void 
madara::utility::ScopedArray<T>::operator= (T *ptr)
{
  decrement ();

  if (ptr)
    ptr_ = new Shim (ptr);
  else
    ptr_ = 0;
}

  /// assignment operator
template <typename T>
void
madara::utility::ScopedArray<T>::operator= (const ScopedArray& rhs)
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
madara::utility::ScopedArray<T>::get_ptr (void)
{
  return ptr_ != 0 ? ptr_->t_ : 0;
}

/// get the underlying pointer
template <typename T>
const T *
madara::utility::ScopedArray<T>::get_ptr (void) const
{
  return ptr_ != 0 ? ptr_->t_ : 0;
}

/// get the underlying pointer
template <typename T>
T *
madara::utility::ScopedArray<T>::get (void)
{
  return ptr_ != 0 ? ptr_->t_ : 0;
}

/// get the underlying pointer
template <typename T>
const T *
madara::utility::ScopedArray<T>::get (void) const
{
  return ptr_ != 0 ? ptr_->t_ : 0;
}


/// implementation of the increment operation
template <typename T>
void 
madara::utility::ScopedArray<T>::increment (void)
{
  if (ptr_)
    ++ptr_->refcount_;
}

  /// implementation of the decrement operation
template <typename T>
void 
madara::utility::ScopedArray<T>::decrement (void)
{
  if (ptr_)
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
madara::utility::ScopedArray<T>::Shim::Shim (T *t)
  : t_ (t), refcount_ (1) 
{
}

template <typename T>
madara::utility::ScopedArray<T>::Shim::~Shim (void) 
{ 
  delete [] t_; 
}

#endif /* _REFCOUNTER_CPP_ */
