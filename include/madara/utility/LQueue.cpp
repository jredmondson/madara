#ifndef _LQUEUE_CPP
#define _LQUEUE_CPP

#include <memory>
#include "madara/utility/LQueue.h"

#ifdef _MSC_VER
// It's fine to use "this" in base-member initializations.
#pragma warning(disable:4355)
#endif

namespace madara
{
  namespace utility
  {
    /**
     * @class LQueueNode
     * @brief Defines a node in the @a LQueue that's implemented as a circular linked list.
     */
    template <class T>
    class LQueueNode
    {
      friend class LQueue<T>;
      friend class LQueueIterator<T>;
      friend class LQueueConstIterator<T>;
    public:
      /// Constructor.
      LQueueNode (const T &item,
                   LQueueNode<T> *next = 0);

      /// Constructor.
      LQueueNode (LQueueNode<T> *next);

      /// Default constructor that doesn't initialize item_.
      LQueueNode (void);

      /// Allocate a new LQueueNode, trying first from the @a free_list_
      /// and if that's empty try from the global operator new.
      void *operator new (size_t bytes);

      /// Return @a ptr to the @a free_list_.
      void operator delete (void *ptr);

      /// Return the next node to which this node points.
      LQueueNode *next (void);

      /// Preallocate n @a LQueueNodes and store them on the @a
      /// free_list_.
      static void free_list_allocate (size_t n);

      /// Returns all dynamic memory on the free list to the free store.
      static void free_list_release (void);

    private:

      /// Head of the free list, which is a stack of @a LQueueNodes
      /// used to speed up allocation.
      static LQueueNode<T> *free_list_;

      /// Item in this node.
      T item_;

      /// Pointer to the next node.
      LQueueNode<T> *next_;

    };
  }
}

/* static */
template <class T> 
madara::utility::LQueueNode<T> *
madara::utility::LQueueNode<T>::free_list_ = 0;

// Allocate a new <LQueueNode>, trying first from the
// <free_list_> and if that's empty try from the global <::operator
// new>.

template <class T> 
void *
madara::utility::LQueueNode<T>::operator new (size_t)
{
  // extract element from the free_list_ if there is one left
  if (LQueueNode<T>::free_list_ != 0)
    {
      // get the top element of the list
      LQueueNode<T>* new_node = LQueueNode<T>::free_list_;

      // "remove" the element from the list and pass it to the caller
      LQueueNode<T>::free_list_ = new_node->next_;

      return new_node;
    }

  return ::operator new(sizeof (LQueueNode<T>));
}

// Return <ptr> to the <free_list_>.

template <class T> void
madara::utility::LQueueNode<T>::operator delete (void * ptr)
{
  // do nothing on a null pointer
  if (ptr != 0)
    {
      // cast to a node pointer
      LQueueNode<T>* node = static_cast<LQueueNode<T>*> (ptr);

      // put the node back into the list
      node->next_ = LQueueNode<T>::free_list_;

      LQueueNode<T>::free_list_ = node;
    }
}

// Returns the next node to which this node points.
template <class T> 
madara::utility::LQueueNode<T> *
madara::utility::LQueueNode<T>::next (void)
{
  return next_;
}

// Returns all dynamic memory on the free list to the free store.

template <class T>
void 
madara::utility::LQueueNode<T>::free_list_release (void)
{
  // delete free list element by element
  while (LQueueNode<T>::free_list_ != 0)
    {
      LQueueNode<T>* node = LQueueNode<T>::free_list_;
      LQueueNode<T>::free_list_ = node->next_;
      ::operator delete (node);
    }
}

// Preallocate <n> <LQueueNodes> and store them on the
// <free_list_>.

