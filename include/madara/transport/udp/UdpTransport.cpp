#include "madara/transport/udp/UdpTransport.h"
#include "madara/transport/udp/UdpTransportReadThread.h"
#include "madara/transport/TransportContext.h"

#include "madara/knowledge/KnowledgeRecord.h"
#include "madara/transport/ReducedMessageHeader.h"
#include "madara/utility/Utility.h"
#include "ace/INET_Addr.h"
#include "ace/SOCK_Dgram.h"


#include <iostream>

madara::transport::UdpTransport::UdpTransport (const std::string & id,
  madara::knowledge::ThreadSafeContext & context, 
  TransportSettings & config, bool launch_transport)
  : Base (id, config, context)
{
  // create a reference to the knowledge base for threading
  knowledge_.use (context);

  // set the data plane for the read threads
  read_threads_.set_data_plane (knowledge_);

  if (launch_transport)
    setup ();
}

madara::transport::UdpTransport::~UdpTransport ()
{
  close ();
}

void
  madara::transport::UdpTransport::close (void)
{
  this->invalidate_transport ();
  
  read_threads_.terminate ();

  read_threads_.wait ();

  // close send port
  if (-1 == write_socket_.close ())
  {
    madara_logger_log (context_.get_logger (), logger::LOG_MAJOR,
      "UdpTransport::cleanup:" \
      " Error closing write socket\n");
  }

  // close receive port
  if (-1 == read_socket_.close ())
  {
    madara_logger_log (context_.get_logger (), logger::LOG_MAJOR,
      "UdpTransport::cleanup:" \
      " Error closing read socket\n");
  }
}

int
  madara::transport::UdpTransport::reliability (void) const
{
  return RELIABLE;
}

int
  madara::transport::UdpTransport::reliability (const int &)
{
  return RELIABLE;
}

int
  madara::transport::UdpTransport::setup (void)
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
    {
      madara_logger_log (context_.get_logger (), logger::LOG_MAJOR,
        "UdpTransport::setup:" \
        " socket failed to open\n");
    }

    int send_buff_size = 0, tar_buff_size (settings_.queue_length);
    int rcv_buff_size = 0;
    int opt_len = sizeof (int);

    write_socket_.get_option (SOL_SOCKET, SO_SNDBUF,
      (void *)&send_buff_size, &opt_len);

    write_socket_.get_option (SOL_SOCKET, SO_RCVBUF,
      (void *)&rcv_buff_size, &opt_len);

    madara_logger_log (context_.get_logger (), logger::LOG_MAJOR,
      "UdpTransport::setup:" \
      " default socket buff size is send=%d, rcv=%d\n",
      send_buff_size, rcv_buff_size);

    if (send_buff_size < tar_buff_size)
    {
      madara_logger_log (context_.get_logger (), logger::LOG_MAJOR,
        "UdpTransport::setup:" \
        " setting send buff size to settings.queue_length (%d)\n",
        tar_buff_size);

      write_socket_.set_option (SOL_SOCKET, SO_SNDBUF,
        (void *)&tar_buff_size, opt_len);

      write_socket_.get_option (SOL_SOCKET, SO_SNDBUF,
        (void *)&send_buff_size, &opt_len);

      madara_logger_log (context_.get_logger (), logger::LOG_MAJOR,
        "UdpTransport::setup:" \
        " current socket buff size is send=%d, rcv=%d\n",
        send_buff_size, rcv_buff_size);
    }

    if (rcv_buff_size < tar_buff_size)
    {
      madara_logger_log (context_.get_logger (), logger::LOG_MAJOR,
        "UdpTransport::setup:" \
        " setting rcv buff size to settings.queue_length (%d)\n",
        tar_buff_size);

      write_socket_.set_option (SOL_SOCKET, SO_RCVBUF,
        (void *)&tar_buff_size, opt_len);

      write_socket_.get_option (SOL_SOCKET, SO_RCVBUF,
        (void *)&rcv_buff_size, &opt_len);

      madara_logger_log (context_.get_logger (), logger::LOG_MAJOR,
        "UdpTransport::setup:" \
        " current socket buff size is send=%d, rcv=%d\n",
        send_buff_size, rcv_buff_size);
    }
    
    
    if (addresses_.size () > 0)
    {
      // for receiving, we only want to bind to the local port
      ACE_INET_Addr local (addresses_[settings_.hosts[0]].get_port_number ());
  
      if (-1 == read_socket_.open (local, 2, 0, 0))
      {
        madara_logger_log (context_.get_logger (), logger::LOG_MAJOR,
          "UdpTransport::setup:" \
          " Error subscribing to udp address %s:%d\n",
          local.get_host_addr (), local.get_port_number ());
      } 
      else
      {
        madara_logger_log (context_.get_logger (), logger::LOG_MAJOR,
          "UdpTransport::setup:" \
          " Success subscribing to udp address %s:%d\n",
          local.get_host_addr (), local.get_port_number ());
      
        int send_buff_size = 0, tar_buff_size (settings_.queue_length);
        int rcv_buff_size = 0;
        int opt_len = sizeof (int);
    
        ACE_SOCK_Dgram & bare_socket = read_socket_;

        bare_socket.get_option (SOL_SOCKET, SO_RCVBUF,
          (void *)&rcv_buff_size, &opt_len);

        madara_logger_log (context_.get_logger (), logger::LOG_MAJOR,
          "UdpTransport::setup:" \
          " default socket buff size is send=%d, rcv=%d\n",
          send_buff_size, rcv_buff_size);
  
        if (send_buff_size < tar_buff_size)
        {
          madara_logger_log (context_.get_logger (), logger::LOG_MAJOR,
            "UdpTransport::setup:" \
            " setting send buff size to settings.queue_length (%d)\n",
            tar_buff_size);
  
          bare_socket.set_option (SOL_SOCKET, SO_SNDBUF,
            (void *)&tar_buff_size, opt_len);
    
          bare_socket.get_option (SOL_SOCKET, SO_SNDBUF,
            (void *)&send_buff_size, &opt_len);

          madara_logger_log (context_.get_logger (), logger::LOG_MAJOR,
            "UdpTransport::setup:" \
            " current socket buff size is send=%d, rcv=%d\n",
            send_buff_size, rcv_buff_size);
        }
  
        if (rcv_buff_size < tar_buff_size)
        {
          madara_logger_log (context_.get_logger (), logger::LOG_MAJOR,
            "UdpTransport::setup:" \
            " setting rcv buff size to settings.queue_length (%d)\n",
            tar_buff_size);
  
          bare_socket.set_option (SOL_SOCKET, SO_RCVBUF,
            (void *)&tar_buff_size, opt_len);
    
          bare_socket.get_option (SOL_SOCKET, SO_RCVBUF,
            (void *)&rcv_buff_size, &opt_len);

          madara_logger_log (context_.get_logger (), logger::LOG_MINOR,
            "UdpTransportReadThread::setup:" \
            " current socket buff size is send=%d, rcv=%d\n",
            send_buff_size, rcv_buff_size);
        }
    
      } // if successful socket
       
    } // if appropriate addresses

    if (!settings_.no_receiving)
    {
      double hertz = settings_.read_thread_hertz;
      if (hertz < 0.0)
      {
        hertz = 0.0;
      }

      madara_logger_log (context_.get_logger (), logger::LOG_MAJOR,
        "UdpTransportReadThread::setup:" \
        " starting %d threads at %f hertz\n", settings_.read_threads,
        hertz);

      for (uint32_t i = 0; i < settings_.read_threads; ++i)
      {
        std::stringstream thread_name;
        thread_name << "read";
        thread_name << i;

        read_threads_.run (hertz, thread_name.str (),
          new UdpTransportReadThread (
            settings_, id_, addresses_, write_socket_, read_socket_,
            send_monitor_, receive_monitor_, packet_scheduler_));
      }
    }
  }

  return this->validate_transport ();
}

