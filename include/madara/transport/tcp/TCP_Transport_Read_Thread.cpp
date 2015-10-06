#include "madara/transport/tcp/TCP_Transport_Read_Thread.h"


#include <iostream>

madara::transport::TCP_Transport_Read_Thread::TCP_Transport_Read_Thread (
  const Settings & settings,
  const std::string & id,
  std::map <std::string, ACE_INET_Addr> & addresses,
  Bandwidth_Monitor & send_monitor,
  Bandwidth_Monitor & receive_monitor,
  Packet_Scheduler & packet_scheduler)
  : settings_ (settings), id_ (id), context_ (0),
  addresses_ (addresses),
  send_monitor_ (send_monitor),
  receive_monitor_ (receive_monitor),
  packet_scheduler_ (packet_scheduler)
{
}

void
madara::transport::TCP_Transport_Read_Thread::init (
knowledge::Knowledge_Base & knowledge)
{
  context_ = &(knowledge.get_context ());

  // setup the receive buffer
  if (settings_.queue_length > 0)
    buffer_ = new char[settings_.queue_length];

  if (context_)
  {
    // check for an on_data_received ruleset
    if (settings_.on_data_received_logic.length () != 0)
    {

#ifndef _MADARA_NO_KARL_
      madara_logger_log (context_->get_logger (), logger::LOG_MAJOR,
        "TCP_Transport_Read_Thread::init:" \
        " setting rules to %s\n",
        settings_.on_data_received_logic.c_str ());

      madara::expression::Interpreter interpreter;
      on_data_received_ = context_->compile (settings_.on_data_received_logic);
#endif // _MADARA_NO_KARL_
    }
    else
    {
      madara_logger_log (context_->get_logger (), logger::LOG_MINOR,
        "TCP_Transport_Read_Thread::init:" \
        " no permanent rules were set");
    }
  }
}

void
madara::transport::TCP_Transport_Read_Thread::rebroadcast (
const char * print_prefix,
Message_Header * header,
const Knowledge_Map & records)
{
  int64_t buffer_remaining = (int64_t)settings_.queue_length;
  char * buffer = buffer_.get_ptr ();
  int result (0);

  if (!settings_.no_sending)
  {
    result = prep_rebroadcast (*context_, buffer, buffer_remaining,
      settings_, print_prefix,
      header, records,
      packet_scheduler_);

    if (result > 0)
    {
      uint64_t bytes_sent = 0;
      uint64_t packet_size = (uint64_t)result;

      if (packet_size > settings_.max_fragment_size)
      {
        Fragment_Map map;

        madara_logger_log (context_->get_logger (), logger::LOG_MAJOR,
          "%s:" \
          " fragmenting %" PRIu64 " byte packet (%" PRIu32 " bytes is max fragment size)\n",
          print_prefix, packet_size, settings_.max_fragment_size);

        // fragment the message
        frag (buffer_.get_ptr (), settings_.max_fragment_size, map);

        for (Fragment_Map::iterator i = map.begin (); i != map.end (); ++i)
        {
          size_t frag_size =
            (size_t)Message_Header::get_size (i->second);

          for (std::map <std::string, ACE_INET_Addr>::const_iterator addr =
            addresses_.begin (); addr != addresses_.end (); ++addr)
          {
            if (addr->first != settings_.hosts[0])
            {
              ssize_t actual_sent = -1;

              // sleep between fragments, if such a slack time is specified
              if (settings_.slack_time > 0)
                madara::utility::sleep (settings_.slack_time);

              madara_logger_log (context_->get_logger (), logger::LOG_MAJOR,
                "%s:" \
                " Send result was %d of %d byte fragment to %s\n",
                print_prefix, actual_sent, frag_size, addr->first.c_str ());

              if (actual_sent > 0)
              {
                send_monitor_.add ((uint32_t)frag_size);
                bytes_sent += actual_sent;
              }
            }
          }
        }

        madara_logger_log (context_->get_logger (), logger::LOG_MAJOR,
          "%s:" \
          " Sent fragments totalling %" PRIu64 " bytes\n",
          print_prefix, bytes_sent);

        delete_fragments (map);
      }
      else
      {
        for (std::map <std::string, ACE_INET_Addr>::const_iterator i =
          addresses_.begin (); i != addresses_.end (); ++i)
        {
          if (i->first != settings_.hosts[0])
          {
            ssize_t actual_sent = -1;

            madara_logger_log (context_->get_logger (), logger::LOG_MAJOR,
              "%s:" \
              " Sent %d packet to %s\n",
              print_prefix, packet_size, i->first.c_str ());

            if (actual_sent > 0)
            {
              send_monitor_.add ((uint32_t)actual_sent);
              bytes_sent += actual_sent;
            }
          }
        }

        madara_logger_log (context_->get_logger (), logger::LOG_MAJOR,
          "%s:" \
          " Sent %d total bytes via rebroadcast\n",
          print_prefix, bytes_sent);
      }

      madara_logger_log (context_->get_logger (), logger::LOG_MAJOR,
        "%s:" \
        " Send bandwidth = %" PRIu64 " B/s\n",
        print_prefix,
        send_monitor_.get_bytes_per_second ());
    }
  }
}

