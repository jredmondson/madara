/* -*- C++ -*- */
#ifndef _MADARA_TREE_ITERATOR_IMPL_H_
#define _MADARA_TREE_ITERATOR_IMPL_H_

#include <stdlib.h>
#include <stdexcept>

#include "madara/utility/LStack.h"
#include "madara/utility/LQueue.h"
#include "madara/knowledge/KnowledgeRecord.h"

namespace madara
{
namespace utility
{
template<typename T>
class Refcounter;
}

namespace expression
{
class ComponentNode;
class ExpressionTreeIterator;
class ExpressionTree;

/**
 * @class ExpressionTreeIteratorImpl
 * @brief Implementation of the ExpressionTreeIterator pattern that
 *        is used to define the various iterations algorithms that can be
 *        performed to traverse the expression tree.
 *
 *        Plays the role of the "implementor" base class in the Bridge
 *        pattern that is used as the basis for the subclasses that
 *        actually define the various iteration algorithms.
 *
 * @see   ExpressionTreeLevelOrderIteratorImpl,
 *        ExpressionTreeInOrderIteratorImpl,
 *        ExpressionTreePreOrderIteratorImpl,
 *        ExpressionTreePostOrderIteratorImpl
 */
class ExpressionTreeIteratorImpl
{
  friend class ExpressionTreeIterator;

public:
  /// Construct an ExpressionTreeIteratorImpl to iterate over a tree.
  ExpressionTreeIteratorImpl(const ExpressionTree& tree);

  /// Dtor.
  virtual ~ExpressionTreeIteratorImpl(void);

  /// Dereference operator returns a reference to the item contained
  /// at the current position.
  virtual ExpressionTree operator*(void) = 0;

  /// Returns a const reference to the item contained at the current
  /// position.
  virtual const ExpressionTree operator*(void)const = 0;

  /// Increment operator (used for both pre- and post-increment).
  virtual void operator++(void) = 0;

  /// Equality operator.
  virtual bool operator==(const ExpressionTreeIteratorImpl& rhs) const = 0;

  /// Nonequality operator.
  virtual bool operator!=(const ExpressionTreeIteratorImpl& rhs) const = 0;

  /// Method for cloning an impl. Necessary for post increments.
  virtual ExpressionTreeIteratorImpl* clone(void) = 0;

  /// = Necessary traits
  typedef ::std::forward_iterator_tag iterator_category;
  typedef madara::knowledge::KnowledgeRecord value_type;
  typedef int* pointer;
  typedef int& reference;
  typedef int difference_type;

protected:
  /// The tree we are iterating over.
  const ExpressionTree& tree_;
};

/**
 * @class InOrderIteratorImpl
 * @brief Iterates through an @a ExpressionTree in in-order.
 *
 *        Plays the role of the "implementor" class in the Bridge
 *        pattern that defines the in-order iteration algorithm.
 */
class InOrderIteratorImpl : public ExpressionTreeIteratorImpl
{
  friend class ExpressionTreeIterator;
  friend class utility::Refcounter<ExpressionTreeIteratorImpl>;

public:
  /// Construct an InOrderIteratorImpl. If end_iter
  /// is set to true, the iterator points to the end of the
  /// tree. Otherwise, the iterator starts with a free tree.
  InOrderIteratorImpl(const ExpressionTree& tree, bool end_iter = false);

  /// Dtor.
  virtual ~InOrderIteratorImpl(void);

  /// Dereference operator returns a reference to the item contained
  /// at the current position.
  virtual ExpressionTree operator*(void);

  /// Returns a const reference to the item contained at the current
  /// position.
  virtual const ExpressionTree operator*(void)const;

  /// Increment operator (used for both pre- and post-increment).
  virtual void operator++(void);

  /// Equality operator.
  virtual bool operator==(const ExpressionTreeIteratorImpl& rhs) const;

  /// Nonequality operator.
  virtual bool operator!=(const ExpressionTreeIteratorImpl& lhs) const;

  /// Method for cloning an impl. Necessary for post increments.
  virtual ExpressionTreeIteratorImpl* clone(void);

  // = Necessary traits
  typedef ::std::forward_iterator_tag iterator_category;
  typedef madara::knowledge::KnowledgeRecord value_type;
  typedef int* pointer;
  typedef int& reference;
  typedef int difference_type;

private:
  /// Our current position in the iteration.
  madara::utility::LStack<ExpressionTree> stack_;
};

/**
 * @class PreOrderIteratorImpl
 * @brief Iterates through an @a ExpressionTree in level-order.
 *
 *        Plays the role of the "implementor" class in the Bridge
 *        pattern that defines the level-order iteration algorithm.
 */
class PreOrderIteratorImpl : public ExpressionTreeIteratorImpl
{
  friend class ExpressionTreeIterator;
  friend class utility::Refcounter<ExpressionTreeIteratorImpl>;

public:
  /// Construct an LevelOrderExpressionTreeIterator. If end_iter
  /// is set to true, the iterator points to the end of the
  /// tree. Otherwise, the iterator starts with a free tree.
  PreOrderIteratorImpl(const ExpressionTree& tree, bool end_iter = false);

