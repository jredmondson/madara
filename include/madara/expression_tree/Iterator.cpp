#ifndef _TREE_ITERATOR_CPP
#define _TREE_ITERATOR_CPP

#ifndef _MADARA_NO_KARL_

#include <stdlib.h>
#include <algorithm>
#include <sstream>

#include "madara/utility/Refcounter.h"
#include "madara/expression_tree/Iterator.h"
#include "madara/expression_tree/Expression_Tree.h"

/// Copy ctor - needed for reference counting.

Madara::Expression_Tree::Expression_Tree_Iterator::Expression_Tree_Iterator (
  const Expression_Tree_Iterator &ts)
  : impl_ (ts.impl_)
{

}
  
/// Copy ctor for impl *.

Madara::Expression_Tree::Expression_Tree_Iterator::Expression_Tree_Iterator (
  Expression_Tree_Iterator_Impl *impl)
  : impl_ (impl)
{
}

/// Dereference operator returns a reference to the item contained at
/// the current position.

Madara::Expression_Tree::Expression_Tree 
Madara::Expression_Tree::Expression_Tree_Iterator::operator* (void)
{
  // return impl_->operator* ();
  return *(*impl_);
}

/// Returns a const reference to the item contained at the current position

const Madara::Expression_Tree::Expression_Tree 
Madara::Expression_Tree::Expression_Tree_Iterator::operator* (void) const
{
  // return impl_->operator* ();
  return *(*impl_);
}

/// Pre-increment operator

Madara::Expression_Tree::Expression_Tree_Iterator &
Madara::Expression_Tree::Expression_Tree_Iterator::operator++ (void)
{
  // return impl_->operator++ ();
  ++(*impl_);
  return *this;
}

/// Post-increment operator

Madara::Expression_Tree::Expression_Tree_Iterator 
Madara::Expression_Tree::Expression_Tree_Iterator::operator++ (int)
{
  Expression_Tree_Iterator temp (impl_->clone ());
  // return impl_->operator++ ();
  ++(*impl_);
  return temp;
}

/// Equality operator

bool 
Madara::Expression_Tree::Expression_Tree_Iterator::operator== (
  const Expression_Tree_Iterator &rhs)
{
  // return impl_->operator () == rhs.impl_->operator ();
  return *impl_ == *rhs.impl_;
}

/// Nonequality operator

bool 
Madara::Expression_Tree::Expression_Tree_Iterator::operator!= (
  const Expression_Tree_Iterator &rhs)
{
  return !(*this == rhs);
}

/// Copy ctor - needed for reference counting.

Madara::Expression_Tree::Expression_Tree_Const_Iterator::Expression_Tree_Const_Iterator (
  const Expression_Tree_Const_Iterator &ts)
  : impl_ (ts.impl_)
{
}
  
/// Copy ctor for impl *.

Madara::Expression_Tree::Expression_Tree_Const_Iterator::Expression_Tree_Const_Iterator (
  Expression_Tree_Iterator_Impl * impl)
  : impl_ (impl)
{
}

/// Returns a const reference to the item contained at the current position

const Madara::Expression_Tree::Expression_Tree 
Madara::Expression_Tree::Expression_Tree_Const_Iterator::operator* (void) const
{
  return *(*impl_);
}

/// Preincrement operator

Madara::Expression_Tree::Expression_Tree_Const_Iterator &
Madara::Expression_Tree::Expression_Tree_Const_Iterator::operator++ (void)
{
  ++(*impl_);
  return *this;
}

/// Preincrement operator

Madara::Expression_Tree::Expression_Tree_Const_Iterator 
Madara::Expression_Tree::Expression_Tree_Const_Iterator::operator++ (int)
{
  Expression_Tree_Const_Iterator temp (impl_.get_ptr ()->clone ());
  ++(*impl_);
  return *this;
}

/// Equality operator

bool 
Madara::Expression_Tree::Expression_Tree_Const_Iterator::operator== (
  const Expression_Tree_Const_Iterator &rhs)
{
  return impl_->operator == (*rhs.impl_);
}

/// Nonequality operator

bool 
Madara::Expression_Tree::Expression_Tree_Const_Iterator::operator!= (
  const Expression_Tree_Const_Iterator &rhs)
{
  return !(*this == rhs);
}

#endif // _MADARA_NO_KARL_

#endif /* _TREE_ITERATOR_CPP */
