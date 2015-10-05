/* -*- C++ -*- */
#ifndef _COMPOSITE_SUBTRACT_NODE_H_
#define _COMPOSITE_SUBTRACT_NODE_H_

#ifndef _MADARA_NO_KARL_

#include "madara/knowledge/Knowledge_Record.h"

namespace madara
{
  namespace expression_tree
  {
    class Component_Node;
    class Visitor;

    /**
     * @class Composite_Subtract_Node
     * @brief A composite node that encompasses subtraction of a right
     *        expression from a left expression
     */
    class Composite_Subtract_Node : public Composite_Binary_Node
    {
    public:
      /**
       * Constructor
       * @param   logger the logger to use for printing
       * @param   left   left expression
       * @param   right  right expression
       **/
      Composite_Subtract_Node (logger::Logger & logger,
        Component_Node *left, Component_Node *right);

      /**
       * Destructor
       **/
      virtual ~Composite_Subtract_Node (void);

      /**
       * Returns the printable character of the node
       * @return    value of the node
       **/
      virtual madara::Knowledge_Record item (void) const;

      /** 
       * Prunes the expression tree of unnecessary nodes. 
       * @param     can_change   set to true if variable nodes are contained
       * @return    value of right expression subtracted from left expression
       **/
      virtual madara::Knowledge_Record prune (bool & can_change);

      /** 
       * Evaluates the node. 
       * @param     settings     settings for evaluating the node
       * @return    value of right expression subtracted from left expression
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

#endif /* _COMPOSITE_SUBTRACT_NODE_H_ */
