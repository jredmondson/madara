#include "madara/transport/zmq/ZMQTransportReadThread.h"

#include "madara/transport/ReducedMessageHeader.h"
#include "madara/transport/Fragmentation.h"
#include "madara/utility/Utility.h"
#include "ZMQContext.h"

#include <iostream>
#include <algorithm>

madara::transport::ZMQTransportReadThread::ZMQTransportReadThread(
    const TransportSettings& settings, const std::string& id,
    void* write_socket, BandwidthMonitor& send_monitor,
    BandwidthMonitor& receive_monitor, PacketScheduler& packet_scheduler)
  : settings_(settings),
    id_(id),
    context_(0),
    write_socket_(write_socket),
    read_socket_(0),
    send_monitor_(send_monitor),
    receive_monitor_(receive_monitor),
    packet_scheduler_(packet_scheduler)
{
}

void madara::transport::ZMQTransportReadThread::init(
    knowledge::KnowledgeBase& knowledge)
{
  context_ = &(knowledge.get_context());

  if (!settings_.no_receiving) {
    // int send_buff_size = 0;
    int rcv_buff_size = 0;
    int timeout = 1000;
    int buff_size = settings_.queue_length;
    int result;
    int zero = 0;
    size_t opt_len = sizeof(int);

    if (settings_.debug_to_kb_prefix != "") {
      received_packets_.set_name(
          settings_.debug_to_kb_prefix + ".received_packets", knowledge);
      failed_receives_.set_name(
          settings_.debug_to_kb_prefix + ".failed_receives", knowledge);
      received_data_max_.set_name(
          settings_.debug_to_kb_prefix + ".received_data_max", knowledge);
      received_data_min_.set_name(
          settings_.debug_to_kb_prefix + ".received_data_min", knowledge);
      received_data_.set_name(
          settings_.debug_to_kb_prefix + ".received_data", knowledge);
    }

    // setup the receive buffer
    if (settings_.queue_length > 0)
      buffer_ = new char[settings_.queue_length];

    madara_logger_log(context_->get_logger(), logger::LOG_MAJOR,
        "ZMQTransportReadThread::init:"
        " setting up read socket\n");

    read_socket_ = zmq_socket(zmq_context.get_context(), ZMQ_SUB);

    if (read_socket_ == NULL) {
      madara_logger_log(context_->get_logger(), logger::LOG_ERROR,
          "ZMQTransportReadThread::init:"
          " ERROR: could not create SUB socket\n");
      madara_logger_log(context_->get_logger(), logger::LOG_ERROR,
          "ZMQTransportReadThread::init:"
          " ERROR: errno = %s\n",
          zmq_strerror(zmq_errno()));
    }

    // subscribe to all messages
    result = zmq_setsockopt(read_socket_, ZMQ_SUBSCRIBE, 0, 0);

    if (result == 0) {
      madara_logger_log(context_->get_logger(), logger::LOG_MAJOR,
          "ZMQTransportReadThread::init:"
          " successfully set sockopt for ZMQ_SUBSCRIBE\n");
    } else {
      madara_logger_log(context_->get_logger(), logger::LOG_ERROR,
          "ZMQTransportReadThread::init:"
          " ERROR: errno = %s\n",
          zmq_strerror(zmq_errno()));
    }

    // if you don't do this, ZMQ waits forever for no reason. Super smart.
    result =
        zmq_setsockopt(read_socket_, ZMQ_LINGER, (void*)&zero, sizeof(int));

    madara_logger_log(context_->get_logger(), logger::LOG_MAJOR,
        "ZMQTransportReadThread::init:"
        " setting rcv buff size to settings.queue_length (%d)\n",
        buff_size);

    result =
        zmq_setsockopt(read_socket_, ZMQ_RCVBUF, (void*)&buff_size, opt_len);

    if (result == 0) {
      result = zmq_getsockopt(
          read_socket_, ZMQ_RCVBUF, (void*)&rcv_buff_size, &opt_len);

      madara_logger_log(context_->get_logger(), logger::LOG_MAJOR,
          "ZMQTransportReadThread::init:"
          " successfully set sockopt rcvbuf size to %d. Actual %d allocated\n",
          buff_size, rcv_buff_size);
    } else {
      madara_logger_log(context_->get_logger(), logger::LOG_ERROR,
          "ZMQTransportReadThread::init:"
          " ERROR: errno = %s\n",
          zmq_strerror(zmq_errno()));
    }

    result =
        zmq_setsockopt(read_socket_, ZMQ_RCVTIMEO, (void*)&timeout, opt_len);

    if (result == 0) {
      result =
          zmq_getsockopt(read_socket_, ZMQ_RCVTIMEO, (void*)&timeout, &opt_len);

      madara_logger_log(context_->get_logger(), logger::LOG_MAJOR,
          "ZMQTransportReadThread::init:"
          " successfully set rcv timeout to %d\n",
          timeout);
    } else {
      madara_logger_log(context_->get_logger(), logger::LOG_ERROR,
          "ZMQTransportReadThread::init:"
          " ERROR: When setting timeout on rcv, errno = %s\n",
          zmq_strerror(zmq_errno()));
    }

    if (settings_.hosts.size() >= 1) {
      // if the first host was a reliable multicast, we need to connect to it
      if (utility::begins_with(settings_.hosts[0], "pgm") ||
          utility::begins_with(settings_.hosts[0], "epgm")) {
      }
    }

    // connect the reader to the host sockets
    size_t i = 0;

    if (settings_.hosts.size() >= 1) {
      // we ignore first host unless it is pgm because writer is on first host
      if (!utility::begins_with(settings_.hosts[0], "pgm") &&
          !utility::begins_with(settings_.hosts[0], "epgm")) {
        ++i;
      }
    }

    for (; i < settings_.hosts.size(); ++i) {
      int connect_result =
          zmq_connect(read_socket_, settings_.hosts[i].c_str());

      if (connect_result == 0) {
        madara_logger_log(context_->get_logger(), logger::LOG_MAJOR,
            "ZMQTransportReadThread::init:"
            " successfully connected to %s\n",
            settings_.hosts[i].c_str());
      } else {
        madara_logger_log(context_->get_logger(), logger::LOG_ERROR,
            "ZMQTransportReadThread::init:"
            " ERROR: could not connect to %s\n",
            settings_.hosts[i].c_str());
        madara_logger_log(context_->get_logger(), logger::LOG_ERROR,
            "ZMQTransportReadThread::init:"
            " ERROR: errno = %s\n",
            zmq_strerror(zmq_errno()));
      }
    }
  }

  if (context_) {
    // check for an on_data_received ruleset
    if (settings_.on_data_received_logic.length() != 0) {
      madara_logger_log(this->context_->get_logger(), logger::LOG_MAJOR,
          "ZMQTransportReadThread::init:"
          " setting rules to %s\n",
          settings_.on_data_received_logic.c_str());

#ifndef _MADARA_NO_KARL_
      madara::expression::Interpreter interpreter;
      on_data_received_ = context_->compile(settings_.on_data_received_logic);
#endif  // _MADARA_NO_KARL_
    } else {
      madara_logger_log(this->context_->get_logger(), logger::LOG_MAJOR,
          "ZMQTransportReadThread::init:"
          " no permanent rules were set\n");
    }
  }
}

