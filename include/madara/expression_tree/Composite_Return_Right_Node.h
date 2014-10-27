#ifndef _COMPOSITE_RETURN_RIGHT_NODE_H_
#define _COMPOSITE_RETURN_RIGHT_NODE_H_


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
     * @class Composite_Return_Right_Node
     * @brief A composite node that evaluates both left and right
     *        expressions regardless of their evaluations
     */
    class Composite_Return_Right_Node : public Composite_Ternary_Node
    {
    public:
      /**
       * Constructor
       * @param   nodes  2+ nodes to be executed in sequence
       **/
      Composite_Return_Right_Node (const Component_Nodes & nodes);

      /**
       * Returns the printable character of the node
       * @return    value of the node
       **/
      virtual Madara::Knowledge_Record item (void) const;

      /** 
       * Prunes the expression tree of unnecessary nodes. 
       * @param     can_change   set to true if variable nodes are contained
       * @return    the maximum of the value returned by the
       *            left and right expressions
       **/
      virtual Madara::Knowledge_Record prune (bool & can_change);

      /** 
       * Evaluates the node. 
       * @param     settings     settings for evaluating the node
       * @return    the maximum of the value returned by the
       *            left and right expressions
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


#endif // _COMPOSITE_RETURN_RIGHT_NODE_H_
