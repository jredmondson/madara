#ifdef _MADARA_NDDS_

#include "NddsListener.h"
#include "madara/knowledge/UpdateTypes.h"
#include "madara/utility/Utility.h"
#include "madara/logger/GlobalLogger.h"

namespace logger = madara::logger;

#include <sstream>

madara::transport::NddsListener::NddsListener(const TransportSettings& settings,
    const std::string& id, knowledge::ThreadSafeContext& context,
    BandwidthMonitor& send_monitor, BandwidthMonitor& receive_monitor,
    PacketScheduler& packet_scheduler)
  : settings_(settings),
    id_(id),
    context_(&context),
    send_monitor_(send_monitor),
    receive_monitor_(receive_monitor),
    packet_scheduler_(packet_scheduler)
{
  // setup the receive buffer
  if (settings_.queue_length > 0)
    buffer_ = new char[settings_.queue_length];

  // check for an on_data_received ruleset
  if (settings_.on_data_received_logic.length() != 0)
  {
    madara_logger_log(context_->get_logger(), logger::LOG_MAJOR,
        "NddsListener::NddsListener:"
        " setting rules to %s\n",
        settings_.on_data_received_logic.c_str());

    on_data_received_ = context_->compile(settings_.on_data_received_logic);
  }
  else
  {
    madara_logger_log(context_->get_logger(), logger::LOG_MINOR,
        "NddsListener::NddsListener:"
        " no permanent rules were set\n");
  }
}

madara::transport::NddsListener::NddsListener(const NddsListener& ref)
  : settings_(ref.settings_),
    id_(ref.id_),
    context_(ref.context_),
    send_monitor_(ref.send_monitor_),
    receive_monitor_(ref.receive_monitor_),
    packet_scheduler_(ref.packet_scheduler_)
{
}

madara::transport::NddsListener::~NddsListener() {}

void madara::transport::NddsListener::on_subscription_matched(
    DDSDataReader* reader, const DDS_SubscriptionMatchedStatus& status)
{
  context_->set_changed();
}

void madara::transport::NddsListener::rebroadcast(const char* print_prefix,
    MessageHeader* header, const knowledge::KnowledgeMap& records)
{
  int64_t buffer_remaining = (int64_t)settings_.queue_length;
  char* buffer = buffer_.get_ptr();
  unsigned long result = prep_rebroadcast(*context_, buffer, buffer_remaining,
      settings_, print_prefix, header, records, packet_scheduler_);

  if (result > 0)
  {
    ssize_t bytes_sent(result + sizeof(Ndds_Knowledge_Update));

    madara_logger_log(context_->get_logger(), logger::LOG_MAJOR,
        "%s:"
        " Sent packet of size %d\n",
        print_prefix, bytes_sent);

    send_monitor_.add((uint32_t)bytes_sent);

    madara_logger_log(context_->get_logger(), logger::LOG_MINOR,
        "%s:"
        " Send bandwidth = %d B/s\n",
        print_prefix, send_monitor_.get_bytes_per_second());
  }
}

void madara::transport::NddsListener::on_data_available(DDSDataReader* reader)
{
  Ndds_Knowledge_UpdateSeq update_data_list;
  DDS_SampleInfoSeq info_seq;
  DDS_ReturnCode_t rc;

  const char* print_prefix = "NddsListener::svc";

  Ndds_Knowledge_UpdateDataReader* update_reader =
      Ndds_Knowledge_UpdateDataReader::narrow(reader);
  if (update_reader == NULL)
  {
    madara_logger_log(context_->get_logger(), logger::LOG_ERROR,
        "%s:"
        " Unable to create specialized reader. Leaving callback...\n",
        print_prefix);

    return;
  }

  rc = update_reader->take(update_data_list, info_seq, DDS_LENGTH_UNLIMITED,
      DDS_ANY_SAMPLE_STATE, DDS_ANY_VIEW_STATE, DDS_ANY_INSTANCE_STATE);

  if (rc == DDS_RETCODE_NO_DATA)
  {
    return;
  }
  else if (rc != DDS_RETCODE_OK)
  {
    madara_logger_log(context_->get_logger(), logger::LOG_MINOR,
        "%s:"
        " could not take current sample.\n",
        print_prefix);
    return;
  }

  for (int i = 0; i < update_data_list.length(); ++i)
  {
    if (info_seq[i].valid_data)
    {
      // if we are evaluating a message from ourselves, just continue
      // to the next one. It's also possible to receive null originators
      // from what I can only guess is the ospl daemon messing up
      if (strncmp(update_data_list[i].originator, "", 1) == 0)
      {
        // if we don't check originator for null, we get phantom sends
        // when the program exits.
        madara_logger_log(context_->get_logger(), logger::LOG_DETAILED,
            "%s:"
            " discarding null originator event.\n",
            print_prefix);

        continue;
      }

      if (update_data_list[i].type != madara::transport::MULTIASSIGN)
      {
        // we do not allow any other type than multiassign
        madara_logger_log(context_->get_logger(), logger::LOG_DETAILED,
            "%s:"
            " discarding non-assignment event.\n",
            print_prefix);

        continue;
      }

      knowledge::KnowledgeMap rebroadcast_records;
      MessageHeader* header = 0;

      if (knowledge::MULTIPLE_ASSIGNMENT == update_data_list[i].type)
      {
        madara_logger_log(context_->get_logger(), logger::LOG_DETAILED,
            "%s:"
            " processing multassignment from %s with time %llu and quality "
            "%d.\n",
            print_prefix, update_data_list[i].originator,
            update_data_list[i].clock, update_data_list[i].quality);

        process_received_update(
            (char*)update_data_list[i].buffer.get_contiguous_buffer(),
            update_data_list[i].buffer.length(), id_, *context_, settings_,
            send_monitor_, receive_monitor_, rebroadcast_records,
            on_data_received_, print_prefix, "", header);
      }
    }
  }

  rc = update_reader->return_loan(update_data_list, info_seq);
  if (rc != DDS_RETCODE_OK)
  {
    madara_logger_log(context_->get_logger(), logger::LOG_DETAILED,
        "%s:"
        " could return DDS sample instance loan.\n",
        print_prefix);
  }
}

#endif
