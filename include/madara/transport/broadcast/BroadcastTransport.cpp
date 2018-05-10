#include "madara/transport/broadcast/BroadcastTransport.h"
#include "madara/transport/broadcast/BroadcastTransportReadThread.h"
#include "madara/transport/TransportContext.h"

#include "madara/transport/ReducedMessageHeader.h"
#include "madara/utility/Utility.h"

#include <iostream>

madara::transport::BroadcastTransport::BroadcastTransport (const std::string & id,
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

madara::transport::BroadcastTransport::~BroadcastTransport ()
{
  close ();
}

void
madara::transport::BroadcastTransport::close (void)
{
  this->invalidate_transport ();

  read_threads_.terminate ();

  read_threads_.wait ();
  
  socket_.close ();
}

int
madara::transport::BroadcastTransport::reliability (void) const
{
  return madara::transport::BEST_EFFORT;
}

int
madara::transport::BroadcastTransport::reliability (const int &)
{
  return madara::transport::BEST_EFFORT;
}

int
madara::transport::BroadcastTransport::setup (void)
{
  // call base setup method to initialize certain common variables
  Base::setup ();

  // resize addresses to be the size of the list of hosts
  addresses_.resize (this->settings_.hosts.size ());
  
  if (addresses_.size () > 0)
  {
    // convert the string host:port into an ACE address
    for (unsigned int i = 0; i < addresses_.size (); ++i)
    {
      addresses_[i].set (settings_.hosts[i].c_str ());

      madara_logger_log (context_.get_logger (), logger::LOG_MAJOR,
        "BroadcastTransport::constructor" \
        " settings address[%d] to %s:%d\n", i,
        addresses_[i].get_host_addr (), addresses_[i].get_port_number ());
    }
    
    // open the broadcast socket to any port for sending
    if (socket_.open (ACE_Addr::sap_any) == -1)
    {
      madara_logger_log (context_.get_logger (), logger::LOG_MAJOR,
        "BroadcastTransport::constructor" \
        "socket failed to open\n");
    }
    else
    {
      int send_buff_size = 0, tar_buff_size (settings_.queue_length);
      int rcv_buff_size = 0;
      int opt_len = sizeof (int);

      socket_.get_option (SOL_SOCKET, SO_SNDBUF,
        (void *)&send_buff_size, &opt_len);

      socket_.get_option (SOL_SOCKET, SO_RCVBUF,
        (void *)&rcv_buff_size, &opt_len);

      madara_logger_log (context_.get_logger (), logger::LOG_MAJOR,
        "BroadcastTransport::constructor" \
        " default socket buff size is send=%d, rcv=%d\n",
        send_buff_size, rcv_buff_size);
  
      if (send_buff_size < tar_buff_size)
      {
        madara_logger_log (context_.get_logger (), logger::LOG_MAJOR,
          "BroadcastTransport::constructor" \
          " setting send buff size to settings.queue_length (%d)\n",
          tar_buff_size);
  
        socket_.set_option (SOL_SOCKET, SO_SNDBUF,
          (void *)&tar_buff_size, opt_len);
    
        socket_.get_option (SOL_SOCKET, SO_SNDBUF,
          (void *)&send_buff_size, &opt_len);

        madara_logger_log (context_.get_logger (), logger::LOG_MAJOR,
          "BroadcastTransport::constructor" \
          " current socket buff size is send=%d, rcv=%d\n",
          send_buff_size, rcv_buff_size);
      }
  
      if (rcv_buff_size < tar_buff_size)
      {
        madara_logger_log (context_.get_logger (), logger::LOG_MAJOR,
          "BroadcastTransport::constructor" \
          " setting rcv buff size to settings.queue_length (%d)\n",
          tar_buff_size);
  
        socket_.set_option (SOL_SOCKET, SO_RCVBUF,
          (void *)&tar_buff_size, opt_len);
    
        socket_.get_option (SOL_SOCKET, SO_RCVBUF,
          (void *)&rcv_buff_size, &opt_len);

        madara_logger_log (context_.get_logger (), logger::LOG_MAJOR,
          "BroadcastTransport::constructor" \
          " current socket buff size is send=%d, rcv=%d\n",
          send_buff_size, rcv_buff_size);
      }
      
      if (!settings_.no_receiving)
      {
        // start the read threads
        double hertz = settings_.read_thread_hertz;
        if (hertz < 0.0)
        {
          hertz = 0.0;
        }

        madara_logger_log (context_.get_logger (), logger::LOG_MAJOR,
          "BroadcastTransport::constructor:" \
          " starting %d threads at %f hertz\n", settings_.read_threads,
          hertz);

        for (uint32_t i = 0; i < settings_.read_threads; ++i)
        {
          std::stringstream thread_name;
          thread_name << "read";
          thread_name << i;

          read_threads_.run (hertz, thread_name.str (),
            new BroadcastTransportReadThread (
              settings_, id_, addresses_[0], socket_,
              send_monitor_, receive_monitor_, packet_scheduler_));
        }
      }
    }
    
  }
  return this->validate_transport ();
}

long
madara::transport::BroadcastTransport::send_data (
  const madara::knowledge::VariableReferenceMap & orig_updates)
{
  const char * print_prefix = "BroadcastTransport::send_data";
  long result (0);

  if (!settings_.no_sending)
  {
    result = prep_send (orig_updates, print_prefix);

    if (addresses_.size () > 0 && result > 0)
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

        int j = 0;
        for (FragmentMap::iterator i = map.begin (); i != map.end (); ++i, ++j)
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
            actual_sent = socket_.send (
              i->second,
              (ssize_t)MessageHeader::get_size (i->second),
              addresses_[0]);

            ++send_attempts;

            if (actual_sent > 0)
            {
              madara_logger_log (context_.get_logger (), logger::LOG_MAJOR,
                "%s:" \
                " Sent %d byte fragment\n",
                print_prefix, (int)actual_sent);

              bytes_sent += actual_sent;
            }
            else
            {
              madara_logger_log (context_.get_logger (), logger::LOG_MAJOR,
                "%s:" \
                " Send fragment failed, socket busy\n", print_prefix);
            }
          }

          // sleep between fragments, if such a slack time is specified
          if (settings_.slack_time > 0)
            madara::utility::sleep (settings_.slack_time);
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
          actual_sent = socket_.send (
            buffer_.get_ptr (), (ssize_t)result, addresses_[0]);

          ++send_attempts;

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

      madara_logger_log (context_.get_logger (), logger::LOG_MINOR,
        "%s:" \
        " Send bandwidth = %" PRIu64 " B/s\n",
        print_prefix, send_monitor_.get_bytes_per_second ());

      result = (long) bytes_sent;
    }
  }

  return result;
}
