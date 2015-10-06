/* -*- C++ -*- */
#ifndef _MADARA_COMPOSITE_ARRAY_REFERENCE_
#define _MADARA_COMPOSITE_ARRAY_REFERENCE_

#ifndef _MADARA_NO_KARL_

#include <string>
#include <vector>

#include "madara/expression/Composite_Unary_Node.h"
#include "madara/knowledge/Thread_Safe_Context.h"
#include "madara/knowledge/Knowledge_Record.h"
#include "madara/knowledge/Knowledge_Update_Settings.h"

namespace madara
{
  namespace expression
  {
    // Forward declarations.
    class Visitor;

    /**
     * @class Composite_Array_Reference
     * @brief Defines a terminal node of that references the current
     *        value stored in a variable.
     */

    class Composite_Array_Reference : public Composite_Unary_Node
    {
    public:
      /**
       * Constructor
       * @param   key      the variable name
       * @param   index    index within the array
       * @param   context  the context the variable is contained in
       **/
      Composite_Array_Reference (
        const std::string &key, Component_Node * index,
        madara::knowledge::Thread_Safe_Context & context);
      
      /// Dtor.
      virtual ~Composite_Array_Reference (void);
      
      /// Sets the value stored in the node.
      Knowledge_Record dec (
        const madara::knowledge::Knowledge_Update_Settings & settings =
          knowledge::Knowledge_Update_Settings ());
      
      /// Sets the value stored in the node.
      Knowledge_Record inc (
        const madara::knowledge::Knowledge_Update_Settings & settings =
          knowledge::Knowledge_Update_Settings ());
      
      /// Return the item stored in the node.
      virtual madara::Knowledge_Record item (void) const;

      /// Sets the value stored in the node.
      int set (const madara::Knowledge_Record::Integer & value,
        const madara::knowledge::Knowledge_Update_Settings & settings =
        knowledge::Knowledge_Update_Settings ());
      
      /// Sets the value stored in the node.
      int set (double value,
        const madara::knowledge::Knowledge_Update_Settings & settings =
        knowledge::Knowledge_Update_Settings ());
      
      /// Sets the value stored in the node.
      int set (const madara::Knowledge_Record & value,
        const madara::knowledge::Knowledge_Update_Settings & settings =
        knowledge::Knowledge_Update_Settings ());

      /// Prune the tree of unnecessary nodes. 
      /// Returns evaluation of the node and sets can_change appropriately.
      /// if this node can be changed, that means it shouldn't be pruned.
      virtual madara::Knowledge_Record prune (bool & can_change);

      /// Evaluates the node and its children. This does not prune any of
      /// the expression tree, and is much faster than the prune function
      virtual madara::Knowledge_Record evaluate (
        const madara::knowledge::Knowledge_Update_Settings & settings);

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
      madara::Knowledge_Record * get_record (void)
      {
        if (record_)
          return record_;
        else
          return context_.get_record (expand_key ());
      }

    private:
      /// Key for retrieving value of this variable.
      const std::string key_;
      madara::Knowledge_Record * record_;

      madara::knowledge::Thread_Safe_Context & context_;

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

#endif /* _MADARA_COMPOSITE_ARRAY_REFERENCE_ */
