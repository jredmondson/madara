/* -*- C++ -*- */
#ifndef _LEAF_NODE_CPP_
#define _LEAF_NODE_CPP_

#ifndef _MADARA_NO_KARL_

#include <iostream>

#include "madara/expression/ComponentNode.h"
#include "madara/expression/Visitor.h"
#include "madara/expression/LeafNode.h"



// Ctor
madara::expression::LeafNode::LeafNode (
  logger::Logger & logger, const knowledge::KnowledgeRecord & item)
: ComponentNode (logger), item_ (item)
{
}

// Ctor
madara::expression::LeafNode::LeafNode (
  logger::Logger & logger, madara::knowledge::KnowledgeRecord::Integer item)
: ComponentNode (logger)
{
  item_.set_value (item);
}

// Ctor
madara::expression::LeafNode::LeafNode (
  logger::Logger & logger, const std::string &item)
: ComponentNode (logger)
{
  item_.set_value (item);
}

// Ctor
madara::expression::LeafNode::LeafNode (
  logger::Logger & logger, double item)
: ComponentNode (logger)
{
  item_.set_value (item);
}

// Dtor
madara::expression::LeafNode::~LeafNode (void)
{
}

// return the item
madara::knowledge::KnowledgeRecord
madara::expression::LeafNode::item (void) const
{
  return item_;
}


/// Prune the tree of unnecessary nodes. 
/// Returns evaluation of the node and sets can_change appropriately.
/// if this node can be changed, that means it shouldn't be pruned.
madara::knowledge::KnowledgeRecord
madara::expression::LeafNode::prune (bool & can_change)
{
  can_change = false;

  // we could call item(), but since it is virtual, it incurs unnecessary
  // overhead.
  return item_;
}

/// Evaluates the node and its children. This does not prune any of
/// the expression tree, and is much faster than the prune function
madara::knowledge::KnowledgeRecord 
madara::expression::LeafNode::evaluate (
  const madara::knowledge::KnowledgeUpdateSettings & /*settings*/)
{
  // we could call item(), but since it is virtual, it incurs unnecessary
  // overhead.
  return item_;
}


void 
madara::expression::LeafNode::accept (Visitor &visitor) const
{
  visitor.visit (*this);
}

#endif // _MADARA_NO_KARL_

#endif /* _LEAF_NODE_CPP_ */
