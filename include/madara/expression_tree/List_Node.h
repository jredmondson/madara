/* -*- C++ -*- */
#ifndef _LIST_NODE_H_
#define _LIST_NODE_H_

#ifndef _MADARA_NO_KARL_

#include <list>
#include <string>
#include <vector>

#include "madara/expression_tree/Component_Node.h"
#include "madara/knowledge_engine/Thread_Safe_Context.h"
#include "madara/knowledge_engine/Knowledge_Record.h"

namespace Madara
{
  namespace Expression_Tree
  {
    // Forward declarations.
    class Visitor;

    /**
     * @class List_Node
     * @brief Defines a terminal node that contains a list.
     */

    class List_Node : public Component_Node
    {
    public:
      /// Ctor.
      List_Node (Madara::Knowledge_Engine::Thread_Safe_Context & context);

      /// Dtor.
      virtual ~List_Node (void);

      /// Return the item stored in the node.
      virtual Madara::Knowledge_Record item (void) const;

      /// Prune the tree of unnecessary nodes. 
      /// Returns evaluation of the node and sets can_change appropriately.
      /// if this node can be changed, that means it shouldn't be pruned.
      virtual Madara::Knowledge_Record prune (bool & can_change);

      /// Evaluates the node and its children. This does not prune any of
      /// the expression tree, and is much faster than the prune function
      virtual Madara::Knowledge_Record evaluate (
        const Madara::Knowledge_Engine::Knowledge_Update_Settings & settings);
      
      /// Define the @a accept() operation used for the Visitor pattern.
      virtual void accept (Visitor &visitor) const; 

    private:
      // variables context
      Madara::Knowledge_Engine::Thread_Safe_Context & context_;
      
      /// list of trees in a parameter list
      ::std::list<Component_Node *> list_;
    };
  }
}

#endif // _MADARA_NO_KARL_

#endif /* _LIST_NODE_H_ */
