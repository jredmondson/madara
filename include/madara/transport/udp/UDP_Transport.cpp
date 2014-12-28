#include "madara/transport/udp/UDP_Transport.h"
#include "madara/transport/udp/UDP_Transport_Read_Thread.h"
#include "madara/transport/Transport_Context.h"
#include "madara/utility/Log_Macros.h"
#include "madara/knowledge_engine/Knowledge_Record.h"
#include "madara/transport/Reduced_Message_Header.h"
#include "madara/utility/Utility.h"
#include "ace/INET_Addr.h"
#include "ace/SOCK_Dgram.h"


#include <iostream>

Madara::Transport::UDP_Transport::UDP_Transport (const std::string & id,
  Madara::Knowledge_Engine::Thread_Safe_Context & context, 
  Settings & config, bool launch_transport)
  : Base (id, config, context)
{
  // create a reference to the knowledge base for threading
  knowledge_.use (context);

  // set the data plane for the read threads
  read_threads_.set_data_plane (knowledge_);

  if (launch_transport)
    setup ();
}

Madara::Transport::UDP_Transport::~UDP_Transport ()
{
  close ();
}

void
  Madara::Transport::UDP_Transport::close (void)
{
  this->invalidate_transport ();
  
  read_threads_.terminate ();

  read_threads_.wait ();

  // close send port
  if (-1 == write_socket_.close ())
  { 
    MADARA_DEBUG (MADARA_LOG_MAJOR_EVENT, (LM_DEBUG, 
      DLINFO "UDP_Transport::cleanup:" \
      " Error closing write socket\n"));
  }

  // close receive port
  if (-1 == read_socket_.close ())
  { 
    MADARA_DEBUG (MADARA_LOG_MAJOR_EVENT, (LM_DEBUG, 
      DLINFO "UDP_Transport::cleanup:" \
      " Error closing read socket\n"));
  }
}

int
  Madara::Transport::UDP_Transport::reliability (void) const
{
  return RELIABLE;
}

int
  Madara::Transport::UDP_Transport::reliability (const int &)
{
  return RELIABLE;
}

int
  Madara::Transport::UDP_Transport::setup (void)
{
  // call base setup method to initialize certain common variables
  Base::setup ();

  if (settings_.hosts.size () > 0)
  {
    for (size_t i = 0; i < settings_.hosts.size (); ++i)
    {
      addresses_[settings_.hosts[i]].set (settings_.hosts[i].c_str ());
    }

    // open the broadcast socket to any port for sending
    if (write_socket_.open (ACE_Addr::sap_any) == -1)
      std::cout << "UDP socket failed to open\n";

    int send_buff_size = 0, tar_buff_size (settings_.queue_length);
    int rcv_buff_size = 0;
    int opt_len = sizeof (int);

    write_socket_.get_option (SOL_SOCKET, SO_SNDBUF,
      (void *)&send_buff_size, &opt_len);

    write_socket_.get_option (SOL_SOCKET, SO_RCVBUF,
      (void *)&rcv_buff_size, &opt_len);

    MADARA_DEBUG (MADARA_LOG_MAJOR_EVENT, (LM_DEBUG, 
      DLINFO "UDP_Transport::UDP_Transport:" \
      " default socket buff size is send=%d, rcv=%d\n",
      send_buff_size, rcv_buff_size));

    if (send_buff_size < tar_buff_size)
    {
      MADARA_DEBUG (MADARA_LOG_MAJOR_EVENT, (LM_DEBUG, 
        DLINFO "UDP_Transport::UDP_Transport:" \
        " setting send buff size to settings.queue_length (%d)\n",
        tar_buff_size));

      write_socket_.set_option (SOL_SOCKET, SO_SNDBUF,
        (void *)&tar_buff_size, opt_len);

      write_socket_.get_option (SOL_SOCKET, SO_SNDBUF,
        (void *)&send_buff_size, &opt_len);

      MADARA_DEBUG (MADARA_LOG_MAJOR_EVENT, (LM_DEBUG, 
        DLINFO "UDP_Transport::UDP_Transport:" \
        " current socket buff size is send=%d, rcv=%d\n",
        send_buff_size, rcv_buff_size));
    }

    if (rcv_buff_size < tar_buff_size)
    {
      MADARA_DEBUG (MADARA_LOG_MAJOR_EVENT, (LM_DEBUG, 
        DLINFO
        "UDP_Transport::UDP_Transport:" \
        " setting rcv buff size to settings.queue_length (%d)\n",
        tar_buff_size));

      write_socket_.set_option (SOL_SOCKET, SO_SNDBUF,
        (void *)&tar_buff_size, opt_len);

      write_socket_.get_option (SOL_SOCKET, SO_SNDBUF,
        (void *)&rcv_buff_size, &opt_len);

      MADARA_DEBUG (MADARA_LOG_MAJOR_EVENT, (LM_DEBUG, 
        DLINFO
        "UDP_Transport::UDP_Transport:" \
        " current socket buff size is send=%d, rcv=%d\n",
        send_buff_size, rcv_buff_size));
    }
    
    
    if (addresses_.size () > 0)
    {
      // for receiving, we only want to bind to the local port
      ACE_INET_Addr local (addresses_[settings_.hosts[0]].get_port_number ());
  
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
    
      } // if successful socket
       
    } // if appropriate addresses


    double hertz = settings_.read_thread_hertz;
    if (hertz < 0.0)
    {
      hertz = 0.0;
    }
    
    MADARA_DEBUG (MADARA_LOG_MAJOR_EVENT, (LM_DEBUG, 
      DLINFO "Multicast_Transport::constructor:" \
      " starting %d threads at %f hertz\n", settings_.read_threads, 
      hertz));

    for (uint32_t i = 0; i < settings_.read_threads; ++i)
    {
      std::stringstream thread_name;
      thread_name << "read";
      thread_name << i;

      read_threads_.run (hertz, thread_name.str (),
        new UDP_Transport_Read_Thread (
          settings_, id_, addresses_, write_socket_, read_socket_,
          send_monitor_, receive_monitor_, packet_scheduler_));
    }

  }

  return this->validate_transport ();
}

long
  Madara::Transport::UDP_Transport::send_data (
  const Madara::Knowledge_Records & orig_updates)
{
  const char * print_prefix = "UDP_Transport::send_data";
  
  long result = prep_send (orig_updates, print_prefix);

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
          (size_t) Message_Header::get_size (i->second);

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
              DLINFO "%s: Send result was %d of %d byte fragment to %s\n",
              print_prefix, actual_sent, frag_size, addr->first.c_str ()));

            if (actual_sent > 0)
            {
              send_monitor_.add ((uint32_t)actual_sent);
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
    }

    result = (long) bytes_sent;

    MADARA_DEBUG (MADARA_LOG_MINOR_EVENT, (LM_DEBUG, 
      DLINFO "%s:" \
      " Send bandwidth = %d B/s\n",
      print_prefix, send_monitor_.get_bytes_per_second ()));
  }

  return result;
}