long
  madara::transport::UdpTransport::send_data (
  const madara::knowledge::KnowledgeRecords & orig_updates)
{
  const char * print_prefix = "UdpTransport::send_data";
  
  long result (0);
  
  if (!settings_.no_sending)
  {
    result = prep_send (orig_updates, print_prefix);

    if (result > 0)
    {
      uint64_t bytes_sent = 0;
      uint64_t packet_size = (uint64_t)result;

      if (packet_size > settings_.max_fragment_size)
      {
        FragmentMap map;

        madara_logger_log (context_.get_logger (), logger::LOG_MAJOR,
          "%s:" \
          " fragmenting %" PRIu64 " byte packet (%" PRIu32 " bytes is max fragment size)\n",
          print_prefix, packet_size, settings_.max_fragment_size);

        // fragment the message
        frag (buffer_.get_ptr (), settings_.max_fragment_size, map);

        int j (0);
        for (FragmentMap::iterator i = map.begin (); i != map.end (); ++i, ++j)
        {
          size_t frag_size =
            (size_t) MessageHeader::get_size (i->second);

          for (std::map <std::string, ACE_INET_Addr>::const_iterator addr =
            addresses_.begin (); addr != addresses_.end (); ++addr)
          {
            if (addr->first != settings_.hosts[0])
            {
              madara_logger_log (context_.get_logger (), logger::LOG_MAJOR,
                "%s:" \
                " Sending fragment %d\n",
                print_prefix, j);

              int send_attempts = -1;
              ssize_t actual_sent = -1;

              while (actual_sent < 0 &&
                (settings_.resend_attempts < 0 ||
                send_attempts < settings_.resend_attempts))
              {

                // send the fragment
                actual_sent = write_socket_.send (
                  i->second, frag_size, addr->second);

                ++send_attempts;

                // sleep between fragments, if such a slack time is specified
                if (settings_.slack_time > 0)
                  madara::utility::sleep (settings_.slack_time);

                madara_logger_log (context_.get_logger (), logger::LOG_MAJOR,
                  "%s: Send result was %d of %d byte fragment to %s\n",
                  print_prefix, actual_sent, frag_size, addr->first.c_str ());

                if (actual_sent > 0)
                {
                  bytes_sent = (uint64_t)actual_sent;

                  madara_logger_log (context_.get_logger (), logger::LOG_MAJOR,
                    "%s:" \
                    " Sent packet of size %" PRIu64 "\n",
                    print_prefix, bytes_sent);

                  send_monitor_.add ((uint32_t)actual_sent);
                }
                else if (actual_sent == ECONNRESET)
                {
                  madara_logger_log (context_.get_logger (), logger::LOG_WARNING,
                    "%s:" \
                    " WARNING: Remote socket disappeared during send (ECONNRESET)\n",
                    print_prefix);
                }
                else if (actual_sent == EINTR)
                {
                  madara_logger_log (context_.get_logger (), logger::LOG_WARNING,
                    "%s:" \
                    " Local socket was interrupted during send (EINTR)\n",
                    print_prefix);
                }
                else if (actual_sent == EWOULDBLOCK)
                {
                  madara_logger_log (context_.get_logger (), logger::LOG_WARNING,
                    "%s:" \
                    " Send would have blocked (EWOULDBLOCK)\n",
                    print_prefix);
                }
                else if (actual_sent == ENOTCONN)
                {
                  madara_logger_log (context_.get_logger (), logger::LOG_WARNING,
                    "%s:" \
                    " Send reports socket is not connected (ENOTCONN)\n",
                    print_prefix);
                }
                else if (actual_sent == EADDRINUSE)
                {
                  madara_logger_log (context_.get_logger (), logger::LOG_WARNING,
                    "%s:" \
                    " Send reports the interface is busy (EADDRINUSE)\n",
                    print_prefix);
                }
                else if (actual_sent == EBADF)
                {
                  madara_logger_log (context_.get_logger (), logger::LOG_WARNING,
                    "%s:" \
                    " Send socket is invalid (EBADF)\n",
                    print_prefix);
                }
                else
                {
                  madara_logger_log (context_.get_logger (), logger::LOG_WARNING,
                    "%s:" \
                    " Packet was not sent due to unknown error (%d)\n",
                    print_prefix, (int)actual_sent);
                }
              }
            }
          }
        }

        if (bytes_sent > 0)
        {
          send_monitor_.add ((uint32_t)bytes_sent);
        }

        madara_logger_log (context_.get_logger (), logger::LOG_MAJOR,
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
            madara_logger_log (context_.get_logger (), logger::LOG_MAJOR,
              "%s:" \
              " Sending packet of size %ld\n",
              print_prefix, result);

            int send_attempts = -1;
            ssize_t actual_sent = -1;

            while (actual_sent < 0 &&
              (settings_.resend_attempts < 0 ||
              send_attempts < settings_.resend_attempts))
            {

              // send the fragment
              actual_sent = write_socket_.send (buffer_.get_ptr (),
                (ssize_t)result, i->second);

              ++send_attempts;

              if (actual_sent > 0)
              {
                bytes_sent += actual_sent;

                madara_logger_log (context_.get_logger (), logger::LOG_MAJOR,
                  "%s:" \
                  " Sent packet of size %" PRIu64 "\n",
                  print_prefix, (int)actual_sent);

                send_monitor_.add ((uint32_t)actual_sent);
              }
              else if (actual_sent == ECONNRESET)
              {
                madara_logger_log (context_.get_logger (), logger::LOG_WARNING,
                  "%s:" \
                  " WARNING: Remote socket disappeared during send (ECONNRESET)\n",
                  print_prefix);
              }
              else if (actual_sent == EINTR)
              {
                madara_logger_log (context_.get_logger (), logger::LOG_WARNING,
                  "%s:" \
                  " Local socket was interrupted during send (EINTR)\n",
                  print_prefix);
              }
              else if (actual_sent == EWOULDBLOCK)
              {
                madara_logger_log (context_.get_logger (), logger::LOG_WARNING,
                  "%s:" \
                  " Send would have blocked (EWOULDBLOCK)\n",
                  print_prefix);
              }
              else if (actual_sent == ENOTCONN)
              {
                madara_logger_log (context_.get_logger (), logger::LOG_WARNING,
                  "%s:" \
                  " Send reports socket is not connected (ENOTCONN)\n",
                  print_prefix);
              }
              else if (actual_sent == EADDRINUSE)
              {
                madara_logger_log (context_.get_logger (), logger::LOG_WARNING,
                  "%s:" \
                  " Send reports the interface is busy (EADDRINUSE)\n",
                  print_prefix);
              }
              else if (actual_sent == EBADF)
              {
                madara_logger_log (context_.get_logger (), logger::LOG_WARNING,
                  "%s:" \
                  " Send socket is invalid (EBADF)\n",
                  print_prefix);
              }
              else
              {
                madara_logger_log (context_.get_logger (), logger::LOG_WARNING,
                  "%s:" \
                  " Packet was not sent due to unknown error (%d)\n",
                  print_prefix, (int)actual_sent);
              }
            }
          }
        }
      }

      result = (long) bytes_sent;

      madara_logger_log (context_.get_logger (), logger::LOG_MAJOR,
        "%s:" \
        " Send bandwidth = %" PRIu64 " B/s\n",
        print_prefix, send_monitor_.get_bytes_per_second ());
    }
  }

  return result;
}
