/* -*- C++ -*- */
#ifndef _LEAF_NODE_CPP_
#define _LEAF_NODE_CPP_

#ifndef _MADARA_NO_KARL_

#include <iostream>

#include "madara/expression_tree/Component_Node.h"
#include "madara/expression_tree/Visitor.h"
#include "madara/expression_tree/Leaf_Node.h"

#include "madara/utility/Log_Macros.h"

// Ctor
Madara::Expression_Tree::Leaf_Node::Leaf_Node (const Knowledge_Record & item)
  : Component_Node (), item_ (item)
{
}

// Ctor
Madara::Expression_Tree::Leaf_Node::Leaf_Node (
  Madara::Knowledge_Record::Integer item)
  : Component_Node ()
{
  item_.set_value (item);
}

// Ctor
Madara::Expression_Tree::Leaf_Node::Leaf_Node (const std::string &item)
  : Component_Node ()
{
  item_.set_value (item);
}

// Ctor
Madara::Expression_Tree::Leaf_Node::Leaf_Node (double item)
  : Component_Node ()
{
  item_.set_value (item);
}

// Dtor
Madara::Expression_Tree::Leaf_Node::~Leaf_Node (void)
{
}

// return the item
Madara::Knowledge_Record
Madara::Expression_Tree::Leaf_Node::item (void) const
{
  return item_;
}


/// Prune the tree of unnecessary nodes. 
/// Returns evaluation of the node and sets can_change appropriately.
/// if this node can be changed, that means it shouldn't be pruned.
Madara::Knowledge_Record
Madara::Expression_Tree::Leaf_Node::prune (bool & can_change)
{
  can_change = false;

  // we could call item(), but since it is virtual, it incurs unnecessary
  // overhead.
  return item_;
}

/// Evaluates the node and its children. This does not prune any of
/// the expression tree, and is much faster than the prune function
Madara::Knowledge_Record 
Madara::Expression_Tree::Leaf_Node::evaluate (
  const Madara::Knowledge_Engine::Knowledge_Update_Settings & settings)
{
  // we could call item(), but since it is virtual, it incurs unnecessary
  // overhead.
  return item_;
}


void 
Madara::Expression_Tree::Leaf_Node::accept (Visitor &visitor) const
{
  visitor.visit (*this);
}

#endif // _MADARA_NO_KARL_

#endif /* _LEAF_NODE_CPP_ */