void madara::transport::ZMQTransportReadThread::cleanup(void)
{
  madara_logger_log(context_->get_logger(), logger::LOG_MAJOR,
      "ZMQTransportReadThread::cleanup:"
      " starting cleanup\n");

  if (read_socket_ != 0) {
    madara_logger_log(context_->get_logger(), logger::LOG_MAJOR,
        "ZMQTransportReadThread::cleanup:"
        " closing read socket\n");

    int result = zmq_close(read_socket_);
    read_socket_ = 0;

    if (result != 0) {
      madara_logger_log(context_->get_logger(), logger::LOG_ERROR,
          "ZMQTransportReadThread::cleanup:"
          " ERROR: errno = %s\n",
          zmq_strerror(zmq_errno()));
    }
  }

  madara_logger_log(context_->get_logger(), logger::LOG_MAJOR,
      "ZMQTransportReadThread::cleanup:"
      " finished cleanup\n");
}

void madara::transport::ZMQTransportReadThread::rebroadcast(
    const char* print_prefix, MessageHeader* header,
    const knowledge::KnowledgeMap& records)
{
  int64_t buffer_remaining = (int64_t)settings_.queue_length;
  char* buffer = buffer_.get_ptr();
  int result(0);

  if (!settings_.no_sending) {
    result = prep_rebroadcast(*context_, buffer, buffer_remaining, settings_,
        print_prefix, header, records, packet_scheduler_);

    if (result > 0) {
      if (settings_.hosts.size() > 0 && result > 0) {
        madara_logger_log(context_->get_logger(), logger::LOG_MAJOR,
            "ZMQTransportReadThread::send:"
            " sending %d bytes on socket\n",
            result);

        // send the prepped buffer over ZeroMQ
        result = zmq_send(write_socket_, (void*)buffer_.get_ptr(),
            (size_t)result, ZMQ_DONTWAIT);

        madara_logger_log(context_->get_logger(), logger::LOG_MAJOR,
            "ZMQTransportReadThread::send:"
            " sent %d bytes on socket\n",
            result);
      }
    }
  }
}

