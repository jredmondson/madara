/* -*- C++ -*- */
#ifndef _TERNARY_NODE_H_
#define _TERNARY_NODE_H_

#ifndef _MADARA_NO_KARL_

#include <string>
#include <stdexcept>
#include "madara/knowledge_engine/Knowledge_Record.h"
#include "madara/utility/stdint.h"
#include "madara/expression_tree/Component_Node.h"


namespace Madara
{
  namespace Expression_Tree
  {
    // Forward declaration.
    class Visitor;

    /**
     * @class Ternary_Node
     * @brief An abstract base class defines a simple abstract
     *        implementation of an expression tree node.
     */
    class Composite_Ternary_Node : public Component_Node
    {
    public:
      /**
       * Constructor
       **/
      Composite_Ternary_Node ();

      /**
       * Constructor
       **/
      Composite_Ternary_Node (const Component_Nodes & nodes);
      
      /**
       * Destructor
       **/
      virtual ~Composite_Ternary_Node (void);

      /**
       * Returns the value of the node
       * @return    value of the node
       **/
      virtual Madara::Knowledge_Record item (void) const;

      /** 
       * Prunes the expression tree of unnecessary nodes. 
       * @param     can_change   set to true if variable nodes are contained
       * @return    value of current contained expression tree
       **/
      virtual Madara::Knowledge_Record prune (bool & can_change) = 0;

      /** 
       * Evaluates the node.
       * @param     settings     settings for evaluating the node 
       * @return    value of current contained expression tree
       **/
      virtual Madara::Knowledge_Record evaluate (
        const Madara::Knowledge_Engine::Knowledge_Update_Settings & settings) = 0;

      /** 
       * Accepts a visitor subclassed from the Visitor class
       * @param    visitor   visitor instance to use
       **/
      virtual void accept (Visitor &visitor) const;

    protected:
      Component_Nodes nodes_;
    };
  }
}

#endif // _MADARA_NO_KARL_

#endif /* _TERNARY_NODE_H_ */
