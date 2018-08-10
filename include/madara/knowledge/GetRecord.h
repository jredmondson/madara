/**
 * @file GetRecord.h
 * @author Sam Khalandovsky <sam.khalandovsky@shield.ai>
 *
 * This file provides functions to get KnowledgeRecord data without impilicit conversions
 **/

#ifndef _MADARA_KNOWLEDGE_GET_RECORD_H_
#define _MADARA_KNOWLEDGE_GET_RECORD_H_

#include <madara/knowledge/KnowledgeRecord.h>
#include <madara/knowledge/KnowledgeCast.h>
#include <madara/utility/SupportTest.h>

#include "madara/exceptions/MadaraException.h"

namespace madara
{
  namespace exceptions
  {
    /**
     * A base class for exceptions thrown by knowledge::get
     **/
    class InvalidGetException : public MadaraException
    {
    public:
      using MadaraException::MadaraException;
    };

    /**
     * Exception thrown when a attempting to retrieve a value from an empty record
     */
    class MissingValueException : public InvalidGetException
    {
    public:
      using InvalidGetException::InvalidGetException;
    };

    /**
     * Exception thrown when attempting to retrieve a value from a record of the wrong type
     */
    class MismatchedTypeException : public InvalidGetException
    {
    public:
      using InvalidGetException::InvalidGetException;
    };
  }
}

namespace madara { namespace knowledge {

#ifdef DOXYGEN

/**
 * @brief Check if a KnowledgeRecord type matches a specified type
 * 
 * @param t an instance of the type helper struct to infer the target type
 * @param in the KnowledgeRecord to read from
 * @return true if the types match
 */
template <typename T>
inline bool type_match(type<T> t, const KnowledgeRecord &kr);  
#else

template <typename T>
inline auto type_match(type<T>, const KnowledgeRecord &kr) -> 
  typename std::enable_if<std::is_integral<T>::value, bool>::type
{
  return kr.type() == KnowledgeRecord::INTEGER;
}

template <typename T>
inline auto type_match(type<T>, const KnowledgeRecord &kr) -> 
  typename std::enable_if<std::is_floating_point<T>::value, bool>::type
{
  return kr.type() == KnowledgeRecord::DOUBLE;
}

inline bool type_match(type<std::string>, const KnowledgeRecord &kr) 
{
  return kr.type() == KnowledgeRecord::STRING;
}

template<typename T>
inline auto type_match(type<std::vector<T>>, const KnowledgeRecord &kr) ->
  typename std::enable_if<std::is_integral<T>::value, bool>::type
{
  return kr.type() == KnowledgeRecord::INTEGER_ARRAY;
}

template<typename T>
inline auto type_match(type<std::vector<T>>, const KnowledgeRecord &kr) ->
  typename std::enable_if<std::is_floating_point<T>::value, bool>::type
{
  return kr.type() == KnowledgeRecord::DOUBLE_ARRAY;
}

#endif // DOXYGEN

/**
 * @brief Get the value of a KnowlegeRecord
 * 
 * INTEGER records can be retrieved as any integral type (bool, int, uint, etc)
 * DOUBLE records can be retrieved as any floating point type (double, float)
 * STRING records can be retrieved as std::string
 * INTEGER_ARRAY and DOUBLE_ARRAY can be retreived as an std::vector of integral 
 *    or floating point types respectively.
 * 
 * @throws MissingValueException if the Record in empty
 * @throws MismatchedTypeException if template type does not match stored type
 * 
 * @tparam T type of expected value
 * @param kr Record to read from
 * @return the stored value
 */
template <typename T>
inline T get(const KnowledgeRecord &kr)
{
  if(!kr.exists())
  {
    throw exceptions::MissingValueException("madara::knowledge::get: "
      "KnowledgeRecord does not exist");
  }
  if(!type_match(type<T>{}, kr))
  {
    throw exceptions::MismatchedTypeException("madara::knowledge::get: "
      "Expected type does not match the KnowledgeRecord's type (" + std::to_string(kr.type()) + ")");
  }

  return knowledge_cast<T>(kr);
}

} // namespace knowledge
} // namespace madara

#endif  // _MADARA_KNOWLEDGE_GET_RECORD_H_