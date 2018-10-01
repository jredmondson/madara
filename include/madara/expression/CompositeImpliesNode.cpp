/* -*- C++ -*- */

#ifndef _COMPOSITE_IMPLIES_NODE_CPP
#define _COMPOSITE_IMPLIES_NODE_CPP

#ifndef _MADARA_NO_KARL_

#include <iostream>

#include "madara/expression/ComponentNode.h"
#include "madara/expression/Visitor.h"
#include "madara/expression/CompositeImpliesNode.h"
#include "madara/expression/LeafNode.h"

// Ctor

madara::expression::CompositeImpliesNode::CompositeImpliesNode(
    logger::Logger& logger, ComponentNode* left, ComponentNode* right)
  : madara::expression::CompositeBinaryNode(logger, left, right)
{
}

madara::knowledge::KnowledgeRecord
madara::expression::CompositeImpliesNode::item(void) const
{
  madara::knowledge::KnowledgeRecord record;
  record.set_value("=>");
  return record;
}

/// Prune the tree of unnecessary nodes.
/// Returns evaluation of the node and sets can_change appropriately.
/// if this node can be changed, that means it shouldn't be pruned.
madara::knowledge::KnowledgeRecord
madara::expression::CompositeImpliesNode::prune(bool& can_change)
{
  bool left_child_can_change = false;
  bool right_child_can_change = false;
  madara::knowledge::KnowledgeRecord left_value;
  madara::knowledge::KnowledgeRecord right_value;

  if (this->left_) {
    left_value = this->left_->prune(left_child_can_change);
    if (!left_child_can_change && dynamic_cast<LeafNode*>(left_) == 0) {
      delete this->left_;
      this->left_ = new LeafNode(*(this->logger_), left_value);
    }
  } else {
    madara_logger_ptr_log(logger_, logger::LOG_ERROR,
        "madara::expression::CompositeImpliesNode: "
        "KARL COMPILE ERROR: Implies has no condition\n");

    throw exceptions::KarlException(
        "madara::expression::CompositeImpliesNode: "
        "KARL COMPILE ERROR: "
        "Implies has no condition (left) expression\n");
  }

  if (this->right_) {
    right_value = this->right_->prune(right_child_can_change);
    if (!right_child_can_change && dynamic_cast<LeafNode*>(right_) == 0) {
      delete this->right_;
      this->right_ = new LeafNode(*(this->logger_), right_value);
    }
  } else {
    madara_logger_ptr_log(logger_, logger::LOG_ERROR,
        "madara::expression::CompositeImpliesNode: "
        "KARL COMPILE ERROR: Implies has no expression to "
        "evaluate if conditional is true\n");

    throw exceptions::KarlException("madara::expression::CompositeImpliesNode: "
                                    "KARL COMPILE ERROR: "
                                    "Implies has no right expression\n");
  }

  can_change = left_child_can_change || right_child_can_change;

  // if left_value, then return the right_value. Otherwise, 0
  return left_value;
}

/// Evaluates the node and its children. This does not prune any of
/// the expression tree, and is much faster than the prune function
madara::knowledge::KnowledgeRecord
madara::expression::CompositeImpliesNode::evaluate(
    const madara::knowledge::KnowledgeUpdateSettings& settings)
{
  madara::knowledge::KnowledgeRecord left_value = left_->evaluate(settings);

  // only evaluate right if left evaluates to non-zero
  if (left_value.is_true())
    right_->evaluate(settings);

  return left_value;
}

// accept a visitor
void madara::expression::CompositeImpliesNode::accept(Visitor& visitor) const
{
  visitor.visit(*this);
}

#endif  // _MADARA_NO_KARL_

#endif /* _COMPOSITE_IMPLIES_NODE_CPP */
