
#ifndef _MADARA_VECTOR_H_
#define _MADARA_VECTOR_H_

#include <vector>
#include <string>
#include <iostream>
#include "madara/Lock_Type.h"
#include "madara/knowledge_engine/Knowledge_Base.h"
#include "madara/knowledge_engine/Thread_Safe_Context.h"
#include "madara/knowledge_engine/Knowledge_Update_Settings.h"

/**
 * @file Vector.h
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains a C++ object that manages interactions for an
 * array of variables
 **/

namespace Madara
{
  namespace Knowledge_Engine
  {
    namespace Containers
    {
      // forward declare a friend class
      class Queue;

      /**
       * @class Vector
       * @brief This class stores a vector of KaRL variables
       */
      class MADARA_Export Vector
      {
      public:
        /// Allow the Queue class to manipulate variables
        friend class Queue;

        /**
         * Default constructor
         * @param  settings   settings for evaluating the vector
         **/
        Vector (const Knowledge_Update_Settings & settings = Knowledge_Update_Settings ());

        /**
         * Constructor
         * @param  name       name of the vector in the knowledge base
         * @param  size       size of the vector
         * @param  knowledge  the knowledge base that will contain the vector
         * @param  delete_vars delete indices outside of the specified range
         * @param  settings   settings for evaluating the vector
         **/
        Vector (const std::string & name, 
                Knowledge_Base & knowledge,
                int size = -1,
                bool delete_vars = true,
                const Knowledge_Update_Settings & settings = Knowledge_Update_Settings ());
      
        /**
         * Constructor
         * @param  name       name of the vector in the knowledge base
         * @param  size       size of the vector
         * @param  knowledge  the knowledge base that will contain the vector
         * @param  delete_vars delete indices outside of the specified range
         * @param  settings   settings for evaluating the vector
         **/
        Vector (const std::string & name,
                Variables & knowledge,
                int size = -1,
                bool delete_vars = true,
                const Knowledge_Update_Settings & settings = Knowledge_Update_Settings ());
      
        /**
         * Copy constructor
         **/
        Vector (const Vector & rhs);

        /**
         * Destructor
         **/
        ~Vector ();
        
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
         * Output debug information to stream
         * @param  index  the index to modify
         **/
        void debug (std::ostream & output, bool show_only_modifieds = false);
    
        /**
         * Assignment operator
         * @param  rhs    value to copy
         **/
        void operator= (const Vector & rhs);

        /**
         * Resizes the vector
         * @param   size   maximum size of the vector
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
        void exchange (Vector & other, bool refresh_keys = true,
          bool delete_keys = true);

        /**
         * Transfers elements from this vector to another
         * @param  other  the other vector to transfer to
         **/
        void transfer_to (Vector & other);

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
         * Retrieves a copy of the record from the map.
         * @param  index  the index of the variable entry
         * @return the value of the entry. Modifications to this will
         *         not be reflected in the context. This is a local copy.
         **/
        Knowledge_Record operator[] (size_t index) const;
        
        /**
         * Retrieves a copy of the record from the map. Same functionality
         * as operator[]. This is provided for convenience to match other
         * containers way of converting to a record.
         * @param  index  the index of the variable entry
         * @return the value of the entry. Modifications to this will
         *         not be reflected in the context. This is a local copy.
         **/
        Knowledge_Record to_record (size_t index) const;
        
        /**
         * Copies the vector elements to an STL vector of Knowledge Records
         * @param  target   record values at this instance
         **/
        void copy_to (Knowledge_Vector & target) const;
        
        /**
         * Checks to see if the index has ever been assigned a value
         * @param  index  the index of the variable entry
         * @return true if the record has been set to a value. False is
         *         uninitialized
         **/
        bool exists (size_t index) const;
      
        /**
         * Read a file into the index
         * @param filename           file to read
         * @param index              index within vector
         */
        int read_file (unsigned int index, 
                       const std::string & filename);
      
        /**
         * Read a file into the index
         * @param filename           file to read
         * @param index              index within vector
         * @param settings           settings to use when evaluating/updating
         */
        int read_file (unsigned int index, 
                       const std::string & filename, 
          const Knowledge_Update_Settings & settings);
      
