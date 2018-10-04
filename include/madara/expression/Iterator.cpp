#ifndef _TREE_ITERATOR_CPP
#define _TREE_ITERATOR_CPP

#ifndef _MADARA_NO_KARL_

#include <stdlib.h>
#include <algorithm>
#include <sstream>

#include "madara/utility/Refcounter.h"
#include "madara/expression/Iterator.h"
#include "madara/expression/ExpressionTree.h"

/// Copy ctor - needed for reference counting.

madara::expression::ExpressionTreeIterator::ExpressionTreeIterator(
    const ExpressionTreeIterator& ts)
  : impl_(ts.impl_)
{
}

/// Copy ctor for impl *.

madara::expression::ExpressionTreeIterator::ExpressionTreeIterator(
    ExpressionTreeIteratorImpl* impl)
  : impl_(impl)
{
}

/// Dereference operator returns a reference to the item contained at
/// the current position.

madara::expression::ExpressionTree madara::expression::ExpressionTreeIterator::
operator*(void)
{
  // return impl_->operator* ();
  return *(*impl_);
}

/// Returns a const reference to the item contained at the current position

const madara::expression::ExpressionTree
    madara::expression::ExpressionTreeIterator::operator*(void)const
{
  // return impl_->operator* ();
  return *(*impl_);
}

/// Pre-increment operator

madara::expression::ExpressionTreeIterator&
    madara::expression::ExpressionTreeIterator::operator++(void)
{
  // return impl_->operator++ ();
  ++(*impl_);
  return *this;
}

/// Post-increment operator

madara::expression::ExpressionTreeIterator
    madara::expression::ExpressionTreeIterator::operator++(int)
{
  ExpressionTreeIterator temp(impl_->clone());
  // return impl_->operator++ ();
  ++(*impl_);
  return temp;
}

/// Equality operator

bool madara::expression::ExpressionTreeIterator::operator==(
    const ExpressionTreeIterator& rhs)
{
  // return impl_->operator () == rhs.impl_->operator ();
  return *impl_ == *rhs.impl_;
}

/// Nonequality operator

bool madara::expression::ExpressionTreeIterator::operator!=(
    const ExpressionTreeIterator& rhs)
{
  return !(*this == rhs);
}

/// Copy ctor - needed for reference counting.

madara::expression::ExpressionTreeConstIterator::ExpressionTreeConstIterator(
    const ExpressionTreeConstIterator& ts)
  : impl_(ts.impl_)
{
}

/// Copy ctor for impl *.

madara::expression::ExpressionTreeConstIterator::ExpressionTreeConstIterator(
    ExpressionTreeIteratorImpl* impl)
  : impl_(impl)
{
}

/// Returns a const reference to the item contained at the current position

const madara::expression::ExpressionTree
    madara::expression::ExpressionTreeConstIterator::operator*(void)const
{
  return *(*impl_);
}

/// Preincrement operator

madara::expression::ExpressionTreeConstIterator&
    madara::expression::ExpressionTreeConstIterator::operator++(void)
{
  ++(*impl_);
  return *this;
}

/// Preincrement operator

madara::expression::ExpressionTreeConstIterator
    madara::expression::ExpressionTreeConstIterator::operator++(int)
{
  ExpressionTreeConstIterator temp(impl_.get_ptr()->clone());
  ++(*impl_);
  return *this;
}

/// Equality operator

bool madara::expression::ExpressionTreeConstIterator::operator==(
    const ExpressionTreeConstIterator& rhs)
{
  return impl_->operator==(*rhs.impl_);
}

/// Nonequality operator

bool madara::expression::ExpressionTreeConstIterator::operator!=(
    const ExpressionTreeConstIterator& rhs)
{
  return !(*this == rhs);
}

#endif  // _MADARA_NO_KARL_

#endif /* _TREE_ITERATOR_CPP */
