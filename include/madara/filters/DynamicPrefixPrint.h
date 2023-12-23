

#ifndef _MADARA_FILTERS_DYNAMIC_PREFIX_PRINT_H_
#define _MADARA_FILTERS_DYNAMIC_PREFIX_PRINT_H_

/**
 * @file DynamicPrefixPrint.h
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
class DynamicPrefixPrint : public AggregateFilter
{
public:
  /**
   * Constructor
   * @param string_vector_name   the name of the string vector in the KB
   *                             to use for prefix information
   **/
  DynamicPrefixPrint(const std::string& string_vector_name = ".prefixes")
    : initialized_(false), name(string_vector_name), prefixes_()
  {
  }

  /**
   * Destructor
   **/
  virtual ~DynamicPrefixPrint() = default;

  /**
   * Prints records and transport context information
   * @param   records           the aggregate records vector
   * @param   transport_context the transport context where metadata is stored
   * @param   vars              context for querying current program state
   **/
  inline virtual void filter(knowledge::KnowledgeMap& records,
      const transport::TransportContext& transport_context,
      knowledge::Variables& vars)
  {
    std::stringstream buffer;
    buffer << "DynamicPrefixFilter Arguments:\n";

    // Operation Type
    buffer << "  Operation Type:    ";
    buffer << transport::get_operation_type(transport_context.get_operation());
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
            "DynamicPrefixPrint::filter: "
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

    vars.print(buffer.str(), 0);
  }  // end filter function

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

protected:
  /**
   * A map of discovered peers
   **/
  knowledge::containers::StringVector prefixes_;
};
}
}

#endif  // _MADARA_FILTERS_DYNAMIC_PREFIX_PRINT_H_