void
madara::transport::TCP_Transport_Read_Thread::cleanup (void)
{
}


void
madara::transport::TCP_Transport_Read_Thread::run (void)
{
  if (!settings_.no_receiving)
  {
    ACE_Time_Value wait_time (1);
    ACE_INET_Addr  remote;

    // allocate a buffer to send
    char * buffer = buffer_.get_ptr ();
    const char * print_prefix = "TCP_Transport_Read_Thread::run";
    int64_t buffer_remaining = settings_.queue_length;

    madara_logger_log (context_->get_logger (), logger::LOG_MAJOR,
      "%s:" \
      " entering main service loop.\n",
      print_prefix);

    Knowledge_Map rebroadcast_records;

    if (buffer == 0)
    {
      madara_logger_log (context_->get_logger (), logger::LOG_MINOR,
        "%s:" \
        " Unable to allocate buffer of size " PRIu32 ". Exiting thread.\n",
        print_prefix,
        settings_.queue_length);

      return;
    }

    madara_logger_log (context_->get_logger (), logger::LOG_MINOR,
      "%s:" \
      " entering a recv on the socket.\n",
      print_prefix);

    ssize_t bytes_read = 0;

    madara_logger_log (context_->get_logger (), logger::LOG_MAJOR,
      "%s:" \
      " received a message header of %" PRIu64 " bytes from %s:%d\n",
      print_prefix,
      bytes_read,
      remote.get_host_addr (), remote.get_port_number ());

    if (bytes_read > 0)
    {
      Message_Header * header = 0;

      std::stringstream remote_host;
      remote_host << remote.get_host_addr ();
      remote_host << ":";
      remote_host << remote.get_port_number ();

      process_received_update (buffer, bytes_read, id_, *context_,
        settings_, send_monitor_, receive_monitor_, rebroadcast_records,
#ifndef _MADARA_NO_KARL_
        on_data_received_,
#endif // _MADARA_NO_KARL_
        print_prefix,
        remote_host.str ().c_str (), header);

      if (header)
      {
        if (header->ttl > 0 && rebroadcast_records.size () > 0 &&
          settings_.get_participant_ttl () > 0)
        {
          --header->ttl;
          header->ttl = std::min (
            settings_.get_participant_ttl (), header->ttl);

          rebroadcast (print_prefix, header, rebroadcast_records);
        }

        // delete header
        delete header;
      }
    }
    else
    {
      madara_logger_log (context_->get_logger (), logger::LOG_MAJOR,
        "%s:" \
        " wait timeout on new messages. Proceeding to next wait\n",
        print_prefix);
    }
  }
}
