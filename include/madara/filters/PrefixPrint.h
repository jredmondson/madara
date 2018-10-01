

#ifndef _MADARA_FILTERS_PREFIX_PRINT_H_
#define _MADARA_FILTERS_PREFIX_PRINT_H_

/**
 * @file PrefixPrint.h
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains a filter for printing only approved prefixes
 **/

#include <sstream>

#include "AggregateFilter.h"
#include "madara/knowledge/containers/StringVector.h"
#include "madara/utility/Utility.h"
#include "madara/transport/Transport.h"

namespace madara
{
namespace filters
{
/**
 * Filter for printing variables as they arrive and excluding prefixes
 * that are unwanted via a user-specified MADARA StringVector container
 **/
class MADARA_EXPORT PrefixPrint : public AggregateFilter
{
public:
  /**
   * Constructor
   * @param source_prefixes the static prefixes to use for printing. If
   *        empty, print all variables. If contains prefixes, only print
   *        variables that begin with contained prefixes
   **/
  PrefixPrint(const std::vector<std::string>& source_prefixes = {},
      bool print_verbose = true)
    : prefixes(source_prefixes), verbose(print_verbose)
  {
  }

  /**
   * Destructor
   **/
  virtual ~PrefixPrint() = default;

  /**
   * Prints records and transport context information
   * @param   records           the aggregate records vector
   * @param   transport_context describes transport information
   **/
  inline virtual void filter(knowledge::KnowledgeMap& records,
      const transport::TransportContext& transport_context,
      knowledge::Variables&)
  {
    std::stringstream buffer;

    if (verbose)
    {
      buffer << "PrefixFilter Arguments:\n";

      // Operation Type
      buffer << "  Operation Type:    ";
      buffer << transport::get_operation_type(
          transport_context.get_operation());
      buffer << "\n";

      // Send Bandwidth
      buffer << "  Send Bandwidth:    ";
      buffer << transport_context.get_send_bandwidth();
      buffer << " B/s\n";

      // Receive Bandwidth
      buffer << "  Receive Bandwidth: ";
      buffer << transport_context.get_receive_bandwidth();
      buffer << " B/s\n";

      // Update Time
      buffer << "  Update Time:       ";
      buffer << transport_context.get_message_time();
      buffer << "\n";

      // Current Time
      buffer << "  Current Time:      ";
      buffer << transport_context.get_current_time();
      buffer << "\n";

      // Knowledge Domain
      buffer << "  Knowledge Domain:  ";
      buffer << transport_context.get_domain();
      buffer << "\n";

      // Originator
      buffer << "  Originator:        ";
      buffer << transport_context.get_originator();
      buffer << "\n";

      buffer << "  Updates:\n";
    }

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
            break;
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
            "PrefixPrint::filter: "
            "not printing variable %s\n",
            record.first.c_str())
      }  // end not valid prefix
      else
      {
        // valid prefix so keep the record and proceed to next
        buffer << "    " << record.first << " = ";
        buffer << record.second.to_string() << "\n";
      }  // end valid prefix
    }    // end iteration over all filter records

    madara_logger_ptr_log(madara::logger::global_logger.get(),
        logger::LOG_ALWAYS, buffer.str().c_str());
  }  // end filter function

  /**
   * Prefixes to print. If empty, print all variables. If not empty,
   * print only variables with prefixes that exist in the vector.
   **/
  std::vector<std::string> prefixes;
  bool verbose;
};
}
}

#endif  // _MADARA_FILTERS_PREFIX_PRINT_H_
