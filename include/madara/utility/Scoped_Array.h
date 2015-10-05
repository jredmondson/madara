/* -*- C++ -*- */
#ifndef _MADARA_UTILITY_SCOPED_ARRAY_H_
#define _MADARA_UTILITY_SCOPED_ARRAY_H_


namespace madara
{
  namespace utility
  {
    /**
     * @class Scoped_Array
     * @brief This template class provides transparent reference counting
     *        of its template parameter @a T. This differs from Refcounter
     *        in the members available and the deletion of memory.
     *        Refcounter deletes a single T memory location when the last
     *        instance of the pointer goes out of scope. Scoped_Array deletes
     *        an array of T objects when the pointer goes out of scope.
     *
     *        This class can be used to automate the implementation of the
     *        Bridge pattern in C++.  
     */
    template <typename T>
    class Scoped_Array
    {
    public:
      /// default Ctor
      Scoped_Array (void);

      /// Ctor with refcounting functionality
      Scoped_Array (T * ptr, bool increase_count = false);

      /// copy Ctor
      Scoped_Array (const Scoped_Array & rhs);

      /// Dtor will delete pointer if refcount becomes 0
      virtual ~Scoped_Array (void);

      /// assignment operator for times when you don't want
      /// the reference increased for incoming ptr
      void operator= (T * ptr);

      /// assignment operator
      void operator= (const Scoped_Array & rhs);

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
#include "madara/utility/Scoped_Array.inl"

#endif /* _MADARA_UTILITY_SCOPED_ARRAY_H_ */
