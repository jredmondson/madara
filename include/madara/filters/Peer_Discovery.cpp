#include "Peer_Discovery.h"
#include "madara/utility/Utility.h"

namespace utility = Madara::Utility;
typedef Madara::Knowledge_Record::Integer   Integer;

Madara::Filters::Peer_Discovery::Peer_Discovery (
  const std::string & prefix,
  Knowledge_Record::Integer heart_beat)
  : initialized_ (false), prefix_ (prefix),
    heart_beat_ (heart_beat), last_clear_ (0)
{
}

Madara::Filters::Peer_Discovery::~Peer_Discovery ()
{
}


void
Madara::Filters::Peer_Discovery::filter (
  Knowledge_Map & records,
  const Transport::Transport_Context & transport_context,
  Knowledge_Engine::Variables & vars)
{
  if (!initialized_)
  {
    peers_.set_name (prefix_, vars);
  }

  std::string originator (transport_context.get_originator ());
  Integer cur_time = (Integer)transport_context.get_current_time ();

  peers_.set (originator, cur_time);

  if (heart_beat_ > 0 && last_clear_ != cur_time)
  {
    std::vector <std::string> keys;
    peers_.sync_keys ();

    peers_.keys (keys);

    for (size_t i = 0; i < keys.size (); ++i)
    {
      // if the current originator is old, erase it
      if (cur_time - peers_[keys[i]].to_integer () > heart_beat_)
      {
        peers_.erase (keys[i]);
      }
    }
  }
}