  /// Dtor.
  virtual ~PreOrderIteratorImpl();

  /// Dereference operator returns a reference to the item contained
  /// at the current position.
  virtual ExpressionTree operator*(void);

  /// Returns a const reference to the item contained at the current
  /// position.
  virtual const ExpressionTree operator*(void)const;

  /// Increment operator (used for both pre- and post-increment).
  virtual void operator++(void);

  /// Equality operator.
  virtual bool operator==(const ExpressionTreeIteratorImpl& rhs) const;

  /// Nonequality operator.
  virtual bool operator!=(const ExpressionTreeIteratorImpl& lhs) const;

  /// Method for cloning an impl. Necessary for post increments.
  virtual ExpressionTreeIteratorImpl* clone(void);

  // = Necessary traits
  typedef ::std::forward_iterator_tag iterator_category;
  typedef madara::knowledge::KnowledgeRecord value_type;
  typedef int* pointer;
  typedef int& reference;
  typedef int difference_type;

private:
  /// Our current position in the iteration.
  madara::utility::LStack<ExpressionTree> stack_;
};

/**
 * @class PostOrderIteratorImpl
 * @brief Iterates through an @a ExpressionTree in post-order.
 *
 *        Plays the role of the "implementor" class in the Bridge
 *        pattern that defines the post-order iteration algorithm.
 */
class PostOrderIteratorImpl : public ExpressionTreeIteratorImpl
{
  friend class ExpressionTreeIterator;
  friend class utility::Refcounter<ExpressionTreeIteratorImpl>;

public:
  /// Construct an PostOrderIteratorImpl. If end_iter is set
  /// to true, the iterator points to the end of the tree. Otherwise,
  /// the iterator starts with a free tree.
  PostOrderIteratorImpl(const ExpressionTree& tree, bool end_iter = false);

  /// Dtor.
  virtual ~PostOrderIteratorImpl(void);

  /// Dereference operator returns a reference to the item contained
  /// at the current position.
  virtual ExpressionTree operator*(void);

  /// Returns a const reference to the item contained at the current
  /// position.
  virtual const ExpressionTree operator*(void)const;

  /// Increment operator (used for both pre- and post-increment).
  virtual void operator++(void);

  /// Equality operator.
  virtual bool operator==(const ExpressionTreeIteratorImpl& rhs) const;

  /// Nonequality operator.
  virtual bool operator!=(const ExpressionTreeIteratorImpl& lhs) const;

  /// Method for cloning an impl. Necessary for post increments.
  virtual ExpressionTreeIteratorImpl* clone(void);

  // = Necessary traits
  typedef ::std::forward_iterator_tag iterator_category;
  typedef madara::knowledge::KnowledgeRecord value_type;
  typedef int* pointer;
  typedef int& reference;
  typedef int difference_type;

private:
  /// Our current position in the iteration.
  madara::utility::LStack<ExpressionTree> stack_;
};

/**
 * @class LevelOrderExpressionTreeIteratorImpl
 * @brief Iterates through an @a ExpressionTree in level-order.
 *
 *        Plays the role of the "implementor" class in the Bridge
 *        pattern that defines the post-order iteration algorithm.
 */
class LevelOrderExpressionTreeIteratorImpl : public ExpressionTreeIteratorImpl
{
  friend class ExpressionTreeIterator;
  friend class utility::Refcounter<ExpressionTreeIteratorImpl>;

public:
  /// Construct an LevelOrderExpressionTreeIterator. If end_iter
  /// is set to true, the iterator points to the end of the tree.
  /// Otherwise, the iterator starts with a free tree.
  LevelOrderExpressionTreeIteratorImpl(
      const ExpressionTree& tree, bool end_iter = false);

  /// Dtor.
  virtual ~LevelOrderExpressionTreeIteratorImpl();

  /// Dereference operator returns a reference to the item contained
  /// at the current position.
  virtual ExpressionTree operator*(void);

  /// Returns a const reference to the item contained at the current
  /// position.
  virtual const ExpressionTree operator*(void)const;

  /// Increment operator (used for both pre- and post-increment).
  virtual void operator++(void);

  /// Equality operator.
  virtual bool operator==(const ExpressionTreeIteratorImpl& rhs) const;

  /// Nonequality operator.
  virtual bool operator!=(const ExpressionTreeIteratorImpl& lhs) const;

  /// Method for cloning an impl. Necessary for post increments.
  virtual ExpressionTreeIteratorImpl* clone(void);

  // = Necessary traits
  typedef ::std::forward_iterator_tag iterator_category;
  typedef madara::knowledge::KnowledgeRecord value_type;
  typedef int* pointer;
  typedef int& reference;
  typedef int difference_type;

private:
  /// Our current position in the iteration.
  // @@ Consider zapping this.
  madara::utility::LQueue<ExpressionTree> queue_;
};
}
}
#endif /* _MADARA_TREE_ITERATOR_IMPL_H_ */
