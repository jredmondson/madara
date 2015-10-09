/* -*- C++ -*- */
#ifndef _MADARA_PRINT_VISITOR_H_
#define _MADARA_PRINT_VISITOR_H_

#ifdef _USE_VISITORS_

#include "madara/expression/Visitor.h"

namespace madara
{
  namespace expression
  {
    /**
     * @class PrintVisitor
     * @brief This class serves as a visitor for printing the contents of
     *        nodes to @a std::cout.
     */

    class PrintVisitor : public Visitor
    {
    public:
      /** 
       * Prints a constant
       * @param     node  the node to evaluate
       **/
      virtual void visit (const LeafNode &node);

      /** 
       * Prints a variable
       * @param     node  the node to evaluate
       **/
      virtual void visit (const VariableNode &node);
      
      /** 
       * Prints a variable decrementer node
       * @param     node  the node to evaluate
       **/
      virtual void visit (const VariableDecrementNode &node);
      
      /** 
       * Prints a variable divider node
       * @param     node  the node to evaluate
       **/
      virtual void visit (const VariableDivideNode &node);
      
      /** 
       * Prints a variable incrementer node
       * @param     node  the node to evaluate
       **/
      virtual void visit (const VariableIncrementNode &node);
      
      /** 
       * Prints a variable multiplier node
       * @param     node  the node to evaluate
       **/
      virtual void visit (const VariableMultiplyNode &node);
      
      /** 
       * Prints a variable incrementer node
       * @param     node  the node to evaluate
       **/
      virtual void visit (const VariableCompareNode &node);
      
      /** 
       * Prints a list
       * @param     node  the node to evaluate
       **/
      virtual void visit (const ListNode &node);

      /** 
       * Prints an integer negation
       * @param     node  the node to evaluate
       **/
      virtual void visit (const CompositeNegateNode &node);

      /** 
       * Prints a predecrement
       * @param     node  the node to evaluate
       **/
      virtual void visit (const CompositePredecrementNode &node);
      
      /** 
       * Prints a preincrement
       * @param     node  the node to evaluate
       **/
      virtual void visit (const CompositePreincrementNode &node);
      
      /** 
       * Prints a logical not 
       * @param     node  the node to evaluate
       **/
      virtual void visit (const CompositeNotNode &node);

      /** 
       * Prints an addition
       * @param     node  the node to evaluate
       **/
      virtual void visit (const CompositeAddNode &node);

      /** 
       * Prints an assignment to a variable
       * @param     node  the node to evaluate
       **/
      virtual void visit (const CompositeAssignmentNode &node);


      /** 
       * Prints a logical and
       * @param     node  the node to evaluate
       **/
      virtual void visit (const CompositeAndNode &node);

      /** 
       * Prints a logical or
       * @param     node  the node to evaluate
       **/
      virtual void visit (const CompositeOrNode &node);

      /** 
       * Prints an equality operation
       * @param     node  the node to evaluate
       **/
      virtual void visit (const CompositeEqualityNode &node);

      /** 
       * Prints an inequality operation
       * @param     node  the node to evaluate
       **/
      virtual void visit (const CompositeInequalityNode &node);

      /** 
       * Prints a greater-than-or-equal-to operation
       * @param     node  the node to evaluate
       **/
      virtual void visit (const CompositeGreaterThanEqualNode &node);

      /** 
       * Prints a less-than operation
       * @param     node  the node to evaluate
       **/
      virtual void visit (const CompositeGreaterThanNode &node);

      /** 
       * Prints a less-than-or-equal-to operation
       * @param     node  the node to evaluate
       **/
      virtual void visit (const CompositeLessThanEqualNode &node);

      /** 
       * Prints a less-than operation
       * @param     node  the node to evaluate
       **/
      virtual void visit (const CompositeLessThanNode &node);

      /** 
       * Prints a subtraction
       * @param     node  the node to evaluate
       **/
      virtual void visit (const CompositeSubtractNode &node);

      /** 
       * Prints a division
       * @param     node  the node to evaluate
       **/
      virtual void visit (const CompositeDivideNode &node);

      /** 
       * Prints a multiplication
       * @param     node  the node to evaluate
       **/
      virtual void visit (const CompositeMultiplyNode &node);

      /** 
       * Prints a remainder operation
       * @param     node  the node to evaluate
       **/
      virtual void visit (const CompositeModulusNode &node);

      /** 
       * Prints a left and right expression
       * @param     node  the node to evaluate
       **/
      virtual void visit (const CompositeBothNode &node);
      
      /** 
       * Prints a left and right expression
       * @param     node  the node to evaluate
       **/
      virtual void visit (const CompositeSequentialNode &node);
      
      /** 
       * Prints a function
       * @param     node  the node to evaluate
       **/
      virtual void visit (const CompositeFunctionNode &node);
      
      /** 
       * Prints a for loop
       * @param     node  the node to evaluate
       **/
      virtual void visit (const CompositeForLoop &node);

      /** 
       * Prints an inference rule
       * @param     node  the node to evaluate
       **/
      virtual void visit (const CompositeImpliesNode &node);

    };  
  }
}

#endif // _USE_VISITORS_

#endif /* _MADARA_PRINT_VISITOR_H_ */
