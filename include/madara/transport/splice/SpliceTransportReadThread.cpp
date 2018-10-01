#include "madara/transport/splice/SpliceTransportReadThread.h"
#include "madara/knowledge/UpdateTypes.h"
#include "madara/utility/Utility.h"
#include "madara/logger/GlobalLogger.h"

namespace logger = madara::logger;
namespace utility = madara::utility;

#include <iostream>
#include <sstream>

madara::transport::SpliceReadThread::SpliceReadThread(const std::string& id,
    const TransportSettings& settings,
    madara::knowledge::ThreadSafeContext& context,
    Knowledge::UpdateDataReader_ptr& update_reader,
    Knowledge::UpdateDataWriter_ptr& update_writer,
    BandwidthMonitor& send_monitor, BandwidthMonitor& receive_monitor,
    PacketScheduler& packet_scheduler)
  : id_(id),
    settings_(settings),
    context_(&context),
    update_reader_(update_reader),
    update_writer_(update_writer),
    send_monitor_(send_monitor),
    receive_monitor_(receive_monitor),
    packet_scheduler_(packet_scheduler)
{
}

void madara::transport::SpliceReadThread::init(
    knowledge::KnowledgeBase& knowledge)
{
  context_ = &(knowledge.get_context());

  // setup the receive buffer
  if (settings_.queue_length > 0)
    buffer_ = new char[settings_.queue_length];

  if (context_)
  {
    // check for an on_data_received ruleset
    if (settings_.on_data_received_logic.length() != 0)
    {
#ifndef _MADARA_NO_KARL_
      madara_logger_log(context_->get_logger(), logger::LOG_MAJOR,
          "UdpTransportReadThread::init:"
          " setting rules to %s\n",
          settings_.on_data_received_logic.c_str());

      madara::expression::Interpreter interpreter;
      on_data_received_ = context_->compile(settings_.on_data_received_logic);
#endif  // _MADARA_NO_KARL_
    }
    else
    {
      madara_logger_log(context_->get_logger(), logger::LOG_MINOR,
          "UdpTransportReadThread::init:"
          " no permanent rules were set\n");
    }
  }
}

void madara::transport::SpliceReadThread::cleanup(void) {}

void madara::transport::SpliceReadThread::rebroadcast(const char* print_prefix,
    MessageHeader* header, const knowledge::KnowledgeMap& records)
{
  int64_t buffer_remaining = (int64_t)settings_.queue_length;
  char* buffer = buffer_.get_ptr();
  unsigned long result = prep_rebroadcast(*context_, buffer, buffer_remaining,
      settings_, print_prefix, header, records, packet_scheduler_);

  if (result > 0)
  {
    ssize_t bytes_sent(result + sizeof(Knowledge::Update));
    DDS::ReturnCode_t dds_result;
    DDS::InstanceHandle_t handle;

    // get the maximum quality from the updates
    uint32_t quality = knowledge::max_quality(records);

    /// get current lamport clock.
    unsigned long long cur_clock = context_->get_clock();

    Knowledge::Update data;

    data.buffer =
        Knowledge::seq_oct(result, result, (unsigned char*)buffer_.get_ptr());
    data.clock = cur_clock;
    data.quality = quality;
    data.updates = DDS::ULong(records.size());
    data.originator = DDS::string_dup(id_.c_str());
    data.type = madara::transport::MULTIASSIGN;
    data.ttl = settings_.get_rebroadcast_ttl();
    data.timestamp = utility::get_time();
    data.madara_id = DDS::string_dup(MADARA_IDENTIFIER);

    handle = update_writer_->register_instance(data);
    dds_result = update_writer_->write(data, handle);

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

void madara::transport::SpliceReadThread::run(void)
{
  DDS::SampleInfoSeq_var infoList = new DDS::SampleInfoSeq;
  DDS::ReturnCode_t dds_result;
  int amount;
  DDS::Boolean result = false;
  const char* print_prefix = "SpliceReadThread::svc";
  Knowledge::UpdateSeq_var update_data_list_ = new Knowledge::UpdateSeq;

  DDS::WaitSet waitset_;
  DDS::StatusCondition_ptr condition_;
  // Add update datareader statuscondition to waitset
  condition_ = update_reader_->get_statuscondition();
  condition_->set_enabled_statuses(DDS::DATA_AVAILABLE_STATUS);
  waitset_.attach_condition(condition_);

  ::DDS::Duration_t wait_time;
  wait_time.sec = 3;
  wait_time.nanosec = 0;

  // if we don't check originator for null, we get phantom sends
  // when the program exits.
  madara_logger_log(context_->get_logger(), logger::LOG_MAJOR,
      "%s: entering processing loop.\n", print_prefix);

  // by using conditionals, we can wait for a message for a specific time limit
  // the conditionList would tell us which particular conditions were met, but
  // since we've only set up the wait
  DDS::ConditionSeq_var conditionList = new DDS::ConditionSeq();
  result = waitset_.wait(conditionList.inout(), wait_time);

  madara_logger_log(context_->get_logger(), logger::LOG_DETAILED,
      "%s: entering a take on the DDS reader.\n", print_prefix);

  dds_result = update_reader_->take(update_data_list_, infoList, 20,
      DDS::ANY_SAMPLE_STATE, DDS::ANY_VIEW_STATE, DDS::ANY_INSTANCE_STATE);

  amount = update_data_list_->length();

  if (amount != 0)
  {
    for (int i = 0; i < amount; ++i)
    {
      // if we are evaluating a message from ourselves, just continue
      // to the next one. It's also possible to receive null originators
      // from what I can only guess is the ospl daemon messing up
      if (!update_data_list_[i].originator.val())
      {
        // if we don't check originator for null, we get phantom sends
        // when the program exits.
        madara_logger_log(context_->get_logger(), logger::LOG_DETAILED,
            "%s: discarding null originator event.\n", print_prefix);

        continue;
      }

      if (update_data_list_[i].type != madara::transport::MULTIASSIGN)
      {
        madara_logger_log(context_->get_logger(), logger::LOG_DETAILED,
            "%s: discarding non-assignment event.\n", print_prefix);
        // we do not allow any other type than multiassign

        continue;
      }

      knowledge::KnowledgeMap rebroadcast_records;
      MessageHeader* header = 0;

      process_received_update((char*)update_data_list_[i].buffer.get_buffer(),
          update_data_list_[i].buffer.length(), id_, *context_, settings_,
          send_monitor_, receive_monitor_, rebroadcast_records,
          on_data_received_, print_prefix, "", header);

      if (header)
      {
        if (header->ttl > 0 && rebroadcast_records.size() > 0 &&
            settings_.get_participant_ttl() > 0)
        {
          --header->ttl;
          header->ttl = std::min(settings_.get_participant_ttl(), header->ttl);

          rebroadcast(print_prefix, header, rebroadcast_records);
        }

        // delete header
        delete header;
      }
    }
  }

  dds_result = update_reader_->return_loan(update_data_list_, infoList);
}
