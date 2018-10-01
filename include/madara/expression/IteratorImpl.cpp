/* -*- C++ -*- */
#ifndef _TREE_ITERATOR_IMPL_CPP
#define _TREE_ITERATOR_IMPL_CPP

#ifndef _MADARA_NO_KARL_

#include "madara/expression/ExpressionTree.h"
#include "madara/expression/IteratorImpl.h"
#include "madara/expression/ComponentNode.h"
#include "madara/utility/Refcounter.h"

const size_t LQUEUE_SIZE = 40;

// Constructor for ExpressionTreeIteratorImpl that takes a tree
// to iterate over

madara::expression::ExpressionTreeIteratorImpl::ExpressionTreeIteratorImpl(
    const ExpressionTree& tree)
  : tree_(tree)
{
}

// Destructor

madara::expression::ExpressionTreeIteratorImpl::~ExpressionTreeIteratorImpl(
    void)
{
}

/// Construct an InOrderIteratorImpl. If end_iter is set to true,
/// the iterator points to the end of the tree

madara::expression::InOrderIteratorImpl::InOrderIteratorImpl(
    const ExpressionTree& tree, bool end_iter)
  : ExpressionTreeIteratorImpl(tree), stack_()
{
  // if the caller doesn't want an end iterator, insert the root tree
  // into the queue.
  if (!end_iter && !this->tree_.is_null())
  {
    stack_.push(const_cast<ExpressionTree&>(tree));

    // start at the smallest element (left-most)
    while (!stack_.top().left().is_null())
      stack_.push(stack_.top().left());
  }
}

/// destructor - nothing to do

madara::expression::InOrderIteratorImpl::~InOrderIteratorImpl(void) {}

/// Returns the Node that the iterator is pointing to (non-const version)

madara::expression::ExpressionTree madara::expression::InOrderIteratorImpl::
operator*(void)
{
  return stack_.top();
}

/// Returns the Node that the iterator is pointing to (const version)

const madara::expression::ExpressionTree
    madara::expression::InOrderIteratorImpl::operator*(void)const
{
  return stack_.top();
}

/// moves the iterator to the next node (pre-increment)

void madara::expression::InOrderIteratorImpl::operator++(void)
{
  // we know that at this point there is no left () of top ()
  // because we would have already visited it.

  if (!stack_.is_empty())
  {
    // if we have nodes greater than ourselves
    if (!stack_.top().right().is_null())
    {
      // push the right child node onto the stack
      // and pop the old parent (it's been visited now)
      stack_.push(stack_.pop().right());

      // keep pushing until we get to the left most child
      while (!stack_.top().left().is_null())
        stack_.push(stack_.top().left());
    }
    else
      stack_.pop();
  }
}

/// checks two iterators for equality

bool madara::expression::InOrderIteratorImpl::operator==(
    const ExpressionTreeIteratorImpl& rhs) const
{
  const InOrderIteratorImpl* in_order_rhs =
      dynamic_cast<const InOrderIteratorImpl*>(&rhs);

  // if the rhs was not a level_order iterator then we've already
  // discovered the relation is false.

  if (in_order_rhs)
  {
    // Check if the container we are iterating over has the same
    // root node and that the size of the queues are equal. The
    // latter doesn't truly determine equality. However, this is an
    // easy check for determining most inequalities and it allows us
    // to assume the queue at least has a front node (coupled with
    // the is_empty () function later).

    ExpressionTree& t1 = const_cast<ExpressionTree&>(tree_);
    ExpressionTree& t2 = const_cast<ExpressionTree&>(in_order_rhs->tree_);

    if (t1.get_root() == t2.get_root() &&
        stack_.size() == in_order_rhs->stack_.size())
    {
      // Check for both being is_empty (special condition).
      if (stack_.is_empty() && in_order_rhs->stack_.is_empty())
        return true;

      // check the front's node pointer. If the node pointers are
      // equal, then both iterators are pointing to the same
      // position in the tree.

      if (stack_.top().get_root() == in_order_rhs->stack_.top().get_root())
        return true;
    }
  }

  // either we were trying to compare a non-level order iterator or we
  // were comparing two level order iterators that were obviously at
  // different points in the tree (their queue sizes were different)

  return false;
}

/// checks two iterators for inequality

