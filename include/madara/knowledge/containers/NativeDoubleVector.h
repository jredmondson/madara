
#ifndef _MADARA_NATIVE_DOUBLE_VECTOR_H_
#define _MADARA_NATIVE_DOUBLE_VECTOR_H_

#include <vector>
#include <string>
#include "madara/LockType.h"
#include "madara/knowledge/KnowledgeBase.h"
#include "madara/knowledge/ThreadSafeContext.h"
#include "madara/knowledge/KnowledgeUpdateSettings.h"
#include "BaseContainer.h"

/**
 * @file NativeDoubleVector.h
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains a C++ object that manages interactions for an
 * array of doubles
 **/

namespace madara
{
  namespace knowledge
  {
    namespace containers
    {
      /**
       * @class NativeDoubleVector
       * @brief This class stores a vector of doubles inside of KaRL
       */
      class MADARA_Export NativeDoubleVector : public BaseContainer
      {
      public:
        /// trait that describes the value type
        typedef double  type;
        
        /**
         * Default constructor
         * @param  settings   settings for evaluating the vector
         **/
        NativeDoubleVector (const KnowledgeUpdateSettings & settings =
          KnowledgeUpdateSettings ());

        /**
         * Constructor
         * @param  name       name of the vector in the knowledge base
         * @param  size       size of the vector
         * @param  knowledge  the knowledge base that will contain the vector
         * @param  settings   settings for evaluating the vector
         **/
        NativeDoubleVector (const std::string & name, 
          KnowledgeBase & knowledge,
          int size = -1,
          const KnowledgeUpdateSettings & settings =
            KnowledgeUpdateSettings ());
      
        /**
         * Constructor
         * @param  name       name of the vector in the knowledge base
         * @param  size       size of the vector
         * @param  knowledge  the knowledge base that will contain the vector
         * @param  settings   settings for evaluating the vector
         **/
        NativeDoubleVector (const std::string & name,
          Variables & knowledge,
          int size = -1,
          const KnowledgeUpdateSettings & settings =
            KnowledgeUpdateSettings ());
      
        /**
         * Copy constructor
         **/
        NativeDoubleVector (const NativeDoubleVector & rhs);

        /**
         * Destructor
         **/
        ~NativeDoubleVector ();
        
        /**
         * Mark the value as modified. The vector retains the same value
         * but will resend its value as if it had been modified.
         **/
        void modify (void);

        /**
         * Assignment operator
         * @param  rhs    value to copy
         **/
        void operator= (const NativeDoubleVector & rhs);

        /**
        * Pushes the value to the end of the array after incrementing the
        * array size.
        * @param  value       the value to place at the end of the array
        **/
        void push_back (type value);

        /**
         * Resizes the vector
         * @param   size   maximum size of the vector
         **/
        void resize (size_t size);

        /**
         * Exchanges the vector at this location with the vector at another
         * location.
         * @param  other   the other vector to exchange with
         **/
        void exchange (NativeDoubleVector & other);

        /**
         * Transfers elements from this vector to another
         * @param  other  the other vector to transfer to
         **/
        void transfer_to (NativeDoubleVector & other);
        
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
         * Retrieves a copy of the record from the map.
         * @param  index  the index of the variable entry
         * @return the value of the entry. Modifications to this will
         *         not be reflected in the context. This is a local copy.
         **/
        type operator[] (size_t index) const;

        /**
         * Sets a knowledge variable to a specified value
         *
         * @param index           index to set
         * @param value           value to set at location
         * @return                0 if successful, -1 if key is null, and
         *                        -2 if quality isn't high enough
         **/
        int set (size_t index,
          type value);

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
          const KnowledgeUpdateSettings & settings);
        
        /**
         * Reads values from a STL vector of doubles
         *
         * @param value           array of doubles to set at the location
         * @return                0 if successful, -1 if key is null, and
         *                        -2 if quality isn't high enough
         **/
        int set (
          const std::vector <type> & value);
       
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
         * Retrieves a copy of the record from the vector.
         * @param  index  the index of the variable entry
         * @return the value of the entry. Modifications to this will
         *         not be reflected in the context. This is a local copy.
         **/
        KnowledgeRecord to_record (size_t index) const;
      
        /**
         * Retrieves the entire vector as a native double array in a record 
         * @return the vector in native double array format
         **/
        KnowledgeRecord to_record (void) const;

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
         * Reference to the size field of the vector space
         **/
        VariableReference vector_;
      };

      /// provide the Array alias for the NativeDoubleVector class
      typedef  NativeDoubleVector   NativeDoubleArray;
    }
  }
}

#endif // _MADARA_NATIVE_DOUBLE_VECTOR_H_