template <class T> void 
madara::utility::LQueueNode<T>::free_list_allocate (size_t n)
{
  // add a new element to the stack n times
  for (size_t node_number = 0; node_number < n; ++node_number)
    {
      // create a new element avoiding the overwritten new operator
      LQueueNode<T>* new_node = 
  reinterpret_cast<LQueueNode<T>*> (
    ::operator new (sizeof (LQueueNode<T>)));

      new_node->next_ = LQueueNode<T>::free_list_;
    
      // make the new element the top of the list
      LQueueNode<T>::free_list_ = new_node;
    }
}

template <class T>
madara::utility::LQueueNode<T>::LQueueNode (const T &item,
                             madara::utility::LQueueNode<T> *next) 
  : item_ (item),
    next_ (next)
{
}

template <class T>
madara::utility::LQueueNode<T>::LQueueNode (
  madara::utility::LQueueNode<T> *next) 
  : next_ (next)
{
}

// This method is helpful to implement the dummy node in a concise
// way.
template <class T>
madara::utility::LQueueNode<T>::LQueueNode (void)
  : next_ (this)
{
}

// Returns the current size.
template <class T> size_t 
madara::utility::LQueue<T>::size (void) const
{
  return count_;
}

// Constructor.

template <class T>
madara::utility::LQueue<T>::LQueue (size_t size_hint)
  // Initialize fields here.
  : tail_ (0),
    count_ (0)
{
  // use the size_hint to preallocate  memory for nodes
  LQueueNode<T>::free_list_allocate (size_hint);

  // create the dummy node
  tail_ = new LQueueNode<T> ();
}

// Copy constructor.

template <class T>
madara::utility::LQueue<T>::LQueue (
         const madara::utility::LQueue<T> &rhs)
  // Initialize fields here.
  : tail_ (0),
    count_ (0) // count_ will be set correctly by copy_list
{
  // insert a dummy node first and keep it as an auto_ptr for exception
  // safety issues
  ::std::unique_ptr <LQueueNode<T> > tail (new LQueueNode<T> ());
  tail_ = tail.get ();

  // copy_list has strong exception safety, so no try catch block
  // is necessary here
  copy_list (rhs);

  // from here on, the auto_ptr should not try to delete the 
  // tail pointer anymore.
  tail.release ();
}

// Copy a linked list of nodes
template <class T> void
madara::utility::LQueue<T>::copy_list (
        const madara::utility::LQueue<T> &rhs)
{
  LQueue<T> temp;

  // enqueue the elements into the temporary list
  for (typename LQueue<T>::const_iterator it = rhs.begin (); 
       it != rhs.end (); 
       ++it)
    {
      temp.enqueue (*it);
    }

  // we only swap the lists if the temporary list has been successfully
  // created. This ensures strong exception guarantees.
  ::std::swap (tail_, temp.tail_);

  // swap the counts too
  ::std::swap (count_, temp.count_);
}

// Delete a linked list of nodes
template <class T> void
madara::utility::LQueue<T>::delete_list ()
{
  // we do not delete the dummy node here. This will be done in the destructor
  // we dequeue all elements until the queue is empty again
  while (!is_empty ())
    {
      dequeue_i ();
    }
}

// Assignment operator.
template <class T> 
madara::utility::LQueue<T> &
madara::utility::LQueue<T>::operator= (
            const madara::utility::LQueue<T> &rhs) 
{
  // test for self assignment first
  if (this != &rhs)
    {
      // delete old data of the rhs
      delete_list ();

      // copy new data
      copy_list (rhs);
    }

  return *this;
}

// Perform actions needed when queue goes out of scope.

template <class T>
madara::utility::LQueue<T>::~LQueue (void)
{
  // delete all elements of the list
  delete_list ();

  // delete the last dummy node here
  delete tail_;
}

// Compare this queue with <rhs> for equality.  Returns true if the
// size()'s of the two queues are equal and all the elements from 0
// .. size() are equal, else false.
template <class T> bool 
madara::utility::LQueue<T>::operator== (
            const madara::utility::LQueue<T> &rhs) const
{
  return (size () == rhs.size ()) &&
          ::std::equal (begin (), end (), rhs.begin ());
}

