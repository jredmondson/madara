/* -*- C++ -*- */
#ifndef _COMPOSITE_FOR_LOOP_H_
#define _COMPOSITE_FOR_LOOP_H_

#include "madara/expression_tree/Component_Node.h"
#include "madara/knowledge_engine/Thread_Safe_Context.h"
#include "madara/knowledge_engine/Functions.h"
#include "madara/knowledge_engine/Knowledge_Record.h"

namespace Madara
{
  namespace Expression_Tree
  {
    class Component_Node;
    class Visitor;

    /**
     * @class Composite_For_Loop
     * @brief A composite node that iterates until a condition is met
     */
    class Composite_For_Loop : public Component_Node
    {
    public:
      /**
       * Constructor
       * @param   precondition  executed before loop
       * @param   condition     what must be true for the loop to continue
       * @param   postcondition executed after a successful loop body
       * @param   body          executed if loop condition is true
       * @param   context       context for variable lookups
       **/
      Composite_For_Loop (Component_Node * precondition,
        Component_Node * condition, 
        Component_Node * postcondition,
        Component_Node * body,
        Madara::Knowledge_Engine::Thread_Safe_Context & context);

      /**
       * Destructor
       **/
      virtual ~Composite_For_Loop (void);

      /**
       * Returns the printable character of the node
       * @return    value of the node
       **/
      virtual Madara::Knowledge_Record item (void) const;

      /** 
       * Prunes the expression tree of unnecessary nodes. 
       * @param     can_change   set to true if variable nodes are contained
       * @return    negation of the right expression
       **/
      virtual Madara::Knowledge_Record prune (bool & can_change);

      /** 
       * Evaluates the expression tree. 
       * @param     settings     settings for evaluating the node
       * @return    negation of the right expression
       **/
      virtual Madara::Knowledge_Record evaluate (
        const Madara::Knowledge_Engine::Knowledge_Update_Settings & settings);

      /** 
       * Accepts a visitor subclassed from the Visitor class
       * @param    visitor   visitor instance to use
       **/
      virtual void accept (Visitor &visitor) const;

    private:
      
      // variables context
      Madara::Knowledge_Engine::Thread_Safe_Context & context_;

      // the loop precondition (what happens before the loop)
      Component_Node * precondition_;
      
      // the loop condition (what must be true to continue looping)
      Component_Node * condition_;
      
      // the postcondition (what happens after a loop)
      Component_Node * postcondition_;
      
      // the body (what happens after a condition is true--the loop contents)
      Component_Node * body_;

      // function pointer
      Madara::Knowledge_Engine::Function * function_;
    };
  }
}

#endif /* _COMPOSITE_FOR_LOOP_H_ */
