#include "madara/transport/QoSTransportSettings.h"
#include "madara/knowledge/KnowledgeBase.h"
#include "madara/knowledge/containers/StringVector.h"
#include "madara/knowledge/containers/Map.h"
#include "madara/logger/GlobalLogger.h"

namespace logger = madara::logger;
namespace containers = madara::knowledge::containers;
typedef madara::knowledge::KnowledgeRecord::Integer Integer;

madara::transport::QoSTransportSettings::QoSTransportSettings()
  : TransportSettings(),
    rebroadcast_ttl_(0),
    participant_rebroadcast_ttl_(0),
    trusted_peers_(),
    banned_peers_(),
    packet_drop_rate_(0.0),
    packet_drop_burst_(1),
    max_send_bandwidth_(-1),
    max_total_bandwidth_(-1),
    deadline_(-1)
{
}

madara::transport::QoSTransportSettings::QoSTransportSettings(
    const QoSTransportSettings& settings)
  : TransportSettings(settings),
    rebroadcast_ttl_(settings.rebroadcast_ttl_),
    participant_rebroadcast_ttl_(settings.participant_rebroadcast_ttl_),
    trusted_peers_(settings.trusted_peers_),
    banned_peers_(settings.banned_peers_),
    rebroadcast_filters_(settings.rebroadcast_filters_),
    receive_filters_(settings.receive_filters_),
    send_filters_(settings.send_filters_),
    buffer_filters_(settings.buffer_filters_),
    packet_drop_rate_(settings.packet_drop_rate_),
    packet_drop_type_(settings.packet_drop_type_),
    packet_drop_burst_(settings.packet_drop_burst_),
    max_send_bandwidth_(settings.max_send_bandwidth_),
    max_total_bandwidth_(settings.max_total_bandwidth_),
    deadline_(settings.deadline_)
{
}

madara::transport::QoSTransportSettings::QoSTransportSettings(
    const TransportSettings& settings)
  : TransportSettings(settings),
    rebroadcast_ttl_(0),
    participant_rebroadcast_ttl_(0),
    trusted_peers_(),
    banned_peers_(),
    packet_drop_rate_(0.0),
    max_send_bandwidth_(-1),
    max_total_bandwidth_(-1),
    deadline_(-1)
{
  const QoSTransportSettings* rhs =
      dynamic_cast<const QoSTransportSettings*>(&settings);
  if(rhs)
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
    deadline_ = rhs->deadline_;
  }
  else
  {
    TransportSettings* lhs = dynamic_cast<TransportSettings*>(this);
    *lhs = settings;
  }
}

madara::transport::QoSTransportSettings::~QoSTransportSettings()
{
  // need to clean up functors
}

void madara::transport::QoSTransportSettings::operator=(
    const QoSTransportSettings& rhs)
{
  if(this != &rhs)
  {
    TransportSettings* lhs_base = (TransportSettings*)this;
    TransportSettings* rhs_base = (TransportSettings*)&rhs;

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
    deadline_ = rhs.deadline_;
  }
}

void madara::transport::QoSTransportSettings::operator=(
    const TransportSettings& rhs)
{
  if(this != &rhs)
  {
    rebroadcast_ttl_ = 0;
    participant_rebroadcast_ttl_ = 0;
    trusted_peers_.clear();
    banned_peers_.clear();
    send_filters_.clear(knowledge::KnowledgeRecord::ALL_TYPES);
    receive_filters_.clear(knowledge::KnowledgeRecord::ALL_TYPES);
    rebroadcast_filters_.clear(knowledge::KnowledgeRecord::ALL_TYPES);
    packet_drop_rate_ = 0.0;
    packet_drop_type_ = PACKET_DROP_PROBABLISTIC;
    packet_drop_burst_ = 1;
    max_send_bandwidth_ = -1;
    max_total_bandwidth_ = -1;
    deadline_ = -1;

    TransportSettings* lhs_base = (TransportSettings*)this;
    TransportSettings* rhs_base = (TransportSettings*)&rhs;

    *lhs_base = *rhs_base;
  }
}

