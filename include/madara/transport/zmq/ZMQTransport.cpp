
#include "madara/transport/zmq/ZMQTransport.h"
#include "madara/transport/zmq/ZMQTransportReadThread.h"
#include "madara/transport/TransportContext.h"

#include "madara/transport/ReducedMessageHeader.h"
#include "madara/utility/Utility.h"
#include "madara/expression/ExpressionTree.h"
#include "madara/expression/Interpreter.h"
#include "madara/transport/Fragmentation.h"

#include <iostream>
#include "madara/utility/IntTypes.h"
#include "ZMQContext.h"

madara::transport::ZMQTransport::ZMQTransport (const std::string & id,
        madara::knowledge::ThreadSafeContext & context, 
        TransportSettings & config, bool launch_transport)
: Base (id, config, context), write_socket_ (0)
{
  // create a reference to the knowledge base for threading
  knowledge_.use (context);

  // set the data plane for the read threads
  read_threads_.set_data_plane (knowledge_);

  // keep track of references to context so management is automatic
  zmq_context.add_ref ();

  if (launch_transport)
    setup ();

}

madara::transport::ZMQTransport::~ZMQTransport ()
{
  close ();

  // destroy context if it has no references anymore
  zmq_context.rem_ref ();
}

void
madara::transport::ZMQTransport::close (void)
{
  this->invalidate_transport ();

  if (write_socket_ != 0)
  {
    int option = 2000;

    // if you don't do this, ZMQ waits forever for no reason. Super smart.
    int result = zmq_setsockopt (
      write_socket_, ZMQ_LINGER, (void *)&option, sizeof (int));

    madara::utility::sleep (0.100);

    result = zmq_close (write_socket_);

    if (result != 0)
    {
      madara_logger_log (context_.get_logger (), logger::LOG_ERROR,
        "ZMQTransport::close:" \
        " ERROR: errno = %s\n",
        zmq_strerror (zmq_errno ()));
    }
  }

  madara_logger_log (context_.get_logger (), logger::LOG_MAJOR,
    "ZMQTransport::close:" \
    " calling terminate on read threads\n");

  read_threads_.terminate ();

  madara_logger_log (context_.get_logger (), logger::LOG_MAJOR,
    "ZMQTransport::close:" \
    " waiting on read threads\n");

  read_threads_.wait ();

  madara_logger_log (context_.get_logger (), logger::LOG_MAJOR,
    "ZMQTransport::close:" \
    " waiting on read threads\n");

}

int
madara::transport::ZMQTransport::reliability (void) const
{
  return madara::transport::BEST_EFFORT;
}

int
madara::transport::ZMQTransport::reliability (const int &)
{
  return madara::transport::BEST_EFFORT;
}

