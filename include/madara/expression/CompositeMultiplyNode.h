/* -*- C++ -*- */
#ifndef _MADARA_COMPOSITE_MULTIPLY_NODE_H_
#define _MADARA_COMPOSITE_MULTIPLY_NODE_H_

#ifndef _MADARA_NO_KARL_

#include "madara/expression/CompositeTernaryNode.h"
#include "madara/knowledge/KnowledgeRecord.h"

namespace madara
{
  namespace expression
  {
    class ComponentNode;
    class CompositeBinaryNode;
    class Visitor;

    /**
     * @class CompositeMultiplyNode
     * @brief A composite node that multiplies a left expression by a right
     *        expression
     */
    class CompositeMultiplyNode : public CompositeTernaryNode
    {
    public:
      /**
       * Constructor
       * @param   logger the logger to use for printing
       * @param   nodes  2+ nodes to be executed in sequence
       **/
      CompositeMultiplyNode (logger::Logger & logger,
        const ComponentNodes & nodes);

      /**
       * Destructor
       **/
      virtual ~CompositeMultiplyNode (void);

      /**
       * Returns the printable character of the node
       * @return    value of the node
       **/
      virtual madara::KnowledgeRecord item (void) const;

      /** 
       * Prunes the expression tree of unnecessary nodes. 
       * @param     can_change   set to true if variable nodes are contained
       * @return    value of left expression multiplied by right expression
       **/
      virtual madara::KnowledgeRecord prune (bool & can_change);

      /** 
       * Evaluates the expression tree. 
       * @param     settings     settings for evaluating the node
       * @return    value of left expression multiplied by right expression
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

#endif /* _MADARA_COMPOSITE_MULTIPLY_NODE_H_ */
