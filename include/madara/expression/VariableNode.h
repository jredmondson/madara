/* -*- C++ -*- */
#ifndef _MADARA_VARIABLE_NODE_H_
#define _MADARA_VARIABLE_NODE_H_

#ifndef _MADARA_NO_KARL_

#include <string>
#include <vector>

#include "madara/expression/ComponentNode.h"
#include "madara/knowledge/ThreadSafeContext.h"
#include "madara/knowledge/KnowledgeRecord.h"
#include "madara/knowledge/KnowledgeUpdateSettings.h"

namespace madara
{
  namespace expression
  {
    // Forward declarations.
    class Visitor;

    /**
     * @class VariableNode
     * @brief Defines a terminal node of that references the current
     *        value stored in a variable.
     */

    class VariableNode : public ComponentNode
    {
    public:
      /// Ctor.
      VariableNode (const std::string &key, 
        madara::knowledge::ThreadSafeContext & context);

      /// Dtor.
      virtual ~VariableNode (void);

      /// Return the item stored in the node.
      virtual madara::knowledge::KnowledgeRecord item (void) const;

      /// Sets the value stored in the node.
      int set (const madara::knowledge::KnowledgeRecord::Integer & value,
        const madara::knowledge::KnowledgeUpdateSettings & settings =
        knowledge::KnowledgeUpdateSettings ());
      
      /// Sets the value stored in the node.
      int set (double value,
        const madara::knowledge::KnowledgeUpdateSettings & settings =
        knowledge::KnowledgeUpdateSettings ());
      
      /// Sets the value stored in the node.
      int set (const std::string & value,
        const madara::knowledge::KnowledgeUpdateSettings & settings =
        knowledge::KnowledgeUpdateSettings ());
      
      /// Sets the value stored in the node.
      int set (const madara::knowledge::KnowledgeRecord & value,
        const madara::knowledge::KnowledgeUpdateSettings & settings =
        knowledge::KnowledgeUpdateSettings ());
      
      /// Atomically increment the variable.
      madara::knowledge::KnowledgeRecord inc (
        const madara::knowledge::KnowledgeUpdateSettings & settings =
        knowledge::KnowledgeUpdateSettings ());

      /// Atomically decrement the variable.
      madara::knowledge::KnowledgeRecord dec (
        const madara::knowledge::KnowledgeUpdateSettings & settings =
        knowledge::KnowledgeUpdateSettings ());

      /// Prune the tree of unnecessary nodes. 
      /// Returns evaluation of the node and sets can_change appropriately.
      /// if this node can be changed, that means it shouldn't be pruned.
      virtual madara::knowledge::KnowledgeRecord prune (bool & can_change);

      /// Evaluates the node and its children. This does not prune any of
      /// the expression tree, and is much faster than the prune function
      virtual madara::knowledge::KnowledgeRecord evaluate (
        const madara::knowledge::KnowledgeUpdateSettings & settings);

      /// Expands the key (if necessary). This allow for keys to be defined
      /// with other variables inserted (e.g. var{.id} with .id = 2 expands
      /// to var2)
      std::string expand_key (void) const;

      /// Return the variable key.
      const std::string & key (void) const;

      /// Define the @a accept() operation used for the Visitor pattern.
      virtual void accept (Visitor &visitor) const; 

      /**
       * Retrieves the underlying knowledge::KnowledgeRecord in the context (useful for
       * system calls).
       * @return      the knowledge::KnowledgeRecord inside of the context
       **/
      inline
      madara::knowledge::KnowledgeRecord * get_record (void)
      {
        if (record_)
          return record_;
        else
          return context_.get_record (expand_key ());
      }

    private:
      /// Key for retrieving value of this variable.
      const std::string key_;
      madara::knowledge::KnowledgeRecord * record_;

      madara::knowledge::ThreadSafeContext & context_;

      /// Expansion necessary
      bool key_expansion_necessary_;

      std::vector< std::string> splitters_;
      std::vector< std::string> tokens_;
      std::vector< std::string> pivot_list_;

      /// Reference to context for variable retrieval
    };
  }
}

#endif // _MADARA_NO_KARL_

#endif /* _MADARA_VARIABLE_NODE_H_ */