void madara::transport::QoSTransportSettings::set_rebroadcast_ttl(
    unsigned char ttl)
{
  rebroadcast_ttl_ = ttl;
}

unsigned char madara::transport::QoSTransportSettings::get_rebroadcast_ttl(
    void) const
{
  return rebroadcast_ttl_;
}

void madara::transport::QoSTransportSettings::enable_participant_ttl(
    unsigned char ttl)
{
  participant_rebroadcast_ttl_ = ttl;
}

unsigned char madara::transport::QoSTransportSettings::get_participant_ttl(
    void) const
{
  return participant_rebroadcast_ttl_;
}

void madara::transport::QoSTransportSettings::add_trusted_peer(
    const std::string& peer)
{
  banned_peers_.erase(peer);
  trusted_peers_[peer] = 1;
}

void madara::transport::QoSTransportSettings::add_banned_peer(
    const std::string& peer)
{
  trusted_peers_.erase(peer);
  banned_peers_[peer] = 1;
}

bool madara::transport::QoSTransportSettings::remove_trusted_peer(
    const std::string& peer)
{
  bool condition = false;
  if(trusted_peers_.find(peer) != trusted_peers_.end())
  {
    trusted_peers_.erase(peer);
    condition = true;
  }
  return condition;
}

bool madara::transport::QoSTransportSettings::remove_banned_peer(
    const std::string& peer)
{
  bool condition = false;
  if(banned_peers_.find(peer) != banned_peers_.end())
  {
    banned_peers_.erase(peer);
    condition = true;
  }
  return condition;
}

bool madara::transport::QoSTransportSettings::is_trusted(
    const std::string& peer) const
{
  bool condition = false;

  /**
   * There are three conditions when a peer is trusted. The
   * first is when the trusted_peers are everyone (empty set)
   * and the peer does not exist in the banned list. The
   * second is when trusted_peers contains the peer.
   **/

  if(trusted_peers_.size() == 0)
  {
    if(banned_peers_.find(peer) == banned_peers_.end())
      condition = true;
  }
  else
  {
    condition = (trusted_peers_.find(peer) != trusted_peers_.end());
  }

  return condition;
}

void madara::transport::QoSTransportSettings::add_send_filter(
    uint32_t types, madara::knowledge::KnowledgeRecord (*function)(
                        knowledge::FunctionArguments&, knowledge::Variables&))
{
  send_filters_.add(types, function);
}

void madara::transport::QoSTransportSettings::add_send_filter(
    uint32_t types, filters::RecordFilter* functor)
{
  send_filters_.add(types, functor);
}

void madara::transport::QoSTransportSettings::add_send_filter(void (*function)(
    knowledge::KnowledgeMap&, const TransportContext&, knowledge::Variables&))
{
  send_filters_.add(function);
}

void madara::transport::QoSTransportSettings::add_send_filter(
    filters::AggregateFilter* functor)
{
  send_filters_.add(functor);
}

void madara::transport::QoSTransportSettings::add_receive_filter(
    filters::AggregateFilter* functor)
{
  receive_filters_.add(functor);
}

void madara::transport::QoSTransportSettings::add_filter(
    filters::BufferFilter* functor)
{
  buffer_filters_.push_back(functor);
}

void madara::transport::QoSTransportSettings::add_receive_filter(
    uint32_t types, madara::knowledge::KnowledgeRecord (*function)(
                        knowledge::FunctionArguments&, knowledge::Variables&))
{
  receive_filters_.add(types, function);
}

void madara::transport::QoSTransportSettings::add_receive_filter(
    uint32_t types, filters::RecordFilter* functor)
{
  receive_filters_.add(types, functor);
}

void madara::transport::QoSTransportSettings::add_receive_filter(
    void (*function)(knowledge::KnowledgeMap&, const TransportContext&,
        knowledge::Variables&))
{
  receive_filters_.add(function);
}

void madara::transport::QoSTransportSettings::add_rebroadcast_filter(
    uint32_t types, madara::knowledge::KnowledgeRecord (*function)(
                        knowledge::FunctionArguments&, knowledge::Variables&))
{
  rebroadcast_filters_.add(types, function);
}

