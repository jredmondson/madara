#include "madara/transport/udp/UdpRegistryClient.h"
#include "madara/transport/udp/UdpRegistryClientReadThread.h"
#include "madara/transport/TransportContext.h"

#include "madara/knowledge/KnowledgeRecord.h"
#include "madara/transport/ReducedMessageHeader.h"
#include "madara/utility/Utility.h"
#include "ace/INET_Addr.h"
#include "ace/SOCK_Dgram.h"


#include <iostream>

madara::transport::UdpRegistryClient::UdpRegistryClient (const std::string & id,
  madara::knowledge::ThreadSafeContext & context, 
  TransportSettings & config, bool launch_transport)
  : Base (id, config, context)
{
  // create a reference to the knowledge base for threading
  knowledge_.use (context);

  // set the data plane for the read threads
  read_threads_.set_data_plane (knowledge_);

  endpoints_.set_name ("domain." + config.write_domain + ".endpoints", knowledge_);

  if (launch_transport)
    setup ();
}

madara::transport::UdpRegistryClient::~UdpRegistryClient ()
{
  close ();
}

void
  madara::transport::UdpRegistryClient::close (void)
{
  this->invalidate_transport ();
  
  read_threads_.terminate ();

  read_threads_.wait ();

  // close send port
  if (-1 == socket_.close ())
  {
    madara_logger_log (context_.get_logger (), logger::LOG_MAJOR,
      "UdpRegistryClient::cleanup:" \
      " Error closing socket\n");
  }
}

int
  madara::transport::UdpRegistryClient::reliability (void) const
{
  return RELIABLE;
}

int
  madara::transport::UdpRegistryClient::reliability (const int &)
{
  return RELIABLE;
}

int
  madara::transport::UdpRegistryClient::setup (void)
{
  // call base setup method to initialize certain common variables
  Base::setup ();

  if (settings_.hosts.size () > 0)
  {
    for (size_t i = 0; i < settings_.hosts.size (); ++i)
    {
      madara_logger_log (context_.get_logger (), logger::LOG_MAJOR,
        "UdpRegistryClient::setup:" \
        " adding server %s to registry lookup list\n",
        settings_.hosts[i].c_str ());

      servers_[settings_.hosts[i]].set (settings_.hosts[i].c_str ());
    }

    std::string host;
    unsigned short port = 50100;
    int bind_result = utility::bind_to_ephemeral_port (socket_, host, port);

    // open the socket to any port for sending
    if (bind_result == -1)
    {
      madara_logger_log (context_.get_logger (), logger::LOG_MAJOR,
        "UdpRegistryClient::setup:" \
        " socket failed to open\n");
    }
    else
    {
      ACE_Addr endpoint_addr;
      socket_.get_local_addr (endpoint_addr);
      endpoint_addr.get_addr ();

      madara_logger_log (context_.get_logger (), logger::LOG_MAJOR,
        "UdpRegistryClient::setup:" \
        " socket bound to %s:%d\n", host.c_str (), (int)port);
    }

    int send_buff_size = 0, tar_buff_size (settings_.queue_length);
    int rcv_buff_size = 0;
    int opt_len = sizeof (int);

    socket_.get_option (SOL_SOCKET, SO_SNDBUF,
      (void *)&send_buff_size, &opt_len);

    socket_.get_option (SOL_SOCKET, SO_RCVBUF,
      (void *)&rcv_buff_size, &opt_len);

    madara_logger_log (context_.get_logger (), logger::LOG_MAJOR,
      "UdpRegistryClient::setup:" \
      " default socket buff size is send=%d, rcv=%d\n",
      send_buff_size, rcv_buff_size);

    if (send_buff_size < tar_buff_size)
    {
      madara_logger_log (context_.get_logger (), logger::LOG_MAJOR,
        "UdpRegistryClient::setup:" \
        " setting send buff size to settings.queue_length (%d)\n",
        tar_buff_size);

      socket_.set_option (SOL_SOCKET, SO_SNDBUF,
        (void *)&tar_buff_size, opt_len);

      socket_.get_option (SOL_SOCKET, SO_SNDBUF,
        (void *)&send_buff_size, &opt_len);

      madara_logger_log (context_.get_logger (), logger::LOG_MAJOR,
        "UdpRegistryClient::setup:" \
        " current socket buff size is send=%d, rcv=%d\n",
        send_buff_size, rcv_buff_size);
    }

    if (rcv_buff_size < tar_buff_size)
    {
      madara_logger_log (context_.get_logger (), logger::LOG_MAJOR,
        "UdpRegistryClient::setup:" \
        " setting rcv buff size to settings.queue_length (%d)\n",
        tar_buff_size);

      socket_.set_option (SOL_SOCKET, SO_RCVBUF,
        (void *)&tar_buff_size, opt_len);

      socket_.get_option (SOL_SOCKET, SO_RCVBUF,
        (void *)&rcv_buff_size, &opt_len);

      madara_logger_log (context_.get_logger (), logger::LOG_MAJOR,
        "UdpRegistryClient::setup:" \
        " current socket buff size is send=%d, rcv=%d\n",
        send_buff_size, rcv_buff_size);
    }
    
    if (!settings_.no_receiving)
    {
      double hertz = settings_.read_thread_hertz;
      if (hertz < 0.0)
      {
        hertz = 0.0;
      }

      madara_logger_log (context_.get_logger (), logger::LOG_MAJOR,
        "UdpRegistryClientReadThread::setup:" \
        " starting %d threads at %f hertz\n", settings_.read_threads,
        hertz);

      for (uint32_t i = 0; i < settings_.read_threads; ++i)
      {
        std::stringstream thread_name;
        thread_name << "read";
        thread_name << i;

        read_threads_.run (hertz, thread_name.str (),
          new UdpRegistryClientReadThread (
            settings_, id_, servers_, socket_,
            send_monitor_, receive_monitor_, packet_scheduler_));
      }
    }
  }

  return this->validate_transport ();
}

