/* -*- C++ -*- */
#ifndef _PRINT_VISITOR_CPP_
#define _PRINT_VISITOR_CPP_

#ifdef  _USE_VISITORS_

#include <iostream>

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
#include "madara/expression/Print_Visitor.h"

/// visit function - prints Leaf_Node contents to std::cout

void 
madara::expression::Print_Visitor::visit (const Leaf_Node &node)
{
  MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_INFO,
        "%" PRId64 " ", node.item ()));
  //std::cout << node.item () << " ";
}

/// visit function - prints Variable_Node value to std::cout
void 
madara::expression::Print_Visitor::visit (const Variable_Node &node)
{
  MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_INFO,
    "%s ", node.key ().c_str ()));
}

/// visit function - prints Variable_Decrement_Node value to std::cout
void 
madara::expression::Print_Visitor::visit (const Variable_Decrement_Node &node)
{
  MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_INFO,
    "%s -= ", node.key ().c_str ()));
}

/// visit function - prints Variable_Divide_Node value to std::cout
void 
madara::expression::Print_Visitor::visit (const Variable_Divide_Node &node)
{
  MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_INFO,
    "%s /= ", node.key ().c_str ()));
}

/// visit function - prints Variable_Increment_Node value to std::cout
void 
madara::expression::Print_Visitor::visit (const Variable_Increment_Node &node)
{
  MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_INFO,
    "%s += ", node.key ().c_str ()));
}

/// visit function - prints Variable_Multiply_Node value to std::cout
void 
madara::expression::Print_Visitor::visit (const Variable_Multiply_Node &node)
{
  MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_INFO,
    "%s *= ", node.key ().c_str ()));
}

/// visit function - prints Variable_Compare_Node value to std::cout
void 
madara::expression::Print_Visitor::visit (const Variable_Compare_Node &node)
{
  MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_INFO,
    "compare(%s) ", node.key ().c_str ()));
}

/// visit function - prints List_Node value to std::cout
void 
madara::expression::Print_Visitor::visit (const List_Node &node)
{
  MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_INFO,
    "list "));
}

/// visit function - prints Composite_Negate_Node contents to std::cout
void 
madara::expression::Print_Visitor::visit (const Composite_Negate_Node &)
{
  MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_INFO,
    "-"));
}

/// visit function - prints Composite_Negate_Node contents to std::cout

void 
madara::expression::Print_Visitor::visit (const Composite_Predecrement_Node &)
{
  MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_INFO,
    "--"));
}

/// visit function - prints Composite_Negate_Node contents to std::cout

void 
madara::expression::Print_Visitor::visit (const Composite_Preincrement_Node &)
{
  MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_INFO,
    "++"));
}

/// visit function - prints Composite_Not_Node contents to std::cout

void 
madara::expression::Print_Visitor::visit (const Composite_Not_Node &)
{
  MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_INFO,
    "!"));
}

/// visit function - prints Composite_Add_Node contents to std::cout

void 
madara::expression::Print_Visitor::visit (const Composite_Add_Node &)
{
  MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_INFO,
    "+ "));
}

/// visit function - prints Composite_And_Node contents to std::cout

void 
madara::expression::Print_Visitor::visit (const Composite_And_Node &)
{
  MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_INFO,
    "&& "));
}

/// visit function - prints Composite_Or_Node contents to std::cout

void 
madara::expression::Print_Visitor::visit (const Composite_Or_Node &)
{
  MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_INFO,
    "|| "));
}

/// visit function - prints Composite_Equality_Node contents to std::cout
void 
madara::expression::Print_Visitor::visit (const Composite_Equality_Node &)
{
  MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_INFO,
    "== "));
}

/// visit function - prints Composite_Inequality_Node contents to std::cout
void 
madara::expression::Print_Visitor::visit (const Composite_Inequality_Node &)
{
  MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_INFO,
    "!= "));
}

/// visit function - prints Composite_Greater_Than_Equal_Node contents to std::cout
void 
madara::expression::Print_Visitor::visit (const Composite_Greater_Than_Equal_Node &)
{
  MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_INFO,
    ">= "));
}

/// visit function - prints Composite_Greater_Than_Node contents to std::cout
void 
madara::expression::Print_Visitor::visit (const Composite_Greater_Than_Node &)
{
  MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_INFO,
    "> "));
}

/// visit function - prints Composite_Less_Than_Equal_Node contents to std::cout
void 
madara::expression::Print_Visitor::visit (const Composite_Less_Than_Equal_Node &)
{
  MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_INFO,
    "<= "));
}

/// visit function - prints Composite_Less_Than_Node contents to std::cout
void 
madara::expression::Print_Visitor::visit (const Composite_Less_Than_Node &)
{
  MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_INFO,
    "< "));
}

/// visit function - prints Composite_Assignment_Node contents to std::cout

void 
madara::expression::Print_Visitor::visit (const Composite_Assignment_Node &)
{
  MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_INFO,
    "= "));
}

/// visit function - prints Composite_Subtract_Node contents to std::cout

void 
madara::expression::Print_Visitor::visit (const Composite_Subtract_Node &)
{
  MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_INFO,
    "- "));
}

/// visit function - prints Composite_Divide_Node contents to std::cout

void 
madara::expression::Print_Visitor::visit (const Composite_Divide_Node &)
{
  MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_INFO,
    "/ "));
}

/// visit function - prints Composite_Multiply_Node contents to std::cout

void 
madara::expression::Print_Visitor::visit (const Composite_Multiply_Node &)
{
  MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_INFO,
    "* "));
}

/// visit function - prints Composite_Modulus_Node contents to std::cout

void 
madara::expression::Print_Visitor::visit (const Composite_Modulus_Node &)
{
  MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_INFO,
    "% "));
}

/// visit function - prints Composite_Both_Node contents to std::cout

void 
madara::expression::Print_Visitor::visit (const Composite_Both_Node &)
{
  MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_INFO,
    "; "));
}

/// visit function - prints Composite_Sequential_Node contents to std::cout

void 
madara::expression::Print_Visitor::visit (const Composite_Sequential_Node &)
{
  MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_INFO,
    ", "));
}

/// visit function - prints Composite_Function_Node contents to std::cout

void 
madara::expression::Print_Visitor::visit (const Composite_Function_Node &)
{
  MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_INFO,
    "function call "));
}

/// visit function - prints Composite_For_Loop contents to std::cout

void 
madara::expression::Print_Visitor::visit (const Composite_For_Loop &)
{
  MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_INFO,
    "for loop "));
}

/// visit function - prints Composite_Implies_Node contents to std::cout

void 
madara::expression::Print_Visitor::visit (const Composite_Implies_Node &)
{
  MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_INFO,
    "=> "));
}

#endif // _USE_VISITORS_

#endif /* _PRINT_VISITOR_CPP_ */
