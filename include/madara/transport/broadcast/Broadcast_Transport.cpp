#include "madara/transport/broadcast/Broadcast_Transport.h"
#include "madara/transport/broadcast/Broadcast_Transport_Read_Thread.h"
#include "madara/transport/Transport_Context.h"

#include "madara/transport/Reduced_Message_Header.h"
#include "madara/utility/Utility.h"

#include <iostream>

Madara::Transport::Broadcast_Transport::Broadcast_Transport (const std::string & id,
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

Madara::Transport::Broadcast_Transport::~Broadcast_Transport ()
{
  close ();
}

void
Madara::Transport::Broadcast_Transport::close (void)
{
  this->invalidate_transport ();

  read_threads_.terminate ();

  read_threads_.wait ();
  
  socket_.close ();
}

int
Madara::Transport::Broadcast_Transport::reliability (void) const
{
  return Madara::Transport::BEST_EFFORT;
}

int
Madara::Transport::Broadcast_Transport::reliability (const int &)
{
  return Madara::Transport::BEST_EFFORT;
}

int
Madara::Transport::Broadcast_Transport::setup (void)
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

      this->context_.get_logger ().log (Logger::LOG_MAJOR,
        "Broadcast_Transport::constructor" \
        " settings address[%d] to %s:%d\n", i,
        addresses_[i].get_host_addr (), addresses_[i].get_port_number ());
    }
    
    // open the broadcast socket to any port for sending
    if (socket_.open (ACE_Addr::sap_any) == -1)
    {
      this->context_.get_logger ().log (Logger::LOG_MAJOR,
        "Broadcast_Transport::constructor" \
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

      this->context_.get_logger ().log (Logger::LOG_MAJOR,
        "Broadcast_Transport::constructor" \
        " default socket buff size is send=%d, rcv=%d\n",
        send_buff_size, rcv_buff_size);
  
      if (send_buff_size < tar_buff_size)
      {
        this->context_.get_logger ().log (Logger::LOG_MAJOR,
          "Broadcast_Transport::constructor" \
          " setting send buff size to settings.queue_length (%d)\n",
          tar_buff_size);
  
        socket_.set_option (SOL_SOCKET, SO_SNDBUF,
          (void *)&tar_buff_size, opt_len);
    
        socket_.get_option (SOL_SOCKET, SO_SNDBUF,
          (void *)&send_buff_size, &opt_len);

        this->context_.get_logger ().log (Logger::LOG_MAJOR,
          "Broadcast_Transport::constructor" \
          " current socket buff size is send=%d, rcv=%d\n",
          send_buff_size, rcv_buff_size);
      }
  
      if (rcv_buff_size < tar_buff_size)
      {
        this->context_.get_logger ().log (Logger::LOG_MAJOR,
          "Broadcast_Transport::constructor" \
          " setting rcv buff size to settings.queue_length (%d)\n",
          tar_buff_size);
  
        socket_.set_option (SOL_SOCKET, SO_SNDBUF,
          (void *)&tar_buff_size, opt_len);
    
        socket_.get_option (SOL_SOCKET, SO_SNDBUF,
          (void *)&rcv_buff_size, &opt_len);

        this->context_.get_logger ().log (Logger::LOG_MAJOR,
          "Broadcast_Transport::constructor" \
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

        this->context_.get_logger ().log (Logger::LOG_MAJOR,
          "Broadcast_Transport::constructor:" \
          " starting %d threads at %f hertz\n", settings_.read_threads,
          hertz);

        for (uint32_t i = 0; i < settings_.read_threads; ++i)
        {
          std::stringstream thread_name;
          thread_name << "read";
          thread_name << i;

          read_threads_.run (hertz, thread_name.str (),
            new Broadcast_Transport_Read_Thread (
              settings_, id_, addresses_[0], socket_,
              send_monitor_, receive_monitor_, packet_scheduler_));
        }
      }
    }
    
  }
  return this->validate_transport ();
}

long
Madara::Transport::Broadcast_Transport::send_data (
  const Madara::Knowledge_Records & orig_updates)
{
  const char * print_prefix = "Broadcast_Transport::send_data";
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
        Fragment_Map map;

        this->context_.get_logger ().log (Logger::LOG_MAJOR,
          "%s:" \
          " fragmenting %" PRIu64 " byte packet (%" PRIu32 " bytes is max fragment size)\n",
          print_prefix, packet_size, settings_.max_fragment_size);

        // fragment the message
        frag (buffer_.get_ptr (), settings_.max_fragment_size, map);

        for (Fragment_Map::iterator i = map.begin (); i != map.end (); ++i)
        {
          // send the fragment
          bytes_sent += socket_.send(
            i->second,
            (ssize_t)Message_Header::get_size (i->second),
            addresses_[0]);

          // sleep between fragments, if such a slack time is specified
          if (settings_.slack_time > 0)
            Madara::Utility::sleep (settings_.slack_time);
        }
      
        send_monitor_.add ((uint32_t)bytes_sent);

        this->context_.get_logger ().log (Logger::LOG_MAJOR,
          "%s:" \
          " Sent fragments totalling %" PRIu64 " bytes\n",
          print_prefix, bytes_sent);

        delete_fragments (map);
      }
      else
      {
        bytes_sent = socket_.send(
          buffer_.get_ptr (), (ssize_t)result, addresses_[0]);

        this->context_.get_logger ().log (Logger::LOG_MAJOR,
          "%s:" \
          " Sent packet of size %" PRIu64 " bytes\n",
          print_prefix, bytes_sent);
        send_monitor_.add ((uint32_t)bytes_sent);
      }

      this->context_.get_logger ().log (Logger::LOG_MINOR,
        "%s:" \
        " Send bandwidth = %" PRIu64 " B/s\n",
        print_prefix, send_monitor_.get_bytes_per_second ());

      result = (long) bytes_sent;
    }
  }

  return result;
}
