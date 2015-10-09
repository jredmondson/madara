/* -*- C++ -*- */
#ifndef _COMPOSITE_NODE_CPP_
#define _COMPOSITE_NODE_CPP_

#ifndef _MADARA_NO_KARL_

#include "madara/expression/CompositeUnaryNode.h"

madara::expression::CompositeUnaryNode::CompositeUnaryNode (
  logger::Logger & logger, ComponentNode *right)
: ComponentNode (logger), right_ (right)
{    
}

// Dtor
madara::expression::CompositeUnaryNode::~CompositeUnaryNode (void)
{
  delete right_;
}


// Return the right child pointer
madara::expression::ComponentNode *
madara::expression::CompositeUnaryNode::right (void) const
{
  return right_;
}

#endif // _MADARA_NO_KARL_

#endif /* _COMPOSITE_NODE_CPP_ */
