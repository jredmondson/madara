#include "madara/transport/QoS_Transport_Settings.h"
#include "madara/knowledge_engine/Knowledge_Base.h"
#include "madara/knowledge_engine/containers/String_Vector.h"
#include "madara/knowledge_engine/containers/Map.h"

namespace containers = Madara::Knowledge_Engine::Containers;
typedef Madara::Knowledge_Record::Integer  Integer;

Madara::Transport::QoS_Transport_Settings::QoS_Transport_Settings ()
  : Settings (), rebroadcast_ttl_ (0),
    participant_rebroadcast_ttl_ (0),
    trusted_peers_ (), banned_peers_ (),
    packet_drop_rate_ (0.0), packet_drop_burst_ (1),
    max_send_bandwidth_ (-1), max_total_bandwidth_ (-1),
    latency_deadline_ (-1)
{

}

Madara::Transport::QoS_Transport_Settings::QoS_Transport_Settings (
  const QoS_Transport_Settings & settings)
  : Settings (settings),
    rebroadcast_ttl_ (settings.rebroadcast_ttl_),
    participant_rebroadcast_ttl_ (settings.participant_rebroadcast_ttl_),
    trusted_peers_ (settings.trusted_peers_),
    banned_peers_ (settings.banned_peers_),
    rebroadcast_filters_ (settings.rebroadcast_filters_),
    receive_filters_ (settings.receive_filters_),
    send_filters_ (settings.send_filters_),
    buffer_filters_ (settings.buffer_filters_),
    packet_drop_rate_ (settings.packet_drop_burst_),
    packet_drop_type_ (settings.packet_drop_type_),
    packet_drop_burst_ (settings.packet_drop_burst_),
    max_send_bandwidth_ (settings.max_send_bandwidth_),
    max_total_bandwidth_ (settings.max_total_bandwidth_),
    latency_deadline_ (settings.latency_deadline_)
{
}

Madara::Transport::QoS_Transport_Settings::QoS_Transport_Settings (
  const Settings & settings)
  : Settings (settings),
    rebroadcast_ttl_ (0),
    participant_rebroadcast_ttl_ (0),
    trusted_peers_ (),
    banned_peers_ (),
    packet_drop_rate_ (0.0), 
    max_send_bandwidth_ (-1), max_total_bandwidth_ (-1),
    latency_deadline_ (-1)
{
  const QoS_Transport_Settings * rhs = dynamic_cast <const QoS_Transport_Settings *> (
    &settings);
  if (rhs)
  {
    rebroadcast_ttl_ = rhs->rebroadcast_ttl_;
    participant_rebroadcast_ttl_ = rhs->participant_rebroadcast_ttl_;
    trusted_peers_ = rhs->trusted_peers_;
    banned_peers_ = rhs->banned_peers_;
    buffer_filters_ = rhs->buffer_filters_;
    send_filters_ = rhs->send_filters_;
    receive_filters_ = rhs->receive_filters_;
    rebroadcast_filters_ = rhs->rebroadcast_filters_;
    packet_drop_rate_ = rhs->packet_drop_rate_;
    packet_drop_type_ = rhs->packet_drop_type_;
    packet_drop_burst_ = rhs->packet_drop_burst_;
    max_send_bandwidth_ = rhs->max_send_bandwidth_;
    max_total_bandwidth_ = rhs->max_total_bandwidth_;
    latency_deadline_ = rhs->latency_deadline_;
  }
  else
  {
    Settings * lhs = dynamic_cast <Settings *> (this);
    *lhs = settings;
  }
}

Madara::Transport::QoS_Transport_Settings::~QoS_Transport_Settings ()
{
  // need to clean up functors
}

