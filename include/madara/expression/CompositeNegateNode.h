/* -*- C++ -*- */
#ifndef _MADARA_COMPOSITE_NEGATE_NODE_H_
#define _MADARA_COMPOSITE_NEGATE_NODE_H_

#ifndef _MADARA_NO_KARL_

#include "madara/expression/CompositeUnaryNode.h"
#include "madara/knowledge/KnowledgeRecord.h"

namespace madara
{
  namespace expression
  {
    class ComponentNode;
    class Visitor;

    /**
     * @class CompositeNegateNode
     * @brief A composite node that integrally negates a right expression.
     *        For instance, the negation of 5 is -5.
     */
    class CompositeNegateNode : public CompositeUnaryNode
    {
    public:
      /**
       * Constructor
       * @param   logger the logger to use for printing
       * @param   right  right expression
       **/
      CompositeNegateNode (logger::Logger & logger,
        ComponentNode *right);

      /**
       * Destructor
       **/
      virtual ~CompositeNegateNode (void);

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
       * Evaluates the node. 
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
    };
  }
}

#endif // _MADARA_NO_KARL_

#endif /* _MADARA_COMPOSITE_NEGATE_NODE_H_ */