void madara::transport::QoSTransportSettings::add_rebroadcast_filter(
    uint32_t types, filters::RecordFilter* functor)
{
  rebroadcast_filters_.add(types, functor);
}

void madara::transport::QoSTransportSettings::add_rebroadcast_filter(
    void (*function)(knowledge::KnowledgeMap&, const TransportContext&,
        knowledge::Variables&))
{
  rebroadcast_filters_.add(function);
}

void madara::transport::QoSTransportSettings::add_rebroadcast_filter(
    filters::AggregateFilter* functor)
{
  rebroadcast_filters_.add(functor);
}

#ifdef _MADARA_JAVA_

void madara::transport::QoSTransportSettings::add_receive_filter(
    uint32_t types, jobject& object)
{
  madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_MAJOR,
      "QoSTransportSettings::add: "
      "Adding Java record filter to receive queue\n");

  receive_filters_.add(types, object);
}

void madara::transport::QoSTransportSettings::add_send_filter(
    uint32_t types, jobject& object)
{
  send_filters_.add(types, object);
}

void madara::transport::QoSTransportSettings::add_rebroadcast_filter(
    uint32_t types, jobject& object)
{
  rebroadcast_filters_.add(types, object);
}

void madara::transport::QoSTransportSettings::add_receive_filter(
    jobject& object)
{
  receive_filters_.add(object);
}

void madara::transport::QoSTransportSettings::add_send_filter(jobject& object)
{
  send_filters_.add(object);
}

void madara::transport::QoSTransportSettings::add_rebroadcast_filter(
    jobject& object)
{
  rebroadcast_filters_.add(object);
}

#endif

#ifdef _MADARA_PYTHON_CALLBACKS_

void madara::transport::QoSTransportSettings::add_receive_filter(
    uint32_t types, boost::python::object& object)
{
  receive_filters_.add(types, object);
}

void madara::transport::QoSTransportSettings::add_send_filter(
    uint32_t types, boost::python::object& object)
{
  send_filters_.add(types, object);
}

void madara::transport::QoSTransportSettings::add_rebroadcast_filter(
    uint32_t types, boost::python::object& object)
{
  rebroadcast_filters_.add(types, object);
}

void madara::transport::QoSTransportSettings::add_receive_filter(
    boost::python::object& object)
{
  receive_filters_.add(object);
}

void madara::transport::QoSTransportSettings::add_send_filter(
    boost::python::object& object)
{
  send_filters_.add(object);
}

void madara::transport::QoSTransportSettings::add_rebroadcast_filter(
    boost::python::object& object)
{
  rebroadcast_filters_.add(object);
}

#endif

void madara::transport::QoSTransportSettings::attach(
    knowledge::ThreadSafeContext* context)
{
  send_filters_.attach(context);
  receive_filters_.attach(context);
  rebroadcast_filters_.attach(context);
}

void madara::transport::QoSTransportSettings::clear_send_filters(uint32_t types)
{
  send_filters_.clear(types);
}

void madara::transport::QoSTransportSettings::clear_send_aggregate_filters()
{
  send_filters_.clear_aggregate_filters();
}

void madara::transport::QoSTransportSettings::clear_receive_filters(
    uint32_t types)
{
  receive_filters_.clear(types);
}

void madara::transport::QoSTransportSettings::clear_receive_aggregate_filters()
{
  receive_filters_.clear_aggregate_filters();
}

void madara::transport::QoSTransportSettings::clear_rebroadcast_filters(
    uint32_t types)
{
  rebroadcast_filters_.clear(types);
}

void madara::transport::QoSTransportSettings::clear_buffer_filters(void)
{
  buffer_filters_.clear();
}

void madara::transport::QoSTransportSettings::
    clear_rebroadcast_aggregate_filters()
{
  rebroadcast_filters_.clear_aggregate_filters();
}

madara::knowledge::KnowledgeRecord
madara::transport::QoSTransportSettings::filter_send(
    const madara::knowledge::KnowledgeRecord& input, const std::string& name,
    transport::TransportContext& context) const
{
  return send_filters_.filter(input, name, context);
}

