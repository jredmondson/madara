#include "Transport_Settings.h"
#include "Fragmentation.h"

Madara::Transport::Settings::Settings () : 
        domains (DEFAULT_DOMAIN), 
        queue_length (DEFAULT_QUEUE_LENGTH), 
        deadline (DEFAULT_DEADLINE), 
        type (DEFAULT_TRANSPORT),
        max_fragment_size (62000),
        fragment_queue_length (5),
        reliability (DEFAULT_RELIABILITY),
        id (DEFAULT_ID),
        processes (DEFAULT_PROCESSES),
        on_data_received_logic (),
        delay_launch (false),
        never_exit (false),
        send_reduced_message_header (false),
        slack_time (0),

#ifdef _USE_CID_
        latency_enabled (DEFAULT_LATENCY_ENABLED),
        latency_timeout (DEFAULT_LATENCY_TIMEOUT),
        latency_default (DEFAULT_LATENCY),
        num_responses (0),
        num_summations (0),
        num_voters (0),
        num_votes_received (0),
        redeployment_percentage_allowed (DEFAULT_REDEPLOYMENT_PERCENTAGE),
#endif // _USE_CID_

        hosts ()
{
}

Madara::Transport::Settings::Settings (const Settings & settings) : 
        domains (settings.domains), 
        queue_length (settings.queue_length), 
        deadline (settings.deadline), 
        type (settings.type),
        max_fragment_size (settings.max_fragment_size),
        fragment_queue_length (settings.fragment_queue_length),
        reliability (settings.reliability),
        id (settings.id),
        processes (settings.processes),
        on_data_received_logic (settings.on_data_received_logic),
        delay_launch (settings.delay_launch),
        never_exit (settings.never_exit),
        send_reduced_message_header (settings.send_reduced_message_header),
        slack_time (settings.slack_time),

#ifdef _USE_CID_

        latency_enabled (settings.latency_enabled),
        latency_timeout (settings.latency_timeout),
        latency_default (settings.latency_default),
        latencies (settings.latencies),
        num_responses (0),
        num_summations (0),
        num_voters (settings.num_voters),
        num_votes_received (0),
        redeployment_percentage_allowed (
         settings.redeployment_percentage_allowed),
#endif // _USE_CID_
         hosts ()
{
  hosts.resize (settings.hosts.size ());
  for (unsigned int i = 0; i < settings.hosts.size (); ++i)
    hosts[i] = settings.hosts[i];
}

void
Madara::Transport::Settings::operator= (const Settings & settings)
{
  domains = settings.domains;
  queue_length = settings.queue_length;
  deadline = settings.deadline;
  type = settings.type;
  max_fragment_size = settings.max_fragment_size;
  fragment_queue_length = settings.fragment_queue_length;
  reliability = settings.reliability;
  id = settings.id;
  processes = settings.processes;

  on_data_received_logic = settings.on_data_received_logic;
  delay_launch = settings.delay_launch;
  never_exit = settings.never_exit;

  send_reduced_message_header = settings.send_reduced_message_header;
  slack_time = settings.slack_time;

#ifdef _USE_CID_
  latency_enabled = settings.latency_enabled;
  latency_timeout = settings.latency_timeout;
  latency_default = settings.latency_default;
  latencies = settings.latencies;
  num_responses = 0;
  num_summations = 0;
  num_voters = settings.num_voters;
  num_votes_received = 0;
  redeployment_percentage_allowed = 
    settings.redeployment_percentage_allowed;
#endif // _USE_CID_

  hosts.resize (settings.hosts.size ());
  for (unsigned int i = 0; i < settings.hosts.size (); ++i)
    hosts[i] = settings.hosts[i];
}

Madara::Transport::Settings::~Settings ()
{
  for (Originator_Fragment_Map::iterator originator = fragment_map.begin ();
       originator != fragment_map.end (); ++originator)
  {
    for (Clock_Fragment_Map::iterator clock = originator->second.begin ();
        clock != originator->second.end (); ++clock)
    {
      delete_fragments (clock->second);
    }
  }
}