bool madara::expression::InOrderIteratorImpl::operator!=(
    const ExpressionTreeIteratorImpl& rhs) const
{
  return !(*this == rhs);
}

/// Method for cloning an impl. Necessary for post increments (bridge)
/// @see ExpressionTreeIterator

madara::expression::ExpressionTreeIteratorImpl*
madara::expression::InOrderIteratorImpl::clone(void)
{
  return new InOrderIteratorImpl(*this);
}

/// Construct an PreOrderExpressionTreeIterator.
/// If end_iter is set to true, the iterator points to the end of the tree

madara::expression::PreOrderIteratorImpl::PreOrderIteratorImpl(
    const ExpressionTree& tree, bool end_iter)
  : ExpressionTreeIteratorImpl(tree), stack_()
{
  // if the caller doesn't want an end iterator, insert the root tree
  // into the queue.
  if (!end_iter && !this->tree_.is_null())
    stack_.push(const_cast<ExpressionTree&>(tree));
}

/// destructor - nothing to do

madara::expression::PreOrderIteratorImpl::~PreOrderIteratorImpl(void) {}

/// Returns the Node that the iterator is pointing to (non-const version)

madara::expression::ExpressionTree madara::expression::PreOrderIteratorImpl::
operator*(void)
{
  return stack_.top();
}

/// Returns the Node that the iterator is pointing to (const version)

const madara::expression::ExpressionTree
    madara::expression::PreOrderIteratorImpl::operator*(void)const
{
  return stack_.top();
}

/// moves the iterator to the next node (pre-increment)

void madara::expression::PreOrderIteratorImpl::operator++(void)
{
  // we know that at this point there is no left () of top ()
  // because we would have already visited it.

  if (!stack_.is_empty())
  {
    // we need to pop the node off the stack before pushing the
    // children, or else we'll revisit this node later

    ExpressionTree current = stack_.pop();

    // note the order here: right first, then left. Since this is
    // LIFO, this results in the left child being the first
    // evaluated, which fits into the Pre-order traversal strategy

    if (!current.right().is_null())
      stack_.push(current.right());
    if (!current.left().is_null())
      stack_.push(current.left());
  }
}

/// checks two iterators for equality

bool madara::expression::PreOrderIteratorImpl::operator==(
    const ExpressionTreeIteratorImpl& rhs) const
{
  const PreOrderIteratorImpl* pre_order_rhs =
      dynamic_cast<const PreOrderIteratorImpl*>(&rhs);

  // if the rhs was not a level_order iterator
  // then we've already discovered the relation is false

  if (pre_order_rhs)
  {
    // check if the container we are iterating over has the same
    // root node and that the size of the queues are equal. The
    // latter doesn't truly determine equality. However, this is an
    // easy check for determining most inequalities and it allows us
    // to assume the queue at least has a front node (coupled with
    // the is_empty () function later).

    ExpressionTree& t1 = const_cast<ExpressionTree&>(tree_);
    ExpressionTree& t2 = const_cast<ExpressionTree&>(pre_order_rhs->tree_);

    if (t1.get_root() == t2.get_root() &&
        stack_.size() == pre_order_rhs->stack_.size())
    {
      // check for both being is_empty (special condition)
      if (stack_.is_empty() && pre_order_rhs->stack_.is_empty())
        return true;

      // check the front's node pointer. If the node pointers
      // are equal, then both iterators are pointing to the same
      // position in the tree.

      if (stack_.top().get_root() == pre_order_rhs->stack_.top().get_root())
        return true;
    }
  }

  // either we were trying to compare a non-level order iterator or
  // we were comparing two level order iterators that were obviously
  // at different points in the tree (their queue sizes were different)

  return false;
}

/// checks two iterators for inequality

bool madara::expression::PreOrderIteratorImpl::operator!=(
    const ExpressionTreeIteratorImpl& rhs) const
{
  return !(*this == rhs);
}

/// Method for cloning an impl. Necessary for post increments (bridge)
/// @see ExpressionTreeIterator

madara::expression::ExpressionTreeIteratorImpl*
madara::expression::PreOrderIteratorImpl::clone(void)
{
  return new PreOrderIteratorImpl(*this);
}

/// Construct an PostOrderExpressionTreeIterator.
/// If end_iter is set to true, the iterator points to the end of the tree