void madara::transport::QoSTransportSettings::filter_send(
    knowledge::KnowledgeMap& records,
    const TransportContext& transport_context) const
{
  send_filters_.filter(records, transport_context);
}

int madara::transport::QoSTransportSettings::filter_encode(
    char* source, int size, int max_size) const
{
  // encode from front to back
  for(filters::BufferFilters::const_iterator i = buffer_filters_.begin();
       i != buffer_filters_.end(); ++i)
  {
    madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_MINOR,
        "QoSTransportSettings::filter_encode: size before encode: "
        " %d of %d\n",
        size, max_size);

    size = (*i)->encode(source, size, max_size);

    madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_MINOR,
        "QoSTransportSettings::filter_encode: size after encode: "
        " %d of %d\n",
        size, max_size);

    if(max_size > size + 20)
    {
      memmove(source + 20, source, size);

      filters::BufferFilterHeader header;
      header.read(*i);
      header.size = (uint64_t)size;

      int64_t buffer_remaining = 20;

      header.write((char*)source, buffer_remaining);

      size += (int)filters::BufferFilterHeader::encoded_size();

      madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_MAJOR,
          "QoSTransportSettings::filter_encode: header: "
          "%s:%s within size %d\n",
          header.id, utility::to_string_version(header.version).c_str(), size);
    }
    else
    {
      std::stringstream buffer;
      buffer << "QoSTransportSettings::filter_encode: ";
      buffer << (size + 20) << " 20 byte size encoding cannot fit in ";
      buffer << max_size << " byte buffer\n";

      throw exceptions::MemoryException(buffer.str());
    }
  }

  return size;
}

int madara::transport::QoSTransportSettings::filter_decode(
    char* source, int size, int max_size) const
{
  // if we don't have buffer filters, do a check to see if we should
  filters::BufferFilterHeader header;
  int64_t buffer_size = (int64_t)filters::BufferFilterHeader::encoded_size();

  header.read((char*)source, buffer_size);

  // if this is a fragment, the decode needs to be run after defrag
  if(utility::begins_with (header.id, "KFRG"))
  {
    madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_MAJOR,
        "QoSTransportSettings::filter_decode: header: "
        " Detected %s. decode has to be called on defragged buffer.\n",
        header.id);

    return size;
  }
  else
  {
    madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_MAJOR,
      "QoSTransportSettings::filter_decode: header: "
      " Detected %s\n",
      header.id);
  }

  if(buffer_filters_.size() == 0)
  {
    // id is either karl or KaRL. If it's anything else, then error
    if(utility::begins_with (header.id, "karl") ||
       utility::begins_with (header.id, "KaRL"))
    {
      madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_MAJOR,
          "QoSTransportSettings::filter_decode: header: "
          " Detected %s\n",
          header.id);
    }
    else
    {
      madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_MAJOR,
          "QoSTransportSettings::filter_decode: header: "
          " Detected %s, which is not a message or checkpoint header\n",
          header.id);

      return 0;
    }
  }

  // decode from back to front
  for(filters::BufferFilters::const_reverse_iterator i =
           buffer_filters_.rbegin();
       i != buffer_filters_.rend(); ++i)
  {
    if(size > (int)filters::BufferFilterHeader::encoded_size())
    {
      buffer_size = (int64_t)filters::BufferFilterHeader::encoded_size();

      header.read((char*)source, buffer_size);

      if(header.size > (uint64_t)max_size)
      {
        madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_ERROR,
            "QoSTransportSettings::filter_decode: header: "
            " %d byte size encoding cannot fit in %d byte buffer\n",
            (int)header.size, max_size);

        return 0;
      }

      madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_MAJOR,
          "QoSTransportSettings::filter_decode: header: "
          " %s:%s\n",
          header.id, utility::to_string_version(header.version).c_str());

      if(*i == 0)
      {
        madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_ERROR,
            "QoSTransportSettings::filter_decode: filter is null somehow\n");

        return 0;
      }
      else
      {
        madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_DETAILED,
            "QoSTransportSettings::filter_decode: filter is not null\n");
      }

      if(header.check_filter(*i))
      {
        madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_MAJOR,
            "QoSTransportSettings::filter_decode: buffer filter %s is a "
            "match\n",
            header.id);
      }
      else
      {
        madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_ERROR,
            "QoSTransportSettings::filter_decode: buffer filter %s doesn't "
            "match."
            " Returning 0.\n",
            header.id);

        return 0;
      }

      madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_MINOR,
          "QoSTransportSettings::filter_decode: size before decode: "
          " %d of %d (header.size=%d)\n",
          size, max_size, (int)header.size);

      size = (*i)->decode(source + filters::BufferFilterHeader::encoded_size(),
          (int)header.size, max_size);

      madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_MINOR,
          "QoSTransportSettings::filter_decode: size after decode: "
          " %d of %d (header.size=%d)\n",
          size, max_size, (int)header.size);

      if(size > 0)
      {
        memmove(
            source, source + filters::BufferFilterHeader::encoded_size(), size);
      }
    }
    else
    {
      madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_ERROR,
          "QoSTransportSettings::filter_decode: "
          " %d byte size encoding cannot fit in %d byte buffer\n",
          size, max_size);

      return 0;
    }  // end if size is bigger than the buffer header
  }    // end for loop iteration of filters

  return size;
}

