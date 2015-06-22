#include "madara/transport/broadcast/Broadcast_Transport_Read_Thread.h"

#include "madara/utility/Utility.h"
#include "madara/transport/Reduced_Message_Header.h"
#include "ace/Time_Value.h"
#include "madara/logger/Global_Logger.h"

#include <iostream>

Madara::Transport::Broadcast_Transport_Read_Thread::Broadcast_Transport_Read_Thread (
  const Settings & settings,
  const std::string & id,
  const ACE_INET_Addr & address,
  ACE_SOCK_Dgram_Bcast & socket,
  Bandwidth_Monitor & send_monitor,
  Bandwidth_Monitor & receive_monitor,
  Packet_Scheduler & packet_scheduler)
  : settings_ (settings), id_ (id), context_ (0),
    address_ (address.get_port_number ()),
    write_socket_ (socket),
    send_monitor_ (send_monitor),
    receive_monitor_ (receive_monitor),
    packet_scheduler_ (packet_scheduler)
{
  // for receiving, we only want to bind to the local port
  ACE_INET_Addr local (address.get_port_number ());
  
  qos_settings_ = dynamic_cast <const QoS_Transport_Settings *> (&settings);

  if (-1 == read_socket_.open (local, 2, 0, 1))
  {
    Logger::global_logger->log (Logger::LOG_ERROR,
      "Broadcast_Transport_Read_Thread::constructor:" \
      " Error subscribing to broadcast address %s:%d\n",
      address_.get_host_addr (), address_.get_port_number ());
  } 
  else
  {
    Logger::global_logger->log (Logger::LOG_MAJOR,
      "Broadcast_Transport_Read_Thread::constructor:" \
      " Success subscribing to broadcast address %s:%d\n",
      address_.get_host_addr (), address_.get_port_number ());

    int send_buff_size = 0, tar_buff_size (settings_.queue_length);
    int rcv_buff_size = 0;
    int opt_len = sizeof (int);
    
    ACE_SOCK_Dgram & bare_socket = read_socket_;

    bare_socket.get_option (SOL_SOCKET, SO_RCVBUF,
      (void *)&rcv_buff_size, &opt_len);

    Logger::global_logger->log (Logger::LOG_MAJOR,
      "Broadcast_Transport_Read_Thread::constructor:" \
      " default socket buff size is send=%d, rcv=%d\n",
      send_buff_size, rcv_buff_size);
  
    if (send_buff_size < tar_buff_size)
    {
      Logger::global_logger->log (Logger::LOG_MAJOR,
        "Broadcast_Transport_Read_Thread::constructor:" \
        " setting send buff size to settings.queue_length (%d)\n",
        tar_buff_size);
  
      bare_socket.set_option (SOL_SOCKET, SO_SNDBUF,
        (void *)&tar_buff_size, opt_len);
    
      bare_socket.get_option (SOL_SOCKET, SO_SNDBUF,
        (void *)&send_buff_size, &opt_len);

      Logger::global_logger->log (Logger::LOG_MAJOR,
        "Broadcast_Transport_Read_Thread::constructor:" \
        " current socket buff size is send=%d, rcv=%d\n",
        send_buff_size, rcv_buff_size);
    }
  
    if (rcv_buff_size < tar_buff_size)
    {
      Logger::global_logger->log (Logger::LOG_MAJOR,
        "Broadcast_Transport_Read_Thread::constructor:" \
        " setting rcv buff size to settings.queue_length (%d)\n",
        tar_buff_size);
  
      bare_socket.set_option (SOL_SOCKET, SO_SNDBUF,
        (void *)&tar_buff_size, opt_len);
    
      bare_socket.get_option (SOL_SOCKET, SO_SNDBUF,
        (void *)&rcv_buff_size, &opt_len);

      Logger::global_logger->log (Logger::LOG_MAJOR,
        "Broadcast_Transport_Read_Thread::constructor:" \
        " current socket buff size is send=%d, rcv=%d\n",
        send_buff_size, rcv_buff_size);
    }
    
    // setup the receive buffer
    if (settings_.queue_length > 0)
      buffer_ = new char [settings_.queue_length];
  }
}


void
Madara::Transport::Broadcast_Transport_Read_Thread::init (
  Knowledge_Engine::Knowledge_Base & knowledge)
{
  context_ = &(knowledge.get_context ());

  if (context_)
  {
    // check for an on_data_received ruleset
    if (settings_.on_data_received_logic.length () != 0)
    {
      Logger::global_logger->log (Logger::LOG_MAJOR,
        "Broadcast_Transport_Read_Thread::init:" \
        " setting rules to %s\n",
        settings_.on_data_received_logic.c_str ());
      
#ifndef _MADARA_NO_KARL_
      Madara::Expression_Tree::Interpreter interpreter;
      on_data_received_ = context_->compile (settings_.on_data_received_logic);
#endif // _MADARA_NO_KARL_
    }
    else
    {
      Logger::global_logger->log (Logger::LOG_MAJOR,
        "Broadcast_Transport_Read_Thread::init:" \
        " no permanent rules were set\n");
    }
  }
}


