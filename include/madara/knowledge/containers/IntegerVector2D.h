
#ifndef _MADARA_CONTAINERS_INTEGER_VECTOR2D_H_
#define _MADARA_CONTAINERS_INTEGER_VECTOR2D_H_

#include <vector>
#include <string>
#include "madara/LockType.h"
#include "madara/knowledge/KnowledgeBase.h"
#include "madara/knowledge/ThreadSafeContext.h"
#include "madara/knowledge/KnowledgeUpdateSettings.h"
#include "BaseContainer.h"

/**
 * @file IntegerVector2D.h
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains a C++ object that manages interactions for a
 * 2D array of integers that can have lazy updates on each cell.
 **/

namespace madara
{
  namespace knowledge
  {
    namespace containers
    {
      /**
       * @class IntegerVector2D
       * @brief Manages a 2D array of integers as a virtual overlay in the
       *        KnowledgeBase
       */
      class MADARA_EXPORT IntegerVector2D : public BaseContainer
      {
      public:
        /// two dimensional indexing
        struct Indices
        {
          std::size_t x, y;
        };

        /// convenience typedef for size
        typedef Indices  Dimensions;

        /// convenience typedef for element type
        typedef KnowledgeRecord::Integer type;

        /**
         * Default constructor
         * @param  settings   settings for evaluating the vector
         * @param  delimiter  the delimiter for variables in the vector
         **/
        IntegerVector2D (const KnowledgeUpdateSettings & settings =
          KnowledgeUpdateSettings (),
          const std::string & delimiter = ".");

        /**
         * Constructor
         * @param  name       name of the vector in the knowledge base
         * @param  dimensions size of the vector. {0,0} to check for size.
         * @param  knowledge  the knowledge base that will contain the vector
         * @param  delete_vars delete indices outside of the specified range
         * @param  settings   settings for evaluating the vector
         * @param  delimiter  the delimiter for variables in the vector
         **/
        IntegerVector2D (const std::string & name, 
          KnowledgeBase & knowledge,
          const Dimensions & dimensions = {0,0},
          bool delete_vars = true,
          const KnowledgeUpdateSettings & settings =
            KnowledgeUpdateSettings (),
          const std::string & delimiter = ".");
      
        /**
         * Constructor
         * @param  name       name of the vector in the knowledge base
         * @param  dimensions size of the vector. {0,0} to check for size.
         * @param  knowledge  the knowledge base that will contain the vector
         * @param  delete_vars delete indices outside of the specified range
         * @param  settings   settings for evaluating the vector
         * @param  delimiter  the delimiter for variables in the vector
         **/
        IntegerVector2D (const std::string & name,
          Variables & knowledge,
          const Dimensions & dimensions = {0,0},
          bool delete_vars = true,
          const KnowledgeUpdateSettings & settings =
            KnowledgeUpdateSettings (),
          const std::string & delimiter = ".");
      
        /**
         * Copy constructor
         **/
        IntegerVector2D (const IntegerVector2D & rhs);

        /**
         * Destructor
         **/
        virtual ~IntegerVector2D ();
        
        /**
        * Copies the vector elements to an STL vector
        * @param  target   the target of the copy operation
        **/
        void copy_to (
          std::vector<std::vector<type> > & target) const;

        /**
         * Mark the vector as modified. The vector retains the same values
         * but will resend values as if they had been modified.
         **/
        void modify (void);

        /**
         * Mark the value as modified. The vector element retains its value
         * but will resend its value as if it had been modified.
         * @param  index  the index to modify
         **/
        void modify (const Indices & index);
    
        /**
         * Assignment operator
         * @param  rhs    value to copy
         **/
        void operator= (const IntegerVector2D & rhs);

        /**
         * Resizes the vector
         * @param   dimensions maximum size of the vector. Can be -1 to check
         *                     the knowledge base for size information)
         * @param  delete_vars delete indices outside of the specified range
         **/
        void resize (const Dimensions & dimensions, bool delete_vars = true);
      
        /**
         * Returns the size of the local vector. Call resize() first without
         * arguments to ensure local vector matches data in KnowledgeBase.
         * @return MxN size of the vector
         **/
        Dimensions size (void) const;
      
        /**
         * Sets the variable name that this refers to
         * @param var_name  the name of the variable in the knowledge base
         * @param knowledge  the knowledge base the variable is housed in
         * @param dimensions size of the new vector ({0,0} to not change size)
         **/
        void set_name (const std::string & var_name,
          KnowledgeBase & knowledge, const Dimensions & dimensions = {0,0});
        
        /**
         * Sets the variable name that this refers to
         * @param var_name  the name of the variable in the knowledge base
         * @param knowledge  the knowledge base the variable is housed in
         * @param dimensions  size of the new vector ({0,0} to not change size)
         **/
        void set_name (const std::string & var_name,
          Variables & knowledge, const Dimensions & dimensions = {0,0});
        
        /**
         * Sets the variable name that this refers to
         * @param var_name  the name of the variable in the knowledge base
         * @param knowledge  the knowledge base the variable is housed in
         * @param dimensions size of the new vector ({0,0} to not change size)
         **/
        void set_name (const std::string & var_name,
          ThreadSafeContext & knowledge, const Dimensions & dimensions = {0,0});

