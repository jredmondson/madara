/* -*- C++ -*- */
#ifndef _SCOPED_ARRAY_CPP_
#define _SCOPED_ARRAY_CPP_

#include "madara/utility/Scoped_Array.h"

/// default Ctor
template <typename T>
Madara::Utility::Scoped_Array<T>::Scoped_Array (void)
  : ptr_ (0)
{
}

/// Ctor with refcounting functionality
template <typename T>
Madara::Utility::Scoped_Array<T>::Scoped_Array (T *ptr, bool increase_count)
  : ptr_ (new Shim (ptr))
{
  if (increase_count)
    increment ();
}

  /// copy Ctor
template <typename T>
Madara::Utility::Scoped_Array<T>::Scoped_Array (const Scoped_Array &rhs)
  : ptr_ (rhs.ptr_)
{
  increment ();
}

  /// Dtor will delete pointer if refcount becomes 0
template <typename T>
Madara::Utility::Scoped_Array<T>::~Scoped_Array (void)
{
  decrement ();
}

/// assignment operator for times when you don't want the reference
/// increased for incoming ptr.
template <typename T>
void 
Madara::Utility::Scoped_Array<T>::operator= (T *ptr)
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
Madara::Utility::Scoped_Array<T>::operator= (const Scoped_Array& rhs)
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
Madara::Utility::Scoped_Array<T>::get_ptr (void)
{
  return ptr_->t_;
}

/// get the underlying pointer
template <typename T>
const T *
Madara::Utility::Scoped_Array<T>::get_ptr (void) const
{
  return ptr_->t_;
}


/// implementation of the increment operation
template <typename T>
void 
Madara::Utility::Scoped_Array<T>::increment (void)
{
  if (ptr_)
    ++ptr_->refcount_;
}

  /// implementation of the decrement operation
template <typename T>
void 
Madara::Utility::Scoped_Array<T>::decrement (void)
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
Madara::Utility::Scoped_Array<T>::Shim::Shim (T *t)
  : t_ (t), refcount_ (1) 
{
}

template <typename T>
Madara::Utility::Scoped_Array<T>::Shim::~Shim (void) 
{ 
  delete [] t_; 
}

#endif /* _REFCOUNTER_CPP_ */
