#include "madara/transport/udp/UDP_Transport_Read_Thread.h"
#include "madara/utility/Log_Macros.h"
#include "madara/utility/Utility.h"
#include "madara/transport/Reduced_Message_Header.h"
#include "ace/Time_Value.h"

#include <iostream>

#ifdef WIN32

#include <process.h>

unsigned __stdcall udp_windows_glue (void * param)
{
  Madara::Transport::UDP_Transport_Read_Thread * caller = 
    static_cast <Madara::Transport::UDP_Transport_Read_Thread *> (
      param);
  if (caller)
    return (unsigned) caller->svc ();
  else
    return 0;
}

#endif

Madara::Transport::UDP_Transport_Read_Thread::UDP_Transport_Read_Thread (
  const Settings & settings,
  const std::string & id,
  Madara::Knowledge_Engine::Thread_Safe_Context & context,
  std::map <std::string, ACE_INET_Addr> & addresses,
  ACE_SOCK_Dgram & socket,
  Bandwidth_Monitor & send_monitor,
  Bandwidth_Monitor & receive_monitor,
  Packet_Scheduler & packet_scheduler)
  : settings_ (settings), id_ (id), context_ (context),
    barrier_ (2), terminated_ (false), 
    mutex_ (), is_not_ready_ (mutex_), is_ready_ (false),
    addresses_ (addresses),
    write_socket_ (socket),
    send_monitor_ (send_monitor),
    receive_monitor_ (receive_monitor),
    packet_scheduler_ (packet_scheduler)
{
  if (addresses_.size () > 0)
  {
    // for receiving, we only want to bind to the local port
    ACE_INET_Addr local (addresses_[settings_.hosts[0]].get_port_number ());
  
    qos_settings_ = dynamic_cast <const QoS_Transport_Settings *> (&settings);

    if (-1 == read_socket_.open (local, 2, 0, 0))
    {
      MADARA_DEBUG (MADARA_LOG_MAJOR_EVENT, (LM_DEBUG, 
        DLINFO "UDP_Transport_Read_Thread::UDP_Transport_Read_Thread:" \
        " Error subscribing to udp address %s:%d\n",
        local.get_host_addr (), local.get_port_number ()));
    } 
    else
    {
      MADARA_DEBUG (MADARA_LOG_MAJOR_EVENT, (LM_DEBUG, 
        DLINFO "UDP_Transport_Read_Thread::UDP_Transport_Read_Thread:" \
        " Success subscribing to udp address %s:%d\n",
        local.get_host_addr (), local.get_port_number ()));
      
      int send_buff_size = 0, tar_buff_size (settings_.queue_length);
      int rcv_buff_size = 0;
      int opt_len = sizeof (int);
    
      ACE_SOCK_Dgram & bare_socket = read_socket_;

      bare_socket.get_option (SOL_SOCKET, SO_RCVBUF,
        (void *)&rcv_buff_size, &opt_len);
  
      MADARA_DEBUG (MADARA_LOG_MAJOR_EVENT, (LM_DEBUG, 
        DLINFO
        "UDP_Transport_Read_Thread::UDP_Transport_Read_Thread:" \
        " default socket buff size is send=%d, rcv=%d\n",
        send_buff_size, rcv_buff_size));
  
      if (send_buff_size < tar_buff_size)
      {
        MADARA_DEBUG (MADARA_LOG_MAJOR_EVENT, (LM_DEBUG, 
          DLINFO
          "UDP_Transport_Read_Thread::UDP_Transport_Read_Thread:" \
          " setting send buff size to settings.queue_length (%d)\n",
          tar_buff_size));
  
        bare_socket.set_option (SOL_SOCKET, SO_SNDBUF,
          (void *)&tar_buff_size, opt_len);
    
        bare_socket.get_option (SOL_SOCKET, SO_SNDBUF,
          (void *)&send_buff_size, &opt_len);

        MADARA_DEBUG (MADARA_LOG_MAJOR_EVENT, (LM_DEBUG, 
          DLINFO
          "UDP_Transport_Read_Thread::UDP_Transport_Read_Thread:" \
          " current socket buff size is send=%d, rcv=%d\n",
          send_buff_size, rcv_buff_size));
      }
  
      if (rcv_buff_size < tar_buff_size)
      {
        MADARA_DEBUG (MADARA_LOG_MAJOR_EVENT, (LM_DEBUG, 
          DLINFO
          "UDP_Transport_Read_Thread::UDP_Transport_Read_Thread:" \
          " setting rcv buff size to settings.queue_length (%d)\n",
          tar_buff_size));
  
        bare_socket.set_option (SOL_SOCKET, SO_SNDBUF,
          (void *)&tar_buff_size, opt_len);
    
        bare_socket.get_option (SOL_SOCKET, SO_SNDBUF,
          (void *)&rcv_buff_size, &opt_len);
    
        MADARA_DEBUG (MADARA_LOG_MAJOR_EVENT, (LM_DEBUG, 
          DLINFO
          "UDP_Transport_Read_Thread::UDP_Transport_Read_Thread:" \
          " current socket buff size is send=%d, rcv=%d\n",
          send_buff_size, rcv_buff_size));
      }
    
      // check for an on_data_received ruleset
      if (settings_.on_data_received_logic.length () != 0)
      {
        MADARA_DEBUG (MADARA_LOG_MAJOR_EVENT, (LM_DEBUG, 
          DLINFO "UDP_Transport_Read_Thread::UDP_Transport_Read_Thread:" \
          " setting rules to %s\n", 
          settings_.on_data_received_logic.c_str ()));

        Madara::Expression_Tree::Interpreter interpreter;
        on_data_received_ = context_.compile (settings_.on_data_received_logic);
      }
      else
      {
        MADARA_DEBUG (MADARA_LOG_MAJOR_EVENT, (LM_DEBUG, 
          DLINFO "UDP_Transport_Read_Thread::UDP_Transport_Read_Thread:" \
          " no permanent rules were set\n"));
      }
  
      // setup the receive buffer
      if (settings_.queue_length > 0)
        buffer_ = new char [settings_.queue_length];
        
    int result;

#ifndef WIN32
    result = this->activate ();
#else
    result = 0;
    _beginthreadex(NULL, 0, udp_windows_glue, (void*)this, 0, 0);
    
#endif
    
    if (result != -1)
    {
      MADARA_DEBUG (MADARA_LOG_MAJOR_EVENT, (LM_DEBUG, 
        DLINFO "UDP_Transport_Read_Thread::UDP_Transport_Read_Thread:" \
        " read thread started (result = %d)\n", result));
    }
    else
    {
      MADARA_DEBUG (MADARA_LOG_MAJOR_EVENT, (LM_DEBUG, 
        DLINFO "UDP_Transport_Read_Thread::UDP_Transport_Read_Thread:" \
        " failed to create thread. ERRNO = %d\n", ACE_OS::last_error ()));
    }
    } // if successful socket
  } // if appropriate addresses
}

