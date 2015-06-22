/* -*- C++ -*- */

#ifndef _COMPONENT_NODE_CPP_
#define _COMPONENT_NODE_CPP_

#ifndef _MADARA_NO_KARL_

#include <stdexcept>

#include "madara/expression_tree/Component_Node.h"
#include "madara/expression_tree/Visitor.h"


Madara::Expression_Tree::Component_Node::Component_Node (
  Logger::Logger & logger)
{
  set_logger (logger);
}

// Dtor
Madara::Expression_Tree::Component_Node::~Component_Node (void)
{
}

Madara::Knowledge_Record
Madara::Expression_Tree::Component_Node::item (void) const
{
  return Knowledge_Record ();
}

// default left is to return a null pointer
Madara::Expression_Tree::Component_Node *
Madara::Expression_Tree::Component_Node::left (void) const
{
  return 0;
}

// default right is to return a null pointer
Madara::Expression_Tree::Component_Node *
Madara::Expression_Tree::Component_Node::right (void) const
{
  return 0;
}

// accept a visitor
void 
Madara::Expression_Tree::Component_Node::accept (Madara::Expression_Tree::Visitor &visitor) const
{
}

void
Madara::Expression_Tree::Component_Node::set_logger (Logger::Logger & logger)
{
  logger_ = &logger;
}

#endif // _MADARA_NO_KARL_

#endif /* _COMPONENT_NODE_CPP_ */
