#include "EndpointClear.h"
#include "madara/utility/Utility.h"
#include "madara/logger/GlobalLogger.h"

namespace utility = madara::utility;
typedef madara::knowledge::KnowledgeRecord::Integer Integer;

madara::filters::EndpointClear::EndpointClear(const std::string& prefix)
  : initialized_(false), prefix_(prefix)
{
}

madara::filters::EndpointClear::~EndpointClear() {}

void madara::filters::EndpointClear::filter(knowledge::KnowledgeMap& records,
    const transport::TransportContext& transport_context,
    knowledge::Variables& vars)
{
  if (!initialized_) {
    knowledge::KnowledgeUpdateSettings settings(true);
    endpoints_.set_settings(settings);
    endpoints_.set_name(prefix_, vars);
    initialized_ = true;
  }

  endpoints_.sync_keys();

  madara_logger_log(vars.get_context()->get_logger(), logger::LOG_MAJOR,
      "EndpointClear::filter:"
      " Processing a new update from %s with %d records\n",
      transport_context.get_originator().c_str(), (int)records.size());

  // if a trusted originator has been added, and this is the originator
  if (trusted_originators_.size() != 0 &&
      (trusted_originators_.find(transport_context.get_originator()) !=
          trusted_originators_.end())) {
    madara_logger_log(vars.get_context()->get_logger(), logger::LOG_MAJOR,
        "EndpointClear::filter:"
        " Sender is trusted. Clearing endpoints.\n");

    // then clear the endpoints
    endpoints_.clear();

    if (vars.get_context()->get_logger().get_level() >= logger::LOG_DETAILED) {
      vars.print();
    }
  } else {
    bool found = false;
    const std::string prefix(endpoints_.get_name());

    if (prefix != "") {
      // iterate through all records looking for the common prefix
      for (knowledge::KnowledgeMap::const_iterator i = records.begin();
           i != records.end() && !found; ++i) {
        // if we've found the prefix, then clear the endpoints
        if (utility::begins_with(i->first, prefix)) {
          madara_logger_log(vars.get_context()->get_logger(), logger::LOG_MAJOR,
              "EndpointClear::filter:"
              " Prefix is match. Clearing endpoints.\n");

          endpoints_.clear();
          found = true;

          if (vars.get_context()->get_logger().get_level() >=
              logger::LOG_DETAILED) {
            vars.print();
          }
        }
        // if we're past where the prefix would be, quit
        else if (prefix[0] < i->first[0]) {
          madara_logger_log(vars.get_context()->get_logger(), logger::LOG_MAJOR,
              "EndpointClear::filter:"
              " Prefix is not a match. Not clearing endpoints.\n");

          found = true;
        }
      }
    }
  }
}

void madara::filters::EndpointClear::add_trusted_originator(
    const std::vector<std::string>& originators)
{
  for (std::vector<std::string>::const_iterator i = originators.begin();
       i != originators.end(); ++i) {
    trusted_originators_[*i] = true;
  }
}

void madara::filters::EndpointClear::add_trusted_originator(
    const std::string& originator)
{
  trusted_originators_[originator] = true;
}

void madara::filters::EndpointClear::clear_trusted_originators(void)
{
  trusted_originators_.clear();
}

void madara::filters::EndpointClear::set_prefix(const std::string& prefix)
{
  prefix_ = prefix;
}