        /**
         * Atomically sets the value of an index to an arbitrary string.
         * @param   index     index within vector
         * @param   value     new value of the variable
         * @param   size      indicates the size of the value buffer
         * @return   0 if the value was set. -1 if null key
         **/
        int set_file (unsigned int index,
          const unsigned char * value, size_t size);
      
        /**
         * Atomically sets the value of an index to an arbitrary string.
         * @param   index     index within vector
         * @param   value     new value of the variable
         * @param   size      indicates the size of the value buffer
         * @param   settings  settings for applying the update
         * @return   0 if the value was set. -1 if null key
         **/
        int set_file (unsigned int index,
          const unsigned char * value, size_t size, 
          const Knowledge_Update_Settings & settings);
      
        /**
         * Atomically sets the value of an index to a JPEG image
         * @param   index     index of the variable to set
         * @param   value     new value of the variable
         * @param   size      indicates the size of the value buffer
         * @return   0 if the value was set. -1 if null key
         **/
        int set_jpeg (unsigned int index,
          const unsigned char * value, size_t size);
              
        /**
         * Atomically sets the value of an index to a JPEG image
         * @param   index     index of the variable to set
         * @param   value     new value of the variable
         * @param   size      indicates the size of the value buffer
         * @param   settings  settings for applying the update
         * @return   0 if the value was set. -1 if null key
         **/
        int set_jpeg (unsigned int index,
          const unsigned char * value, size_t size, 
          const Knowledge_Update_Settings & settings);
        
        /**
         * Sets a knowledge variable to a specified value
         *
         * @param index           index to set
         * @param value           value to set at location
         * @return                0 if successful, -1 if key is null, and
         *                        -2 if quality isn't high enough
         **/
        int set (unsigned int index,
          Madara::Knowledge_Record::Integer value = 
            Madara::Knowledge_Record::MODIFIED);

        /**
         * Sets a knowledge variable to a specified value
         *
         * @param index           index to set
         * @param value           value to set at location
         * @param settings        settings for applying the update
         * @return                0 if successful, -1 if key is null, and
         *                        -2 if quality isn't high enough
         **/
        int set (unsigned int index,
          Madara::Knowledge_Record::Integer value, 
          const Knowledge_Update_Settings & settings);

        /**
         * Sets an index within an array to a specified value
         *
         * @param index           index to set in the variable
         * @param sub_index       index of the location in the array
         * @param value           value to set at location
         * @return                0 if successful, -1 if key is null, and
         *                        -2 if quality isn't high enough
         **/
        int set_index (unsigned int index,
          size_t sub_index,
          Madara::Knowledge_Record::Integer value);
        
        /**
         * Sets an index within an array to a specified value
         *
         * @param index           index to set in the variable
         * @param sub_index       index of the location in the array
         * @param value           value to set at location
         * @param settings        settings for applying the update
         * @return                0 if successful, -1 if key is null, and
         *                        -2 if quality isn't high enough
         **/
        int set_index (unsigned int index,
          size_t sub_index,
          Madara::Knowledge_Record::Integer value,
          const Knowledge_Update_Settings & settings);
        
        /**
         * Sets a knowledge variable to a specified value
         *
         * @param index           index to set
         * @param value           array of integers to set at the location
         * @param size            number of elements in the array
         * @return                0 if successful, -1 if key is null, and
         *                        -2 if quality isn't high enough
         **/
        int set (unsigned int index,
          const Madara::Knowledge_Record::Integer * value,
          uint32_t size);
       
        /**
         * Sets a knowledge variable to a specified value
         *
         * @param index           index to set
         * @param value           array of integers to set at the location
         * @param size            number of elements in the array
         * @param settings        settings for applying the update
         * @return                0 if successful, -1 if key is null, and
         *                        -2 if quality isn't high enough
         **/
        int set (unsigned int index,
          const Madara::Knowledge_Record::Integer * value,
          uint32_t size,
          const Knowledge_Update_Settings & settings);
       
        /**
         * Sets a knowledge variable to a specified value
         *
         * @param index           index to set
         * @param value           array of integers to set at the location
         * @return                0 if successful, -1 if key is null, and
         *                        -2 if quality isn't high enough
         **/
        int set (unsigned int index,
          const std::vector <Knowledge_Record::Integer> & value);
       
