#include "madara/transport/multicast/MulticastTransportReadThread.h"

#include "madara/transport/ReducedMessageHeader.h"
#include "madara/transport/Fragmentation.h"
#include "madara/utility/Utility.h"


#include <iostream>
#include <algorithm>

namespace madara { namespace transport {

namespace mcast = ip::multicast;

void
MulticastTransportReadThread::cleanup (void)
{
  if (transport_.addresses_.size () == 0) {
    return;
  }

  try {
    transport_.socket_.set_option (mcast::leave_group (
          transport_.addresses_[0].address ()));
  } catch (const boost::system::system_error &e) {
    madara_logger_log (this->context_->get_logger (), logger::LOG_MAJOR,
      "MulticastTransportReadThread::close:" \
      " Error unsubscribing to multicast address: %s\n", e.what ());
  }

  UdpTransportReadThread::cleanup();
}

} }
