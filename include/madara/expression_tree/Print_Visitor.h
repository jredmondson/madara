/* -*- C++ -*- */
#ifndef _PRINT_VISITOR_H_
#define _PRINT_VISITOR_H_

#ifdef _USE_VISITORS_

#include "madara/expression_tree/Visitor.h"

namespace Madara
{
  namespace Expression_Tree
  {
    /**
     * @class Print_Visitor
     * @brief This class serves as a visitor for printing the contents of
     *        nodes to @a std::cout.
     */

    class Print_Visitor : public Visitor
    {
    public:
      /** 
       * Prints a constant
       * @param     node  the node to evaluate
       **/
      virtual void visit (const Leaf_Node &node);

      /** 
       * Prints a variable
       * @param     node  the node to evaluate
       **/
      virtual void visit (const Variable_Node &node);
      
      /** 
       * Prints a variable decrementer node
       * @param     node  the node to evaluate
       **/
      virtual void visit (const Variable_Decrement_Node &node);
      
      /** 
       * Prints a variable divider node
       * @param     node  the node to evaluate
       **/
      virtual void visit (const Variable_Divide_Node &node);
      
      /** 
       * Prints a variable incrementer node
       * @param     node  the node to evaluate
       **/
      virtual void visit (const Variable_Increment_Node &node);
      
      /** 
       * Prints a variable multiplier node
       * @param     node  the node to evaluate
       **/
      virtual void visit (const Variable_Multiply_Node &node);
      
      /** 
       * Prints a variable incrementer node
       * @param     node  the node to evaluate
       **/
      virtual void visit (const Variable_Compare_Node &node);
      
      /** 
       * Prints a list
       * @param     node  the node to evaluate
       **/
      virtual void visit (const List_Node &node);

      /** 
       * Prints an integer negation
       * @param     node  the node to evaluate
       **/
      virtual void visit (const Composite_Negate_Node &node);

      /** 
       * Prints a predecrement
       * @param     node  the node to evaluate
       **/
      virtual void visit (const Composite_Predecrement_Node &node);
      
      /** 
       * Prints a preincrement
       * @param     node  the node to evaluate
       **/
      virtual void visit (const Composite_Preincrement_Node &node);
      
      /** 
       * Prints a logical not 
       * @param     node  the node to evaluate
       **/
      virtual void visit (const Composite_Not_Node &node);

      /** 
       * Prints an addition
       * @param     node  the node to evaluate
       **/
      virtual void visit (const Composite_Add_Node &node);

      /** 
       * Prints an assignment to a variable
       * @param     node  the node to evaluate
       **/
      virtual void visit (const Composite_Assignment_Node &node);


      /** 
       * Prints a logical and
       * @param     node  the node to evaluate
       **/
      virtual void visit (const Composite_And_Node &node);

      /** 
       * Prints a logical or
       * @param     node  the node to evaluate
       **/
      virtual void visit (const Composite_Or_Node &node);

      /** 
       * Prints an equality operation
       * @param     node  the node to evaluate
       **/
      virtual void visit (const Composite_Equality_Node &node);

      /** 
       * Prints an inequality operation
       * @param     node  the node to evaluate
       **/
      virtual void visit (const Composite_Inequality_Node &node);

      /** 
       * Prints a greater-than-or-equal-to operation
       * @param     node  the node to evaluate
       **/
      virtual void visit (const Composite_Greater_Than_Equal_Node &node);

      /** 
       * Prints a less-than operation
       * @param     node  the node to evaluate
       **/
      virtual void visit (const Composite_Greater_Than_Node &node);

      /** 
       * Prints a less-than-or-equal-to operation
       * @param     node  the node to evaluate
       **/
      virtual void visit (const Composite_Less_Than_Equal_Node &node);

      /** 
       * Prints a less-than operation
       * @param     node  the node to evaluate
       **/
      virtual void visit (const Composite_Less_Than_Node &node);

      /** 
       * Prints a subtraction
       * @param     node  the node to evaluate
       **/
      virtual void visit (const Composite_Subtract_Node &node);

      /** 
       * Prints a division
       * @param     node  the node to evaluate
       **/
      virtual void visit (const Composite_Divide_Node &node);

      /** 
       * Prints a multiplication
       * @param     node  the node to evaluate
       **/
      virtual void visit (const Composite_Multiply_Node &node);

      /** 
       * Prints a remainder operation
       * @param     node  the node to evaluate
       **/
      virtual void visit (const Composite_Modulus_Node &node);

      /** 
       * Prints a left and right expression
       * @param     node  the node to evaluate
       **/
      virtual void visit (const Composite_Both_Node &node);
      
      /** 
       * Prints a left and right expression
       * @param     node  the node to evaluate
       **/
      virtual void visit (const Composite_Sequential_Node &node);
      
      /** 
       * Prints a function
       * @param     node  the node to evaluate
       **/
      virtual void visit (const Composite_Function_Node &node);
      
      /** 
       * Prints a for loop
       * @param     node  the node to evaluate
       **/
      virtual void visit (const Composite_For_Loop &node);

      /** 
       * Prints an inference rule
       * @param     node  the node to evaluate
       **/
      virtual void visit (const Composite_Implies_Node &node);

    };  
  }
}

#endif // _USE_VISITORS_

#endif /* _PRINT_VISITOR_H_ */
