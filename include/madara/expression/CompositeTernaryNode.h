/* -*- C++ -*- */
#ifndef _MADARA_COMPOSITE_TERNARY_NODE_H_
#define _MADARA_COMPOSITE_TERNARY_NODE_H_

#ifndef _MADARA_NO_KARL_

#include <string>
#include <stdexcept>
#include "madara/knowledge/KnowledgeRecord.h"
#include "madara/utility/stdint.h"
#include "madara/expression/ComponentNode.h"


namespace madara
{
  namespace expression
  {
    // Forward declaration.
    class Visitor;

    /**
     * @class TernaryNode
     * @brief An abstract base class defines a simple abstract
     *        implementation of an expression tree node.
     */
    class CompositeTernaryNode : public ComponentNode
    {
    public:
      /**
       * Constructor
       * @param  logger   the logger to use for printing
       **/
      CompositeTernaryNode (logger::Logger & logger);

      /**
       * Constructor
       * @param  logger   the logger to use for printing
       * @param  nodes    a list of nodes necessary for the node
       **/
      CompositeTernaryNode (logger::Logger & logger,
        const ComponentNodes & nodes);
      
      /**
       * Destructor
       **/
      virtual ~CompositeTernaryNode (void);

      /**
       * Returns the value of the node
       * @return    value of the node
       **/
      virtual madara::knowledge::KnowledgeRecord item (void) const;

      /** 
       * Prunes the expression tree of unnecessary nodes. 
       * @param     can_change   set to true if variable nodes are contained
       * @return    value of current contained expression tree
       **/
      virtual madara::knowledge::KnowledgeRecord prune (bool & can_change) = 0;

      /** 
       * Evaluates the node.
       * @param     settings     settings for evaluating the node 
       * @return    value of current contained expression tree
       **/
      virtual madara::knowledge::KnowledgeRecord evaluate (
        const madara::knowledge::KnowledgeUpdateSettings & settings) = 0;

      /** 
       * Accepts a visitor subclassed from the Visitor class
       * @param    visitor   visitor instance to use
       **/
      virtual void accept (Visitor &visitor) const;

    protected:
      ComponentNodes nodes_;
    };
  }
}

#endif // _MADARA_NO_KARL_

#endif /* _MADARA_COMPOSITE_TERNARY_NODE_H_ */
