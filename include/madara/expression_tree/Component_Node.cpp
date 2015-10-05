/* -*- C++ -*- */

#ifndef _COMPONENT_NODE_CPP_
#define _COMPONENT_NODE_CPP_

#ifndef _MADARA_NO_KARL_

#include <stdexcept>

#include "madara/expression_tree/Component_Node.h"
#include "madara/expression_tree/Visitor.h"


madara::expression_tree::Component_Node::Component_Node (
  logger::Logger & logger)
{
  set_logger (logger);
}

// Dtor
madara::expression_tree::Component_Node::~Component_Node (void)
{
}

madara::Knowledge_Record
madara::expression_tree::Component_Node::item (void) const
{
  return Knowledge_Record ();
}

// default left is to return a null pointer
madara::expression_tree::Component_Node *
madara::expression_tree::Component_Node::left (void) const
{
  return 0;
}

// default right is to return a null pointer
madara::expression_tree::Component_Node *
madara::expression_tree::Component_Node::right (void) const
{
  return 0;
}

// accept a visitor
void 
madara::expression_tree::Component_Node::accept (madara::expression_tree::Visitor &visitor) const
{
}

void
madara::expression_tree::Component_Node::set_logger (logger::Logger & logger)
{
  logger_ = &logger;
}

#endif // _MADARA_NO_KARL_

#endif /* _COMPONENT_NODE_CPP_ */
