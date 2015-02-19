/* -*- C++ -*- */
#ifndef _COMPOSITE_NOT_NODE_H_
#define _COMPOSITE_NOT_NODE_H_

#ifndef _MADARA_NO_KARL_

#include "madara/expression_tree/Composite_Unary_Node.h"
#include "madara/knowledge_engine/Knowledge_Record.h"

namespace Madara
{
  namespace Expression_Tree
  {
    class Component_Node;
    class Visitor;

    /**
     * @class Composite_Not_Node
     * @brief A composite node that logically nots a right expression.
     *        For instance, !0 == 1. !8 == 0.
     */
    class Composite_Not_Node : public Composite_Unary_Node
    {
    public:
      /**
       * Constructor
       * @param   right  right expression
       **/
      Composite_Not_Node (Component_Node *right);

      /**
       * Destructor
       **/
      virtual ~Composite_Not_Node (void);

      /**
       * Returns the printable character of the node
       * @return    value of the node
       **/
      virtual Madara::Knowledge_Record item (void) const;

      /** 
       * Prunes the expression tree of unnecessary nodes. 
       * @param     can_change   set to true if variable nodes are contained
       * @return    0 if right expression is non-zero and 1 otherwise.
       **/
      virtual Madara::Knowledge_Record prune (bool & can_change);

      /** 
       * Evaluates the node. 
       * @param     settings     settings for evaluating the node
       * @return    0 if right expression is non-zero and 1 otherwise.
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

#endif /* _COMPOSITE_NEGATE_NODE_H_ */