madara::expression::PostOrderIteratorImpl::PostOrderIteratorImpl(
    const ExpressionTree& tree, bool end_iter)
  : ExpressionTreeIteratorImpl(tree), stack_()
{
  // if the caller doesn't want an end iterator, insert the root tree
  // into the queue.
  if (!end_iter && !this->tree_.is_null())
  {
    ExpressionTree current = const_cast<ExpressionTree&>(tree);
    stack_.push(current);

    // the commented code does not work on unary operator nodes with
    // no left child, but a right child - or at least, there is a
    // certain depth that this will not go down

    while (!current.is_null())
    {
      if (!current.right().is_null())
        stack_.push(current.right());
      if (!current.left().is_null())
      {
        // if there was a left, then update current
        // this is the case for all non-negations
        stack_.push(current.left());
        current = current.left();
      }
      else
        // if there was not a left, then current = current->right_
        // this handles cases of unary nodes, like negations
        current = current.right();
    }
  }
}

/// destructor - nothing to do

madara::expression::PostOrderIteratorImpl::~PostOrderIteratorImpl(void) {}

/// Returns the Node that the iterator is pointing to (non-const version)

madara::expression::ExpressionTree madara::expression::PostOrderIteratorImpl::
operator*(void)
{
  return stack_.top();
}

/// Returns the Node that the iterator is pointing to (const version)

const madara::expression::ExpressionTree
    madara::expression::PostOrderIteratorImpl::operator*(void)const
{
  return stack_.top();
}

/// moves the iterator to the next node (pre-increment)

void madara::expression::PostOrderIteratorImpl::operator++(void)
{
  // we know that at this point there is no left () of top ()
  // because we would have already visited it.

  if (!stack_.is_empty())
  {
    // we need to pop the node off the stack before pushing the
    // children, or else we'll revisit this node later

    ExpressionTree current = stack_.pop();

    // This is where stuff gets a little confusing.

    if (!stack_.is_empty() &&
        stack_.top().left().get_root() != current.get_root() &&
        stack_.top().right().get_root() != current.get_root())

    {
      current = stack_.top();

      while (!current.is_null())
      {
        if (!current.right().is_null())
          stack_.push(current.right());
        if (!current.left().is_null())
        {
          // if there was a left, then update current
          // this is the case for all non-negations
          stack_.push(current.left());
          current = current.left();
        }
        else
        {
          // if there was not a left, then current = current->right_
          // this handles cases of unary nodes, like negations
          current = current.right();
        }
      }
    }
  }
}

/// checks two iterators for equality

bool madara::expression::PostOrderIteratorImpl::operator==(
    const ExpressionTreeIteratorImpl& rhs) const
{
  const PostOrderIteratorImpl* post_order_rhs =
      dynamic_cast<const PostOrderIteratorImpl*>(&rhs);

  // if the rhs was not a level_order iterator
  // then we've already discovered the relation is false

  if (post_order_rhs)
  {
    // check if the container we are iterating over has the same
    // root node and that the size of the queues are equal. The
    // latter doesn't truly determine equality. However, this is an
    // easy check for determining most inequalities and it allows us
    // to assume the queue at least has a front node (coupled with
    // the is_empty () function later).

    ExpressionTree& t1 = const_cast<ExpressionTree&>(tree_);
    ExpressionTree& t2 = const_cast<ExpressionTree&>(post_order_rhs->tree_);

    if (t1.get_root() == t2.get_root() &&
        stack_.size() == post_order_rhs->stack_.size())
    {
      // check for both being is_empty (special condition)
      if (stack_.is_empty() && post_order_rhs->stack_.is_empty())
        return true;

      // check the front's node pointer. If the node pointers are
      // equal, then both iterators are pointing to the same
      // position in the tree.

      if (stack_.top().get_root() == post_order_rhs->stack_.top().get_root())
        return true;
    }
  }

  // either we were trying to compare a non-level order iterator or
  // we were comparing two level order iterators that were obviously
  // at different points in the tree (their queue sizes were different)

  return false;
}

/// checks two iterators for inequality

bool madara::expression::PostOrderIteratorImpl::operator!=(
    const ExpressionTreeIteratorImpl& rhs) const
{
  return !(*this == rhs);
}

