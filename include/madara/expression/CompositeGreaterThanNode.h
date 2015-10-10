#ifndef _MADARA_COMPOSITE_GREATER_THAN_NODE_H_
#define _MADARA_COMPOSITE_GREATER_THAN_NODE_H_

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
     * @class CompositeGreaterThanNode
     * @brief A composite node that compares left and right children
     *        for greater than
     */
    class CompositeGreaterThanNode : public CompositeBinaryNode
    {
    public:
      /**
       * Constructor
       * @param   logger the logger to use for printing
       * @param   left   left expression
       * @param   right  right expression
       **/
      CompositeGreaterThanNode (logger::Logger & logger,
        ComponentNode *left,
        ComponentNode *right);

      /**
       * Returns the printable character of the node
       * @return    value of the node
       **/
      virtual madara::knowledge::KnowledgeRecord item (void) const;

      /** 
       * Prunes the expression tree of unnecessary nodes. 
       * @param     can_change   set to true if variable nodes are contained
       * @return    0 if left expression is less than or equal to right 
       *            expression and 1 otherwise
       **/
      virtual madara::knowledge::KnowledgeRecord prune (bool & can_change);

      /** 
       * Evaluates the expression tree. 
       * @param     settings     settings for evaluating the node
       * @return    0 if left expression is less than or equal to right 
       *            expression and 1 otherwise
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


#endif // _MADARA_COMPOSITE_GREATER_THAN_NODE_H_
