
#ifndef _MADARA_KNOWLEDGE_CONTAINERS_CIRCULARBUFFER_H_
#define _MADARA_KNOWLEDGE_CONTAINERS_CIRCULARBUFFER_H_

#include <vector>
#include <string>
#include "madara/LockType.h"
#include "madara/knowledge/KnowledgeBase.h"
#include "madara/knowledge/ThreadSafeContext.h"
#include "madara/knowledge/KnowledgeUpdateSettings.h"
#include "Vector.h"
#include "Integer.h"

/**
 * @file CircularBuffer.h
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains a high performance thread-safe CircularBuffer
 **/

namespace madara
{
namespace knowledge
{
namespace containers
{
/**
 * @class CircularBuffer
 * @brief This container stores a thread-safe circular buffer with O(1)
 * access times.
 */
class MADARA_EXPORT CircularBuffer
{
public:
  /**
   * Default constructor
   **/
  CircularBuffer(
      const KnowledgeUpdateSettings& settings = KnowledgeUpdateSettings());

  /**
   * Constructor
   * @param  name       name of the integer in the knowledge base
   * @param  knowledge  the knowledge base that will contain the vector
   * @param  settings   settings for evaluating the vector
   * @throw exceptions::NameException  bad name ("")
   **/
  CircularBuffer(const std::string& name, KnowledgeBase& knowledge,
      const KnowledgeUpdateSettings& settings = KnowledgeUpdateSettings());

  /**
   * Constructor
   * @param  name      the name of the map within the variable context
   * @param  knowledge the variable context
   * @param  settings  settings to apply by default
   * @throw exceptions::NameException  bad name ("")
   **/
  CircularBuffer(const std::string& name, Variables& knowledge,
      const KnowledgeUpdateSettings& settings = KnowledgeUpdateSettings());

  /**
   * Default constructor
   * @param  name       name of the integer in the knowledge base
   * @param  knowledge  the knowledge base that will contain the vector
   * @param  size       the size of the CircularBuffer
   * @param  settings   settings for evaluating the vector
   * @throw exceptions::NameException  bad name ("")
   **/
  CircularBuffer(const std::string& name, KnowledgeBase& knowledge, int size,
      const KnowledgeUpdateSettings& settings = KnowledgeUpdateSettings());

  /**
   * Default constructor
   * @param  name       name of the integer in the knowledge base
   * @param  knowledge  the knowledge base that will contain the vector
   * @param  size       the size of the CircularBuffer
   * @param  settings   settings for evaluating the vector
   * @throw exceptions::NameException  bad name ("")
   **/
  CircularBuffer(const std::string& name, Variables& knowledge, int size,
      const KnowledgeUpdateSettings& settings = KnowledgeUpdateSettings());

  /**
   * Destructor
   **/
  virtual ~CircularBuffer() = default;

  /**
   * Returns the name of the variable
   * @return name of the variable
   **/
  std::string get_name(void) const;

  /**
   * Sets the variable name that this refers to
   * @param name  the name of the variable in the knowledge base
   * @param knowledge  the knowledge base the variable is housed in
   * @throw exceptions::NameException  bad name ("")
   **/
  void set_name(const std::string& name, KnowledgeBase& knowledge);

  /**
   * Sets the variable name that this refers to
   * @param name  the name of the variable in the knowledge base
   * @param knowledge  the knowledge base the variable is housed in
   * @throw exceptions::NameException  bad name ("")
   **/
  void set_name(const std::string& name, Variables& knowledge);

  /**
   * Checks for equality
   * @param  value  the value to compare to
   * @return true if equal, false otherwise
   **/
  bool operator==(const CircularBuffer& value) const;

  /**
   * Checks for inequality
   * @param  value  the value to compare to
   * @return true if inequal, false otherwise
   **/
  bool operator!=(const CircularBuffer& value) const;

  /**
   * Adds a record to the end of the CircularBuffer
   * @param  record  the value to add
   * @throw exceptions::IndexException  if index is out of range/invalid
   **/
  void add(const knowledge::KnowledgeRecord& record);

  /**
   * Adds a value to the end of the CircularBuffer
   * @param  value  the value to add
   * @throw exceptions::IndexException  if index is out of range/invalid
   **/
  template<typename T>
  void add(const T& value);

  /**
   * Adds a list of records to the end of the CircularBuffer
   * @param  records  the values to add
   * @throw exceptions::IndexException  if index is out of range/invalid
   **/
  void add(const std::vector<KnowledgeRecord>& records);

  /**
   * Adds a list of user values to the end of the CircularBuffer
   * @param  values  the values to add
   * @throw exceptions::IndexException  if index is out of range/invalid
   **/
  template<typename T>
  void add(const std::vector<T>& values);

  /**
   * Gets the most recently added record
   * @return the last added record
   * @throw exceptions::IndexException  if index is out of range/invalid
   **/
  knowledge::KnowledgeRecord get(void) const;

  /**
   * Gets the most recently added user value
   * @return the last added user value
   * @throw exceptions::IndexException  if index is out of range/invalid
   **/
  template<typename T>
  void get(T& value) const;

