/* -*- C++ -*- */

#ifndef _TERNARY_NODE_CPP_
#define _TERNARY_NODE_CPP_

#ifndef _MADARA_NO_KARL_

#include <stdexcept>

#include "madara/expression/CompositeTernaryNode.h"
#include "madara/expression/Visitor.h"


madara::expression::CompositeTernaryNode::CompositeTernaryNode (
  logger::Logger & logger)
: ComponentNode (logger), nodes_ ()
{

}


madara::expression::CompositeTernaryNode::CompositeTernaryNode (
  logger::Logger & logger, const ComponentNodes & nodes)
: ComponentNode (logger), nodes_ (nodes)
{

}

// Dtor
madara::expression::CompositeTernaryNode::~CompositeTernaryNode (void)
{
  for (ComponentNodes::iterator i = nodes_.begin (); i != nodes_.end (); ++i)
    delete *i;
}

madara::knowledge::KnowledgeRecord
madara::expression::CompositeTernaryNode::item (void) const
{
  return madara::knowledge::KnowledgeRecord::Integer (nodes_.size ());
}

// accept a visitor
void 
madara::expression::CompositeTernaryNode::accept (madara::expression::Visitor &visitor) const
{
}

#endif // _MADARA_NO_KARL_

#endif /* _TERNARY_NODE_CPP_ */
