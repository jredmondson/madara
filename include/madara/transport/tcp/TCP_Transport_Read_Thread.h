#ifndef TCP_TRANSPORT_READ_THREAD_H
#define TCP_TRANSPORT_READ_THREAD_H

#include <string>

#include "madara/knowledge_engine/Thread_Safe_Context.h"
#include "madara/transport/Transport.h"

#include "ace/Task.h"
#include "ace/Mutex.h"
#include "ace/Barrier.h"
#include "ace/Atomic_Op_T.h"
#include "ace/Thread_Mutex.h"

#include "ace/Synch.h"

namespace Madara
{
  namespace Transport
  {
    /**
     * @class TCP_Transport_Read_Thread
     * @brief Thread for reading knowledge updates through a TCP socket
     *        (unused but planned)
     **/
    class TCP_Transport_Read_Thread : public ACE_Task<ACE_MT_SYNCH>
    {
    public:
      TCP_Transport_Read_Thread (
        const std::string & id,
        Madara::Knowledge_Engine::Thread_Safe_Context & context);
      ~TCP_Transport_Read_Thread ();

      /// service exit point for thread
      int enter_barrier (void);
      int close (void);
      int svc (void);
      void wait_for_ready (void);
    private:
      const std::string                                 id_;
      ::Madara::Knowledge_Engine::Thread_Safe_Context & context_;
      ACE_Barrier barrier_;
      ACE_Atomic_Op<ACE_Mutex,bool> terminated_;

      ACE_Thread_Mutex                   mutex_;
      Madara::Transport::Condition       is_not_ready_;
      bool                               is_ready_;
    };
  }
}

#endif // TCP_TRANSPORT_READ_THREAD_H
