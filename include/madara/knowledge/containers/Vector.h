
#ifndef _MADARA_CONTAINERS_VECTOR_H_
#define _MADARA_CONTAINERS_VECTOR_H_

#include <vector>
#include <string>
#include <iostream>
#include "madara/LockType.h"
#include "madara/knowledge/KnowledgeBase.h"
#include "madara/knowledge/ThreadSafeContext.h"
#include "madara/knowledge/KnowledgeUpdateSettings.h"
#include "BaseContainer.h"

/**
 * @file Vector.h
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains a C++ object that manages interactions for an
 * array of variables
 **/

namespace madara
{
  namespace knowledge
  {
    namespace containers
    {
      // forward declare a friend class
      class Queue;

      /**
       * @class Vector
       * @brief This class stores a vector of KaRL variables
       */
      class MADARA_Export Vector : public BaseContainer
      {
      public:
        /// Allow the Queue class to manipulate variables
        friend class Queue;

        /**
         * Default constructor
         * @param  settings   settings for evaluating the vector
         * @param  delimiter  the delimiter for variables in the vector
         **/
        Vector (const KnowledgeUpdateSettings & settings =
            KnowledgeUpdateSettings (),
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
        Vector (const std::string & name, 
          KnowledgeBase & knowledge,
          int size = -1,
          bool delete_vars = true,
          const KnowledgeUpdateSettings & settings =
            KnowledgeUpdateSettings (),
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
        Vector (const std::string & name,
          Variables & knowledge,
          int size = -1,
          bool delete_vars = true,
          const KnowledgeUpdateSettings & settings =
            KnowledgeUpdateSettings (),
          const std::string & delimiter = ".");
      
        /**
         * Copy constructor
         **/
        Vector (const Vector & rhs);

        /**
         * Destructor
         **/
        virtual ~Vector ();
        
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
        void operator= (const Vector & rhs);

        /**
        * Pushes the value to the end of the array after incrementing the
        * array size.
        * @param  value       the value to place at the end of the array
        **/
        void push_back (KnowledgeRecord value);

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
        void exchange (Vector & other, bool refresh_keys = true,
          bool delete_keys = true);

        /**
         * Transfers elements from this vector to another
         * @param  other  the other vector to transfer to
         **/
        void transfer_to (Vector & other);

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
        knowledge::KnowledgeRecord operator[] (size_t index) const;
        
        /**
         * Retrieves a copy of the record from the map. Same functionality
         * as operator[]. This is provided for convenience to match other
         * containers way of converting to a record.
         * @param  index  the index of the variable entry
         * @return the value of the entry. Modifications to this will
         *         not be reflected in the context. This is a local copy.
         **/
        knowledge::KnowledgeRecord to_record (size_t index) const;
        
        /**
         * Copies the vector elements to an STL vector of Knowledge Records
         * @param  target   record values at this instance
         **/
        void copy_to (KnowledgeVector & target) const;
        
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
        int read_file (size_t index, 
                       const std::string & filename);
      
        /**
         * Read a file into the index
         * @param filename           file to read
         * @param index              index within vector
         * @param settings           settings to use when evaluating/updating
         */
        int read_file (size_t index, 
                       const std::string & filename, 
          const KnowledgeUpdateSettings & settings);
      
        /**
         * Atomically sets the value of an index to an arbitrary string.
         * @param   index     index within vector
         * @param   value     new value of the variable
         * @param   size      indicates the size of the value buffer
         * @return   0 if the value was set. -1 if null key
         **/
        int set_file (size_t index,
          const unsigned char * value, size_t size);
      
        /**
         * Atomically sets the value of an index to an arbitrary string.
         * @param   index     index within vector
         * @param   value     new value of the variable
         * @param   size      indicates the size of the value buffer
         * @param   settings  settings for applying the update
         * @return   0 if the value was set. -1 if null key
         **/
        int set_file (size_t index,
          const unsigned char * value, size_t size, 
          const KnowledgeUpdateSettings & settings);
      
        /**
         * Atomically sets the value of an index to a JPEG image
         * @param   index     index of the variable to set
         * @param   value     new value of the variable
         * @param   size      indicates the size of the value buffer
         * @return   0 if the value was set. -1 if null key
         **/
        int set_jpeg (size_t index,
          const unsigned char * value, size_t size);
              
        /**
         * Atomically sets the value of an index to a JPEG image
         * @param   index     index of the variable to set
         * @param   value     new value of the variable
         * @param   size      indicates the size of the value buffer
         * @param   settings  settings for applying the update
         * @return   0 if the value was set. -1 if null key
         **/
        int set_jpeg (size_t index,
          const unsigned char * value, size_t size, 
          const KnowledgeUpdateSettings & settings);
        
        /**
         * Sets a knowledge variable to a specified value
         *
         * @param index           index to set
         * @param value           value to set at location
         * @return                0 if successful, -1 if key is null, and
         *                        -2 if quality isn't high enough
         **/
        int set (size_t index,
          madara::knowledge::KnowledgeRecord::Integer value = 
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
          madara::knowledge::KnowledgeRecord::Integer value, 
          const KnowledgeUpdateSettings & settings);

        /**
         * Sets an index within an array to a specified value
         *
         * @param index           index to set in the variable
         * @param sub_index       index of the location in the array
         * @param value           value to set at location
         * @return                0 if successful, -1 if key is null, and
         *                        -2 if quality isn't high enough
         **/
        int set_index (size_t index,
          size_t sub_index,
          madara::knowledge::KnowledgeRecord::Integer value);
        
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
        int set_index (size_t index,
          size_t sub_index,
          madara::knowledge::KnowledgeRecord::Integer value,
          const KnowledgeUpdateSettings & settings);
        
        /**
         * Sets a knowledge variable to a specified value
         *
         * @param index           index to set
         * @param value           array of integers to set at the location
         * @param size            number of elements in the array
         * @return                0 if successful, -1 if key is null, and
         *                        -2 if quality isn't high enough
         **/
        int set (size_t index,
          const madara::knowledge::KnowledgeRecord::Integer * value,
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
        int set (size_t index,
          const madara::knowledge::KnowledgeRecord::Integer * value,
          uint32_t size,
          const KnowledgeUpdateSettings & settings);
       
        /**
         * Sets a knowledge variable to a specified value
         *
         * @param index           index to set
         * @param value           array of integers to set at the location
         * @return                0 if successful, -1 if key is null, and
         *                        -2 if quality isn't high enough
         **/
        int set (size_t index,
          const std::vector <KnowledgeRecord::Integer> & value);
       
        /**
         * Sets a knowledge variable to a specified value
         *
         * @param index           index to set
         * @param value           array of integers to set at the location
         * @param settings        settings for applying the update
         * @return                0 if successful, -1 if key is null, and
         *                        -2 if quality isn't high enough
         **/
        int set (size_t index,
          const std::vector <KnowledgeRecord::Integer> & value,
          const KnowledgeUpdateSettings & settings);
       
        /**
         * Sets a knowledge variable to a specified value
         *
         * @param index           index to set
         * @param value           value to set at location
         * @return                0 if successful, -1 if key is null, and
         *                        -2 if quality isn't high enough
         **/
        int set (size_t index, double value);

        /**
         * Sets a knowledge variable to a specified value
         *
         * @param index           index to set
         * @param value           value to set at location
         * @param settings        settings for applying the update
         * @return                0 if successful, -1 if key is null, and
         *                        -2 if quality isn't high enough
         **/
        int set (size_t index, double value, 
          const KnowledgeUpdateSettings & settings);
        
        /**
         * Sets an index within an array to a specified value
         *
         * @param index           index to set within the variable
         * @param sub_index       index of the location in the array
         * @param value           value to set at location
         * @return                0 if successful, -1 if key is null, and
         *                        -2 if quality isn't high enough
         **/
        int set_index (size_t index,
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
        int set_index (size_t index,
          size_t sub_index,
          double value,
          const KnowledgeUpdateSettings & settings);
        
        /**
         * Sets a knowledge variable to a specified value
         *
         * @param index           index to set
         * @param value           array of doubles to set at the location
         * @param size            number of elements in the array
         * @return                0 if successful, -1 if key is null, and
         *                        -2 if quality isn't high enough
         **/
        int set (size_t index,
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
        int set (size_t index,
          const double * value,
          uint32_t size,
          const KnowledgeUpdateSettings & settings);
       
        /**
         * Sets a knowledge variable to a specified value
         *
         * @param index           index to set
         * @param value           array of doubles to set at the location
         * @return                0 if successful, -1 if key is null, and
         *                        -2 if quality isn't high enough
         **/
        int set (size_t index,
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
        int set (size_t index,
          const std::vector <double> & value,
          const KnowledgeUpdateSettings & settings);
        
        /**
         * Sets a knowledge variable to a specified value
         *
         * @param index           index to set
         * @param value           value to set at location
         * @return                0 if successful, -1 if key is null, and
         *                        -2 if quality isn't high enough
         **/
        int set (size_t index, const std::string & value);
        
        /**
         * Sets a knowledge variable to a specified value
         *
         * @param index           index to set
         * @param value           value to set at location
         * @param settings        settings for applying the update
         * @return                0 if successful, -1 if key is null, and
         *                        -2 if quality isn't high enough
         **/
        int set (size_t index, const std::string & value, 
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

      /// provide the Array alias for the Vector class
      typedef  Vector   Array;
    }
  }
}




#endif // _MADARA_CONTAINERS_VECTOR_H_
