/* -*- C++ -*- */
#ifndef _PRINT_VISITOR_CPP_
#define _PRINT_VISITOR_CPP_

#ifdef  _USE_VISITORS_

#include <iostream>

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
#include "madara/expression/PrintVisitor.h"

/// visit function - prints LeafNode contents to std::cout

void 
madara::expression::PrintVisitor::visit (const LeafNode &node)
{
  MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_INFO,
        "%" PRId64 " ", node.item ()));
  //std::cout << node.item () << " ";
}

/// visit function - prints VariableNode value to std::cout
void 
madara::expression::PrintVisitor::visit (const VariableNode &node)
{
  MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_INFO,
    "%s ", node.key ().c_str ()));
}

/// visit function - prints VariableDecrementNode value to std::cout
void 
madara::expression::PrintVisitor::visit (const VariableDecrementNode &node)
{
  MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_INFO,
    "%s -= ", node.key ().c_str ()));
}

/// visit function - prints VariableDivideNode value to std::cout
void 
madara::expression::PrintVisitor::visit (const VariableDivideNode &node)
{
  MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_INFO,
    "%s /= ", node.key ().c_str ()));
}

/// visit function - prints VariableIncrementNode value to std::cout
void 
madara::expression::PrintVisitor::visit (const VariableIncrementNode &node)
{
  MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_INFO,
    "%s += ", node.key ().c_str ()));
}

/// visit function - prints VariableMultiplyNode value to std::cout
void 
madara::expression::PrintVisitor::visit (const VariableMultiplyNode &node)
{
  MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_INFO,
    "%s *= ", node.key ().c_str ()));
}

/// visit function - prints VariableCompareNode value to std::cout
void 
madara::expression::PrintVisitor::visit (const VariableCompareNode &node)
{
  MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_INFO,
    "compare(%s) ", node.key ().c_str ()));
}

/// visit function - prints ListNode value to std::cout
void 
madara::expression::PrintVisitor::visit (const ListNode &node)
{
  MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_INFO,
    "list "));
}

/// visit function - prints CompositeNegateNode contents to std::cout
void 
madara::expression::PrintVisitor::visit (const CompositeNegateNode &)
{
  MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_INFO,
    "-"));
}

/// visit function - prints CompositeNegateNode contents to std::cout

void 
madara::expression::PrintVisitor::visit (const CompositePredecrementNode &)
{
  MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_INFO,
    "--"));
}

/// visit function - prints CompositeNegateNode contents to std::cout

void 
madara::expression::PrintVisitor::visit (const CompositePreincrementNode &)
{
  MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_INFO,
    "++"));
}

/// visit function - prints CompositeNotNode contents to std::cout

void 
madara::expression::PrintVisitor::visit (const CompositeNotNode &)
{
  MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_INFO,
    "!"));
}

/// visit function - prints CompositeAddNode contents to std::cout

void 
madara::expression::PrintVisitor::visit (const CompositeAddNode &)
{
  MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_INFO,
    "+ "));
}

/// visit function - prints CompositeAndNode contents to std::cout

void 
madara::expression::PrintVisitor::visit (const CompositeAndNode &)
{
  MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_INFO,
    "&& "));
}

/// visit function - prints CompositeOrNode contents to std::cout

void 
madara::expression::PrintVisitor::visit (const CompositeOrNode &)
{
  MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_INFO,
    "|| "));
}

/// visit function - prints CompositeEqualityNode contents to std::cout
void 
madara::expression::PrintVisitor::visit (const CompositeEqualityNode &)
{
  MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_INFO,
    "== "));
}

/// visit function - prints CompositeInequalityNode contents to std::cout
void 
madara::expression::PrintVisitor::visit (const CompositeInequalityNode &)
{
  MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_INFO,
    "!= "));
}

/// visit function - prints CompositeGreaterThanEqualNode contents to std::cout
void 
madara::expression::PrintVisitor::visit (const CompositeGreaterThanEqualNode &)
{
  MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_INFO,
    ">= "));
}

/// visit function - prints CompositeGreaterThanNode contents to std::cout
void 
madara::expression::PrintVisitor::visit (const CompositeGreaterThanNode &)
{
  MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_INFO,
    "> "));
}

/// visit function - prints CompositeLessThanEqualNode contents to std::cout
void 
madara::expression::PrintVisitor::visit (const CompositeLessThanEqualNode &)
{
  MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_INFO,
    "<= "));
}

/// visit function - prints CompositeLessThanNode contents to std::cout
void 
madara::expression::PrintVisitor::visit (const CompositeLessThanNode &)
{
  MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_INFO,
    "< "));
}

/// visit function - prints CompositeAssignmentNode contents to std::cout

void 
madara::expression::PrintVisitor::visit (const CompositeAssignmentNode &)
{
  MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_INFO,
    "= "));
}

/// visit function - prints CompositeSubtractNode contents to std::cout

void 
madara::expression::PrintVisitor::visit (const CompositeSubtractNode &)
{
  MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_INFO,
    "- "));
}

/// visit function - prints CompositeDivideNode contents to std::cout

void 
madara::expression::PrintVisitor::visit (const CompositeDivideNode &)
{
  MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_INFO,
    "/ "));
}

/// visit function - prints CompositeMultiplyNode contents to std::cout

void 
madara::expression::PrintVisitor::visit (const CompositeMultiplyNode &)
{
  MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_INFO,
    "* "));
}

/// visit function - prints CompositeModulusNode contents to std::cout

void 
madara::expression::PrintVisitor::visit (const CompositeModulusNode &)
{
  MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_INFO,
    "% "));
}

/// visit function - prints CompositeBothNode contents to std::cout

void 
madara::expression::PrintVisitor::visit (const CompositeBothNode &)
{
  MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_INFO,
    "; "));
}

/// visit function - prints CompositeSequentialNode contents to std::cout

void 
madara::expression::PrintVisitor::visit (const CompositeSequentialNode &)
{
  MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_INFO,
    ", "));
}

/// visit function - prints CompositeFunctionNode contents to std::cout

void 
madara::expression::PrintVisitor::visit (const CompositeFunctionNode &)
{
  MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_INFO,
    "function call "));
}

/// visit function - prints CompositeForLoop contents to std::cout

void 
madara::expression::PrintVisitor::visit (const CompositeForLoop &)
{
  MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_INFO,
    "for loop "));
}

/// visit function - prints CompositeImpliesNode contents to std::cout

void 
madara::expression::PrintVisitor::visit (const CompositeImpliesNode &)
{
  MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_INFO,
    "=> "));
}

#endif // _USE_VISITORS_

#endif /* _PRINT_VISITOR_CPP_ */