Madara::Transport::UDP_Transport_Read_Thread::~UDP_Transport_Read_Thread ()
{
  // close broadcast port
  if (-1 == read_socket_.close ())
  { 
    MADARA_DEBUG (MADARA_LOG_MAJOR_EVENT, (LM_DEBUG, 
      DLINFO "UDP_Transport_Read_Thread::close:" \
      " Error closing datagram socket\n"));
  }
}

int
Madara::Transport::UDP_Transport_Read_Thread::close (void)
{
  terminated_ = true;
  enter_barrier ();

  return 0;
}

void
Madara::Transport::UDP_Transport_Read_Thread::wait_for_ready (void)
{
  if (!is_ready_)
    is_not_ready_.wait ();
}

int Madara::Transport::UDP_Transport_Read_Thread::enter_barrier (void)
{
  MADARA_DEBUG (MADARA_LOG_MAJOR_EVENT, (LM_DEBUG, 
    DLINFO "UDP_Transport_Read_Thread::enter_barrier:" \
    " beginning thread barrier for shut down of read thread\n"));

  barrier_.wait ();
  return 0;
}


void
Madara::Transport::UDP_Transport_Read_Thread::rebroadcast (
  const char * print_prefix,
  Message_Header * header,
  const Knowledge_Map & records)
{
  int64_t buffer_remaining = (int64_t) settings_.queue_length;
  char * buffer = buffer_.get_ptr ();
  int result = prep_rebroadcast (buffer, buffer_remaining,
                                 *qos_settings_, print_prefix,
                                 header, records,
                                 packet_scheduler_);

  if (result > 0)
  {
    uint64_t bytes_sent = 0;
    uint64_t packet_size = Message_Header::get_size (buffer_.get_ptr ());

    if (packet_size > settings_.max_fragment_size)
    {
      Fragment_Map map;
      
      MADARA_DEBUG (MADARA_LOG_MAJOR_EVENT, (LM_DEBUG, 
        DLINFO "%s:" \
        " fragmenting %Q byte packet (%d bytes is max fragment size)\n",
        print_prefix, packet_size, settings_.max_fragment_size));

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
            ssize_t actual_sent = write_socket_.send (
              i->second, frag_size, addr->second);
            
            // sleep between fragments, if such a slack time is specified
            if (settings_.slack_time > 0)
              Madara::Utility::sleep (settings_.slack_time);

            MADARA_DEBUG (MADARA_LOG_MAJOR_EVENT, (LM_DEBUG, 
              DLINFO "%s:" \
              " Send result was %d of %d byte fragment to %s\n",
              print_prefix, actual_sent, frag_size, addr->first.c_str ()));

            if (actual_sent > 0)
            {
              send_monitor_.add ((uint32_t)frag_size);
              bytes_sent += actual_sent;
            }
          }
        }
      }
      
      MADARA_DEBUG (MADARA_LOG_MAJOR_EVENT, (LM_DEBUG, 
        DLINFO "%s:" \
        " Sent fragments totalling %Q bytes\n",
        print_prefix, bytes_sent));

      delete_fragments (map);
    }
    else
    {
      for (std::map <std::string, ACE_INET_Addr>::const_iterator i =
              addresses_.begin (); i != addresses_.end (); ++i)
      {
        if (i->first != settings_.hosts[0])
        {
          ssize_t actual_sent = write_socket_.send (buffer_.get_ptr (),
            (ssize_t)result, i->second);
            
          MADARA_DEBUG (MADARA_LOG_MAJOR_EVENT, (LM_DEBUG, 
            DLINFO "%s:" \
            " Sent %Q packet to %s\n",
            print_prefix, packet_size, i->first.c_str ()));

          if (actual_sent > 0)
          {
            send_monitor_.add ((uint32_t)actual_sent);
            bytes_sent += actual_sent;
          }
        }
      }

      MADARA_DEBUG (MADARA_LOG_MAJOR_EVENT, (LM_DEBUG, 
        DLINFO "%s:" \
        " Sent %Q total bytes via rebroadcast\n",
        print_prefix, bytes_sent));
    }

    MADARA_DEBUG (MADARA_LOG_MINOR_EVENT, (LM_DEBUG, 
      DLINFO "%s:" \
      " Send bandwidth = %d B/s\n",
      print_prefix,
      send_monitor_.get_bytes_per_second ()));
  }
}


