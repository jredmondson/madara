
#ifndef _MADARA_NATIVE_DOUBLE_VECTOR_STAGED_H_
#define _MADARA_NATIVE_DOUBLE_VECTOR_STAGED_H_

#include <vector>
#include <string>
#include "madara/LockType.h"
#include "madara/knowledge/KnowledgeBase.h"
#include "madara/knowledge/ThreadSafeContext.h"
#include "madara/knowledge/KnowledgeUpdateSettings.h"
#include "BaseContainer.h"

/**
 * @file NativeDoubleVectorStaged.h
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains a C++ object that manages interactions for an
 * array of doubles through a staged reads and writes from a knowledge base
 **/

namespace madara
{
  namespace knowledge
  {
    namespace containers
    {
      /**
       * @class NativeDoubleVectorStaged
       * @brief This class stores a vector of doubles inside of KaRL.  This
       *        container is highly optimized for using a staged value (an
       *        intermediate value between the user application and the
       *        knowledge base) for common operations and then writing that
       *        staged value to the knowledge base after operations are
       *        complete with the write method. The value is only updated
       *        from the knowledge base on construction and when read is
       *        called.
       */
      class MADARA_EXPORT NativeDoubleVectorStaged : public BaseContainer
      {
      public:
        /// trait that describes the value type
        typedef double  type;
        
        /**
         * Default constructor
         * @param  settings   settings for evaluating the vector
         **/
        NativeDoubleVectorStaged (const KnowledgeUpdateSettings & settings =
          KnowledgeUpdateSettings ());

        /**
         * Constructor
         * @param  name       name of the vector in the knowledge base
         * @param  size       size of the vector
         * @param  knowledge  the knowledge base that will contain the vector
         * @param  settings   settings for evaluating the vector
         **/
        NativeDoubleVectorStaged (const std::string & name, 
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
        NativeDoubleVectorStaged (const std::string & name,
          Variables & knowledge,
          int size = -1,
          const KnowledgeUpdateSettings & settings =
            KnowledgeUpdateSettings ());
      
        /**
         * Copy constructor
         **/
        NativeDoubleVectorStaged (const NativeDoubleVectorStaged & rhs);

        /**
         * Destructor
         **/
        virtual ~NativeDoubleVectorStaged ();
        
        /**
         * Mark the value as modified. The vector retains the same value
         * but will resend its value as if it had been modified.
         **/
        void modify (void);

        /**
         * Assignment operator
         * @param  rhs    value to copy
         **/
        void operator= (const NativeDoubleVectorStaged & rhs);

        /**
        * Assignment operator
        * @param  rhs    value to copy
        **/
        void operator= (const std::vector <type> & rhs);

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
        void exchange (NativeDoubleVectorStaged & other);

        /**
         * Transfers elements from this vector to another
         * @param  other  the other vector to transfer to
         **/
        void transfer_to (NativeDoubleVectorStaged & other);
        
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
         * @param sync       synchronize the container to the knowledge base
         **/
        void set_name (const std::string & var_name,
          KnowledgeBase & knowledge, int size = -1,
          bool sync = true);
        
        /**
         * Sets the variable name that this refers to
         * @param var_name  the name of the variable in the knowledge base
         * @param knowledge  the knowledge base the variable is housed in
         * @param size       size of the new vector (-1 to not change size)
         * @param sync       synchronize the container to the knowledge base
         **/
        void set_name (const std::string & var_name,
          Variables & knowledge, int size = -1,
          bool sync = true);
        
        /**
         * Sets the variable name that this refers to
         * @param var_name  the name of the variable in the knowledge base
         * @param knowledge  the knowledge base the variable is housed in
         * @param size       size of the new vector (-1 to not change size)
         * @param sync       synchronize the container to the knowledge base
         **/
        void set_name (const std::string & var_name,
          ThreadSafeContext & knowledge, int size = -1,
          bool sync = true);

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
         **/
        void set (size_t index,
          type value);

        /**
         * Reads values from a STL vector of doubles
         *
         * @param value           array of doubles to set at the location
         **/
        void set (
          const std::vector <type> & value);
       
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
        knowledge::KnowledgeRecord to_record (size_t index) const;
      
        /**
         * Retrieves the entire vector as a native double array in a record 
         * @return the vector in native double array format
         **/
        knowledge::KnowledgeRecord to_record (void) const;

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

        /**
        * Reads the value from the knowledge base
        **/
        void read (void);

        /**
        * Writes the value to the knowledge base
        **/
        void write (void);

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
         * Reference to the size field of the vector space
         **/
        VariableReference vector_;

        /**
         * The staged value of the vector
         **/
        KnowledgeRecord value_;

        /**
        * Tracks if value_ has changed since last read
        **/
        bool has_changed_;
      };

      /// provide the Array alias for the NativeDoubleVectorStaged class
      typedef  NativeDoubleVectorStaged   NativeDoubleArrayStaged;
    }
  }
}

#include "NativeDoubleVectorStaged.inl"

#endif // _MADARA_NATIVE_DOUBLE_VECTOR_STAGED_H_
