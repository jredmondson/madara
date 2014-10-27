#include "madara/transport/tcp/TCP_Transport.h"
#include "madara/transport/tcp/TCP_Transport_Read_Thread.h"
#include "madara/utility/Log_Macros.h"

#include <iostream>

Madara::Transport::TCP_Transport::TCP_Transport (const std::string & id,
        Madara::Knowledge_Engine::Thread_Safe_Context & context, 
        Settings & config, bool launch_transport)
: Base (id, config, context),
  thread_ (0), valid_setup_ (false)
{
  this->validate_transport ();
}

Madara::Transport::TCP_Transport::~TCP_Transport ()
{
  close ();
}

void
Madara::Transport::TCP_Transport::close (void)
{
  this->invalidate_transport ();

  if (thread_)
  {
    thread_->close ();
    delete thread_;
  }

  this->shutting_down_ = false;
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
  thread_ = new Madara::Transport::TCP_Transport_Read_Thread (id_, context_);
  
  return this->validate_transport ();
}

long
Madara::Transport::TCP_Transport::send_data (
  const Madara::Knowledge_Records & updates)
{
  // check to see if we are shutting down
  long ret = this->check_transport ();
  if (-1 == ret)
  {
    MADARA_DEBUG (MADARA_LOG_MAJOR_EVENT, (LM_DEBUG, 
      DLINFO "TCP_Transport::send_data: transport has been told to shutdown")); 
    return ret;
  }
  else if (-2 == ret)
  {
    MADARA_DEBUG (MADARA_LOG_MAJOR_EVENT, (LM_DEBUG, 
      DLINFO "TCP_Transport::send_data: transport is not valid")); 
    return ret;
  }
  
  uint32_t quality = Madara::max_quality (updates);

  return 0;
}
