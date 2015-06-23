#include "Transport.h"

#include "madara/utility/Utility.h"
#include "madara/expression_tree/Interpreter.h"

#include <algorithm>

Madara::Transport::Base::Base (const std::string & id,
  Settings & new_settings,
  Knowledge_Engine::Thread_Safe_Context & context) 
  : is_valid_ (false), shutting_down_ (false),
  valid_setup_ (mutex_), id_ (id),
  settings_ (new_settings), context_ (context)

#ifndef _MADARA_NO_KARL_
  , on_data_received_ (context.get_logger ())
#endif // _MADARA_NO_KARL_
{
  settings_.attach (&context_);
  packet_scheduler_.attach (&settings_);

#ifdef _USE_CID_
  settings_.setup ();
#endif // _USE_CID_
}

Madara::Transport::Base::~Base ()
{
}

int
Madara::Transport::Base::setup (void) 
{
  // check for an on_data_received ruleset
  if (settings_.on_data_received_logic.length () != 0)
  {
    context_.get_logger ().log (Logger::LOG_MAJOR,
      "Transport::Base::setup" \
      " setting rules to %s\n",
      settings_.on_data_received_logic.c_str ());
    
#ifndef _MADARA_NO_KARL_
    Madara::Expression_Tree::Interpreter interpreter;
    on_data_received_ = interpreter.interpret (context_,
      settings_.on_data_received_logic);
#endif // _MADARA_NO_KARL_
  }
  else
  {
    context_.get_logger ().log (Logger::LOG_MAJOR,
      "Transport::Base::setup" \
      " no permanent rules were set\n");
  }
  
  // setup the send buffer
  if (settings_.queue_length > 0)
    buffer_ = new char [settings_.queue_length];

  return validate_transport ();
}

void
Madara::Transport::Base::close (void)
{
  invalidate_transport ();
}