void
Madara::Transport::QoS_Transport_Settings::operator= (
  const QoS_Transport_Settings & rhs)
{
  if (this != &rhs)
  {
    Settings * lhs_base = (Settings *)this;
    Settings * rhs_base = (Settings *)&rhs;

    *lhs_base = *rhs_base;
    
    rebroadcast_ttl_ = rhs.rebroadcast_ttl_;
    participant_rebroadcast_ttl_ = rhs.participant_rebroadcast_ttl_;
    trusted_peers_ = rhs.trusted_peers_;
    banned_peers_ = rhs.banned_peers_;
    send_filters_ = rhs.send_filters_;
    receive_filters_ = rhs.receive_filters_;
    buffer_filters_ = rhs.buffer_filters_;
    rebroadcast_filters_ = rhs.rebroadcast_filters_;
    packet_drop_rate_ = rhs.packet_drop_rate_;
    packet_drop_type_ = rhs.packet_drop_type_;
    packet_drop_burst_ = rhs.packet_drop_burst_;
    max_send_bandwidth_ = rhs.max_send_bandwidth_;
    max_total_bandwidth_ = rhs.max_total_bandwidth_;
    latency_deadline_ = rhs.latency_deadline_;
  }
}

void
Madara::Transport::QoS_Transport_Settings::operator= (
  const Settings & rhs)
{
  if (this != &rhs)
  {
    rebroadcast_ttl_ = 0;
    participant_rebroadcast_ttl_ = 0;
    trusted_peers_.clear ();
    banned_peers_.clear ();
    send_filters_.clear (Knowledge_Record::ALL_TYPES);
    receive_filters_.clear (Knowledge_Record::ALL_TYPES);
    rebroadcast_filters_.clear (Knowledge_Record::ALL_TYPES);
    packet_drop_rate_ = 0.0;
    packet_drop_type_ = PACKET_DROP_PROBABLISTIC;
    packet_drop_burst_ = 1;
    max_send_bandwidth_ = -1;
    max_total_bandwidth_ = -1;
    latency_deadline_ = -1;

    Settings * lhs_base = (Settings *)this;
    Settings * rhs_base = (Settings *)&rhs;

    *lhs_base = *rhs_base;
  }
}

void
Madara::Transport::QoS_Transport_Settings::set_rebroadcast_ttl (unsigned char ttl)
{
  rebroadcast_ttl_ = ttl;
}

unsigned char
Madara::Transport::QoS_Transport_Settings::get_rebroadcast_ttl (void) const
{
  return rebroadcast_ttl_;
}

void
Madara::Transport::QoS_Transport_Settings::enable_participant_ttl (
  unsigned char ttl)
{
  participant_rebroadcast_ttl_ = ttl;
}

unsigned char
Madara::Transport::QoS_Transport_Settings::get_participant_ttl (void) const
{
  return participant_rebroadcast_ttl_;
}

void
Madara::Transport::QoS_Transport_Settings::add_trusted_peer (
  const std::string & peer)
{
  banned_peers_.erase (peer);
  trusted_peers_[peer] = 1;
}

void
Madara::Transport::QoS_Transport_Settings::add_banned_peer (
  const std::string & peer)
{
  trusted_peers_.erase (peer);
  banned_peers_[peer] = 1;
}

bool
Madara::Transport::QoS_Transport_Settings::remove_trusted_peer (
  const std::string & peer)
{
  bool condition = false;
  if (trusted_peers_.find (peer) != trusted_peers_.end ())
  {
    trusted_peers_.erase (peer);
    condition = true;
  }
  return condition;
}

bool
Madara::Transport::QoS_Transport_Settings::remove_banned_peer (
  const std::string & peer)
{
  bool condition = false;
  if (banned_peers_.find (peer) != banned_peers_.end ())
  {
    banned_peers_.erase (peer);
    condition = true;
  }
  return condition;
}

bool
Madara::Transport::QoS_Transport_Settings::is_trusted (
  const std::string & peer) const
{
  bool condition = false;

  /**
   * There are three conditions when a peer is trusted. The
   * first is when the trusted_peers are everyone (empty set)
   * and the peer does not exist in the banned list. The
   * second is when trusted_peers contains the peer.
   **/

  if (trusted_peers_.size () == 0)
  {
    if (banned_peers_.find (peer) == banned_peers_.end ())
      condition = true;
  }
  else
  {
    condition = (trusted_peers_.find (peer) != trusted_peers_.end ());
  }

  return condition;
}


