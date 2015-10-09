#ifndef _NDDS_TRANSPORT_READ_THREAD_H_
#define _NDDS_TRANSPORT_READ_THREAD_H_

#include <string>

#include "madara/knowledge/ThreadSafeContext.h"
#include "madara/transport/Transport.h"

#include <ndds/ndds_cpp.h>
#include "madara/transport/ndds/NddsKnowledgeUpdate.h"
#include "madara/transport/ndds/NddsKnowledgeUpdateSupport.h"

#include "ace/Task.h"
#include "ace/Mutex.h"
#include "ace/Barrier.h"
#include "ace/Atomic_Op_T.h"
#include "ace/Thread_Mutex.h"

#include "ace/Synch.h"

namespace madara
{
  namespace transport
  {
    /**
     * @class NDDSReadThread
     * @brief Thread for reading knowledge updates via a NDDS waitset
     *        (deprecated and unused).
     **/
    class NDDSReadThread : public ACE_Task<ACE_MT_SYNCH>
    {
    public:
      NDDSReadThread (
        const std::string & id,
        knowledge::ThreadSafeContext & context,
            NDDSKnowledgeUpdateDataReader * reader);
      ~NDDSReadThread ();

      /// service exit point for thread
      int enter_barrier (void);
      int close (void);
      int svc (void);
      void wait_for_ready (void);
    private:
      void handle_assignment (NDDSKnowledgeUpdate & data);
      void handle_multiassignment (NDDSKnowledgeUpdate & data);

      ::std::vector< ::std::string>                        assignment_symbols_;

      const std::string                                    id_;
      knowledge::ThreadSafeContext &    context_;
      /// typdef for a threadsafe counter
      ACE_Barrier barrier_;
      ACE_Atomic_Op<ACE_Mutex,bool>                        terminated_;

      ACE_Thread_Mutex                   mutex_;
      transport::Condition               is_not_ready_;
      bool                               is_ready_;
      bool                               enable_mutexing_;

      // NDDS variables
      NDDSKnowledgeUpdateDataReader * data_reader_;
    };
  }
}

#endif
