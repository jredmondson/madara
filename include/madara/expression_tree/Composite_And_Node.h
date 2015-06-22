#ifndef COMPOSITE_AND_NODE_H
#define COMPOSITE_AND_NODE_H

#ifndef _MADARA_NO_KARL_


#include "madara/expression_tree/Composite_Ternary_Node.h"
#include "madara/knowledge_engine/Knowledge_Record.h"

namespace Madara
{
  namespace Expression_Tree
  {
    // Forward declaration.
    class Component_Node;
    class Visitor;

    /**
     * @class Composite_And_Node
     * @brief A composite node that performs a logical and
     */
    class Composite_And_Node : public Composite_Ternary_Node
    {
    public:
      /**
       * Constructor
       * @param   logger   the logger to use for printing
       * @param   nodes  2+ nodes to be executed in sequence
       **/
      Composite_And_Node (Logger::Logger & logger,
        const Component_Nodes & nodes);

      /**
       * Returns the printable character of the node
       * @return    value of the node
       **/
      virtual Madara::Knowledge_Record item (void) const;

      /** 
       * Prunes the expression tree of unnecessary nodes. 
       * @param     can_change   set to true if variable nodes are contained
       * @return    zero unless both left and right expressions 
       *            evaluate to non-zero. Right is not evaluated
       *            if left expression returns zero.
       **/
      virtual Madara::Knowledge_Record prune (bool & can_change);

      /** 
       * Evaluates the expression tree. 
       * @param     settings     settings for evaluating the node
       * @return    zero unless both left and right expressions 
       *            evaluate to non-zero. Right is not evaluated
       *            if left expression returns zero.
       **/
      virtual Madara::Knowledge_Record evaluate (
        const Madara::Knowledge_Engine::Knowledge_Update_Settings & settings);

      /** 
       * Accepts a visitor subclassed from the Visitor class
       * @param    visitor   visitor instance to use
       **/
      virtual void accept (Visitor &visitor) const;
    };
  }
}

#endif // _MADARA_NO_KARL_

#endif // COMPOSITE_AND_NODE_H
