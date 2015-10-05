#ifndef COMPOSITE_ASSIGNMENT_NODE_H
#define COMPOSITE_ASSIGNMENT_NODE_H

#ifndef _MADARA_NO_KARL_


#include "madara/expression_tree/Composite_Binary_Node.h"
#include "madara/knowledge/Knowledge_Record.h"
#include "madara/expression_tree/Variable_Node.h"
#include "madara/expression_tree/Composite_Array_Reference.h"

namespace madara
{
  namespace expression_tree
  {
    // Forward declaration.
    class Component_Node;
    class Visitor;

    /**
     * @class Composite_Assignment_Node
     * @brief A composite node that allows for variable assignment.
     *        Left child should be a variable and right child should
     *        be an expression to evaluate
     */
    class Composite_Assignment_Node : public Composite_Unary_Node
    {
    public:
      /**
       * Constructor
       * @param   logger the logger to use for printing
       * @param   left   variable to assign the right expression to
       * @param   right  right expression to assign to the variable
       **/
      Composite_Assignment_Node (
        logger::Logger & logger,
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
       * @return    value of evaluated right expression
       **/
      virtual madara::Knowledge_Record prune (bool & can_change);

      /** 
       * Evaluates the expression tree. 
       * @param     settings     settings for evaluating the node
       * @return    value of evaluated right expression
       **/
      virtual madara::Knowledge_Record evaluate (
        const madara::knowledge::Knowledge_Update_Settings & settings);

      /** 
       * Accepts a visitor subclassed from the Visitor class
       * @param    visitor   visitor instance to use
       **/
      virtual void accept (Visitor &visitor) const;

    private:

      /**
       * Left should always be a variable node. Using Variable_Node
       * instead of having to dynamic cast a Symbol * should be much
       * fater.
       **/
      Variable_Node * var_;
      Composite_Array_Reference * array_;
    };
  }
}

#endif // _MADARA_NO_KARL_

#endif //COMPOSITE_ASSIGNMENT_NODE_H
