/* -*- C++ -*- */
#ifndef _DIVIDE_NODE_CPP_
#define _DIVIDE_NODE_CPP_

#ifndef _MADARA_NO_KARL_

#include <math.h>
#include <iostream>

#include "madara/expression/CompositeBinaryNode.h"
#include "madara/expression/CompositeDivideNode.h"
#include "madara/expression/Visitor.h"
#include "madara/expression/LeafNode.h"

// Ctor
madara::expression::CompositeDivideNode::CompositeDivideNode(
    logger::Logger& logger, ComponentNode* left, ComponentNode* right)
  : CompositeBinaryNode(logger, left, right)
{
}

// Dtor
madara::expression::CompositeDivideNode::~CompositeDivideNode(void) {}

madara::knowledge::KnowledgeRecord
madara::expression::CompositeDivideNode::item(void) const
{
  madara::knowledge::KnowledgeRecord record;
  record.set_value("/");
  return record;
}

/// Prune the tree of unnecessary nodes.
/// Returns evaluation of the node and sets can_change appropriately.
/// if this node can be changed, that means it shouldn't be pruned.
madara::knowledge::KnowledgeRecord
madara::expression::CompositeDivideNode::prune(bool& can_change)
{
  bool left_child_can_change = false;
  bool right_child_can_change = false;
  madara::knowledge::KnowledgeRecord left_value;
  madara::knowledge::KnowledgeRecord right_value;

  if (this->left_)
  {
    left_value = this->left_->prune(left_child_can_change);
    if (!left_child_can_change && dynamic_cast<LeafNode*>(left_) == 0)
    {
      delete this->left_;
      this->left_ = new LeafNode(*(this->logger_), left_value);
    }
  }
  else
  {
    madara_logger_ptr_log(logger_, logger::LOG_ERROR,
        "madara::expression::CompositeDivideNode: "
        "KARL COMPILE ERROR: Division has no left expression\n");

    throw exceptions::KarlException("madara::expression::CompositeDivideNode: "
                                    "KARL COMPILE ERROR: "
                                    "Division has no left expression\n");
  }

  if (this->right_)
  {
    right_value = this->right_->prune(right_child_can_change);

    if (!right_child_can_change && dynamic_cast<LeafNode*>(right_) == 0)
    {
      // leave this check which is important
      if (right_value.is_true())
      {
        delete right_;
        right_ = new LeafNode(*(this->logger_), right_value);
      }
    }

    // if (!right_child_can_change)
    //  right_child_can_change = true;
  }
  else
  {
    madara_logger_ptr_log(logger_, logger::LOG_ERROR,
        "KARL COMPILE ERROR: Division has no right expression (divide by "
        "zero)\n");

    throw exceptions::KarlException("madara::expression::CompositeDivideNode: "
                                    "KARL COMPILE ERROR: "
                                    "Division has no right expression\n");
  }

  can_change = left_child_can_change || right_child_can_change;

  return left_value / right_value;
}

/// Evaluates the node and its children. This does not prune any of
/// the expression tree, and is much faster than the prune function
madara::knowledge::KnowledgeRecord
madara::expression::CompositeDivideNode::evaluate(
    const madara::knowledge::KnowledgeUpdateSettings& settings)
{
  return left_->evaluate(settings) / right_->evaluate(settings);
}

// accept a visitor
void madara::expression::CompositeDivideNode::accept(Visitor& visitor) const
{
  visitor.visit(*this);
}

#endif  // _MADARA_NO_KARL_

#endif /* _DIVIDE_NODE_CPP_ */