void
Madara::Transport::QoS_Transport_Settings::add_send_filter (uint32_t types,
  Madara::Knowledge_Record (*function) (
    Knowledge_Engine::Function_Arguments &, Knowledge_Engine::Variables &))
{
  send_filters_.add (types, function);
}

void
Madara::Transport::QoS_Transport_Settings::add_send_filter (uint32_t types,
  Filters::Record_Filter * functor)
{
  send_filters_.add (types, functor);
}

void
Madara::Transport::QoS_Transport_Settings::add_send_filter (
  void (*function) (
        Knowledge_Map &, const Transport::Transport_Context &,
        Knowledge_Engine::Variables &))
{
  send_filters_.add (function);
}

void
Madara::Transport::QoS_Transport_Settings::add_send_filter (
  Filters::Aggregate_Filter * functor)
{
  send_filters_.add (functor);
}

void
Madara::Transport::QoS_Transport_Settings::add_receive_filter (
Filters::Aggregate_Filter * functor)
{
  receive_filters_.add (functor);
}

void
Madara::Transport::QoS_Transport_Settings::add_filter (
Filters::Buffer_Filter * functor)
{
  buffer_filters_.push_back (functor);
}

void
Madara::Transport::QoS_Transport_Settings::add_receive_filter (uint32_t types,
  Madara::Knowledge_Record (*function) (
    Knowledge_Engine::Function_Arguments &, Knowledge_Engine::Variables &))
{
  receive_filters_.add (types, function);
}

void
Madara::Transport::QoS_Transport_Settings::add_receive_filter (uint32_t types,
  Filters::Record_Filter * functor)
{
  receive_filters_.add (types, functor);
}

void
  Madara::Transport::QoS_Transport_Settings::add_receive_filter (
  void (*function) (
    Knowledge_Map &, const Transport::Transport_Context &,
    Knowledge_Engine::Variables &))
{
  receive_filters_.add (function);
}

void
Madara::Transport::QoS_Transport_Settings::add_rebroadcast_filter (uint32_t types,
  Madara::Knowledge_Record (*function) (
    Knowledge_Engine::Function_Arguments &, Knowledge_Engine::Variables &))
{
  rebroadcast_filters_.add (types, function);
}

void
Madara::Transport::QoS_Transport_Settings::add_rebroadcast_filter (uint32_t types,
  Filters::Record_Filter * functor)
{
  rebroadcast_filters_.add (types, functor);
}

void
Madara::Transport::QoS_Transport_Settings::add_rebroadcast_filter (
  void (*function) (
    Knowledge_Map &, const Transport::Transport_Context &,
    Knowledge_Engine::Variables &))
{
  rebroadcast_filters_.add (function);
}

void
Madara::Transport::QoS_Transport_Settings::add_rebroadcast_filter (
  Filters::Aggregate_Filter * functor)
{
  rebroadcast_filters_.add (functor);
}

#ifdef _MADARA_JAVA_

void
Madara::Transport::QoS_Transport_Settings::add_receive_filter (
                                                               uint32_t types, jobject & object)
{
  receive_filters_.add (types, object);
}

void
Madara::Transport::QoS_Transport_Settings::add_send_filter (
                                                            uint32_t types, jobject & object)
{
  send_filters_.add (types, object);
}

void
Madara::Transport::QoS_Transport_Settings::add_rebroadcast_filter (
                                                                   uint32_t types, jobject & object)
{
  rebroadcast_filters_.add (types, object);
}


void
Madara::Transport::QoS_Transport_Settings::add_receive_filter (
                                                               jobject & object)
{
  receive_filters_.add (object);
}