int
Madara::Transport::process_received_update (
  const char * buffer,
  uint32_t bytes_read,
  const std::string & id,
  Knowledge_Engine::Thread_Safe_Context & context,
  const QoS_Transport_Settings & settings,
  Bandwidth_Monitor & send_monitor,
  Bandwidth_Monitor & receive_monitor,
  Knowledge_Map & rebroadcast_records,
#ifndef _MADARA_NO_KARL_
  Knowledge_Engine::Compiled_Expression & on_data_received,
#endif // _MADARA_NO_KARL_

  const char * print_prefix,
  const char * remote_host,
  Message_Header *& header)
{
  // reset header to 0, so it is safe to delete
  header = 0;
  
  int max_buffer_size = (int)bytes_read;

  // tell the receive bandwidth monitor about the transaction
  receive_monitor.add (bytes_read);

  context.get_logger ().log (Logger::LOG_MAJOR,
    "%s:" \
    " Receive bandwidth = %" PRIu64 " B/s\n",
    print_prefix,
    receive_monitor.get_bytes_per_second ());

  bool is_reduced = false;
  bool is_fragment = false;
  bool previously_defragged = false;

  context.get_logger ().log (Logger::LOG_MAJOR,
    "%s:" \
    " calling decode filters on %" PRIu64 " bytes\n",
    print_prefix, bytes_read);

  // call decodes, if applicable
  bytes_read = (uint32_t)settings.filter_decode ((unsigned char *)buffer,
    max_buffer_size, max_buffer_size);

  context.get_logger ().log (Logger::LOG_MAJOR,
    "%s:" \
    " Decoding resulted in %d final bytes\n",
    print_prefix, bytes_read);

  // setup buffer remaining, used by the knowledge record read method
  int64_t buffer_remaining = (int64_t)bytes_read;

  // clear the rebroadcast records
  rebroadcast_records.clear ();

  // receive records will be what we pass to the aggregate filter
  Knowledge_Map updates;

  // check the buffer for a reduced message header
  if (bytes_read > Reduced_Message_Header::static_encoded_size () &&
      Reduced_Message_Header::reduced_message_header_test (buffer))
  {
    context.get_logger ().log (Logger::LOG_MINOR,
      "%s:" \
      " processing reduced KaRL message from %s\n",
      print_prefix,
      remote_host);

    header = new Reduced_Message_Header ();
    is_reduced = true;
  }
  else if (bytes_read > Message_Header::static_encoded_size () &&
    Message_Header::message_header_test (buffer))
  {
    context.get_logger ().log (Logger::LOG_MINOR,
      "%s:" \
      " processing KaRL message from %s\n",
      print_prefix,
      remote_host);
        
    header = new Message_Header ();
  }
  else if (bytes_read > Fragment_Message_Header::static_encoded_size () &&
    Fragment_Message_Header::fragment_message_header_test (buffer))
  {
    context.get_logger ().log (Logger::LOG_MINOR,
      "%s:" \
      " processing KaRL message from %s\n",
      print_prefix,
      remote_host);
        
    header = new Fragment_Message_Header ();
    is_fragment = true;
  }
  else
  {
    context.get_logger ().log (Logger::LOG_MINOR,
      "%s:" \
      " dropping non-KaRL message from %s:%d\n",
      print_prefix,
      remote_host);

    return -1;
  }
          
  const char * update = header->read (buffer, buffer_remaining);

  if (header->size < bytes_read)
  {
    context.get_logger ().log (Logger::LOG_MAJOR,
      "%s:" \
      " Message header.size (%" PRIu64 " bytes) is less than actual"
      " bytes read (%" PRIu64 " bytes). Dropping message.\n",
      print_prefix, header->size, bytes_read);

    return -1;
  }

  if (is_fragment && 
      exists (header->originator, header->clock,
      ((Fragment_Message_Header *)header)->update_number, settings.fragment_map))
  {
    context.get_logger ().log (Logger::LOG_MAJOR,
      "%s:" \
      " Fragment already exists in fragment map. Dropping.\n",
      print_prefix);

    return -1;
  }

  if (!is_reduced)
  {
    // reject the message if it is us as the originator (no update necessary)
    if (id == header->originator)
    {
      context.get_logger ().log (Logger::LOG_MAJOR,
        "%s:" \
        " dropping message from ourself\n",
        print_prefix);

      return -2;
    }
    else
    {
      context.get_logger ().log (Logger::LOG_DETAILED,
        "%s:" \
        " remote id (%s) is not our own\n",
        print_prefix,
        remote_host);
    }

    if (settings.is_trusted (remote_host))
    {
      context.get_logger ().log (Logger::LOG_DETAILED,
        "%s: remote id (%s) is trusted\n",
        print_prefix,
        remote_host);
    }
    else
    {
      context.get_logger ().log (Logger::LOG_MAJOR,
        "%s:" \
        " dropping message from untrusted peer (%s\n",
        print_prefix,
        remote_host);

      // delete the header and continue to the svc loop
      return -3;
    }

    std::string originator (header->originator);
        
    if (settings.is_trusted (originator))
    {
      context.get_logger ().log (Logger::LOG_MINOR,
        "%s:" \
        " originator (%s) is trusted\n",
        print_prefix,
        originator.c_str ());
    }
    else
    {
      context.get_logger ().log (Logger::LOG_MAJOR,
        "%s:" \
        " dropping message from untrusted originator (%s)\n",
        print_prefix,
        originator.c_str ());

      return -4;
    }

    // reject the message if it is from a different domain
    if (settings.domains != header->domain)
    {
      context.get_logger ().log (Logger::LOG_MAJOR,
        "%s:" \
        " remote id (%s) in a different domain (%s). Dropping message.\n",
        print_prefix,
        remote_host,
        header->domain);

      // delete the header and continue to the svc loop
      return -5;
    }
    else
    {
      context.get_logger ().log (Logger::LOG_MINOR,
        "%s:" \
        " remote id (%s) message is in our domain\n",
        print_prefix,
        remote_host);
    }
  }

  // fragments are special cases
  if (is_fragment)
  {
    // grab the fragment header
    Fragment_Message_Header * frag_header =
      dynamic_cast <Fragment_Message_Header *> (header);

    context.get_logger ().log (Logger::LOG_MAJOR,
      "%s:" \
      " Processing fragment %d of %s:%d.\n",
      print_prefix, frag_header->update_number,
      frag_header->originator, frag_header->clock);

    // add the fragment and attempt to defrag the message
    char * message = Transport::add_fragment (
      frag_header->originator, frag_header->clock,
      frag_header->update_number, buffer, settings.fragment_queue_length,
      settings.fragment_map, true);

    // if we have no return message, we may have previously defragged it
    if (!message)
    {
      //if (Transport::is_complete (frag_header->originator,
      //  frag_header->clock, settings.fragment_map))
      //{
      //  previously_defragged = true;
      //}
      return 0;
    }
    else
    {
      context.get_logger ().log (Logger::LOG_MAJOR,
        "%s:" \
        " Message has been pieced together from fragments. Processing...\n",
        print_prefix);

      /**
       * if we defragged the message, then we need to process the message.
       * In order to do that, we need to overwrite buffer with message
       * so it can be processed normally.
       **/
      buffer_remaining = (int64_t)frag_header->get_size (message);
      if (buffer_remaining <= settings.queue_length)
      {
        char * buffer_override = (char *)buffer;
        memcpy (buffer_override, message, frag_header->get_size (message));

        // check the buffer for a reduced message header
        if (Reduced_Message_Header::reduced_message_header_test (buffer))
        {
          context.get_logger ().log (Logger::LOG_MINOR,
            "%s:" \
            " processing reduced KaRL message from %s\n",
            print_prefix,
            remote_host);

          header = new Reduced_Message_Header ();
          is_reduced = true;
          update = header->read (buffer, buffer_remaining);
        }
        else if (Message_Header::message_header_test (buffer))
        {
          context.get_logger ().log (Logger::LOG_MINOR,
            "%s:" \
            " processing KaRL message from %s\n",
            print_prefix,
            remote_host);
        
          header = new Message_Header ();
          update = header->read (buffer, buffer_remaining);
        }

        delete [] message;
      }
    }
  }

  int actual_updates = 0;
  uint64_t current_time = time (NULL);
  double deadline = settings.get_deadline ();
  Transport_Context transport_context (
    Transport_Context::RECEIVING_OPERATION,
    receive_monitor.get_bytes_per_second (),
    send_monitor.get_bytes_per_second (),
    header->timestamp, time (NULL),
    header->domain, header->originator);

  uint64_t latency (0);
  
  if (deadline > 0)
  {
    if (header->timestamp < current_time)
    {
      latency = current_time - header->timestamp;

      if (latency > deadline)
      {
        context.get_logger ().log (Logger::LOG_MAJOR,
          "%s:" \
          " deadline violation (latency is %" PRIu64 ", deadline is %f).\n",
          print_prefix,
          latency, deadline);

        return -6;
      }
    }
    else
    {
      context.get_logger ().log (Logger::LOG_MINOR,
        "%s:" \
        " Cannot compute message latency." \
        " Message header timestamp is in the future.\n",
        " message.timestamp = %d, cur_timestamp = %d.\n",
        print_prefix,
        header->timestamp, current_time); 
    }
  }

  context.get_logger ().log (Logger::LOG_MINOR,
    "%s:" \
    " iterating over the %d updates\n",
    print_prefix,
    header->updates);

  // temporary record for reading from the updates buffer
  Knowledge_Record record;
  record.quality = header->quality;
  record.clock = header->clock;
  std::string key;

  bool dropped = false;

  if (send_monitor.is_bandwidth_violated (
        settings.get_send_bandwidth_limit ()))
  {
    dropped = true;
    context.get_logger ().log (Logger::LOG_MAJOR,
      "%s:" \
      " Send monitor has detected violation of bandwidth limit." \
      " Dropping packet from rebroadcast list\n", print_prefix);
  }
  else if (receive_monitor.is_bandwidth_violated (
    settings.get_total_bandwidth_limit ()))
  {
    dropped = true;
    context.get_logger ().log (Logger::LOG_MAJOR,
      "%s:" \
      " Receive monitor has detected violation of bandwidth limit." \
      " Dropping packet from rebroadcast list...\n", print_prefix);
  }
  else if (settings.get_participant_ttl () < header->ttl)
  {
    dropped = true;
    context.get_logger ().log (Logger::LOG_MAJOR,
      "%s:" \
      " Transport participant TTL is lower than header ttl." \
      " Dropping packet from rebroadcast list...\n", print_prefix);
  }

  context.get_logger ().log (Logger::LOG_MAJOR,
    "%s:" \
    " Applying %d updates\n", print_prefix, header->updates);

  // iterate over the updates
  for (uint32_t i = 0; i < header->updates; ++i)
  {
    // read converts everything into host format from the update stream
    update = record.read (update, key, buffer_remaining);

    if (buffer_remaining < 0)
    {
      context.get_logger ().log (Logger::LOG_EMERGENCY,
        "%s:" \
        " unable to process message. Buffer remaining is negative." \
        " Server is likely being targeted by custom KaRL tools.\n",
        print_prefix);

      // we do not delete the header as this will be cleaned up later
      break;
    }
    else
    {
      context.get_logger ().log (Logger::LOG_MINOR,
        "%s:" \
        " Applying receive filter to %s\n", print_prefix, key.c_str ());

      record = settings.filter_receive (record, key, transport_context);

      if (record.exists ())
      {
        context.get_logger ().log (Logger::LOG_MINOR,
          "%s:" \
          " Filter results were %s\n", print_prefix, record.to_string ().c_str ());

        updates[key] = record;
      }
      else
      {
        context.get_logger ().log (Logger::LOG_MINOR,
          "%s:" \
          " Filter resulted in dropping %s\n", print_prefix, key.c_str ());
      }
    }
  }
  
  const Knowledge_Map & additionals = transport_context.get_records ();
  
  if (additionals.size () > 0)
  {
    context.get_logger ().log (Logger::LOG_MAJOR,
      "%s:" \
      " %d additional records being handled after receive.\n", print_prefix,
      additionals.size ());

    for (Knowledge_Map::const_iterator i = additionals.begin ();
          i != additionals.end (); ++i)
    {
      updates[i->first] = i->second;
    }

    transport_context.clear_records ();

    if (header->ttl < 2)
      header->ttl = 2;

    // modify originator to indicate we are the originator of modifications
    strncpy (header->originator, id.c_str (), 
             sizeof (header->originator) - 1);

  }

  // apply aggregate receive filters
  if (settings.get_number_of_receive_aggregate_filters () > 0
      && updates.size () > 0)
  {
    context.get_logger ().log (Logger::LOG_MAJOR,
      "%s:" \
      " Applying aggregate receive filters.\n", print_prefix);


    settings.filter_receive (updates, transport_context);
  }
  else
  {
    context.get_logger ().log (Logger::LOG_MINOR,
      "%s:" \
      " No aggregate receive filters were applied...\n",
      print_prefix);
  }

  context.get_logger ().log (Logger::LOG_MINOR,
    "%s:" \
    " Locking the context to apply updates.\n", print_prefix);

  // lock the context
  context.lock ();

  context.get_logger ().log (Logger::LOG_MINOR,
    "%s:" \
    " Applying updates to context.\n", print_prefix);

  // apply updates from the update list
  for (Knowledge_Map::iterator i = updates.begin ();
    i != updates.end (); ++i)
  {
    int result = 0;

    result = i->second.apply (context, i->first, header->quality,
      header->clock, false);
    ++actual_updates;

    if (result != 1)
    {
      context.get_logger ().log (Logger::LOG_MAJOR,
        "%s:" \
        " update %s=%s was rejected\n",
        print_prefix,
        key.c_str (), record.to_string ().c_str ());
    }
    else
    {
      context.get_logger ().log (Logger::LOG_MAJOR,
        "%s:" \
        " update %s=%s was accepted\n",
        print_prefix,
        key.c_str (), record.to_string ().c_str ());
    }
  }

  // unlock the context
  context.unlock ();
  
  context.set_changed ();
  
  if (!dropped)
  {
    transport_context.set_operation (
      Transport_Context::REBROADCASTING_OPERATION);

    context.get_logger ().log (Logger::LOG_MINOR,
      "%s:" \
      " Applying rebroadcast filters to receive results.\n", print_prefix);

    // create a list of rebroadcast records from the updates
    for (Knowledge_Map::iterator i = updates.begin ();
         i != updates.end (); ++i)
    {
      i->second = settings.filter_rebroadcast (
        i->second, i->first, transport_context);

      if (i->second.exists ())
      {
        if (i->second.to_string () != "")
        {
          context.get_logger ().log (Logger::LOG_MINOR,
            "%s:" \
            " Filter results were %s\n", print_prefix,
            i->second.to_string ().c_str ());
        }
        rebroadcast_records[i->first] = i->second;
      }
      else
      {
        context.get_logger ().log (Logger::LOG_MINOR,
          "%s:" \
          " Filter resulted in dropping %s\n", print_prefix,
          i->first.c_str ());
      }
    }
  
    const Knowledge_Map & additionals = transport_context.get_records ();

    for (Knowledge_Map::const_iterator i = additionals.begin ();
          i != additionals.end (); ++i)
    {
      rebroadcast_records[i->first] = i->second;
    }

    context.get_logger ().log (Logger::LOG_MAJOR,
      "%s:" \
      " Applying aggregate rebroadcast filters to %d records.\n",
      print_prefix, rebroadcast_records.size ());
    
    // apply aggregate filters to the rebroadcast records
    if (settings.get_number_of_rebroadcast_aggregate_filters () > 0
      && rebroadcast_records.size () > 0)
    {
      settings.filter_rebroadcast (rebroadcast_records, transport_context);
    }
    else
    {
      context.get_logger ().log (Logger::LOG_MINOR,
        "%s:" \
        " No aggregate rebroadcast filters were applied...\n",
        print_prefix);
    }

    context.get_logger ().log (Logger::LOG_MINOR,
      "%s:" \
      " Returning to caller with %d rebroadcast records.\n",
      print_prefix, rebroadcast_records.size ());
    
  }
  else
  {
    context.get_logger ().log (Logger::LOG_MINOR,
      "%s:" \
      " Rebroadcast packet was dropped...\n",
      print_prefix);
  }

  // before we send to others, we first execute rules
  if (settings.on_data_received_logic.length () != 0)
  {
#ifndef _MADARA_NO_KARL_
    context.get_logger ().log (Logger::LOG_MAJOR,
      "%s:" \
      " evaluating rules in %s\n",
      print_prefix,
      settings.on_data_received_logic.c_str ());

    context.evaluate (on_data_received);
#endif // _MADARA_NO_KARL_

  }
  else
  {
    context.get_logger ().log (Logger::LOG_MINOR,
      "%s:" \
      " no permanent rules were set\n",
      print_prefix);
  }

  return actual_updates;
}


