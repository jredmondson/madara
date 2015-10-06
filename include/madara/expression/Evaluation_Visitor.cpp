/* -*- C++ -*- */
#ifndef _EVALUATION_VISITOR_CPP_
#define _EVALUATION_VISITOR_CPP_

#ifdef _USE_VISITORS_

#include <iostream>
#include <typeinfo>
#include <algorithm>

#include "madara/expression/Leaf_Node.h"
#include "madara/expression/Variable_Node.h"
#include "madara/expression/Variable_Decrement_Node.h"
#include "madara/expression/Variable_Divide_Node.h"
#include "madara/expression/Variable_Increment_Node.h"
#include "madara/expression/Variable_Multiply_Node.h"
#include "madara/expression/Variable_Compare_Node.h"
#include "madara/expression/List_Node.h"
#include "madara/expression/Composite_Negate_Node.h"
#include "madara/expression/Composite_Predecrement_Node.h"
#include "madara/expression/Composite_Preincrement_Node.h"
#include "madara/expression/Composite_Not_Node.h"
#include "madara/expression/Composite_Add_Node.h"
#include "madara/expression/Composite_Assignment_Node.h"
#include "madara/expression/Composite_And_Node.h"
#include "madara/expression/Composite_Or_Node.h"
#include "madara/expression/Composite_Equality_Node.h"
#include "madara/expression/Composite_Inequality_Node.h"
#include "madara/expression/Composite_Greater_Than_Equal_Node.h"
#include "madara/expression/Composite_Greater_Than_Node.h"
#include "madara/expression/Composite_Less_Than_Equal_Node.h"
#include "madara/expression/Composite_Less_Than_Node.h"
#include "madara/expression/Composite_Subtract_Node.h"
#include "madara/expression/Composite_Divide_Node.h"
#include "madara/expression/Composite_Multiply_Node.h"
#include "madara/expression/Composite_Modulus_Node.h"
#include "madara/expression/Composite_Both_Node.h"
#include "madara/expression/Composite_Sequential_Node.h"
#include "madara/expression/Composite_Function_Node.h"
#include "madara/expression/Composite_For_Loop.h"
#include "madara/expression/Composite_Implies_Node.h"

#include "madara/expression/Evaluation_Visitor.h"

/// Evaluate a Leaf_Node (holds a static value)
void 
madara::expression::Evaluation_Visitor::visit (
  const madara::expression::Leaf_Node &node)
{
  stack_.push (node.item ());
}

/// Evaluate a Variable_Node (holds a dynamic value)
void 
madara::expression::Evaluation_Visitor::visit (
  const madara::expression::Variable_Node &node)
{
  stack_.push (node.item ());
}

/// Evaluate a Variable_Decrement_Node (holds a dynamic value)
void 
madara::expression::Evaluation_Visitor::visit (
  const madara::expression::Variable_Decrement_Node &node)
{
  stack_.push (node.item ());
}

/// Evaluate a Variable_Divide_Node (holds a dynamic value)
void 
madara::expression::Evaluation_Visitor::visit (
  const madara::expression::Variable_Divide_Node &node)
{
  stack_.push (node.item ());
}

/// Evaluate a Variable_Increment_Node (holds a dynamic value)
void 
madara::expression::Evaluation_Visitor::visit (
  const madara::expression::Variable_Increment_Node &node)
{
  stack_.push (node.item ());
}

/// Evaluate a Variable_Multiply_Node (holds a dynamic value)
void 
madara::expression::Evaluation_Visitor::visit (
  const madara::expression::Variable_Multiply_Node &node)
{
  stack_.push (node.item ());
}

/// Evaluate a Variable_Compare_Node (holds a dynamic value)
void 
madara::expression::Evaluation_Visitor::visit (
  const madara::expression::Variable_Compare_Node &node)
{
  stack_.push (node.item ());
}

/// Evaluate a List_Node (holds a dynamic list)
void 
madara::expression::Evaluation_Visitor::visit (
  const madara::expression::List_Node &node)
{
}

