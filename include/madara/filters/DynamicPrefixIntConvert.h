

#ifndef _MADARA_FILTERS_DYNAMIC_PREFIX_INTCONVERT_H_
#define _MADARA_FILTERS_DYNAMIC_PREFIX_INTCONVERT_H_

/**
 * @file DynamicPrefixIntConvert.h
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains a filter for printing only approved prefixes
 **/

#include <sstream>

#include "AggregateFilter.h"
#include "madara/knowledge/containers/StringVector.h"
#include "madara/utility/Utility.h"

namespace madara
{
namespace filters
{
/**
 * Filter for integerizing variables (usually before sending)
 **/
class DynamicPrefixIntConvert : public AggregateFilter
{
public:
  /**
   * Constructor
   * @param string_vector_name   the name of the string vector in the KB
   *                             to use for prefix information
   * @param new_value            the value to change matching prefix
   *                             variables to
   **/
  DynamicPrefixIntConvert(const std::string& string_vector_name = ".prefixes",
      knowledge::KnowledgeRecord::Integer new_value = 0)
    : initialized_(false),
      name(string_vector_name),
      value(new_value),
      prefixes_()
  {
  }

  /**
   * Destructor
   **/
  virtual ~DynamicPrefixIntConvert() = default;

  /**
   * Converts records to integers
   * @param   records           the aggregate records vector
   * @param   vars              context for querying current program state
   **/
  inline virtual void filter(knowledge::KnowledgeMap& records,
      const transport::TransportContext&, knowledge::Variables& vars)
  {
    // local copy (fast access to prefix vector)
    std::vector<std::string> prefixes;

    if (!initialized_)
    {
      // set the initial vector up
      prefixes_.set_name(name, vars);
      initialized_ = true;
    }
    else
    {
      // resize for what the current vector is indicating
      prefixes_.resize();
    }

    // copy the knowledge base prefixes to a STL vector for speed
    prefixes_.copy_to(prefixes);

    // because of the usage of erase, don't auto inc record in for loop
    for (auto record : records)
    {
      // check for valid prefix
      bool accepted_prefix = false;

      if (prefixes.size() > 0)
      {
        for (auto prefix : prefixes)
        {
          if (utility::begins_with(record.first, prefix))
          {
            accepted_prefix = true;
          }
        }
      }
      else
      {
        accepted_prefix = true;
      }

      // if not valid, remove the record and update iterator
      if (!accepted_prefix)
      {
        madara_logger_ptr_log(madara::logger::global_logger.get(),
            logger::LOG_MAJOR,
            "DynamicPrefixIntConvert::filter: "
            "not integerizing variable %s\n",
            record.first.c_str())
      }  // end not valid prefix
      else
      {
        // valid prefix so set the value
        record.second = value;
      }  // end valid prefix
    }    // end iteration over all filter records
  }      // end filter function

private:
  /**
   * Tracks if the prefix vector has been initialized
   **/
  bool initialized_;

public:
  /**
   * The string vector name to bind to inside the knowledge base
   **/
  std::string name;

  /// integer to convert all valid records to
  knowledge::KnowledgeRecord::Integer value;

protected:
  /**
   * A map of discovered peers
   **/
  knowledge::containers::StringVector prefixes_;
};
}
}

#endif  // _MADARA_FILTERS_DYNAMIC_PREFIX_INTCONVERT_H_
