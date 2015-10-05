/* -*- C++ -*- */
#ifndef _COMPOSITE_LR_NODE_CPP_
#define _COMPOSITE_LR_NODE_CPP_

#ifndef _MADARA_NO_KARL_

#include "madara/expression_tree/Composite_Binary_Node.h"

// Ctor
madara::expression_tree::Composite_Binary_Node::Composite_Binary_Node (
  logger::Logger & logger,
  Component_Node * left,
  Component_Node *right)
: Composite_Unary_Node (logger, right), left_ (left)
{   
}

// Dtor
madara::expression_tree::Composite_Binary_Node::~Composite_Binary_Node (void)
{
  delete left_;
}


// Return the left child pointer
madara::expression_tree::Component_Node *
madara::expression_tree::Composite_Binary_Node::left (void) const
{
  return left_;
}


#endif // _MADARA_NO_KARL_

#endif /* _COMPOSITE_LR_NODE_CPP_ */
