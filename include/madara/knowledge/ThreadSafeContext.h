#ifndef MADARA_THREADSAFECONTEXT_H
#define MADARA_THREADSAFECONTEXT_H

/**
 * @file ThreadSafeContext.h
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains a thread safe context for state information
 */

#include "madara/knowledge/FileHeader.h"
#include "madara/logger/Logger.h"

#include <string>
#include <map>
#include <memory>
#include <fstream>
#include "madara/utility/IntTypes.h"

#include "madara/MadaraExport.h"
#include "madara/LockType.h"
#include "madara/knowledge/KnowledgeRecord.h"
#include "madara/knowledge/KnowledgeRequirements.h"
#include "madara/knowledge/VariableReference.h"
#include "madara/knowledge/FunctionMap.h"
#include "madara/knowledge/KnowledgeUpdateSettings.h"
#include "madara/knowledge/KnowledgeReferenceSettings.h"
#include "madara/knowledge/CompiledExpression.h"
#include "madara/knowledge/CheckpointSettings.h"
#include "madara/knowledge/BaseStreamer.h"
#include "madara/transport/MessageHeader.h"

#ifdef _MADARA_JAVA_
#include "madara_jni.h"
#endif

#ifdef _MADARA_PYTHON_CALLBACKS_
#include "madara/Boost.h"
#include "boost/python/object.hpp"
#endif

namespace madara
{
namespace expression
{
class Interpreter;
class CompositeArrayReference;
class VariableNode;
}

namespace knowledge
{
namespace rcw
{
class BaseTracker;
}

/**
 * Typedef for set of copyable keys. @see copy. We use map instead
 * of set so we are not wasting significant memory/time with copying
 * the key to the value (a STL set uses the type as both the key
 * and value).
 **/
typedef std::map<std::string, bool> CopySet;

/**
 * Typedef for vector of prefixes to use for a generic task. @see copy.
 **/
typedef std::vector<std::string> PrefixVector;

/// forward declare for friendship
class KnowledgeBaseImpl;

/**
 * @class ThreadSafeContext
 * @brief This class stores variables and their values for use by any entity
 *        needing state information in a thread safe way
 */
class MADARA_EXPORT ThreadSafeContext
{
public:
  friend class KnowledgeBaseImpl;
  friend class expression::CompositeArrayReference;
  friend class expression::VariableNode;
  friend class rcw::BaseTracker;

  /**
   * Constructor.
   **/
  ThreadSafeContext();

  /**
   * Destructor.
   **/
  ~ThreadSafeContext(void);

  /**
   * Atomically returns the current value of a variable.
   * @param   key       unique identifier of the variable
   * @param   settings  the settings for referring to variables
   * @return         the madara::knowledge::KnowledgeRecord::Integer value for
   *the variable
   **/
  madara::knowledge::KnowledgeRecord get(
      const std::string& key, const KnowledgeReferenceSettings& settings =
                                  KnowledgeReferenceSettings()) const;

  /**
   * Atomically returns the current value of a variable.
   * @param   variable  reference to a variable (@see get_ref)
   * @param   settings  the settings for referring to variables
   * @return         the madara::knowledge::KnowledgeRecord::Integer value for
   *the variable
   **/
  madara::knowledge::KnowledgeRecord get(const VariableReference& variable,
      const KnowledgeReferenceSettings& settings =
          KnowledgeReferenceSettings()) const;

  /**
   * Atomically returns the underlying value of a variable, including
   * history. If record has no history, equivalent to get.
   * @param   key       unique identifier of the variable
   * @param   settings  the settings for referring to variables
   * @return         the madara::knowledge::KnowledgeRecord::Integer value for
   *the variable
   **/
  madara::knowledge::KnowledgeRecord get_actual(
      const std::string& key, const KnowledgeReferenceSettings& settings =
                                  KnowledgeReferenceSettings()) const;

  /**
   * Atomically returns the underlying value of a variable, including
   * history. If record has no history, equivalent to get.
   * @param   variable  reference to a variable (@see get_ref)
   * @param   settings  the settings for referring to variables
   * @return         the madara::knowledge::KnowledgeRecord::Integer value for
   *the variable
   **/
  madara::knowledge::KnowledgeRecord get_actual(
      const VariableReference& variable,
      const KnowledgeReferenceSettings& settings =
          KnowledgeReferenceSettings()) const;

private:
  /**
   * Atomically returns a reference to the variable
   * @param   key       unique identifier of the variable
   * @param   settings  the settings for referring to variables
   * @return  pointer to record, nullptr if none exists
   **/
  madara::knowledge::KnowledgeRecord* with(
      const std::string& key, const KnowledgeReferenceSettings& settings =
                                  KnowledgeReferenceSettings());

  /**
   * Atomically returns the value of a variable.
   * @param   variable  reference to a variable (@see get_ref)
   * @param   settings  the settings for referring to variables
   * @return  pointer to record, nullptr if none exists
   **/
  madara::knowledge::KnowledgeRecord* with(const VariableReference& variable,
      const KnowledgeReferenceSettings& settings =
          KnowledgeReferenceSettings());
  /**
   * Atomically returns a reference to the variable
   * @param   key       unique identifier of the variable
   * @param   settings  the settings for referring to variables
   * @return         the madara::knowledge::KnowledgeRecord::Integer value for
   *the variable
   **/
  const madara::knowledge::KnowledgeRecord* with(
      const std::string& key, const KnowledgeReferenceSettings& settings =
                                  KnowledgeReferenceSettings()) const;

  /**
   * Atomically returns the value of a variable.
   * @param   variable  reference to a variable (@see get_ref)
   * @param   settings  the settings for referring to variables
   * @return         the madara::knowledge::KnowledgeRecord::Integer value for
   *the variable
   **/
  const madara::knowledge::KnowledgeRecord* with(
      const VariableReference& variable,
      const KnowledgeReferenceSettings& settings =
          KnowledgeReferenceSettings()) const;

  template<typename T>
  using MethodType = std::shared_ptr<T> (KnowledgeRecord::*)(void);

  template<typename T, MethodType<T> Get, typename K>
  std::shared_ptr<T> get_shared(
      K&& key, const KnowledgeReferenceSettings& settings)
  {
    auto rec = with(std::forward<K>(key), settings);
    if(rec)
    {
      return (rec->*Get)();
    }
    return nullptr;
  }

  template<typename T>
  using ConstMethodType = std::shared_ptr<T> (KnowledgeRecord::*)(void) const;

  template<typename T, ConstMethodType<T> Get, typename K>
  std::shared_ptr<T> get_shared(
      K&& key, const KnowledgeReferenceSettings& settings) const
  {
    auto rec = with(std::forward<K>(key), settings);
    if(rec)
    {
      return (rec->*Get)();
    }
    return nullptr;
  }

public:
  /**
   * @return a shared_ptr, sharing with the internal one.
   * If this record is not a string, returns NULL shared_ptr
   **/
  std::shared_ptr<const std::string> share_string(
      const std::string& key, const KnowledgeReferenceSettings& settings =
                                  KnowledgeReferenceSettings()) const
  {
    return get_shared<const std::string, &KnowledgeRecord::share_string>(
        key, settings);
  }

  /**
   * @return a shared_ptr, sharing with the internal one.
   * If this record is not a string, returns NULL shared_ptr
   **/
  std::shared_ptr<const std::string> share_string(
      const VariableReference& key, const KnowledgeReferenceSettings& settings =
                                        KnowledgeReferenceSettings()) const
  {
    return get_shared<const std::string, &KnowledgeRecord::share_string>(
        key, settings);
  }

  /**
   * @return a shared_ptr, sharing with the internal one.
   * If this record is not an integer array, returns NULL shared_ptr
   **/
  std::shared_ptr<const std::vector<KnowledgeRecord::Integer>> share_integers(
      const std::string& key, const KnowledgeReferenceSettings& settings =
                                  KnowledgeReferenceSettings()) const
  {
    return get_shared<const std::vector<KnowledgeRecord::Integer>,
        &KnowledgeRecord::share_integers>(key, settings);
  }

  /**
   * @return a shared_ptr, sharing with the internal one.
   * If this record is not an integer array, returns NULL shared_ptr
   **/
  std::shared_ptr<const std::vector<KnowledgeRecord::Integer>> share_integers(
      const VariableReference& key, const KnowledgeReferenceSettings& settings =
                                        KnowledgeReferenceSettings()) const
  {
    return get_shared<const std::vector<KnowledgeRecord::Integer>,
        &KnowledgeRecord::share_integers>(key, settings);
  }