int
Madara::Transport::UDP_Transport_Read_Thread::svc (void)
{
  ACE_Time_Value wait_time (1);
  ACE_INET_Addr  remote;
  
  // allocate a buffer to send
  char * buffer = buffer_.get_ptr ();
  const char * print_prefix = "UDP_Transport_Read_Thread::svc";
  int64_t buffer_remaining = settings_.queue_length;
  
  MADARA_DEBUG (MADARA_LOG_MINOR_EVENT, (LM_DEBUG, 
    DLINFO "%s:" \
    " entering main service loop.\n",
    print_prefix));
    
  while (false == terminated_.value ())
  {
    Knowledge_Map rebroadcast_records;

    if (buffer == 0)
    {
      MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_DEBUG, 
        DLINFO "%s:" \
        " Unable to allocate buffer of size %d. Exiting thread.\n",
        print_prefix,
        settings_.queue_length));
    
      break;
    }
    
    MADARA_DEBUG (MADARA_LOG_MINOR_EVENT, (LM_DEBUG, 
      DLINFO "%s:" \
      " entering a recv on the socket.\n",
      print_prefix));
    
    // read the message
    ssize_t bytes_read = read_socket_.recv ((void *)buffer, 
      settings_.queue_length, remote, 0, &wait_time);
 

    MADARA_DEBUG (MADARA_LOG_MAJOR_EVENT, (LM_DEBUG, 
      DLINFO "%s:" \
      " received a message header of %d bytes from %s:%d\n",
      print_prefix,
      bytes_read,
      remote.get_host_addr (), remote.get_port_number ()));
    
    if (bytes_read > 0)
    {
      Message_Header * header = 0;

      std::stringstream remote_host;
      remote_host << remote.get_host_addr ();
      remote_host << ":";
      remote_host << remote.get_port_number ();

      process_received_update (buffer, bytes_read, id_, context_,
        *qos_settings_, send_monitor_, receive_monitor_, rebroadcast_records,
        on_data_received_, print_prefix,
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
      MADARA_DEBUG (MADARA_LOG_MAJOR_EVENT, (LM_DEBUG, 
        DLINFO "%s:" \
        " wait timeout on new messages. Proceeding to next wait\n",
        print_prefix));
    }
  }
  
  MADARA_DEBUG (MADARA_LOG_MAJOR_EVENT, (LM_DEBUG, 
    DLINFO "%s:" \
    " entering barrier.\n",
    print_prefix));

  barrier_.wait ();
  return 0;
}