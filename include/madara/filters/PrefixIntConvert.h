

#ifndef _MADARA_FILTERS_PREFIX_INTCONVERT_H_
#define _MADARA_FILTERS_PREFIX_INTCONVERT_H_

/**
 * @file PrefixIntConvert.h
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
 * Filter for printing variables as they arrive and excluding prefixes
 * that are unwanted via a user-specified MADARA StringVector container
 **/
class MADARA_EXPORT PrefixIntConvert : public AggregateFilter
{
public:
  /**
   * Constructor
   * @param source_prefixes the static prefixes to use for printing. If
   *        empty, print all variables. If contains prefixes, only print
   *        variables that begin with contained prefixes
   * @param new_value            the value to change matching prefix
   *                             variables to
   **/
  PrefixIntConvert(const std::vector<std::string>& source_prefixes = {},
      knowledge::KnowledgeRecord::Integer new_value = 0)
    : value(new_value), prefixes(source_prefixes)
  {
  }

  /**
   * Destructor
   **/
  virtual ~PrefixIntConvert() = default;

  /**
   * Converts the records into integer values
   * @param   records           the aggregate records vector
   * @param   vars              context for querying current program state
   **/
  inline virtual void filter(knowledge::KnowledgeMap& records,
      const transport::TransportContext&, knowledge::Variables&)
  {
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
            "PrefixIntConvert::filter: "
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

  /// integer to convert all valid records to
  knowledge::KnowledgeRecord::Integer value;

  /**
   * Prefixes to print. If empty, print all variables. If not empty,
   * print only variables with prefixes that exist in the vector.
   **/
  std::vector<std::string> prefixes;
};
}
}

#endif  // _MADARA_FILTERS_PREFIX_INTCONVERT_H_
