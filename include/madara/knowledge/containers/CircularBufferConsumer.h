
#ifndef _MADARA_KNOWLEDGE_CONTAINERS_CIRCULARBUFFERCONSUMER_H_
#define _MADARA_KNOWLEDGE_CONTAINERS_CIRCULARBUFFERCONSUMER_H_

#include <vector>
#include <string>
#include "madara/LockType.h"
#include "madara/knowledge/KnowledgeBase.h"
#include "madara/knowledge/ThreadSafeContext.h"
#include "madara/knowledge/KnowledgeUpdateSettings.h"
#include "Vector.h"
#include "Integer.h"

/**
 * @file CircularBufferConsumer.h
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains a high performance thread-safe CircularBufferConsumer
 **/

namespace madara
{
  namespace knowledge
  {
    namespace containers
    {
      /**
       * @class CircularBufferConsumer
       * @brief This container stores a thread-safe, personalized consumer for
       * CircularBuffer instances. Unlike functions in CircularBuffer, almost
       * all operations in CircularBufferConsumer use a local index into the
       * CircularBuffer and encourage one-time access of buffer elements.
       */
      class MADARA_EXPORT CircularBufferConsumer
      {
      public:
        /**
         * Default constructor
         **/
        CircularBufferConsumer ();
      
        /**
         * Constructor
         * @param  name       name of the integer in the knowledge base
         * @param  knowledge  the knowledge base that will contain the vector
         * @param  settings   settings for evaluating the vector
         * @throw exceptions::NameException  bad name ("")
         **/
        CircularBufferConsumer (const std::string & name,
                KnowledgeBase & knowledge);
      
        /**
         * Constructor
         * @param  name      the name of the map within the variable context
         * @param  knowledge the variable context
         * @param  settings  settings to apply by default
         * @throw exceptions::NameException  bad name ("")
         **/
        CircularBufferConsumer (const std::string & name,
                Variables & knowledge);
      
        /**
         * Copy constructor
         **/
        CircularBufferConsumer (const CircularBufferConsumer & rhs);

        /**
         * Destructor
         **/
        virtual ~CircularBufferConsumer () = default;
        
        /**
         * Assignment operator
         * @param  rhs    value to copy
         **/
        void operator= (const CircularBufferConsumer & rhs);

        /**
         * Returns the name of the variable
         * @return name of the variable
         **/
        std::string get_name (void) const;
        
        /**
         * Sets the variable name that this refers to
         * @param name  the name of the variable in the knowledge base
         * @param knowledge  the knowledge base the variable is housed in
         * @throw exceptions::NameException  bad name ("")
         **/
        void set_name (const std::string & name,
          KnowledgeBase & knowledge);
        
        /**
         * Sets the variable name that this refers to
         * @param name  the name of the variable in the knowledge base
         * @param knowledge  the knowledge base the variable is housed in
         * @throw exceptions::NameException  bad name ("")
         **/
        void set_name (const std::string & name,
          Variables & knowledge);
        
        /**
         * Checks for equality
         * @param  value  the value to compare to
         * @return true if equal, false otherwise
         **/
        bool operator== (const CircularBufferConsumer & value) const;
        
        /**
         * Checks for inequality
         * @param  value  the value to compare to
         * @return true if inequal, false otherwise
         **/
        bool operator!= (const CircularBufferConsumer & value) const;
        
        /**
         * Gets the most recently added record
         * @return the last added record
         * @throw exceptions::IndexException  if index is out of range/invalid
         **/
        knowledge::KnowledgeRecord get (void) const;
        
        /**
         * Gets the most recently added records up to a specified count
         * @param  count   the maximum number of records to return
         * @return the last added records
         * @throw exceptions::IndexException  if index is unreachable
         **/
        std::vector <KnowledgeRecord> get_latest (size_t count) const;
        
        /**
         * Gets the most recently added records up to a specified count
         * @param  count   the maximum number of records to return
         * @return the last added records
         * @throw exceptions::IndexException  if index is unreachable
         **/
        std::vector <KnowledgeRecord> get_earliest (size_t count) const;
        
        /**
         * Retrieves a record at a position relative to last added
         * @param  position  the relative position of the requested record
         *                   from the latest added record. Can be negative
         * @return  the record at the position in the CircularBufferConsumer
         * @throw exceptions::ContextException if name or context haven't
         *                      been set appropriately
         * @throw exceptions::IndexException  if index is out of range/invalid
         **/
        knowledge::KnowledgeRecord inspect (KnowledgeRecord::Integer position) const;

        /**
         * Returns the number of records remaining that have not been consumed
         * with get_latest, get, or get_earliest
         * @return the number of records remaining for get*
         * @throw exceptions::ContextException  if name or context haven't
         *                      been set appropriately
         **/
        size_t remaining (void) const;

        /**
         * Returns the number of records in the CircularBufferConsumer
         * @return the number of records in the CircularBufferConsumer
         * @throw exceptions::ContextException  if name or context haven't
         *                      been set appropriately
         **/
        size_t count (void) const;

        /**
         * Returns the maximum size of the CircularBufferConsumer
         * @return the size of the CircularBufferConsumer
         **/
        size_t size (void) const;

        /**
         * Resizes the CircularBufferConsumer
         * @param  size      the size of the CircularBufferConsumer. -1 means use the size that
         *                   exists in the knowledge base already.
         **/
        void resize (void);

        /**
         * Sets the local index to an arbitrary position
         * @param  index   the new index to use
         **/
        void set_index (KnowledgeRecord::Integer index);

        /**
         * Sets the local index to the current buffer index
         **/
        void resync (void);

      private:
        /**
         * Increments the base by the value, using size as a boundary
         * @param  base   the base value to increment
         * @param  value  the value to increment base by
         * @return the resulting index
         **/
        KnowledgeRecord::Integer increment (
          KnowledgeRecord::Integer base, KnowledgeRecord::Integer value) const;


        /// guard for access and changes
      
        /**
         * Variable context that we are modifying
         **/
        mutable ThreadSafeContext * context_;

        /**
         * Prefix of variable
         **/
        std::string name_;

        /**
         * Index for latest item in circular buffer
         **/
        Integer index_;

        /**
         * Index for latest item read by  
         **/
        mutable KnowledgeRecord::Integer local_index_;

        /**
         * Underlying array of records
         **/
        Vector buffer_;
      };
    }
  }
}


#include "CircularBufferConsumer.inl"

#endif // _MADARA_KNOWLEDGE_CONTAINERS_CIRCULARBUFFERCONSUMER_H_
