/* -*- C++ -*- */
#ifndef _MADARA_UTILITY_THREADSAFE_REFCOUNTER_H_
#define _MADARA_UTILITY_THREADSAFE_REFCOUNTER_H_

#include "ace/Guard_T.h"
#include "ace/Recursive_Thread_Mutex.h"
#include "madara/LockType.h"

namespace madara
{
  namespace utility
  {
    /**
     * @class ThreadSafeRefcounter
     * @brief This template class provides transparent reference counting
     *        of its template parameter @a T. This differs from ScopedArray
     *        in the type of memory it is meant to manage. ThreadSafeRefcounter manages
     *        single object instances. ScopedArray manages an array of
     *        instances,
     *
     *        This class can be used to automate the implementation of the
     *        Bridge pattern in C++.  
     */
    template <typename T>
    class ThreadSafeRefcounter
    {
    public:
      /// default Ctor
      ThreadSafeRefcounter (void);

      /// Ctor with refcounting functionality
      ThreadSafeRefcounter (T * ptr,
        bool increase_count = false, bool manage = true);

      /// copy Ctor
      ThreadSafeRefcounter (const ThreadSafeRefcounter & rhs);

      /// Dtor will delete pointer if refcount becomes 0
      virtual ~ThreadSafeRefcounter (void);

      /// assignment operator for times when you don't want
      /// the reference increased for incoming ptr
      void operator= (T * ptr);

      /// assignment operator
      void operator= (const ThreadSafeRefcounter & rhs);

      /// dereference operator
      inline T & operator* (void);

      /// dereference operator
      inline const T & operator* (void) const;

      /// mimic pointer dereferencing
      inline T * operator-> (void);

      /// mimic pointer dereferencing
      inline const T * operator-> (void) const;

      /// get the underlying pointer
      T * get_ptr (void);

      /// get the underlying pointer
      const T * get_ptr (void) const;

    private:
      /// implementation of the increment operation
      inline void increment (void);

      /// implementation of the decrement operation
      inline void decrement (void);

      

      /// A shim class that keeps track of the reference count and a
      /// pointer to the type @a T that's reference counted.
      struct Shim
      {
        /// Constructor.
        Shim (T * t, bool manage = true);
        
        /// Destructor.
        ~Shim (void);

        /// Pointer to the object that's being reference counted.
        T * t_;

        /// mutex for updating refcount_
        mutable MADARA_LOCK_TYPE mutex_;

        /// Current value of the reference count.
        volatile int refcount_;

        /// allow unmanaged references
        bool manage_;
      };

      /// Pointer to the @a Shim.
      Shim * ptr_;
    };
  }
}
#include "madara/utility/ThreadSafeRefcounter.cpp"

#endif /* _MADARA_UTILITY_THREADSAFE_REFCOUNTER_H_ */