void
Madara::Transport::QoS_Transport_Settings::add_send_filter (
                                                            jobject & object)
{
  send_filters_.add (object);
}

void
Madara::Transport::QoS_Transport_Settings::add_rebroadcast_filter (
                                                                   jobject & object)
{
  rebroadcast_filters_.add (object);
}

#endif

#ifdef _MADARA_PYTHON_CALLBACKS_
      
void
Madara::Transport::QoS_Transport_Settings::add_receive_filter (
  uint32_t types, boost::python::object & object)
{
  receive_filters_.add (types, object);
}
     
void
Madara::Transport::QoS_Transport_Settings::add_send_filter (
  uint32_t types, boost::python::object & object)
{
  send_filters_.add (types, object);
}

void
Madara::Transport::QoS_Transport_Settings::add_rebroadcast_filter (
  uint32_t types, boost::python::object & object)
{
  rebroadcast_filters_.add (types, object);
}

      
void
Madara::Transport::QoS_Transport_Settings::add_receive_filter (
  boost::python::object & object)
{
  receive_filters_.add (object);
}
     
void
Madara::Transport::QoS_Transport_Settings::add_send_filter (
  boost::python::object & object)
{
  send_filters_.add (object);
}

void
Madara::Transport::QoS_Transport_Settings::add_rebroadcast_filter (
  boost::python::object & object)
{
  rebroadcast_filters_.add (object);
}
     

#endif

void
Madara::Transport::QoS_Transport_Settings::attach (
  Knowledge_Engine::Thread_Safe_Context * context)
{
  send_filters_.attach (context);
  receive_filters_.attach (context);
  rebroadcast_filters_.attach (context);
}

void
Madara::Transport::QoS_Transport_Settings::clear_send_filters (uint32_t types)
{
  send_filters_.clear (types);
}

void
Madara::Transport::QoS_Transport_Settings::clear_send_aggregate_filters
  ()
{
  send_filters_.clear_aggregate_filters ();
}

void
Madara::Transport::QoS_Transport_Settings::clear_receive_filters (uint32_t types)
{
  receive_filters_.clear (types);
}

void
Madara::Transport::QoS_Transport_Settings::clear_receive_aggregate_filters
  ()
{
  receive_filters_.clear_aggregate_filters ();
}

void
Madara::Transport::QoS_Transport_Settings::clear_rebroadcast_filters (uint32_t types)
{
  rebroadcast_filters_.clear (types);
}


void
Madara::Transport::QoS_Transport_Settings::clear_buffer_filters (void)
{
  buffer_filters_.clear ();
}

void
Madara::Transport::QoS_Transport_Settings::clear_rebroadcast_aggregate_filters
  ()
{
  rebroadcast_filters_.clear_aggregate_filters ();
}

      
Madara::Knowledge_Record
Madara::Transport::QoS_Transport_Settings::filter_send (
  const Madara::Knowledge_Record & input,
  const std::string & name,
  Transport::Transport_Context & context) const
{
  return send_filters_.filter (input, name, context);
}

void
Madara::Transport::QoS_Transport_Settings::filter_send (
  Knowledge_Map & records,
  const Transport::Transport_Context & transport_context) const
{
  send_filters_.filter (records, transport_context);
}
   

int
Madara::Transport::QoS_Transport_Settings::filter_encode (
  unsigned char * source, int size, int max_size) const
{
  // encode from front to back
  for (Filters::Buffer_Filters::const_iterator i = buffer_filters_.begin ();
    i != buffer_filters_.end (); ++i)
  {
    size = (*i)->encode (source, size, max_size);
  }

  return size;
}


int
Madara::Transport::QoS_Transport_Settings::filter_decode (
unsigned char * source, int size, int max_size) const
{
  // decode from back to front
  for (Filters::Buffer_Filters::const_reverse_iterator i = buffer_filters_.rbegin ();
    i != buffer_filters_.rend (); ++i)
  {
    size = (*i)->decode (source, size, max_size);
  }

  return size;
}


