#include "TransportSettings.h"
#include "Fragmentation.h"
#include "madara/knowledge/KnowledgeBase.h"
#include "madara/knowledge/containers/StringVector.h"
#include "madara/knowledge/containers/Map.h"

namespace containers = madara::knowledge::containers;
typedef madara::knowledge::KnowledgeRecord::Integer Integer;

std::string madara::transport::types_to_string(int id)
{
  if (NO_TRANSPORT == id)
  {
    return "None";
  }
  if (SPLICE == id)
  {
    return "Splice DDS";
  }
  if (NDDS == id)
  {
    return "RTI DDS";
  }
  if (UDP == id)
  {
    return "UDP";
  }
  if (TCP == id)
  {
    return "TCP (unsupported)";
  }
  if (MULTICAST == id)
  {
    return "UDP Multicast";
  }
  if (BROADCAST == id)
  {
    return "UDP Broadcast";
  }
  if (REGISTRY_SERVER == id)
  {
    return "UDP Registry Server";
  }
  if (REGISTRY_CLIENT == id)
  {
    return "UDP Registry Client";
  }
  if (ZMQ == id)
  {
    return "0MQ";
  }

  // otherwise, it's a custom transport
  return "Custom";
}

madara::transport::TransportSettings::TransportSettings(
    const TransportSettings& settings)
  : write_domain(settings.write_domain),
    read_threads(settings.read_threads),
    queue_length(settings.queue_length),
    type(settings.type),
    max_fragment_size(settings.max_fragment_size),
    resend_attempts(settings.resend_attempts),
    fragment_queue_length(settings.fragment_queue_length),
    reliability(settings.reliability),
    id(settings.id),
    processes(settings.processes),
    on_data_received_logic(settings.on_data_received_logic),
    delay_launch(settings.delay_launch),
    never_exit(settings.never_exit),
    send_reduced_message_header(settings.send_reduced_message_header),
    slack_time(settings.slack_time),
    read_thread_hertz(settings.read_thread_hertz),
    max_send_hertz(settings.max_send_hertz),
    hosts(),
    no_sending(settings.no_sending),
    no_receiving(settings.no_receiving),
    send_history(settings.send_history),
    debug_to_kb_prefix(settings.debug_to_kb_prefix),
    read_domains_(settings.read_domains_)
{
  hosts.resize(settings.hosts.size());
  for (unsigned int i = 0; i < settings.hosts.size(); ++i)
    hosts[i] = settings.hosts[i];
}

void madara::transport::TransportSettings::operator=(
    const TransportSettings& settings)
{
  read_threads = settings.read_threads;
  write_domain = settings.write_domain;
  read_domains_ = settings.read_domains_;
  queue_length = settings.queue_length;
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
  max_send_hertz = settings.max_send_hertz;

  hosts.resize(settings.hosts.size());
  for (unsigned int i = 0; i < settings.hosts.size(); ++i)
    hosts[i] = settings.hosts[i];

  no_sending = settings.no_sending;
  no_receiving = settings.no_receiving;

  send_history = settings.send_history;

  debug_to_kb_prefix = settings.debug_to_kb_prefix;
}

madara::transport::TransportSettings::~TransportSettings()
{
  for (OriginatorFragmentMap::iterator originator = fragment_map.begin();
       originator != fragment_map.end(); ++originator)
  {
    for (ClockFragmentMap::iterator clock = originator->second.begin();
         clock != originator->second.end(); ++clock)
    {
      delete_fragments(clock->second);
    }
  }

  hosts.clear();
}

void madara::transport::TransportSettings::add_host(
  const std::string & host)
{
  hosts.push_back(host);
}

void madara::transport::TransportSettings::clear_hosts()
{
  hosts.clear();
}