  /**
   * @return a shared_ptr, sharing with the internal one.
   * If this record is not a doubles array, returns NULL shared_ptr
   **/
  std::shared_ptr<const std::vector<double>> share_doubles(
      const std::string& key, const KnowledgeReferenceSettings& settings =
                                  KnowledgeReferenceSettings()) const
  {
    return get_shared<const std::vector<double>,
        &KnowledgeRecord::share_doubles>(key, settings);
  }

  /**
   * @return a shared_ptr, sharing with the internal one.
   * If this record is not a doubles array, returns NULL shared_ptr
   **/
  std::shared_ptr<const std::vector<double>> share_doubles(
      const VariableReference& key, const KnowledgeReferenceSettings& settings =
                                        KnowledgeReferenceSettings()) const
  {
    return get_shared<const std::vector<double>,
        &KnowledgeRecord::share_doubles>(key, settings);
  }

  /**
   * @return a shared_ptr, sharing with the internal one.
   * If this record is not a binary file, returns NULL shared_ptr
   **/
  std::shared_ptr<const std::vector<unsigned char>> share_binary(
      const std::string& key, const KnowledgeReferenceSettings& settings =
                                  KnowledgeReferenceSettings()) const
  {
    return get_shared<const std::vector<unsigned char>,
        &KnowledgeRecord::share_binary>(key, settings);
  }

  /**
   * @return a shared_ptr, sharing with the internal one.
   * If this record is not a binary file, returns NULL shared_ptr
   **/
  std::shared_ptr<const std::vector<unsigned char>> share_binary(
      const VariableReference& key, const KnowledgeReferenceSettings& settings =
                                        KnowledgeReferenceSettings()) const
  {
    return get_shared<const std::vector<unsigned char>,
        &KnowledgeRecord::share_binary>(key, settings);
  }

  /**
   * @return a shared_ptr, sharing with the internal one.
   * If this record is not an Any type, returns NULL shared_ptr
   **/
  std::shared_ptr<const ConstAny> share_any(
      const std::string& key, const KnowledgeReferenceSettings& settings =
                                  KnowledgeReferenceSettings()) const
  {
    return get_shared<const ConstAny, &KnowledgeRecord::share_any>(
        key, settings);
  }

  /**
   * @return a shared_ptr, sharing with the internal one.
   * If this record is not an Any type, returns NULL shared_ptr
   **/
  std::shared_ptr<const ConstAny> share_any(
      const VariableReference& key, const KnowledgeReferenceSettings& settings =
                                        KnowledgeReferenceSettings()) const
  {
    return get_shared<const ConstAny, &KnowledgeRecord::share_any>(
        key, settings);
  }

  /**
   * Gets the contents of a record as a shared pointer to the given type.
   * @tparam T type requested
   * @return a shared_ptr, sharing with the internal one.
   * @throw BadAnyAccess if this record is not an Any holding the given type
   **/
  template<typename T>
  std::shared_ptr<const T> share_any(
      const std::string& key, const KnowledgeReferenceSettings& settings =
                                  KnowledgeReferenceSettings()) const
  {
    return get_shared<const T, &KnowledgeRecord::share_any>(
        key, settings);
  }

  /**
   * Gets the contents of a record as a shared pointer to the given type.
   * @tparam T type requested
   * @return a shared_ptr, sharing with the internal one.
   * @throw BadAnyAccess if this record is not an Any holding the given type
   **/
  template<typename T>
  std::shared_ptr<const T> share_any(
      const VariableReference& key, const KnowledgeReferenceSettings& settings =
                                        KnowledgeReferenceSettings()) const
  {
    return get_shared<const T, &KnowledgeRecord::share_any>(
        key, settings);
  }

  /**
   * Atomically returns a reference to the variable. Variable references are
   * efficient mechanisms for reference variables individually--similar to
   * speedups seen from CompiledExpression.
   * @param   key       unique identifier of the variable
   * @param settings         settings for referring to knowledge variables
   * @return            reference to the variable in the context
   **/
  VariableReference get_ref(
      const std::string& key, const KnowledgeReferenceSettings& settings =
                                  KnowledgeReferenceSettings());

  /**
   * Atomically returns a reference to the variable.
   * @param   key       unique identifier of the variable
   * @param settings    settings for referring to knowledge variables
   * @return            reference to the variable in the context. If variable
   *                    doesn't exist, then a null reference is returned.
   **/
  VariableReference get_ref(
      const std::string& key, const KnowledgeReferenceSettings& settings =
                                  KnowledgeReferenceSettings()) const;

  /**
   * Retrieves a value at a specified index within a knowledge array
   * @param key              knowledge location
   * @param index            index within the array
   * @param settings         settings for referring to knowledge variables
   * @return                 value at knowledge location
   **/
  KnowledgeRecord retrieve_index(const std::string& key, size_t index,
      const KnowledgeReferenceSettings& settings =
          KnowledgeReferenceSettings());

  /**
   * Retrieves a value at a specified index within a knowledge array
   * @param   variable   reference to a variable (@see get_ref)
   * @param   index      index within the array
   * @param   settings   settings for referring to knowledge variables
   * @return             value at knowledge location
   **/
  KnowledgeRecord retrieve_index(const VariableReference& variable,
      size_t index,
      const KnowledgeReferenceSettings& settings =
          KnowledgeReferenceSettings());

  /**
   * Retrieves a knowledge record from the key. This function is useful
   * for performance reasons and also for using a knowledge::KnowledgeRecord
   *that can be one of multiple types
   * @param   key    unique identifier of the variable. Allows variable
   *                 expansion.
   * @param  settings  the settings for referring to variables
   * @return         the knowledge record for the variable
   **/
  KnowledgeRecord* get_record(
      const std::string& key, const KnowledgeReferenceSettings& settings =
                                  KnowledgeReferenceSettings());

  /**
   * Atomically sets the value of a variable to an XML string.
   * @param   key       unique identifier of the variable
   * @param   value     new value of the variable
   * @param   size      indicates the size of the value buffer
   * @param   settings  settings for applying the update
   * @return   0 if the value was set. -1 if null key
   **/
  int set_xml(const std::string& key, const char* value, size_t size,
      const KnowledgeUpdateSettings& settings = KnowledgeUpdateSettings());

  /**
   * Atomically sets the value of a variable to an XML string.
   * @param   variable  reference to a variable (@see get_ref)
   * @param   value     new value of the variable
   * @param   size      indicates the size of the value buffer
   * @param   settings  settings for applying the update
   * @return   0 if the value was set. -1 if null key
   **/
  int set_xml(const VariableReference& variable, const char* value, size_t size,
      const KnowledgeUpdateSettings& settings = KnowledgeUpdateSettings());

  /**
   * Atomically sets the value of a variable to a JPEG image
   * @param   key       unique identifier of the variable
   * @param   value     new value of the variable
   * @param   size      indicates the size of the value buffer
   * @param   settings  settings for applying the update
   * @return   0 if the value was set. -1 if null key
   **/
  int set_jpeg(const std::string& key, const unsigned char* value, size_t size,
      const KnowledgeUpdateSettings& settings = KnowledgeUpdateSettings());

  /**
   * Atomically sets the value of a variable to a JPEG image
   * @param   variable  reference to a variable (@see get_ref)
   * @param   value     new value of the variable
   * @param   size      indicates the size of the value buffer
   * @param   settings  settings for applying the update
   * @return   0 if the value was set. -1 if null key
   **/
  int set_jpeg(const VariableReference& variable, const unsigned char* value,
      size_t size,
      const KnowledgeUpdateSettings& settings = KnowledgeUpdateSettings());

  /**
   * Atomically sets the value of a variable to an arbitrary string.
   * @param   key       unique identifier of the variable
   * @param   value     new value of the variable
   * @param   size      indicates the size of the value buffer
   * @param   settings  settings for applying the update
   * @return   0 if the value was set. -1 if null key
   **/
  int set_file(const std::string& key, const unsigned char* value, size_t size,
      const KnowledgeUpdateSettings& settings = KnowledgeUpdateSettings());