// Compare this queue with <rhs> for inequality such that <*this> !=
// <s> is always the complement of the boolean return value of
// <*this> == <s>.

template <class T> bool 
madara::utility::LQueue<T>::operator!= (
          const madara::utility::LQueue<T> &rhs) const
{
  return !(*this == rhs);
}

// Place a <new_item> at the tail of the queue.  Throws
// the <Overflow> exception if the queue is full.

template <class T> void
madara::utility::LQueue<T>::enqueue (const T &new_item)
{
  try
    {
      // assign the value to the tail element before the new allocation
      // to make sure that exceptions thrown in the assignment operator
      // of T does leave the queue structure altered
      tail_->item_ = new_item;

      // integrate the new node into the list
      tail_->next_ = new LQueueNode<T> (tail_->next_);
      tail_ = tail_->next_;

      // increment the element count
      ++count_;
    }
  catch (const ::std::bad_alloc&)
    {
      // we transform a bad_alloc excption into an overflow exception,
      // because it basically means, that it is no longer possible
      // to enqueue new elements
      throw Overflow ();
    }
}

// Remove and return the front item on the queue.  
// Throws the <Underflow> exception if the queue is empty. 

template <class T> T 
madara::utility::LQueue<T>::dequeue (void)
{
  // check for empty queue first
  if (is_empty ())
    {
      throw Underflow ();
    }

  // extract the value of the head node. This is done before we actually
  // remove the element for exceptions could be thrown in the assignment
  // operator.
  T item = tail_->next_->item_;  

  // call actual dequeue implementation
  dequeue_i ();

  return item;
}

template <class T> 
void
madara::utility::LQueue<T>::dequeue_i (void)
{
  // remember the current queue head
  LQueueNode<T>* head = tail_->next_;

  // remove the head from the queue
  tail_->next_ = head->next_;

  // decrement the element count
  --count_;

  // delete the old head node
  delete head;
}

// Returns the front queue item without removing it. 
// Throws the <Underflow> exception if the queue is empty. 

template <class T> 
T 
madara::utility::LQueue<T>::front (void) const
{
  // check for empty queue first
  if (is_empty())
    throw Underflow ();

  // return the item in head
  return tail_->next_->item_;
}

// Returns true if the queue is empty, otherwise returns false.

template <class T> 
bool
madara::utility::LQueue<T>::is_empty (void) const 
{
  return count_ == 0;
}

// Returns true if the queue is full, otherwise returns false.

template <class T> bool
madara::utility::LQueue<T>::is_full (void) const 
{
  // there is no upper limit for the queue
  return false;
}

// Get an iterator to the begining of the queue
template <typename T> 
typename madara::utility::LQueue<T>::iterator
madara::utility::LQueue<T>::begin (void)
{
  // iterator starts at the head element
  return typename LQueue<T>::iterator (*this, tail_->next_);
}

// Get an iterator pointing past the end of the queue
template <typename T> 
typename madara::utility::LQueue<T>::iterator
madara::utility::LQueue<T>::end (void)
{
  // iterator starts at the tail element
  return typename LQueue<T>::iterator (*this, tail_);
}

// Get an iterator to the begining of the queue
template <typename T> 
typename madara::utility::LQueue<T>::const_iterator
madara::utility::LQueue<T>::begin (void) const
{
  // iterator starts at the head element
  return typename LQueue<T>::const_iterator (*this, tail_->next_);
}

// Get an iterator pointing past the end of the queue
template <typename T> 
typename madara::utility::LQueue<T>::const_iterator
madara::utility::LQueue<T>::end (void) const
{
  // iterator starts at the tail element
  return typename LQueue<T>::const_iterator (*this, tail_);
}

template <typename T> T &
madara::utility::LQueueIterator<T>::operator* (void)
{
  return pos_->item_;
}

