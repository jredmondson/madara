/* -*- C++ -*- */

#ifndef _ASSIGNMENT_NODE_CPP_
#define _ASSIGNMENT_NODE_CPP_

#ifndef _MADARA_NO_KARL_

#include <iostream>

#include "madara/expression_tree/Component_Node.h"
#include "madara/expression_tree/Visitor.h"
#include "madara/expression_tree/Composite_Assignment_Node.h"
#include "madara/expression_tree/Leaf_Node.h"


// Ctor

madara::expression_tree::Composite_Assignment_Node::Composite_Assignment_Node (
  logger::Logger & logger,
  Component_Node *left, Component_Node *right)
: Composite_Unary_Node (logger, right)
{
  var_ = dynamic_cast <Variable_Node *> (left);

  if (!var_)
    array_ = dynamic_cast <Composite_Array_Reference *> (left);
}

madara::Knowledge_Record
madara::expression_tree::Composite_Assignment_Node::item (void) const
{
  return "=";
}


madara::Knowledge_Record
madara::expression_tree::Composite_Assignment_Node::prune (bool & can_change)
{
  bool left_child_can_change = false;
  bool right_child_can_change = false;
  madara::Knowledge_Record right_value;

  if (this->var_ != 0 || this->array_ != 0)
    left_child_can_change = true;
  else
  {
    madara_logger_ptr_log (logger_, logger::LOG_EMERGENCY,
      "KARL COMPILE ERROR: Variable assignment has no variable\n");
    exit (-1);    
  }

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
      "KARL COMPILE ERROR: Assignment has no right expression\n");

    exit (-1);
  }

  can_change = left_child_can_change || right_child_can_change;

  return right_value;
}

/// Evaluates the node and its children. This does not prune any of
/// the expression tree, and is much faster than the prune function
madara::Knowledge_Record 
madara::expression_tree::Composite_Assignment_Node::evaluate (
  const madara::knowledge::Knowledge_Update_Settings & settings)
{
  madara::Knowledge_Record rhs = right_->evaluate (settings);

  // get the value from the right side and set the variable's value with it
  //madara::Knowledge_Record value = right_->evaluate ();
  if (var_)
  {
    madara_logger_ptr_log (logger_, logger::LOG_MINOR,
      "Composite_Assignment_Node::evaluate: "
      "Attempting to set variable %s to %s.\n",
      var_->expand_key ().c_str (),
      rhs.to_string ().c_str ());

    var_->set (rhs, settings);
  }
  else if (array_)
  {
    madara_logger_ptr_log (logger_, logger::LOG_MINOR,
      "Composite_Assignment_Node::evaluate: "
      "Attempting to set index of var %s to %s.\n",
      array_->expand_key ().c_str (),
      rhs.to_string ().c_str ());

    array_->set (rhs, settings);
  }
  else
  {
    madara_logger_ptr_log (logger_, logger::LOG_MINOR,
      "left hand side was neither a variable nor an array reference. "
      "Check your expression for errors.\n");
  }

  // return the value
  return rhs;
}


// accept a visitor
void 
madara::expression_tree::Composite_Assignment_Node::accept (Visitor &visitor) const
{
  visitor.visit (*this);
}

#endif // _MADARA_NO_KARL_

#endif /* _ASSIGNMENT_NODE_CPP_ */
