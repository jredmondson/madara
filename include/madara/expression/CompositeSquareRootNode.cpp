/* -*- C++ -*- */
#ifndef _MADARA_COMPOSITE_SQUARE_ROOT_CPP_
#define _MADARA_COMPOSITE_SQUARE_ROOT_CPP_

#ifndef _MADARA_NO_KARL_

#include <iostream>
#include <math.h>

#include "madara/expression/ComponentNode.h"
#include "madara/expression/CompositeUnaryNode.h"
#include "madara/expression/Visitor.h"
#include "madara/expression/CompositeSquareRootNode.h"
#include "madara/expression/LeafNode.h"

madara::expression::CompositeSquareRootNode::CompositeSquareRootNode (
  logger::Logger & logger, ComponentNode *right)
  : CompositeUnaryNode (logger, right)
{    
}

// Dtor
madara::expression::CompositeSquareRootNode::~CompositeSquareRootNode (void)
{
}

madara::knowledge::KnowledgeRecord
madara::expression::CompositeSquareRootNode::item (void) const
{
  return knowledge::KnowledgeRecord ("#sqrt");
}

/// Prune the tree of unnecessary nodes. 
/// Returns evaluation of the node and sets can_change appropriately.
/// if this node can be changed, that means it shouldn't be pruned.
madara::knowledge::KnowledgeRecord
madara::expression::CompositeSquareRootNode::prune (bool & can_change)
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
      "KARL COMPILE ERROR: "

      "Sqrt (alt 251) has no right expression\n");

    throw KarlException ("madara::expression::CompositeSquareRootNode: "
      "KARL COMPILE ERROR: "
      "Node has no right expression\n"); 
  }

  can_change = right_child_can_change;

  return knowledge::KnowledgeRecord (sqrt (right_value.to_double ()));
}

/// Evaluates the node and its children. This does not prune any of
/// the expression tree, and is much faster than the prune function
madara::knowledge::KnowledgeRecord 
madara::expression::CompositeSquareRootNode::evaluate (
  const madara::knowledge::KnowledgeUpdateSettings & settings)
{
  madara::knowledge::KnowledgeRecord value = right_->evaluate (settings);

  madara_logger_ptr_log (logger_, logger::LOG_DETAILED,
    "CompositeSquareRootNode: "
    "taking square root of %s.\n", value.to_string ().c_str ());

  return knowledge::KnowledgeRecord (sqrt (value.to_double ()));
}

// accept a visitor
void 
madara::expression::CompositeSquareRootNode::accept (Visitor &visitor) const
{
  visitor.visit (*this);
}

#endif // _MADARA_NO_KARL_

#endif /* _MADARA_COMPOSITE_SQUARE_ROOT_CPP_ */