void madara::transport::TransportSettings::load(
    const std::string& filename, const std::string& prefix)
{
  knowledge::KnowledgeBase knowledge;
  knowledge.load_context(filename);
  
  knowledge::KnowledgeRecord value;

  value = knowledge.get(prefix + ".read_threads");
  if (value.exists())
  {
    read_threads = (uint32_t)value.to_integer();
  }
  
  value = knowledge.get(prefix + ".write_domain");
  if (value.exists())
  {
    write_domain = value.to_string();
  }
  
  value = knowledge.get(prefix + ".queue_length");
  if (value.exists())
  {
    queue_length = (uint32_t)value.to_integer();
  }
  
  value = knowledge.get(prefix + ".type");
  if (value.exists())
  {
    type = (uint32_t)value.to_integer();
  }
  
  value = knowledge.get(prefix + ".max_fragment_size");
  if (value.exists())
  {
    max_fragment_size = (uint32_t)value.to_integer();
  }
  
  value = knowledge.get(prefix + ".resend_attempts");
  if (value.exists())
  {
    resend_attempts = (uint32_t)value.to_integer();
  }
  
  value = knowledge.get(prefix + ".fragment_queue_length");
  if (value.exists())
  {
    fragment_queue_length = (uint32_t)value.to_integer();
  }
  
  value = knowledge.get(prefix + ".reliability");
  if (value.exists())
  {
    reliability = (uint32_t)value.to_integer();
  }
  
  value = knowledge.get(prefix + ".id");
  if (value.exists())
  {
    id = (uint32_t)value.to_integer();
  }
  
  value = knowledge.get(prefix + ".processes");
  if (value.exists())
  {
    processes = (uint32_t)value.to_integer();
  }
  
  value = knowledge.get(prefix + ".on_data_received_logic");
  if (value.exists())
  {
    on_data_received_logic = value.to_string();
  }
  
  value = knowledge.get(prefix + ".delay_launch");
  if (value.exists())
  {
    delay_launch = value.is_true();
  }
  
  value = knowledge.get(prefix + ".never_exit");
  if (value.exists())
  {
    never_exit = value.is_true();
  }
  
  value = knowledge.get(prefix + ".send_reduced_message_header");
  if (value.exists())
  {
    send_reduced_message_header = value.is_true();
  }
  
  value = knowledge.get(prefix + ".slack_time");
  if (value.exists())
  {
    slack_time = value.to_double();
  }
  
  value = knowledge.get(prefix + ".max_send_hertz");
  if (value.exists())
  {
    max_send_hertz = value.to_double();
  }
  
  value = knowledge.get(prefix + ".hosts.size");
  if (value.exists())
  {
    containers::StringVector kb_hosts(prefix + ".hosts", knowledge);
    
    hosts.resize(kb_hosts.size());
    for (unsigned int i = 0; i < hosts.size(); ++i)
      hosts[i] = kb_hosts[i];

  }

  containers::Map kb_read_domains(prefix + ".read_domains", knowledge);

  std::vector<std::string> keys;
  kb_read_domains.keys(keys);

  for (unsigned int i = 0; i < keys.size(); ++i)
  {
    read_domains_[keys[i]] = 1;
  }
  
  value = knowledge.get(prefix + ".no_sending");
  if (value.exists())
  {
    no_sending = value.is_true();
  }
  
  value = knowledge.get(prefix + ".no_receiving");
  if (value.exists())
  {
    no_receiving = value.is_true();
  }
  
  value = knowledge.get(prefix + ".debug_to_kb_prefix");
  if (value.exists())
  {
    debug_to_kb_prefix = value.is_true();
  }
}

