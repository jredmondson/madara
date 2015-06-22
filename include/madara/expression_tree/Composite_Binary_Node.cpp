/* -*- C++ -*- */
#ifndef _COMPOSITE_LR_NODE_CPP_
#define _COMPOSITE_LR_NODE_CPP_

#ifndef _MADARA_NO_KARL_

#include "madara/expression_tree/Composite_Binary_Node.h"

// Ctor
Madara::Expression_Tree::Composite_Binary_Node::Composite_Binary_Node (
  Logger::Logger & logger,
  Component_Node * left,
  Component_Node *right)
: Composite_Unary_Node (logger, right), left_ (left)
{   
}

// Dtor
Madara::Expression_Tree::Composite_Binary_Node::~Composite_Binary_Node (void)
{
  delete left_;
}


// Return the left child pointer
Madara::Expression_Tree::Component_Node *
Madara::Expression_Tree::Composite_Binary_Node::left (void) const
{
  return left_;
}


#endif // _MADARA_NO_KARL_

#endif /* _COMPOSITE_LR_NODE_CPP_ */
