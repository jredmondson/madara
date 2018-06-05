
#ifndef _MADARA_CONTAINERS_INTEGER_VECTOR_H_
#define _MADARA_CONTAINERS_INTEGER_VECTOR_H_

#include <vector>
#include <string>
#include "madara/LockType.h"
#include "madara/knowledge/KnowledgeBase.h"
#include "madara/knowledge/ThreadSafeContext.h"
#include "madara/knowledge/KnowledgeUpdateSettings.h"
#include "BaseContainer.h"

/**
 * @file IntegerVector.h
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains a C++ object that manages interactions for an
 * array of integers
 **/

namespace madara
{
  namespace knowledge
  {
    namespace containers
    {
      /**
       * @class IntegerVector
       * @brief This class stores a vector of integers inside of KaRL
       */
      class MADARA_EXPORT IntegerVector : public BaseContainer
      {
      public:
        /// trait that describes the value type
        typedef knowledge::KnowledgeRecord::Integer  type;
        
        /**
         * Default constructor
         * @param  settings   settings for evaluating the vector
         * @param  delimiter  the delimiter for variables in the vector
         **/
        IntegerVector (const KnowledgeUpdateSettings & settings =
            KnowledgeUpdateSettings (),
          const std::string & delimiter = ".");

        /**
         * Constructor
         * @param  name       name of the vector in the knowledge base
         * @param  size       size of the vector. -1 to check for size.
         * @param  knowledge  the knowledge base that will contain the vector
         * @param  delete_vars delete indices outside of the specified range
         * @param  settings   settings for evaluating the vector
         * @param  delimiter  the delimiter for variables in the vector
         **/
        IntegerVector (const std::string & name, 
          KnowledgeBase & knowledge,
          int size = -1,
          bool delete_vars = true,
          const KnowledgeUpdateSettings & settings =
            KnowledgeUpdateSettings (),
          const std::string & delimiter = ".");
      
        /**
         * Constructor
         * @param  name       name of the vector in the knowledge base
         * @param  size       size of the vector. -1 to check for size.
         * @param  knowledge  the knowledge base that will contain the vector
         * @param  delete_vars delete indices outside of the specified range
         * @param  settings   settings for evaluating the vector
         * @param  delimiter  the delimiter for variables in the vector
         **/
        IntegerVector (const std::string & name,
          Variables & knowledge,
          int size = -1,
          bool delete_vars = true,
          const KnowledgeUpdateSettings & settings =
            KnowledgeUpdateSettings (),
          const std::string & delimiter = ".");
      
        /**
         * Copy constructor
         **/
        IntegerVector (const IntegerVector & rhs);

        /**
         * Destructor
         **/
        virtual ~IntegerVector ();

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
        void modify (size_t index);
            
        /**
         * Assignment operator
         * @param  rhs    value to copy
         **/
        void operator= (const IntegerVector & rhs);

        /**
        * Pushes the value to the end of the array after incrementing the
        * array size.
        * @param  value       the value to place at the end of the array
        **/
        void push_back (type value);

        /**
         * Resizes the vector
         * @param   size   maximum size of the vector. Can be -1 to check
         *                 the knowledge base for size information)
         * @param  delete_vars delete indices outside of the specified range
         **/
        void resize (int size = -1, bool delete_vars = true);
      
        /**
         * Exchanges the vector at this location with the vector at another
         * location.
         * @param  other   the other vector to exchange with
         * @param  refresh_keys  force a refresh of the keys in both maps to
         *                       ensure all keys are swapped
         * @param  delete_keys   delete any unused keys
         **/
        void exchange (IntegerVector & other, bool refresh_keys = true,
          bool delete_keys = true);

        /**
         * Transfers elements from this vector to another
         * @param  other  the other vector to transfer to
         **/
        void transfer_to (IntegerVector & other);
        
        /**
         * Copies the vector elements to an STL vector of Knowledge Records
         * @param  target   record values at this instance
         **/
        void copy_to (KnowledgeVector & target) const;

        /**
        * Copies the vector elements to an STL vector
        * @param  target   the target of the copy operation
        **/
        void copy_to (std::vector <type> & target) const;

        /**
         * Returns the size of the local vector. Call resize() first without
         * arguments to ensure local vector matches data in KnowledgeBase.
         * @return size of the vector
         **/
        size_t size (void) const;
      
