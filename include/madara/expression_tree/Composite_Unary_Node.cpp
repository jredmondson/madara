/* -*- C++ -*- */
#ifndef _COMPOSITE_NODE_CPP_
#define _COMPOSITE_NODE_CPP_

#include "madara/expression_tree/Composite_Unary_Node.h"

// Ctor

Madara::Expression_Tree::Composite_Unary_Node::Composite_Unary_Node (Component_Node *right)
  : Component_Node (), 
    right_ (right)
{    
}

// Dtor
Madara::Expression_Tree::Composite_Unary_Node::~Composite_Unary_Node (void)
{
  delete right_;
}


// Return the right child pointer
Madara::Expression_Tree::Component_Node *
Madara::Expression_Tree::Composite_Unary_Node::right (void) const
{
  return right_;
}


#endif /* _COMPOSITE_NODE_CPP_ */
