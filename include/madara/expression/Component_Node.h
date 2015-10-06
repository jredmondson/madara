/* -*- C++ -*- */
#ifndef _MADARA_COMPONENT_NODE_H_
#define _MADARA_COMPONENT_NODE_H_

#ifndef _MADARA_NO_KARL_

#include <string>
#include <deque>
#include <stdexcept>
#include "madara/knowledge/Knowledge_Record.h"
#include "madara/knowledge/Knowledge_Update_Settings.h"
#include "madara/utility/stdint.h"
#include "madara/logger/Logger.h"

namespace madara
{
  namespace knowledge
  {
    class Thread_Safe_Context;
  }

  namespace expression
  {
    // Forward declaration.
    class Visitor;

    /**
     * @class Component_Node
     * @brief An abstract base class defines a simple abstract
     *        implementation of an expression tree node.
     *        
     * @see   See Composite_Unary_Node, Composite_Binary_Node, and Leaf Node
     *        for immediate subclasses of this class.
     */
    class Component_Node
    {
    public:
      /**
       * Constructor
       * @param  logger   the logger to use for printing
       **/
      Component_Node (logger::Logger & logger);

      /**
       * Destructor
       **/
      virtual ~Component_Node (void) = 0;

      /**
       * Returns the value of the node
       * @return    value of the node
       **/
      virtual madara::Knowledge_Record item (void) const;

      /** 
       * Prunes the expression tree of unnecessary nodes. 
       * @return    value of current contained expression tree
       **/
      virtual madara::Knowledge_Record prune (bool & can_change) = 0;

      /** 
       * Evaluates the expression tree. 
       * @param     settings     settings for evaluating the node
       * @return    value of current contained expression tree
       **/
      virtual madara::Knowledge_Record evaluate (
        const madara::knowledge::Knowledge_Update_Settings & settings)
          = 0;

      /** 
       * Returns the left expression. 
       * @return    a pointer to the left expression
       **/
      virtual Component_Node *left (void) const;

      /** 
       * Returns the right expression. 
       * @return    a pointer to the right expression
       **/
      virtual Component_Node *right (void) const;

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
    typedef   std::deque <Component_Node *>   Component_Nodes;
  }
}

#endif // _MADARA_NO_KARL_

#endif /* _MADARA_COMPONENT_NODE_H_ */
