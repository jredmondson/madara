/* -*- C++ -*- */
#ifndef _COMPOSITE_PREDECREMENT_NODE_CPP_
#define _COMPOSITE_PREDECREMENT_NODE_CPP_

#ifndef _MADARA_NO_KARL_

#include <iostream>

#include "madara/expression/Component_Node.h"
#include "madara/expression/Composite_Unary_Node.h"
#include "madara/expression/Visitor.h"
#include "madara/expression/Composite_Predecrement_Node.h"
#include "madara/expression/Leaf_Node.h"
#include "madara/expression/Variable_Node.h"

madara::expression::Composite_Predecrement_Node::Composite_Predecrement_Node (
  logger::Logger & logger, Component_Node *right)
: Composite_Unary_Node (logger, right)
{
  var_ = dynamic_cast <Variable_Node *> (right);

  if (!var_)
    array_ = dynamic_cast <Composite_Array_Reference *> (right);
}

// Dtor
madara::expression::Composite_Predecrement_Node::~Composite_Predecrement_Node (void)
{
}

madara::Knowledge_Record
madara::expression::Composite_Predecrement_Node::item (void) const
{
  madara::Knowledge_Record record;
  record.set_value ("--");
  return record;
}

/// Prune the tree of unnecessary nodes. 
/// Returns evaluation of the node and sets can_change appropriately.
/// if this node can be changed, that means it shouldn't be pruned.
madara::Knowledge_Record
madara::expression::Composite_Predecrement_Node::prune (bool & can_change)
{
  bool right_child_can_change = false;
  madara::Knowledge_Record right_value;

  if (this->right_)
  {
    right_value = this->right_->prune (right_child_can_change);
    if (!right_child_can_change && dynamic_cast <Leaf_Node *> (right_) == 0)
    {
      delete this->right_;
      this->right_ = new Leaf_Node (*(this->logger_), right_value);
    }
  }
  else
  {
    madara_logger_ptr_log (logger_, logger::LOG_EMERGENCY,
      "KARL COMPILE ERROR: Predecrement has no right expression\n");

    exit (-1);
  }

  can_change = right_child_can_change;

  return --right_value;
}

/// Evaluates the node and its children. This does not prune any of
/// the expression tree, and is much faster than the prune function
madara::Knowledge_Record 
madara::expression::Composite_Predecrement_Node::evaluate (
  const madara::knowledge::Knowledge_Update_Settings & settings)
{
  if (var_)
    return var_->dec (settings);
  else if (array_)
    return array_->dec (settings);
  else
    return --(this->right_->evaluate (settings));
}

// accept a visitor
void 
madara::expression::Composite_Predecrement_Node::accept (Visitor &visitor) const
{
  visitor.visit (*this);
}

#endif // _MADARA_NO_KARL_

#endif /* _COMPOSITE_PREDECREMENT_NODE_CPP_ */
