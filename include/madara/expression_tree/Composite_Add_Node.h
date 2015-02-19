/* -*- C++ -*- */
#ifndef _COMPOSITE_ADD_NODE_H_
#define _COMPOSITE_ADD_NODE_H_

#ifndef _MADARA_NO_KARL_

#include "madara/expression_tree/Composite_Ternary_Node.h"

namespace Madara
{
  namespace Expression_Tree
  {
    // Forward declaration.
    class Component_Node;
    class Visitor;

    /**
     * @class Composite_Add_Node
     * @brief A composite node that encompasses addition of two expressions
     */
    class Composite_Add_Node : public Composite_Ternary_Node
    {
    public:
      /**
       * Constructor
       * @param   nodes  2+ nodes to be executed in sequence
       **/
      Composite_Add_Node (const Component_Nodes & nodes);

      /**
       * Returns the printable character of the node
       * @return    value of the node
       **/
      virtual Madara::Knowledge_Record item (void) const;

      /** 
       * Prunes the expression tree of unnecessary nodes. 
       * @param     can_change   set to true if variable nodes are contained
       * @return    value of left expression plus the value of the right one
       **/
      virtual Madara::Knowledge_Record prune (bool & can_change);

      /** 
       * Evaluates the expression tree. 
       * @param     settings     settings for evaluating the node
       * @return    value of left expression plus the value of the right one
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

#endif /* _COMPOSITE_ADD_NODE_H_ */
