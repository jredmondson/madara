/* -*- C++ -*- */
#ifndef _MADARA_COMPOSITE_SUBTRACT_NODE_H_
#define _MADARA_COMPOSITE_SUBTRACT_NODE_H_

#ifndef _MADARA_NO_KARL_

#include "madara/knowledge/KnowledgeRecord.h"

namespace madara
{
  namespace expression
  {
    class ComponentNode;
    class Visitor;

    /**
     * @class CompositeSubtractNode
     * @brief A composite node that encompasses subtraction of a right
     *        expression from a left expression
     */
    class CompositeSubtractNode : public CompositeBinaryNode
    {
    public:
      /**
       * Constructor
       * @param   logger the logger to use for printing
       * @param   left   left expression
       * @param   right  right expression
       **/
      CompositeSubtractNode (logger::Logger & logger,
        ComponentNode *left, ComponentNode *right);

      /**
       * Destructor
       **/
      virtual ~CompositeSubtractNode (void);

      /**
       * Returns the printable character of the node
       * @return    value of the node
       **/
      virtual madara::knowledge::KnowledgeRecord item (void) const;

      /** 
       * Prunes the expression tree of unnecessary nodes. 
       * @param     can_change   set to true if variable nodes are contained
       * @return    value of right expression subtracted from left expression
       **/
      virtual madara::knowledge::KnowledgeRecord prune (bool & can_change);

      /** 
       * Evaluates the node. 
       * @param     settings     settings for evaluating the node
       * @return    value of right expression subtracted from left expression
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

#endif /* _MADARA_COMPOSITE_SUBTRACT_NODE_H_ */
