
#include "madara/transport/multicast/MulticastTransport.h"
#include "madara/transport/multicast/MulticastTransportReadThread.h"
#include "madara/transport/TransportContext.h"

#include "madara/transport/ReducedMessageHeader.h"
#include "madara/utility/Utility.h"
#include "madara/expression/ExpressionTree.h"
#include "madara/expression/Interpreter.h"
#include "madara/transport/Fragmentation.h"

#include <iostream>
#include "madara/utility/IntTypes.h"

namespace madara { namespace transport {

namespace mcast = ip::multicast;

MulticastTransport::MulticastTransport (const std::string & id,
        knowledge::ThreadSafeContext & context,
        TransportSettings & config, bool launch_transport)
: UdpTransport (id, context, config, false)
{
  if (launch_transport)
    setup ();
}

int
MulticastTransport::setup_read_thread (
    double hertz, const std::string &name)
{
  read_threads_.run (hertz, name,
    new MulticastTransportReadThread (*this));

  return 0;
}

int
MulticastTransport::setup_read_socket (void)
{
  try {
    asio::socket_base::reuse_address reuse_option(true);
    socket_.set_option(reuse_option);
  } catch (const boost::system::system_error &e) {
    madara_logger_log (context_.get_logger (), logger::LOG_MAJOR,
      "MulticastTransport::setup:" \
      " Error setting up read socket: %s\n", e.what ());

    this->invalidate_transport ();
    return -1;
  }

  if (UdpTransport::setup_read_socket () < 0) {
    return -1;
  }

  try {
    socket_.set_option (mcast::join_group (addresses_[0].address ()));
  } catch (const boost::system::system_error &e) {
    madara_logger_log (context_.get_logger (), logger::LOG_MAJOR,
      "MulticastTransport::setup_read_socket:" \
      " Error subscribing: %s\n", e.what ());

    this->invalidate_transport ();
    return -1;
  }

  return 0;
}

int
MulticastTransport::setup_write_socket (void)
{
  if (UdpTransport::setup_write_socket () < 0) {
    return -1;
  }

  try {
    socket_.set_option (mcast::hops (1));
  } catch (const boost::system::system_error &e) {
    madara_logger_log (context_.get_logger (), logger::LOG_MAJOR,
      "MulticastTransport::setup_write_socket:" \
      " Error setting TTL: %s\n", e.what ());

    this->invalidate_transport ();
    return -1;
  }

  return 0;
}

} }