        /**
         * Sets a knowledge variable to a specified value
         *
         * @param index           index to set
         * @param value           array of integers to set at the location
         * @param settings        settings for applying the update
         * @return                0 if successful, -1 if key is null, and
         *                        -2 if quality isn't high enough
         **/
        int set (unsigned int index,
          const std::vector <Knowledge_Record::Integer> & value,
          const Knowledge_Update_Settings & settings);
       
        /**
         * Sets a knowledge variable to a specified value
         *
         * @param index           index to set
         * @param value           value to set at location
         * @return                0 if successful, -1 if key is null, and
         *                        -2 if quality isn't high enough
         **/
        int set (unsigned int index, double value);

        /**
         * Sets a knowledge variable to a specified value
         *
         * @param index           index to set
         * @param value           value to set at location
         * @param settings        settings for applying the update
         * @return                0 if successful, -1 if key is null, and
         *                        -2 if quality isn't high enough
         **/
        int set (unsigned int index, double value, 
          const Knowledge_Update_Settings & settings);
        
        /**
         * Sets an index within an array to a specified value
         *
         * @param index           index to set within the variable
         * @param sub_index       index of the location in the array
         * @param value           value to set at location
         * @return                0 if successful, -1 if key is null, and
         *                        -2 if quality isn't high enough
         **/
        int set_index (unsigned int index,
          size_t sub_index,
          double value);

        /**
         * Sets an index within an array to a specified value
         *
         * @param index           index to set within the variable
         * @param sub_index       index of the location in the array
         * @param value           value to set at location
         * @param settings        settings for applying the update
         * @return                0 if successful, -1 if key is null, and
         *                        -2 if quality isn't high enough
         **/
        int set_index (unsigned int index,
          size_t sub_index,
          double value,
          const Knowledge_Update_Settings & settings);
        
        /**
         * Sets a knowledge variable to a specified value
         *
         * @param index           index to set
         * @param value           array of doubles to set at the location
         * @param size            number of elements in the array
         * @return                0 if successful, -1 if key is null, and
         *                        -2 if quality isn't high enough
         **/
        int set (unsigned int index,
          const double * value,
          uint32_t size);
       
        /**
         * Sets a knowledge variable to a specified value
         *
         * @param index           index to set
         * @param value           array of doubles to set at the location
         * @param size            number of elements in the array
         * @param settings        settings for applying the update
         * @return                0 if successful, -1 if key is null, and
         *                        -2 if quality isn't high enough
         **/
        int set (unsigned int index,
          const double * value,
          uint32_t size,
          const Knowledge_Update_Settings & settings);
       
        /**
         * Sets a knowledge variable to a specified value
         *
         * @param index           index to set
         * @param value           array of doubles to set at the location
         * @return                0 if successful, -1 if key is null, and
         *                        -2 if quality isn't high enough
         **/
        int set (unsigned int index,
          const std::vector <double> & value);
        
        /**
         * Sets a knowledge variable to a specified value
         *
         * @param index           index to set
         * @param value           array of doubles to set at the location
         * @param settings        settings for applying the update
         * @return                0 if successful, -1 if key is null, and
         *                        -2 if quality isn't high enough
         **/
        int set (unsigned int index,
          const std::vector <double> & value,
          const Knowledge_Update_Settings & settings);
        
        /**
         * Sets a knowledge variable to a specified value
         *
         * @param index           index to set
         * @param value           value to set at location
         * @return                0 if successful, -1 if key is null, and
         *                        -2 if quality isn't high enough
         **/
        int set (unsigned int index, const std::string & value);
        
        /**
         * Sets a knowledge variable to a specified value
         *
         * @param index           index to set
         * @param value           value to set at location
         * @param settings        settings for applying the update
         * @return                0 if successful, -1 if key is null, and
         *                        -2 if quality isn't high enough
         **/
        int set (unsigned int index, const std::string & value, 
          const Knowledge_Update_Settings & settings);
        
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
        void set_quality (unsigned int index, uint32_t quality,
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

      /// provide the Array alias for the Vector class
      typedef  Vector   Array;
    }
  }
}




#endif // _MADARA_VECTOR_H_