#ifndef _COMPOSITE_IMPLIES_NODE_H
#define _COMPOSITE_IMPLIES_NODE_H

#ifndef _MADARA_NO_KARL_


#include "madara/expression_tree/Composite_Binary_Node.h"
#include "madara/knowledge/Knowledge_Record.h"

namespace madara
{
  namespace expression_tree
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
       * @param   logger the logger to use for printing
       * @param   left   left expression
       * @param   right  right expression
       **/
      Composite_Implies_Node (logger::Logger & logger,
        Component_Node *left,
        Component_Node *right);

      /**
       * Returns the printable character of the node
       * @return    value of the node
       **/
      virtual madara::Knowledge_Record item (void) const;

      /** 
       * Prunes the expression tree of unnecessary nodes. 
       * @param     can_change   set to true if variable nodes are contained
       * @return    the value of the condition (left expression)
       **/
      virtual madara::Knowledge_Record prune (bool & can_change);

      /** 
       * Evaluates the expression tree. If the left expression is
       * true, then the right expression is evaluated. The right
       * expression's value does not percolate back up.
       * @param     settings     settings for evaluating the node
       * @return    the value of the condition (left expression)
       **/
      virtual madara::Knowledge_Record evaluate (
        const madara::knowledge::Knowledge_Update_Settings & settings);

      /** 
       * Accepts a visitor subclassed from the Visitor class
       * @param    visitor   visitor instance to use
       **/
      virtual void accept (Visitor &visitor) const;
    };
  }
}

#endif // _MADARA_NO_KARL_


#endif // _COMPOSITE_IMPLIES_NODE_H
