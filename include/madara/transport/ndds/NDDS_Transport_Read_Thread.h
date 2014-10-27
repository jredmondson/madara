#ifndef _NDDS_TRANSPORT_READ_THREAD_H_
#define _NDDS_TRANSPORT_READ_THREAD_H_

#include <string>

#include "madara/knowledge_engine/Thread_Safe_Context.h"
#include "madara/transport/Transport.h"

#include <ndds/ndds_cpp.h>
#include "madara/transport/ndds/Ndds_Knowledge_Update.h"
#include "madara/transport/ndds/Ndds_Knowledge_UpdateSupport.h"

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
     * @class NDDS_Read_Thread
     * @brief Thread for reading knowledge updates via a NDDS waitset
     *        (deprecated and unused).
     **/
    class NDDS_Read_Thread : public ACE_Task<ACE_MT_SYNCH>
    {
    public:
      NDDS_Read_Thread (
        const std::string & id,
        Madara::Knowledge_Engine::Thread_Safe_Context & context,
            NDDS_Knowledge_UpdateDataReader * reader);
      ~NDDS_Read_Thread ();

      /// service exit point for thread
      int enter_barrier (void);
      int close (void);
      int svc (void);
      void wait_for_ready (void);
    private:
      void handle_assignment (NDDS_Knowledge_Update & data);
      void handle_multiassignment (NDDS_Knowledge_Update & data);

      ::std::vector< ::std::string>                        assignment_symbols_;

      const std::string                                    id_;
      ::Madara::Knowledge_Engine::Thread_Safe_Context &    context_;
      /// typdef for a threadsafe counter
      ACE_Barrier barrier_;
      ACE_Atomic_Op<ACE_Mutex,bool>                        terminated_;

      ACE_Thread_Mutex                   mutex_;
      Madara::Transport::Condition       is_not_ready_;
      bool                               is_ready_;
      bool                               enable_mutexing_;

      // NDDS variables
      NDDS_Knowledge_UpdateDataReader * data_reader_;
    };
  }
}

#endif
