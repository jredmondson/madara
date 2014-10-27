#ifndef _COMPOSITE_IMPLIES_NODE_H
#define _COMPOSITE_IMPLIES_NODE_H


#include "madara/expression_tree/Composite_Binary_Node.h"
#include "madara/knowledge_engine/Knowledge_Record.h"

namespace Madara
{
  namespace Expression_Tree
  {
    // Forward declaration.
    class Component_Node;
    class Visitor;

    /**
     * @class Composite_Implies_Node
     * @brief A composite node that performs an implication (inference rule)
     */
    class Composite_Implies_Node : public Composite_Binary_Node
    {
    public:
      /**
       * Constructor
       * @param   left   left expression
       * @param   right  right expression
       **/
      Composite_Implies_Node (Component_Node *left, 
                          Component_Node *right);

      /**
       * Returns the printable character of the node
       * @return    value of the node
       **/
      virtual Madara::Knowledge_Record item (void) const;

      /** 
       * Prunes the expression tree of unnecessary nodes. 
       * @param     can_change   set to true if variable nodes are contained
       * @return    the value of the condition (left expression)
       **/
      virtual Madara::Knowledge_Record prune (bool & can_change);

      /** 
       * Evaluates the expression tree. If the left expression is
       * true, then the right expression is evaluated. The right
       * expression's value does not percolate back up.
       * @param     settings     settings for evaluating the node
       * @return    the value of the condition (left expression)
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


#endif // _COMPOSITE_IMPLIES_NODE_H