void madara::transport::TransportSettings::load_text(
    const std::string& filename, const std::string& prefix)
{
  knowledge::KnowledgeBase knowledge;

#ifndef _MADARA_NO_KARL_

  knowledge.evaluate(madara::utility::file_to_string(filename));

#endif // end karl support

  knowledge::KnowledgeRecord value;

  value = knowledge.get(prefix + ".read_threads");
  if (value.exists())
  {
    read_threads = (uint32_t)value.to_integer();
  }
  
  value = knowledge.get(prefix + ".write_domain");
  if (value.exists())
  {
    write_domain = value.to_string();
  }
  
  value = knowledge.get(prefix + ".queue_length");
  if (value.exists())
  {
    queue_length = (uint32_t)value.to_integer();
  }
  
  value = knowledge.get(prefix + ".type");
  if (value.exists())
  {
    type = (uint32_t)value.to_integer();
  }
  
  value = knowledge.get(prefix + ".max_fragment_size");
  if (value.exists())
  {
    max_fragment_size = (uint32_t)value.to_integer();
  }
  
  value = knowledge.get(prefix + ".resend_attempts");
  if (value.exists())
  {
    resend_attempts = (uint32_t)value.to_integer();
  }
  
  value = knowledge.get(prefix + ".fragment_queue_length");
  if (value.exists())
  {
    fragment_queue_length = (uint32_t)value.to_integer();
  }
  
  value = knowledge.get(prefix + ".reliability");
  if (value.exists())
  {
    reliability = (uint32_t)value.to_integer();
  }
  
  value = knowledge.get(prefix + ".id");
  if (value.exists())
  {
    id = (uint32_t)value.to_integer();
  }
  
  value = knowledge.get(prefix + ".processes");
  if (value.exists())
  {
    processes = (uint32_t)value.to_integer();
  }
  
  value = knowledge.get(prefix + ".on_data_received_logic");
  if (value.exists())
  {
    on_data_received_logic = value.to_string();
  }
  
  value = knowledge.get(prefix + ".delay_launch");
  if (value.exists())
  {
    delay_launch = value.is_true();
  }
  
  value = knowledge.get(prefix + ".never_exit");
  if (value.exists())
  {
    never_exit = value.is_true();
  }
  
  value = knowledge.get(prefix + ".send_reduced_message_header");
  if (value.exists())
  {
    send_reduced_message_header = value.is_true();
  }
  
  value = knowledge.get(prefix + ".slack_time");
  if (value.exists())
  {
    slack_time = value.to_double();
  }
  
  value = knowledge.get(prefix + ".read_thread_hertz");
  if (value.exists())
  {
    read_thread_hertz = value.to_double();
  }
  
  value = knowledge.get(prefix + ".max_send_hertz");
  if (value.exists())
  {
    max_send_hertz = value.to_double();
  }
  
  value = knowledge.get(prefix + ".hosts.size");
  if (value.exists())
  {
    containers::StringVector kb_hosts(prefix + ".hosts", knowledge);
    
    hosts.resize(kb_hosts.size());
    for (unsigned int i = 0; i < hosts.size(); ++i)
      hosts[i] = kb_hosts[i];

  }

  containers::Map kb_read_domains(prefix + ".read_domains", knowledge);

  std::vector<std::string> keys;
  kb_read_domains.keys(keys);

  for (unsigned int i = 0; i < keys.size(); ++i)
  {
    read_domains_[keys[i]] = 1;
  }
  
  value = knowledge.get(prefix + ".no_sending");
  if (value.exists())
  {
    no_sending = value.is_true();
  }
  
  value = knowledge.get(prefix + ".no_receiving");
  if (value.exists())
  {
    no_receiving = value.is_true();
  }
  
  value = knowledge.get(prefix + ".debug_to_kb_prefix");
  if (value.exists())
  {
    debug_to_kb_prefix = value.is_true();
  }
}

