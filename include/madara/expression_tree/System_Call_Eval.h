/* -*- C++ -*- */
#ifndef _SYSTEM_CALL_EVAL_H_
#define _SYSTEM_CALL_EVAL_H_

/**
 * @file System_Call_Eval.h
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains the System_Call_Print class which evaluates a
 * Knowledge Record and returns the result from within a KaRL logic.
 **/

#include <string>
#include <stdexcept>
#include "madara/utility/stdint.h"
#include "madara/expression_tree/System_Call_Node.h"


namespace Madara
{
  namespace Expression_Tree
  {
    // Forward declaration.
    class Visitor;

    /**
     * @class System_Call_Eval
     * @brief Evaluates a Knowledge_Record and returns the evaluation result
     */
    class System_Call_Eval : public System_Call_Node
    {
    public:
      /**
       * Constructor
       **/
      System_Call_Eval (
        Madara::Knowledge_Engine::Thread_Safe_Context & context,
        const Component_Nodes & nodes);
      
      /**
       * Destructor
       **/
      virtual ~System_Call_Eval (void);

      /**
       * Returns the value of the node
       * @return    value of the node
       **/
      virtual Madara::Knowledge_Record item (void) const;

      /** 
       * Prunes the expression tree of unnecessary nodes. 
       * @param     can_change   set to true if variable nodes are contained
       * @return    value of current contained expression tree
       **/
      virtual Madara::Knowledge_Record prune (bool & can_change);

      /** 
       * Evaluates the expression tree. 
       * @return    value of current contained expression tree
       **/
      virtual Madara::Knowledge_Record evaluate (
        const Madara::Knowledge_Engine::Knowledge_Update_Settings & settings);

      /** 
       * Accepts a visitor subclassed from the Visitor class
       * @param    visitor   visitor instance to use
       **/
      virtual void accept (Visitor &visitor) const;
    };
  }
}

#endif /* _SYSTEM_CALL_EVAL_H_ */
