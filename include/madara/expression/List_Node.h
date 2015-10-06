/* -*- C++ -*- */
#ifndef _MADARA_LIST_NODE_H_
#define _MADARA_LIST_NODE_H_

#ifndef _MADARA_NO_KARL_

#include <list>
#include <string>
#include <vector>

#include "madara/expression/Component_Node.h"
#include "madara/knowledge/Thread_Safe_Context.h"
#include "madara/knowledge/Knowledge_Record.h"

namespace madara
{
  namespace expression
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
      List_Node (madara::knowledge::Thread_Safe_Context & context);

      /// Dtor.
      virtual ~List_Node (void);

      /// Return the item stored in the node.
      virtual madara::Knowledge_Record item (void) const;

      /// Prune the tree of unnecessary nodes. 
      /// Returns evaluation of the node and sets can_change appropriately.
      /// if this node can be changed, that means it shouldn't be pruned.
      virtual madara::Knowledge_Record prune (bool & can_change);

      /// Evaluates the node and its children. This does not prune any of
      /// the expression tree, and is much faster than the prune function
      virtual madara::Knowledge_Record evaluate (
        const madara::knowledge::Knowledge_Update_Settings & settings);
      
      /// Define the @a accept() operation used for the Visitor pattern.
      virtual void accept (Visitor &visitor) const; 

    private:
      // variables context
      madara::knowledge::Thread_Safe_Context & context_;
      
      /// list of trees in a parameter list
      ::std::list<Component_Node *> list_;
    };
  }
}

#endif // _MADARA_NO_KARL_

#endif /* _MADARA_LIST_NODE_H_ */
