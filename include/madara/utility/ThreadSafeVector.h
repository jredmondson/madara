/* -*- C++ -*- */
#ifndef _MADARA_UTILITY_THREADSAFE_VECTOR_H_
#define _MADARA_UTILITY_THREADSAFE_VECTOR_H_

#include <vector>
#include "madara/LockType.h"

namespace madara
{
  namespace utility
  {
    /**
     * @class ThreadSafeVector
     * @brief Manages a thread safe STL vector
     */
    template <typename T>
    class ThreadSafeVector
    {
    public:
      /**
       * Constructor
       **/
      ThreadSafeVector (void);
      
      /**
       * Copy constructor
       * @param  rhs   the vector to copy
       **/
      ThreadSafeVector (const ThreadSafeVector & rhs);
      
      /**
       * Copy constructor
       * @param  rhs   the vector to copy
       **/
      ThreadSafeVector (const std::vector<T> & rhs);
      
      /**
       * Destructor
       **/
      virtual ~ThreadSafeVector (void);
      
      /**
       * Assignment operator
       * @param  rhs  a vector to copy
       **/
      void operator= (const ThreadSafeVector & rhs);
      
      /**
       * Assignment operator
       * @param  rhs  a vector to copy
       **/
      void operator= (const std::vector<T> & rhs);
      
      /**
       * Accesses an element of the vector
       * @param  index  index to access
       * @return the element at the index
       **/
      inline T & operator[] (size_t index);
      
      /**
       * Accesses an element of the vector
       * @param  index  index to access
       * @return the element at the index
       **/
      inline const T & operator[] (size_t index) const;
      
      /**
       * Erases an element
       * @param  index  index of element to erase
       * @return the new size of the vector
       **/
      inline size_t erase (size_t index);
      
      /**
       * Pushes a value onto the end of the vector
       * @param  value  a value to add to the end of the vector
       **/
      inline void push_back (T & value);
      
      /**
       * Returns the last element of the vector
       * @return  the last element of the vector
       **/
      inline const T & back (void) const;
      
      /**
       * Returns the last element of the vector
       * @return  the last element of the vector
       **/
      inline T & back (void);
      
      /**
       * Returns the last element before removing it
       * @return  the deleted element of the vector
       **/
      inline T pop_back (void);
      
      /**
       * Reserves a number of elements the vector
       * @param  new_size  the new size
       **/
      inline void reserve (size_t new_size) const;
      
      /**
       * Resizes the vector
       * @param  new_size  the new size
       **/
      inline void resize (size_t new_size) const;
      
      /**
       * returns the current size of the vector
       * @return  the current size
       **/
      inline size_t size (void) const;
      
      /**
       * returns the max size of the vector
       * @return  the max size
       **/
      inline size_t max_size (void) const;
      
      /**
       * Clears the vector
       **/
      inline void clear (void);
      
      /**
       * Locks the mutex
       **/
      inline void lock (void) const;
      
      /**
       * Unlocks the mutex
       **/
      inline void unlock (void) const;
      
      /**
       * Locks the mutex
       **/
      inline void acquire (void) const;
      
      /**
       * Unlocks the mutex
       **/
      inline void release (void) const;

    private:

      
      
      /// mutex for updating refcount_
      mutable MADARA_LOCK_TYPE mutex_;

      /// the encapsulated vector
      std::vector <T> vector_;
    };
  }
}
#include "madara/utility/ThreadSafeVector.cpp"

#endif /* _MADARA_UTILITY_THREADSAFE_VECTOR_H_ */