  /**
   * Atomically sets the value of a variable to an arbitrary string.
   * @param   variable  reference to a variable (@see get_ref)
   * @param   value     new value of the variable
   * @param   size      indicates the size of the value buffer
   * @param   settings  settings for applying the update
   * @return   0 if the value was set. -1 if null key
   **/
  int set_file(const VariableReference& variable, const unsigned char* value,
      size_t size,
      const KnowledgeUpdateSettings& settings = KnowledgeUpdateSettings());

  /**
   * Atomically sets the value of a variable to an XML string.
   * @param   key       unique identifier of the variable
   * @param   value     new value of the variable
   * @param   size      indicates the size of the value buffer
   * @param   settings  settings for applying the update
   * @return   0 if the value was set. -1 if null key
   **/
  int set_text(const std::string& key, const char* value, size_t size,
      const KnowledgeUpdateSettings& settings = KnowledgeUpdateSettings());

  /**
   * Atomically sets the value of a variable to a text file's contents.
   * @param   variable  reference to a variable (@see get_ref)
   * @param   value     new value of the variable
   * @param   size      indicates the size of the value buffer
   * @param   settings  settings for applying the update
   * @return   0 if the value was set. -1 if null key
   **/
  int set_text(const VariableReference& variable, const char* value,
      size_t size,
      const KnowledgeUpdateSettings& settings = KnowledgeUpdateSettings());

  /**
   * Atomically sets the value of an array index to a value.
   * @param   key       unique identifier of the variable
   * @param   index     index within array
   * @param   value     new value of the array index
   * @param   settings  settings for applying the update
   * @return   0 if the value was set. -1 if null key
   **/
  template<typename T>
  int set_index(const std::string& key, size_t index, T&& value,
      const KnowledgeUpdateSettings& settings = KnowledgeUpdateSettings());

  /**
   * Atomically sets the value of an array index to a value.
   * @param   variable  reference to a variable (@see get_ref)
   * @param   index     index within array
   * @param   value     new value of the array index
   * @param   settings  settings for applying the update
   * @return   0 if the value was set. -1 if null key
   **/
  template<typename T>
  int set_index(const VariableReference& variable, size_t index, T&& value,
      const KnowledgeUpdateSettings& settings = KnowledgeUpdateSettings());

  /**
   * NON-Atomically sets the value of an array index to a value.
   * THIS IS NOT A THREAD-SAFE FUNCTION.
   * @param   variable  reference to a variable (@see get_ref)
   * @param   index     index within array
   * @param   value     new value of the array index
   * @param   settings  settings for applying the update
   * @return   0 if the value was set. -1 if null key
   **/
  template<typename T>
  int set_index_unsafe(const VariableReference& variable, size_t index,
      T&& value,
      const KnowledgeUpdateSettings& settings = KnowledgeUpdateSettings());

  /**
   * Atomically sets the value of a variable to the specific record.
   * Note, this does not copy meta information (e.g. quality, clock).
   * @param   key       unique identifier of the variable
   * @param   value     new value of the variable
   * @param   settings  settings for applying the update
   * @return   0 if the value was set. -1 if null key
   **/
  template<typename T>
  int set(const std::string& key, T&& value,
      const KnowledgeUpdateSettings& settings = KnowledgeUpdateSettings());

  /**
   * Atomically sets the value of a variable to an array.
   * @param   variable  reference to a variable (@see get_ref)
   * @param   value     new value of the variable
   * @param   settings  settings for applying the update
   * @return   0 if the value was set. -1 if null key
   **/
  template<typename T>
  int set(const VariableReference& variable, T&& value,
      const KnowledgeUpdateSettings& settings = KnowledgeUpdateSettings());

  /**
   * Atomically sets the record to the value specified, as an Any value.
   *
   * Note, this does not copy meta information (e.g. quality, clock).
   * @param   key       unique identifier of the variable
   * @param   value     new value of the variable
   * @param   settings  settings for applying the update
   * @return   0 if the value was set. -1 if null key
   **/
  template<typename T>
  int set_any(const std::string& key, T&& value,
      const KnowledgeUpdateSettings& settings = KnowledgeUpdateSettings());

  /**
   * Atomically sets the record to the value specified, as an Any value.
   *
   * @param   variable  reference to a variable (@see get_ref)
   * @param   value     new value of the variable
   * @param   settings  settings for applying the update
   * @return   0 if the value was set. -1 if null key
   **/
  template<typename T>
  int set_any(const VariableReference& variable, T&& value,
      const KnowledgeUpdateSettings& settings = KnowledgeUpdateSettings());

  /**
   * Atomically emplaces an Any value within the given variable.
   *
   * @param   key       unique identifier of the variable
   * @param   args      arguments to emplace_any of KnowledgeRecord
   * @param   settings  settings for applying the update
   * @return   0 if the value was set. -1 if null key
   **/
  template<typename... Args>
  int emplace_any(const std::string& key,
      const KnowledgeUpdateSettings& settings, Args&&... args);

  /**
   * Atomically emplaces an Any value within the given variable.
   *
   * @param   variable  reference to a variable (@see get_ref)
   * @param   args      arguments to emplace_any of KnowledgeRecord
   * @param   settings  settings for applying the update
   * @return   0 if the value was set. -1 if null key
   **/
  template<typename... Args>
  int emplace_any(const VariableReference& variable,
      const KnowledgeUpdateSettings& settings, Args&&... args);

  /**
   * Atomically emplaces an Any value within the given variable.
   *
   * @param   key       unique identifier of the variable
   * @param   args      arguments to emplace_any of KnowledgeRecord
   * @return   0 if the value was set. -1 if null key
   **/
  template<typename Arg, typename... Args>
  auto emplace_any(const std::string& key, Arg&& arg, Args&&... args)
      -> enable_if_<!is_convertible<Arg, const KnowledgeUpdateSettings&>(), int>
  {
    return emplace_any(key, KnowledgeUpdateSettings{}, std::forward<Arg>(arg),
        std::forward<Args>(args)...);
  }

  /**
   * Atomically emplaces an Any value within the given variable.
   *
   * @param   variable  reference to a variable (@see get_ref)
   * @param   args      arguments to emplace_any of KnowledgeRecord
   * @return   0 if the value was set. -1 if null key
   **/
  template<typename Arg, typename... Args>
  auto emplace_any(const VariableReference& variable, Arg&& arg, Args&&... args)
      -> enable_if_<!is_convertible<Arg, const KnowledgeUpdateSettings&>(), int>
  {
    return emplace_any(variable, KnowledgeUpdateSettings{},
        std::forward<Arg>(arg), std::forward<Args>(args)...);
  }

  /**
   * Atomically emplaces an empty Any value within the given variable.
   *
   * @param   key       unique identifier of the variable
   * @return   0 if the value was set. -1 if null key
   **/
  int emplace_any(const std::string& key)
  {
    return emplace_any(key, KnowledgeUpdateSettings{});
  }

  /**
   * Atomically emplaces an empty Any value within the given variable.
   *
   * @param   variable  reference to a variable (@see get_ref)
   * @return   0 if the value was set. -1 if null key
   **/
  int emplace_any(const VariableReference& variable)
  {
    return emplace_any(variable, KnowledgeUpdateSettings{});
  }

  /**
   * NON-ATOMICALLY emplaces an Any value within the given variable.
   *
   * @param   variable  reference to a variable (@see get_ref)
   * @param   args      arguments to emplace_any of KnowledgeRecord
   * @param   settings  settings for applying the update
   * @return   0 if the value was set. -1 if null key
   **/
  template<typename... Args>
  int emplace_any_unsafe(const VariableReference& variable,
      const KnowledgeUpdateSettings& settings, Args&&... args);

  /**
   * Atomically sets the value of a variable to an array.
   * @param   key       unique identifier of the variable
   * @param   value     an array
   * @param   size      number of elements in the array
   * @param   settings  settings for applying the update
   * @return   0 if the value was set. -1 if null key
   **/
  template<typename T>
  int set(const std::string& key, const T* value, uint32_t size,
      const KnowledgeUpdateSettings& settings = KnowledgeUpdateSettings());

  /**
   * Atomically sets the value of a variable to an array.
   * @param   variable  reference to a variable (@see get_ref)
   * @param   value     an array of type T
   * @param   size      number of elements in the array
   * @param   settings  settings for applying the update
   * @return   0 if the value was set. -1 if null key
   **/
  template<typename T>
  int set(const VariableReference& variable, const T* value, uint32_t size,
      const KnowledgeUpdateSettings& settings = KnowledgeUpdateSettings());

