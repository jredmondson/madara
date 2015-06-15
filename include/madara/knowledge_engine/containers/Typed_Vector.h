
#ifndef _MADARA_CONTAINERS_TYPED_VECTOR_H_
#define _MADARA_CONTAINERS_TYPED_VECTOR_H_

#include <vector>
#include <string>
#include <iostream>
#include "madara/Lock_Type.h"
#include "madara/knowledge_engine/Knowledge_Base.h"
#include "madara/knowledge_engine/Thread_Safe_Context.h"

/**
 * @file Typed_Vector.h
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains a C++ object that manages interactions for an
 * array of user-defined, madara-enabled types
 **/

namespace Madara
{
  namespace Knowledge_Engine
  {
    namespace Containers
    {
      /**
       * @class Typed_Vector
       * @brief This class stores a vector of user-defined, MADARA-enabled
       *        container classes that are linked to a knowledge base
       */
      template <class T>
      class MADARA_Export Typed_Vector
      {
      public:
        /**
         * Default constructor
         * @param  settings   settings for evaluating the vector
         * @param  delimiter  the delimiter for variables in the vector
         **/
        Typed_Vector (const Knowledge_Update_Settings & settings =
            Knowledge_Update_Settings (),
          const std::string & delimiter = ".");

        /**
         * Constructor
         * @param  name       name of the vector in the knowledge base
         * @param  size       size of the vector. -1 will check knowledge
         *                    base for size information
         * @param  knowledge  the knowledge base that will contain the vector
         * @param  delete_vars delete indices outside of the specified range
         * @param  settings   settings for evaluating the vector
         * @param  delimiter  the delimiter for variables in the vector
         **/
        Typed_Vector (const std::string & name, 
          Knowledge_Base & knowledge,
          int size = -1,
          bool delete_vars = true,
          const Knowledge_Update_Settings & settings =
            Knowledge_Update_Settings (),
          const std::string & delimiter = ".");
      
        /**
         * Constructor
         * @param  name       name of the vector in the knowledge base
         * @param  size       size of the vector. -1 will check knowledge
         *                    base for size information
         * @param  knowledge  the knowledge base that will contain the vector
         * @param  delete_vars delete indices outside of the specified range
         * @param  settings   settings for evaluating the vector
         * @param  delimiter  the delimiter for variables in the vector
         **/
        Typed_Vector (const std::string & name,
          Variables & knowledge,
          int size = -1,
          bool delete_vars = true,
          const Knowledge_Update_Settings & settings =
            Knowledge_Update_Settings (),
          const std::string & delimiter = ".");
      
        /**
         * Copy constructor
         **/
        Typed_Vector (const Typed_Vector<T> & rhs);

        /**
         * Destructor
         **/
        ~Typed_Vector ();
        
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
        void operator= (const Typed_Vector<T> & rhs);

        /**
        * Pushes the user class instance to the end of the array after
        * incrementing the array size.
        * @param  value       the user class to place at the end of the array
        **/
        void push_back (const T & value);

        /**
         * Resizes the vector
         * @param  size        size of the vector. -1 will check knowledge
         *                     base for size information
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
        void exchange (Typed_Vector<T> & other, bool refresh_keys = true,
          bool delete_keys = true);

        /**
         * Transfers elements from this vector to another
         * @param  other  the other vector to transfer to
         **/
        void transfer_to (Typed_Vector<T> & other);

        /**
         * Returns the size of the vector
         * @return size of the vector
         **/
        size_t size (void) const;
      
        /**
         * Returns the name of the vector
         * @return name of the vector
         **/
        std::string get_name (void) const;
        
        /**
         * Sets the variable name that this refers to
         * @param var_name  the name of the variable in the knowledge base
         * @param knowledge  the knowledge base the variable is housed in
         * @param size       size of the new vector (-1 to not change size)
         **/
        void set_name (const std::string & var_name,
          Knowledge_Base & knowledge, int size = -1);
        
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
          Thread_Safe_Context & knowledge, int size = -1);

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
         * Retrieves a copy of the user-defined class from the list.
         * @param  index  the index of the variable entry
         * @return the value of the entry. Modifications to this will
         *         not be reflected in the context. This is a local copy.
         **/
        T operator[] (size_t index) const;

        /**
        * Retrieves the user-defined class instance from the list. This is a
        * more efficient operation than the const version because this version
        * is in-place with no copying.
        * @param  index  the index of the user element to retrieve
        * @return the actual element in the array. Modifications to
        *         this element will be reflected in the vector itself.
        **/
        T & operator[] (size_t index);

        /**
         * Sets the update settings for the variable
         * @param  settings  the new settings to use
         * @return the old update settings
         **/
        Knowledge_Update_Settings set_settings (
          const Knowledge_Update_Settings & settings);

        /**
         * Sets the quality of writing to a certain variable from this entity
         *
         * @param index           index to set
         * @param quality         quality of writing to this location
         * @param settings        settings for referring to knowledge variables
         **/
        void set_quality (size_t index, uint32_t quality,
               const Knowledge_Reference_Settings & settings =
                       Knowledge_Reference_Settings (false));
      
      private:
        /**
        * Returns a reference to the size field of the current name
        * @return reference to the size field
        **/
        Variable_Reference get_size_ref (void);

        /// guard for access and changes
        typedef ACE_Guard<MADARA_LOCK_TYPE> Guard;
      
        /**
         * Mutex for local changes
         **/
        mutable MADARA_LOCK_TYPE mutex_;

        /**
         * Variable context that we are modifying
         **/
        Thread_Safe_Context * context_;

        /**
         * Prefix of variable
         **/
        std::string name_;

        /**
         * Values of the array
         **/
        std::vector <T> vector_;

        /**
         * Reference to the size field of the vector space
         **/
        Variable_Reference size_;

        /**
         * Settings for modifications
         **/
        Knowledge_Update_Settings settings_;

        /**
        * Delimiter for the prefix to subvars
        **/
        std::string delimiter_;

        /**
         * A reference to return in case of undefined accesses 
         **/
        T undefined_;
      };
    }
  }
}




#endif // _MADARA_CONTAINERS_TYPED_VECTOR_H_