/* -*- C++ -*- */
#ifndef _EXPRESSION_TREE_CPP_
#define _EXPRESSION_TREE_CPP_

#ifndef _MADARA_NO_KARL_

#include <stdlib.h>
#include <algorithm>
#include <sstream>
#include <string>
#include <map>
#include <stdexcept>

#include "madara/expression/ComponentNode.h"
#include "madara/expression/Iterator.h"
#include "madara/expression/IteratorImpl.h"
#include "madara/expression/ExpressionTree.h"
#include "madara/expression/LeafNode.h"

namespace madara
{
namespace expression
{
/**
 * @class ExpressionTreeIteratorFactory
 * @brief Implementation of a factory pattern that dynamically allocates
 *        the appropriate @a ExpressionTreeIteratorImpl object.
 *
 *        This is a variant of the Abstract Factory pattern that
 *        has a set of related factory methods but which doesn't use
 *        inheritance.
 *
 * @see   ExpressionTreeLevelOrderIteratorImpl,
 *        ExpressionTreeInOrderIteratorImpl,
 *        ExpressionTreePreOrderIteratorImpl, and
 *        ExpressionTreePostOrderIteratorImpl.
 */

enum {
  EQUALITY_CHAR = 0 - 1,
  INEQUALITY_CHAR = 0 - 2,
  GREATER_THAN_EQUAL = 0 - 3,
  LESSER_THAN_EQUAL = 0 - 4,
};

class ExpressionTreeIteratorFactory
{
public:
  /// Constructor.
  ExpressionTreeIteratorFactory(void);

  /// Dynamically allocate a new @a ExpressionTreeIteratorImpl
  /// object based on the designated @a traversal_order and @a end_iter.
  ExpressionTreeIteratorImpl* make_tree_iterator(
      ExpressionTree& tree, const std::string& traversal_order, bool end_iter);

private:
  /// Dynamically allocate a new @a ExpressionTreeLevelOrderIteratorImpl
  /// object based on the designated @a end_iter.
  ExpressionTreeIteratorImpl* make_in_order_tree_iterator(
      ExpressionTree& tree, bool end_iter);

  /// Dynamically allocate a new @a ExpressionTreePreOrderIteratorImpl
  /// object based on the designated @a end_iter.
  ExpressionTreeIteratorImpl* make_pre_order_tree_iterator(
      ExpressionTree& tree, bool end_iter);

  /// Dynamically allocate a new @a ExpressionTreePostOrderIteratorImpl
  /// object based on the designated @a end_iter.
  ExpressionTreeIteratorImpl* make_post_order_tree_iterator(
      ExpressionTree& tree, bool end_iter);

  /// Dynamically allocate a new @a ExpressionTreeLevelOrderIteratorImpl
  /// object based on the designated @a end_iter.
  ExpressionTreeIteratorImpl* make_level_order_tree_iterator(
      ExpressionTree& tree, bool end_iter);

  typedef ExpressionTreeIteratorImpl* (
      ExpressionTreeIteratorFactory::*TRAVERSAL_PTMF)(
      ExpressionTree& tree, bool end_iter);
  typedef ::std::map<std::string, TRAVERSAL_PTMF> TRAVERSAL_MAP;

  TRAVERSAL_MAP traversal_map_;
};
}
}

madara::expression::ExpressionTreeIteratorFactory::
    ExpressionTreeIteratorFactory(void)
{
  traversal_map_["in-order"] =
      &ExpressionTreeIteratorFactory::make_in_order_tree_iterator;
  traversal_map_["pre-order"] =
      &ExpressionTreeIteratorFactory::make_pre_order_tree_iterator;
  traversal_map_["post-order"] =
      &ExpressionTreeIteratorFactory::make_post_order_tree_iterator;
  traversal_map_["level-order"] =
      &ExpressionTreeIteratorFactory::make_level_order_tree_iterator;
}

madara::expression::ExpressionTreeIteratorImpl* madara::expression::
    ExpressionTreeIteratorFactory::make_level_order_tree_iterator(
        madara::expression::ExpressionTree& tree, bool end_iter)
{
  return new LevelOrderExpressionTreeIteratorImpl(tree, end_iter);
}

madara::expression::ExpressionTreeIteratorImpl*
madara::expression::ExpressionTreeIteratorFactory::make_in_order_tree_iterator(
    madara::expression::ExpressionTree& tree, bool end_iter)
{
  return new InOrderIteratorImpl(tree, end_iter);
}

madara::expression::ExpressionTreeIteratorImpl*
madara::expression::ExpressionTreeIteratorFactory::make_pre_order_tree_iterator(
    madara::expression::ExpressionTree& tree, bool end_iter)
{
  return new PreOrderIteratorImpl(tree, end_iter);
}

madara::expression::ExpressionTreeIteratorImpl* madara::expression::
    ExpressionTreeIteratorFactory::make_post_order_tree_iterator(
        ExpressionTree& tree, bool end_iter)
{
  return new PostOrderIteratorImpl(tree, end_iter);
}

