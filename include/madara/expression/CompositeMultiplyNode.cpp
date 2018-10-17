/* -*- C++ -*- */
#ifndef _MULTIPLY_NODE_CPP_
#define _MULTIPLY_NODE_CPP_

#ifndef _MADARA_NO_KARL_

#include <iostream>

#include "madara/expression/ComponentNode.h"
#include "madara/expression/CompositeBinaryNode.h"
#include "madara/expression/CompositeMultiplyNode.h"
#include "madara/expression/Visitor.h"
#include "madara/expression/LeafNode.h"

madara::expression::CompositeMultiplyNode::CompositeMultiplyNode(
    logger::Logger& logger, const ComponentNodes& nodes)
  : madara::expression::CompositeTernaryNode(logger, nodes)
{
}

// Dtor
madara::expression::CompositeMultiplyNode::~CompositeMultiplyNode(void) {}

madara::knowledge::KnowledgeRecord
madara::expression::CompositeMultiplyNode::item(void) const
{
  return knowledge::KnowledgeRecord("*");
}

/// Prune the tree of unnecessary nodes.
/// Returns evaluation of the node and sets can_change appropriately.
/// if this node can be changed, that means it shouldn't be pruned.
madara::knowledge::KnowledgeRecord
madara::expression::CompositeMultiplyNode::prune(bool& can_change)
{
  madara::knowledge::KnowledgeRecord return_value;

  if(nodes_.size() >= 2)
  {
    int j = 0;
    for(ComponentNodes::iterator i = nodes_.begin(); i != nodes_.end();
         ++i, ++j)
    {
      bool value_changes = false;
      madara::knowledge::KnowledgeRecord value;
      value = (*i)->prune(value_changes);
      if(!value_changes && dynamic_cast<LeafNode*>(*i) == 0)
      {
        delete *i;
        *i = new LeafNode(*(this->logger_), value);
      }

      if(j == 0)
        return_value = value;
      else
        return_value *= value;

      can_change = can_change || value_changes;
    }
  }
  else
  {
    madara_logger_ptr_log(logger_, logger::LOG_ERROR,
        "madara::expression::SystemCallLogLevel: "
        "KARL COMPILE ERROR: "
        "Multiplication is impossible without at least two values\n");

    throw exceptions::KarlException(
        "madara::expression::SystemCallLogLevel: "
        "KARL COMPILE ERROR: "
        "Multiplication is impossible without at least two values\n");
  }

  return return_value;
}

/// Evaluates the node and its children. This does not prune any of
/// the expression tree, and is much faster than the prune function
madara::knowledge::KnowledgeRecord
madara::expression::CompositeMultiplyNode::evaluate(
    const madara::knowledge::KnowledgeUpdateSettings& settings)
{
  madara::knowledge::KnowledgeRecord return_value;

  int j = 0;
  for(ComponentNodes::iterator i = nodes_.begin(); i != nodes_.end(); ++i, ++j)
  {
    madara::knowledge::KnowledgeRecord value = (*i)->evaluate(settings);

    if(j == 0)
      return_value = value;
    else
      return_value *= value;
  }

  return return_value;
}

// accept a visitor
void madara::expression::CompositeMultiplyNode::accept(Visitor& visitor) const
{
  visitor.visit(*this);
}

#endif  // _MADARA_NO_KARL_

#endif /* _MULTIPLY_NODE_CPP_ */
