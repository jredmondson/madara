#include "TransportSettings.h"
#include "Fragmentation.h"
#include "madara/knowledge/KnowledgeBase.h"
#include "madara/knowledge/containers/StringVector.h"

namespace containers = madara::knowledge::containers;
typedef madara::knowledge::KnowledgeRecord::Integer  Integer;

madara::transport::Settings::Settings () : 
  domains (DEFAULT_DOMAIN), 
  read_threads (1),
  queue_length (DEFAULT_QUEUE_LENGTH), 
  deadline (DEFAULT_DEADLINE), 
  type (DEFAULT_TRANSPORT),
  max_fragment_size (62000),
  resend_attempts (-1),
  fragment_queue_length (5),
  reliability (DEFAULT_RELIABILITY),
  id (DEFAULT_ID),
  processes (DEFAULT_PROCESSES),
  on_data_received_logic (),
  delay_launch (false),
  never_exit (false),
  send_reduced_message_header (false),
  slack_time (0),
  read_thread_hertz (0.0),

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

  hosts (),
  no_sending (false),
  no_receiving (false)
{
}

madara::transport::Settings::Settings (const Settings & settings) : 
        domains (settings.domains), 
        read_threads (settings.read_threads),
        queue_length (settings.queue_length), 
        deadline (settings.deadline), 
        type (settings.type),
        max_fragment_size (settings.max_fragment_size),
        resend_attempts (settings.resend_attempts),
        fragment_queue_length (settings.fragment_queue_length),
        reliability (settings.reliability),
        id (settings.id),
        processes (settings.processes),
        on_data_received_logic (settings.on_data_received_logic),
        delay_launch (settings.delay_launch),
        never_exit (settings.never_exit),
        send_reduced_message_header (settings.send_reduced_message_header),
        slack_time (settings.slack_time),
        read_thread_hertz (settings.read_thread_hertz),

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
         hosts (),
         no_sending (settings.no_sending),
         no_receiving (settings.no_receiving)
{
  hosts.resize (settings.hosts.size ());
  for (unsigned int i = 0; i < settings.hosts.size (); ++i)
    hosts[i] = settings.hosts[i];
}

void
madara::transport::Settings::operator= (const Settings & settings)
{
  read_threads = settings.read_threads;
  domains = settings.domains;
  queue_length = settings.queue_length;
  deadline = settings.deadline;
  type = settings.type;
  max_fragment_size = settings.max_fragment_size;
  resend_attempts = settings.resend_attempts;
  fragment_queue_length = settings.fragment_queue_length;
  reliability = settings.reliability;
  id = settings.id;
  processes = settings.processes;

  on_data_received_logic = settings.on_data_received_logic;
  delay_launch = settings.delay_launch;
  never_exit = settings.never_exit;

  send_reduced_message_header = settings.send_reduced_message_header;
  slack_time = settings.slack_time;
  read_thread_hertz = settings.read_thread_hertz;

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

  no_sending = settings.no_sending;
  no_receiving = settings.no_receiving;
}

madara::transport::Settings::~Settings ()
{
  for (OriginatorFragmentMap::iterator originator = fragment_map.begin ();
       originator != fragment_map.end (); ++originator)
  {
    for (ClockFragmentMap::iterator clock = originator->second.begin ();
        clock != originator->second.end (); ++clock)
    {
      delete_fragments (clock->second);
    }
  }
}


/**
* Loads the settings from a file
* @param  filename    the file to load from
**/
void
madara::transport::Settings::load (const std::string filename)
{
  knowledge::KnowledgeBase knowledge;
  knowledge.load_context (filename);

  read_threads = (uint32_t) knowledge.get ("transport.read_threads").to_integer ();
  domains = knowledge.get ("transport.domains").to_string ();
  queue_length = (uint32_t)knowledge.get ("transport.queue_length").to_integer ();
  deadline = (uint32_t)knowledge.get ("transport.deadline").to_integer ();
  type = (uint32_t)knowledge.get ("transport.type").to_integer ();
  max_fragment_size = (uint32_t)knowledge.get ("transport.max_fragment_size").to_integer ();
  resend_attempts = (uint32_t)knowledge.get ("transport.resend_attempts").to_integer ();
  fragment_queue_length = (uint32_t)knowledge.get ("transport.fragment_queue_length").to_integer ();
  reliability = (uint32_t)knowledge.get ("transport.reliability").to_integer ();
  id = (uint32_t)knowledge.get ("transport.id").to_integer ();
  processes = (uint32_t)knowledge.get ("transport.processes").to_integer ();

  on_data_received_logic = knowledge.get ("transport.on_data_received_logic").to_string ();
  delay_launch = knowledge.get ("transport.delay_launch").is_true ();
  never_exit = knowledge.get ("transport.never_exit").is_true ();

  send_reduced_message_header = knowledge.get ("transport.send_reduced_message_header").is_true ();
  slack_time = knowledge.get ("transport.slack_time").to_double ();
  read_thread_hertz = knowledge.get ("transport.read_thread_hertz").to_double ();

  containers::StringVector kb_hosts ("transport.hosts", knowledge);

  hosts.resize (kb_hosts.size ());
  for (unsigned int i = 0; i < hosts.size (); ++i)
    hosts[i] = kb_hosts[i];

  no_sending = knowledge.get ("transport.no_sending").is_true ();
  no_receiving = knowledge.get ("transport.no_receiving").is_true ();
}

void
madara::transport::Settings::save (const std::string filename) const
{
  knowledge::KnowledgeBase knowledge;

  containers::StringVector kb_hosts ("transport.hosts", knowledge,
    (int)hosts.size ());

  knowledge.set ("transport.read_threads", Integer (read_threads));
  knowledge.set ("transport.domains", domains);
  knowledge.set ("transport.queue_length", Integer (queue_length));
  knowledge.set ("transport.deadline", Integer (deadline));
  knowledge.set ("transport.type", Integer (type));
  knowledge.set ("transport.max_fragment_size", Integer (max_fragment_size));
  knowledge.set ("transport.resend_attempts", Integer (resend_attempts));
  knowledge.set ("transport.fragment_queue_length",
    Integer (fragment_queue_length));
  knowledge.set ("transport.reliability", Integer (reliability));
  knowledge.set ("transport.id", Integer (id));
  knowledge.set ("transport.processes", Integer (processes));

  knowledge.set ("transport.on_data_received_logic", on_data_received_logic);
  knowledge.set ("transport.delay_launch", Integer (delay_launch));
  knowledge.set ("transport.never_exit", Integer (never_exit));

  knowledge.set ("transport.send_reduced_message_header",
    Integer (send_reduced_message_header));
  knowledge.set ("transport.slack_time", slack_time);
  knowledge.set ("transport.read_thread_hertz", read_thread_hertz);

  for (size_t i = 0; i < hosts.size (); ++i)
    kb_hosts.set (i, hosts[i]);

  knowledge.set ("transport.no_sending", Integer (no_sending));
  knowledge.set ("transport.no_receiving", Integer (no_receiving));

  knowledge.save_context (filename);
}