template <typename T> const T &
madara::utility::LQueueIterator<T>::operator* (void) const
{
  return pos_->item_;
}

template <typename T> 
madara::utility::LQueueIterator<T> &
madara::utility::LQueueIterator<T>::operator++ (void) 
{
  // advance to the next position
  pos_ = pos_->next_;

  return *this;
}

// Post-increment.
template <typename T> 
madara::utility::LQueueIterator<T> 
madara::utility::LQueueIterator<T>::operator++ (int) 
{
  // keep copy of the original iterator
  LQueueIterator<T> copy = *this;

  // advance to the next position
  pos_ = pos_->next_;

  // return original iterator
  return copy;
}

template <typename T> bool
madara::utility::LQueueIterator<T>::operator== (
  const madara::utility::LQueueIterator<T> &rhs) const
{
  // check if the iterator points to the same position in the same queue
  // (we could even omit the check for queue equality, because it is
  //  very unlikely that two queues share the same node pointer)
  return (pos_ == rhs.pos_);
}

template <typename T> bool
madara::utility::LQueueIterator<T>::operator!= (
    const madara::utility::LQueueIterator<T> &rhs) const
{
  // implement != in terms of ==
  return !(*this == rhs);
}

template <typename T>
madara::utility::LQueueIterator<T>::LQueueIterator (
  madara::utility::LQueue<T> &queue, size_t pos)
  : queue_ (queue),
    pos_ (queue.tail_->next_)
{
  // iterator over the queue unto the right position
  // we save iterations for values > count_ by doing modulo calculations
  for (pos = pos % (queue_.count_ -1); 
       pos > 0;
       --pos)
    {
      // advance one position each time
      pos_ = pos_->next_;
    }
}

template <typename T>
madara::utility::LQueueIterator<T>::LQueueIterator (
  madara::utility::LQueue<T> &queue, madara::utility::LQueueNode<T> *pos)
  : queue_ (queue),
    pos_ (pos)
{
}

template <typename T> const T &
madara::utility::LQueueConstIterator<T>::operator* (void) const
{
  return pos_->item_;
}

template <typename T> const madara::utility::LQueueConstIterator<T> &
madara::utility::LQueueConstIterator<T>::operator++ (void) const
{
  // advance to the next position
  pos_ = pos_->next_;

  return *this;
}

template <typename T> madara::utility::LQueueConstIterator<T>
madara::utility::LQueueConstIterator<T>::operator++ (int) const
{
  // keep copy of the original iterator
  LQueueConstIterator<T> copy = *this;

  // advance to the next position
  pos_ = pos_->next_;

  // return original iterator
  return copy;
}

template <typename T> bool
madara::utility::LQueueConstIterator<T>::operator== (
  const madara::utility::LQueueConstIterator<T> &rhs) const
{
  // check if the iterator points to the same position in the same queue
  return (pos_ == rhs.pos_);
}

template <typename T> bool
madara::utility::LQueueConstIterator<T>::operator!= (
  const madara::utility::LQueueConstIterator<T> &rhs) const
{
  return !(*this == rhs);
}

template <typename T>
madara::utility::LQueueConstIterator<T>::LQueueConstIterator (
  const madara::utility::LQueue<T> &queue, size_t pos)
  : queue_ (queue),
    pos_ (queue.tail_->next_)
{
  // iterator over the queue unto the right position
  // we save iterations for values > count_ by doing modulo calculations
  for (pos = pos % (queue_.count_ -1); 
       pos > 0;
       --pos)
    {
      // advance one position each time
      pos_ = pos_->next_;
    }
}

template <typename T>
madara::utility::LQueueConstIterator<T>::LQueueConstIterator (
  const LQueue<T> &queue, LQueueNode<T> *pos)
  : queue_ (queue),
    pos_ (pos)
{
}

#endif /* _LQUEUE_CPP */
