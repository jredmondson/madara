/* -*- C++ -*- */
#ifndef _MODULUS_NODE_CPP_
#define _MODULUS_NODE_CPP_

#ifndef _MADARA_NO_KARL_

#include <iostream>

#include "madara/expression/Component_Node.h"
#include "madara/expression/Composite_Binary_Node.h"
#include "madara/expression/Composite_Modulus_Node.h"
#include "madara/expression/Visitor.h"
#include "madara/expression/Leaf_Node.h"

madara::expression::Composite_Modulus_Node::Composite_Modulus_Node (
  logger::Logger & logger, Component_Node * left, Component_Node * right)
  : Composite_Binary_Node (logger, left, right)
{    
}

// Dtor
madara::expression::Composite_Modulus_Node::~Composite_Modulus_Node (void)
{
}

madara::Knowledge_Record
madara::expression::Composite_Modulus_Node::item (void) const
{
  madara::Knowledge_Record record;
  record.set_value ("%");
  return record;
}


/// Prune the tree of unnecessary nodes. 
/// Returns evaluation of the node and sets can_change appropriately.
/// if this node can be changed, that means it shouldn't be pruned.
madara::Knowledge_Record
madara::expression::Composite_Modulus_Node::prune (bool & can_change)
{
  bool left_child_can_change = false;
  bool right_child_can_change = false;
  madara::Knowledge_Record left_value;
  madara::Knowledge_Record right_value;

  if (this->left_)
  {
    left_value = this->left_->prune (left_child_can_change);
    if (!left_child_can_change && dynamic_cast <Leaf_Node *> (left_) == 0)
    {
      delete this->left_;
      this->left_ = new Leaf_Node (*(this->logger_), left_value);
    }
  }
  else
  {
    madara_logger_ptr_log (logger_, logger::LOG_EMERGENCY,
      "KARL COMPILE ERROR: Modulus has no left expression\n");

    exit (-1); 
  }

  if (this->right_)
  {
    right_value = this->right_->prune (right_child_can_change);
    if (!right_child_can_change && dynamic_cast <Leaf_Node *> (right_) == 0)
    {
      delete this->right_;
      this->right_ = new Leaf_Node (*(this->logger_), right_value);

      if (right_value == madara::Knowledge_Record::Integer (0))
      {
        madara_logger_ptr_log (logger_, logger::LOG_EMERGENCY,
          "KARL COMPILE ERROR: Modulus results in permanent divide by zero\n");

        exit (-1);
      }
    }
  }
  else
  {
    madara_logger_ptr_log (logger_, logger::LOG_EMERGENCY,
      "KARL COMPILE ERROR: "
      "Modulus has no right expression (divide by zero)\n");

    exit (-1);
  }

  can_change = left_child_can_change || right_child_can_change;

  return left_value % right_value;
}

/// Evaluates the node and its children. This does not prune any of
/// the expression tree, and is much faster than the prune function
madara::Knowledge_Record 
madara::expression::Composite_Modulus_Node::evaluate (
  const madara::knowledge::Knowledge_Update_Settings & settings)
{
  return left_->evaluate (settings) % right_->evaluate (settings);
}


// accept a visitor
void 
madara::expression::Composite_Modulus_Node::accept (Visitor& visitor) const
{
  visitor.visit (*this);
}

#endif // _MADARA_NO_KARL_

#endif /* _MODULUS_NODE_CPP_ */