madara::knowledge::KnowledgeRecord
madara::transport::QoSTransportSettings::filter_receive(
    const madara::knowledge::KnowledgeRecord& input, const std::string& name,
    transport::TransportContext& context) const
{
  return receive_filters_.filter(input, name, context);
}

void madara::transport::QoSTransportSettings::filter_receive(
    knowledge::KnowledgeMap& records,
    const transport::TransportContext& transport_context) const
{
  receive_filters_.filter(records, transport_context);
}

madara::knowledge::KnowledgeRecord
madara::transport::QoSTransportSettings::filter_rebroadcast(
    const madara::knowledge::KnowledgeRecord& input, const std::string& name,
    transport::TransportContext& context) const
{
  return rebroadcast_filters_.filter(input, name, context);
}

void madara::transport::QoSTransportSettings::filter_rebroadcast(
    knowledge::KnowledgeMap& records,
    const transport::TransportContext& transport_context) const
{
  rebroadcast_filters_.filter(records, transport_context);
}

void madara::transport::QoSTransportSettings::print_num_filters_send(void) const
{
  send_filters_.print_num_filters();
}

void madara::transport::QoSTransportSettings::print_num_filters_receive(
    void) const
{
  receive_filters_.print_num_filters();
}

void madara::transport::QoSTransportSettings::print_num_filters_rebroadcast(
    void) const
{
  rebroadcast_filters_.print_num_filters();
}

size_t
madara::transport::QoSTransportSettings::get_number_of_send_filtered_types(
    void) const
{
  return send_filters_.get_number_of_filtered_types();
}

size_t
madara::transport::QoSTransportSettings::get_number_of_send_aggregate_filters(
    void) const
{
  return send_filters_.get_number_of_aggregate_filters();
}

size_t madara::transport::QoSTransportSettings::
    get_number_of_rebroadcast_filtered_types(void) const
{
  return rebroadcast_filters_.get_number_of_filtered_types();
}

size_t madara::transport::QoSTransportSettings::
    get_number_of_rebroadcast_aggregate_filters(void) const
{
  return rebroadcast_filters_.get_number_of_aggregate_filters();
}

size_t
madara::transport::QoSTransportSettings::get_number_of_receive_filtered_types(
    void) const
{
  return receive_filters_.get_number_of_filtered_types();
}

size_t madara::transport::QoSTransportSettings::
    get_number_of_receive_aggregate_filters(void) const
{
  return receive_filters_.get_number_of_aggregate_filters();
}

size_t madara::transport::QoSTransportSettings::get_number_of_buffer_filters(
    void) const
{
  return buffer_filters_.size();
}

void madara::transport::QoSTransportSettings::update_drop_rate(
    double drop_rate, int drop_type, uint64_t drop_burst)
{
  packet_drop_rate_ = drop_rate;
  packet_drop_type_ = drop_type;
  packet_drop_burst_ = drop_burst;
}

