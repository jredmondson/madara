/* -*- C++ -*- */

#ifndef COMPOSITE_AND_NODE_CPP
#define COMPOSITE_AND_NODE_CPP

#ifndef _MADARA_NO_KARL_

#include <iostream>

#include "madara/expression_tree/Component_Node.h"
#include "madara/expression_tree/Visitor.h"
#include "madara/expression_tree/Composite_And_Node.h"
#include "madara/expression_tree/Leaf_Node.h"


// Ctor

madara::expression_tree::Composite_And_Node::Composite_And_Node (
  logger::Logger & logger, const Component_Nodes & nodes)
: madara::expression_tree::Composite_Ternary_Node (logger, nodes)
{
}

madara::Knowledge_Record
madara::expression_tree::Composite_And_Node::item (void) const
{
  return "&&";
}


/// Prune the tree of unnecessary nodes. 
/// Returns evaluation of the node and sets can_change appropriately.
/// if this node can be changed, that means it shouldn't be pruned.
madara::Knowledge_Record
madara::expression_tree::Composite_And_Node::prune (bool & can_change)
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
      *i = new Leaf_Node (*this->logger_, value);
    }

    if (j == 0)
      return_value = value;
    else
      return_value = value && return_value;

    can_change = can_change || value_changes;
  }

  return return_value;
}

/// Evaluates the node and its children. This does not prune any of
/// the expression tree, and is much faster than the prune function
madara::Knowledge_Record 
madara::expression_tree::Composite_And_Node::evaluate (
  const madara::knowledge::Knowledge_Update_Settings & settings)
{
  int j = 0;
  for (Component_Nodes::iterator i = nodes_.begin ();
       i != nodes_.end (); ++i, ++j)
  {
    // if we have a zero eval, return 0 immediately
    if ((*i)->evaluate (settings).is_false ())
      return madara::Knowledge_Record::Integer ();
  }

  // if everything was true, return true
  return madara::Knowledge_Record::Integer (1);
}


// accept a visitor
void 
madara::expression_tree::Composite_And_Node::accept (Visitor &visitor) const
{
  visitor.visit (*this);
}

#endif // _MADARA_NO_KARL_

#endif /* COMPOSITE_AND_NODE_CPP */
