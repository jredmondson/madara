/* -*- C++ -*- */
#ifndef _COMPOSITE_PREINCREMENT_NODE_CPP_
#define _COMPOSITE_PREINCREMENT_NODE_CPP_

#ifndef _MADARA_NO_KARL_

#include <iostream>

#include "madara/expression/ComponentNode.h"
#include "madara/expression/CompositeUnaryNode.h"
#include "madara/expression/Visitor.h"
#include "madara/expression/CompositePreincrementNode.h"
#include "madara/expression/LeafNode.h"
#include "madara/expression/VariableNode.h"

madara::expression::CompositePreincrementNode::CompositePreincrementNode(
    logger::Logger& logger, ComponentNode* right)
  : CompositeUnaryNode(logger, right)
{
  var_ = dynamic_cast<VariableNode*>(right);

  if (!var_)
    array_ = dynamic_cast<CompositeArrayReference*>(right);
}

// Dtor
madara::expression::CompositePreincrementNode::~CompositePreincrementNode(void)
{
}

madara::knowledge::KnowledgeRecord
madara::expression::CompositePreincrementNode::item(void) const
{
  madara::knowledge::KnowledgeRecord record;
  record.set_value("++");
  return record;
}

/// Prune the tree of unnecessary nodes.
/// Returns evaluation of the node and sets can_change appropriately.
/// if this node can be changed, that means it shouldn't be pruned.
madara::knowledge::KnowledgeRecord
madara::expression::CompositePreincrementNode::prune(bool& can_change)
{
  bool right_child_can_change = false;
  madara::knowledge::KnowledgeRecord right_value;

  if (this->right_)
  {
    right_value = this->right_->prune(right_child_can_change);
    if (!right_child_can_change && dynamic_cast<LeafNode*>(right_) == 0)
    {
      delete this->right_;
      this->right_ = new LeafNode(*(this->logger_), right_value);
    }
  }
  else if (!var_ && !array_)
  {
    madara_logger_ptr_log(logger_, logger::LOG_ERROR,
        "madara::expression::CompositePreincrementNode: "
        "KARL COMPILE ERROR: "
        "Preincrement has no var, array, or expression\n");

    throw exceptions::KarlException(
        "madara::expression::CompositePreincrementNode: "
        "KARL COMPILE ERROR: "
        "Preincrement has no var, array, or expression\n");
  }

  can_change = right_child_can_change;

  return ++right_value;
}

/// Evaluates the node and its children. This does not prune any of
/// the expression tree, and is much faster than the prune function
madara::knowledge::KnowledgeRecord
madara::expression::CompositePreincrementNode::evaluate(
    const madara::knowledge::KnowledgeUpdateSettings& settings)
{
  if (var_)
    return var_->inc(settings);
  else if (array_)
    return array_->inc(settings);
  else if (right_)
    return ++(this->right_->evaluate(settings));
  else
  {
    madara_logger_ptr_log(logger_, logger::LOG_ERROR,
        "madara::expression::CompositePreincrementNode: "
        "KARL RUNTIME ERROR: "
        "Preincrement has no var, array, or expression\n");

    throw exceptions::KarlException(
        "madara::expression::CompositePreincrementNode: "
        "KARL RUNTIME ERROR: "
        "Preincrement has no var, array, or expression\n");
  }
}

// accept a visitor
void madara::expression::CompositePreincrementNode::accept(
    Visitor& visitor) const
{
  visitor.visit(*this);
}

#endif  // _MADARA_NO_KARL_

#endif /* _COMPOSITE_PREINCREMENT_NODE_CPP_ */
