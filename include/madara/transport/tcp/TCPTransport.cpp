#include "madara/transport/tcp/TCPTransport.h"
#include "madara/transport/tcp/TCPTransportReadThread.h"
#include "madara/transport/TransportContext.h"

#include "madara/knowledge/KnowledgeRecord.h"
#include "madara/transport/ReducedMessageHeader.h"
#include "madara/utility/Utility.h"
#include "ace/INET_Addr.h"
#include "ace/SOCK_Dgram.h"


#include <iostream>

madara::transport::TCPTransport::TCPTransport (const std::string & id,
  madara::knowledge::ThreadSafeContext & context,
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

madara::transport::TCPTransport::~TCPTransport ()
{
  close ();
}

void
madara::transport::TCPTransport::close (void)
{
  this->invalidate_transport ();

  read_threads_.terminate ();

  read_threads_.wait ();
}

int
madara::transport::TCPTransport::reliability (void) const
{
  return RELIABLE;
}

int
madara::transport::TCPTransport::reliability (const int &)
{
  return RELIABLE;
}

int
madara::transport::TCPTransport::setup (void)
{
  // call base setup method to initialize certain common variables
  Base::setup ();

  if (settings_.hosts.size () > 0)
  {
    for (size_t i = 0; i < settings_.hosts.size (); ++i)
    {
      addresses_[settings_.hosts[i]].set (settings_.hosts[i].c_str ());
    }

    int send_buff_size = 0, tar_buff_size (settings_.queue_length);
    int rcv_buff_size = 0;
    int opt_len = sizeof (int);

    madara_logger_log (context_.get_logger (), logger::LOG_MAJOR,
      "TCPTransport::setup:" \
      " default socket buff size is send=%d, rcv=%d\n",
      send_buff_size, rcv_buff_size);

    if (send_buff_size < tar_buff_size)
    {
      madara_logger_log (context_.get_logger (), logger::LOG_MAJOR,
        "TCPTransport::setup:" \
        " setting send buff size to settings.queue_length (%d)\n",
        tar_buff_size);

      madara_logger_log (context_.get_logger (), logger::LOG_MAJOR,
        "TCPTransport::setup:" \
        " current socket buff size is send=%d, rcv=%d\n",
        send_buff_size, rcv_buff_size);
    }

    if (rcv_buff_size < tar_buff_size)
    {
      madara_logger_log (context_.get_logger (), logger::LOG_MAJOR,
        "TCPTransport::setup:" \
        " setting rcv buff size to settings.queue_length (%d)\n",
        tar_buff_size);
      
      madara_logger_log (context_.get_logger (), logger::LOG_MAJOR,
        "TCPTransport::setup:" \
        " current socket buff size is send=%d, rcv=%d\n",
        send_buff_size, rcv_buff_size);
    }


    if (addresses_.size () > 0)
    {

    } // if appropriate addresses

    if (!settings_.no_receiving)
    {
      double hertz = settings_.read_thread_hertz;
      if (hertz < 0.0)
      {
        hertz = 0.0;
      }

      madara_logger_log (context_.get_logger (), logger::LOG_MAJOR,
        "TCPTransportReadThread::setup:" \
        " starting %d threads at %f hertz\n", settings_.read_threads,
        hertz);

      for (uint32_t i = 0; i < settings_.read_threads; ++i)
      {
        std::stringstream thread_name;
        thread_name << "read";
        thread_name << i;

        read_threads_.run (hertz, thread_name.str (),
          new TCPTransportReadThread (
          settings_, id_, addresses_,
          send_monitor_, receive_monitor_, packet_scheduler_));
      }
    }
  }

  return this->validate_transport ();
}

long
madara::transport::TCPTransport::send_data (
const madara::KnowledgeRecords & orig_updates)
{
  const char * print_prefix = "TCPTransport::send_data";

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

        for (FragmentMap::iterator i = map.begin (); i != map.end (); ++i)
        {
          size_t frag_size =
            (size_t)MessageHeader::get_size (i->second);

          for (std::map <std::string, ACE_INET_Addr>::const_iterator addr =
            addresses_.begin (); addr != addresses_.end (); ++addr)
          {
            if (addr->first != settings_.hosts[0])
            {
              ssize_t actual_sent = -1;

              // sleep between fragments, if such a slack time is specified
              if (settings_.slack_time > 0)
                madara::utility::sleep (settings_.slack_time);

              madara_logger_log (context_.get_logger (), logger::LOG_MAJOR,
                "%s: Send result was %d of %d byte fragment to %s\n",
                print_prefix, actual_sent, frag_size, addr->first.c_str ());

              if (actual_sent > 0)
              {
                send_monitor_.add ((uint32_t)actual_sent);
                bytes_sent += actual_sent;
              }
            }
          }
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
            ssize_t actual_sent = -1;

            madara_logger_log (context_.get_logger (), logger::LOG_MAJOR,
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
      }

      result = (long)bytes_sent;

      madara_logger_log (context_.get_logger (), logger::LOG_MAJOR,
        "%s:" \
        " Send bandwidth = %" PRIu64 " B/s\n",
        print_prefix, send_monitor_.get_bytes_per_second ());
    }
  }

  return result;
}