Madara::Knowledge_Record
Madara::Transport::QoS_Transport_Settings::filter_receive (
  const Madara::Knowledge_Record & input,
  const std::string & name,
  Transport::Transport_Context & context) const
{
  return receive_filters_.filter (input, name, context);
}
      
void
Madara::Transport::QoS_Transport_Settings::filter_receive (
  Knowledge_Map & records,
  const Transport::Transport_Context & transport_context) const
{
  receive_filters_.filter (records, transport_context);
}
  
Madara::Knowledge_Record
Madara::Transport::QoS_Transport_Settings::filter_rebroadcast (
  const Madara::Knowledge_Record & input,
  const std::string & name,
  Transport::Transport_Context & context) const
{
  return rebroadcast_filters_.filter (input, name, context);
}

void
Madara::Transport::QoS_Transport_Settings::filter_rebroadcast (
  Knowledge_Map & records,
  const Transport::Transport_Context & transport_context) const
{
  rebroadcast_filters_.filter (records, transport_context);
}
       
void
Madara::Transport::QoS_Transport_Settings::print_num_filters_send (
  void) const
{
  send_filters_.print_num_filters ();
}
       
void
Madara::Transport::QoS_Transport_Settings::print_num_filters_receive (
  void) const
{
  receive_filters_.print_num_filters ();
}
       
void
Madara::Transport::QoS_Transport_Settings::print_num_filters_rebroadcast (
  void) const
{
  rebroadcast_filters_.print_num_filters ();
}



size_t
Madara::Transport::QoS_Transport_Settings::get_number_of_send_filtered_types (
  void) const
{
  return send_filters_.get_number_of_filtered_types ();
}

size_t
Madara::Transport::QoS_Transport_Settings::get_number_of_send_aggregate_filters (
  void) const
{
  return send_filters_.get_number_of_aggregate_filters ();
}

size_t
Madara::Transport::QoS_Transport_Settings::get_number_of_rebroadcast_filtered_types (
  void) const
{
  return rebroadcast_filters_.get_number_of_filtered_types ();
}

size_t
Madara::Transport::QoS_Transport_Settings::get_number_of_rebroadcast_aggregate_filters (
  void) const
{
  return rebroadcast_filters_.get_number_of_aggregate_filters ();
}


size_t
Madara::Transport::QoS_Transport_Settings::get_number_of_receive_filtered_types (
  void) const
{
  return receive_filters_.get_number_of_filtered_types ();
}

size_t
Madara::Transport::QoS_Transport_Settings::get_number_of_receive_aggregate_filters (
  void) const
{
  return receive_filters_.get_number_of_aggregate_filters ();
}

size_t
Madara::Transport::QoS_Transport_Settings::get_number_of_buffer_filters (
void) const
{
  return buffer_filters_.size ();
}

void
Madara::Transport::QoS_Transport_Settings::update_drop_rate (
  double drop_rate,
  int drop_type,
  uint64_t drop_burst)
{
  packet_drop_rate_ = drop_rate;
  packet_drop_type_ = drop_type;
  packet_drop_burst_ = drop_burst;
}

double
Madara::Transport::QoS_Transport_Settings::get_drop_rate (void) const
{
  return packet_drop_rate_;
}

int
Madara::Transport::QoS_Transport_Settings::get_drop_type (void) const
{
  return packet_drop_type_;
}

uint64_t
Madara::Transport::QoS_Transport_Settings::get_drop_burst (void) const
{
  return packet_drop_burst_;
}

void
Madara::Transport::QoS_Transport_Settings::set_send_bandwidth_limit (
  int64_t send_bandwidth)
{
  max_send_bandwidth_ = send_bandwidth;
}

int64_t
Madara::Transport::QoS_Transport_Settings::get_send_bandwidth_limit (
  void) const
{
  return max_send_bandwidth_;
}

