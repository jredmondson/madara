#include "CustomTransport.h"
#include "CustomTransportReadThread.h"
#include "madara/transport/MessageHeader.h"
#include "madara/utility/Utility.h"
#include "madara/logger/GlobalLogger.h"

#include <iostream>

/**
* Define helpful shortened namespaces that we can refer to later
**/
namespace engine = Madara::KnowledgeEngine;
namespace transport = Madara::Transport;
namespace logger = Madara::Logger;

CustomTransport::CustomTransport (const std::string & id,
        engine::ThreadSafeContext & context, 
        transport::Settings & config, bool launch_transport)
: Base (id, config, context),
  thread_ (0), valid_setup_ (false),
  socket_ (ACE_sap_any_cast (ACE_INET_Addr &), PF_INET, 0, 1)
{
  if (launch_transport)
    setup ();
}

CustomTransport::~CustomTransport ()
{
  close ();
}

void
CustomTransport::close (void)
{
  this->invalidate_transport ();

  if (thread_)
  {
    thread_->close ();
    delete thread_;
    thread_ = 0;
  }
  
  socket_.close ();

  this->shutting_down_ = false;
}

int
CustomTransport::reliability (void) const
{
  return transport::BEST_EFFORT;
}

int
CustomTransport::reliability (const int &)
{
  return transport::BEST_EFFORT;
}

int
CustomTransport::setup (void)
{
  // resize addresses to be the size of the list of hosts
  addresses_.resize (this->settings_.hosts.size ());

  int ttl = 1;

  socket_.set_option (IPPROTO_IP,
                     IP_MULTICAST_TTL,
                     (void *) &ttl,
                     sizeof ttl);

  if (addresses_.size () > 0)
  {
    // convert the string host:port into an ACE address
    for (unsigned int i = 0; i < addresses_.size (); ++i)
    {
      addresses_[i].set (settings_.hosts[i].c_str ());

      madara_logger_log (context_.get_logger (), logger::LOG_MAJOR,
        "CustomTransport::setup:" \
        " settings address[%d] to %s:%d\n",
        (int)i,
        addresses_[i].get_host_addr (),
        (int)addresses_[i].get_port_number ());
    }
    
    int port = addresses_[0].get_port_number ();
    const char * host = addresses_[0].get_host_addr ();

    // start thread with the addresses (only looks at the first one for now)
    thread_ = new CustomTransportReadThread (
                    settings_, id_, context_, addresses_[0]);
  }
  return this->validate_transport ();
}

long
CustomTransport::send_data (
  const Madara::KnowledgeRecords & updates)
{
  // check to see if we are shutting down
  long ret = this->check_transport ();
  if (-1 == ret)
  {
    madara_logger_log (context_.get_logger (), logger::LOG_MAJOR,
      "CustomTransport::send_data: transport"
      "has been told to shutdown");

    return ret;
  }
  else if (-2 == ret)
  {
    madara_logger_log (context_.get_logger (), logger::LOG_MAJOR,
      "CustomTransport::send_data: transport is not valid");

    return ret;
  }
 
  // get the maximum quality from the updates
  uint32_t quality = Madara::max_quality (updates);


  // allocate a buffer to send
  char buffer [transport::MAX_PACKET_SIZE];
  int64_t buffer_remaining = transport::MAX_PACKET_SIZE;

  // set the header to the beginning of the buffer
  transport::MessageHeader header;
  
  // get the clock
  header.clock = Madara::Utility::endian_swap (context_.get_clock ());

  // copy the Madara transport version identification
  strncpy (header.madara_id, MADARA_IDENTIFIER, 7);

  // copy the domain from settings
  strncpy (header.domain, this->settings_.domains.c_str (),
    sizeof (header.domain) - 1);

  // get the quality of the key
  header.quality = Madara::Utility::endian_swap (quality);

  // copy the message originator (our id)
  strncpy (header.originator, id_.c_str (), sizeof (header.originator) - 1);

  // only 1 update in a send_data message
  header.updates = uint32_t (updates.size ());

  // send data is generally an assign type. However, MessageHeader is
  // flexible enough to support both, and this will simply our read thread
  // handling
  header.type =  transport::MULTIASSIGN;
  
  // compute size of this header
  header.size = header.encoded_size ();

  // set the update to the end of the header
  char * update = header.write (buffer, buffer_remaining);
  uint64_t * message_size = (uint64_t *)buffer;
  
  // Message header format
  // [size|id|domain|originator|type|updates|quality|clock|list of updates]
  
  /**
   * size = buffer[0] (unsigned 64 bit)
   * transport id = buffer[8] (8 byte)
   * domain = buffer[16] (32 byte domain name)
   * originator = buffer[48] (64 byte originator host:port)
   * type = buffer[112] (unsigned 32 bit type of message--usually MULTIASSIGN)
   * updates = buffer[116] (unsigned 32 bit number of updates)
   * quality = buffer[120] (unsigned 32 bit quality of message)
   * clock = buffer[124] (unsigned 64 bit clock for this message)
   * knowledge = buffer[132] (the new knowledge starts here)
  **/

  // Message update format
  // [key|value]
  
  int j = 0;
  for (Madara::KnowledgeRecords::const_iterator i = updates.begin ();
    i != updates.end (); ++i, ++j)
  {
    update = i->second->write (update, i->first, buffer_remaining);
  }
  
  if (buffer_remaining > 0)
  {
    int size = (int)(transport::MAX_PACKET_SIZE - buffer_remaining);
    *message_size = Madara::Utility::endian_swap ((uint64_t)size);
    
    // send the buffer contents to the multicast address
  
    if (addresses_.size () > 0)
    {
      int bytes_sent = socket_.send(
        buffer, size, addresses_[0]);
    }
  }

  return 0;
}