double madara::transport::QoSTransportSettings::get_drop_rate(void) const
{
  return packet_drop_rate_;
}

int madara::transport::QoSTransportSettings::get_drop_type(void) const
{
  return packet_drop_type_;
}

uint64_t madara::transport::QoSTransportSettings::get_drop_burst(void) const
{
  return packet_drop_burst_;
}

void madara::transport::QoSTransportSettings::set_send_bandwidth_limit(
    int64_t send_bandwidth)
{
  max_send_bandwidth_ = send_bandwidth;
}

int64_t madara::transport::QoSTransportSettings::get_send_bandwidth_limit(
    void) const
{
  return max_send_bandwidth_;
}

void madara::transport::QoSTransportSettings::set_total_bandwidth_limit(
    int64_t total_bandwidth)
{
  max_total_bandwidth_ = total_bandwidth;
}

int64_t madara::transport::QoSTransportSettings::get_total_bandwidth_limit(
    void) const
{
  return max_total_bandwidth_;
}

void madara::transport::QoSTransportSettings::set_deadline(double deadline)
{
  deadline_ = deadline;
}

double madara::transport::QoSTransportSettings::get_deadline(void) const
{
  return deadline_;
}

void madara::transport::QoSTransportSettings::load(
    const std::string& filename, const std::string& prefix)
{
  TransportSettings::load(filename, prefix);

  knowledge::KnowledgeBase knowledge;
  knowledge.load_context(filename);

  containers::Map trusted_peers(prefix + ".trusted_peers", knowledge);
  containers::Map banned_peers(prefix + ".banned_peers", knowledge);

  rebroadcast_ttl_ =
      (unsigned char)knowledge.get(prefix + ".rebroadcast_ttl").to_integer();
  participant_rebroadcast_ttl_ =
      (unsigned char)knowledge.get(prefix + ".participant_rebroadcast_ttl")
          .to_integer();

  std::vector<std::string> trusted_keys, banned_keys;

  trusted_peers.keys(trusted_keys);
  banned_peers.keys(banned_keys);

  for(size_t i = 0; i < trusted_keys.size(); ++i)
  {
    trusted_peers_[trusted_keys[i]] = 1;
  }

  for(size_t i = 0; i < banned_keys.size(); ++i)
  {
    banned_peers_[banned_keys[i]] = 1;
  }

  packet_drop_rate_ = knowledge.get(prefix + ".packet_drop_rate").to_double();
  packet_drop_type_ =
      (int)knowledge.get(prefix + ".packet_drop_type").to_integer();
  packet_drop_burst_ =
      (uint64_t)knowledge.get(prefix + ".packet_drop_burst").to_integer();

  max_send_bandwidth_ =
      (int64_t)knowledge.get(prefix + ".max_send_bandwidth").to_integer();
  max_total_bandwidth_ =
      (int64_t)knowledge.get(prefix + ".max_total_bandwidth").to_integer();

  deadline_ = knowledge.get(prefix + ".deadline").to_double();
}

void madara::transport::QoSTransportSettings::load_text(
    const std::string& filename, const std::string& prefix)
{
  TransportSettings::load_text(filename, prefix);

  knowledge::KnowledgeBase knowledge;
  knowledge.evaluate(madara::utility::file_to_string(filename));

  containers::Map trusted_peers(prefix + ".trusted_peers", knowledge);
  containers::Map banned_peers(prefix + ".banned_peers", knowledge);

  rebroadcast_ttl_ =
      (unsigned char)knowledge.get(prefix + ".rebroadcast_ttl").to_integer();
  participant_rebroadcast_ttl_ =
      (unsigned char)knowledge.get(prefix + ".participant_rebroadcast_ttl")
          .to_integer();

  std::vector<std::string> trusted_keys, banned_keys;

  trusted_peers.keys(trusted_keys);
  banned_peers.keys(banned_keys);

  for(size_t i = 0; i < trusted_keys.size(); ++i)
  {
    trusted_peers_[trusted_keys[i]] = 1;
  }

  for(size_t i = 0; i < banned_keys.size(); ++i)
  {
    banned_peers_[banned_keys[i]] = 1;
  }

  packet_drop_rate_ = knowledge.get(prefix + ".packet_drop_rate").to_double();
  packet_drop_type_ =
      (int)knowledge.get(prefix + ".packet_drop_type").to_integer();
  packet_drop_burst_ =
      (uint64_t)knowledge.get(prefix + ".packet_drop_burst").to_integer();

  max_send_bandwidth_ =
      (int64_t)knowledge.get(prefix + ".max_send_bandwidth").to_integer();
  max_total_bandwidth_ =
      (int64_t)knowledge.get(prefix + ".max_total_bandwidth").to_integer();

  deadline_ = knowledge.get(prefix + ".deadline").to_double();
}