  /**
   * NON-Atomically sets the value of a variable to the specific value.
   * THIS IS NOT A THREAD-SAFE FUNCTION.
   * Note, this does not copy meta information (e.g. quality, clock).
   * @param   variable  reference to a variable (@see get_ref)
   * @param   value     new value of the variable
   * @param   settings  settings for applying the update
   * @return   0 if the value was set. -1 if null key
   **/
  template<typename T>
  int set_unsafe(const VariableReference& variable, T&& value,
      const KnowledgeUpdateSettings& settings = KnowledgeUpdateSettings());

  /**
   * NON-Atomically sets the value of a variable to the specific array.
   * THIS IS NOT A THREAD-SAFE FUNCTION.
   * Note, this does not copy meta information (e.g. quality, clock).
   * @param   variable  reference to a variable (@see get_ref)
   * @param   array     new array value of the variable
   * @param   size      size of array pointed to by array
   * @param   settings  settings for applying the update
   * @return   0 if the value was set. -1 if null key
   **/
  template<typename T>
  int set_unsafe(const VariableReference& variable, const T* array, size_t size,
      const KnowledgeUpdateSettings& settings = KnowledgeUpdateSettings());

  /**
   * Atomically reads a file into a variable
   * @param   key       unique identifier of the variable
   * @param   filename  file to read
   * @param   settings  settings for applying the update
   * @return   0 if the value was set. -1 if unsuccessful
   **/
  int read_file(const std::string& key, const std::string& filename,
      const KnowledgeUpdateSettings& settings = KnowledgeUpdateSettings());

  /**
   * Atomically reads a file into a variable
   * @param   variable  reference to a variable (@see get_ref)
   * @param   filename  file to read
   * @param   settings  settings for applying the update
   * @return   0 if the value was set. -1 if unsuccessful
   **/
  int read_file(const VariableReference& variable, const std::string& filename,
      const KnowledgeUpdateSettings& settings = KnowledgeUpdateSettings());

  /**
   * Atomically sets if the variable value will be different
   * @param   key       unique identifier of the variable
   * @param   value     new value of the variable
   * @param   quality   quality to set the variable at (type of priority)
   * @param   clock     clock value of the update
   * @param   settings  settings for applying the update
   * @return   1 if the value was changed. 0 if not changed.
   *          -1 if null key
   **/
  int set_if_unequal(const std::string& key,
      madara::knowledge::KnowledgeRecord::Integer value, uint32_t quality,
      uint64_t clock,
      const KnowledgeUpdateSettings& settings = KnowledgeUpdateSettings());

  /**
   * Atomically sets if the variable value will be different
   * @param   key       unique identifier of the variable
   * @param   value     new value of the variable
   * @param   quality   quality to set the variable at (type of priority)
   * @param   clock     clock value of the update
   * @param   settings  settings for applying the update
   * @return   1 if the value was changed. 0 if not changed.
   *          -1 if null key
   **/
  int set_if_unequal(const std::string& key, double value, uint32_t quality,
      uint64_t clock,
      const KnowledgeUpdateSettings& settings = KnowledgeUpdateSettings());

  /**
   * Atomically sets if the variable value will be different
   * @param   key       unique identifier of the variable
   * @param   value     new value of the variable
   * @param   quality   quality to set the variable at (type of priority)
   * @param   clock     clock value of the update
   * @param   settings  settings for applying the update
   * @return   1 if the value was changed. 0 if not changed.
   *          -1 if null key
   **/
  int set_if_unequal(const std::string& key, const std::string& value,
      uint32_t quality, uint64_t clock,
      const KnowledgeUpdateSettings& settings = KnowledgeUpdateSettings());

  /**
   * Atomically sets if the variable value meets update conditions.
   * Appropriate conditions include clock being >= old clock, quality
   * >= old quality, etc.
   * @param   key       unique identifier of the variable
   * @param   rhs     new value of the variable
   * @param   settings  settings for applying the update
   * @return   1 if the value was changed. 0 if not changed.
   *          -1 if null key
   **/
  int update_record_from_external(const std::string& key,
      const knowledge::KnowledgeRecord& rhs,
      const KnowledgeUpdateSettings& settings = KnowledgeUpdateSettings(true));

  /**
   * Atomically sets if the variable value meets update conditions.
   * Appropriate conditions include clock being >= old clock, quality
   * >= old quality, etc.
   * @param   target    the target reference in the knowledge base
   * @param   rhs       new value of the variable
   * @param   settings  settings for applying the update
   * @return   1 if the value was changed. 0 if not changed.
   *          -1 if null key
   **/
  int update_record_from_external(const VariableReference& target,
      const knowledge::KnowledgeRecord& new_value,
      const KnowledgeUpdateSettings& settings = KnowledgeUpdateSettings(true));

  /**
   * Atomically gets quality of a variable
   * @param   key       unique identifier of the
   * @param   settings       settings for referring to a knowledge variable
   * @return   quality associated with the variable
   **/
  uint32_t get_quality(
      const std::string& key, const KnowledgeReferenceSettings& settings =
                                  KnowledgeReferenceSettings());

  /**
   * Atomically gets write quality of this process for a variable
   * @param   key       unique identifier of the variable
   * @param   settings       settings for referring to a knowledge variable
   * @return   write quality associated with the variable
   **/
  uint32_t get_write_quality(
      const std::string& key, const KnowledgeReferenceSettings& settings =
                                  KnowledgeReferenceSettings());

  /**
   * Atomically sets quality of this process for a variable
   * @param   key            unique identifier of the variable
   * @param   quality        quality of this process
   * @param   force_update   force an update to variable, even if lower
   * @param   settings       settings for referring to a knowledge variable
   * @return   write quality associated with the variable
   **/
  uint32_t set_quality(const std::string& key, uint32_t quality,
      bool force_update, const KnowledgeReferenceSettings& settings);

  /**
   * Force a change to be registered, waking up anyone waiting on entry
   **/
  void set_changed(void);

  /**
   * Atomically sets write quality of this process for a variable
   * @param   key            unique identifier of the variable
   * @param   quality        write quality of this process
   * @param   settings       settings for referring to a knowledge variable
   **/
  void set_write_quality(const std::string& key, uint32_t quality,
      const KnowledgeReferenceSettings& settings);

  /**
   * Retrieves a list of modified variables. Useful for building a
   * disseminatable knowledge update.
   * @return  the modified knowledge records
   **/
  const VariableReferenceMap& get_modifieds(void) const;

  /**
   * Retrieves the current modifieds map
   * @param   send_list map of variables that limit what will be sent
   * @param   reset     reset modifieds atomically
   * @return  the modified knowledge records
   **/
  KnowledgeMap get_modifieds_current(
      const std::map<std::string, bool> & send_list, bool reset = true);

  /**
   * Adds a list of VariableReferences to the current modified list.
   * @param  modifieds  a list of variables to add to modified list
   **/
  void add_modifieds(const VariableReferences& modifieds) const;

  /**
   * Saves the list of modified records to use later for resending. This
   * does not clear the modified list. This feature is useful if you
   * want to remember what has been modified and then resend later, e.g.,
   * if you believe packets may be dropped and want to resend information.
   * Use this function in conjunction with @see add_modifieds to remodify
   * @return a vector of VariableReferences to the current modified list
   **/
  VariableReferences save_modifieds(void) const;

  /**
   * Retrieves a stringified list of all modified variables that are ready
   * to send over transport on next send_modifieds call
   * @return  stringified list of the modified knowledge records
   **/
  std::string debug_modifieds(void) const;

  /**
   * Retrieves a list of modified local variables. Useful for building a
   * comprehensive checkpoint.
   * @return  the modified knowledge records
   **/
  const VariableReferenceMap& get_local_modified(void) const;

  /**
   * Reset all variables to be unmodified. This will clear all global
   * knowledge updates. Use with caution.
   **/
  void reset_modified(void);

  /**
   * Reset all checkpoint variables in the modified lists. This essentially
   * purges the diff and starts fresh
   **/
  void reset_checkpoint(void) const;

  /**
   * Changes all global variables to modified at current clock.
   **/
  void apply_modified(void);

  /**
   * Marks the variable reference as updated for the purposes of sending or
   * checkpointing knowledge (for globals and locals respectively)
   * @param   variable  reference to a variable (@see get_ref)
   * @param  settings  the settings for referring to variables
   **/
  void mark_modified(const VariableReference& variable,
      const KnowledgeUpdateSettings& settings = KnowledgeUpdateSettings());

