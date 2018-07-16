#ifndef  _MADARA_VARIABLE_REFERENCE_H_
#define  _MADARA_VARIABLE_REFERENCE_H_


/**
 * @file CompiledExpression.h
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains the CompiledExpression class which is used by
 * the madara::knowledge::KnowledgeBase class
 */

#include <string>
#include <cstring>
#include "madara/MadaraExport.h"
#include "madara/expression/ExpressionTree.h"
#include "madara/utility/ScopedArray.h"

namespace madara
{
  namespace knowledge
  {
    class ThreadSafeContext;
    class KnolwedgeBaseImpl;
    class Variables;

    namespace rcw
    {
      class BaseTracker;
    }

    /**
     * @class VariableReference
     * @brief Optimized reference to a variable within the knowledge base
     **/
    class MADARA_EXPORT VariableReference
    {
    public:
      friend class ThreadSafeContext;
      friend class KnowledgeBaseImpl;
      friend class Variables;
      friend class rcw::BaseTracker;

      using pair_ptr = KnowledgeMap::value_type *;

      /**
       * Default constructor
       **/
      VariableReference () = default;

      /**
       * Construct from a pointer to entry in a KnowledgeMap
       **/
      VariableReference (pair_ptr entry);

      /**
       * Checks to see if the variable reference has been initialized
       * @return  true if valid, false otherwise
       **/
      bool is_valid (void) const;

      /**
       * Equality operator
       * @param   rhs      variable reference to compare to
       **/
      bool operator== (const VariableReference & rhs) const;

      /**
       * Inequality operator
       * @param   rhs      variable reference to compare to
       **/
      bool operator!= (const VariableReference & rhs) const;

      /**
       * Returns the name of the variable.
       * Returns nullptr if is_valid() is false.
       *
       * @return    the name of the referenced variable
       **/
      const char * get_name (void) const;

      /**
       * Returns a pointer to the variable's KnowledgeRecord
       * Do not use this pointer unless you've locked the ThreadSafeContext
       * it belongs to. Returns nullptr if is_valid() is false.
       *
       * @return    the pointer to the KnowledgeRecord
       **/
      KnowledgeRecord *get_record_unsafe (void) const;

      /**
       * Refer to a different variable
       **/
      void assign (pair_ptr entry);

    private:
      pair_ptr entry_ = nullptr;
    };

    /// a vector of variable references
    typedef std::vector <VariableReference>  VariableReferences;

    /// a map of variable references
    typedef std::map <const char *, VariableReference,
            compare_const_char_ptr> VariableReferenceMap;
  }
}

#include "VariableReference.inl"

#endif   // _MADARA_VARIABLE_REFERENCE_H_
