/* -*- C++ -*- */

#ifndef _COMPONENT_NODE_CPP_
#define _COMPONENT_NODE_CPP_

#ifndef _MADARA_NO_KARL_

#include <stdexcept>

#include "madara/expression/ComponentNode.h"
#include "madara/expression/Visitor.h"


madara::expression::ComponentNode::ComponentNode (
  logger::Logger & logger)
{
  set_logger (logger);
}

// Dtor
madara::expression::ComponentNode::~ComponentNode (void)
{
}

madara::KnowledgeRecord
madara::expression::ComponentNode::item (void) const
{
  return KnowledgeRecord ();
}

// default left is to return a null pointer
madara::expression::ComponentNode *
madara::expression::ComponentNode::left (void) const
{
  return 0;
}

// default right is to return a null pointer
madara::expression::ComponentNode *
madara::expression::ComponentNode::right (void) const
{
  return 0;
}

// accept a visitor
void 
madara::expression::ComponentNode::accept (madara::expression::Visitor &visitor) const
{
}

void
madara::expression::ComponentNode::set_logger (logger::Logger & logger)
{
  logger_ = &logger;
}

#endif // _MADARA_NO_KARL_

#endif /* _COMPONENT_NODE_CPP_ */