void madara::transport::TransportSettings::save(
    const std::string& filename, const std::string& prefix) const
{
  knowledge::KnowledgeBase knowledge;

  // load what exists at the file so we can append/overwrite it
  knowledge.load_context(madara::utility::file_to_string(filename));

  containers::StringVector kb_hosts(
      prefix + ".hosts", knowledge, (int)hosts.size());

  knowledge.set(prefix + ".read_threads", Integer(read_threads));
  knowledge.set(prefix + ".write_domain", write_domain);
  knowledge.set(prefix + ".queue_length", Integer(queue_length));
  knowledge.set(prefix + ".type", Integer(type));
  knowledge.set(prefix + ".max_fragment_size", Integer(max_fragment_size));
  knowledge.set(prefix + ".resend_attempts", Integer(resend_attempts));
  knowledge.set(
      prefix + ".fragment_queue_length", Integer(fragment_queue_length));
  knowledge.set(prefix + ".reliability", Integer(reliability));
  knowledge.set(prefix + ".id", Integer(id));
  knowledge.set(prefix + ".processes", Integer(processes));

  knowledge.set(prefix + ".on_data_received_logic", on_data_received_logic);
  knowledge.set(prefix + ".delay_launch", Integer(delay_launch));
  knowledge.set(prefix + ".never_exit", Integer(never_exit));

  knowledge.set(prefix + ".send_reduced_message_header",
      Integer(send_reduced_message_header));
  knowledge.set(prefix + ".slack_time", slack_time);
  knowledge.set(prefix + ".read_thread_hertz", read_thread_hertz);
  knowledge.set(prefix + ".max_send_hertz", max_send_hertz);

  for (size_t i = 0; i < hosts.size(); ++i)
    kb_hosts.set(i, hosts[i]);

  knowledge.set(prefix + ".no_sending", Integer(no_sending));
  knowledge.set(prefix + ".no_receiving", Integer(no_receiving));
  knowledge.set(prefix + ".debug_to_kb_prefix", debug_to_kb_prefix);

  knowledge::containers::Map kb_read_domains(
      prefix + ".read_domains", knowledge);
  for (std::map<std::string, int>::const_iterator i = read_domains_.begin();
       i != read_domains_.end(); ++i)
  {
    kb_read_domains.set(
        i->first, (knowledge::KnowledgeRecord::Integer)i->second);
  }

  knowledge.save_context(filename);
}

void madara::transport::TransportSettings::save_text(
    const std::string& filename, const std::string& prefix) const
{
  knowledge::KnowledgeBase knowledge;

  // load what exists at the file so we can append/overwrite it
#ifndef _MADARA_NO_KARL_

  knowledge.evaluate(madara::utility::file_to_string(filename));

#endif // end karl support


  containers::StringVector kb_hosts(
      prefix + ".hosts", knowledge, (int)hosts.size());

  knowledge.set(prefix + ".read_threads", Integer(read_threads));
  knowledge.set(prefix + ".write_domain", write_domain);
  knowledge.set(prefix + ".queue_length", Integer(queue_length));
  knowledge.set(prefix + ".type", Integer(type));
  knowledge.set(prefix + ".max_fragment_size", Integer(max_fragment_size));
  knowledge.set(prefix + ".resend_attempts", Integer(resend_attempts));
  knowledge.set(
      prefix + ".fragment_queue_length", Integer(fragment_queue_length));
  knowledge.set(prefix + ".reliability", Integer(reliability));
  knowledge.set(prefix + ".id", Integer(id));
  knowledge.set(prefix + ".processes", Integer(processes));

  knowledge.set(prefix + ".on_data_received_logic", on_data_received_logic);
  knowledge.set(prefix + ".delay_launch", Integer(delay_launch));
  knowledge.set(prefix + ".never_exit", Integer(never_exit));

  knowledge.set(prefix + ".send_reduced_message_header",
      Integer(send_reduced_message_header));
  knowledge.set(prefix + ".slack_time", slack_time);
  knowledge.set(prefix + ".read_thread_hertz", read_thread_hertz);
  knowledge.set(prefix + ".max_send_hertz", max_send_hertz);

  for (size_t i = 0; i < hosts.size(); ++i)
    kb_hosts.set(i, hosts[i]);

  knowledge.set(prefix + ".no_sending", Integer(no_sending));
  knowledge.set(prefix + ".no_receiving", Integer(no_receiving));
  knowledge.set(prefix + ".debug_to_kb_prefix", debug_to_kb_prefix);

  knowledge::containers::Map kb_read_domains(
      prefix + ".read_domains", knowledge);
  for (std::map<std::string, int>::const_iterator i = read_domains_.begin();
       i != read_domains_.end(); ++i)
  {
    kb_read_domains.set(
        i->first, (knowledge::KnowledgeRecord::Integer)i->second);
  }

  knowledge.save_as_karl(filename);
}