int
madara::transport::ZMQTransport::setup (void)
{
  // call base setup method to initialize certain common variables
  Base::setup ();

  if (settings_.hosts.size () > 0)
  {

    madara_logger_log (context_.get_logger (), logger::LOG_MAJOR,
      "ZMQTransport::setup:" \
      " setting up write socket\n");

    write_socket_ = zmq_socket (zmq_context.get_context (), ZMQ_PUB);

    if (write_socket_ == NULL)
    {
      madara_logger_log (context_.get_logger (), logger::LOG_ERROR,
        "ZMQTransport::setup:" \
        " ERROR: could not create PUB socket\n");
      madara_logger_log (context_.get_logger (), logger::LOG_ERROR,
        "ZMQTransport::setup:" \
        " ERROR: errno = %s\n",
        zmq_strerror (zmq_errno ()));
    }

    madara_logger_log (context_.get_logger (), logger::LOG_MAJOR,
      "ZMQTransport::setup:" \
      " binding write to %s\n",
      settings_.hosts[0].c_str ());

    int bind_result = zmq_bind (write_socket_, settings_.hosts[0].c_str ());
    
    if (bind_result != 0)
    {
      madara_logger_log (context_.get_logger (), logger::LOG_ERROR,
        "ZMQTransport::setup:" \
        " ERROR: could not bind to %s\n",
        settings_.hosts[0].c_str ());
      madara_logger_log (context_.get_logger (), logger::LOG_ERROR,
        "ZMQTransport::setup:" \
        " ERROR: errno = %s\n",
        zmq_strerror (zmq_errno ()));
    }
    else
    {
      madara_logger_log (context_.get_logger (), logger::LOG_MAJOR,
        "ZMQTransport::setup:" \
        " successfully bound to %s\n",
        settings_.hosts[0].c_str ());
    }


    int send_buff_size = 0;
    //int rcv_buff_size = 0;
    int buff_size = settings_.queue_length;
    int timeout = 300;
    size_t opt_len = sizeof (int);

    madara_logger_log (context_.get_logger (), logger::LOG_MAJOR,
      "ZMQTransport::setup:" \
      " setting send buff size to settings.queue_length (%d)\n",
      buff_size);

    int result = zmq_setsockopt (
      write_socket_, ZMQ_SNDBUF, (void *)&buff_size, opt_len);

    if (result == 0)
    {
      result = zmq_getsockopt (
        write_socket_, ZMQ_SNDBUF, (void *)&send_buff_size, &opt_len);

      madara_logger_log (context_.get_logger (), logger::LOG_MAJOR,
        "ZMQTransport::setup:" \
        " successfully set sockopt sendbuf size to %d. Actual %d allocated\n",
        buff_size, send_buff_size);
    }
    else
    {
      madara_logger_log (context_.get_logger (), logger::LOG_ERROR,
        "ZMQTransport::setup:" \
        " ERROR: errno = %s\n",
        zmq_strerror (zmq_errno ()));
    }


    result = zmq_setsockopt (
      write_socket_, ZMQ_SNDTIMEO, (void *)&timeout, opt_len);

    if (result == 0)
    {
      result = zmq_getsockopt (
        write_socket_, ZMQ_SNDTIMEO, (void *)&timeout, &opt_len);

      madara_logger_log (context_.get_logger (), logger::LOG_MAJOR,
        "ZMQTransport::setup:" \
        " successfully set send timeout to %d\n",
        timeout);
    }
    else
    {
      madara_logger_log (context_.get_logger (), logger::LOG_ERROR,
        "ZMQTransport::setup:" \
        " ERROR: When setting timeout on send, errno = %s\n",
        zmq_strerror (zmq_errno ()));
    }

    if (!settings_.no_receiving)
    {
      double hertz = settings_.read_thread_hertz;
      if (hertz < 0.0)
      {
        hertz = 0.0;
      }

      madara_logger_log (context_.get_logger (), logger::LOG_MAJOR,
        "ZMQTransport::setup:" \
        " starting %d threads at %f hertz\n", settings_.read_threads,
        hertz);

      for (uint32_t i = 0; i < settings_.read_threads; ++i)
      {
        std::stringstream thread_name;
        thread_name << "read";
        thread_name << i;

        read_threads_.run (hertz, thread_name.str (),
          new ZMQTransportReadThread (
            settings_, id_, write_socket_,
            send_monitor_, receive_monitor_, packet_scheduler_));
      }
    }
  }

  return this->validate_transport ();
}

long
madara::transport::ZMQTransport::send_data (
  const madara::knowledge::VariableReferenceMap & orig_updates)
{
  long result (0);
  const char * print_prefix = "ZMQTransport::send_data";

  if (!settings_.no_sending)
  {
    result = prep_send (orig_updates, print_prefix);

    if (settings_.hosts.size () > 0 && result > 0)
    {
      madara_logger_log (context_.get_logger (), logger::LOG_MAJOR,
        "ZMQTransport::send:" \
        " sending %d bytes on socket\n", (int)result);

      //send the prepped buffer over ZeroMQ with timeout of 300ms
      result = (long) zmq_send (
        write_socket_, (void *)buffer_.get_ptr (), (size_t)result, 0);

      madara_logger_log (context_.get_logger (), logger::LOG_MAJOR,
        "ZMQTransport::send:" \
        " sent %d bytes on socket\n", (int)result);
    }
  }

  return result;
}
