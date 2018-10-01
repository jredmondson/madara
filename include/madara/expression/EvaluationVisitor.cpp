/* -*- C++ -*- */
#ifndef _EVALUATION_VISITOR_CPP_
#define _EVALUATION_VISITOR_CPP_

#ifdef _USE_VISITORS_

#include <iostream>
#include <typeinfo>
#include <algorithm>

#include "madara/expression/LeafNode.h"
#include "madara/expression/VariableNode.h"
#include "madara/expression/VariableDecrementNode.h"
#include "madara/expression/VariableDivideNode.h"
#include "madara/expression/VariableIncrementNode.h"
#include "madara/expression/VariableMultiplyNode.h"
#include "madara/expression/VariableCompareNode.h"
#include "madara/expression/ListNode.h"
#include "madara/expression/CompositeNegateNode.h"
#include "madara/expression/CompositePredecrementNode.h"
#include "madara/expression/CompositePreincrementNode.h"
#include "madara/expression/CompositeNotNode.h"
#include "madara/expression/CompositeAddNode.h"
#include "madara/expression/CompositeAssignmentNode.h"
#include "madara/expression/CompositeAndNode.h"
#include "madara/expression/CompositeOrNode.h"
#include "madara/expression/CompositeEqualityNode.h"
#include "madara/expression/CompositeInequalityNode.h"
#include "madara/expression/CompositeGreaterThanEqualNode.h"
#include "madara/expression/CompositeGreaterThanNode.h"
#include "madara/expression/CompositeLessThanEqualNode.h"
#include "madara/expression/CompositeLessThanNode.h"
#include "madara/expression/CompositeSubtractNode.h"
#include "madara/expression/CompositeDivideNode.h"
#include "madara/expression/CompositeMultiplyNode.h"
#include "madara/expression/CompositeModulusNode.h"
#include "madara/expression/CompositeBothNode.h"
#include "madara/expression/CompositeSequentialNode.h"
#include "madara/expression/CompositeFunctionNode.h"
#include "madara/expression/CompositeForLoop.h"
#include "madara/expression/CompositeImpliesNode.h"

#include "madara/expression/EvaluationVisitor.h"

/// Evaluate a LeafNode (holds a static value)
void madara::expression::EvaluationVisitor::visit(
    const madara::expression::LeafNode& node)
{
  stack_.push(node.item());
}

/// Evaluate a VariableNode (holds a dynamic value)
void madara::expression::EvaluationVisitor::visit(
    const madara::expression::VariableNode& node)
{
  stack_.push(node.item());
}

/// Evaluate a VariableDecrementNode (holds a dynamic value)
void madara::expression::EvaluationVisitor::visit(
    const madara::expression::VariableDecrementNode& node)
{
  stack_.push(node.item());
}

/// Evaluate a VariableDivideNode (holds a dynamic value)
void madara::expression::EvaluationVisitor::visit(
    const madara::expression::VariableDivideNode& node)
{
  stack_.push(node.item());
}

/// Evaluate a VariableIncrementNode (holds a dynamic value)
void madara::expression::EvaluationVisitor::visit(
    const madara::expression::VariableIncrementNode& node)
{
  stack_.push(node.item());
}

/// Evaluate a VariableMultiplyNode (holds a dynamic value)
void madara::expression::EvaluationVisitor::visit(
    const madara::expression::VariableMultiplyNode& node)
{
  stack_.push(node.item());
}

/// Evaluate a VariableCompareNode (holds a dynamic value)
void madara::expression::EvaluationVisitor::visit(
    const madara::expression::VariableCompareNode& node)
{
  stack_.push(node.item());
}

/// Evaluate a ListNode (holds a dynamic list)
void madara::expression::EvaluationVisitor::visit(
    const madara::expression::ListNode& node)
{
}

/// evaluation of a negation (CompositeNegateNode)
void madara::expression::EvaluationVisitor::visit(
    const madara::expression::CompositeNegateNode&)
{
  if (stack_.size() >= 1)
    stack_.push(-stack_.pop());
  else
  {
    MADARA_ERROR(MADARA_LOG_TERMINAL_ERROR,
        (LM_ERROR, DLINFO "\nKARL EVAL ERROR: Negate"
                          " requires a right expression"));

    throw exceptions::KarlException("madara::expression::CompositeNegateNode: "
                                    "KARL COMPILE ERROR: "
                                    "Node has no right expression\n");
  }
}