int
Madara::Transport::prep_rebroadcast (
  Knowledge_Engine::Thread_Safe_Context & context,
  char * buffer,
  int64_t & buffer_remaining,
  const QoS_Transport_Settings & settings,
  const char * print_prefix,
  Message_Header * header,
  const Knowledge_Map & records,
  Packet_Scheduler & packet_scheduler)
{
  int result = 0;
  
  if (header->ttl > 0 && records.size () > 0 && packet_scheduler.add ())
  {
    // keep track of the message_size portion of buffer
    uint64_t * message_size = (uint64_t *)buffer;
    int max_buffer_size = (int)buffer_remaining;

    // the number of updates will be the size of the records map
    header->updates = uint32_t (records.size ());

    // set the update to the end of the header
    char * update = header->write (buffer, buffer_remaining);

    for (Knowledge_Map::const_iterator i = records.begin ();
         i != records.end (); ++i)
    {
      update = i->second.write (update, i->first, buffer_remaining);
    }
    
    if (buffer_remaining > 0)
    {
      int size = (int)(settings.queue_length - buffer_remaining);
      *message_size = Madara::Utility::endian_swap ((uint64_t)size);

      context.get_logger ().log (Logger::LOG_MINOR,
        "%s:" \
        " %" PRIu64 " bytes prepped for rebroadcast packet\n",
        print_prefix, size);

      result = size;

      context.get_logger ().log (Logger::LOG_MAJOR,
        "%s:" \
        " calling encode filters\n",
        print_prefix);

      settings.filter_encode ((unsigned char *)buffer,
        result, max_buffer_size);
    }
    else
    {
      context.get_logger ().log (Logger::LOG_MAJOR,
        "%s:" \
        " Not enough buffer for rebroadcasting packet\n",
        print_prefix);

      result = -2;
    }
  }
  else
  {
    context.get_logger ().log (Logger::LOG_MINOR,
      "%s:" \
      " No rebroadcast necessary.\n",
      print_prefix);

    result = -1;
  }
  
  packet_scheduler.print_status (Logger::LOG_DETAILED, print_prefix);

  return result;
}

