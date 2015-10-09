#ifndef _MADARA_COMPOSITE_BOTH_NODE_H_
#define _MADARA_COMPOSITE_BOTH_NODE_H_

#ifndef _MADARA_NO_KARL_


#include "madara/expression/CompositeTernaryNode.h"
#include "madara/knowledge/KnowledgeRecord.h"

namespace madara
{
  namespace expression
  {
    // Forward declaration.
    class ComponentNode;
    class Visitor;

    /**
     * @class CompositeBothNode
     * @brief A composite node that evaluates both left and right
     *        expressions regardless of their evaluations
     */
    class CompositeBothNode : public CompositeTernaryNode
    {
    public:
      /**
       * Constructor
       * @param   logger the logger to use for printing
       * @param   nodes   listing of nodes to evaluate
       **/
      CompositeBothNode (logger::Logger & logger,
        const ComponentNodes & nodes);

      /**
       * Returns the printable character of the node
       * @return    value of the node
       **/
      virtual madara::KnowledgeRecord item (void) const;

      /** 
       * Prunes the expression tree of unnecessary nodes. 
       * @param     can_change   set to true if variable nodes are contained
       * @return    the maximum of the value returned by the
       *            left and right expressions
       **/
      virtual madara::KnowledgeRecord prune (bool & can_change);

      /** 
       * Evaluates the composite node. 
       * @param     settings   settings for evaluating variables
       * @return    the maximum of the value returned by the
       *            left and right expressions
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

#endif // _MADARA_COMPOSITE_BOTH_NODE_H_