void madara::transport::ZMQTransportReadThread::run(void)
{
  if (!settings_.no_receiving) {
    // allocate a buffer to send
    char* buffer = buffer_.get_ptr();
    const char* print_prefix = "ZMQTransportReadThread::run";
    int64_t buffer_remaining = settings_.queue_length;
    size_t zmq_buffer_size = buffer_remaining;

    madara_logger_log(context_->get_logger(), logger::LOG_MAJOR,
        "%s:"
        " entering main service loop.\n",
        print_prefix);

    knowledge::KnowledgeMap rebroadcast_records;

    if (buffer == 0) {
      madara_logger_log(context_->get_logger(), logger::LOG_MINOR,
          "%s:"
          " Unable to allocate buffer of size " PRIu32 ". Exiting thread.\n",
          print_prefix, settings_.queue_length);

      return;
    }

    madara_logger_log(context_->get_logger(), logger::LOG_MINOR,
        "%s:"
        " entering a recv on the socket.\n",
        print_prefix);

    // blocking receive up to rcv timeout (1 second)
    buffer_remaining =
        (int64_t)zmq_recv(read_socket_, (void*)buffer, zmq_buffer_size, 0);

    madara_logger_log(context_->get_logger(), logger::LOG_MINOR,
        "%s:"
        " past recv on the socket.\n",
        print_prefix);

    if (buffer_remaining > 0) {
      if (settings_.debug_to_kb_prefix != "") {
        received_data_ += buffer_remaining;
        ++received_packets_;

        if (received_data_max_ < buffer_remaining) {
          received_data_max_ = buffer_remaining;
        }
        if (received_data_min_ > buffer_remaining || received_data_min_ == 0) {
          received_data_min_ = buffer_remaining;
        }
      }

      MessageHeader* header = (MessageHeader*)buffer;

      madara_logger_log(context_->get_logger(), logger::LOG_MINOR,
          "%s:"
          " processing %d byte update from %s.\n",
          print_prefix, (int)buffer_remaining, header->originator);

      process_received_update(buffer, (uint32_t)buffer_remaining, id_,
          *context_, settings_, send_monitor_, receive_monitor_,
          rebroadcast_records,
#ifndef _MADARA_NO_KARL_
          on_data_received_,
#endif  // _MADARA_NO_KARL_
          print_prefix, header->originator, header);

      madara_logger_log(context_->get_logger(), logger::LOG_MINOR,
          "%s:"
          " done processing %d byte update from %s.\n",
          print_prefix, (int)buffer_remaining, header->originator);

      if (header) {
        // delete header
        delete header;
      }
    } else {
      madara_logger_log(context_->get_logger(), logger::LOG_MAJOR,
          "%s:"
          " wait timeout on new messages. Proceeding to next wait\n",
          print_prefix);

      if (settings_.debug_to_kb_prefix != "") {
        ++failed_receives_;
      }
    }
  }
}
