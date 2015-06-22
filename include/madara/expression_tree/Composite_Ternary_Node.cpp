/* -*- C++ -*- */

#ifndef _TERNARY_NODE_CPP_
#define _TERNARY_NODE_CPP_

#ifndef _MADARA_NO_KARL_

#include <stdexcept>

#include "madara/expression_tree/Composite_Ternary_Node.h"
#include "madara/expression_tree/Visitor.h"


Madara::Expression_Tree::Composite_Ternary_Node::Composite_Ternary_Node (
  Logger::Logger & logger)
: Component_Node (logger), nodes_ ()
{

}


Madara::Expression_Tree::Composite_Ternary_Node::Composite_Ternary_Node (
  Logger::Logger & logger, const Component_Nodes & nodes)
: Component_Node (logger), nodes_ (nodes)
{

}

// Dtor
Madara::Expression_Tree::Composite_Ternary_Node::~Composite_Ternary_Node (void)
{
  for (Component_Nodes::iterator i = nodes_.begin (); i != nodes_.end (); ++i)
    delete *i;
}

Madara::Knowledge_Record
Madara::Expression_Tree::Composite_Ternary_Node::item (void) const
{
  return Madara::Knowledge_Record::Integer (nodes_.size ());
}

// accept a visitor
void 
Madara::Expression_Tree::Composite_Ternary_Node::accept (Madara::Expression_Tree::Visitor &visitor) const
{
}

#endif // _MADARA_NO_KARL_

#endif /* _TERNARY_NODE_CPP_ */
