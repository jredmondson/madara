#ifndef  _VARIABLE_REFERENCE_H_
#define  _VARIABLE_REFERENCE_H_


/**
 * @file Compiled_Expression.h
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains the Compiled_Expression class which is used by
 * the madara::knowledge::Knowledge_Base class
 */

#include <string>
#include "madara/MADARA_export.h"
#include "madara/expression_tree/Expression_Tree.h"
#include "madara/utility/Scoped_Array.h"

namespace madara
{
  class Knowledge_Record;

  namespace knowledge
  {
    class Thread_Safe_Context;
    class Knolwedge_Base_Impl;
    class Variables;

    /**
     * @class Variable_Reference
     * @brief Optimized reference to a variable within the knowledge base
     **/
    class MADARA_Export Variable_Reference
    {
    public:
      //class Knowledge_Base_Impl;
      friend class Thread_Safe_Context;
      friend class Knowledge_Base_Impl;
      friend class Variables;

      /**
       * Default constructor
       **/
      Variable_Reference ();
      
      /**
       * Copy Constructor
       * @param   rhs      variable reference to copy from
       **/
      Variable_Reference (const Variable_Reference & rhs);

      /**
       * Destructor
       **/
      ~Variable_Reference ();

      /**
       * Checks to see if the variable reference has been initialized
       * @return  true if valid, false otherwise
       **/
      bool is_valid (void) const;

      /**
       * Assignment operator
       * @param   input      variable reference to copy from
       **/
      void operator= (const Variable_Reference & input);
      
      /**
       * Equality operator
       * @param   rhs      variable reference to compare to
       **/
      bool operator== (const Variable_Reference & rhs) const;
      
      /**
       * Inequality operator
       * @param   rhs      variable reference to compare to
       **/
      bool operator!= (const Variable_Reference & rhs) const;

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
      utility::Scoped_Array <const char> name_;

      /**
       * Reference to knowledge record
       **/
      Knowledge_Record * record_;
    };
  }
}

#endif   // _VARIABLE_REFERENCE_H_