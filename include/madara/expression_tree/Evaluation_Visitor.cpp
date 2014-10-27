/* -*- C++ -*- */
#ifndef _EVALUATION_VISITOR_CPP_
#define _EVALUATION_VISITOR_CPP_

#ifdef _USE_VISITORS_

#include <iostream>
#include <typeinfo>
#include <algorithm>

#include "madara/expression_tree/Leaf_Node.h"
#include "madara/expression_tree/Variable_Node.h"
#include "madara/expression_tree/Variable_Decrement_Node.h"
#include "madara/expression_tree/Variable_Divide_Node.h"
#include "madara/expression_tree/Variable_Increment_Node.h"
#include "madara/expression_tree/Variable_Multiply_Node.h"
#include "madara/expression_tree/Variable_Compare_Node.h"
#include "madara/expression_tree/List_Node.h"
#include "madara/expression_tree/Composite_Negate_Node.h"
#include "madara/expression_tree/Composite_Predecrement_Node.h"
#include "madara/expression_tree/Composite_Preincrement_Node.h"
#include "madara/expression_tree/Composite_Not_Node.h"
#include "madara/expression_tree/Composite_Add_Node.h"
#include "madara/expression_tree/Composite_Assignment_Node.h"
#include "madara/expression_tree/Composite_And_Node.h"
#include "madara/expression_tree/Composite_Or_Node.h"
#include "madara/expression_tree/Composite_Equality_Node.h"
#include "madara/expression_tree/Composite_Inequality_Node.h"
#include "madara/expression_tree/Composite_Greater_Than_Equal_Node.h"
#include "madara/expression_tree/Composite_Greater_Than_Node.h"
#include "madara/expression_tree/Composite_Less_Than_Equal_Node.h"
#include "madara/expression_tree/Composite_Less_Than_Node.h"
#include "madara/expression_tree/Composite_Subtract_Node.h"
#include "madara/expression_tree/Composite_Divide_Node.h"
#include "madara/expression_tree/Composite_Multiply_Node.h"
#include "madara/expression_tree/Composite_Modulus_Node.h"
#include "madara/expression_tree/Composite_Both_Node.h"
#include "madara/expression_tree/Composite_Sequential_Node.h"
#include "madara/expression_tree/Composite_Function_Node.h"
#include "madara/expression_tree/Composite_For_Loop.h"
#include "madara/expression_tree/Composite_Implies_Node.h"

#include "madara/expression_tree/Evaluation_Visitor.h"

/// Evaluate a Leaf_Node (holds a static value)
void 
Madara::Expression_Tree::Evaluation_Visitor::visit (
  const Madara::Expression_Tree::Leaf_Node &node)
{
  stack_.push (node.item ());
}

/// Evaluate a Variable_Node (holds a dynamic value)
void 
Madara::Expression_Tree::Evaluation_Visitor::visit (
  const Madara::Expression_Tree::Variable_Node &node)
{
  stack_.push (node.item ());
}

/// Evaluate a Variable_Decrement_Node (holds a dynamic value)
void 
Madara::Expression_Tree::Evaluation_Visitor::visit (
  const Madara::Expression_Tree::Variable_Decrement_Node &node)
{
  stack_.push (node.item ());
}

/// Evaluate a Variable_Divide_Node (holds a dynamic value)
void 
Madara::Expression_Tree::Evaluation_Visitor::visit (
  const Madara::Expression_Tree::Variable_Divide_Node &node)
{
  stack_.push (node.item ());
}

/// Evaluate a Variable_Increment_Node (holds a dynamic value)
void 
Madara::Expression_Tree::Evaluation_Visitor::visit (
  const Madara::Expression_Tree::Variable_Increment_Node &node)
{
  stack_.push (node.item ());
}

/// Evaluate a Variable_Multiply_Node (holds a dynamic value)
void 
Madara::Expression_Tree::Evaluation_Visitor::visit (
  const Madara::Expression_Tree::Variable_Multiply_Node &node)
{
  stack_.push (node.item ());
}