void
Madara::Transport::Broadcast_Transport_Read_Thread::cleanup (void)
{
  // close broadcast port
  if (-1 == read_socket_.close ())
  {
    Logger::global_logger->log (Logger::LOG_MAJOR,
      "Broadcast_Transport_Read_Thread::cleanup:" \
      " Error closing broadcast socket\n");
  }
}

void
Madara::Transport::Broadcast_Transport_Read_Thread::rebroadcast (
        const char * print_prefix,
        Message_Header * header,
        const Knowledge_Map & records)
{
  int64_t buffer_remaining = (int64_t) settings_.queue_length;
  char * buffer = buffer_.get_ptr ();
  int result (0);

  if (!settings_.no_sending)
  {
    result = prep_rebroadcast (*context_, buffer, buffer_remaining,
                                   *qos_settings_, print_prefix,
                                   header, records,
                                   packet_scheduler_);

    if (result > 0)
    {
      ssize_t bytes_sent = 0;
      uint64_t packet_size = Message_Header::get_size (buffer_.get_ptr ());

      if (packet_size > settings_.max_fragment_size)
      {
        Fragment_Map map;

        Logger::global_logger->log (Logger::LOG_MAJOR,
          "%s:" \
          " fragmenting %d byte packet (%d bytes is max fragment size)\n",
          print_prefix, packet_size, settings_.max_fragment_size);

        // fragment the message
        frag (buffer_.get_ptr (), settings_.max_fragment_size, map);

        for (Fragment_Map::iterator i = map.begin (); i != map.end (); ++i)
        {
          // send the fragment
          bytes_sent += write_socket_.send(
            i->second,
            (ssize_t)Message_Header::get_size (i->second),
            address_);

          // sleep between fragments, if such a slack time is specified
          if (settings_.slack_time > 0)
            Madara::Utility::sleep (settings_.slack_time);
        }
      
        send_monitor_.add ((uint32_t)bytes_sent);

        Logger::global_logger->log (Logger::LOG_MAJOR,
          "%s:" \
          " Sent fragments totalling %d bytes\n",
          print_prefix,
          bytes_sent);

        delete_fragments (map);
      }
      else
      {
        bytes_sent = write_socket_.send(
          buffer_.get_ptr (), (ssize_t)result, address_);

        Logger::global_logger->log (Logger::LOG_MAJOR,
          "%s:" \
          " Sent packet of size %d\n",
          print_prefix, bytes_sent);

        send_monitor_.add ((uint32_t)bytes_sent);
      }

      Logger::global_logger->log (Logger::LOG_MINOR,
        "%s:" \
        " Send bandwidth = %d B/s\n",
        print_prefix,
        send_monitor_.get_bytes_per_second ());
    }
  }
}

void
Madara::Transport::Broadcast_Transport_Read_Thread::run (void)
{
  if (!settings_.no_receiving)
  {
    ACE_Time_Value wait_time (1);
    ACE_INET_Addr  remote;
  
    // allocate a buffer to send
    char * buffer = buffer_.get_ptr ();
    const char * print_prefix = "Broadcast_Transport_Read_Thread::run";
    int64_t buffer_remaining = settings_.queue_length;

    Logger::global_logger->log (Logger::LOG_MINOR,
      "%s:" \
      " entering main service loop.\n",
      print_prefix);

    Knowledge_Map rebroadcast_records;

    if (buffer == 0)
    {
      Logger::global_logger->log (Logger::LOG_EMERGENCY,
        "%s:" \
        " Unable to allocate buffer of size %d. Exiting thread.\n",
        print_prefix,
        settings_.queue_length);
    
      return;
    }

    Logger::global_logger->log (Logger::LOG_MINOR,
      "%s:" \
      " entering a recv on the socket.\n",
      print_prefix);
    
    // read the message
    ssize_t bytes_read = read_socket_.recv ((void *)buffer, 
      settings_.queue_length, remote, 0, &wait_time);

    Logger::global_logger->log (Logger::LOG_MAJOR,
      "%s:" \
      " received a message header of %d bytes from %s:%d\n",
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
        *qos_settings_, send_monitor_, receive_monitor_, rebroadcast_records,
  #ifndef _MADARA_NO_KARL_
        on_data_received_,
  #endif // _MADARA_NO_KARL_
        print_prefix,
        remote_host.str ().c_str (), header);
      
      if (header)
      {
        if (header->ttl > 0 && rebroadcast_records.size () > 0 &&
            qos_settings_ && qos_settings_->get_participant_ttl () > 0)
        {
          --header->ttl;
          header->ttl = std::min (
            qos_settings_->get_participant_ttl (), header->ttl);

          rebroadcast (print_prefix, header, rebroadcast_records);
        }

        // delete header
        delete header;
      }
    }
    else
    {
      Logger::global_logger->log (Logger::LOG_MAJOR,
        "%s:" \
        " wait timeout on new messages. Proceeding to next wait\n",
        print_prefix);
    }
  }
}