  /**
   * Gets the most recently added records up to a specified count
   * @param  count   the maximum number of records to return
   * @return the last added records
   * @throw exceptions::IndexException  if index is unreachable
   **/
  std::vector<KnowledgeRecord> get_latest(size_t count) const;

  /**
   * Gets the most recently added values up to a specified count
   * @param  count   the maximum number of records to return
   * @param  values  the resulting values
   * @throw exceptions::IndexException  if index is unreachable
   **/
  template<typename T>
  void get_latest(size_t count, std::vector<T>& values) const;

  /**
   * Gets the oldest added records up to a specified count
   * @param  count   the maximum number of records to return
   * @return the added records
   * @throw exceptions::IndexException  if index is unreachable
   **/
  std::vector<KnowledgeRecord> get_earliest(size_t count) const;

  /**
   * Gets the oldest added values up to a specified count
   * @param  count   the maximum number of records to return
   * @param  values  the added values
   * @throw exceptions::IndexException  if index is unreachable
   **/
  template<typename T>
  void get_earliest(size_t count, std::vector<T>& values) const;

  /**
   * Retrieves a record at a position relative to last added
   * @param  position  the relative position of the requested record
   *                   from the latest added record. Can be negative
   * @return  the record at the position in the CircularBuffer
   * @throw exceptions::ContextException if name or context haven't
   *                      been set appropriately
   * @throw exceptions::IndexException  if index is out of range/invalid
   **/
  knowledge::KnowledgeRecord inspect(KnowledgeRecord::Integer position) const;

  /**
   * Retrieves a record at a position relative to last added
   * @param  position  the relative position of the requested record
   *                   from the latest added record. Can be negative
   * @param  value     the value at the position in the CircularBuffer
   * @throw exceptions::ContextException if name or context haven't
   *                      been set appropriately
   * @throw exceptions::IndexException  if index is out of range/invalid
   **/
  template<typename T>
  void inspect(KnowledgeRecord::Integer position, T& value) const;

  /**
   * Retrieves a vector of records at a position relative to last added
   * @param  position  the relative position of the requested record
   *                   from the latest added record. Can be negative
   * @param  count   the maximum number of records to return
   * @return  the record at the position in the CircularBufferConsumer
   * @throw exceptions::ContextException if name or context haven't
   *                      been set appropriately
   * @throw exceptions::IndexException  if index is out of range/invalid
   **/
  std::vector<KnowledgeRecord> inspect(
      KnowledgeRecord::Integer position, size_t count) const;

  /**
   * Retrieves a vector of records at a position relative to last added
   * @param  position  the relative position of the requested record
   *                   from the latest added record. Can be negative
   * @param  count   the maximum number of records to return
   * @param  values  the values at the position in the CircularBufferConsumer
   * @throw exceptions::ContextException if name or context haven't
   *                      been set appropriately
   * @throw exceptions::IndexException  if index is out of range/invalid
   **/
  template<typename T>
  void inspect(KnowledgeRecord::Integer position, size_t count,
      std::vector<T>& values) const;

  /**
   * Clears the CircularBuffer
   * @throw exceptions::ContextException  if name or context haven't
   *                      been set appropriately
   **/
  void clear(void);

  /**
   * Returns the number of records in the CircularBuffer
   * @return the number of records in the CircularBuffer
   * @throw exceptions::ContextException  if name or context haven't
   *                      been set appropriately
   **/
  size_t count(void) const;

  /**
   * Returns the maximum size of the CircularBuffer
   * @return the size of the CircularBuffer
   **/
  size_t size(void) const;

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
   * Resizes the CircularBuffer
   * @param  size      the size of the CircularBuffer. -1 means use the size
   *that exists in the knowledge base already.
   **/
  void resize(int size = -1);

  /**
   * Sets the index into the circular buffer to an arbitrary position
   * @param  index   the new index to use
   **/
  void set_index(KnowledgeRecord::Integer index);

private:
  /**
   * Increments the base by the value, using size as a boundary
   * @param  base   the base value to increment
   * @param  value  the value to increment base by
   * @return the resulting index
   **/
  KnowledgeRecord::Integer increment(
      KnowledgeRecord::Integer base, KnowledgeRecord::Integer value) const;

  /// guard for access and changes

  /**
   * Variable context that we are modifying
   **/
  mutable ThreadSafeContext* context_;

  /**
   * Prefix of variable
   **/
  std::string name_;

  /**
   * Index for latest item in circular buffer
   **/
  Integer index_;

  /**
   * Underlying array of records
   **/
  Vector buffer_;

  /**
   * Reference to the size field of the vector space
   **/
  VariableReference size_ref_;

  /**
   * Settings for modifications
   **/
  KnowledgeUpdateSettings settings_;

  // Call to throw if preconditions aren't met
  void check_name(const char* func) const;
  void check_context(const char* func) const;
  void check_all(const char* func) const;
};
}
}
}

#include "CircularBuffer.inl"

#endif  // _MADARA_KNOWLEDGE_CONTAINERS_CIRCULARBUFFER_H_
