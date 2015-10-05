/* -*- C++ -*- */
#ifndef _EVALUATION_VISITOR_H_
#define _EVALUATION_VISITOR_H_

#ifdef _USE_VISITORS_

#include "madara/expression_tree/Visitor.h"
#include "madara/utility/LStack.h"
#include "madara/knowledge/Knowledge_Record.h"

namespace madara
{
  namespace expression_tree
  {
    /**
     * @class Evaluation_Visitor
     * @brief This plays the role of a visitor for evaluating
     *        nodes in an expression tree that is being iterated in
     *        post-order fashion (and does not work correctly with any
     *        other iterator).
     */
    class Evaluation_Visitor : public Visitor
    {
    public:
      /** 
       * Evaluates a constant
       * @param     node  the node to evaluate
       **/
      virtual void visit (const Leaf_Node &node);

      /** 
       * Evaluates a variable
       * @param     node  the node to evaluate
       **/
      virtual void visit (const Variable_Node &node);
      
      /** 
       * Evaluates a variable decrementer node
       * @param     node  the node to evaluate
       **/
      virtual void visit (const Variable_Decrement_Node &node);
      
      /** 
       * Evaluates a variable divider node
       * @param     node  the node to evaluate
       **/
      virtual void visit (const Variable_Divide_Node &node);
      
      /** 
       * Evaluates a variable incrementer node
       * @param     node  the node to evaluate
       **/
      virtual void visit (const Variable_Increment_Node &node);
      
      /** 
       * Evaluates a variable multiplier node
       * @param     node  the node to evaluate
       **/
      virtual void visit (const Variable_Multiply_Node &node);
      
      /** 
       * Evaluates a variable comparison node
       * @param     node  the node to evaluate
       **/
      virtual void visit (const Variable_Compare_Node &node);
      
      /** 
       * Evaluates a list
       * @param     node  the node to evaluate
       **/
      virtual void visit (const List_Node &node);

      /** 
       * Evaluates an integer negation
       * @param     node  the node to evaluate
       **/
      virtual void visit (const Composite_Negate_Node &node);

      /** 
       * Evaluates a predecrement
       * @param     node  the node to evaluate
       **/
      virtual void visit (const Composite_Predecrement_Node &node);
      
      /** 
       * Evaluates a preincrement
       * @param     node  the node to evaluate
       **/
      virtual void visit (const Composite_Preincrement_Node &node);
      
      /** 
       * Evaluates a logical not 
       * @param     node  the node to evaluate
       **/
      virtual void visit (const Composite_Not_Node &node);

      /** 
       * Evaluates an addition
       * @param     node  the node to evaluate
       **/
      virtual void visit (const Composite_Add_Node &node);

      /** 
       * Evaluates an assignment to a variable
       * @param     node  the node to evaluate
       **/
      virtual void visit (const Composite_Assignment_Node &node);


      /** 
       * Evaluates a logical and
       * @param     node  the node to evaluate
       **/
      virtual void visit (const Composite_And_Node &node);

      /** 
       * Evaluates a logical or
       * @param     node  the node to evaluate
       **/
      virtual void visit (const Composite_Or_Node &node);

      /** 
       * Evaluates an equality operation
       * @param     node  the node to evaluate
       **/
      virtual void visit (const Composite_Equality_Node &node);

      /** 
       * Evaluates an inequality operation
       * @param     node  the node to evaluate
       **/
      virtual void visit (const Composite_Inequality_Node &node);

      /** 
       * Evaluates a greater-than-or-equal-to operation
       * @param     node  the node to evaluate
       **/
      virtual void visit (const Composite_Greater_Than_Equal_Node &node);

      /** 
       * Evaluates a less-than operation
       * @param     node  the node to evaluate
       **/
      virtual void visit (const Composite_Greater_Than_Node &node);

      /** 
       * Evaluates a less-than-or-equal-to operation
       * @param     node  the node to evaluate
       **/
      virtual void visit (const Composite_Less_Than_Equal_Node &node);

      /** 
       * Evaluates a less-than operation
       * @param     node  the node to evaluate
       **/
      virtual void visit (const Composite_Less_Than_Node &node);

      /** 
       * Evaluates a subtraction
       * @param     node  the node to evaluate
       **/
      virtual void visit (const Composite_Subtract_Node &node);

      /** 
       * Evaluates a division
       * @param     node  the node to evaluate
       **/
      virtual void visit (const Composite_Divide_Node &node);

      /** 
       * Evaluates a multiplication
       * @param     node  the node to evaluate
       **/
      virtual void visit (const Composite_Multiply_Node &node);

      /** 
       * Evaluates a remainder operation
       * @param     node  the node to evaluate
       **/
      virtual void visit (const Composite_Modulus_Node &node);

      /** 
       * Evaluates a left and right expression
       * @param     node  the node to evaluate
       **/
      virtual void visit (const Composite_Both_Node &node);
      
      /** 
       * Evaluates a left and right expression
       * @param     node  the node to evaluate
       **/
      virtual void visit (const Composite_Sequential_Node &node);
      
      /** 
       * Evaluates a function
       * @param     node  the node to evaluate
       **/
      virtual void visit (const Composite_Function_Node &node);
      
      /** 
       * Evaluates a for loop
       * @param     node  the node to evaluate
       **/
      virtual void visit (const Composite_For_Loop &node);

      /** 
       * Evaluates an inference rule
       * @param     node  the node to evaluate
       **/
      virtual void visit (const Composite_Implies_Node &node);

      /** 
       * Returns the result of the evaluation
       * @return    result of the evaluation
       **/
      madara::Knowledge_Record::Integer total (void);

      /** 
       * Resets the evaluation
       **/
      void reset (void);

    private:
      /// Stack used for temporarily storing evaluations.
      madara::utility::LStack<madara::Knowledge_Record> stack_;
    };  
  }
}

#endif // _USE_VISITORS_

#endif /* _VISITOR_H_ */
