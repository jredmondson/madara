/* -*- C++ -*- */
#ifndef _COMPOSITE_LR_NODE_CPP_
#define _COMPOSITE_LR_NODE_CPP_

#ifndef _MADARA_NO_KARL_

#include "madara/expression/CompositeBinaryNode.h"

// Ctor
madara::expression::CompositeBinaryNode::CompositeBinaryNode(
    logger::Logger& logger, ComponentNode* left, ComponentNode* right)
  : CompositeUnaryNode(logger, right), left_(left)
{
}

// Dtor
madara::expression::CompositeBinaryNode::~CompositeBinaryNode(void)
{
  delete left_;
}

// Return the left child pointer
madara::expression::ComponentNode*
madara::expression::CompositeBinaryNode::left(void) const
{
  return left_;
}

#endif  // _MADARA_NO_KARL_

#endif /* _COMPOSITE_LR_NODE_CPP_ */
