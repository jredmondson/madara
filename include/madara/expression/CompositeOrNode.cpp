/* -*- C++ -*- */

#ifndef COMPOSITE_OR_NODE_CPP
#define COMPOSITE_OR_NODE_CPP

#ifndef _MADARA_NO_KARL_

#include <iostream>

#include "madara/expression/ComponentNode.h"
#include "madara/expression/Visitor.h"
#include "madara/expression/CompositeOrNode.h"
#include "madara/expression/LeafNode.h"

madara::expression::CompositeOrNode::CompositeOrNode (
  logger::Logger & logger, const ComponentNodes & nodes)
: madara::expression::CompositeTernaryNode (logger, nodes)
{
}

madara::knowledge::KnowledgeRecord
madara::expression::CompositeOrNode::item (void) const
{
  return knowledge::KnowledgeRecord ("||");
}

/// Prune the tree of unnecessary nodes. 
/// Returns evaluation of the node and sets can_change appropriately.
/// if this node can be changed, that means it shouldn't be pruned.
madara::knowledge::KnowledgeRecord
madara::expression::CompositeOrNode::prune (bool & can_change)
{
  madara::knowledge::KnowledgeRecord return_value;

  int j = 0;
  for (ComponentNodes::iterator i = nodes_.begin ();
       i != nodes_.end (); ++i, ++j)
  {
    bool value_changes = false;
    madara::knowledge::KnowledgeRecord value;
    value = (*i)->prune (value_changes);
    if (!value_changes && dynamic_cast <LeafNode *> (*i) == 0)
    {
      delete *i;
      *i = new LeafNode (*(this->logger_), value);
    }

    if (j == 0)
      return_value = value;
    else
      return_value = knowledge::KnowledgeRecord (
        value || return_value);

    can_change = can_change || value_changes;
  }

  if (nodes_.size () < 2)
  {
    madara_logger_ptr_log (logger_, logger::LOG_ERROR,
      "CompositeOrNode: "
      "KARL COMPILE ERROR (||): "
      "Or should have a left and right-hand side argument.\n");

    throw KarlException ("CompositeOrNode: "
      "KARL COMPILE ERROR (||): "
      "Or should have a left and right-hand side argument.\n");
  }

  return return_value;
}

/// Evaluates the node and its children. This does not prune any of
/// the expression tree, and is much faster than the prune function
madara::knowledge::KnowledgeRecord 
madara::expression::CompositeOrNode::evaluate (
  const madara::knowledge::KnowledgeUpdateSettings & settings)
{
  int j = 0;
  for (ComponentNodes::iterator i = nodes_.begin ();
       i != nodes_.end (); ++i, ++j)
  {
    // if we have a non-zero eval, return 1 immediately
    if ((*i)->evaluate (settings).is_true ())
      return madara::knowledge::KnowledgeRecord (1);
  }

  // if nothing was true, return false
  return madara::knowledge::KnowledgeRecord ();
}

// accept a visitor
void 
madara::expression::CompositeOrNode::accept (Visitor &visitor) const
{
  visitor.visit (*this);
}

#endif // _MADARA_NO_KARL_

#endif /* COMPOSITE_OR_NODE_CPP */