void
madara::transport::UdpRegistryClient::send_register (void)
{
  const char * print_prefix = "UdpRegistryClient::register";

  long result (0);
  uint64_t bytes_sent = 0;

  if (servers_.size () > 0)
  {
    // Register messages always use the message header to include domain
    MessageHeader header;
    header.type = transport::REGISTER;
    strncpy (header.originator, this->id_.c_str (), sizeof (header.originator) - 1);
    strncpy (header.domain, this->settings_.write_domain.c_str (),
      sizeof (header.domain) - 1);
    header.updates = 0;
    header.clock = context_.get_clock ();
    // compute size of this header
    header.size = header.encoded_size ();

    int64_t buffer_remaining = settings_.queue_length;

    char * update = header.write (buffer_.get_ptr (), buffer_remaining);

    result = (long)(update - buffer_.get_ptr ());

    for (std::map <std::string, ACE_INET_Addr>::const_iterator i =
      servers_.begin (); i != servers_.end (); ++i)
    {
      madara_logger_log (context_.get_logger (), logger::LOG_MAJOR,
        "%s:" \
        " Sending register of size %d to %s\n",
        print_prefix, (int)result, i->first.c_str ());

      int send_attempts = -1;
      ssize_t actual_sent = -1;

      while (actual_sent < 0 &&
        (settings_.resend_attempts < 0 ||
        send_attempts < settings_.resend_attempts))
      {

        // send the fragment
        actual_sent = socket_.send (buffer_.get_ptr (),
          (ssize_t)result, i->second);

        ++send_attempts;

        if (actual_sent > 0)
        {
          bytes_sent += actual_sent;

          madara_logger_log (context_.get_logger (), logger::LOG_MAJOR,
            "%s:" \
            " Sent register of size %d to %s\n",
            print_prefix, (int)actual_sent, i->first.c_str ());

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
            " register was not sent due to unknown error (%d)\n",
            print_prefix, (int)actual_sent);
        }
      }
    }
  }
  else
  {
    madara_logger_log (context_.get_logger (), logger::LOG_MAJOR,
      "%s:" \
      " ERROR: no servers available for sending. Failed to register.\n");
  }
}