long Madara::Transport::Base::prep_send (
  const Madara::Knowledge_Records & orig_updates,
  const char * print_prefix)
{
  // check to see if we are shutting down
  long ret = this->check_transport ();
  if (-1 == ret)
  {
    context_.get_logger ().log (Logger::LOG_MAJOR,
      "%s: transport has been told to shutdown",
      print_prefix);

    return ret;
  }
  else if (-2 == ret)
  {
    context_.get_logger ().log (Logger::LOG_MAJOR,
      "%s: transport is not valid",
      print_prefix);

    return ret;
  }
 
  // get the maximum quality from the updates
  uint32_t quality = Madara::max_quality (orig_updates);
  bool reduced = false;

  Knowledge_Map filtered_updates;

  context_.get_logger ().log (Logger::LOG_MINOR,
    "%s:" \
    " Applying filters before sending...\n",
    print_prefix);
  
  Transport_Context transport_context (Transport_Context::SENDING_OPERATION,
      receive_monitor_.get_bytes_per_second (),
      send_monitor_.get_bytes_per_second (),
      (uint64_t) time (NULL), (uint64_t) time (NULL),
      settings_.domains,
      id_);

  bool dropped = false;

  if (send_monitor_.is_bandwidth_violated (
        settings_.get_send_bandwidth_limit ()))
  {
    dropped = true;
    context_.get_logger ().log (Logger::LOG_MAJOR,
      "%s:" \
      " Send monitor has detected violation of bandwidth limit." \
      " Dropping packet...\n", print_prefix);
  }
  else if (receive_monitor_.is_bandwidth_violated (
    settings_.get_total_bandwidth_limit ()))
  {
    dropped = true;
    context_.get_logger ().log (Logger::LOG_MAJOR,
      "%s:" \
      " Receive monitor has detected violation of bandwidth limit." \
      " Dropping packet...\n", print_prefix);
  }

  if (!dropped && packet_scheduler_.add ())
  {
    /**
     * filter the updates according to the filters specified by
     * the user in QoS_Transport_Settings (if applicable)
     **/
    for (Knowledge_Records::const_iterator i = orig_updates.begin ();
          i != orig_updates.end (); ++i)
    {
      // filter the record according to the send filter chain
      Knowledge_Record result = settings_.filter_send (*i->second, i->first,
        transport_context);

      if (result.exists ())
        filtered_updates[i->first] = result;
    }

    const Knowledge_Map & additionals = transport_context.get_records ();

    for (Knowledge_Map::const_iterator i = additionals.begin ();
         i != additionals.end (); ++i)
    {
      filtered_updates[i->first] = i->second;
    }
  }
  else
  {
    context_.get_logger ().log (Logger::LOG_MAJOR,
      "%s:" \
      " Packet scheduler has dropped packet...\n", print_prefix);

    return 0;
  }

  context_.get_logger ().log (Logger::LOG_MINOR,
    "%s:" \
    " Applying %d aggregate update send filters to %d updates...\n",
    print_prefix, settings_.get_number_of_send_aggregate_filters (),
    filtered_updates.size ());

  // apply the aggregate filters
  if (settings_.get_number_of_send_aggregate_filters () > 0 &&
      filtered_updates.size () > 0)
  {
    settings_.filter_send (filtered_updates, transport_context);
  }
  else
  {
    context_.get_logger ().log (Logger::LOG_MINOR,
      "%s:" \
      " No aggregate send filters were applied...\n",
      print_prefix);
  }

  packet_scheduler_.print_status (Logger::LOG_DETAILED, print_prefix);

  context_.get_logger ().log (Logger::LOG_MINOR,
    "%s:" \
    " Finished applying filters before sending...\n",
    print_prefix);

  if (filtered_updates.size () == 0)
  {
    context_.get_logger ().log (Logger::LOG_MINOR,
      "%s:" \
      " Filters removed all data. Nothing to send.\n",
      print_prefix);

    return 0;
  }

  // allocate a buffer to send
  char * buffer = buffer_.get_ptr ();
  int64_t buffer_remaining = settings_.queue_length;
  
  if (buffer == 0)
  {
    context_.get_logger ().log (Logger::LOG_EMERGENCY,
      "%s:" \
      " Unable to allocate buffer of size " PRIu32 ". Exiting thread.\n",
      print_prefix,
      settings_.queue_length);
    
    return -3;
  }


  // set the header to the beginning of the buffer
  Message_Header * header = 0;

  if (settings_.send_reduced_message_header)
  {
    context_.get_logger ().log (Logger::LOG_MINOR,
      "%s:" \
      " Preparing message with reduced message header.\n",
      print_prefix);

    header = new Reduced_Message_Header ();
    reduced = true;
  }
  else
  {
    context_.get_logger ().log (Logger::LOG_MINOR,
      "%s:" \
      " Preparing message with normal message header.\n",
      print_prefix);

    header = new Message_Header ();
  }

  // get the clock
  header->clock = context_.get_clock ();

  if (!reduced)
  {
    // copy the domain from settings
    strncpy (header->domain, this->settings_.domains.c_str (),
      sizeof (header->domain) - 1);

    // get the quality of the key
    header->quality = quality;

    // copy the message originator (our id)
    strncpy (header->originator, id_.c_str (), sizeof (header->originator) - 1);

    // send data is generally an assign type. However, Message_Header is
    // flexible enough to support both, and this will simply our read thread
    // handling
    header->type = Madara::Transport::MULTIASSIGN;

    // set the time-to-live
    header->ttl = settings_.get_rebroadcast_ttl ();
  }

  header->updates = uint32_t (filtered_updates.size ());

  // compute size of this header
  header->size = header->encoded_size ();

  // keep track of the maximum buffer size for encoding
  int max_buffer_size = (int)buffer_remaining;

  // set the update to the end of the header
  char * update = header->write (buffer, buffer_remaining);
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
   * ttl = buffer[132] (the new knowledge starts here)
   * knowledge = buffer[133] (the new knowledge starts here)
  **/

  // zero out the memory
  //memset(buffer, 0, Madara::Transport::MAX_PACKET_SIZE);

  // Message update format
  // [key|value]
  
  int j = 0;
  for (Knowledge_Map::const_iterator i = filtered_updates.begin ();
    i != filtered_updates.end (); ++i, ++j)
  {
    update = i->second.write (update, i->first, buffer_remaining);
    
    if (buffer_remaining > 0)
    {
      context_.get_logger ().log (Logger::LOG_MINOR,
        "%s:" \
        " update[%d] => encoding %s of type %d and size %d\n",
        print_prefix,
        j, i->first.c_str (), i->second.type (), i->second.size ());
    }
    else
    {
      context_.get_logger ().log (Logger::LOG_EMERGENCY,
        "%s:" \
        " unable to encode update[%d] => %s of type %d and size %d\n",
        print_prefix,
        j, i->first.c_str (), i->second.type (), i->second.size ());
    }
  }
  
  long size (0);

  if (buffer_remaining > 0)
  {
    size = (long)(settings_.queue_length - buffer_remaining);
    *message_size = Madara::Utility::endian_swap ((uint64_t)size);
    
    // before we send to others, we first execute rules
    if (settings_.on_data_received_logic.length () != 0)
    {
#ifndef _MADARA_NO_KARL_

      context_.get_logger ().log (Logger::LOG_MAJOR,
        "%s:" \
        " evaluating rules in %s\n",
        print_prefix,
        settings_.on_data_received_logic.c_str ());
      
      on_data_received_.evaluate ();

      context_.get_logger ().log (Logger::LOG_MAJOR,
        "%s:" \
        " rules have been successfully evaluated\n",
        print_prefix);

#endif // _MADARA_NO_KARL_

    }
    else
    {
      context_.get_logger ().log (Logger::LOG_MINOR,
        "%s:" \
        " no permanent rules were set\n",
        print_prefix);
    }
  }

  context_.get_logger ().log (Logger::LOG_MAJOR,
    "%s:" \
    " calling encode filters\n",
    print_prefix);

  // buffer is ready encoding
  size = (long)settings_.filter_encode ((unsigned char *)buffer_.get_ptr (),
    (int)size, max_buffer_size);

  delete header;

  return size;
}
