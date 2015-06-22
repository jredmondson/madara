
#ifndef _MADARA_NO_KARL_
#include "madara/expression_tree/Visitor.h"
#include "madara/expression_tree/Leaf_Node.h"
#include "madara/expression_tree/Variable_Compare_Node.h"
#include "madara/utility/Utility.h"


#include <string>
#include <sstream>

Madara::Expression_Tree::Variable_Compare_Node::Variable_Compare_Node (
  Component_Node * lhs, Madara::Knowledge_Record value, int compare_type,
  Component_Node * rhs, Madara::Knowledge_Engine::Thread_Safe_Context &context)
: Component_Node (context.get_logger ()), var_ (0), array_ (0),
  value_ (value), rhs_ (rhs),
  compare_type_ (compare_type), context_ (context)
{
  var_ = dynamic_cast <Variable_Node *> (lhs);

  if (!var_)
    array_ = dynamic_cast <Composite_Array_Reference *> (lhs);
}

Madara::Expression_Tree::Variable_Compare_Node::~Variable_Compare_Node ()
{
  // do not clean up record_. Let the context clean that up.
}

void
Madara::Expression_Tree::Variable_Compare_Node::accept (Visitor &visitor) const
{
  visitor.visit (*this);
}

Madara::Knowledge_Record
Madara::Expression_Tree::Variable_Compare_Node::item () const
{
  Knowledge_Record value;

  if (var_)
    value = var_->item ();
  else if (array_)
    value = array_->item ();

  return value;
}

/// Prune the tree of unnecessary nodes. 
/// Returns evaluation of the node and sets can_change appropriately.
/// if this node can be changed, that means it shouldn't be pruned.
Madara::Knowledge_Record
Madara::Expression_Tree::Variable_Compare_Node::prune (bool & can_change)
{
  bool left_child_can_change = false;
  bool right_child_can_change = false;
  Madara::Knowledge_Record right_value;

  if (this->var_ != 0 || this->array_ != 0)
    left_child_can_change = true;
  else
  {
    logger_->log (Logger::LOG_EMERGENCY,
      "KARL COMPILE ERROR: Variable assignment has no variable\\n");

    exit (-1);    
  }

  if (this->rhs_)
  {
    right_value = this->rhs_->prune (right_child_can_change);
    if (!right_child_can_change && dynamic_cast <Leaf_Node *> (rhs_) == 0)
    {
      delete this->rhs_;
      this->rhs_ = new Leaf_Node (*(this->logger_), right_value);
    }
  }
  else
  {
    logger_->log (Logger::LOG_EMERGENCY,
      "KARL COMPILE ERROR: Assignment has no right expression\n");

    exit (-1);
  }

  can_change = left_child_can_change || right_child_can_change;

  return right_value;
}

/// Evaluates the node and its children. This does not prune any of
/// the expression tree, and is much faster than the prune function
Madara::Knowledge_Record 
Madara::Expression_Tree::Variable_Compare_Node::evaluate (
  const Madara::Knowledge_Engine::Knowledge_Update_Settings & settings)
{
  Madara::Knowledge_Record lhs;

  if (var_)
    lhs = var_->evaluate (settings);
  else if (array_)
    lhs = array_->evaluate (settings);

  if (rhs_)
  {
    if      (compare_type_ == LESS_THAN)
      return lhs < rhs_->evaluate (settings);
    else if (compare_type_ == LESS_THAN_EQUAL)
      return lhs <= rhs_->evaluate (settings);
    else if (compare_type_ == EQUAL)
      return lhs == rhs_->evaluate (settings);
    else if (compare_type_ == GREATER_THAN_EQUAL)
      return lhs >= rhs_->evaluate (settings);
    else
      return lhs > rhs_->evaluate (settings);
  }
  else
  {
    if      (compare_type_ == LESS_THAN)
      return lhs < value_;
    else if (compare_type_ == LESS_THAN_EQUAL)
      return lhs <= value_;
    else if (compare_type_ == EQUAL)
      return lhs == value_;
    else if (compare_type_ == GREATER_THAN_EQUAL)
      return lhs >= value_;
    else
      return lhs > value_;
  }
}

#endif // _MADARA_NO_KARL_