/// Method for cloning an impl. Necessary for post increments (bridge)
/// @see ExpressionTreeIterator

madara::expression::ExpressionTreeIteratorImpl*
madara::expression::PostOrderIteratorImpl::clone(void)
{
  return new PostOrderIteratorImpl(*this);
}

/// Construct an LevelOrderExpressionTreeIterator. If end_iter is set to
/// true, the iterator points to the end of the tree

madara::expression::LevelOrderExpressionTreeIteratorImpl::
    LevelOrderExpressionTreeIteratorImpl(
        const ExpressionTree& tree, bool end_iter)
  : ExpressionTreeIteratorImpl(tree), queue_(LQUEUE_SIZE)
{
  // if the caller doesn't want an end iterator, insert the root tree
  // into the queue.
  if (!end_iter && !this->tree_.is_null())
    queue_.enqueue(const_cast<ExpressionTree&>(tree));
}

/// destructor - nothing to do

madara::expression::LevelOrderExpressionTreeIteratorImpl::
    ~LevelOrderExpressionTreeIteratorImpl(void)
{
}

/// Returns the Node that the iterator is pointing to (non-const version)

madara::expression::ExpressionTree
    madara::expression::LevelOrderExpressionTreeIteratorImpl::operator*(void)
{
  return queue_.front();
}

/// Returns the Node that the iterator is pointing to (const version)

const madara::expression::ExpressionTree
    madara::expression::LevelOrderExpressionTreeIteratorImpl::operator*(
        void)const
{
  return queue_.front();
}

/// moves the iterator to the next node (pre-increment)

void madara::expression::LevelOrderExpressionTreeIteratorImpl::operator++(void)
{
  if (!queue_.is_empty())
  {
    // If the queue is not empty, dequeue an element
    ExpressionTree root = queue_.dequeue();

    if (!root.is_null())
    {
      // If the element wasn't null, enqueue its children
      if (!root.left().is_null())
        queue_.enqueue(root.left());
      if (!root.right().is_null())
        queue_.enqueue(root.right());
    }
  }
}

/// checks two iterators for equality

bool madara::expression::LevelOrderExpressionTreeIteratorImpl::operator==(
    const ExpressionTreeIteratorImpl& rhs) const
{
  const LevelOrderExpressionTreeIteratorImpl* level_order_rhs =
      dynamic_cast<const LevelOrderExpressionTreeIteratorImpl*>(&rhs);

  // if the rhs was not a level_order iterator then we've already
  // discovered the relation is false.

  if (level_order_rhs)
  {
    // check if the container we are iterating over has the same
    // root node and that the size of the queues are equal. The
    // latter doesn't truly determine equality. However, this is an
    // easy check for determining most inequalities and it allows us
    // to assume the queue at least has a front node (coupled with
    // the is_empty () function later).

    ExpressionTree& t1 = const_cast<ExpressionTree&>(tree_);
    ExpressionTree& t2 = const_cast<ExpressionTree&>(level_order_rhs->tree_);

    if (t1.get_root() == t2.get_root() &&
        queue_.size() == level_order_rhs->queue_.size())
    {
      // check for both being is_empty (special condition)
      if (queue_.is_empty() && level_order_rhs->queue_.is_empty())
        return true;

      // check the front's node pointer. If the node pointers
      // are equal, then both iterators are pointing to the same
      // position in the tree.

      if (queue_.front().get_root() ==
          level_order_rhs->queue_.front().get_root())
        return true;
    }
  }

  // either we were trying to compare a non-level order iterator or we
  // were comparing two level order iterators that were obviously at
  // different points in the tree (their queue sizes were different)

  return false;
}

/// checks two iterators for inequality

bool madara::expression::LevelOrderExpressionTreeIteratorImpl::operator!=(
    const ExpressionTreeIteratorImpl& rhs) const
{
  return !(*this == rhs);
}

/// Method for cloning an impl. Necessary for post increments (bridge)
/// @see ExpressionTreeIterator

madara::expression::ExpressionTreeIteratorImpl*
madara::expression::LevelOrderExpressionTreeIteratorImpl::clone(void)
{
  return new LevelOrderExpressionTreeIteratorImpl(*this);
}

#endif  // _MADARA_NO_KARL_

#endif /* _TREE_ITERATOR_IMPL_CPP */