/// evaluation of a decrement (CompositePredecrementNode)
void madara::expression::EvaluationVisitor::visit(
    const madara::expression::CompositePredecrementNode& node)
{
  if (stack_.size() >= 1)
  {
    madara::knowledge::KnowledgeRecord old_value = stack_.pop();
    try
    {
      VariableNode* right = dynamic_cast<VariableNode*>(node.right());

      madara::knowledge::KnowledgeRecord new_value = --old_value;
      if (right)
      {
        new_value = right->dec();
      }
      stack_.push(new_value);
    }
    catch (::std::bad_cast&)
    {
      stack_.push(--old_value);
    }
  }
  else
  {
    MADARA_ERROR(MADARA_LOG_TERMINAL_ERROR,
        (LM_ERROR, DLINFO "\nKARL EVAL ERROR: Predecrement"
                          " requires a right expression"));
    exit(-1);
  }
}

/// evaluation of a increment (CompositePreincrementNode)
void madara::expression::EvaluationVisitor::visit(
    const madara::expression::CompositePreincrementNode& node)
{
  if (stack_.size() >= 1)
  {
    madara::knowledge::KnowledgeRecord old_value = stack_.pop();
    try
    {
      VariableNode* right = dynamic_cast<VariableNode*>(node.right());

      madara::knowledge::KnowledgeRecord new_value = ++old_value;
      if (right)
      {
        new_value = right->inc();
      }
      stack_.push(new_value);
    }
    catch (::std::bad_cast&)
    {
      stack_.push(++old_value);
    }
  }
  else
  {
    MADARA_ERROR(MADARA_LOG_TERMINAL_ERROR,
        (LM_ERROR, DLINFO "\nKARL EVAL ERROR: Preincrement"
                          " requires a right expression"));
    exit(-1);
  }
}

/// evaluation of a negation (CompositeNotNode)
void madara::expression::EvaluationVisitor::visit(
    const madara::expression::CompositeNotNode&)
{
  if (stack_.size() >= 1)
    stack_.push(!stack_.pop());
  else
  {
    MADARA_ERROR(MADARA_LOG_TERMINAL_ERROR,
        (LM_ERROR, DLINFO "\nKARL EVAL ERROR: Not"
                          " requires a right expression"));
    exit(-1);
  }
}

/// evaluation of an addition (CompositeAddNode)
void madara::expression::EvaluationVisitor::visit(
    const madara::expression::CompositeAddNode&)
{
  if (stack_.size() >= 2)
    stack_.push(stack_.pop() + stack_.pop());
  else
  {
    MADARA_ERROR(MADARA_LOG_TERMINAL_ERROR,
        (LM_ERROR, DLINFO "\nKARL EVAL ERROR: Add"
                          " requires both a left and right expression"));
    exit(-1);
  }
}

/// evaluation of an assignment (CompositeAssignmentNode)
void madara::expression::EvaluationVisitor::visit(
    const madara::expression::CompositeAssignmentNode& node)
{
  if (stack_.size() >= 2)
  {
    // for an assignment to be valid, we need a variable to the left
    // and an expression tree to the right.
    try
    {
      // this is really backwards logic, but it was the only way I could think
      // of to allow for a = b = c with this type of tree and post-order flow
      madara::knowledge::KnowledgeRecord right = stack_.pop();
      stack_.pop();
      VariableNode* left = dynamic_cast<VariableNode*>(node.left());
      left->set(right.to_integer());
      stack_.push(right);
    }
    catch (::std::bad_cast&)
    {
      MADARA_ERROR(MADARA_LOG_TERMINAL_ERROR,
          (LM_ERROR, DLINFO "\nKARL EVAL ERROR: Assignment"
                            " must have a variable as the left expression\n"));
      exit(-1);
    }
  }
  // ::std::cout << "add current top: " << stack_.top () << ::std::endl;
}

/// evaluation of a logical and comparison (CompositeAndNode)
void madara::expression::EvaluationVisitor::visit(
    const madara::expression::CompositeAndNode&)
{
  if (stack_.size() >= 2)
  {
    madara::knowledge::KnowledgeRecord right = stack_.pop();
    madara::knowledge::KnowledgeRecord left = stack_.pop();

    stack_.push(left && right);
  }
  else
  {
    MADARA_ERROR(MADARA_LOG_TERMINAL_ERROR,
        (LM_ERROR, DLINFO "\nKARL EVAL ERROR: And"
                          " requires both a left and right expression\n"));
    exit(-1);
  }
}

/// evaluation of a logical or comparison (CompositeOrNode)
void madara::expression::EvaluationVisitor::visit(
    const madara::expression::CompositeOrNode&)
{
  if (stack_.size() >= 2)
  {
    madara::knowledge::KnowledgeRecord right = stack_.pop();
    madara::knowledge::KnowledgeRecord left = stack_.pop();

    stack_.push(left || right);
  }
  else
  {
    MADARA_ERROR(MADARA_LOG_TERMINAL_ERROR,
        (LM_ERROR, DLINFO "\nKARL EVAL ERROR: Or"
                          " requires both a left and right expression\n"));
    exit(-1);
  }
}