/// Evaluate a Variable_Compare_Node (holds a dynamic value)
void 
Madara::Expression_Tree::Evaluation_Visitor::visit (
  const Madara::Expression_Tree::Variable_Compare_Node &node)
{
  stack_.push (node.item ());
}

/// Evaluate a List_Node (holds a dynamic list)
void 
Madara::Expression_Tree::Evaluation_Visitor::visit (
  const Madara::Expression_Tree::List_Node &node)
{
}

/// evaluation of a negation (Composite_Negate_Node)
void 
Madara::Expression_Tree::Evaluation_Visitor::visit (
  const Madara::Expression_Tree::Composite_Negate_Node &)
{
  if (stack_.size () >= 1)
    stack_.push (-stack_.pop ());
  else
  {
    MADARA_ERROR (MADARA_LOG_TERMINAL_ERROR, (LM_ERROR, DLINFO
      "\nKARL EVAL ERROR: Negate" \
      " requires a right expression"));
    exit (-1);
  }
}

/// evaluation of a decrement (Composite_Predecrement_Node)
void 
Madara::Expression_Tree::Evaluation_Visitor::visit (
  const Madara::Expression_Tree::Composite_Predecrement_Node &node)
{
  if (stack_.size () >= 1)
  {
    Madara::Knowledge_Record old_value = stack_.pop ();
    try
    {
      Variable_Node * right = dynamic_cast <Variable_Node *> (node.right ());

      Madara::Knowledge_Record new_value = --old_value;
      if (right)
      {
        new_value = right->dec ();
      }
      stack_.push (new_value);
    }
    catch (::std::bad_cast &)
    {
      stack_.push (--old_value);
    }
  }
  else
  {
    MADARA_ERROR (MADARA_LOG_TERMINAL_ERROR, (LM_ERROR, DLINFO
      "\nKARL EVAL ERROR: Predecrement" \
      " requires a right expression"));
    exit (-1);
  }
}

/// evaluation of a increment (Composite_Preincrement_Node)
void 
Madara::Expression_Tree::Evaluation_Visitor::visit (
  const Madara::Expression_Tree::Composite_Preincrement_Node &node)
{
  if (stack_.size () >= 1)
  {
    Madara::Knowledge_Record old_value = stack_.pop ();
    try
    {
      Variable_Node * right = dynamic_cast <Variable_Node *> (node.right ());

      Madara::Knowledge_Record new_value = ++old_value;
      if (right)
      {
        new_value = right->inc ();
      }
      stack_.push (new_value);
    }
    catch (::std::bad_cast &)
    {
      stack_.push (++old_value);
    }
  }
  else
  {
    MADARA_ERROR (MADARA_LOG_TERMINAL_ERROR, (LM_ERROR, DLINFO
      "\nKARL EVAL ERROR: Preincrement" \
      " requires a right expression"));
    exit (-1);
  }
}

/// evaluation of a negation (Composite_Not_Node)
void 
Madara::Expression_Tree::Evaluation_Visitor::visit (
  const Madara::Expression_Tree::Composite_Not_Node &)
{
  if (stack_.size () >= 1)
    stack_.push (!stack_.pop ());
  else
  {
    MADARA_ERROR (MADARA_LOG_TERMINAL_ERROR, (LM_ERROR, DLINFO
      "\nKARL EVAL ERROR: Not" \
      " requires a right expression"));
    exit (-1);
  }
}

/// evaluation of an addition (Composite_Add_Node)
void 
Madara::Expression_Tree::Evaluation_Visitor::visit (
  const Madara::Expression_Tree::Composite_Add_Node &)
{
  if (stack_.size () >= 2)
    stack_.push (stack_.pop () + stack_.pop ());
  else
  {
    MADARA_ERROR (MADARA_LOG_TERMINAL_ERROR, (LM_ERROR, DLINFO
      "\nKARL EVAL ERROR: Add" \
      " requires both a left and right expression"));
    exit (-1);
  }
}

