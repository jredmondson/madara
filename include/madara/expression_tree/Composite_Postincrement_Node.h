/* -*- C++ -*- */
#ifndef _COMPOSITE_POSTINCREMENT_NODE_H_
#define _COMPOSITE_POSTINCREMENT_NODE_H_

#include "madara/expression_tree/Composite_Unary_Node.h"
#include "madara/knowledge_engine/Knowledge_Record.h"
#include "madara/expression_tree/Composite_Array_Reference.h"
#include "madara/expression_tree/Variable_Node.h"

namespace Madara
{
  namespace Expression_Tree
  {
    class Component_Node;
    class Visitor;

    /**
     * @class Composite_Postincrement_Node
     * @brief A composite node that increments a right expression. If
     *        right expression is a variable, then the variable will
     *        be updated with the new value.
     */
    class Composite_Postincrement_Node : public Composite_Unary_Node
    {
    public:
      /**
       * Constructor
       * @param   right  right expression
       **/
      Composite_Postincrement_Node (Component_Node *right);

      /**
       * Destructor
       **/
      virtual ~Composite_Postincrement_Node (void);

      /**
       * Returns the printable character of the node
       * @return    value of the node
       **/
      virtual Madara::Knowledge_Record item (void) const;

      /** 
       * Prunes the expression tree of unnecessary nodes. 
       * @param     can_change   set to true if variable nodes are contained
       * @return    one added to the value returned by the right expression
       **/
      virtual Madara::Knowledge_Record prune (bool & can_change);

      /** 
       * Evaluates the node. 
       * @param     settings     settings for evaluating the node
       * @return    one added to the value returned by the right expression
       **/
      virtual Madara::Knowledge_Record evaluate (
        const Madara::Knowledge_Engine::Knowledge_Update_Settings & settings);

      /** 
       * Accepts a visitor subclassed from the Visitor class
       * @param    visitor   visitor instance to use
       **/
      virtual void accept (Visitor &visitor) const;

    private:

      /// variable holder
      Variable_Node * var_;

      /// variable index holder
      Composite_Array_Reference * array_;
    };
  }
}

#endif /* _COMPOSITE_POSTINCREMENT_NODE_H_ */
