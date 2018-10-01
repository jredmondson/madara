/* -*- C++ -*- */

#ifndef _ADD_NODE_CPP_
#define _ADD_NODE_CPP_

#ifndef _MADARA_NO_KARL_

#include <iostream>

#include "madara/expression/ComponentNode.h"
#include "madara/expression/Visitor.h"
#include "madara/expression/CompositeAddNode.h"
#include "madara/expression/LeafNode.h"

// Ctor

madara::expression::CompositeAddNode::CompositeAddNode(
    logger::Logger& logger, const ComponentNodes& nodes)
  : madara::expression::CompositeTernaryNode(logger, nodes)
{
}

madara::knowledge::KnowledgeRecord madara::expression::CompositeAddNode::item(
    void) const
{
  return madara::knowledge::KnowledgeRecord("+");
}

/// Prune the tree of unnecessary nodes.
/// Returns evaluation of the node and sets can_change appropriately.
/// if this node can be changed, that means it shouldn't be pruned.
madara::knowledge::KnowledgeRecord madara::expression::CompositeAddNode::prune(
    bool& can_change)
{
  madara::knowledge::KnowledgeRecord return_value;

  int j = 0;
  for (ComponentNodes::iterator i = nodes_.begin(); i != nodes_.end(); ++i, ++j)
  {
    bool value_changes = false;
    madara::knowledge::KnowledgeRecord value;
    value = (*i)->prune(value_changes);
    if (!value_changes && dynamic_cast<LeafNode*>(*i) == 0)
    {
      delete *i;
      *i = new LeafNode(*this->logger_, value);
    }

    if (j == 0)
      return_value = value;
    else
      return_value += value;

    can_change = can_change || value_changes;
  }

  if (nodes_.size() < 2)
  {
    madara_logger_ptr_log(logger_, logger::LOG_ERROR,
        "CompositeAddNode: "
        "KARL COMPILE ERROR (+): "
        "Add should have a left and right-hand side argument.\n");

    throw exceptions::KarlException(
        "CompositeAddNode: "
        "KARL COMPILE ERROR (+): "
        "Add should have a left and right-hand side argument.\n");
  }

  return return_value;
}

/// Evaluates the node and its children. This does not prune any of
/// the expression tree, and is much faster than the prune function
madara::knowledge::KnowledgeRecord
madara::expression::CompositeAddNode::evaluate(
    const madara::knowledge::KnowledgeUpdateSettings& settings)
{
  madara::knowledge::KnowledgeRecord return_value;

  int j = 0;
  for (ComponentNodes::iterator i = nodes_.begin(); i != nodes_.end(); ++i, ++j)
  {
    madara::knowledge::KnowledgeRecord value = (*i)->evaluate(settings);

    if (j == 0)
      return_value = value;
    else
      return_value += value;
  }

  return return_value;
}

// accept a visitor
void madara::expression::CompositeAddNode::accept(Visitor& visitor) const
{
  visitor.visit(*this);
}

#endif  // _MADARA_NO_KARL_

#endif /* _ADD_NODE_CPP_ */
