#include "EndpointDiscovery.h"
#include "madara/utility/Utility.h"
#include "madara/logger/GlobalLogger.h"

namespace utility = madara::utility;
typedef madara::knowledge::KnowledgeRecord::Integer Integer;

madara::filters::EndpointDiscovery::EndpointDiscovery(
    const std::string& prefix, knowledge::KnowledgeRecord::Integer heart_beat)
  : initialized_(false),
    prefix_(prefix),
    heart_beat_(heart_beat * 1000000000),
    last_clear_(0)
{
}

madara::filters::EndpointDiscovery::~EndpointDiscovery() {}

void madara::filters::EndpointDiscovery::filter(
    knowledge::KnowledgeMap& records,
    const transport::TransportContext& transport_context,
    knowledge::Variables& vars)
{
  if (!initialized_)
  {
    endpoints_.set_name(prefix_, vars);
    initialized_ = true;
  }

  madara_logger_log(vars.get_context()->get_logger(), logger::LOG_MAJOR,
      "EndpointDiscovery::filter:"
      " Processing a new update with %d records\n",
      (int)records.size());

  std::string endpoint(transport_context.get_endpoint());
  Integer cur_time = (Integer)transport_context.get_current_time();

  endpoints_.set(endpoint, cur_time);

  if (heart_beat_ > 0 && last_clear_ != cur_time)
  {
    std::vector<std::string> keys;
    endpoints_.sync_keys();

    endpoints_.keys(keys);

    for (size_t i = 0; i < keys.size(); ++i)
    {
      // if the current endpoint is old, erase it
      if (cur_time - endpoints_[keys[i]].to_integer() > heart_beat_)
      {
        madara_logger_log(vars.get_context()->get_logger(), logger::LOG_MINOR,
            "EndpointDiscovery::filter:"
            " Erasing endpoint %s\n",
            keys[i].c_str());

        endpoints_.erase(keys[i]);
      }
    }

    last_clear_ = cur_time;
  }
}
