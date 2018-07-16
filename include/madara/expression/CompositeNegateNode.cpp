/* -*- C++ -*- */
#ifndef _NEGATE_NODE_CPP_
#define _NEGATE_NODE_CPP_

#ifndef _MADARA_NO_KARL_

#include <iostream>

#include "madara/expression/ComponentNode.h"
#include "madara/expression/CompositeUnaryNode.h"
#include "madara/expression/Visitor.h"
#include "madara/expression/CompositeNegateNode.h"
#include "madara/expression/LeafNode.h"

madara::expression::CompositeNegateNode::CompositeNegateNode (
  logger::Logger & logger, ComponentNode *right)
  : CompositeUnaryNode (logger, right)
{    
}

// Dtor
madara::expression::CompositeNegateNode::~CompositeNegateNode (void)
{
}

madara::knowledge::KnowledgeRecord
madara::expression::CompositeNegateNode::item (void) const
{
  return knowledge::KnowledgeRecord ("-");
}

/// Prune the tree of unnecessary nodes. 
/// Returns evaluation of the node and sets can_change appropriately.
/// if this node can be changed, that means it shouldn't be pruned.
madara::knowledge::KnowledgeRecord
madara::expression::CompositeNegateNode::prune (bool & can_change)
{
  bool right_child_can_change = false;
  madara::knowledge::KnowledgeRecord right_value;

  if (this->right_)
  {
    right_value = this->right_->prune (right_child_can_change);
    if (!right_child_can_change && dynamic_cast <LeafNode *> (right_) == 0)
    {
      delete this->right_;
      this->right_ = new LeafNode (*(this->logger_), right_value);
    }
  }
  else
  {
    madara_logger_ptr_log (logger_, logger::LOG_ERROR,
      "madara::expression::CompositeNegateNode: "
      "KARL COMPILE ERROR: "
      "Negate has no right expression\n");

    throw exceptions::KarlException ("madara::expression::CompositeNegateNode: "
      "KARL COMPILE ERROR: "
      "Negate has no right expression\n"); 
  }

  can_change = right_child_can_change;

  return -right_value;
}

/// Evaluates the node and its children. This does not prune any of
/// the expression tree, and is much faster than the prune function
madara::knowledge::KnowledgeRecord 
madara::expression::CompositeNegateNode::evaluate (
  const madara::knowledge::KnowledgeUpdateSettings & settings)
{
  madara::knowledge::KnowledgeRecord value = right_->evaluate (settings);

  madara_logger_ptr_log (logger_, logger::LOG_DETAILED,
    "madara::expression::CompositeNegateNode:: Negating %s.\n",
    value.to_string ().c_str ());

  return -value;
}

// accept a visitor
void 
madara::expression::CompositeNegateNode::accept (Visitor &visitor) const
{
  visitor.visit (*this);
}

#endif // _MADARA_NO_KARL_

#endif /* _NEGATE_NODE_CPP_ */
