

#ifndef _MADARA_FILTERS_VARIABLEMAP_H_
#define _MADARA_FILTERS_VARIABLEMAP_H_

/**
 * @file VariableMapFilter.h
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains a filter for printing only approved prefixes
 **/

#include <string>
#include <vector>
#include <map>
#include <sstream>

#include "AggregateFilter.h"
#include "madara/knowledge/containers/StringVector.h"
#include "madara/utility/Utility.h"
#include "madara/transport/Transport.h"
#include "madara/exceptions/FilterException.h"

namespace madara
{
namespace filters
{
/**
 * Filter for mapping variables in the KB into a filter's outgoing or
 * incoming messages
 **/
class MADARA_EXPORT VariableMapFilter : public AggregateFilter
{
public:
  /**
   * Processes mappings from a config file. Mappings should be in the
   * form of "var_in_kb -> new_var".
   * @param config   the configuration file contents to process
   **/
  inline void process_config(const std::string& config)
  {
    initialized_ = false;

    std::vector<std::string> splitters, tokens, pivots;

    splitters.push_back("->");
    splitters.push_back("\n");

    utility::tokenizer(config, splitters, tokens, pivots);

    if(tokens.size() % 2 == 0)
    {
      // valid configuration file
      for(size_t i = 0; i + 1 < tokens.size(); i += 2)
      {
        utility::strip_white_space(tokens[i]);
        utility::strip_white_space(tokens[i + 1]);

        if(tokens[i] == "" || tokens[i + 1] == "")
        {
          // invalid configuration file
          madara_logger_ptr_log(madara::logger::global_logger.get(),
              logger::LOG_ERROR,
              "VariableMapFilter::process_config: ERROR: "
              "configuration mappings has an empty map endpoint. "
              "Unable to process config.\n")

              throw exceptions::FilterException(
                  "VariableMapFilter::process_config: ERROR: "
                  "configuration mappings has an empty map endpoint. "
                  "Unable to process config.");
        }
        else
        {
          knowledge::VariableReference ref;
          // std::pair <std::string, knowledge::VariableReference> entry;
          // entry.first = tokens[i + 1];
          // entry.second = ref;

          map_[tokens[i]] = {tokens[i + 1], ref};
        }
      }
    }
    else
    {
      // invalid configuration file
      madara_logger_ptr_log(madara::logger::global_logger.get(),
          logger::LOG_ERROR,
          "VariableMapFilter::process_config: ERROR: "
          "configuration mappings are incorrect. tokens.size=%d."
          "Mappings must be in 'kb_var -> new_var' format, one each line. "
          "Unable to process config.\n",
          (int)tokens.size())

          throw exceptions::FilterException(
              "VariableMapFilter::process_config: ERROR: "
              "configuration mappings are incorrect. Unable to process "
              "config.");
    }
  }

  /**
   * Reads a config file and processes the contents. Mappings should be
   * in the form of "var_in_kb -> new_var".
   * @param filename  the name of the configuration file to process
   **/
  inline void read_config(const std::string& filename)
  {
    if(filename != "")
    {
      process_config(utility::file_to_string(filename));
    }
  }

  /**
   * Constructor
   * @param filename the filename of the config to load mappings from
   **/
  VariableMapFilter(const std::string& filename = "") : initialized_(false)
  {
    process_config(filename);
  }

  /**
   * Destructor
   **/
  virtual ~VariableMapFilter() = default;

  /**
   * Adds to the counter
   * @param   records           the aggregate records vector
   * @param   vars              context for querying current program state
   **/
  inline virtual void filter(knowledge::KnowledgeMap& records,
      const transport::TransportContext&, knowledge::Variables& vars)
  {
    // if we haven't setup valid VariableReferences yet, do so
    if(!initialized_)
    {
      std::map<std::string,
          std::pair<std::string, knowledge::VariableReference>>::iterator i =
          map_.begin();

      for(; i != map_.end(); ++i)
      {
        // set the ref for the kb local variable
        i->second.second = vars.get_ref(i->first);
      }
    }

    // iterate through the map and add the local variables to records
    for(auto entry : map_)
    {
      records[entry.second.first] = vars.get(entry.second.second);
    }
  }  // end filter function

protected:
  bool initialized_;

  /**
   * Map of local KB variable to a variable to add and a VariableReference
   * to the local KB variable
   **/
  std::map<std::string, std::pair<std::string, knowledge::VariableReference>>
      map_;
};
}
}

#endif  // _MADARA_FILTERS_VARIABLEMAP_H_