  /**
   * Changes global variables to modified at current clock for the purposes
   * of sending or checkpointing knowledge (globals and locals respectively)
   * @param  key     the key of the record you are changing
   * @param  settings  the settings for referring to variables
   **/
  void mark_modified(const std::string& key,
      const KnowledgeUpdateSettings& settings = KnowledgeUpdateSettings());

  /**
   * Changes variable reference to modified at current clock, and queues it
   * to send, even if it is a local that would not ordinarily be sent
   * @param   variable  reference to a variable (@see get_ref)
   * @param  settings  the settings for referring to variables
   **/
  void mark_to_send(const VariableReference& variable,
      const KnowledgeUpdateSettings& settings = KnowledgeUpdateSettings());

  /**
   * Changes variable to modified at current clock, and queues it to send,
   * even if it is a local that would not ordinarily be sent
   * @param  key     the key of the record you are marking
   * @param  settings  the settings for referring to variables
   **/
  void mark_to_send(const std::string& key,
      const KnowledgeUpdateSettings& settings = KnowledgeUpdateSettings());

  /**
   * Changes variable reference to modified at current clock for the
   * purposes of checkpointing (even if it is a global).
   * @param   variable  reference to a variable (@see get_ref)
   * @param  settings  the settings for referring to variables
   **/
  void mark_to_checkpoint(const VariableReference& variable,
      const KnowledgeUpdateSettings& settings = KnowledgeUpdateSettings());

  /**
   * Changes variable to modified at current clock for the purposes of
   * checkpointing (even if it is a global).
   * @param  key     the key of the record you are marking
   * @param  settings  the settings for referring to variables
   **/
  void mark_to_checkpoint(const std::string& key,
      const KnowledgeUpdateSettings& settings = KnowledgeUpdateSettings());

  /**
   * Resets a variable to unmodified
   * @param   key            unique identifier of the variable
   **/
  void reset_modified(const std::string& key);

  /**
   * Atomically increments the value of the variable
   * @param   key            unique identifier of the variable
   * @param   settings  settings for applying the update
   * @return                 new value of variable
   **/
  madara::knowledge::KnowledgeRecord inc(const std::string& key,
      const KnowledgeUpdateSettings& settings = KnowledgeUpdateSettings());

  /**
   * Atomically increments the value of the variable
   * @param   variable  reference to a variable (@see get_ref)
   * @param   settings  settings for applying the update
   * @return                 new value of variable
   **/
  madara::knowledge::KnowledgeRecord inc(const VariableReference& variable,
      const KnowledgeUpdateSettings& settings = KnowledgeUpdateSettings());

  /**
   * Wait for a change to happen to the context.
   * @param   extra_release  perform extra release of lock for nested locks
   **/
  void wait_for_change(bool extra_release = false);

  /**
   * Atomically decrements the value of the variable
   * @param   key            unique identifier of the variable
   * @param   settings  settings for applying the update
   * @return                 new value of variable
   **/
  madara::knowledge::KnowledgeRecord dec(const std::string& key,
      const KnowledgeUpdateSettings& settings = KnowledgeUpdateSettings());

  /**
   * Atomically decrements the value of the variable
   * @param   variable  reference to a variable (@see get_ref)
   * @param   settings  settings for applying the update
   * @return                 new value of variable
   **/
  madara::knowledge::KnowledgeRecord dec(const VariableReference& variable,
      const KnowledgeUpdateSettings& settings = KnowledgeUpdateSettings());

  /**
   * Clears a variable. This is much safer than @see delete_variable.
   * It clears the memory used in the variable and marks it as UNCREATED,
   * meaning that it is effectively deleted, will not show up in
   * @see print statements or @see save_checkpoint.
   * @param   key            unique identifier of the variable
   * @param   settings       settings for referring to variables
   * @return                 true if variable exists
   **/
  bool clear(
      const std::string& key, const KnowledgeReferenceSettings& settings =
                                  KnowledgeReferenceSettings());

  /**
   * Clears a variable. This is much safer than @see delete_variable.
   * It clears the memory used in the variable and marks it as UNCREATED,
   * meaning that it is effectively deleted, will not show up in
   * @see print statements or @see save_checkpoint.
   * @param   variable       reference to a variable (@see get_ref)
   **/
  bool clear(const VariableReference& variable);

  /**
   * Deletes the key. Note that this is extremely unsafe. You
   * can cause illegal operations in the knowledge base by using
   * this method and trying to access this variable again with
   * 1) a VariableReference, 2) a @see saved_modifieds listing,
   * 3) a compiled expression in KaRL that uses the variable.
   * There are very, very few reasons to ever use this function.
   * Actually, there is one: you will never, ever, ever use this
   * variable again. That's it. Otherwise, use @see clear with
   * the default argument
   * @param   key            unique identifier of the variable
   * @param   settings       settings for referring to variables
   * @return                 true if variable exists
   **/
  bool delete_variable(
      const std::string& key, const KnowledgeReferenceSettings& settings =
                                  KnowledgeReferenceSettings());

  /**
   * Deletes the variable. Note that this is extremely unsafe. You
   * can cause illegal operations in the knowledge base by using
   * this method and trying to access this variable again with
   * 1) a VariableReference, 2) a @see saved_modifieds listing,
   * 3) a compiled expression in KaRL that uses the variable.
   * There are very, very few reasons to ever use this function.
   * Actually, there is one: you will never, ever, ever use this
   * variable again. That's it. Otherwise, use @see clear with
   * the default argument
   * @param   var            VariableReference to variable. Do not use
   *                         object this after calling this method!
   *
   * @param   settings       settings for referring to variables
   * @return                 true if variable exists
   **/
  bool delete_variable(
      const VariableReference& var, const KnowledgeReferenceSettings& settings =
                                        KnowledgeReferenceSettings());

  /**
   * Deletes variables. Note that this is extremely unsafe. You
   * can cause illegal operations in the knowledge base by using
   * this method and trying to access this variable again with
   * 1) a VariableReference, 2) a @see saved_modifieds listing,
   * 3) a compiled expression in KaRL that uses the variable.
   * There are very, very few reasons to ever use this function.
   * Actually, there is one: you will never, ever, ever use this
   * variable again. That's it. Otherwise, use @see clear with
   * the default argument
   * @param   begin          iterator to variables inside the map
   * @param   end            iterator to variables inside the map
   * @param   settings       settings for referring to variables
   * @return                 true if variable exists
   **/
  void delete_variables(KnowledgeMap::iterator begin,
      KnowledgeMap::iterator end,
      const KnowledgeReferenceSettings& settings =
          KnowledgeReferenceSettings());

  /**
   * Deletes keys starting with the given prefix
   * @param   prefix         string which starts all variables to delete
   * @param   settings       settings for referring to variables
   **/
  void delete_prefix(
      const std::string& prefix, const KnowledgeReferenceSettings& settings =
                                     KnowledgeReferenceSettings());

  /**
   * Deletes the expression from the interpreter cache
   * @param   expression     the KaRL logic in the interpreter context
   * @return                 true if variable exists
   **/
  bool delete_expression(const std::string& expression);

  /**
   * Atomically checks to see if a variable already exists
   * @param   key            unique identifier of the variable
   * @param   settings       settings for referring to variables
   * @return                 true if variable exists
   **/
  bool exists(
      const std::string& key, const KnowledgeReferenceSettings& settings =
                                  KnowledgeReferenceSettings()) const;

  /**
   * Checks if a knowledge variable exists in the context
   *
   * @param variable        knowledge variable reference
   * @param settings        settings for referring to knowledge variables
   * @return                true if location has been set
   **/
  bool exists(const VariableReference& variable,
      const KnowledgeReferenceSettings& settings = KnowledgeReferenceSettings(
          false)) const;

  /**
   * Atomically prints all variables and values in the context
   * @param   level          log level. @see LogMacros.h
   **/
  void print(unsigned int level) const;

  /**
   * Atomically Print a statement, similar to printf (variable expansions
   * allowed) e.g., input = "MyVar{.id} = {MyVar{.id}}\n";
   * @param   statement      templated statement to print from
   * @param   level          log level. @see LogMacros.h
   **/
  void print(const std::string& statement, unsigned int level) const;

