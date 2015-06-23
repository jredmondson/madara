#include "madara/transport/tcp/TCP_Transport.h"
#include "madara/transport/tcp/TCP_Transport_Read_Thread.h"
#include "madara/transport/Transport_Context.h"

#include "madara/knowledge_engine/Knowledge_Record.h"
#include "madara/transport/Reduced_Message_Header.h"
#include "madara/utility/Utility.h"
#include "ace/INET_Addr.h"
#include "ace/SOCK_Dgram.h"


#include <iostream>

Madara::Transport::TCP_Transport::TCP_Transport (const std::string & id,
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

Madara::Transport::TCP_Transport::~TCP_Transport ()
{
  close ();
}

void
Madara::Transport::TCP_Transport::close (void)
{
  this->invalidate_transport ();

  read_threads_.terminate ();

  read_threads_.wait ();
}

int
Madara::Transport::TCP_Transport::reliability (void) const
{
  return RELIABLE;
}

int
Madara::Transport::TCP_Transport::reliability (const int &)
{
  return RELIABLE;
}

int
Madara::Transport::TCP_Transport::setup (void)
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

    context_.get_logger ().log (Logger::LOG_MAJOR,
      "TCP_Transport::setup:" \
      " default socket buff size is send=%d, rcv=%d\n",
      send_buff_size, rcv_buff_size);

    if (send_buff_size < tar_buff_size)
    {
      context_.get_logger ().log (Logger::LOG_MAJOR,
        "TCP_Transport::setup:" \
        " setting send buff size to settings.queue_length (%d)\n",
        tar_buff_size);

      context_.get_logger ().log (Logger::LOG_MAJOR,
        "TCP_Transport::setup:" \
        " current socket buff size is send=%d, rcv=%d\n",
        send_buff_size, rcv_buff_size);
    }

    if (rcv_buff_size < tar_buff_size)
    {
      context_.get_logger ().log (Logger::LOG_MAJOR,
        "TCP_Transport::setup:" \
        " setting rcv buff size to settings.queue_length (%d)\n",
        tar_buff_size);
      
      context_.get_logger ().log (Logger::LOG_MAJOR,
        "TCP_Transport::setup:" \
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

      context_.get_logger ().log (Logger::LOG_MAJOR,
        "TCP_Transport_Read_Thread::setup:" \
        " starting %d threads at %f hertz\n", settings_.read_threads,
        hertz);

      for (uint32_t i = 0; i < settings_.read_threads; ++i)
      {
        std::stringstream thread_name;
        thread_name << "read";
        thread_name << i;

        read_threads_.run (hertz, thread_name.str (),
          new TCP_Transport_Read_Thread (
          settings_, id_, addresses_,
          send_monitor_, receive_monitor_, packet_scheduler_));
      }
    }
  }

  return this->validate_transport ();
}

long
Madara::Transport::TCP_Transport::send_data (
const Madara::Knowledge_Records & orig_updates)
{
  const char * print_prefix = "TCP_Transport::send_data";

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
        Fragment_Map map;

        context_.get_logger ().log (Logger::LOG_MAJOR,
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
              ssize_t actual_sent = 0;

              // sleep between fragments, if such a slack time is specified
              if (settings_.slack_time > 0)
                Madara::Utility::sleep (settings_.slack_time);

              context_.get_logger ().log (Logger::LOG_MAJOR,
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

        context_.get_logger ().log (Logger::LOG_MAJOR,
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
            ssize_t actual_sent = 0;

            context_.get_logger ().log (Logger::LOG_MAJOR,
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

      context_.get_logger ().log (Logger::LOG_MAJOR,
        "%s:" \
        " Send bandwidth = %" PRIu64 " B/s\n",
        print_prefix, send_monitor_.get_bytes_per_second ());
    }
  }

  return result;
}
