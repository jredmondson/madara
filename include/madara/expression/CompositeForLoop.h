/* -*- C++ -*- */
#ifndef _MADARA_COMPOSITE_FOR_LOOP_H_
#define _MADARA_COMPOSITE_FOR_LOOP_H_

#ifndef _MADARA_NO_KARL_

#include "madara/expression/ComponentNode.h"
#include "madara/knowledge/ThreadSafeContext.h"
#include "madara/knowledge/Functions.h"
#include "madara/knowledge/KnowledgeRecord.h"

namespace madara
{
  namespace expression
  {
    class ComponentNode;
    class Visitor;

    /**
     * @class CompositeForLoop
     * @brief A composite node that iterates until a condition is met
     */
    class CompositeForLoop : public ComponentNode
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
      CompositeForLoop (
        ComponentNode * precondition,
        ComponentNode * condition, 
        ComponentNode * postcondition,
        ComponentNode * body,
        madara::knowledge::ThreadSafeContext & context);

      /**
       * Destructor
       **/
      virtual ~CompositeForLoop (void);

      /**
       * Returns the printable character of the node
       * @return    value of the node
       **/
      virtual madara::knowledge::KnowledgeRecord item (void) const;

      /** 
       * Prunes the expression tree of unnecessary nodes. 
       * @param     can_change   set to true if variable nodes are contained
       * @return    negation of the right expression
       **/
      virtual madara::knowledge::KnowledgeRecord prune (bool & can_change);

      /** 
       * Evaluates the expression tree. 
       * @param     settings     settings for evaluating the node
       * @return    negation of the right expression
       **/
      virtual madara::knowledge::KnowledgeRecord evaluate (
        const madara::knowledge::KnowledgeUpdateSettings & settings);

      /** 
       * Accepts a visitor subclassed from the Visitor class
       * @param    visitor   visitor instance to use
       **/
      virtual void accept (Visitor &visitor) const;

    private:
      
      // variables context
      madara::knowledge::ThreadSafeContext & context_;

      // the loop precondition (what happens before the loop)
      ComponentNode * precondition_;
      
      // the loop condition (what must be true to continue looping)
      ComponentNode * condition_;
      
      // the postcondition (what happens after a loop)
      ComponentNode * postcondition_;
      
      // the body (what happens after a condition is true--the loop contents)
      ComponentNode * body_;

      // function pointer
      madara::knowledge::Function * function_;
    };
  }
}

#endif // _MADARA_NO_KARL_

#endif /* _MADARA_COMPOSITE_FOR_LOOP_H_ */