  /**
   * Clears the context
   * @param  erase   If true, completely erase keys.
   *                 If false, reset knowledge
   **/
  void clear(bool erase = false);

  /**
   * Locks the mutex on this context. Warning: this will cause
   * all operations to block until the unlock call is made.
   *
   * Use ContextGuard instead of calling this directly.
   **/
  void lock(void) const;

  /**
   * Locks the mutex on this context. Warning: this will cause
   * all operations to block until the unlock call is made.
   *
   * Returns immediately without blocking. Returns true if lock
   * was successful, false otherwise.
   **/
  bool try_lock(void) const;

  /**
   * Unlocks the mutex on this context.
   **/
  void unlock(void) const;

  /**
   * Atomically increments the Lamport clock and returns the new
   * clock time (intended for sending knowledge updates).
   * @param   settings  settings for applying the update
   * @return        new clock time
   **/
  uint64_t inc_clock(
      const KnowledgeUpdateSettings& settings = KnowledgeUpdateSettings());

  /**
   * Atomically increments the Lamport clock of a variable and returns the
   * new clock time (intended for sending knowledge updates).
   * @param   key   unique identifier of the variable
   * @param   settings  settings for applying the update
   * @return        new clock time for variable
   **/
  uint64_t inc_clock(const std::string& key,
      const KnowledgeUpdateSettings& settings = KnowledgeUpdateSettings());

  /**
   * Atomically sets the lamport clock.
   * @param  clock  new global clock
   * @return        new clock time
   **/
  uint64_t set_clock(uint64_t clock);

  /**
   * Atomically sets the Lamport clock of a variable and returns the
   * new clock time (intended for sending knowledge updates).
   * @param   key       unique identifier of the variable
   * @param   clock     new variable clock
   * @param   settings  settings for applying the update
   * @return           new clock time for variable
   **/
  uint64_t set_clock(const std::string& key, uint64_t clock,
      const KnowledgeReferenceSettings& settings =
          KnowledgeReferenceSettings());

  /**
   * Atomically gets the Lamport clock
   * @return           current global clock
   **/
  uint64_t get_clock(void) const;

  /**
   * Atomically gets the Lamport clock of a variable
   * @param   key       unique identifier of the variable
   * @param   settings  settings for reading the variable
   * @return           current variable clock
   **/
  uint64_t get_clock(
      const std::string& key, const KnowledgeReferenceSettings& settings =
                                  KnowledgeReferenceSettings()) const;

  /**
   * Signals that this thread is done with the context. Anyone
   * waiting on the underlying condition is awoken.
   **/
  void signal(bool lock = true) const;

  /**
   * Expands a string with variable expansion. For instance, if
   * .id == 5, and a statement of "MyVar{.id} = {.id} * 30" then
   * then expanded statement would be "MyVar5 = 5 * 30".
   * @param  statement  statement to expand. Useful for printing.
   * @return            variable expanded statement
   **/
  std::string expand_statement(const std::string& statement) const;

  /**
   * Copies variables and values from source to this context.
   * PERFORMANCE NOTES: predicates with prefixes can limit
   * copying to O(log n). predices with suffixes and no prefix
   * force O(n) copy cost since all records could fit requirements
   *
   * @param  source    the source context to copy from
   * @param  reqs      requirements that must be met
   * @param  settings  settings for appplying updates to context
   **/
  void copy(const ThreadSafeContext& source, const KnowledgeRequirements& reqs,
      const KnowledgeUpdateSettings& settings);

  /**
   * Copies variables and values from source to this context. PERFORMANCE
   * NOTES: worst case depends on size of copy_set. If empty, performance
   * is always O (n), where n is number of variables in the source context.
   * If copy_set is not empty, then performance is O (m log n) where m is
   * the number of variables in copy_set and n is number of variables in
   * the source context.
   * <br>&nbsp;<br>
   * Note that this is a deep copy due to the fact
   * that source and destination are expected to have their own thread
   * management (ref-counted variables can be problematic if shallow copy).
   *
   * @param  source    the source context to copy from
   * @param  copy_set  a map of variables that should be copied. If
   *                   empty, then everything is copied. If not empty,
   *                   only the supplied variables will be copied.
   * @param  clean_copy  if true, clear the destination context (this)
   *                     before copying.
   * @param  settings  settings for appplying updates to context
   **/
  void copy(const ThreadSafeContext& source,
      const CopySet& copy_set = CopySet(), bool clean_copy = false,
      const KnowledgeUpdateSettings& settings = KnowledgeUpdateSettings());

#ifndef _MADARA_NO_KARL_

  /**
   * Compiles a KaRL expression into an expression tree
   *
   * @param expression         expression to compile
   * @return                   compiled, optimized expression tree
   * @throw exceptions::KarlException  failure during compile/evaluate
   **/
  CompiledExpression compile(const std::string& expression);

  /**
   * Defines an external function
   * @param  name       name of the function
   * @param  func       external function to call with this name
   * @param  settings   settings for referring to variables
   **/
  void define_function(const std::string& name,
      knowledge::KnowledgeRecord (*func)(FunctionArguments&, Variables&),
      const KnowledgeReferenceSettings& settings =
          KnowledgeReferenceSettings());

  /**
   * Defines a named function that can distinguish the name it was called
   * with in MADARA
   * @param  name       name of the function
   * @param  func       external function to call with this name
   * @param  settings   settings for referring to variables
   **/
  void define_function(const std::string& name,
      knowledge::KnowledgeRecord (*func)(
          const char*, FunctionArguments&, Variables&),
      const KnowledgeReferenceSettings& settings =
          KnowledgeReferenceSettings());

#ifdef _MADARA_JAVA_
  /**
   * Defines a named java function
   * @param  name       name of the function
   * @param  callable   external java object to call with this name
   * @param  settings   settings for referring to variables
   **/
  void define_function(const std::string& name, jobject callable,
      const KnowledgeReferenceSettings& settings =
          KnowledgeReferenceSettings());
#endif

#ifdef _MADARA_PYTHON_CALLBACKS_
  /**
   * Defines a named python function
   * @param  name       name of the function
   * @param  callable   external python function to call with this name
   * @param  settings   settings for referring to variables
   **/
  void define_function(const std::string& name, boost::python::object callable,
      const KnowledgeReferenceSettings& settings =
          KnowledgeReferenceSettings());
#endif

  /**
   * Defines a MADARA KaRL function
   * @param  name       name of the function
   * @param  expression KaRL function body
   * @param  settings   settings for referring to variables
   **/
  void define_function(const std::string& name, const std::string& expression,
      const KnowledgeReferenceSettings& settings =
          KnowledgeReferenceSettings());

  /**
   * Defines a MADARA KaRL function
   * @param  name       name of the function
   * @param  expression KaRL function body
   * @param  settings   settings for referring to variables
   **/
  void define_function(const std::string& name,
      const CompiledExpression& expression,
      const KnowledgeReferenceSettings& settings =
          KnowledgeReferenceSettings());

  /**
   * Retrieves an external function
   * @param  name       name of the function to retrieve
   * @param  settings   settings for referring to variables
   * @return            the mapped external function
   **/
  Function* retrieve_function(
      const std::string& name, const KnowledgeReferenceSettings& settings =
                                   KnowledgeReferenceSettings());

  /**
   * Evaluate a compiled expression. Please note that if you update
   * any variables here, they will not be sent through any transports
   * until you call through the KnowledgeBase.
   * @param   expression  A compiled expressio to run.
   * @param   settings    settings for applying the update
   * @return              result of the evaluation
   * @throw exceptions::KarlException  failure during compile/evaluate
   **/
  knowledge::KnowledgeRecord evaluate(CompiledExpression expression,
      const KnowledgeUpdateSettings& settings = KnowledgeUpdateSettings());

  /**
   * Evaluate a component node-rooted tree. Please note that if you update
   * any variables here, they will not be sent through any transports
   * until you call through the KnowledgeBase.
   * @param   root        Root of an expression tree
   * @param   settings    settings for applying the update
   * @return              result of the evaluation
   * @throw exceptions::KarlException  failure during compile/evaluate
   **/
  knowledge::KnowledgeRecord evaluate(expression::ComponentNode* root,
      const KnowledgeUpdateSettings& settings = KnowledgeUpdateSettings());

#endif  // _MADARA_NO_KARL_

