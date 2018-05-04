/* -*- C++ -*- */

#ifndef _COMPOSITE_RETURN_RIGHT_NODE_CPP_
#define _COMPOSITE_RETURN_RIGHT_NODE_CPP_

#ifndef _MADARA_NO_KARL_

#include <iostream>
#include <algorithm>

#include "madara/expression/ComponentNode.h"
#include "madara/expression/Visitor.h"
#include "madara/expression/CompositeReturnRightNode.h"
#include "madara/expression/LeafNode.h"

madara::expression::CompositeReturnRightNode::CompositeReturnRightNode (
  logger::Logger & logger, const ComponentNodes & nodes)
: madara::expression::CompositeTernaryNode (logger, nodes)
{
}

madara::knowledge::KnowledgeRecord
madara::expression::CompositeReturnRightNode::item (void) const
{
  return knowledge::KnowledgeRecord (";>");
}

/// Prune the tree of unnecessary nodes. 
/// Returns evaluation of the node and sets can_change appropriately.
/// if this node can be changed, that means it shouldn't be pruned.
madara::knowledge::KnowledgeRecord
madara::expression::CompositeReturnRightNode::prune (bool & can_change)
{
  madara::knowledge::KnowledgeRecord return_value;

  unsigned int j = 0;
  for (ComponentNodes::iterator i = nodes_.begin ();
       i != nodes_.end (); ++i, ++j)
  {
    bool value_changes = false;
    madara::knowledge::KnowledgeRecord value;
    if (j + 1 == nodes_.size ())
      return_value = (*i)->prune (value_changes);
    else
      (*i)->prune (value_changes);

    if (!value_changes && dynamic_cast <LeafNode *> (*i) == 0)
    {
      delete *i;
      *i = new LeafNode (*(this->logger_), value);
    }

    can_change = can_change || value_changes;
  }

  return return_value;
}

/// Evaluates the node and its children. This does not prune any of
/// the expression tree, and is much faster than the prune function
/// @ returns    maximum value of the left and right evaluations
madara::knowledge::KnowledgeRecord 
madara::expression::CompositeReturnRightNode::evaluate (
  const madara::knowledge::KnowledgeUpdateSettings & settings)
{
  madara::knowledge::KnowledgeRecord return_value;

  int j = 0;
  for (ComponentNodes::iterator i = nodes_.begin ();
       i != nodes_.end (); ++i, ++j)
  {
    if (j + 1 == (int)nodes_.size ())
      return_value = (*i)->evaluate (settings);
    else
      (*i)->evaluate (settings);
  }

  return return_value;
}

// accept a visitor
void 
madara::expression::CompositeReturnRightNode::accept (Visitor &visitor) const
{
  visitor.visit (*this);
}

#endif // _MADARA_NO_KARL_

#endif /* _COMPOSITE_RETURN_RIGHT_NODE_CPP_ */
