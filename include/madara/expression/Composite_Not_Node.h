/* -*- C++ -*- */
#ifndef _MADARA_COMPOSITE_NOT_NODE_H_
#define _MADARA_COMPOSITE_NOT_NODE_H_

#ifndef _MADARA_NO_KARL_

#include "madara/expression/Composite_Unary_Node.h"
#include "madara/knowledge/Knowledge_Record.h"

namespace madara
{
  namespace expression
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
       * @param   logger the logger to use for printing
       * @param   right  right expression
       **/
      Composite_Not_Node (logger::Logger & logger,
        Component_Node *right);

      /**
       * Destructor
       **/
      virtual ~Composite_Not_Node (void);

      /**
       * Returns the printable character of the node
       * @return    value of the node
       **/
      virtual madara::Knowledge_Record item (void) const;

      /** 
       * Prunes the expression tree of unnecessary nodes. 
       * @param     can_change   set to true if variable nodes are contained
       * @return    0 if right expression is non-zero and 1 otherwise.
       **/
      virtual madara::Knowledge_Record prune (bool & can_change);

      /** 
       * Evaluates the node. 
       * @param     settings     settings for evaluating the node
       * @return    0 if right expression is non-zero and 1 otherwise.
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

#endif /* _MADARA_COMPOSITE_NOT_NODE_H_ */