/// evaluation of both left and right (CompositeBothNode)
void madara::expression::EvaluationVisitor::visit(
    const madara::expression::CompositeBothNode&)
{
  if (stack_.size() >= 2)
  {
    madara::knowledge::KnowledgeRecord right_v = stack_.pop();
    madara::knowledge::KnowledgeRecord left_v = stack_.pop();

    // I was trying to use std::max, but it was giving me
    // some grief, so I just implemented it as is
    stack_.push(left_v > right_v ? left_v : right_v);
  }
  else
  {
    MADARA_ERROR(MADARA_LOG_TERMINAL_ERROR,
        (LM_ERROR, DLINFO "\nKARL EVAL ERROR: And"
                          " requires both a left and right expression\n"));
    exit(-1);
  }
}

/// evaluation of both left and right (CompositeSequentialNode)
void madara::expression::EvaluationVisitor::visit(
    const madara::expression::CompositeSequentialNode&)
{
  if (stack_.size() >= 2)
  {
    madara::knowledge::KnowledgeRecord right_v = stack_.pop();
    madara::knowledge::KnowledgeRecord left_v = stack_.pop();

    // I was trying to use std::max, but it was giving me
    // some grief, so I just implemented it as is
    stack_.push(left_v > right_v ? right_v : left_v);
  }
  else
  {
    MADARA_ERROR(MADARA_LOG_TERMINAL_ERROR,
        (LM_ERROR, DLINFO "\nKARL EVAL ERROR: And"
                          " requires both a left and right expression\n"));
    exit(-1);
  }
}

/// evaluation of function (CompositeFunctionNode)
void madara::expression::EvaluationVisitor::visit(
    const madara::expression::CompositeFunctionNode&)
{
}

/// evaluation of function (CompositeFunctionNode)
void madara::expression::EvaluationVisitor::visit(
    const madara::expression::CompositeForLoop&)
{
}

/// evaluation of an equality comparison (CompositeEqualityNode)
void madara::expression::EvaluationVisitor::visit(
    const madara::expression::CompositeEqualityNode&)
{
  if (stack_.size() >= 2)
  {
    madara::knowledge::KnowledgeRecord right = stack_.pop();
    madara::knowledge::KnowledgeRecord left = stack_.pop();

    stack_.push(left == right);
  }
  else
  {
    MADARA_ERROR(MADARA_LOG_TERMINAL_ERROR,
        (LM_ERROR, DLINFO "\nKARL EVAL ERROR: Equality"
                          " requires both a left and right expression\n"));
    exit(-1);
  }
}

/// evaluation of an inequality comparison (CompositeInequalityNode)
void madara::expression::EvaluationVisitor::visit(
    const madara::expression::CompositeInequalityNode&)
{
  if (stack_.size() >= 2)
  {
    madara::knowledge::KnowledgeRecord right = stack_.pop();
    madara::knowledge::KnowledgeRecord left = stack_.pop();

    stack_.push(left != right);
  }
  else
  {
    MADARA_ERROR(MADARA_LOG_TERMINAL_ERROR,
        (LM_ERROR, DLINFO "\nKARL EVAL ERROR: Inequality"
                          " requires both a left and right expression\n"));
    exit(-1);
  }
}

/// evaluation of a greater than equal to comparison
/// (CompositeGreaterThanEqualNode)
void madara::expression::EvaluationVisitor::visit(
    const madara::expression::CompositeGreaterThanEqualNode&)
{
  if (stack_.size() >= 2)
  {
    madara::knowledge::KnowledgeRecord right = stack_.pop();
    madara::knowledge::KnowledgeRecord left = stack_.pop();

    stack_.push(left >= right);
  }
  else
  {
    MADARA_ERROR(MADARA_LOG_TERMINAL_ERROR,
        (LM_ERROR, DLINFO "\nKARL EVAL ERROR: Greater-than-equal"
                          " requires both a left and right expression\n"));
    exit(-1);
  }
}

/// evaluation of a greater than comparison (CompositeGreaterThanNode)
void madara::expression::EvaluationVisitor::visit(
    const madara::expression::CompositeGreaterThanNode&)
{
  if (stack_.size() >= 2)
  {
    madara::knowledge::KnowledgeRecord right = stack_.pop();
    madara::knowledge::KnowledgeRecord left = stack_.pop();

    stack_.push(left > right);
  }
  else
  {
    MADARA_ERROR(MADARA_LOG_TERMINAL_ERROR,
        (LM_ERROR, DLINFO "\nKARL EVAL ERROR: Greater-than"
                          " requires both a left and right expression\n"));
    exit(-1);
  }
}

