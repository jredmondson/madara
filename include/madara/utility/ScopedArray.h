/* -*- C++ -*- */
#ifndef _MADARA_UTILITY_SCOPED_ARRAY_H_
#define _MADARA_UTILITY_SCOPED_ARRAY_H_

namespace madara
{
  namespace utility
  {
    /**
     * @class ScopedArray
     * @brief This template class provides transparent reference counting
     *        of its template parameter @a T. This differs from Refcounter
     *        in the members available and the deletion of memory.
     *        Refcounter deletes a single T memory location when the last
     *        instance of the pointer goes out of scope. ScopedArray deletes
     *        an array of T objects when the pointer goes out of scope.
     *
     *        This class can be used to automate the implementation of the
     *        Bridge pattern in C++.  
     */
    template <typename T>
    class ScopedArray
    {
    public:
      /// default Ctor
      ScopedArray (void);

      /// Ctor with refcounting functionality
      ScopedArray (T * ptr, bool increase_count = false);

      /// copy Ctor
      ScopedArray (const ScopedArray & rhs);

      /// Dtor will delete pointer if refcount becomes 0
      virtual ~ScopedArray (void);

      /// assignment operator for times when you don't want
      /// the reference increased for incoming ptr
      void operator= (T * ptr);

      /// assignment operator
      void operator= (const ScopedArray & rhs);

      /// get the underlying pointer
      T * get_ptr (void);

      /// get the underlying pointer
      const T * get_ptr (void) const;

      /// get the underlying pointer
      T * get (void);

      /// get the underlying pointer
      const T * get (void) const;

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
        Shim (T * t);
        
        /// Destructor.
        ~Shim (void);

        /// Pointer to the object that's being reference counted.
        T * t_;

        /// Current value of the reference count.
        int refcount_;
      };

      /// Pointer to the @a Shim.
      Shim * ptr_;
    };
  }
}
#include "madara/utility/ScopedArray.inl"

#endif /* _MADARA_UTILITY_SCOPED_ARRAY_H_ */
