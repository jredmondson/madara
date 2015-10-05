/* -*- C++ -*- */
#ifndef _COMPOSITE_FOR_LOOP_H_
#define _COMPOSITE_FOR_LOOP_H_

#ifndef _MADARA_NO_KARL_

#include "madara/expression_tree/Component_Node.h"
#include "madara/knowledge/Thread_Safe_Context.h"
#include "madara/knowledge/Functions.h"
#include "madara/knowledge/Knowledge_Record.h"

namespace madara
{
  namespace expression_tree
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
      Composite_For_Loop (
        Component_Node * precondition,
        Component_Node * condition, 
        Component_Node * postcondition,
        Component_Node * body,
        madara::knowledge::Thread_Safe_Context & context);

      /**
       * Destructor
       **/
      virtual ~Composite_For_Loop (void);

      /**
       * Returns the printable character of the node
       * @return    value of the node
       **/
      virtual madara::Knowledge_Record item (void) const;

      /** 
       * Prunes the expression tree of unnecessary nodes. 
       * @param     can_change   set to true if variable nodes are contained
       * @return    negation of the right expression
       **/
      virtual madara::Knowledge_Record prune (bool & can_change);

      /** 
       * Evaluates the expression tree. 
       * @param     settings     settings for evaluating the node
       * @return    negation of the right expression
       **/
      virtual madara::Knowledge_Record evaluate (
        const madara::knowledge::Knowledge_Update_Settings & settings);

      /** 
       * Accepts a visitor subclassed from the Visitor class
       * @param    visitor   visitor instance to use
       **/
      virtual void accept (Visitor &visitor) const;

    private:
      
      // variables context
      madara::knowledge::Thread_Safe_Context & context_;

      // the loop precondition (what happens before the loop)
      Component_Node * precondition_;
      
      // the loop condition (what must be true to continue looping)
      Component_Node * condition_;
      
      // the postcondition (what happens after a loop)
      Component_Node * postcondition_;
      
      // the body (what happens after a condition is true--the loop contents)
      Component_Node * body_;

      // function pointer
      madara::knowledge::Function * function_;
    };
  }
}

#endif // _MADARA_NO_KARL_

#endif /* _COMPOSITE_FOR_LOOP_H_ */