/// evaluation of a less than equal to comparison (CompositeLessThanEqualNode)
void madara::expression::EvaluationVisitor::visit(
    const madara::expression::CompositeLessThanEqualNode&)
{
  if (stack_.size() >= 2)
  {
    madara::knowledge::KnowledgeRecord right = stack_.pop();
    madara::knowledge::KnowledgeRecord left = stack_.pop();

    stack_.push(left <= right);
  }
  else
  {
    MADARA_ERROR(MADARA_LOG_TERMINAL_ERROR,
        (LM_ERROR, DLINFO "\nKARL EVAL ERROR: Less-than-equal"
                          " requires both a left and right expression\n"));
    exit(-1);
  }
}

/// evaluation of a less than comparison (CompositeLessThanNode)
void madara::expression::EvaluationVisitor::visit(
    const madara::expression::CompositeLessThanNode&)
{
  if (stack_.size() >= 2)
  {
    madara::knowledge::KnowledgeRecord right = stack_.pop();
    madara::knowledge::KnowledgeRecord left = stack_.pop();

    stack_.push(left < right);
  }
  else
  {
    MADARA_ERROR(MADARA_LOG_TERMINAL_ERROR,
        (LM_ERROR, DLINFO "\nKARL EVAL ERROR: Less-than"
                          " requires both a left and right expression\n"));
    exit(-1);
  }
}

/// evaluation of an addition (CompositeSubtractNode)
void madara::expression::EvaluationVisitor::visit(
    const madara::expression::CompositeSubtractNode&)
{
  if (stack_.size() >= 2)
  {
    madara::knowledge::KnowledgeRecord rhs = stack_.pop();
    stack_.push(stack_.pop() - rhs);
  }
  else
  {
    MADARA_ERROR(MADARA_LOG_TERMINAL_ERROR,
        (LM_ERROR, DLINFO "\nKARL EVAL ERROR: Subtract"
                          " requires both a left and right expression\n"));
    exit(-1);
  }
}

/// evaluations of a division (CompositeDivideNode)
void madara::expression::EvaluationVisitor::visit(
    const madara::expression::CompositeDivideNode&)
{
  if (stack_.size() >= 2 && stack_.top())
  {
    madara::knowledge::KnowledgeRecord rhs = stack_.pop();
    stack_.push(stack_.pop() / rhs);
  }
  else
  {
    MADARA_ERROR(MADARA_LOG_TERMINAL_ERROR,
        (LM_ERROR, DLINFO "\nKARL EVAL ERROR: Division"
                          " requires both a left and right expression"
                          " (and right must not be 0)\n"));
    exit(-1);
  }
}

/// evaluations of a division (CompositeMultiplyNode)
void madara::expression::EvaluationVisitor::visit(
    const madara::expression::CompositeMultiplyNode&)
{
  if (stack_.size() >= 2)
    stack_.push(stack_.pop() * stack_.pop());
  else
  {
    MADARA_ERROR(MADARA_LOG_TERMINAL_ERROR,
        (LM_ERROR, DLINFO "\nKARL EVAL ERROR: Multiply"
                          " requires both a left and right expression\n"));
    exit(-1);
  }
}

/// evaluations of a division (CompositeModulusNode)
void madara::expression::EvaluationVisitor::visit(
    const madara::expression::CompositeModulusNode&)
{
  if (stack_.size() >= 2 && stack_.top())
  {
    madara::knowledge::KnowledgeRecord rhs = stack_.pop();
    stack_.push(stack_.pop() / rhs);
  }
  else
  {
    MADARA_ERROR(MADARA_LOG_TERMINAL_ERROR,
        (LM_ERROR, DLINFO "\nKARL EVAL ERROR: Modulus"
                          " requires both a left and right expression"
                          " (and right must not be 0)\n"));
    exit(-1);
  }
}

/// evaluations of a division (CompositeModulusNode)
void madara::expression::EvaluationVisitor::visit(
    const madara::expression::CompositeImpliesNode&)
{
  if (stack_.size() >= 2)
    stack_.push(stack_.pop() ? stack_.pop() : 0);
  else
  {
    MADARA_ERROR(MADARA_LOG_TERMINAL_ERROR,
        (LM_ERROR, DLINFO "\nKARL EVAL ERROR: Implies"
                          " requires both a left and right expression"));
    exit(-1);
  }
}

/// print a total for the evaluation
int64_t madara::expression::EvaluationVisitor::total(void)
{
  if (!stack_.is_empty())
    return stack_.top();
  else
    return 0;
}

/// reset the evaluation
void madara::expression::EvaluationVisitor::reset(void)
{
  stack_.erase();
}

#endif  // _USE_VISITORS_

#endif /* _EVALUATION_VISITOR_CPP_ */
