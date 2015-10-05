#ifndef COMPOSITE_GREATER_THAN_EQUAL_NODE_H
#define COMPOSITE_GREATER_THAN_EQUAL_NODE_H

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
     * @class Composite_Greater_Than_Equal_Node
     * @brief A composite node that compares left and right expressions
     *        for greater than or equal to
     */
    class Composite_Greater_Than_Equal_Node : public Composite_Binary_Node
    {
    public:
      /**
       * Constructor
       * @param   logger the logger to use for printing
       * @param   left   left expression
       * @param   right  right expression
       **/
      Composite_Greater_Than_Equal_Node (logger::Logger & logger,
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
       * @return    0 if left expression is less than right expression
       *            and 1 otherwise
       **/
      virtual madara::Knowledge_Record prune (bool & can_change);

      /** 
       * Evaluates the expression tree. 
       * @param     settings     settings for evaluating the node
       * @return    0 if left expression is less than right expression
       *            and 1 otherwise
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

#endif // COMPOSITE_GREATER_THAN_EQUAL_NODE_H