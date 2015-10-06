/* -*- C++ -*- */

#ifndef _TERNARY_NODE_CPP_
#define _TERNARY_NODE_CPP_

#ifndef _MADARA_NO_KARL_

#include <stdexcept>

#include "madara/expression/Composite_Ternary_Node.h"
#include "madara/expression/Visitor.h"


madara::expression::Composite_Ternary_Node::Composite_Ternary_Node (
  logger::Logger & logger)
: Component_Node (logger), nodes_ ()
{

}


madara::expression::Composite_Ternary_Node::Composite_Ternary_Node (
  logger::Logger & logger, const Component_Nodes & nodes)
: Component_Node (logger), nodes_ (nodes)
{

}

// Dtor
madara::expression::Composite_Ternary_Node::~Composite_Ternary_Node (void)
{
  for (Component_Nodes::iterator i = nodes_.begin (); i != nodes_.end (); ++i)
    delete *i;
}

madara::Knowledge_Record
madara::expression::Composite_Ternary_Node::item (void) const
{
  return madara::Knowledge_Record::Integer (nodes_.size ());
}

// accept a visitor
void 
madara::expression::Composite_Ternary_Node::accept (madara::expression::Visitor &visitor) const
{
}

#endif // _MADARA_NO_KARL_

#endif /* _TERNARY_NODE_CPP_ */
