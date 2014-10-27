#include "madara/transport/tcp/TCP_Transport_Read_Thread.h"
#include "madara/utility/Log_Macros.h"

#include <iostream>

Madara::Transport::TCP_Transport_Read_Thread::TCP_Transport_Read_Thread (
  const std::string & id,
  Madara::Knowledge_Engine::Thread_Safe_Context & context)
  : id_ (id), context_ (context),
    barrier_ (2), terminated_ (false), 
    mutex_ (), is_not_ready_ (mutex_), is_ready_ (false)
{
  this->activate (THR_NEW_LWP | THR_DETACHED, 1);

  MADARA_DEBUG (MADARA_LOG_MAJOR_EVENT, (LM_DEBUG, 
    DLINFO "TCP_Transport_Read_Thread::TCP_Transport_Read_Thread:" \
    " read thread started\n"));
}

Madara::Transport::TCP_Transport_Read_Thread::~TCP_Transport_Read_Thread ()
{
}

int
Madara::Transport::TCP_Transport_Read_Thread::close (void)
{
  terminated_ = true;
  enter_barrier ();

  return 0;
}

void
Madara::Transport::TCP_Transport_Read_Thread::wait_for_ready (void)
{
  if (!is_ready_)
    is_not_ready_.wait ();
}

int Madara::Transport::TCP_Transport_Read_Thread::enter_barrier (void)
{
  MADARA_DEBUG (MADARA_LOG_MAJOR_EVENT, (LM_DEBUG, 
    DLINFO "TCP_Transport_Read_Thread::enter_barrier:" \
    " beginning thread barrier for shut down of read thread\n"));

  barrier_.wait ();
  return 0;
}


int
Madara::Transport::TCP_Transport_Read_Thread::svc (void)
{
  while (false == terminated_.value ())
  {
 
  }

  enter_barrier ();
  return 0;
}