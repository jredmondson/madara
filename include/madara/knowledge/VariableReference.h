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
#include "madara/MADARA_export.h"
#include "madara/expression/ExpressionTree.h"
#include "madara/utility/ScopedArray.h"

namespace madara
{
  namespace knowledge
  {
    class ThreadSafeContext;
    class KnolwedgeBaseImpl;
    class Variables;

    /**
     * @class VariableReference
     * @brief Optimized reference to a variable within the knowledge base
     **/
    class MADARA_Export VariableReference
    {
    public:
      friend class ThreadSafeContext;
      friend class KnowledgeBaseImpl;
      friend class Variables;

      /**
       * Default constructor
       **/
      VariableReference ();
      
      /**
       * Copy Constructor
       * @param   rhs      variable reference to copy from
       **/
      VariableReference (const VariableReference & rhs);

      /**
       * Destructor
       **/
      ~VariableReference ();

      /**
       * Checks to see if the variable reference has been initialized
       * @return  true if valid, false otherwise
       **/
      bool is_valid (void) const;

      /**
       * Assignment operator
       * @param   input      variable reference to copy from
       **/
      void operator= (const VariableReference & input);
      
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
       * Returns the name of the variable
       * @return    the name of the referenced variable
       **/
      const char * get_name (void);

    private:
      
      /**
       * Sets the name of the variable
       * @param    name    the name of the variable
       **/
      void set_name (const std::string & name);
      
      /**
       * potential string value of the node (size int)
       **/
      utility::ScopedArray <const char> name_;

      /**
       * Reference to knowledge record
       **/
      knowledge::KnowledgeRecord * record_;
    };
  }
}

#endif   // _MADARA_VARIABLE_REFERENCE_H_