madara::expression::ExpressionTreeIteratorImpl*
madara::expression::ExpressionTreeIteratorFactory::make_tree_iterator(
    madara::expression::ExpressionTree& tree,
    const std::string& traversal_order, bool end_iter)
{
  TRAVERSAL_MAP::iterator iter = traversal_map_.find(traversal_order);
  if (iter == traversal_map_.end()) {
    return 0;
  } else {
    ExpressionTreeIteratorFactory::TRAVERSAL_PTMF ptmf = iter->second;
    return (this->*ptmf)(tree, end_iter);
  }
}

// Define a single instance of a factory that's local to this class.
static madara::expression::ExpressionTreeIteratorFactory tree_iterator_factory;

// Default ctor

madara::expression::ExpressionTree::ExpressionTree(logger::Logger& logger)
  : logger_(&logger), root_(0)
{
}

// Ctor take an underlying NODE*.

madara::expression::ExpressionTree::ExpressionTree(logger::Logger& logger,
    madara::expression::ComponentNode* root, bool increase_count)
  : logger_(&logger), root_(root, increase_count)
{
}

// Copy ctor

madara::expression::ExpressionTree::ExpressionTree(
    logger::Logger& logger, const madara::expression::ExpressionTree& t)
  : logger_(&logger), root_(t.root_)
{
}

// Assignment operator

void madara::expression::ExpressionTree::operator=(
    const madara::expression::ExpressionTree& t)
{
  // Refcounter class takes care of the internal decrements and
  // increments.
  if (this != &t) {
    logger_ = t.logger_;
    root_ = t.root_;
  }
}

// Dtor

madara::expression::ExpressionTree::~ExpressionTree(void)
{
  // taken care of by Refcounter class
}

// Check if the tree is empty.

bool madara::expression::ExpressionTree::is_null(void) const
{
  return root_.get_ptr() == 0;
}

/// Prune the tree of unnecessary nodes.
/// Returns evaluation of the node and sets can_change appropriately.
/// if this node can be changed, that means it shouldn't be pruned.
madara::knowledge::KnowledgeRecord madara::expression::ExpressionTree::prune(
    void)
{
  bool root_can_change = false;
  madara::knowledge::KnowledgeRecord root_value;

  if (this->root_.get_ptr()) {
    root_value = this->root_->prune(root_can_change);
    if (!root_can_change &&
        dynamic_cast<LeafNode*>(this->root_.get_ptr()) == 0) {
      root_ = new LeafNode(*(this->logger_), root_value);
    }
  }

  return root_value;
}

/// Evaluates the node and its children. This does not prune any of
/// the expression tree, and is much faster than the prune function
madara::knowledge::KnowledgeRecord madara::expression::ExpressionTree::evaluate(
    const madara::knowledge::KnowledgeUpdateSettings& settings)
{
  if (root_.get_ptr() != 0)
    return root_->evaluate(settings);
  else
    return madara::knowledge::KnowledgeRecord(0);
}

// return root pointer
madara::expression::ComponentNode* madara::expression::ExpressionTree::get_root(
    void)
{
  return root_.get_ptr();
}

// Return the stored item.

madara::knowledge::KnowledgeRecord madara::expression::ExpressionTree::item(
    void) const
{
  return root_->item();
}

// Return the left branch.

madara::expression::ExpressionTree madara::expression::ExpressionTree::left(
    void)
{
  return ExpressionTree(*logger_, root_->left(), true);
}

// Return the left branch.

madara::expression::ExpressionTree madara::expression::ExpressionTree::right(
    void)
{
  return ExpressionTree(*logger_, root_->right(), true);
}

// Return a begin iterator of a specified type.

madara::expression::ExpressionTree::iterator
madara::expression::ExpressionTree::begin(const std::string& traversal_order)
{
  return ExpressionTree::iterator(
      tree_iterator_factory.make_tree_iterator(*this, traversal_order, false));
}

// Return an end iterator of a specified type.

madara::expression::ExpressionTree::iterator
madara::expression::ExpressionTree::end(const std::string& traversal_order)
{
  return ExpressionTree::iterator(
      tree_iterator_factory.make_tree_iterator(*this, traversal_order, true));
}

// Return a begin iterator of a specified type.

madara::expression::ExpressionTreeConstIterator
madara::expression::ExpressionTree::begin(
    const std::string& traversal_order) const
{
  ExpressionTree* non_const_this = const_cast<ExpressionTree*>(this);
  return ExpressionTree::const_iterator(
      tree_iterator_factory.make_tree_iterator(
          *non_const_this, traversal_order, false));
}

// Return an end iterator of a specified type.

madara::expression::ExpressionTreeConstIterator
madara::expression::ExpressionTree::end(
    const std::string& traversal_order) const
{
  ExpressionTree* non_const_this = const_cast<ExpressionTree*>(this);
  return ExpressionTree::const_iterator(
      tree_iterator_factory.make_tree_iterator(
          *non_const_this, traversal_order, true));
}

/// Accept a visitor to perform some action on the ExpressionTree.

void madara::expression::ExpressionTree::accept(Visitor& visitor) const
{
  root_->accept(visitor);
}

#endif  // _MADARA_NO_KARL_

#endif /* _EXPRESSION_TREE_CPP_ */
