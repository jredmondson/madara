/* -*- C++ -*- */

#ifndef _COMPOSITE_BOTH_NODE_CPP
#define _COMPOSITE_BOTH_NODE_CPP

#ifndef _MADARA_NO_KARL_

#include <iostream>
#include <algorithm>

#include "madara/expression/Component_Node.h"
#include "madara/expression/Visitor.h"
#include "madara/expression/Composite_Both_Node.h"
#include "madara/expression/Leaf_Node.h"

// Ctor

madara::expression::Composite_Both_Node::Composite_Both_Node (
  logger::Logger & logger, const Component_Nodes & nodes)
: madara::expression::Composite_Ternary_Node (logger, nodes)
{
}

madara::Knowledge_Record
madara::expression::Composite_Both_Node::item (void) const
{
  return ";";
}

/// Prune the tree of unnecessary nodes. 
/// Returns evaluation of the node and sets can_change appropriately.
/// if this node can be changed, that means it shouldn't be pruned.
madara::Knowledge_Record
madara::expression::Composite_Both_Node::prune (bool & can_change)
{
  madara::Knowledge_Record return_value;

  int j = 0;
  for (Component_Nodes::iterator i = nodes_.begin ();
       i != nodes_.end (); ++i, ++j)
  {
    bool value_changes = false;
    madara::Knowledge_Record value;
    value = (*i)->prune (value_changes);
    if (!value_changes && dynamic_cast <Leaf_Node *> (*i) == 0)
    {
      delete *i;
      *i = new Leaf_Node (*(this->logger_), value);
    }

    if (j == 0 || value > return_value)
      return_value = value;

    can_change = can_change || value_changes;
  }

  return return_value;
}

/// Evaluates the node and its children. This does not prune any of
/// the expression tree, and is much faster than the prune function
/// @ returns    maximum value of the left and right evaluations
madara::Knowledge_Record 
madara::expression::Composite_Both_Node::evaluate (
  const madara::knowledge::Knowledge_Update_Settings & settings)
{
  madara::Knowledge_Record return_value;

  int j = 0;
  for (Component_Nodes::iterator i = nodes_.begin ();
       i != nodes_.end (); ++i, ++j)
  {
    madara::Knowledge_Record value = (*i)->evaluate (settings);

    if (j == 0 || value > return_value)
      return_value = value;
  }

  return return_value;
}

// accept a visitor
void 
madara::expression::Composite_Both_Node::accept (Visitor &visitor) const
{
  visitor.visit (*this);
}

#endif // _MADARA_NO_KARL_

#endif /* _COMPOSITE_BOTH_NODE_CPP */
