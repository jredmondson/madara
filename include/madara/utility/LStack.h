/* -*- C++ -*- */
#ifndef _MADARA_LSTACK_H_
#define _MADARA_LSTACK_H_

// This header defines "size_t"
#include <stdlib.h>
#include <stdexcept>

namespace madara
{
  namespace utility
  {
    // Solve circular include problem via forward decls.
    template <typename T>
    class LStackNode;

    template <typename T>
    class LStackIterator;

    template <typename T>
    class LStackConstIterator;

    /**
    * @class LStack
    * @brief Defines a generic "last-in/first-out" (LIFO) Abstract Data
    *        Type (ADT) using a stack that's implemented as a linked list.
    */
    template <class T> 
    class LStack
    {
      friend class LStackIterator<T>;
      friend class LStackConstIterator<T>;
    public:
      // Define a "trait"
      typedef T value_type;

      /**
      * @class Underflow
      * @brief Exception thrown by methods in this class when an
      *        underflow condition occurs. 
      */
      class Underflow {};

      /**
      * @class Overflow.
      * @brief Exception thrown by methods in this class when an overflow
      *        condition occurs.  
      */
      class Overflow {};

      /// Constructor.
      LStack (size_t size_hint = 0);

      /// Copy constructor.
      LStack (const LStack<T> &rhs);

      /// Assignment operator.
      LStack<T> &operator = (const LStack<T> &rhs);

      /// Perform actions needed when stack goes out of scope. 
      ~LStack (void);

      /// Place a @a new_item at the tail of the stack.  Throws the @a
      /// Overflow exception if the stack is full, e.g., if memory is
      /// exhausted.
      void push (const T &new_item);

      /// Remove and return the front item on the stack.  Throws the @a
      /// Underflow exception if the stack is empty.
      T pop (void);

      /// Returns the front stack item without removing it.  Throws the @a
      /// Underflow exception if the stack is empty.
      T top (void) const;

      /// Returns 1 if the stack is empty, otherwise returns 0. 
      bool is_empty (void) const;

      /// Returns 1 if the stack is full, otherwise returns 0. 
      bool is_full (void) const;

      /// Returns the current number of elements in the stack.
      size_t size (void) const;

      /// Compare this stack with @a rhs for equality.  Returns true if
      /// the size's of the two stacks are equal and all the elements from
      /// 0 .. size() are equal, else false.
      bool operator== (const LStack<T> &rhs) const;

      // Compare this stack with @a rhs for inequality such that @a
      // *this!=s is always the complement of the boolean return value of
      // @a *this==s.
      bool operator!= (const LStack<T> &s) const;

      /// Delete all the nodes in the LStack.
      void erase (void);

      typedef LStackIterator<T> iterator;
      typedef LStackConstIterator<T> const_iterator;

      /// Get an iterator that points to the beginning of the stack.
      iterator begin (void);

      /// Get a const iterator that points to the beginning of the stack.
      const_iterator begin (void) const;

      /// Get an iterator that points to the end of the stack.
      iterator end (void);

      /// Get a const iterator that points to the end of the stack.
      const_iterator end (void) const;

    protected:
      /// Remove the front item on the stack.  does not throw exceptions.
      void pop_i (void);

      /// Copy a linked list of nodes.  This can throw a bad_alloc
      /// exception.
      void copy_list (const LStack<T> &rhs);

      /// Delete a linked list of nodes.
      void delete_list (void);

    private:
      /// We only need to keep a single pointer for the circular linked
      /// list.  This points to the tail of the stack.  Since the list is
      /// circular, the head of the stack is always @a head_->next_.
      LStackNode<T> *head_;

      /// Number of items that are currently in the stack.
      size_t count_;
    };

    /**
    * @class LStackIterator
    * @brief Implements a forward iterator for LStack type classes.
    *
    * Note:  Having a const ExpressionTreeIterator does not guarantee that the current
    * *position* that it points to will not change, it only guarantees that
    * you cannot change the underlying stack!
    */
    template <typename T>
    class LStackIterator 
    {
    public:
      /// Construct an LStackIterator at position pos.  
      LStackIterator (LStack<T> &stack,
        size_t pos = 0);

      /// Construct an LStackIterator at node pos.
      LStackIterator (LStack<T> &stack,
        LStackNode<T> *pos = 0);

      /// Dereference operator returns a reference to the item contained
      /// at the current position
      T& operator* (void);

      /// Returns a const reference to the item contained at the current position
      const T& operator* (void) const;

      /// Preincrement operator
      LStackIterator<T> &operator++ (void);

      /// Postincrement operator
      LStackIterator<T> operator++ (int);

      /// Equality operator
      bool operator== (const LStackIterator<T> &rhs) const;

      /// Nonequality operator
      bool operator!= (const LStackIterator<T> &lhs) const;

      // = Necessary traits
      typedef ::std::forward_iterator_tag iterator_category;
      typedef T value_type;
      typedef T *pointer;
      typedef T &reference;
      typedef int difference_type;

    private:
      /// the stack we are dealing with
      LStack<T> &stack_;

      // the position in the linked list
      mutable LStackNode<T> *pos_;
    };

    /**
    * @class LStackConstIterator
    * @brief Implements a forward iterator for LStack type classes.
    *
    * Note:  Having a const ExpressionTreeIterator does not guarantee that the current
    * *position* that it points to will not change, it only guarantees that
    * you cannot change the underlying stack!
    */
    template <typename T>
    class LStackConstIterator 
    {
    public:
      /// Construct an LStackIterator at position pos.  
      LStackConstIterator (const LStack<T> &stack,
        size_t pos = 0);

      /// Construct an LStackIterator at node pos.  
      LStackConstIterator (const LStack<T> &stack,
        LStackNode<T> *pos);

      /// Dereference operator returns a const reference to the item
      /// contained at the current position.
      const T& operator* (void) const;

      /// Preincrement operator
      const LStackConstIterator<T> &operator++ (void) const;

      /// Postincrement operator
      LStackConstIterator<T> operator++ (int) const;

      /// Equality operator
      bool operator== (const LStackConstIterator<T> &rhs) const;

      /// Nonequality operator
      bool operator!= (const LStackConstIterator<T> &lhs) const;

      // = Necessary traits
      typedef ::std::forward_iterator_tag iterator_category;
      typedef T value_type;
      typedef T *pointer;
      typedef T &reference;
      typedef int difference_type;

    private:
      /// the stack we are dealing with
      const LStack<T> &stack_;

      // the position in the linked list
      mutable LStackNode<T> *pos_;
    };
  }
}

#include "LStack.cpp"

#endif /* _MADARA_LSTACK_H_ */
