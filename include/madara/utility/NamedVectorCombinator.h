#ifndef _MADARA_UTILITY_NAMED_VECTOR_COMBINATOR_H_
#define _MADARA_UTILITY_NAMED_VECTOR_COMBINATOR_H_

#include <map>
#include <set>
#include <vector>
#include <string>
#include <sstream>
#include <iterator>

#include "Utility.h"
#include "madara/exceptions/NameException.h"

namespace madara
{
namespace utility
{
/**
 * @class NamedVectorCombinator
 * @brief A helper class for combining named vectors of strings into vectors
 *        of unique strings.
 *        
 */
class NamedVectorCombinator
{
public:
  /**
   * Constructor
   **/
  NamedVectorCombinator() = default;

  /**
   * Reads a file containing strings separated by newlines into a named vector
   * @param name     the name of the mapped vector to import into
   * @param filename the name of the file to read in the newline-delimited
   *                 strings
   **/
  inline void from_file(const std::string& name, const std::string& filename)
  {
    named_vectors_[name] = string_to_vector(file_to_string(filename));
  }

  /**
   * Adds a vector of strings to the mapped vectors with a specific name
   * @param name     the name of the mapped vector to import into
   * @param input    a string containing newline-delimited strings to import
   **/
  inline void add(const std::string& name,
    const std::string& input)
  {
    named_vectors_[name] = string_to_vector(input);
  }

  /**
   * Adds a vector of strings to the mapped vectors with a specific name
   * @param name     the name of the mapped vector to import into
   * @param strings  a vector of strings to associate with the name
   **/
  inline void add(const std::string& name,
    const std::vector<std::string>& strings)
  {
    named_vectors_[name] = strings;
  }

  /**
   * Merges named vectors into a result vector. This is a useful function
   * for building prefix lists for CheckpointSettings and other classes
   * that have vectors of strings.
   * @param  named_vectors   a vector of named entities that you want to merge.
   *                         Each string should exist in the known map by
   *                         first calling from_file to create the lists
   * @param  result          the result of the merge
   * @param  throw_on_missing if true, throw an exception if the named entity
   *                         does not exist 
   * @param  make_unique     if true, make sure the resulting merge is unique
   * @param  clear_first     if true, clear the result before inserting
   * @throw  exceptions::NameException  if throw_on_missing and named vector
   *                         contains an unknown mapping, throw this exception
   **/ 
  inline void merge(const std::vector<std::string> & named_vectors,
    std::vector<std::string> & result, bool throw_on_missing = true,
    bool make_unique = true, bool clear_first = true)
  {
    std::set<std::string> unique_list;

    if(clear_first)
    {
      result.clear();
    }

    for(auto name : named_vectors)
    {
      auto found = named_vectors_.find(name);
      if(found != named_vectors_.end())
      {
        // if uniqueness is not important, then copy the list over
        if(!make_unique)
        {
          result.insert(
            result.end(), found->second.begin(), found->second.end());
        }
        // if uniqueness is important, use the set instead
        else
        {
          unique_list.insert(found->second.begin(), found->second.end());
        }
      }
      else if(throw_on_missing)
      {
        std::stringstream buffer;
        buffer << "madara::utility::NamedVectorCombinator: ";
        buffer << name << " could not be found in mapped named vectors.";
         
        throw exceptions::NameException(buffer.str());
      }
    }

    if(make_unique)
    {
      result.insert(result.end(), unique_list.begin(), unique_list.end());

        // in C++17, this works and reduces copying costs. May switch then.
        // std::make_move_iterator(unique_list.begin()),
        // std::make_move_iterator(unique_list.end()));
    }
  }

private:

  /// the mapping of named lists to their contents
  std::map <std::string, std::vector<std::string>> named_vectors_; 
};
}
}

#endif  // _MADARA_UTILITY_NAMED_VECTOR_COMBINATOR_H_
