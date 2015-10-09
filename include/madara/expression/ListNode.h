/* -*- C++ -*- */
#ifndef _MADARA_LIST_NODE_H_
#define _MADARA_LIST_NODE_H_

#ifndef _MADARA_NO_KARL_

#include <list>
#include <string>
#include <vector>

#include "madara/expression/ComponentNode.h"
#include "madara/knowledge/ThreadSafeContext.h"
#include "madara/knowledge/KnowledgeRecord.h"

namespace madara
{
  namespace expression
  {
    // Forward declarations.
    class Visitor;

    /**
     * @class ListNode
     * @brief Defines a terminal node that contains a list.
     */

    class ListNode : public ComponentNode
    {
    public:
      /// Ctor.
      ListNode (madara::knowledge::ThreadSafeContext & context);

      /// Dtor.
      virtual ~ListNode (void);

      /// Return the item stored in the node.
      virtual madara::KnowledgeRecord item (void) const;

      /// Prune the tree of unnecessary nodes. 
      /// Returns evaluation of the node and sets can_change appropriately.
      /// if this node can be changed, that means it shouldn't be pruned.
      virtual madara::KnowledgeRecord prune (bool & can_change);

      /// Evaluates the node and its children. This does not prune any of
      /// the expression tree, and is much faster than the prune function
      virtual madara::KnowledgeRecord evaluate (
        const madara::knowledge::KnowledgeUpdateSettings & settings);
      
      /// Define the @a accept() operation used for the Visitor pattern.
      virtual void accept (Visitor &visitor) const; 

    private:
      // variables context
      madara::knowledge::ThreadSafeContext & context_;
      
      /// list of trees in a parameter list
      ::std::list<ComponentNode *> list_;
    };
  }
}

#endif // _MADARA_NO_KARL_

#endif /* _MADARA_LIST_NODE_H_ */
