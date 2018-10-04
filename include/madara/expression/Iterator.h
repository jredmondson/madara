/* -*- C++ -*- */
#ifndef _MADARA_TREE_ITERATOR_H_
#define _MADARA_TREE_ITERATOR_H_

#include <string>

#include "madara/utility/Refcounter.h"
#include "madara/expression/IteratorImpl.h"

namespace madara
{
namespace expression
{
// Forward declarations.
class ExpressionTreeCommandFactory;
class ComponentNode;

/**
 * @class ExpressionTreeIterator
 * @brief Non-const iterator for traversing an expression tree
 *
 * @see   ExpressionTreeLevelOrderIteratorImpl,
 * ExpressionTreeInOrderIteratorImpl, ExpressionTreePreOrderIteratorImpl, and
 * ExpressionTreePostOrderIteratorImpl.
 */
class ExpressionTreeIterator
{
public:
  /// Copy ctor - needed for reference counting.
  ExpressionTreeIterator(const ExpressionTreeIterator& ts);

  /// Copy ctor for impl *.
  ExpressionTreeIterator(ExpressionTreeIteratorImpl* impl);

  /// Dereference operator returns a reference to the item contained
  /// at the current position
  // @@ ExpressionTree operator* (void);
  ExpressionTree operator*(void);

  /// Returns a const reference to the item contained at the current position
  // @@ ExpressionTree operator* (void);
  const ExpressionTree operator*(void)const;

  /// Preincrement operator
  ExpressionTreeIterator& operator++(void);

  /// Postincrement operator.
  ExpressionTreeIterator operator++(int);

  /// Equality operator
  bool operator==(const ExpressionTreeIterator& rhs);

  /// In-equality operator
  bool operator!=(const ExpressionTreeIterator& rhs);

  // = Necessary traits
  typedef std::forward_iterator_tag iterator_category;
  typedef long value_type;
  typedef int* pointer;
  typedef int& reference;
  typedef int difference_type;

private:
  madara::utility::Refcounter<ExpressionTreeIteratorImpl> impl_;
};

/**
 * @class ExpressionTreeConstIterator
 * @brief Constant iterator over an expression tree
 *
 * @see   ExpressionTreeLevelOrderIteratorImpl,
 *        ExpressionTreeInOrderIteratorImpl,
 *        ExpressionTreePreOrderIteratorImpl,
 *        and ExpressionTreePostOrderIteratorImpl
 */
class ExpressionTreeConstIterator
{
public:
  /// Copy ctor - needed for reference counting.
  ExpressionTreeConstIterator(const ExpressionTreeConstIterator& ts);

  /// Copy ctor for impl *.
  ExpressionTreeConstIterator(ExpressionTreeIteratorImpl* impl);

  /// Returns a const reference to the item contained at the current position
  const ExpressionTree operator*(void)const;

  /// Preincrement operator
  ExpressionTreeConstIterator& operator++(void);

  /// Postincrement operator.
  ExpressionTreeConstIterator operator++(int);

  /// Equality operator
  bool operator==(const ExpressionTreeConstIterator& rhs);

  /// Nonequality operator
  bool operator!=(const ExpressionTreeConstIterator& rhs);

  // = Necessary traits
  typedef std::forward_iterator_tag iterator_category;
  typedef long value_type;
  typedef int* pointer;
  typedef int& reference;
  typedef int difference_type;

private:
  /// Pointer to actual implementation, i.e., the "bridge", which is
  /// reference counted to automate memory management.
  madara::utility::Refcounter<ExpressionTreeIteratorImpl> impl_;
};
}
}
#endif /* _MADARA_TREE_ITERATOR_H_ */