  /**
   * Saves all keys and values into a string, using the underlying
   * knowledge::KnowledgeRecord::to_string function. This is an optimized
   * version that allows the specification of a target string to
   * avoid string copying (which can be expensive with longer
   * strings across multiple function boundaries). This function differs
   * from knowledge::KnowledgeRecord to_string in that it is intended to save
   *the database in a format that can be easily parseable. Consequently, strings
   *are delineated in this function by being included in single quotes. Arrays
   *are delineated with array indices [].
   *
   * This is not appropriate
   * for saving the context if it has binary data inside of it, as
   * only the size of the data entry would be saved.
   * @param target  where to store the resulting string operation.
   * @param array_delimiter  for any arrays, the characters in between
   *                         data elements.
   * @param record_delimiter the characters to place between individual
   *                         key/value pairs
   * @param key_val_delimiter the characters to place between keys and
   *                          values.
   **/
  void to_string(std::string& target, const std::string& array_delimiter = ",",
      const std::string& record_delimiter = ";\n",
      const std::string& key_val_delimiter = "=") const;

  /**
   * Fills a vector with Knowledge Records that begin with a common subject
   * and have a finite range of integer values.
   * @param   subject     The common subject of the variable names. For
   *                      instance, if we are looking for a range of vars
   *                      like "var0", "var1", "var2", then the common
   *                      subject would be "var".
   * @param   start       An inclusive start index
   * @param   end         An inclusive end index
   * @param   target      The vector that will be filled with
   *                      Knowledge Record instances within the subject
   *                      range.
   * @return              entries in the resulting vector
   **/
  size_t to_vector(const std::string& subject, unsigned int start,
      unsigned int end, std::vector<KnowledgeRecord>& target);

  /**
   * Creates an iteration of VariableReferences to all keys matching
   * the prefix and suffix
   * @param  prefix   string that the key must start with
   * @param  suffix   string that the key must end with
   * @param  matches  all VariableReferences matching the prefix and suffix
   **/
  void get_matches(const std::string& prefix, const std::string& suffix,
      VariableReferences& matches);

  /**
   * Fills a variable map with Knowledge Records that match an expression.
   * At the moment, this expression must be of the form "subject*"
   * @param   subject     An expression that matches the variable names
   *                      that are of interest. Wildcards may only be
   *                      at the end.
   * @param   target      The map that will be filled with variable names
   *                      and the Knowledge Records that correspond to
   * @return              entries in the resulting map
   **/
  size_t to_map(const std::string& subject,
      std::map<std::string, knowledge::KnowledgeRecord>& target);

  /**
   * Adds a file to the logger
   * @param  filename  the file to add to logger
   **/
  void add_logger(const std::string& filename);

  /**
   * Gets the log level
   * @return the maximum detail level to print
   **/
  int get_log_level(void);

  /**
   * Sets the log level
   * @param  level  the maximum detail level to print
   **/
  void set_log_level(int level);

  /**
   * Gets the logger used for information printing
   * @return the context's logger
   **/
  logger::Logger& get_logger(void) const;

  /**
   * Attaches a logger to be used for printing
   * @param logger  the logger the context should lose
   **/
  void attach_logger(logger::Logger& logger) const;

  /**
   * Fills a variable map with list of keys according to a matching prefix,
   * suffix, and delimiter hierarchy. This is useful for understanding the
   * logical hierarchy of your variables (and also a key utility of
   * containers like @see containers::FlexMap).
   * @param   prefix      Text that must be present at the front of the key
   * @param   delimiter   Text that signifies a logical boundary in hierarchy If
   *                      empty, no check is performed.
   * @param   suffix      Text that must be present at the end of the key. If
   *                      empty, no check is performed.
   * @param   next_keys   The immediate keys in the hierarchy after prefix
   * @param   result      The map that will be filled with full variable names
   *                      and the Knowledge Records that correspond to
   * @param   just_keys   if true, do not fill result, only next_keys
   * @return              entries in the resulting map
   **/
  size_t to_map(const std::string& prefix, const std::string& delimiter,
      const std::string& suffix, std::vector<std::string>& next_keys,
      std::map<std::string, knowledge::KnowledgeRecord>& result,
      bool just_keys);

  /**
   * Creates a map with Knowledge Records that begin with the given
   * prefix. Runs in O(log n + m) time, where n is the size of the
   * KnowledgeBase, and m is the number of matching records
   *
   * @param   prefix      Prefix string to match with
   * @return              A new map with just entries starting with prefix
   **/
  knowledge::KnowledgeMap to_map(const std::string& prefix) const;

  /**
   * Creates a map with Knowledge Records that begin with the given
   * prefix. Runs in O(log n + m) time, where n is the size of the
   * KnowledgeBase, and m is the number of matching records
   *
   * All key names have the "prefix" portion stripped off their front in
   * the returned map.
   *
   * @param   prefix      Prefix string to match with
   * @return              A new map with just entries starting with prefix
   **/
  knowledge::KnowledgeMap to_map_stripped(const std::string& prefix) const;

  /**
   * Saves the context to a file
   * @param   filename    name of the file to open
   * @param   id          unique identifier of the context holder
   * @return              -1 if file open failed<br />
   *                      -2 if file write failed<br />
   *                      >0 if successful (number of bytes written)
   * @throw exceptions::MemoryException  not enough buffer to encode
   **/
  int64_t save_context(
      const std::string& filename, const std::string& id = "") const;

  /**
   * Saves the context to a file
   * @param   settings    the settings to save
   * @return              -1 if file open failed<br />
   *                      -2 if file write failed<br />
   *                      >0 if successful (number of bytes written)
   * @throw exceptions::MemoryException  not enough buffer to encode
   **/
  int64_t save_context(const CheckpointSettings& settings) const;

  /**
   * Saves the context to a file as JSON
   * @param   filename    name of the file to save to
   * @return  total bytes written
   **/
  int64_t save_as_json(const std::string& filename) const;

  /**
   * Saves the context to a file as JSON
   * @param   settings    the settings to save
   * @return  total bytes written
   **/
  int64_t save_as_json(const CheckpointSettings& settings) const;

  /**
   * Saves the context to a file as karl assignments, rather than binary
   * @param   filename    name of the file to save to
   * @return  total bytes written
   **/
  int64_t save_as_karl(const std::string& filename) const;

  /**
   * Saves the context to a file as karl assignments, rather than binary
   * @param   settings    the settings to save
   * @return  total bytes written
   **/
  int64_t save_as_karl(const CheckpointSettings& settings) const;

  /**
   * Loads the context from a file
   * @param   filename    name of the file to open
   * @param   id          unique identifier of the context holder (will
   *                      be changed by the function)
   * @param   settings    settings for applying the update
   * @return              -1 if file open failed<br />
   *                      -2 if file read failed<br />
   *                      >0 if successful (number of bytes written)
   **/
  int64_t load_context(const std::string& filename, std::string& id,
      const KnowledgeUpdateSettings& settings = KnowledgeUpdateSettings(
          true, true, true, false));

  /**
   * Loads the context from a file
   * @param   filename    name of the file to open
   * @param   meta        a file header that will contain metadata such
   *                      as originator, timestamp, lamport clock, etc.
   * @param   settings    settings for applying the update
   * @return              -1 if file open failed<br />
   *                      -2 if file read failed<br />
   *                      >0 if successful (number of bytes written)
   * @throw exceptions::MemoryException  not enough buffer to encode
   **/
  int64_t load_context(const std::string& filename, FileHeader& meta,
      const KnowledgeUpdateSettings& settings = KnowledgeUpdateSettings(
          true, true, true, false));

  /**
   * Loads the context from a file
   * @param   checkpoint_settings  checkpoint settings to load
   * @param   update_settings      settings for applying the updates
   * @return              -1 if file open failed<br />
   *                      -2 if file read failed<br />
   *                      >0 if successful (number of bytes written)
   * @throw exceptions::MemoryException  not enough buffer to encode
   **/
  int64_t load_context(CheckpointSettings& checkpoint_settings,
      const KnowledgeUpdateSettings& update_settings = KnowledgeUpdateSettings(
          true, true, true, false));

  /**
   * Loads and evaluates a karl script from a file
   * @param   checkpoint_settings  checkpoint settings to load
   * @param   update_settings      settings for applying the updates
   * @return                       result of evaluation
   * @throw exceptions::MemoryException  not enough buffer to encode
   **/
  KnowledgeRecord evaluate_file(CheckpointSettings& checkpoint_settings,
      const KnowledgeUpdateSettings& update_settings = KnowledgeUpdateSettings(
          true, true, true, false));

