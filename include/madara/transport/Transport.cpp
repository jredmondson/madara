#include "Transport.h"

#include "madara/utility/Utility.h"
#include "madara/expression/Interpreter.h"
#include "madara/knowledge/ContextGuard.h"

#include <algorithm>

namespace madara
{
namespace transport
{
Base::Base(const std::string& id, TransportSettings& new_settings,
    knowledge::ThreadSafeContext& context)
  : is_valid_(false),
    shutting_down_(false),
    id_(id),
    settings_(new_settings),
    context_(context)

#ifndef _MADARA_NO_KARL_
    ,
    on_data_received_(context.get_logger())
#endif  // _MADARA_NO_KARL_
{
  settings_.attach(&context_);
  packet_scheduler_.attach(&settings_);
}

Base::~Base() {}

int Base::setup(void)
{
  // check for an on_data_received ruleset
  if (settings_.on_data_received_logic.length() != 0) {
    madara_logger_log(context_.get_logger(), logger::LOG_MAJOR,
        "transport::Base::setup"
        " setting rules to %s\n",
        settings_.on_data_received_logic.c_str());

#ifndef _MADARA_NO_KARL_
    expression::Interpreter interpreter;
    on_data_received_ =
        interpreter.interpret(context_, settings_.on_data_received_logic);
#endif  // _MADARA_NO_KARL_
  } else {
    madara_logger_log(context_.get_logger(), logger::LOG_MAJOR,
        "transport::Base::setup"
        " no permanent rules were set\n");
  }

  // setup the send buffer
  if (settings_.queue_length > 0)
    buffer_ = new char[settings_.queue_length];

  // if read domains has not been set, then set to write domain
  if (settings_.num_read_domains() == 0) {
    madara_logger_log(context_.get_logger(), logger::LOG_MAJOR,
        "transport::Base::setup"
        " no read domains set. Adding write domain (%s)\n",
        settings_.write_domain.c_str());

    settings_.add_read_domain(settings_.write_domain);
  } else {
    madara_logger_log(context_.get_logger(), logger::LOG_MAJOR,
        "transport::Base::setup"
        " settings configured with %d read domains\n",
        settings_.num_read_domains());
  }

  if (settings_.num_read_domains() > 0 &&
      context_.get_logger().get_level() >= logger::LOG_MAJOR) {
    std::vector<std::string> domains;
    settings_.get_read_domains(domains);

    std::stringstream buffer;

    for (unsigned int i = 0; i < domains.size(); ++i) {
      buffer << domains[i];

      if (i != domains.size() - 1) {
        buffer << ", ";
      }
    }

    madara_logger_log(context_.get_logger(), logger::LOG_MAJOR,
        "transport::Base::setup"
        " Write domain: %s. Read domains: %s\n",
        settings_.write_domain.c_str(), buffer.str().c_str());
  }

  return validate_transport();
}

void Base::close(void)
{
  invalidate_transport();
}

int process_received_update(const char* buffer, uint32_t bytes_read,
    const std::string& id, knowledge::ThreadSafeContext& context,
    const QoSTransportSettings& settings, BandwidthMonitor& send_monitor,
    BandwidthMonitor& receive_monitor,
    knowledge::KnowledgeMap& rebroadcast_records,
#ifndef _MADARA_NO_KARL_
    knowledge::CompiledExpression& on_data_received,
#endif  // _MADARA_NO_KARL_

    const char* print_prefix, const char* remote_host, MessageHeader*& header)
{
  // reset header to 0, so it is safe to delete
  header = 0;

  int max_buffer_size = (int)bytes_read;

  // tell the receive bandwidth monitor about the transaction
  receive_monitor.add(bytes_read);

  madara_logger_log(context.get_logger(), logger::LOG_MAJOR,
      "%s:"
      " Receive bandwidth = %" PRIu64 " B/s\n",
      print_prefix, receive_monitor.get_bytes_per_second());

  bool is_reduced = false;
  bool is_fragment = false;

  madara_logger_log(context.get_logger(), logger::LOG_MAJOR,
      "%s:"
      " calling decode filters on %" PRIu32 " bytes\n",
      print_prefix, bytes_read);

  // call decodes, if applicable
  bytes_read = (uint32_t)settings.filter_decode(
      (char*)buffer, max_buffer_size, max_buffer_size);

  madara_logger_log(context.get_logger(), logger::LOG_MAJOR,
      "%s:"
      " Decoding resulted in %" PRIu32 " final bytes\n",
      print_prefix, bytes_read);

  // setup buffer remaining, used by the knowledge record read method
  int64_t buffer_remaining = (int64_t)bytes_read;

  // clear the rebroadcast records
  rebroadcast_records.clear();

  // receive records will be what we pass to the aggregate filter
  knowledge::KnowledgeMap updates;

  // if a key appears multiple times, keep to add to buffer history
  std::map<std::string, std::vector<knowledge::KnowledgeRecord>> past_updates;

  // check the buffer for a reduced message header
  if (bytes_read >= ReducedMessageHeader::static_encoded_size() &&
      ReducedMessageHeader::reduced_message_header_test(buffer)) {
    madara_logger_log(context.get_logger(), logger::LOG_MINOR,
        "%s:"
        " processing reduced KaRL message from %s\n",
        print_prefix, remote_host);

    header = new ReducedMessageHeader();
    is_reduced = true;
  } else if (bytes_read >= MessageHeader::static_encoded_size() &&
             MessageHeader::message_header_test(buffer)) {
    madara_logger_log(context.get_logger(), logger::LOG_MINOR,
        "%s:"
        " processing KaRL message from %s\n",
        print_prefix, remote_host);

    header = new MessageHeader();
  } else if (bytes_read >= FragmentMessageHeader::static_encoded_size() &&
             FragmentMessageHeader::fragment_message_header_test(buffer)) {
    madara_logger_log(context.get_logger(), logger::LOG_MINOR,
        "%s:"
        " processing KaRL fragment message from %s\n",
        print_prefix, remote_host);

    header = new FragmentMessageHeader();
    is_fragment = true;
  } else if (bytes_read >= 8 + MADARA_IDENTIFIER_LENGTH) {
    // get the text that appears as identifier.
    char identifier[MADARA_IDENTIFIER_LENGTH];
    strncpy(identifier, buffer + 8, MADARA_IDENTIFIER_LENGTH);
    identifier[7] = 0;

    madara_logger_log(context.get_logger(), logger::LOG_MINOR,
        "%s:"
        " dropping non-KaRL message with id %s from %s\n",
        print_prefix, identifier, remote_host);

    return -1;
  } else {
    madara_logger_log(context.get_logger(), logger::LOG_MINOR,
        "%s:"
        " dropping too short message from %s (length %i)\n",
        print_prefix, remote_host, bytes_read);

    return -1;
  }

  const char* update = header->read(buffer, buffer_remaining);

  madara_logger_log(context.get_logger(), logger::LOG_MINOR,
      "%s:"
      " header info: %s\n",
      print_prefix, header->to_string().c_str());

  if (header->size < bytes_read) {
    madara_logger_log(context.get_logger(), logger::LOG_MAJOR,
        "%s:"
        " Message header.size (%" PRIu64 " bytes) is less than actual"
        " bytes read (%" PRIu32 " bytes). Dropping message.\n",
        print_prefix, header->size, bytes_read);

    return -1;
  }

  if (is_fragment && exists(header->originator, header->clock,
                         ((FragmentMessageHeader*)header)->update_number,
                         settings.fragment_map)) {
    madara_logger_log(context.get_logger(), logger::LOG_MAJOR,
        "%s:"
        " Fragment already exists in fragment map. Dropping.\n",
        print_prefix);

    return -1;
  }

  if (!is_reduced) {
    // reject the message if it is us as the originator (no update necessary)
    if (id == header->originator) {
      madara_logger_log(context.get_logger(), logger::LOG_MAJOR,
          "%s:"
          " dropping message from ourself\n",
          print_prefix);

      return -2;
    } else {
      madara_logger_log(context.get_logger(), logger::LOG_DETAILED,
          "%s:"
          " remote id (%s) is not our own\n",
          print_prefix, remote_host);
    }

    if (settings.is_trusted(remote_host)) {
      madara_logger_log(context.get_logger(), logger::LOG_DETAILED,
          "%s: remote id (%s) is trusted\n", print_prefix, remote_host);
    } else {
      madara_logger_log(context.get_logger(), logger::LOG_MAJOR,
          "%s:"
          " dropping message from untrusted peer (%s\n",
          print_prefix, remote_host);

      // delete the header and continue to the svc loop
      return -3;
    }

    std::string originator(header->originator);

    if (settings.is_trusted(originator)) {
      madara_logger_log(context.get_logger(), logger::LOG_MINOR,
          "%s:"
          " originator (%s) is trusted\n",
          print_prefix, originator.c_str());
    } else {
      madara_logger_log(context.get_logger(), logger::LOG_MAJOR,
          "%s:"
          " dropping message from untrusted originator (%s)\n",
          print_prefix, originator.c_str());

      return -4;
    }

    // reject the message if it is from a different domain
    if (!settings.is_reading_domain(header->domain)) {
      madara_logger_log(context.get_logger(), logger::LOG_MAJOR,
          "%s:"
          " remote id (%s) has an untrusted domain (%s). Dropping message.\n",
          print_prefix, remote_host, header->domain);

      // delete the header and continue to the svc loop
      return -5;
    } else {
      madara_logger_log(context.get_logger(), logger::LOG_MINOR,
          "%s:"
          " remote id (%s) message is in our domain\n",
          print_prefix, remote_host);
    }
  }

  // fragments are special cases
  if (is_fragment) {
    // grab the fragment header
    FragmentMessageHeader* frag_header =
        dynamic_cast<FragmentMessageHeader*>(header);

    madara_logger_log(context.get_logger(), logger::LOG_MAJOR,
        "%s:"
        " Processing fragment %" PRIu32 " of %s:%" PRIu64 ".\n",
        print_prefix, frag_header->update_number, frag_header->originator,
        frag_header->clock);

    // add the fragment and attempt to defrag the message
    char* message = transport::add_fragment(frag_header->originator,
        frag_header->clock, frag_header->update_number, buffer,
        settings.fragment_queue_length, settings.fragment_map, true);

    // if we have no return message, we may have previously defragged it
    if (!message) {
      return 0;
    } else {
      madara_logger_log(context.get_logger(), logger::LOG_MAJOR,
          "%s:"
          " Message has been pieced together from fragments. Processing...\n",
          print_prefix);

      /**
       * if we defragged the message, then we need to process the message.
       * In order to do that, we need to overwrite buffer with message
       * so it can be processed normally.
       **/
      buffer_remaining = (int64_t)frag_header->get_size(message);
      if (buffer_remaining <= settings.queue_length) {
        char* buffer_override = (char*)buffer;
        memcpy(buffer_override, message, frag_header->get_size(message));

        // check the buffer for a reduced message header
        if (ReducedMessageHeader::reduced_message_header_test(buffer)) {
          madara_logger_log(context.get_logger(), logger::LOG_MINOR,
              "%s:"
              " processing reduced KaRL message from %s\n",
              print_prefix, remote_host);

          header = new ReducedMessageHeader();
          is_reduced = true;
          update = header->read(buffer, buffer_remaining);
        } else if (MessageHeader::message_header_test(buffer)) {
          madara_logger_log(context.get_logger(), logger::LOG_MINOR,
              "%s:"
              " processing KaRL message from %s\n",
              print_prefix, remote_host);

          header = new MessageHeader();
          update = header->read(buffer, buffer_remaining);
        }

        delete[] message;
      }
    }
  }

  int actual_updates = 0;
  uint64_t current_time = utility::get_time();
  double deadline = settings.get_deadline();
  TransportContext transport_context(TransportContext::RECEIVING_OPERATION,
      receive_monitor.get_bytes_per_second(),
      send_monitor.get_bytes_per_second(), header->timestamp, current_time,
      header->domain, header->originator, remote_host);

  uint64_t latency(0);

  if (deadline >= 1.0) {
    if (header->timestamp < current_time) {
      latency = current_time - header->timestamp;

      if (latency > deadline) {
        madara_logger_log(context.get_logger(), logger::LOG_MAJOR,
            "%s:"
            " deadline violation (latency is %" PRIu64 ", deadline is %f).\n",
            print_prefix, latency, deadline);

        return -6;
      }
    } else {
      madara_logger_log(context.get_logger(), logger::LOG_MINOR,
          "%s:"
          " Cannot compute message latency."
          " Message header timestamp is in the future."
          " message.timestamp = %" PRIu64 ", cur_timestamp = %" PRIu64 ".\n",
          print_prefix, header->timestamp, current_time);
    }
  }

  madara_logger_log(context.get_logger(), logger::LOG_MINOR,
      "%s:"
      " iterating over the %" PRIu32 " updates\n",
      print_prefix, header->updates);

  // temporary record for reading from the updates buffer
  knowledge::KnowledgeRecord record;
  record.quality = header->quality;
  record.clock = header->clock;
  std::string key;

  bool dropped = false;

  if (send_monitor.is_bandwidth_violated(settings.get_send_bandwidth_limit())) {
    dropped = true;
    madara_logger_log(context.get_logger(), logger::LOG_MAJOR,
        "%s:"
        " Send monitor has detected violation of bandwidth limit."
        " Dropping packet from rebroadcast list\n",
        print_prefix);
  } else if (receive_monitor.is_bandwidth_violated(
                 settings.get_total_bandwidth_limit())) {
    dropped = true;
    madara_logger_log(context.get_logger(), logger::LOG_MAJOR,
        "%s:"
        " Receive monitor has detected violation of bandwidth limit."
        " Dropping packet from rebroadcast list...\n",
        print_prefix);
  } else if (settings.get_participant_ttl() < header->ttl) {
    dropped = true;
    madara_logger_log(context.get_logger(), logger::LOG_MAJOR,
        "%s:"
        " Transport participant TTL is lower than header ttl."
        " Dropping packet from rebroadcast list...\n",
        print_prefix);
  }

  madara_logger_log(context.get_logger(), logger::LOG_MAJOR,
      "%s:"
      " Applying %" PRIu32 " updates\n",
      print_prefix, header->updates);

  const auto add_record = [&](const std::string& key,
                              knowledge::KnowledgeRecord rec) {
    auto& entry = updates[key];
    if (entry.exists()) {
      using std::swap;

      swap(rec, entry);

      past_updates[key].emplace_back(std::move(rec));
    } else {
      entry = std::move(rec);
    }
  };

  // iterate over the updates
  for (uint32_t i = 0; i < header->updates; ++i) {
    // read converts everything into host format from the update stream
    update = record.read(update, key, buffer_remaining);

    if (buffer_remaining < 0) {
      madara_logger_log(context.get_logger(), logger::LOG_EMERGENCY,
          "%s:"
          " unable to process message. Buffer remaining is negative."
          " Server is likely being targeted by custom KaRL tools.\n",
          print_prefix);

      // we do not delete the header as this will be cleaned up later
      break;
    } else {
      madara_logger_log(context.get_logger(), logger::LOG_MINOR,
          "%s:"
          " Applying receive filter to %s (clk %i, qual %i) = %s\n",
          print_prefix, key.c_str(), record.clock, record.quality,
          record.to_string().c_str());

      record = settings.filter_receive(record, key, transport_context);

      if (record.exists()) {
        madara_logger_log(context.get_logger(), logger::LOG_MINOR,
            "%s:"
            " Filter results for %s were %s\n",
            print_prefix, key.c_str(), record.to_string().c_str());

        add_record(key, record);
      } else {
        madara_logger_log(context.get_logger(), logger::LOG_MINOR,
            "%s:"
            " Filter resulted in dropping %s\n",
            print_prefix, key.c_str());
      }
    }
  }

  const knowledge::KnowledgeMap& additionals = transport_context.get_records();

  if (additionals.size() > 0) {
    madara_logger_log(context.get_logger(), logger::LOG_MAJOR,
        "%s:"
        " %lld additional records being handled after receive.\n",
        print_prefix, (long long)additionals.size());

    for (knowledge::KnowledgeMap::const_iterator i = additionals.begin();
         i != additionals.end(); ++i) {
      add_record(i->first, i->second);
    }

    transport_context.clear_records();

    if (header->ttl < 2)
      header->ttl = 2;

    // modify originator to indicate we are the originator of modifications
    strncpy(header->originator, id.c_str(), sizeof(header->originator) - 1);
  }

  // apply aggregate receive filters
  if (settings.get_number_of_receive_aggregate_filters() > 0 &&
      (updates.size() > 0 || header->type == transport::REGISTER)) {
    madara_logger_log(context.get_logger(), logger::LOG_MAJOR,
        "%s:"
        " Applying aggregate receive filters.\n",
        print_prefix);

    settings.filter_receive(updates, transport_context);
  } else {
    madara_logger_log(context.get_logger(), logger::LOG_MINOR,
        "%s:"
        " No aggregate receive filters were applied...\n",
        print_prefix);
  }

  madara_logger_log(context.get_logger(), logger::LOG_MINOR,
      "%s:"
      " Locking the context to apply updates.\n",
      print_prefix);

  {
    knowledge::ContextGuard guard(context);

    madara_logger_log(context.get_logger(), logger::LOG_MINOR,
        "%s:"
        " Applying updates to context.\n",
        print_prefix);

    uint64_t now = utility::get_time();
    // apply updates from the update list
    for (knowledge::KnowledgeMap::iterator i = updates.begin();
         i != updates.end(); ++i) {
      const auto apply = [&](knowledge::KnowledgeRecord& record) {
        int result = 0;

        record.set_toi(now);
        result = record.apply(
            context, i->first, header->quality, header->clock, false);
        ++actual_updates;

        if (result != 1) {
          madara_logger_log(context.get_logger(), logger::LOG_MAJOR,
              "%s:"
              " update %s=%s was rejected\n",
              print_prefix, key.c_str(), record.to_string().c_str());
        } else {
          madara_logger_log(context.get_logger(), logger::LOG_MAJOR,
              "%s:"
              " update %s=%s was accepted\n",
              print_prefix, key.c_str(), record.to_string().c_str());
        }
      };

      auto iter = past_updates.find(i->first);
      if (iter != past_updates.end()) {
        for (auto& cur : iter->second) {
          if (cur.exists()) {
            apply(cur);
          }
        }
      }

      apply(i->second);
    }
  }

  context.set_changed();

  if (!dropped) {
    transport_context.set_operation(TransportContext::REBROADCASTING_OPERATION);

    madara_logger_log(context.get_logger(), logger::LOG_MINOR,
        "%s:"
        " Applying rebroadcast filters to receive results.\n",
        print_prefix);

    // create a list of rebroadcast records from the updates
    for (knowledge::KnowledgeMap::iterator i = updates.begin();
         i != updates.end(); ++i) {
      i->second =
          settings.filter_rebroadcast(i->second, i->first, transport_context);

      if (i->second.exists()) {
        if (i->second.to_string() != "") {
          madara_logger_log(context.get_logger(), logger::LOG_MINOR,
              "%s:"
              " Filter results for key %s were %s\n",
              print_prefix, i->first.c_str(), i->second.to_string().c_str());
        }
        rebroadcast_records[i->first] = i->second;
      } else {
        madara_logger_log(context.get_logger(), logger::LOG_MINOR,
            "%s:"
            " Filter resulted in dropping %s\n",
            print_prefix, i->first.c_str());
      }
    }

    const knowledge::KnowledgeMap& additionals =
        transport_context.get_records();

    for (knowledge::KnowledgeMap::const_iterator i = additionals.begin();
         i != additionals.end(); ++i) {
      rebroadcast_records[i->first] = i->second;
    }

    madara_logger_log(context.get_logger(), logger::LOG_MAJOR,
        "%s:"
        " Applying aggregate rebroadcast filters to %d records.\n",
        print_prefix, rebroadcast_records.size());

    // apply aggregate filters to the rebroadcast records
    if (settings.get_number_of_rebroadcast_aggregate_filters() > 0 &&
        rebroadcast_records.size() > 0) {
      settings.filter_rebroadcast(rebroadcast_records, transport_context);
    } else {
      madara_logger_log(context.get_logger(), logger::LOG_MINOR,
          "%s:"
          " No aggregate rebroadcast filters were applied...\n",
          print_prefix);
    }

    madara_logger_log(context.get_logger(), logger::LOG_MINOR,
        "%s:"
        " Returning to caller with %d rebroadcast records.\n",
        print_prefix, rebroadcast_records.size());

  } else {
    madara_logger_log(context.get_logger(), logger::LOG_MINOR,
        "%s:"
        " Rebroadcast packet was dropped...\n",
        print_prefix);
  }

  // before we send to others, we first execute rules
  if (settings.on_data_received_logic.length() != 0) {
#ifndef _MADARA_NO_KARL_
    madara_logger_log(context.get_logger(), logger::LOG_MAJOR,
        "%s:"
        " evaluating rules in %s\n",
        print_prefix, settings.on_data_received_logic.c_str());

    context.evaluate(on_data_received);
#endif  // _MADARA_NO_KARL_

  } else {
    madara_logger_log(context.get_logger(), logger::LOG_MINOR,
        "%s:"
        " no permanent rules were set\n",
        print_prefix);
  }

  return actual_updates;
}

int prep_rebroadcast(knowledge::ThreadSafeContext& context, char* buffer,
    int64_t& buffer_remaining, const QoSTransportSettings& settings,
    const char* print_prefix, MessageHeader* header,
    const knowledge::KnowledgeMap& records, PacketScheduler& packet_scheduler)
{
  int result = 0;

  if (header->ttl > 0 && records.size() > 0 && packet_scheduler.add()) {
    // keep track of the message_size portion of buffer
    uint64_t* message_size = (uint64_t*)buffer;
    int max_buffer_size = (int)buffer_remaining;

    // the number of updates will be the size of the records map
    header->updates = uint32_t(records.size());

    // set the update to the end of the header
    char* update = header->write(buffer, buffer_remaining);

    for (knowledge::KnowledgeMap::const_iterator i = records.begin();
         i != records.end(); ++i) {
      update = i->second.write(update, i->first, buffer_remaining);
    }

    if (buffer_remaining > 0) {
      int size = (int)(settings.queue_length - buffer_remaining);
      *message_size = utility::endian_swap((uint64_t)size);

      madara_logger_log(context.get_logger(), logger::LOG_MINOR,
          "%s:"
          " %" PRIu64 " bytes prepped for rebroadcast packet\n",
          print_prefix, size);

      result = size;

      madara_logger_log(context.get_logger(), logger::LOG_MAJOR,
          "%s:"
          " calling encode filters\n",
          print_prefix);

      settings.filter_encode(buffer, result, max_buffer_size);
    } else {
      madara_logger_log(context.get_logger(), logger::LOG_MAJOR,
          "%s:"
          " Not enough buffer for rebroadcasting packet\n",
          print_prefix);

      result = -2;
    }
  } else {
    madara_logger_log(context.get_logger(), logger::LOG_MINOR,
        "%s:"
        " No rebroadcast necessary.\n",
        print_prefix);

    result = -1;
  }

  packet_scheduler.print_status(logger::LOG_DETAILED, print_prefix);

  return result;
}

long Base::prep_send(const knowledge::VariableReferenceMap& orig_updates,
    const char* print_prefix)
{
  // check to see if we are shutting down
  long ret = this->check_transport();
  if (-1 == ret) {
    madara_logger_log(context_.get_logger(), logger::LOG_MAJOR,
        "%s: transport has been told to shutdown", print_prefix);

    return ret;
  } else if (-2 == ret) {
    madara_logger_log(context_.get_logger(), logger::LOG_MAJOR,
        "%s: transport is not valid", print_prefix);

    return ret;
  }

  // get the maximum quality from the updates
  uint32_t quality = knowledge::max_quality(orig_updates);
  uint64_t latest_toi = 0;
  bool reduced = false;

  knowledge::KnowledgeMap filtered_updates;

  madara_logger_log(context_.get_logger(), logger::LOG_MINOR,
      "%s:"
      " Applying filters before sending...\n",
      print_prefix);

  TransportContext transport_context(TransportContext::SENDING_OPERATION,
      receive_monitor_.get_bytes_per_second(),
      send_monitor_.get_bytes_per_second(), (uint64_t)utility::get_time(),
      (uint64_t)utility::get_time(), settings_.write_domain, id_);

  bool dropped = false;

  if (send_monitor_.is_bandwidth_violated(
          settings_.get_send_bandwidth_limit())) {
    dropped = true;
    madara_logger_log(context_.get_logger(), logger::LOG_MAJOR,
        "%s:"
        " Send monitor has detected violation of bandwidth limit."
        " Dropping packet...\n",
        print_prefix);
  } else if (receive_monitor_.is_bandwidth_violated(
                 settings_.get_total_bandwidth_limit())) {
    dropped = true;
    madara_logger_log(context_.get_logger(), logger::LOG_MAJOR,
        "%s:"
        " Receive monitor has detected violation of bandwidth limit."
        " Dropping packet...\n",
        print_prefix);
  }

  if (!dropped && packet_scheduler_.add()) {
    if (settings_.get_number_of_send_filtered_types() > 0) {
      /**
       * filter the updates according to the filters specified by
       * the user in QoSTransportSettings (if applicable)
       **/
      for (auto e : orig_updates) {
        madara_logger_log(context_.get_logger(), logger::LOG_MAJOR,
            "%s:"
            " Calling filter chain of %s.\n",
            print_prefix, e.first);

        const auto* record = e.second.get_record_unsafe();

        if (record->toi() > latest_toi) {
          latest_toi = record->toi();
        }

        // filter the record according to the send filter chain
        knowledge::KnowledgeRecord result =
            settings_.filter_send(*record, e.first, transport_context);

        madara_logger_log(context_.get_logger(), logger::LOG_MAJOR,
            "%s:"
            " Filter returned for %s.\n",
            print_prefix, e.first);

        if (result.exists()) {
          madara_logger_log(context_.get_logger(), logger::LOG_MINOR,
              "%s:"
              " Adding record to update list.\n",
              print_prefix);

          filtered_updates.emplace(std::make_pair(e.first, result));
        } else {
          madara_logger_log(context_.get_logger(), logger::LOG_MAJOR,
              "%s:"
              " Filter removed record from update list.\n",
              print_prefix);
        }
      }

      madara_logger_log(context_.get_logger(), logger::LOG_DETAILED,
          "%s:"
          " Through individual record filters. Proceeding to add update "
          "list.\n",
          print_prefix);

      const knowledge::KnowledgeMap& additionals =
          transport_context.get_records();

      for (knowledge::KnowledgeMap::const_iterator i = additionals.begin();
           i != additionals.end(); ++i) {
        madara_logger_log(context_.get_logger(), logger::LOG_MAJOR,
            "%s:"
            " Filter added a record %s to the update list.\n",
            print_prefix, i->first.c_str());
        filtered_updates.emplace(std::make_pair(i->first, i->second));
      }
    } else {
      for (auto e : orig_updates) {
        const auto* record = e.second.get_record_unsafe();

        if (record->toi() > latest_toi) {
          latest_toi = record->toi();
        }

        if (record) {
          madara_logger_log(context_.get_logger(), logger::LOG_MINOR,
              "%s:"
              " Adding record %s to update list.\n",
              print_prefix, e.first);

          filtered_updates.emplace(std::make_pair(e.first, *record));
        } else {
          std::stringstream message;
          message << print_prefix;
          message << ": record " << e.first << " produced a null record ";
          message << "from get_record_unsafe ()\n";

          madara_logger_log(
              context_.get_logger(), logger::LOG_ERROR, message.str().c_str());

          throw exceptions::MemoryException(message.str());
        }
      }
    }
  } else {
    madara_logger_log(context_.get_logger(), logger::LOG_MAJOR,
        "%s:"
        " Packet scheduler has dropped packet...\n",
        print_prefix);

    return 0;
  }

  madara_logger_log(context_.get_logger(), logger::LOG_MINOR,
      "%s:"
      " Applying %d aggregate update send filters to %d updates...\n",
      print_prefix, (int)settings_.get_number_of_send_aggregate_filters(),
      (int)filtered_updates.size());

  // apply the aggregate filters
  if (settings_.get_number_of_send_aggregate_filters() > 0 &&
      filtered_updates.size() > 0) {
    settings_.filter_send(filtered_updates, transport_context);
  } else {
    madara_logger_log(context_.get_logger(), logger::LOG_MINOR,
        "%s:"
        " No aggregate send filters were applied...\n",
        print_prefix);
  }

  packet_scheduler_.print_status(logger::LOG_DETAILED, print_prefix);

  madara_logger_log(context_.get_logger(), logger::LOG_MINOR,
      "%s:"
      " Finished applying filters before sending...\n",
      print_prefix);

  if (filtered_updates.size() == 0) {
    madara_logger_log(context_.get_logger(), logger::LOG_MINOR,
        "%s:"
        " Filters removed all data. Nothing to send.\n",
        print_prefix);

    return 0;
  }

  // allocate a buffer to send
  char* buffer = buffer_.get_ptr();
  int64_t buffer_remaining = settings_.queue_length;

  if (buffer == 0) {
    madara_logger_log(context_.get_logger(), logger::LOG_EMERGENCY,
        "%s:"
        " Unable to allocate buffer of size " PRIu32 ". Exiting thread.\n",
        print_prefix, settings_.queue_length);

    return -3;
  }

  // set the header to the beginning of the buffer
  MessageHeader* header = 0;

  if (settings_.send_reduced_message_header) {
    madara_logger_log(context_.get_logger(), logger::LOG_MINOR,
        "%s:"
        " Preparing message with reduced message header.\n",
        print_prefix);

    header = new ReducedMessageHeader();
    reduced = true;
  } else {
    madara_logger_log(context_.get_logger(), logger::LOG_MINOR,
        "%s:"
        " Preparing message with normal message header.\n",
        print_prefix);

    header = new MessageHeader();
  }

  // get the clock
  header->clock = context_.get_clock();

  if (!reduced) {
    // copy the domain from settings
    strncpy(header->domain, this->settings_.write_domain.c_str(),
        sizeof(header->domain) - 1);

    // get the quality of the key
    header->quality = quality;

    // copy the message originator (our id)
    strncpy(header->originator, id_.c_str(), sizeof(header->originator) - 1);

    // send data is generally an assign type. However, MessageHeader is
    // flexible enough to support both, and this will simply our read thread
    // handling
    header->type = MULTIASSIGN;
  }

  // set the time-to-live
  header->ttl = settings_.get_rebroadcast_ttl();

  header->updates = uint32_t(filtered_updates.size());

  // compute size of this header
  header->size = header->encoded_size();

  // keep track of the maximum buffer size for encoding
  int max_buffer_size = (int)buffer_remaining;

  // set the update to the end of the header
  char* update = header->write(buffer, buffer_remaining);
  uint64_t* message_size = (uint64_t*)buffer;
  uint32_t* message_updates = (uint32_t*)(buffer + 116);

  // Message header format
  // [size|id|domain|originator|type|updates|quality|clock|list of updates]

  /**
   * size = buffer[0] (unsigned 64 bit)
   * transport id = buffer[8] (8 byte)
   * domain = buffer[16] (32 byte domain name)
   * originator = buffer[48] (64 byte originator host:port)
   * type = buffer[112] (unsigned 32 bit type of message--usually MULTIASSIGN)
   * updates = buffer[116] (unsigned 32 bit number of updates)
   * quality = buffer[120] (unsigned 32 bit quality of message)
   * clock = buffer[124] (unsigned 64 bit clock for this message)
   * ttl = buffer[132] (the new knowledge starts here)
   * knowledge = buffer[133] (the new knowledge starts here)
   **/

  // zero out the memory
  // memset(buffer, 0, MAX_PACKET_SIZE);

  // Message update format
  // [key|value]

  int j = 0;
  uint32_t actual_updates = 0;
  for (knowledge::KnowledgeMap::const_iterator i = filtered_updates.begin();
       i != filtered_updates.end(); ++i) {
    const auto& key = i->first;
    const auto& rec = i->second;
    const auto do_write = [&](const knowledge::KnowledgeRecord& rec) {
      if (!rec.exists()) {
        madara_logger_log(context_.get_logger(), logger::LOG_MINOR,
            "%s:"
            " update[%d] => value is empty\n",
            print_prefix, j, key.c_str());
        return;
      }

      update = rec.write(update, key, buffer_remaining);

      if (buffer_remaining > 0) {
        madara_logger_log(context_.get_logger(), logger::LOG_MINOR,
            "%s:"
            " update[%d] => encoding %s of type %" PRId32 " and size %" PRIu32
            " @%" PRIu64 "\n",
            print_prefix, j, key.c_str(), rec.type(), rec.size(), rec.toi());
        ++actual_updates;
        ++j;
      } else {
        madara_logger_log(context_.get_logger(), logger::LOG_EMERGENCY,
            "%s:"
            " unable to encode update[%d] => %s of type %" PRId32
            " and size %" PRIu32 "\n",
            print_prefix, j, key.c_str(), rec.type(), rec.size());
      }
    };

    if (!settings_.send_history || !rec.has_history()) {
      do_write(rec);
    } else {
      auto buf = rec.share_circular_buffer();
      auto end = buf->end();
      auto cur = buf->begin();

      if (last_toi_sent_ > 0) {
        cur = std::upper_bound(cur, end, last_toi_sent_,
            [](uint64_t lhs, const knowledge::KnowledgeRecord& rhs) {
              return lhs < rhs.toi();
            });
      }
      for (; cur != end; ++cur) {
        do_write(*cur);
      }
    }
  }

  long size(0);

  if (buffer_remaining > 0) {
    size = (long)(settings_.queue_length - buffer_remaining);
    header->size = size;
    *message_size = utility::endian_swap((uint64_t)size);
    header->updates = actual_updates;
    *message_updates = utility::endian_swap(actual_updates);

    // before we send to others, we first execute rules
    if (settings_.on_data_received_logic.length() != 0) {
#ifndef _MADARA_NO_KARL_

      madara_logger_log(context_.get_logger(), logger::LOG_MAJOR,
          "%s:"
          " evaluating rules in %s\n",
          print_prefix, settings_.on_data_received_logic.c_str());

      on_data_received_.evaluate();

      madara_logger_log(context_.get_logger(), logger::LOG_MAJOR,
          "%s:"
          " rules have been successfully evaluated\n",
          print_prefix);

#endif  // _MADARA_NO_KARL_

    } else {
      madara_logger_log(context_.get_logger(), logger::LOG_MINOR,
          "%s:"
          " no permanent rules were set\n",
          print_prefix);
    }
  }

  madara_logger_log(context_.get_logger(), logger::LOG_MAJOR,
      "%s:"
      " calling encode filters\n",
      print_prefix);

  // buffer is ready encoding
  size = (long)settings_.filter_encode(
      buffer_.get_ptr(), (int)size, max_buffer_size);

  madara_logger_log(context_.get_logger(), logger::LOG_MINOR,
      "%s:"
      " header info before encode: %s\n",
      print_prefix, header->to_string().c_str());

  delete header;

  last_toi_sent_ = latest_toi;

  return size;
}
}
}
