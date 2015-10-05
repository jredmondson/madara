/* -*- C++ -*- */
#ifndef _MADARA_THREADSAFE_REFCOUNTER_CPP_
#define _MADARA_THREADSAFE_REFCOUNTER_CPP_

#include "madara/utility/Thread_Safe_Refcounter.h"

/// default Ctor
template <typename T>
madara::utility::Thread_Safe_Refcounter<T>::Thread_Safe_Refcounter (void)
  : ptr_ (0)
{
}

/// Ctor with refcounting functionality
template <typename T>
madara::utility::Thread_Safe_Refcounter<T>::Thread_Safe_Refcounter (T *ptr,
  bool increase_count, bool manage)
  : ptr_ (new Shim (ptr, manage))
{
  if (increase_count)
    increment ();
}

  /// copy Ctor
template <typename T>
madara::utility::Thread_Safe_Refcounter<T>::Thread_Safe_Refcounter (
  const Thread_Safe_Refcounter &rhs)
  : ptr_ (rhs.ptr_)
{
  increment ();
}

  /// Dtor will delete pointer if refcount becomes 0
template <typename T>
madara::utility::Thread_Safe_Refcounter<T>::~Thread_Safe_Refcounter (void)
{
  decrement ();
}

/// assignment operator for times when you don't want the reference
/// increased for incoming ptr.
template <typename T>
void 
madara::utility::Thread_Safe_Refcounter<T>::operator= (T *ptr)
{
  decrement ();
  ptr_ = new Shim (ptr);
}

  /// assignment operator
template <typename T>
void
madara::utility::Thread_Safe_Refcounter<T>::operator= (const Thread_Safe_Refcounter& rhs)
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
madara::utility::Thread_Safe_Refcounter<T>::get_ptr (void)
{
  return ptr_->t_;
}

/// get the underlying pointer
template <typename T>
const T *
madara::utility::Thread_Safe_Refcounter<T>::get_ptr (void) const
{
  return ptr_->t_;
}


/// dereference operator
template <typename T>
T & 
madara::utility::Thread_Safe_Refcounter<T>::operator* (void)
{
  return *ptr_->t_;
}

/// dereference operator
template <typename T>
const 
T &
madara::utility::Thread_Safe_Refcounter<T>::operator* (void) const
{
  return *ptr_->t_;
}

/// mimic pointer dereferencing
template <typename T>
T *
madara::utility::Thread_Safe_Refcounter<T>::operator-> (void)
{
  return ptr_->t_;
}

/// mimic pointer dereferencing
template <typename T>
const T *
madara::utility::Thread_Safe_Refcounter<T>::operator-> (void) const
{
  return ptr_->t_;
}

/// implementation of the increment operation
template <typename T>
void 
madara::utility::Thread_Safe_Refcounter<T>::increment (void)
{
  if (ptr_ && ptr_->manage_)
  {
    ++ptr_->refcount_;
  }
}

  /// implementation of the decrement operation
template <typename T>
void 
madara::utility::Thread_Safe_Refcounter<T>::decrement (void)
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
madara::utility::Thread_Safe_Refcounter<T>::Shim::Shim (T *t, bool manage)
  : t_ (t), refcount_ (1), manage_ (manage)
{
}

template <typename T>
madara::utility::Thread_Safe_Refcounter<T>::Shim::~Shim (void) 
{
  if (manage_)
    delete t_; 
}

#endif /* _MADARA_THREADSAFE_REFCOUNTER_CPP_ */
