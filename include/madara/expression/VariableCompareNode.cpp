
#ifndef _MADARA_NO_KARL_
#include "madara/expression/Visitor.h"
#include "madara/expression/LeafNode.h"
#include "madara/expression/VariableCompareNode.h"
#include "madara/utility/Utility.h"

typedef madara::knowledge::KnowledgeRecord  KnowledgeRecord;

#include <string>
#include <sstream>

madara::expression::VariableCompareNode::VariableCompareNode (
  ComponentNode * lhs, madara::knowledge::KnowledgeRecord value, int compare_type,
  ComponentNode * rhs, madara::knowledge::ThreadSafeContext &context)
: ComponentNode (context.get_logger ()), var_ (0), array_ (0),
  value_ (value), rhs_ (rhs),
  compare_type_ (compare_type), context_ (context)
{
  var_ = dynamic_cast <VariableNode *> (lhs);

  if (!var_)
    array_ = dynamic_cast <CompositeArrayReference *> (lhs);
}

madara::expression::VariableCompareNode::~VariableCompareNode ()
{
  // do not clean up record_. Let the context clean that up.
}

void
madara::expression::VariableCompareNode::accept (Visitor &visitor) const
{
  visitor.visit (*this);
}

KnowledgeRecord
madara::expression::VariableCompareNode::item () const
{
  knowledge::KnowledgeRecord value;

  if (var_)
    value = var_->item ();
  else if (array_)
    value = array_->item ();

  return value;
}

/// Prune the tree of unnecessary nodes. 
/// Returns evaluation of the node and sets can_change appropriately.
/// if this node can be changed, that means it shouldn't be pruned.
KnowledgeRecord
madara::expression::VariableCompareNode::prune (bool & can_change)
{
  bool left_child_can_change = false;
  bool right_child_can_change = false;
  madara::knowledge::KnowledgeRecord right_value;

  if (this->var_ != 0 || this->array_ != 0)
    left_child_can_change = true;
  else
  {
    madara_logger_ptr_log (logger_, logger::LOG_EMERGENCY,
      "KARL COMPILE ERROR: Variable assignment has no variable\\n");

    exit (-1);    
  }

  if (this->rhs_)
  {
    right_value = this->rhs_->prune (right_child_can_change);
    if (!right_child_can_change && dynamic_cast <LeafNode *> (rhs_) == 0)
    {
      delete this->rhs_;
      this->rhs_ = new LeafNode (*(this->logger_), right_value);
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
KnowledgeRecord 
madara::expression::VariableCompareNode::evaluate (
  const madara::knowledge::KnowledgeUpdateSettings & settings)
{
  KnowledgeRecord lhs;
  KnowledgeRecord::Integer result (0);

  if (var_)
    lhs = var_->evaluate (settings);
  else if (array_)
    lhs = array_->evaluate (settings);

  if (rhs_)
  {
    if (compare_type_ == LESS_THAN)
    {
      result = lhs < rhs_->evaluate (settings);
    }
    else if (compare_type_ == LESS_THAN_EQUAL)
    {
      result = lhs <= rhs_->evaluate (settings);
    }
    else if (compare_type_ == EQUAL)
    {
      result = lhs == rhs_->evaluate (settings);
    }
    else if (compare_type_ == GREATER_THAN_EQUAL)
    {
      result = lhs >= rhs_->evaluate (settings);
    }
    else
    {
      result = lhs > rhs_->evaluate (settings);
    }
  }
  else
  {
    if (compare_type_ == LESS_THAN)
    {
      result = lhs < value_;
    }
    else if (compare_type_ == LESS_THAN_EQUAL)
    {
      result = lhs <= value_;
    }
    else if (compare_type_ == EQUAL)
    {
      result = lhs == value_;
    }
    else if (compare_type_ == GREATER_THAN_EQUAL)
    {
      result = lhs >= value_;
    }
    else
    {
      result = lhs > value_;
    }
  }

  return result;
}

#endif // _MADARA_NO_KARL_