/// evaluation of an assignment (Composite_Assignment_Node)
void 
Madara::Expression_Tree::Evaluation_Visitor::visit (
  const Madara::Expression_Tree::Composite_Assignment_Node &node)
{
  if (stack_.size () >= 2)
  {
    // for an assignment to be valid, we need a variable to the left
    // and an expression tree to the right. 
    try
    {
      // this is really backwards logic, but it was the only way I could think of
      // to allow for a = b = c with this type of tree and post-order flow
      Madara::Knowledge_Record right = stack_.pop ();
      stack_.pop ();
      Variable_Node * left = dynamic_cast <Variable_Node *> (node.left ());
      left->set (right.to_integer ());
      stack_.push (right);
    }
    catch (::std::bad_cast &)
    {
      MADARA_ERROR (MADARA_LOG_TERMINAL_ERROR, (LM_ERROR, DLINFO
        "\nKARL EVAL ERROR: Assignment" \
        " must have a variable as the left expression\n"));
      exit (-1);
    }
  }
  // ::std::cout << "add current top: " << stack_.top () << ::std::endl;
}

/// evaluation of a logical and comparison (Composite_And_Node)
void 
Madara::Expression_Tree::Evaluation_Visitor::visit (
  const Madara::Expression_Tree::Composite_And_Node &)
{
  if (stack_.size () >= 2)
  {
    Madara::Knowledge_Record right = stack_.pop ();
    Madara::Knowledge_Record left = stack_.pop ();

    stack_.push (left && right);
  }
  else
  {
    MADARA_ERROR (MADARA_LOG_TERMINAL_ERROR, (LM_ERROR, DLINFO
      "\nKARL EVAL ERROR: And" \
      " requires both a left and right expression\n"));
    exit (-1);
  }
}

/// evaluation of a logical or comparison (Composite_Or_Node)
void 
Madara::Expression_Tree::Evaluation_Visitor::visit (
  const Madara::Expression_Tree::Composite_Or_Node &)
{
  if (stack_.size () >= 2)
  {
    Madara::Knowledge_Record right = stack_.pop ();
    Madara::Knowledge_Record left = stack_.pop ();

    stack_.push (left || right);
  }
  else
  {
    MADARA_ERROR (MADARA_LOG_TERMINAL_ERROR, (LM_ERROR, DLINFO
      "\nKARL EVAL ERROR: Or" \
      " requires both a left and right expression\n"));
    exit (-1);
  }
}

/// evaluation of both left and right (Composite_Both_Node)
void 
Madara::Expression_Tree::Evaluation_Visitor::visit (
  const Madara::Expression_Tree::Composite_Both_Node &)
{
  if (stack_.size () >= 2)
  {
    Madara::Knowledge_Record right_v = stack_.pop ();
    Madara::Knowledge_Record left_v = stack_.pop ();

    // I was trying to use std::max, but it was giving me
    // some grief, so I just implemented it as is
    stack_.push (left_v > right_v ? left_v : right_v);
  }
  else
  {
    MADARA_ERROR (MADARA_LOG_TERMINAL_ERROR, (LM_ERROR, DLINFO
      "\nKARL EVAL ERROR: And" \
      " requires both a left and right expression\n"));
    exit (-1);
  }
}

/// evaluation of both left and right (Composite_Sequential_Node)
void 
Madara::Expression_Tree::Evaluation_Visitor::visit (
  const Madara::Expression_Tree::Composite_Sequential_Node &)
{
  if (stack_.size () >= 2)
  {
    Madara::Knowledge_Record right_v = stack_.pop ();
    Madara::Knowledge_Record left_v = stack_.pop ();

    // I was trying to use std::max, but it was giving me
    // some grief, so I just implemented it as is
    stack_.push (left_v > right_v ? right_v : left_v);
  }
  else
  {
    MADARA_ERROR (MADARA_LOG_TERMINAL_ERROR, (LM_ERROR, DLINFO
      "\nKARL EVAL ERROR: And" \
      " requires both a left and right expression\n"));
    exit (-1);
  }
}

/// evaluation of function (Composite_Function_Node)
void 
Madara::Expression_Tree::Evaluation_Visitor::visit (
  const Madara::Expression_Tree::Composite_Function_Node &)
{
}

