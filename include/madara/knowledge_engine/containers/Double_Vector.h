
#ifndef _MADARA_DOUBLE_VECTOR_H_
#define _MADARA_DOUBLE_VECTOR_H_

#include <vector>
#include <string>
#include "madara/Lock_Type.h"
#include "madara/knowledge_engine/Knowledge_Base.h"
#include "madara/knowledge_engine/Thread_Safe_Context.h"
#include "madara/knowledge_engine/Knowledge_Update_Settings.h"

/**
 * @file Double_Vector.h
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains a C++ object that manages interactions for an
 * array of doubles
 **/

namespace Madara
{
  namespace Knowledge_Engine
  {
    namespace Containers
    {
      /**
       * @class Double_Vector
       * @brief This class stores a vector of doubles inside of KaRL
       */
      class MADARA_Export Double_Vector
      {
      public:
        /// trait that describes the value type
        typedef double  type;
        
        /**
         * Default constructor
         * @param  settings   settings for evaluating the vector
         **/
        Double_Vector (const Knowledge_Update_Settings & settings =
          Knowledge_Update_Settings ());

        /**
         * Constructor
         * @param  name       name of the vector in the knowledge base
         * @param  size       size of the vector. -1 to check for size.
         * @param  knowledge  the knowledge base that will contain the vector
         * @param  delete_vars delete indices outside of the specified range
         * @param  settings   settings for evaluating the vector
         **/
        Double_Vector (const std::string & name, 
          Knowledge_Base & knowledge,
          int size = -1,
          bool delete_vars = true,
          const Knowledge_Update_Settings & settings =
            Knowledge_Update_Settings ());
      
        /**
         * Constructor
         * @param  name       name of the vector in the knowledge base
         * @param  size       size of the vector. -1 to check for size.
         * @param  knowledge  the knowledge base that will contain the vector
         * @param  delete_vars delete indices outside of the specified range
         * @param  settings   settings for evaluating the vector
         **/
        Double_Vector (const std::string & name,
          Variables & knowledge,
          int size = -1,
          bool delete_vars = true,
          const Knowledge_Update_Settings & settings =
            Knowledge_Update_Settings ());
      
        /**
         * Copy constructor
         **/
        Double_Vector (const Double_Vector & rhs);

        /**
         * Destructor
         **/
        ~Double_Vector ();
        
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
        void operator= (const Double_Vector & rhs);

        /**
        * Pushes the value to the end of the array after incrementing the
        * array size.
        * @param  value       the value to place at the end of the array
        * @param  delete_vars delete indices outside of the specified range
        **/
        void push_back (type value, bool delete_vars = true);

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
        void exchange (Double_Vector & other, bool refresh_keys = true,
          bool delete_keys = true);
        
        /**
         * Copies the vector elements to an STL vector of Knowledge Records
         * @param  target   record values at this instance
         **/
        void copy_to (Knowledge_Vector & target) const;
        
        /**
         * Transfers elements from this vector to another
         * @param  other  the other vector to transfer to
         **/
        void transfer_to (Double_Vector & other);

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
         * Retrieves a copy of the record from the vector.
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
        Knowledge_Record to_record (size_t index) const;
      
        /**
         * Retrieves the entire vector as a native double array in a record 
         * @return the vector in native double array format
         **/
        Knowledge_Record to_record (void) const;
      
        /**
         * Checks to see if the index has ever been assigned a value
         * @param  index  the index of the variable entry
         * @return true if the record has been set to a value. False is
         *         uninitialized
         **/
        bool exists (size_t index) const;
      
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
          type value, 
          const Knowledge_Update_Settings & settings =
            Knowledge_Update_Settings ());

        /**
         * Reads values from a STL vector of doubles
         *
         * @param value           array of doubles to set at the location
         * @param settings        settings for applying the update
         * @return                0 if successful, -1 if key is null, and
         *                        -2 if quality isn't high enough
         **/
        int set (
          const std::vector <type> & value,
          const Knowledge_Update_Settings & settings =
            Knowledge_Update_Settings ());
       
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
      
        /**
         * Returns a reference to the size field of the current name
         * @return reference to the size field
         **/
        Variable_Reference get_size_ref (void);

      private:
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
        std::vector <Variable_Reference> vector_;

        /**
         * Reference to the size field of the vector space
         **/
        Variable_Reference size_;

        /**
         * Settings for modifications
         **/
        Knowledge_Update_Settings settings_;
      };

      /// provide the Array alias for the Double_Vector class
      typedef  Double_Vector   Double_Array;
    }
  }
}

#endif // _MADARA_DOUBLE_VECTOR_H_