void madara::transport::QoSTransportSettings::save(
    const std::string& filename, const std::string& prefix) const
{
  // Save the underlying base settings first
  TransportSettings::save(filename, prefix);

  // then load the savings
  knowledge::KnowledgeBase knowledge;
  knowledge.load_context(filename);

  containers::Map trusted_peers(prefix + ".trusted_peers", knowledge);
  containers::Map banned_peers(prefix + ".banned_peers", knowledge);

  knowledge.set(prefix + ".rebroadcast_ttl", Integer(rebroadcast_ttl_));
  knowledge.set(prefix + ".participant_rebroadcast_ttl",
      Integer(participant_rebroadcast_ttl_));

  for(std::map<std::string, int>::const_iterator i = trusted_peers_.begin();
       i != trusted_peers_.end(); ++i)
  {
    trusted_peers.set(i->first, Integer(1));
  }

  for(std::map<std::string, int>::const_iterator i = banned_peers_.begin();
       i != banned_peers_.end(); ++i)
  {
    banned_peers.set(i->first, Integer(1));
  }

  knowledge.set(prefix + ".packet_drop_rate", packet_drop_rate_);
  knowledge.set(prefix + ".packet_drop_type", Integer(packet_drop_type_));
  knowledge.set(prefix + ".packet_drop_burst", Integer(packet_drop_burst_));

  knowledge.set(prefix + ".max_send_bandwidth", Integer(max_send_bandwidth_));
  knowledge.set(prefix + ".max_total_bandwidth", Integer(max_total_bandwidth_));
  knowledge.set(prefix + ".deadline", deadline_);

  knowledge.save_context(filename);
}

void madara::transport::QoSTransportSettings::save_text(
    const std::string& filename, const std::string& prefix) const
{
  // Save the underlying base settings first
  TransportSettings::save_text(filename, prefix);

  // then load the savings
  knowledge::KnowledgeBase knowledge;
  knowledge.evaluate(madara::utility::file_to_string(filename));

  containers::Map trusted_peers(prefix + ".trusted_peers", knowledge);
  containers::Map banned_peers(prefix + ".banned_peers", knowledge);

  knowledge.set(prefix + ".rebroadcast_ttl", Integer(rebroadcast_ttl_));
  knowledge.set(prefix + ".participant_rebroadcast_ttl",
      Integer(participant_rebroadcast_ttl_));

  for(std::map<std::string, int>::const_iterator i = trusted_peers_.begin();
       i != trusted_peers_.end(); ++i)
  {
    trusted_peers.set(i->first, Integer(1));
  }

  for(std::map<std::string, int>::const_iterator i = banned_peers_.begin();
       i != banned_peers_.end(); ++i)
  {
    banned_peers.set(i->first, Integer(1));
  }

  knowledge.set(prefix + ".packet_drop_rate", packet_drop_rate_);
  knowledge.set(prefix + ".packet_drop_type", Integer(packet_drop_type_));
  knowledge.set(prefix + ".packet_drop_burst", Integer(packet_drop_burst_));

  knowledge.set(prefix + ".max_send_bandwidth", Integer(max_send_bandwidth_));
  knowledge.set(prefix + ".max_total_bandwidth", Integer(max_total_bandwidth_));
  knowledge.set(prefix + ".deadline", deadline_);

  knowledge.save_as_karl(filename);
}
