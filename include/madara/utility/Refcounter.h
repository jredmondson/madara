/* -*- C++ -*- */
#ifndef _MADARA_UTILITY_REFCOUNTER_H_
#define _MADARA_UTILITY_REFCOUNTER_H_


namespace madara
{
  namespace utility
  {
    /**
     * @class Refcounter
     * @brief This template class provides transparent reference counting
     *        of its template parameter @a T. This differs from Scoped_Array
     *        in the type of memory it is meant to manage. Refcounter manages
     *        single object instances. Scoped_Array manages an array of
     *        instances,
     *
     *        This class can be used to automate the implementation of the
     *        Bridge pattern in C++.  
     */
    template <typename T>
    class Refcounter
    {
    public:
      /// default Ctor
      Refcounter (void);

      /// Ctor with refcounting functionality
      Refcounter (T * ptr, bool increase_count = false);

      /// copy Ctor
      Refcounter (const Refcounter & rhs);

      /// Dtor will delete pointer if refcount becomes 0
      virtual ~Refcounter (void);

      /// assignment operator for times when you don't want
      /// the reference increased for incoming ptr
      void operator= (T * ptr);

      /// assignment operator
      void operator= (const Refcounter & rhs);

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
#include "madara/utility/Refcounter.cpp"

#endif /* _MADARA_UTILITY_REFCOUNTER_H_ */