        /**
        * Sets the delimiter for adding and detecting subvariables. By default,
        * MADARA uses a '.' delimiter for names like "com.madara.Var1". Other
        * conventions can include '/' for ROS-like topic directory structures,
        * e.g., com/madara/Var1.
        * @param delimiter  the delimiter to use for variable demarcation
        **/
        void set_delimiter (const std::string & delimiter);

        /**
        * Gets the delimiter for adding and detecting subvariables. By default,
        * MADARA uses a '.' delimiter for names like "com.madara.Var1". Other
        * conventions can include '/' for ROS-like topic directory structures,
        * e.g., com/madara/Var1.
        * @return   the delimiter to use for variable demarcation
        **/
        std::string get_delimiter (void);

        /**
         * Retrieves an index from the multi-dimensional array
         * @param  index  the index of the variable entry
         * @return the value of the entry. Modifications to this will
         *         not be reflected in the context. This is a local copy.
         **/
        type operator[] (const Indices & index) const;
      
        /**
         * Checks to see if the index has ever been assigned a value
         * @param  index  the index of the variable entry
         * @return true if the record has been set to a value. False is
         *         uninitialized
         **/
        bool exists (const Indices & index) const;

        /**
        * Sets a knowledge variable to a specified value
        *
        * @param index           location index to set
        * @param value           value to set at location
        * @return                0 if successful, -1 if key is null, and
        *                        -2 if quality isn't high enough
        **/
        int set (const Indices & index, type value);

        /**
        * Reads values from a STL vector of KnowledgeRecord::Integers
        *
        * @param value           array to set at the location
        * @return                always returns 0
        **/
        int set (const std::vector<std::vector<type> > & value);

        /**
         * Sets a knowledge variable to a specified value
         *
         * @param index           location index to set
         * @param value           value to set at location
         * @param settings        settings for applying the update
         * @return                0 if successful, -1 if key is null, and
         *                        -2 if quality isn't high enough
         **/
        int set (const Indices & index,
          type value, 
          const KnowledgeUpdateSettings & settings);

        /**
         * Reads values from a STL vector 
         *
         * @param value           array of values to set at the location
         * @param settings        settings for applying the update
         * @return                always returns 0
         **/
        int set (
          const std::vector<std::vector<type> > & value,
          const KnowledgeUpdateSettings & settings);
       
        /**
         * Sets the quality of writing to a certain variable from this entity
         *
         * @param index           index to set
         * @param quality         quality of writing to this location
         * @param settings        settings for referring to knowledge variables
         **/
        void set_quality (const Indices & index, uint32_t quality,
               const KnowledgeReferenceSettings & settings =
                       KnowledgeReferenceSettings (false));
      
        /**
         * Returns a reference to the size of vector
         * @return reference to the size field
         **/
        VariableReference get_size_ref (void);

        /**
        * Returns the type of the container along with name and any other
        * useful information. The provided information should be useful
        * for developers wishing to debug container operations, especially
        * as it pertains to pending network operations (i.e., when used
        * in conjunction with modify)
        *
        * @return info in format {container}: {name}{ = value, if appropriate}
        **/
        std::string get_debug_info (void);

        /**
        * Clones this container
        * @return  a deep copy of the container that must be managed
        *          by the user (i.e., you have to delete the return value)
        **/
        virtual BaseContainer * clone (void) const;

        /**
        * Determines if all values in the vector are true
        * @return true if all values are true
        **/
        bool is_true (void) const;

        /**
        * Determines if the value of the vector is false
        * @return true if at least one value is false
        **/
        bool is_false (void) const;

      private:

        /**
        * Polymorphic is true method which can be used to determine if
        * all values in the container are true
        **/
        virtual bool is_true_ (void) const;

        /**
        * Polymorphic is false method which can be used to determine if
        * at least one value in the container is false
        **/
        virtual bool is_false_ (void) const;

        /**
        * Polymorphic modify method used by collection containers. This
        * method calls the modify method for this class. We separate the
        * faster version (modify) from this version (modify_) to allow
        * users the opportunity to have a fastery version that does not
        * use polymorphic functions (generally virtual functions are half
        * as efficient as normal function calls)
        **/
        virtual void modify_ (void);

        /**
        * Returns the type of the container along with name and any other
        * useful information. The provided information should be useful
        * for developers wishing to debug container operations, especially
        * as it pertains to pending network operations (i.e., when used
        * in conjunction with modify)
        *
        * @return info in format {container}: {name}{ = value, if appropriate}
        **/
        virtual std::string get_debug_info_ (void);

        /**
         * Variable context that we are modifying
         **/
        ThreadSafeContext * context_;

        /**
         * Values of the array
         **/
        std::vector <std::vector <VariableReference> > vector_;

        /**
         * Reference to the size of 2D array
         **/
        VariableReference size_;

        /**
        * Delimiter for the prefix to subvars
        **/
        std::string delimiter_;
      };
    }
  }
}

#endif // _MADARA_CONTAINERS_INTEGER_VECTOR2D_H_

