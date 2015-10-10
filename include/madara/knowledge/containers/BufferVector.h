
#ifndef _MADARA_CONTAINERS_BUFFER_VECTOR_H_
#define _MADARA_CONTAINERS_BUFFER_VECTOR_H_

#include <vector>
#include <string>
#include "madara/LockType.h"
#include "madara/knowledge/KnowledgeBase.h"
#include "madara/knowledge/ThreadSafeContext.h"
#include "madara/knowledge/KnowledgeUpdateSettings.h"
#include "BaseContainer.h"

/**
 * @file BufferVector.h
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains a C++ object that manages interactions for an
 * array of character buffers
 **/

namespace madara
{
  namespace knowledge
  {
    namespace containers
    {
      /**
       * @class BufferVector
       * @brief This class stores a vector of character buffers
       */
      class MADARA_Export BufferVector : public BaseContainer
      {
      public:
        /**
         * Default constructor
         * @param  settings   settings for evaluating the vector
         * @param  delimiter  the delimiter for variables in the vector
         **/
        BufferVector (const KnowledgeUpdateSettings & settings =
          KnowledgeUpdateSettings (),
          const std::string & delimiter = ".");

        /**
         * Constructor
         * @param  name       name of the vector in the knowledge base
         * @param  size       size of the vector
         * @param  knowledge  the knowledge base that will contain the vector
         * @param  delete_vars delete indices outside of the specified range
         * @param  settings   settings for evaluating the vector
         * @param  delimiter  the delimiter for variables in the vector
         **/
        BufferVector (const std::string & name, 
                KnowledgeBase & knowledge,
                int size = -1,
                bool delete_vars = true,
                const KnowledgeUpdateSettings & settings =
                  KnowledgeUpdateSettings (),
                const std::string & delimiter = ".");
      
        /**
         * Constructor
         * @param  name       name of the vector in the knowledge base
         * @param  size       size of the vector (-1 to check knowledge base for size)
         * @param  knowledge  the knowledge base that will contain the vector
         * @param  delete_vars delete indices outside of the specified range
         * @param  settings   settings for evaluating the vector
         * @param  delimiter  the delimiter for variables in the vector
         **/
        BufferVector (const std::string & name,
                Variables & knowledge,
                int size = -1,
                bool delete_vars = true,
                const KnowledgeUpdateSettings & settings =
                  KnowledgeUpdateSettings (),
                const std::string & delimiter = ".");
      
        /**
         * Copy constructor
         **/
        BufferVector (const BufferVector & rhs);

        /**
         * Destructor
         **/
        virtual ~BufferVector ();
    
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
        void operator= (const BufferVector & rhs);

        /**
        * Pushes the value to the end of the array after incrementing the
        * array size.
        * @param  value       the value to place at the end of the array
        * @param  size        the size of the buffer at value
        **/
        void push_back (const unsigned char * value, size_t size);

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
        void exchange (BufferVector & other, bool refresh_keys = true,
          bool delete_keys = true);

        /**
         * Transfers elements from this vector to another
         * @param  other  the other vector to transfer to
         **/
        void transfer_to (BufferVector & other);
        
        /**
         * Copies the vector elements to an STL vector of Knowledge Records
         * @param  target   record values at this instance
         **/
        void copy_to (KnowledgeVector & target) const;
        
        /**
         * Returns the size of the vector
         * @return size of the vector
         **/
        size_t size (void) const;
      
        /**
         * Sets the variable name that this refers to
         * @param var_name  the name of the variable in the knowledge base
         * @param knowledge  the knowledge base the variable is housed in
         * @param size       size of the vector. -1 to check for size.
         **/
        void set_name (const std::string & var_name,
          KnowledgeBase & knowledge, int size = -1);
        
        /**
         * Sets the variable name that this refers to
         * @param var_name  the name of the variable in the knowledge base
         * @param knowledge  the knowledge base the variable is housed in
         * @param size       size of the vector. -1 to check for size.
         **/
        void set_name (const std::string & var_name,
          Variables & knowledge, int size = -1);
        
        /**
         * Sets the variable name that this refers to
         * @param var_name  the name of the variable in the knowledge base
         * @param knowledge  the knowledge base the variable is housed in
         * @param size       size of the vector. -1 to check for size.
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
         * Checks to see if the index has ever been assigned a value
         * @param  index  the index of the variable entry
         * @return true if the record has been set to a value. False if
         *         uninitialized
         **/
        bool exists (size_t index) const;
      
        /**
         * Read a file into the index using stored settings
         * @param filename           file to read
         * @param index              index within vector
         */
        int read_file (size_t index, 
                       const std::string & filename);
      
        /**
         * Read a file into the index using temporary settings
         * @param filename           file to read
         * @param index              index within vector
         * @param settings           settings to use when evaluating/updating
         */
        int read_file (size_t index, 
                       const std::string & filename, 
          const KnowledgeUpdateSettings & settings);
      
        /**
         * Atomically sets the value of an index to a provided record
         * @param   index     index within vector
         * @param   value     new value of the variable
         * @return   0 if the value was set. -1 if null key
         **/
        int set (size_t index, const knowledge::KnowledgeRecord & value);
      
        /**
         * Atomically sets the value of an index to an arbitrary string
         * using stored settings
         * @param   index     index within vector
         * @param   value     new value of the variable
         * @param   size      indicates the size of the value buffer
         * @return   0 if the value was set. -1 if null key
         **/
        int set_file (size_t index,
          const unsigned char * value, size_t size);
      
        /**
         * Atomically sets the value of an index to an arbitrary string
         * using temporary settings
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
        * @return info in format <container>: <name>< = value, if appropriate>
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
        * @return info in format <container>: <name>< = value, if appropriate>
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

      /// provide the Array alias for the BufferVector class
      typedef  BufferVector   BufferArray;
    }
  }
}

#endif // _MADARA_CONTAINERS_BUFFER_VECTOR_H_