  /**
   * Loads and returns a karl script from a file with encode/decode
   * @param   checkpoint_settings  checkpoint settings to load
   * @throw exceptions::MemoryException  not enough buffer to encode
   **/
  std::string file_to_string(CheckpointSettings& checkpoint_settings);

  /**
   * Saves a checkpoint of a list of changes to a file
   * @param   filename    name of the file to open
   * @param   id          unique identifier of the context holder
   * @return              -1 if file open failed<br />
   *                      -2 if file write failed<br />
   *                      >0 if successful (number of bytes written)
   * @throw exceptions::MemoryException  not enough buffer to encode
   **/

  int64_t save_checkpoint(
      const std::string& filename, const std::string& id = "") const;

  /**
   * Saves a checkpoint of a list of changes to a file
   * @param   settings    checkpoint settings to load
   * @return              -1 if file open failed<br />
   *                      -2 if file write failed<br />
   *                      >0 if successful (number of bytes written)
   * @throw exceptions::MemoryException  not enough buffer to encode
   **/

  int64_t save_checkpoint(const CheckpointSettings& settings) const;

  /**
   * Attach a streaming provider object, inherited from BaseStreamer,
   * such as CheckpointStreamer. Once attached, all updates to records
   * in this ThreadSafeContext will be provided to the streamer. May
   * pass nullptr to stop streaming.
   *
   * @param streamer the new streamer to attach
   * @return the old streamer, or nullptr if there wasn't any.
   **/
  std::unique_ptr<BaseStreamer> attach_streamer(
      std::unique_ptr<BaseStreamer> streamer)
  {
    MADARA_GUARD_TYPE guard(mutex_);

    using std::swap;
    swap(streamer, streamer_);

    return streamer;
  }

  /**
   * NOT THREAD SAFE!
   *
   * Retrieves a reference to the underlying KnowledgeMap. This is not
   * thread-safe. Only call this if you already hold this Context's lock.
   * Reading the map is then generally safe, but writting to it will bypass
   * important mechanisms such as modification tracking. Make sure you know
   * what you're doing, and consider whether other methods fit your needs.
   *
   * @return a reference to this context's KnowledgeMap
   **/
  KnowledgeMap& get_map_unsafe(void)
  {
    return map_;
  }

  /**
   * NOT THREAD SAFE!
   *
   * Retrieves a reference to the underlying KnowledgeMap. This is not
   * thread-safe. Only call this if you already hold this Context's lock.
   * Reading the map is then generally safe.
   *
   * Ensure you know what you're doing, and consider whether other
   * methods fit your needs.
   *
   * @return a reference to this context's KnowledgeMap
   **/
  const KnowledgeMap& get_map_unsafe(void) const
  {
    return map_;
  }

  template<typename Callable>
  auto invoke(const std::string& key, Callable&& callable,
      const KnowledgeUpdateSettings& settings = KnowledgeUpdateSettings())
      -> decltype(invoke_(
          std::forward<Callable>(callable), std::declval<KnowledgeRecord&>()))
  {
    MADARA_GUARD_TYPE guard(mutex_);
    auto ref = get_ref(key, settings);
    return invoke_(std::forward<Callable>(callable), *ref.get_record_unsafe());
  }

  template<typename Callable>
  auto invoke(const VariableReference& key, Callable&& callable,
      const KnowledgeUpdateSettings& settings = KnowledgeUpdateSettings())
      -> decltype(invoke_(
          std::forward<Callable>(callable), std::declval<KnowledgeRecord&>()))
  {
    MADARA_GUARD_TYPE guard(mutex_);
    (void)settings;
    return invoke_(std::forward<Callable>(callable), *key.get_record_unsafe());
  }

  template<typename Callable>
  auto invoke(const std::string& key, Callable&& callable,
      const KnowledgeReferenceSettings& settings = KnowledgeReferenceSettings())
      const -> decltype(invoke_(
          std::forward<Callable>(callable), std::declval<KnowledgeRecord&>()))
  {
    MADARA_GUARD_TYPE guard(mutex_);
    const KnowledgeRecord* ptr = with(key, settings);
    if(ptr)
    {
      return invoke_(std::forward<Callable>(callable), *ptr);
    }
    const KnowledgeRecord empty;
    return invoke_(std::forward<Callable>(callable), empty);
  }

  template<typename Callable>
  auto invoke(const VariableReference& key, Callable&& callable,
      const KnowledgeReferenceSettings& settings = KnowledgeReferenceSettings())
      const -> decltype(invoke_(
          std::forward<Callable>(callable), std::declval<KnowledgeRecord&>()))
  {
    MADARA_GUARD_TYPE guard(mutex_);
    (void)settings;
    return invoke_(std::forward<Callable>(callable),
        const_cast<const KnowledgeRecord&>(*key.get_record_unsafe()));
  }

  template<typename Callable>
  auto cinvoke(const std::string& key, Callable&& callable,
      const KnowledgeReferenceSettings& settings = KnowledgeReferenceSettings())
      const -> decltype(invoke_(
          std::forward<Callable>(callable), std::declval<KnowledgeRecord&>()))
  {
    return invoke(key, std::forward<Callable>(callable), settings);
  }

  template<typename Callable>
  auto cinvoke(const VariableReference& key, Callable&& callable,
      const KnowledgeReferenceSettings& settings = KnowledgeReferenceSettings())
      const -> decltype(invoke_(
          std::forward<Callable>(callable), std::declval<KnowledgeRecord&>()))
  {
    return invoke(key, std::forward<Callable>(callable), settings);
  }

protected:
private:
  /**
   * Changes variable to modified at current clock, and queues it to send,
   * even if it is a local that would not ordinarily be sent. Skips all
   * safety checks and variable expansions.
   * @param  ref       a reference to a variable in the knowledge base
   * @param  settings  the settings for referring to variables
   **/
  void mark_to_send_unsafe(VariableReference ref,
      const KnowledgeUpdateSettings& settings = KnowledgeUpdateSettings());

  /**
   * Changes variable to modified at current clock for the purposes of
   * checkpointing. Skips all safety checks and variable expansions.
   * @param  ref       a reference to a variable in the knowledge base
   * @param  settings  the settings for referring to variables
   **/
  void mark_to_checkpoint_unsafe(VariableReference ref,
      const KnowledgeUpdateSettings& settings = KnowledgeUpdateSettings());

  /**
   * method for marking a record modified and signaling changes
   * @param  ref       a reference to a variable in the knowledge base
   * @param  settings  settings for applying modification and signalling
   **/
  void mark_and_signal(VariableReference ref,
      const KnowledgeUpdateSettings& settings = KnowledgeUpdateSettings());

  template<typename... Args>
  int set_unsafe_impl(const VariableReference& variable,
      const KnowledgeUpdateSettings& settings, Args&&... args);

  template<typename... Args>
  int emplace_any_unsafe_impl(const VariableReference& variable,
      const KnowledgeUpdateSettings& settings, Args&&... args);

  template<typename T>
  int set_index_unsafe_impl(const VariableReference& variable, size_t index,
      T&& value,
      const KnowledgeUpdateSettings& settings = KnowledgeUpdateSettings());

  std::pair<KnowledgeMap::const_iterator, KnowledgeMap::const_iterator>
  get_prefix_range(const std::string& prefix) const;

  std::pair<KnowledgeMap::iterator, KnowledgeMap::iterator> get_prefix_range(
      const std::string& prefix);

  /// Hash table containing variable names and values.
  madara::knowledge::KnowledgeMap map_;
  mutable MADARA_LOCK_TYPE mutex_;
  mutable MADARA_CONDITION_TYPE changed_;
  std::vector<std::string> expansion_splitters_;
  mutable uint64_t clock_;
  mutable VariableReferenceMap changed_map_;
  mutable VariableReferenceMap local_changed_map_;

  /// map of function names to functions
  FunctionMap functions_;

  /// KaRL interpreter
  madara::expression::Interpreter* interpreter_;

  /// Logger for printing
  mutable logger::Logger* logger_;

  /// Streaming provider for saving all updates
  std::unique_ptr<BaseStreamer> streamer_ = nullptr;
};
}
}

#include "ThreadSafeContext.inl"

#endif
