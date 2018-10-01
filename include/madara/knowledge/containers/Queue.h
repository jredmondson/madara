
#ifndef _MADARA_CONTAINERS_QUEUE_H_
#define _MADARA_CONTAINERS_QUEUE_H_

#include <vector>
#include <string>
#include "madara/LockType.h"
#include "madara/knowledge/KnowledgeBase.h"
#include "madara/knowledge/ThreadSafeContext.h"
#include "madara/knowledge/KnowledgeUpdateSettings.h"
#include "Vector.h"
#include "Integer.h"

/**
 * @file Queue.h
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains a high performance thread-safe queue
 **/

namespace madara
{
namespace knowledge
{
namespace containers
{
/**
 * @class Queue
 * @brief This class stores thread-safe queue within the knowledge base. The
 *        Queue allows for O(1) enqueue/dequeue time. It also allows for O(1)
 *        inspection of arbitrary elements and features asynchronous and
 *        synchronous dequeues.
 */
class MADARA_EXPORT Queue
{
public:
  /**
   * Default constructor
   **/
  Queue(const KnowledgeUpdateSettings& settings = KnowledgeUpdateSettings());

  /**
   * Constructor
   * @param  name       name of the integer in the knowledge base
   * @param  knowledge  the knowledge base that will contain the vector
   * @param  settings   settings for evaluating the vector
   **/
  Queue(const std::string& name, KnowledgeBase& knowledge,
      const KnowledgeUpdateSettings& settings = KnowledgeUpdateSettings());

  /**
   * Constructor
   * @param  name      the name of the map within the variable context
   * @param  knowledge the variable context
   * @param  settings  settings to apply by default
   **/
  Queue(const std::string& name, Variables& knowledge,
      const KnowledgeUpdateSettings& settings = KnowledgeUpdateSettings());

  /**
   * Default constructor
   * @param  name       name of the integer in the knowledge base
   * @param  knowledge  the knowledge base that will contain the vector
   * @param  size       the size of the queue
   * @param  settings   settings for evaluating the vector
   **/
  Queue(const std::string& name, KnowledgeBase& knowledge, int size,
      const KnowledgeUpdateSettings& settings = KnowledgeUpdateSettings());

  /**
   * Default constructor
   * @param  name       name of the integer in the knowledge base
   * @param  knowledge  the knowledge base that will contain the vector
   * @param  size       the size of the queue
   * @param  settings   settings for evaluating the vector
   **/
  Queue(const std::string& name, Variables& knowledge, int size,
      const KnowledgeUpdateSettings& settings = KnowledgeUpdateSettings());

  /**
   * Copy constructor
   **/
  Queue(const Queue& rhs);

  /**
   * Destructor
   **/
  virtual ~Queue() = default;

  /**
   * Assignment operator
   * @param  rhs    value to copy
   **/
  void operator=(const Queue& rhs);

  /**
   * Returns the name of the variable
   * @return name of the variable
   **/
  std::string get_name(void) const;

  /**
   * Sets the variable name that this refers to
   * @param var_name  the name of the variable in the knowledge base
   * @param knowledge  the knowledge base the variable is housed in
   **/
  void set_name(const std::string& var_name, KnowledgeBase& knowledge);

  /**
   * Sets the variable name that this refers to
   * @param var_name  the name of the variable in the knowledge base
   * @param knowledge  the knowledge base the variable is housed in
   **/
  void set_name(const std::string& var_name, Variables& knowledge);

  /**
   * Checks for equality
   * @param  value  the value to compare to
   * @return true if equal, false otherwise
   **/
  bool operator==(const Queue& value) const;

  /**
   * Checks for inequality
   * @param  value  the value to compare to
   * @return true if inequal, false otherwise
   **/
  bool operator!=(const Queue& value) const;

  /**
   * Enqueues a record to the end of the queue
   * @param  record  the value to enqueue
   * @return true if the record was enqueued and false if full
   **/
  bool enqueue(const knowledge::KnowledgeRecord& record);

  /**
   * Enqueues a new record to the end of the queue
   * @param  args  arguments to pass to KnowledgeRecord
   * @return true if the record was enqueued and false if full
   **/
  template<typename... Args>
  bool emplace(Args&&... args);

  /**
   * Dequeues a record from the front of the queue. This method
   * support both blocking and non-blocking dequeues. The default
   * operation is to wait for an element to become available in
   * the queue and only return a valid element. Setting wait
   * to false enables an asynchronous call that returns immediately
   * with either a valid record or an UNINITIALIZED record, the
   * latter of which means there was nothing in queue.
   * @return a record from the front of the queue. Will return
   *         an uncreated record if queue was empty on asynchronous
   *         call. knowledge::KnowledgeRecord::status () can be checked for
   *         UNCREATED. Can also use knowledge::KnowledgeRecord::is_valid to
   *         check for valid data on return.
   **/
  knowledge::KnowledgeRecord dequeue(bool wait = true);

  /**
   * Retrieves a record at a position in the queue
   * @param  position  the position of the record in the queue
   * @return  the record at the position in the queue
   **/
  knowledge::KnowledgeRecord inspect(size_t position);

  /**
   * Clears the queue
   **/
  void clear(void);

  /**
   * Returns the number of records in the queue
   * @return the number of records in the queue
   **/
  size_t count(void);

  /**
   * Returns the maximum size of the queue
   * @return the size of the queue
   **/
  size_t size(void);

  /**
   * Sets the update settings for the variable
   * @param  settings  the new settings to use
   * @return the old update settings
   **/
  KnowledgeUpdateSettings set_settings(const KnowledgeUpdateSettings& settings);

  /**
   * Sets the quality of writing to the counter variables
   *
   * @param quality         quality of writing to this location
   * @param settings        settings for referring to knowledge variables
   **/
  void set_quality(uint32_t quality,
      const KnowledgeReferenceSettings& settings = KnowledgeReferenceSettings(
          false));

  /**
   * Resizes the queue
   * @param  size      the size of the queue. -1 means use the size that
   *                   exists in the knowledge base already.
   **/
  void resize(int size = -1);

private:
  /**
   * Increments the base by the value, using size as a boundary
   * @param  base   the base value to increment
   * @param  value  the value to increment base by
   * @return the resulting index
   **/
  inline knowledge::KnowledgeRecord::Integer increment(
      knowledge::KnowledgeRecord::Integer base,
      knowledge::KnowledgeRecord::Integer value)
  {
    return (base + value) % queue_.size();
  }

  /// guard for access and changes

  /**
   * Mutex for local changes
   **/
  mutable MADARA_LOCK_TYPE mutex_;

  /**
   * Variable context that we are modifying
   **/
  mutable ThreadSafeContext* context_;

  /**
   * Prefix of variable
   **/
  std::string name_;

  /**
   * Count of elements in queue
   **/
  Integer count_;

  /**
   * Head of the queue
   **/
  Integer head_;

  /**
   * Tail of the queue
   **/
  Integer tail_;

  /**
   * Underlying array of records
   **/
  Vector queue_;

  /**
   * Settings for modifications
   **/
  KnowledgeUpdateSettings settings_;
};
}
}
}

#include "Queue.inl"

#endif  // _MADARA_CONTAINERS_QUEUE_H_