/// evaluation of a negation (Composite_Negate_Node)
void 
madara::expression::Evaluation_Visitor::visit (
  const madara::expression::Composite_Negate_Node &)
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
madara::expression::Evaluation_Visitor::visit (
  const madara::expression::Composite_Predecrement_Node &node)
{
  if (stack_.size () >= 1)
  {
    madara::Knowledge_Record old_value = stack_.pop ();
    try
    {
      Variable_Node * right = dynamic_cast <Variable_Node *> (node.right ());

      madara::Knowledge_Record new_value = --old_value;
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
madara::expression::Evaluation_Visitor::visit (
  const madara::expression::Composite_Preincrement_Node &node)
{
  if (stack_.size () >= 1)
  {
    madara::Knowledge_Record old_value = stack_.pop ();
    try
    {
      Variable_Node * right = dynamic_cast <Variable_Node *> (node.right ());

      madara::Knowledge_Record new_value = ++old_value;
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
madara::expression::Evaluation_Visitor::visit (
  const madara::expression::Composite_Not_Node &)
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
madara::expression::Evaluation_Visitor::visit (
  const madara::expression::Composite_Add_Node &)
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
madara::expression::Evaluation_Visitor::visit (
  const madara::expression::Composite_Assignment_Node &node)
{
  if (stack_.size () >= 2)
  {
    // for an assignment to be valid, we need a variable to the left
    // and an expression tree to the right. 
    try
    {
      // this is really backwards logic, but it was the only way I could think of
      // to allow for a = b = c with this type of tree and post-order flow
      madara::Knowledge_Record right = stack_.pop ();
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
madara::expression::Evaluation_Visitor::visit (
  const madara::expression::Composite_And_Node &)
{
  if (stack_.size () >= 2)
  {
    madara::Knowledge_Record right = stack_.pop ();
    madara::Knowledge_Record left = stack_.pop ();

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
madara::expression::Evaluation_Visitor::visit (
  const madara::expression::Composite_Or_Node &)
{
  if (stack_.size () >= 2)
  {
    madara::Knowledge_Record right = stack_.pop ();
    madara::Knowledge_Record left = stack_.pop ();

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
madara::expression::Evaluation_Visitor::visit (
  const madara::expression::Composite_Both_Node &)
{
  if (stack_.size () >= 2)
  {
    madara::Knowledge_Record right_v = stack_.pop ();
    madara::Knowledge_Record left_v = stack_.pop ();

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
madara::expression::Evaluation_Visitor::visit (
  const madara::expression::Composite_Sequential_Node &)
{
  if (stack_.size () >= 2)
  {
    madara::Knowledge_Record right_v = stack_.pop ();
    madara::Knowledge_Record left_v = stack_.pop ();

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
madara::expression::Evaluation_Visitor::visit (
  const madara::expression::Composite_Function_Node &)
{
}

/// evaluation of function (Composite_Function_Node)
void 
madara::expression::Evaluation_Visitor::visit (
  const madara::expression::Composite_For_Loop &)
{
}

/// evaluation of an equality comparison (Composite_Equality_Node)
void 
madara::expression::Evaluation_Visitor::visit (
  const madara::expression::Composite_Equality_Node &)
{
  if (stack_.size () >= 2)
  {
    madara::Knowledge_Record right = stack_.pop ();
    madara::Knowledge_Record left = stack_.pop ();

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
madara::expression::Evaluation_Visitor::visit (
  const madara::expression::Composite_Inequality_Node &)
{
  if (stack_.size () >= 2)
  {
    madara::Knowledge_Record right = stack_.pop ();
    madara::Knowledge_Record left = stack_.pop ();

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
madara::expression::Evaluation_Visitor::visit (
  const madara::expression::Composite_Greater_Than_Equal_Node &)
{
  if (stack_.size () >= 2)
  {
    madara::Knowledge_Record right = stack_.pop ();
    madara::Knowledge_Record left = stack_.pop ();

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
madara::expression::Evaluation_Visitor::visit (
  const madara::expression::Composite_Greater_Than_Node &)
{
  if (stack_.size () >= 2)
  {
    madara::Knowledge_Record right = stack_.pop ();
    madara::Knowledge_Record left = stack_.pop ();

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
madara::expression::Evaluation_Visitor::visit (
  const madara::expression::Composite_Less_Than_Equal_Node &)
{
  if (stack_.size () >= 2)
  {
    madara::Knowledge_Record right = stack_.pop ();
    madara::Knowledge_Record left = stack_.pop ();

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
madara::expression::Evaluation_Visitor::visit (
  const madara::expression::Composite_Less_Than_Node &)
{
  if (stack_.size () >= 2)
  {
    madara::Knowledge_Record right = stack_.pop ();
    madara::Knowledge_Record left = stack_.pop ();

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
madara::expression::Evaluation_Visitor::visit (
  const madara::expression::Composite_Subtract_Node &)
{
  if (stack_.size () >= 2)
  {
    madara::Knowledge_Record rhs = stack_.pop ();
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
madara::expression::Evaluation_Visitor::visit (
  const madara::expression::Composite_Divide_Node &)
{
  if (stack_.size () >= 2 && stack_.top ())
  {
    madara::Knowledge_Record rhs = stack_.pop ();
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
madara::expression::Evaluation_Visitor::visit (
  const madara::expression::Composite_Multiply_Node &)
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
madara::expression::Evaluation_Visitor::visit (
  const madara::expression::Composite_Modulus_Node &)
{
  if (stack_.size () >= 2 && stack_.top ())
  {
    madara::Knowledge_Record rhs = stack_.pop ();
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
madara::expression::Evaluation_Visitor::visit (
  const madara::expression::Composite_Implies_Node &)
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
madara::expression::Evaluation_Visitor::total (void)
{
  if (!stack_.is_empty ())
    return stack_.top ();
  else
    return 0;
}

/// reset the evaluation
void 
madara::expression::Evaluation_Visitor::reset (void)
{
  stack_.erase ();
}

#endif // _USE_VISITORS_

#endif /* _EVALUATION_VISITOR_CPP_ */
