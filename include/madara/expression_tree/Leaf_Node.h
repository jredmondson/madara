/* -*- C++ -*- */
#ifndef _LEAF_NODE_H_
#define _LEAF_NODE_H_

#include <string>
#include "madara/expression_tree/Component_Node.h"

namespace Madara
{
  namespace Expression_Tree
  {
    // Forward declarations.
    class Visitor;

    /**
     * @class Leaf_Node
     * @brief Defines a node that contains a Madara::Knowledge_Record::Integer value
     */

    class Leaf_Node : public Component_Node
    {
    public:
      /**
       * Constructor
       * @param   item    value of the node
       **/
      Leaf_Node (const Knowledge_Record & item);
      
      /**
       * Constructor
       * @param   item    value of the node
       **/
      Leaf_Node (Madara::Knowledge_Record::Integer item);
      
      /**
       * Constructor
       * @param   item    value of the node
       **/
      Leaf_Node (double item);

      /**
       * Constructor
       * @param   item    value of the node
       **/
      Leaf_Node (const std::string &item);

      /**
       * Destructor
       **/
      virtual ~Leaf_Node (void);

      /**
       * Returns the printable value of the node
       * @return    value of the node
       **/
      virtual Madara::Knowledge_Record item (void) const;

      /** 
       * Prunes the expression tree of unnecessary nodes. 
       * @param     can_change   set to true if variable nodes are contained
       * @return    value of this node
       **/
      virtual Madara::Knowledge_Record prune (bool & can_change);

      /** 
       * Evaluates the expression tree. 
       * @return    value of this node
       **/
      virtual Madara::Knowledge_Record evaluate (
        const Madara::Knowledge_Engine::Knowledge_Update_Settings & settings);

      /** 
       * Accepts a visitor subclassed from the Visitor class
       * @param    visitor   visitor instance to use
       **/
      virtual void accept (Visitor &visitor) const; 

    private:
      /// Integer value associated with the operand.
      Madara::Knowledge_Record item_;
    };

  }
}

#endif /* _LEAF_NODE_H_ */