void
Madara::Transport::QoS_Transport_Settings::set_total_bandwidth_limit (
  int64_t total_bandwidth)
{
  max_total_bandwidth_ = total_bandwidth;
}

int64_t
Madara::Transport::QoS_Transport_Settings::get_total_bandwidth_limit (
  void) const
{
  return max_total_bandwidth_;
}

void
Madara::Transport::QoS_Transport_Settings::set_deadline (double deadline)
{
  latency_deadline_ = deadline;
}

double
Madara::Transport::QoS_Transport_Settings::get_deadline (void) const
{
  return latency_deadline_;
}

void
Madara::Transport::QoS_Transport_Settings::load (const std::string filename)
{
  Settings::load (filename);

  Knowledge_Engine::Knowledge_Base knowledge;
  knowledge.load_context (filename);

  containers::Map trusted_peers ("transport.trusted_peers", knowledge);
  containers::Map banned_peers ("transport.banned_peers_", knowledge);

  rebroadcast_ttl_ = (unsigned char)knowledge.get (
    "transport.rebroadcast_ttl").to_integer ();
  participant_rebroadcast_ttl_ = (unsigned char)knowledge.get (
    "transport.participant_rebroadcast_ttl").to_integer ();

  std::vector <std::string> trusted_keys, banned_keys;

  trusted_peers.keys (trusted_keys);
  banned_peers.keys (banned_keys);

  for (size_t i = 0; i < trusted_keys.size (); ++i)
  {
    trusted_peers_[trusted_keys[i]] = 1;
  }

  for (size_t i = 0; i < banned_keys.size (); ++i)
  {
    banned_peers_[banned_keys[i]] = 1;
  }

  packet_drop_rate_ = knowledge.get (
    "transport.packet_drop_rate").to_double ();
  packet_drop_type_ = (int)knowledge.get (
    "transport.packet_drop_type").to_integer ();
  packet_drop_burst_ = (uint64_t)knowledge.get (
    "transport.packet_drop_burst").to_integer ();

  max_send_bandwidth_ = (int64_t)knowledge.get (
    "transport.max_send_bandwidth").to_integer ();
  max_total_bandwidth_ = (int64_t)knowledge.get (
    "transport.max_total_bandwidth").to_integer ();

  latency_deadline_ = knowledge.get (
    "transport.latency_deadline").to_double ();
}

void
Madara::Transport::QoS_Transport_Settings::save (
  const std::string filename) const
{
  // Save the underlying base settings first
  Settings::save (filename);

  // then load the savings
  Knowledge_Engine::Knowledge_Base knowledge;
  knowledge.load_context (filename);

  containers::Map trusted_peers ("transport.trusted_peers", knowledge);
  containers::Map banned_peers ("transport.banned_peers_", knowledge);

  knowledge.set ("transport.rebroadcast_ttl", Integer (rebroadcast_ttl_));
  knowledge.set ("transport.participant_rebroadcast_ttl",
    Integer (participant_rebroadcast_ttl_));

  for (std::map <std::string, int>::const_iterator i = trusted_peers_.begin ();
    i != trusted_peers_.end (); ++i)
  {
    trusted_peers.set (i->first, Integer (1));
  }

  for (std::map <std::string, int>::const_iterator i = banned_peers_.begin ();
    i != banned_peers_.end (); ++i)
  {
    banned_peers.set (i->first, Integer (1));
  }

  knowledge.set ("transport.packet_drop_rate", packet_drop_rate_);
  knowledge.set ("transport.packet_drop_type", Integer (packet_drop_type_));
  knowledge.set ("transport.packet_drop_burst", Integer (packet_drop_burst_));

  knowledge.set ("transport.max_send_bandwidth",
    Integer (max_send_bandwidth_));
  knowledge.set ("transport.max_total_bandwidth",
    Integer (max_total_bandwidth_));
  knowledge.set ("transport.latency_deadline", latency_deadline_);

  knowledge.save_context (filename);
}