long
madara::transport::UdpRegistryClient::send_data (
  const madara::knowledge::VariableReferenceMap & orig_updates)
{
  const char * print_prefix = "UdpRegistryClient::send_data";
  
  long result (0);
  
  if (!settings_.no_sending)
  {
    this->endpoints_.sync_keys ();

    std::vector <std::string> hosts;
    this->clients_.clear ();
    this->endpoints_.keys (hosts);

    for (size_t i = 0; i < hosts.size (); ++i)
    {
      clients_[hosts[i]].set (hosts[i].c_str ());
    }

    send_register ();

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
          " fragmenting %" PRIu64 " byte packet (%" PRIu32
          " bytes is max fragment size)\n",
          print_prefix, packet_size, settings_.max_fragment_size);

        // fragment the message
        frag (buffer_.get_ptr (), settings_.max_fragment_size, map);

        int j (0);
        for (FragmentMap::iterator i = map.begin (); i != map.end (); ++i, ++j)
        {
          size_t frag_size =
            (size_t) MessageHeader::get_size (i->second);

          for (std::map <std::string, ACE_INET_Addr>::const_iterator addr =
            clients_.begin (); addr != clients_.end (); ++addr)
          {
            if (addr->first != settings_.hosts[0])
            {
              madara_logger_log (context_.get_logger (), logger::LOG_MAJOR,
                "%s:" \
                " Sending fragment %d to %s\n",
                print_prefix, j, addr->first.c_str ());

              int send_attempts = -1;
              ssize_t actual_sent = -1;

              while (actual_sent < 0 &&
                (settings_.resend_attempts < 0 ||
                send_attempts < settings_.resend_attempts))
              {

                // send the fragment
                actual_sent = socket_.send (
                  i->second, frag_size, addr->second);

                ++send_attempts;

                // sleep between fragments, if such a slack time is specified
                if (settings_.slack_time > 0)
                  madara::utility::sleep (settings_.slack_time);

                madara_logger_log (context_.get_logger (), logger::LOG_MAJOR,
                  "%s: Send result was %d of %d byte fragment to %s\n",
                  print_prefix, (int)actual_sent, (int)frag_size, addr->first.c_str ());

                if (actual_sent > 0)
                {
                  bytes_sent = (uint64_t)actual_sent;

                  madara_logger_log (context_.get_logger (), logger::LOG_MAJOR,
                    "%s:" \
                    " Sent packet of size %" PRIu64 " to %s\n",
                    print_prefix, bytes_sent, addr->first.c_str ());

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
          clients_.begin (); i != clients_.end (); ++i)
        {
          if (i->first != settings_.hosts[0])
          {
            madara_logger_log (context_.get_logger (), logger::LOG_MAJOR,
              "%s:" \
              " Sending packet of size %d to %s\n",
              print_prefix, (int)result, i->first.c_str ());

            int send_attempts = -1;
            ssize_t actual_sent = -1;

            while (actual_sent < 0 &&
              (settings_.resend_attempts < 0 ||
              send_attempts < settings_.resend_attempts))
            {

              // send the fragment
              actual_sent = socket_.send (buffer_.get_ptr (),
                (ssize_t)result, i->second);

              ++send_attempts;

              if (actual_sent > 0)
              {
                bytes_sent += actual_sent;

                madara_logger_log (context_.get_logger (), logger::LOG_MAJOR,
                  "%s:" \
                  " Sent packet of size %d to %s\n",
                  print_prefix, (int)actual_sent, i->first.c_str ());

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