/// evaluation of function (Composite_Function_Node)
void 
Madara::Expression_Tree::Evaluation_Visitor::visit (
  const Madara::Expression_Tree::Composite_For_Loop &)
{
}

/// evaluation of an equality comparison (Composite_Equality_Node)
void 
Madara::Expression_Tree::Evaluation_Visitor::visit (
  const Madara::Expression_Tree::Composite_Equality_Node &)
{
  if (stack_.size () >= 2)
  {
    Madara::Knowledge_Record right = stack_.pop ();
    Madara::Knowledge_Record left = stack_.pop ();

    stack_.push (left == right);
  }
  else
  {
    MADARA_ERROR (MADARA_LOG_TERMINAL_ERROR, (LM_ERROR, DLINFO
      "\nKARL EVAL ERROR: Equality" \
      " requires both a left and right expression\n"));
    exit (-1);
  }
}

/// evaluation of an inequality comparison (Composite_Inequality_Node)
void 
Madara::Expression_Tree::Evaluation_Visitor::visit (
  const Madara::Expression_Tree::Composite_Inequality_Node &)
{
  if (stack_.size () >= 2)
  {
    Madara::Knowledge_Record right = stack_.pop ();
    Madara::Knowledge_Record left = stack_.pop ();

    stack_.push (left != right);
  }
  else
  {
    MADARA_ERROR (MADARA_LOG_TERMINAL_ERROR, (LM_ERROR, DLINFO
      "\nKARL EVAL ERROR: Inequality" \
      " requires both a left and right expression\n"));
    exit (-1);
  }
}

/// evaluation of a greater than equal to comparison (Composite_Greater_Than_Equal_Node)
void 
Madara::Expression_Tree::Evaluation_Visitor::visit (
  const Madara::Expression_Tree::Composite_Greater_Than_Equal_Node &)
{
  if (stack_.size () >= 2)
  {
    Madara::Knowledge_Record right = stack_.pop ();
    Madara::Knowledge_Record left = stack_.pop ();

    stack_.push (left >= right);
  }
  else
  {
    MADARA_ERROR (MADARA_LOG_TERMINAL_ERROR, (LM_ERROR, DLINFO
      "\nKARL EVAL ERROR: Greater-than-equal" \
      " requires both a left and right expression\n"));
    exit (-1);
  }
}

/// evaluation of a greater than comparison (Composite_Greater_Than_Node)
void 
Madara::Expression_Tree::Evaluation_Visitor::visit (
  const Madara::Expression_Tree::Composite_Greater_Than_Node &)
{
  if (stack_.size () >= 2)
  {
    Madara::Knowledge_Record right = stack_.pop ();
    Madara::Knowledge_Record left = stack_.pop ();

    stack_.push (left > right);
  }
  else
  {
    MADARA_ERROR (MADARA_LOG_TERMINAL_ERROR, (LM_ERROR, DLINFO
      "\nKARL EVAL ERROR: Greater-than" \
      " requires both a left and right expression\n"));
    exit (-1);
  }
}

/// evaluation of a less than equal to comparison (Composite_Less_Than_Equal_Node)
void 
Madara::Expression_Tree::Evaluation_Visitor::visit (
  const Madara::Expression_Tree::Composite_Less_Than_Equal_Node &)
{
  if (stack_.size () >= 2)
  {
    Madara::Knowledge_Record right = stack_.pop ();
    Madara::Knowledge_Record left = stack_.pop ();

    stack_.push (left <= right);
  }
  else
  {
    MADARA_ERROR (MADARA_LOG_TERMINAL_ERROR, (LM_ERROR, DLINFO
      "\nKARL EVAL ERROR: Less-than-equal" \
      " requires both a left and right expression\n"));
    exit (-1);
  }
}

