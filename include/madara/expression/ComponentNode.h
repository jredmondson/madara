/* -*- C++ -*- */
#ifndef _MADARA_COMPONENT_NODE_H_
#define _MADARA_COMPONENT_NODE_H_

#ifndef _MADARA_NO_KARL_

#include <string>
#include <deque>
#include <stdexcept>
#include "madara/knowledge/KnowledgeRecord.h"
#include "madara/knowledge/KnowledgeUpdateSettings.h"
#include "madara/utility/stdint.h"
#include "madara/logger/Logger.h"

namespace madara
{
  namespace knowledge
  {
    class ThreadSafeContext;
  }

  namespace expression
  {
    // Forward declaration.
    class Visitor;

    /**
     * @class ComponentNode
     * @brief An abstract base class defines a simple abstract
     *        implementation of an expression tree node.
     *        
     * @see   See CompositeUnaryNode, CompositeBinaryNode, and Leaf Node
     *        for immediate subclasses of this class.
     */
    class ComponentNode
    {
    public:
      /**
       * Constructor
       * @param  logger   the logger to use for printing
       **/
      ComponentNode (logger::Logger & logger);

      /**
       * Destructor
       **/
      virtual ~ComponentNode (void) = 0;

      /**
       * Returns the value of the node
       * @return    value of the node
       **/
      virtual madara::knowledge::KnowledgeRecord item (void) const;

      /** 
       * Prunes the expression tree of unnecessary nodes. 
       * @return    value of current contained expression tree
       **/
      virtual madara::knowledge::KnowledgeRecord prune (bool & can_change) = 0;

      /** 
       * Evaluates the expression tree. 
       * @param     settings     settings for evaluating the node
       * @return    value of current contained expression tree
       **/
      virtual madara::knowledge::KnowledgeRecord evaluate (
        const madara::knowledge::KnowledgeUpdateSettings & settings)
          = 0;

      /** 
       * Returns the left expression. 
       * @return    a pointer to the left expression
       **/
      virtual ComponentNode *left (void) const;

      /** 
       * Returns the right expression. 
       * @return    a pointer to the right expression
       **/
      virtual ComponentNode *right (void) const;

      /** 
       * Accepts a visitor subclassed from the Visitor class
       * @param    visitor   visitor instance to use
       **/
      virtual void accept (Visitor &visitor) const;

      /**
       * Sets the logger for printing errors and debugging info
       * @param  logger the logger to use
       **/
      void set_logger (logger::Logger & logger);

    protected:
      /// handle the context
      logger::Logger * logger_;
    };

    /// a vector of Component Nodes
    typedef   std::deque <ComponentNode *>   ComponentNodes;
  }
}

#endif // _MADARA_NO_KARL_

#endif /* _MADARA_COMPONENT_NODE_H_ */
