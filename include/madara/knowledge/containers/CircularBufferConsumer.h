
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
         * Destructor
         **/
        virtual ~CircularBufferConsumer () = default;

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
         * Consumes the record at the local index (not the producer index)
         * @return the last added record. exists() will return false if the
         *         record is invalid
         * @throw exceptions::IndexException  buffer has zero size
         **/
        KnowledgeRecord consume (void) const;
        
        /**
         * Consumes the record at the local index (not the producer index)
         * @return the last added record. exists() will return false if the
         *         record is invalid
         * @param  dropped the number of dropped packets. Drops can
         *                 occur when the producer produces faster than the
         *                 consumer can consume. This value is essentially
         *                 index_ - local_index - size ().
         * @throw exceptions::IndexException  buffer has zero size
         **/
        KnowledgeRecord consume (size_t & dropped) const;
        
        /**
         * Consumes the record at the local index (not the producer index)
         * @param value  the last added value.
         * @throw exceptions::IndexException  if no data to consume
         **/
        template <typename T>
        void consume (T & value) const;
        
        /**
         * Consumes the record at the local index (not the producer index)
         * @param value   the last added value.
         * @param dropped the number of dropped packets. Drops can
         *                occur when the producer produces faster than the
         *                consumer can consume. This value is essentially
         *                index_ - local_index - size ().
         * @throw exceptions::IndexException  if no data to consume
         **/
        template <typename T>
        void consume (T & value, size_t & dropped) const;
        
        /**
         * Consumes the record at the local index (not the producer index)
         * @param  count   the maximum number of records to return
         * @return the last added records
         * @throw exceptions::IndexException  if index is unreachable
         **/
        std::vector <KnowledgeRecord> consume_latest (size_t count) const;
        
        /**
         * Consumes latest records added by the producer and updates the
         * local index to the producer index
         * @param  count   the maximum number of records to return
         * @param  dropped the number of dropped packets. Drops can
         *                 occur when the producer produces faster than the
         *                 consumer can consume. This value is essentially
         *                 index_ - local_index - size ().
         * @return the last added records
         * @throw exceptions::IndexException  if index is unreachable
         **/
        std::vector <KnowledgeRecord> consume_latest (
          size_t count, size_t & dropped) const;
        
        /**
         * Consumes latest records added by the producer and updates the
         * local index to the producer index
         * @param  count   the maximum number of records to return
         * @param  values  the last added records
         * @throw exceptions::IndexException  if index is unreachable
         **/
        template <typename T>
        void consume_latest (size_t count,
          std::vector <T> & values) const;
        
        /**
         * Consumes latest records added by the producer and updates the
         * local index to the producer index
         * @param  count   the maximum number of records to return
         * @param  values  the last added records
         * @param  dropped the number of dropped packets. Drops can
         *                 occur when the producer produces faster than the
         *                 consumer can consume. This value is essentially
         *                 index_ - local_index - size ().
         * @throw exceptions::IndexException  if index is unreachable
         **/
        template <typename T>
        void consume_latest (size_t count,
          std::vector <T> & values, size_t & dropped) const;
        
        /**
         * Consumes earliest records from the local index in the buffer
         * @param  count   the maximum number of records to return
         * @return the last added records
         * @throw exceptions::IndexException  if index is unreachable
         **/
        std::vector <KnowledgeRecord> consume_earliest (size_t count) const;
        
        /**
         * Consumes earliest records from the local index in the buffer
         * @param  count   the maximum number of records to return
         * @param  dropped the number of dropped packets. Drops can
         *                 occur when the producer produces faster than the
         *                 consumer can consume. This value is essentially
         *                 index_ - local_index - size ().
         * @return the last added records
         * @throw exceptions::IndexException  if index is unreachable
         **/
        std::vector <KnowledgeRecord> consume_earliest (
          size_t count, size_t & dropped) const;
        
        /**
         * Consumes earliest records from the local index in the buffer
         * @param  count   the maximum number of records to return
         * @param  values  the last added records
         * @throw exceptions::IndexException  if index is unreachable
         **/
        template <typename T>
        void consume_earliest (size_t count,
          std::vector <T> & values) const;
       
        /**
         * Consumes earliest records from the local index in the buffer
         * @param  count   the maximum number of records to return
         * @param  values  the last added records
         * @param  dropped the number of dropped packets. Drops can
         *                 occur when the producer produces faster than the
         *                 consumer can consume. This value is essentially
         *                 index_ - local_index - size ().
         * @throw exceptions::IndexException  if index is unreachable
         **/
        template <typename T>
        void consume_earliest (size_t count,
          std::vector <T> & values, size_t & dropped) const;
         
        /**
         * Returns the number of known drops since last consume
         * @return the number of drops
         **/
        size_t get_dropped (void) const;

        /**
         * Retrieves a record at a position relative to local index
         * @param  position  the relative position of the requested record
         *                   from the latest added record. Can be negative
         * @return  the record at the position in the CircularBufferConsumer
         * @throw exceptions::ContextException if name or context haven't
         *                      been set appropriately
         * @throw exceptions::IndexException  if index is out of range/invalid
         **/
        knowledge::KnowledgeRecord inspect (KnowledgeRecord::Integer position) const;

        /**
         * Retrieves a record at a position relative to local index
         * @param  position  the relative position of the requested record
         *                   from the latest added record. Can be negative
         * @param  value    the record at the position in the CircularBufferConsumer
         * @throw exceptions::ContextException if name or context haven't
         *                      been set appropriately
         * @throw exceptions::IndexException  if index is out of range/invalid
         **/
        template <typename T>
        void inspect (KnowledgeRecord::Integer position, T & value) const;

        /**
         * Retrieves a vector of records at a position relative to local index
         * @param  position  the relative position of the requested record
         *                   from the latest added record. Can be negative
         * @param  count   the maximum number of records to return
         * @return  the values at the position in the CircularBufferConsumer
         * @throw exceptions::ContextException if name or context haven't
         *                      been set appropriately
         * @throw exceptions::IndexException  if index is out of range/invalid
         **/
        std::vector <KnowledgeRecord> inspect (KnowledgeRecord::Integer position,
          size_t count) const;

        /**
         * Retrieves a vector of records at a position relative to local index
         * @param  position  the relative position of the requested record
         *                   from the latest added record. Can be negative
         * @param  count   the maximum number of records to return
         * @param  values  the values at the position in the CircularBufferConsumer
         * @throw exceptions::ContextException if name or context haven't
         *                      been set appropriately
         * @throw exceptions::IndexException  if index is out of range/invalid
         **/
        template <typename T>
        void inspect (KnowledgeRecord::Integer position,
          size_t count, std::vector <T> & values) const;

        /**
         * Peeks at the most recently added record. This does not use or modify
         * the local index. It instead references what the producer most recently
         * added.
         * @return the last added record. exists() will return false if the
         *         record is invalid
         * @throw exceptions::IndexException  if index is out of range/invalid
         **/
        knowledge::KnowledgeRecord peek_latest (void) const;
        
        /**
         * Peeks at the most recently added value. This does not use or modify
         * the local index. It instead references what the producer most recently
         * added.
         * @param value the last added value. exists() will return false if the
         *              record is invalid
         * @throw exceptions::IndexException  if index is out of range/invalid
         **/
        template <typename T>
        void peek_latest (T & value) const;
        
        /**
         * Peeks at the most recently added records. This does not use or modify
         * the local index. It instead references what the producer most recently
         * added.
         * @param  count  the maximum number of items to look for
         * @return the last added records up to a certain count
         * @throw exceptions::IndexException  if index is out of range/invalid
         **/
        std::vector <KnowledgeRecord> peek_latest (size_t count) const;
        
        /**
         * Peeks at the most recently added records. This does not use or modify
         * the local index. It instead references what the producer most recently
         * added.
         * @param  count  the maximum number of items to look for
         * @param  values the last added records up to a certain count
         * @throw exceptions::IndexException  if index is out of range/invalid
         **/
        template <typename T>
        void peek_latest (size_t count,
          std::vector <T> & values) const;
        
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
         * Resizes the buffer size to the producer's buffer size
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

        // Call to throw if preconditions aren't met
        void check_name (const char * func) const;
        void check_context (const char * func) const;
        void check_all (const char * func) const;
      };
    }
  }
}


#include "CircularBufferConsumer.inl"

#endif // _MADARA_KNOWLEDGE_CONTAINERS_CIRCULARBUFFERCONSUMER_H_
