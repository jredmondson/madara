/* -*- C++ -*- */
#ifndef _COMPOSITE_NODE_CPP_
#define _COMPOSITE_NODE_CPP_

#ifndef _MADARA_NO_KARL_

#include "madara/expression_tree/Composite_Unary_Node.h"

madara::expression_tree::Composite_Unary_Node::Composite_Unary_Node (
  logger::Logger & logger, Component_Node *right)
: Component_Node (logger), right_ (right)
{    
}

// Dtor
madara::expression_tree::Composite_Unary_Node::~Composite_Unary_Node (void)
{
  delete right_;
}


// Return the right child pointer
madara::expression_tree::Component_Node *
madara::expression_tree::Composite_Unary_Node::right (void) const
{
  return right_;
}

#endif // _MADARA_NO_KARL_

#endif /* _COMPOSITE_NODE_CPP_ */
