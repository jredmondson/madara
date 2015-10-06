/* -*- C++ -*- */
#ifndef _LEAF_NODE_CPP_
#define _LEAF_NODE_CPP_

#ifndef _MADARA_NO_KARL_

#include <iostream>

#include "madara/expression/Component_Node.h"
#include "madara/expression/Visitor.h"
#include "madara/expression/Leaf_Node.h"



// Ctor
madara::expression::Leaf_Node::Leaf_Node (
  logger::Logger & logger, const Knowledge_Record & item)
: Component_Node (logger), item_ (item)
{
}

// Ctor
madara::expression::Leaf_Node::Leaf_Node (
  logger::Logger & logger, madara::Knowledge_Record::Integer item)
: Component_Node (logger)
{
  item_.set_value (item);
}

// Ctor
madara::expression::Leaf_Node::Leaf_Node (
  logger::Logger & logger, const std::string &item)
: Component_Node (logger)
{
  item_.set_value (item);
}

// Ctor
madara::expression::Leaf_Node::Leaf_Node (
  logger::Logger & logger, double item)
: Component_Node (logger)
{
  item_.set_value (item);
}

// Dtor
madara::expression::Leaf_Node::~Leaf_Node (void)
{
}

// return the item
madara::Knowledge_Record
madara::expression::Leaf_Node::item (void) const
{
  return item_;
}


/// Prune the tree of unnecessary nodes. 
/// Returns evaluation of the node and sets can_change appropriately.
/// if this node can be changed, that means it shouldn't be pruned.
madara::Knowledge_Record
madara::expression::Leaf_Node::prune (bool & can_change)
{
  can_change = false;

  // we could call item(), but since it is virtual, it incurs unnecessary
  // overhead.
  return item_;
}

/// Evaluates the node and its children. This does not prune any of
/// the expression tree, and is much faster than the prune function
madara::Knowledge_Record 
madara::expression::Leaf_Node::evaluate (
  const madara::knowledge::Knowledge_Update_Settings & settings)
{
  // we could call item(), but since it is virtual, it incurs unnecessary
  // overhead.
  return item_;
}


void 
madara::expression::Leaf_Node::accept (Visitor &visitor) const
{
  visitor.visit (*this);
}

#endif // _MADARA_NO_KARL_

#endif /* _LEAF_NODE_CPP_ */
