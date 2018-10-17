#include "PeerDiscovery.h"
#include "madara/utility/Utility.h"

namespace utility = madara::utility;
typedef madara::knowledge::KnowledgeRecord::Integer Integer;

madara::filters::PeerDiscovery::PeerDiscovery(
    const std::string& prefix, knowledge::KnowledgeRecord::Integer heart_beat)
  : initialized_(false),
    prefix_(prefix),
    heart_beat_(heart_beat * 1000000000),
    last_clear_(0)
{
}

madara::filters::PeerDiscovery::~PeerDiscovery() {}

void madara::filters::PeerDiscovery::filter(
    knowledge::KnowledgeMap& /*records*/,
    const transport::TransportContext& transport_context,
    knowledge::Variables& vars)
{
  if(!initialized_)
  {
    peers_.set_name(prefix_, vars);
    initialized_ = true;
  }

  std::string originator(transport_context.get_originator());
  Integer cur_time = (Integer)transport_context.get_current_time();

  peers_.set(originator, cur_time);

  if(heart_beat_ > 0 && last_clear_ != cur_time)
  {
    std::vector<std::string> keys;
    peers_.sync_keys();

    peers_.keys(keys);

    for(size_t i = 0; i < keys.size(); ++i)
    {
      // if the current originator is old, erase it
      if(cur_time - peers_[keys[i]].to_integer() > heart_beat_)
      {
        peers_.erase(keys[i]);
      }
    }
  }
}