/// evaluation of a less than comparison (Composite_Less_Than_Node)
void 
Madara::Expression_Tree::Evaluation_Visitor::visit (
  const Madara::Expression_Tree::Composite_Less_Than_Node &)
{
  if (stack_.size () >= 2)
  {
    Madara::Knowledge_Record right = stack_.pop ();
    Madara::Knowledge_Record left = stack_.pop ();

    stack_.push (left < right);
  }
  else
  {
    MADARA_ERROR (MADARA_LOG_TERMINAL_ERROR, (LM_ERROR, DLINFO
      "\nKARL EVAL ERROR: Less-than" \
      " requires both a left and right expression\n"));
    exit (-1);
  }
}

/// evaluation of an addition (Composite_Subtract_Node)
void 
Madara::Expression_Tree::Evaluation_Visitor::visit (
  const Madara::Expression_Tree::Composite_Subtract_Node &)
{
  if (stack_.size () >= 2)
  {
    Madara::Knowledge_Record rhs = stack_.pop ();
    stack_.push (stack_.pop () - rhs);
  }
  else
  {
    MADARA_ERROR (MADARA_LOG_TERMINAL_ERROR, (LM_ERROR, DLINFO
      "\nKARL EVAL ERROR: Subtract" \
      " requires both a left and right expression\n"));
    exit (-1);
  }
}

/// evaluations of a division (Composite_Divide_Node)
void 
Madara::Expression_Tree::Evaluation_Visitor::visit (
  const Madara::Expression_Tree::Composite_Divide_Node &)
{
  if (stack_.size () >= 2 && stack_.top ())
  {
    Madara::Knowledge_Record rhs = stack_.pop ();
    stack_.push (stack_.pop () / rhs );
  }
  else
  {
    MADARA_ERROR (MADARA_LOG_TERMINAL_ERROR, (LM_ERROR, DLINFO
      "\nKARL EVAL ERROR: Division" \
      " requires both a left and right expression" \
      " (and right must not be 0)\n"));
    exit (-1);
  }
}

/// evaluations of a division (Composite_Multiply_Node)
void 
Madara::Expression_Tree::Evaluation_Visitor::visit (
  const Madara::Expression_Tree::Composite_Multiply_Node &)
{
  if (stack_.size () >= 2)
    stack_.push (stack_.pop () * stack_.pop ());
  else
  {
    MADARA_ERROR (MADARA_LOG_TERMINAL_ERROR, (LM_ERROR, DLINFO
      "\nKARL EVAL ERROR: Multiply" \
      " requires both a left and right expression\n"));
    exit (-1);
  }
}

/// evaluations of a division (Composite_Modulus_Node)
void 
Madara::Expression_Tree::Evaluation_Visitor::visit (
  const Madara::Expression_Tree::Composite_Modulus_Node &)
{
  if (stack_.size () >= 2 && stack_.top ())
  {
    Madara::Knowledge_Record rhs = stack_.pop ();
    stack_.push (stack_.pop () / rhs );
  }
  else
  {
    MADARA_ERROR (MADARA_LOG_TERMINAL_ERROR, (LM_ERROR, DLINFO
      "\nKARL EVAL ERROR: Modulus" \
      " requires both a left and right expression" \
      " (and right must not be 0)\n"));
    exit (-1);
  }
}

/// evaluations of a division (Composite_Modulus_Node)
void 
Madara::Expression_Tree::Evaluation_Visitor::visit (
  const Madara::Expression_Tree::Composite_Implies_Node &)
{
  if (stack_.size () >= 2)
    stack_.push (stack_.pop () ? stack_.pop () : 0);
  else
  {
    MADARA_ERROR (MADARA_LOG_TERMINAL_ERROR, (LM_ERROR, DLINFO
      "\nKARL EVAL ERROR: Implies" \
      " requires both a left and right expression"));
    exit (-1);
  }
}

/// print a total for the evaluation
int64_t 
Madara::Expression_Tree::Evaluation_Visitor::total (void)
{
  if (!stack_.is_empty ())
    return stack_.top ();
  else
    return 0;
}

/// reset the evaluation
void 
Madara::Expression_Tree::Evaluation_Visitor::reset (void)
{
  stack_.erase ();
}

#endif // _USE_VISITORS_

#endif /* _EVALUATION_VISITOR_CPP_ */
