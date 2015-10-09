#ifndef _MADARA_COMPOSITE_IMPLIES_NODE_H_
#define _MADARA_COMPOSITE_IMPLIES_NODE_H_

#ifndef _MADARA_NO_KARL_


#include "madara/expression/CompositeBinaryNode.h"
#include "madara/knowledge/KnowledgeRecord.h"

namespace madara
{
  namespace expression
  {
    // Forward declaration.
    class ComponentNode;
    class Visitor;

    /**
     * @class CompositeImpliesNode
     * @brief A composite node that performs an implication (inference rule)
     */
    class CompositeImpliesNode : public CompositeBinaryNode
    {
    public:
      /**
       * Constructor
       * @param   logger the logger to use for printing
       * @param   left   left expression
       * @param   right  right expression
       **/
      CompositeImpliesNode (logger::Logger & logger,
        ComponentNode *left,
        ComponentNode *right);

      /**
       * Returns the printable character of the node
       * @return    value of the node
       **/
      virtual madara::KnowledgeRecord item (void) const;

      /** 
       * Prunes the expression tree of unnecessary nodes. 
       * @param     can_change   set to true if variable nodes are contained
       * @return    the value of the condition (left expression)
       **/
      virtual madara::KnowledgeRecord prune (bool & can_change);

      /** 
       * Evaluates the expression tree. If the left expression is
       * true, then the right expression is evaluated. The right
       * expression's value does not percolate back up.
       * @param     settings     settings for evaluating the node
       * @return    the value of the condition (left expression)
       **/
      virtual madara::KnowledgeRecord evaluate (
        const madara::knowledge::KnowledgeUpdateSettings & settings);

      /** 
       * Accepts a visitor subclassed from the Visitor class
       * @param    visitor   visitor instance to use
       **/
      virtual void accept (Visitor &visitor) const;
    };
  }
}

#endif // _MADARA_NO_KARL_


#endif // _MADARA_COMPOSITE_IMPLIES_NODE_H_
