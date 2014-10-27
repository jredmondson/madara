#ifndef SPLICE_DATAREADER_LISTENER_H
#define SPLICE_DATAREADER_LISTENER_H

#include <string>

#include "madara/knowledge_engine/Thread_Safe_Context.h"
#include "madara/transport/Transport.h"

#include "ccpp_dds_dcps.h"
#include "madara/transport/splice/ccpp_Splice_Knowledge_Update.h"

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
     * @class Splice_DataReader_Listener
     * @brief Container for DDS-related callbacks (deprecated and unused)
     **/
    class Splice_DataReader_Listener: public DDS::DataReaderListener
    {
    public:
      Splice_DataReader_Listener (const std::string & id,
        Madara::Knowledge_Engine::Thread_Safe_Context & context);

      Splice_DataReader_Listener (const Splice_DataReader_Listener &ref);

      ~Splice_DataReader_Listener();

      void handle_assignment (Knowledge::Update & data);
      void handle_multiassignment (Knowledge::Update & data);

    public:
      /**
       * DDS callback for deadline being missed
       **/
      void on_requested_deadline_missed (DDS::DataReader_ptr,
        const DDS::RequestedDeadlineMissedStatus & status);
      
      /**
       * DDS callback for incompatible qos
       **/
      void on_requested_incompatible_qos (DDS::DataReader_ptr,
        const DDS::RequestedIncompatibleQosStatus & status);
      
      /**
       * DDS callback for sample rejected
       **/
      void on_sample_rejected (DDS::DataReader_ptr,
        const DDS::SampleRejectedStatus & status);

      /**
       * DDS callback for sample rejected
       **/
      void on_liveliness_changed (DDS::DataReader_ptr,
        const DDS::LivelinessChangedStatus & status);

      /**
       * DDS callback for sample lost
       **/
      void on_sample_lost (DDS::DataReader_ptr,
        const DDS::SampleLostStatus & status);

      /**
       * DDS callback for subscription matched
       **/
      void on_subscription_matched (DDS::DataReader_ptr reader,
        const DDS::SubscriptionMatchedStatus & status);

      void on_data_available(DDS::DataReader_ptr reader);
    private:

      // NDDS variables
      const std::string id_;
      ::Madara::Knowledge_Engine::Thread_Safe_Context & context_;

    };  // End of class NDDS_Listener
  }
}

#endif // SPLICE_DATAREADER_LISTENER_H
