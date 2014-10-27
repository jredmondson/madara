/* -*- C++ -*- */

#ifndef COMPOSITE_OR_NODE_CPP
#define COMPOSITE_OR_NODE_CPP

#include <iostream>

#include "madara/expression_tree/Component_Node.h"
#include "madara/expression_tree/Visitor.h"
#include "madara/expression_tree/Composite_Or_Node.h"
#include "madara/expression_tree/Leaf_Node.h"

#include "madara/utility/Log_Macros.h"

// Ctor

Madara::Expression_Tree::Composite_Or_Node::Composite_Or_Node (
  const Component_Nodes & nodes)
: Madara::Expression_Tree::Composite_Ternary_Node (nodes)
{
}

Madara::Knowledge_Record
Madara::Expression_Tree::Composite_Or_Node::item (void) const
{
  return "||";
}

/// Prune the tree of unnecessary nodes. 
/// Returns evaluation of the node and sets can_change appropriately.
/// if this node can be changed, that means it shouldn't be pruned.
Madara::Knowledge_Record
Madara::Expression_Tree::Composite_Or_Node::prune (bool & can_change)
{
  Madara::Knowledge_Record return_value;

  int j = 0;
  for (Component_Nodes::iterator i = nodes_.begin ();
       i != nodes_.end (); ++i, ++j)
  {
    bool value_changes = false;
    Madara::Knowledge_Record value;
    value = (*i)->prune (value_changes);
    if (!value_changes && dynamic_cast <Leaf_Node *> (*i) == 0)
    {
      delete *i;
      *i = new Leaf_Node (value);
    }

    if (j == 0)
      return_value = value;
    else
      return_value = value || return_value;

    can_change = can_change || value_changes;
  }

  return return_value;
}

/// Evaluates the node and its children. This does not prune any of
/// the expression tree, and is much faster than the prune function
Madara::Knowledge_Record 
Madara::Expression_Tree::Composite_Or_Node::evaluate (
  const Madara::Knowledge_Engine::Knowledge_Update_Settings & settings)
{
  int j = 0;
  for (Component_Nodes::iterator i = nodes_.begin ();
       i != nodes_.end (); ++i, ++j)
  {
    // if we have a non-zero eval, return 1 immediately
    if ((*i)->evaluate (settings).is_true ())
      return Madara::Knowledge_Record::Integer (1);
  }

  // if nothing was true, return false
  return Madara::Knowledge_Record::Integer ();
}

// accept a visitor
void 
Madara::Expression_Tree::Composite_Or_Node::accept (Visitor &visitor) const
{
  visitor.visit (*this);
}

#endif /* COMPOSITE_OR_NODE_CPP */