        /**
         * Sets the variable name that this refers to
         * @param var_name  the name of the variable in the knowledge base
         * @param knowledge  the knowledge base the variable is housed in
         * @param size       size of the new vector (-1 to not change size)
         **/
        void set_name (const std::string & var_name,
          KnowledgeBase & knowledge, int size = -1);
        
        /**
         * Sets the variable name that this refers to
         * @param var_name  the name of the variable in the knowledge base
         * @param knowledge  the knowledge base the variable is housed in
         * @param size       size of the new vector (-1 to not change size)
         **/
        void set_name (const std::string & var_name,
          Variables & knowledge, int size = -1);
        
        /**
         * Sets the variable name that this refers to
         * @param var_name  the name of the variable in the knowledge base
         * @param knowledge  the knowledge base the variable is housed in
         * @param size       size of the new vector (-1 to not change size)
         **/
        void set_name (const std::string & var_name,
          ThreadSafeContext & knowledge, int size = -1);

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
         * Retrieves a copy of the record from the map.
         * @param  index  the index of the variable entry
         * @return the value of the entry. Modifications to this will
         *         not be reflected in the context. This is a local copy.
         **/
        type operator[] (size_t index) const;
      
        /**
         * Retrieves a copy of the record from the vector.
         * @param  index  the index of the variable entry
         * @return the value of the entry. Modifications to this will
         *         not be reflected in the context. This is a local copy.
         **/
        knowledge::KnowledgeRecord to_record (size_t index) const;
      
        /**
         * Retrieves the entire vector as a native double array in a record 
         * @return the vector in native double array format
         **/
        knowledge::KnowledgeRecord to_record (void) const;
      
        /**
         * Checks to see if the index has ever been assigned a value
         * @param  index  the index of the variable entry
         * @return true if the record has been set to a value. False is
         *         uninitialized
         **/
        bool exists (size_t index) const;

        /**
        * Increments an index by a specified value
        *
        * @param index           index to set
        * @return                new value
        **/
        type inc (size_t index);

        /**
         * Sets a knowledge variable to a specified value
         *
         * @param index           index to set
         * @param value           value to set at location
         * @return                0 if successful, -1 if key is null, and
         *                        -2 if quality isn't high enough
         **/
        int set (size_t index,
          const type & value = 
            madara::knowledge::KnowledgeRecord::MODIFIED);

        /**
         * Sets a knowledge variable to a specified value
         *
         * @param index           index to set
         * @param value           value to set at location
         * @param settings        settings for applying the update
         * @return                0 if successful, -1 if key is null, and
         *                        -2 if quality isn't high enough
         **/
        int set (size_t index,
          const type & value, 
          const KnowledgeUpdateSettings & settings);

        /**
         * Reads values from a STL vector of integers
         *
         * @param value           array of integers to set at the location
         * @return                0 if successful, -1 if key is null, and
         *                        -2 if quality isn't high enough
         **/
        int set (
          const std::vector <type> & value);
       
        /**
         * Reads values from a STL vector of integers
         *
         * @param value           array of integers to set at the location
         * @param settings        settings for applying the update
         * @return                0 if successful, -1 if key is null, and
         *                        -2 if quality isn't high enough
         **/
        int set (
          const std::vector <type> & value,
          const KnowledgeUpdateSettings & settings);
       
        /**
         * Sets the quality of writing to a certain variable from this entity
         *
         * @param index           index to set
         * @param quality         quality of writing to this location
         * @param settings        settings for referring to knowledge variables
         **/
        void set_quality (size_t index, uint32_t quality,
               const KnowledgeReferenceSettings & settings =
                       KnowledgeReferenceSettings (false));
      
        /**
         * Returns a reference to the size field of the current name
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
        std::vector <VariableReference> vector_;

        /**
         * Reference to the size field of the vector space
         **/
        VariableReference size_;

        /**
        * Delimiter for the prefix to subvars
        **/
        std::string delimiter_;
      };

      /// provide the Array alias for the IntegerVector class
      typedef  IntegerVector   IntegerArray;
    }
  }
}

#endif // _MADARA_CONTAINERS_INTEGER_VECTOR_H_
