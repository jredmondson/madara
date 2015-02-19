/* -*- C++ -*- */
#ifndef _VARIABLE_NODE_H_
#define _VARIABLE_NODE_H_

#ifndef _MADARA_NO_KARL_

#include <string>
#include <vector>

#include "madara/expression_tree/Component_Node.h"
#include "madara/knowledge_engine/Thread_Safe_Context.h"
#include "madara/knowledge_engine/Knowledge_Record.h"
#include "madara/knowledge_engine/Knowledge_Update_Settings.h"

namespace Madara
{
  namespace Expression_Tree
  {
    // Forward declarations.
    class Visitor;

    /**
     * @class Variable_Node
     * @brief Defines a terminal node of that references the current
     *        value stored in a variable.
     */

    class Variable_Node : public Component_Node
    {
    public:
      /// Ctor.
      Variable_Node (const std::string &key, 
        Madara::Knowledge_Engine::Thread_Safe_Context & context);

      /// Dtor.
      virtual ~Variable_Node (void);

      /// Return the item stored in the node.
      virtual Madara::Knowledge_Record item (void) const;

      /// Sets the value stored in the node.
      int set (const Madara::Knowledge_Record::Integer & value,
        const Madara::Knowledge_Engine::Knowledge_Update_Settings & settings =
        Knowledge_Engine::Knowledge_Update_Settings ());
      
      /// Sets the value stored in the node.
      int set (double value,
        const Madara::Knowledge_Engine::Knowledge_Update_Settings & settings =
        Knowledge_Engine::Knowledge_Update_Settings ());
      
      /// Sets the value stored in the node.
      int set (const std::string & value,
        const Madara::Knowledge_Engine::Knowledge_Update_Settings & settings =
        Knowledge_Engine::Knowledge_Update_Settings ());
      
      /// Sets the value stored in the node.
      int set (const Madara::Knowledge_Record & value,
        const Madara::Knowledge_Engine::Knowledge_Update_Settings & settings =
        Knowledge_Engine::Knowledge_Update_Settings ());
      
      /// Atomically increment the variable.
      Madara::Knowledge_Record inc (
        const Madara::Knowledge_Engine::Knowledge_Update_Settings & settings =
        Knowledge_Engine::Knowledge_Update_Settings ());

      /// Atomically decrement the variable.
      Madara::Knowledge_Record dec (
        const Madara::Knowledge_Engine::Knowledge_Update_Settings & settings =
        Knowledge_Engine::Knowledge_Update_Settings ());

      /// Prune the tree of unnecessary nodes. 
      /// Returns evaluation of the node and sets can_change appropriately.
      /// if this node can be changed, that means it shouldn't be pruned.
      virtual Madara::Knowledge_Record prune (bool & can_change);

      /// Evaluates the node and its children. This does not prune any of
      /// the expression tree, and is much faster than the prune function
      virtual Madara::Knowledge_Record evaluate (
        const Madara::Knowledge_Engine::Knowledge_Update_Settings & settings);

      /// Expands the key (if necessary). This allow for keys to be defined
      /// with other variables inserted (e.g. var{.id} with .id = 2 expands
      /// to var2)
      std::string expand_key (void) const;

      /// Return the variable key.
      const std::string & key (void) const;

      /// Define the @a accept() operation used for the Visitor pattern.
      virtual void accept (Visitor &visitor) const; 

      /**
       * Retrieves the underlying Knowledge_Record in the context (useful for
       * system calls).
       * @return      the Knowledge_Record inside of the context
       **/
      inline
      Madara::Knowledge_Record * get_record (void)
      {
        if (record_)
          return record_;
        else
          return context_.get_record (expand_key ());
      }

    private:
      /// Key for retrieving value of this variable.
      const std::string key_;
      Madara::Knowledge_Record * record_;

      Madara::Knowledge_Engine::Thread_Safe_Context & context_;

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

#endif /* _VARIABLE_NODE_H_ */
