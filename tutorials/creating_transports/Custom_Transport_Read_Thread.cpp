#include "Custom_Transport_Read_Thread.h"
#include "madara/utility/Log_Macros.h"
#include "madara/transport/Message_Header.h"
#include "ace/Time_Value.h"
#include "madara/utility/Utility.h"
#include "madara/knowledge_engine/Thread_Safe_Context.h"
#include "madara/transport/Transport.h"

#include <iostream>
#include <algorithm>

Custom_Transport_Read_Thread::Custom_Transport_Read_Thread (
  const Madara::Transport::Settings & settings, const std::string & id,
  Madara::Knowledge_Engine::Thread_Safe_Context & context,
  const ACE_INET_Addr & address)
  : settings_ (settings), id_ (id), context_ (context),
    barrier_ (2), terminated_ (false), 
    mutex_ (), is_not_ready_ (mutex_), is_ready_ (false), address_ (address)
{
  // Subscribe
  int port = address_.get_port_number ();
  const char * host = address_.get_host_addr ();

  socket_.join (address_, 1);

  this->activate (THR_NEW_LWP | THR_DETACHED, 1);
}

Custom_Transport_Read_Thread::~Custom_Transport_Read_Thread ()
{
  socket_.leave (address_);
  socket_.close ();
}

int
Custom_Transport_Read_Thread::close (void)
{
  terminated_ = true;

  enter_barrier ();

  return 0;
}

void
Custom_Transport_Read_Thread::wait_for_ready (void)
{
  if (!is_ready_)
    is_not_ready_.wait ();
}

int Custom_Transport_Read_Thread::enter_barrier (void)
{
  barrier_.wait ();
  return 0;
}


int
Custom_Transport_Read_Thread::svc (void)
{
  ACE_Time_Value wait_time (1);
  ACE_INET_Addr  remote;
  
  // specify a 131k packet size limit. This is unlikely to ever be used
  char buffer [Madara::Transport::MAX_PACKET_SIZE];

  while (false == terminated_.value ())
  {
    // read the message
    ssize_t bytes_read = socket_.recv ((void *)buffer, 
      sizeof (buffer), remote, 0, &wait_time);
 
    if (bytes_read > 0)
    {
      int64_t buffer_remaining = (int64_t)bytes_read;
      Madara::Transport::Message_Header header;
      const char * update = header.read (buffer, buffer_remaining);

      // reject the message if it is not KaRL
      if (strncmp (header.madara_id, "KaRL", 4) != 0)
      {
        continue;
      }
    
      // reject the message if it is us as the originator (no update necessary)
      if (strncmp (header.originator, id_.c_str (),
           std::min (sizeof (header.originator), id_.size ())) == 0)
      {
        continue;
      }
      
      // reject the message if it is from a different domain
      if (strncmp (header.domain, settings_.domains.c_str (),
           std::min (sizeof (header.domain), settings_.domains.size ())) != 0)
      {
        continue;
      }
      
      // temporary record for reading from the updates buffer
      Madara::Knowledge_Record record;
      record.quality = header.quality;
      record.clock = header.clock;
      std::string key;

      // lock the context
      context_.lock ();

      // iterate over the updates
      for (uint32_t i = 0; i < header.updates; ++i)
      {
        // read converts everything into host format from the update stream
        update = record.read (update, key, buffer_remaining);
        
        if (buffer_remaining < 0)
        {
          break;
        }

        int result = record.apply (context_, key, header.quality,
          header.clock, false);
      }
      
      // unlock the context
      context_.unlock ();
      context_.set_changed ();
    }
  }
  
  barrier_.wait ();
  return 0;
}