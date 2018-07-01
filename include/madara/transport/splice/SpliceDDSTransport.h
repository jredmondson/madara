#ifndef _MADARA_SPLICE_DDS_TRANSPORT_H__
#define _MADARA_SPLICE_DDS_TRANSPORT_H__

#include <string>

#include "madara/transport/splice/SpliceTransportReadThread.h"
#include "madara/knowledge/ThreadSafeContext.h"
#include "madara/transport/Transport.h"
#include "ccpp_dds_dcps.h"
#include "madara/transport/splice/ccpp_SpliceKnowledgeUpdate.h"
#include "madara/threads/Threader.h"

namespace madara
{
  namespace transport
  {
    /**
     * @class SpliceDDSTransport
     * @brief This class provides an interface into the Open Splice
     *        dissemination transport
     */
    class SpliceDDSTransport : public Base
    {
    public:

      enum {
        ERROR_OSPL_NOT_STARTED = -1,
        ERROR_SUB_BAD_CREATE = -2,
        ERROR_PUB_BAD_CREATE = -3
      };

      static const int PROFILES = 2;

      /**
       * Constructor
       * @param   id   unique identifer - usually a combination of host:port
       * @param   context  knowledge context
       * @param   config   transport configuration settings
       * @param   launch_transport  whether or not to launch this transport
       **/
      SpliceDDSTransport (const std::string & id, 
        knowledge::ThreadSafeContext & context, 
        TransportSettings & config, bool launch_transport);

      /**
       * Destructor
       **/
      ~SpliceDDSTransport ();
      
      /**
       * Sends a list of knowledge updates to listeners
       * @param   updates listing of all updates that must be sent
       * @return  result of write operation or -1 if we are shutting down
       **/
      long send_data (const knowledge::VariableReferenceMap & updates) override;

      /**
       * Accesses reliability setting
       * @return  whether we are using reliable dissemination or not
       **/
      int reliability (void) const;
      
      /**
       * Sets the reliability setting
       * @return  the changed setting
       **/
      int reliability (const int & setting);

      /**
       * Closes this transport
       **/
      void close (void);

      /**
       * Activates this transport
       * @return  0 for success. This function may exit the process
       *          if the OSPL daemon hasn't been started.
       **/
      int setup (void);
    protected:
    private:

      /// knowledge base for threads to use
      knowledge::KnowledgeBase          knowledge_;

      /// threads for reading knowledge updates
      threads::Threader                 read_threads_;

      const static char *               topic_names_[];
      const static char *               partition_;
      const static char *               ret_code_names[];

      DDS::DomainParticipantQos          part_qos_;
      DDS::TopicQos                      topic_qos_;
      DDS::PublisherQos                  pub_qos_;
      DDS::DataWriterQos                 datawriter_qos_;
      DDS::SubscriberQos                 sub_qos_;
      DDS::DataReaderQos                 datareader_qos_;

      DDS::DomainId_t                    domain_;
      DDS::DomainParticipantFactory_var  domain_factory_;
      DDS::DomainParticipant_var         domain_participant_;
      DDS::Publisher_var                 publisher_;
      DDS::Subscriber_var                subscriber_;
      DDS::DataWriter_var                datawriter_;
      DDS::DataReader_var                datareader_;
      DDS::DataWriter_var                latencywriter_;

      Knowledge::UpdateDataWriter_var    update_writer_;
      Knowledge::UpdateDataWriter_var    latency_update_writer_;
      Knowledge::UpdateDataReader_var    update_reader_;
      Knowledge::UpdateTypeSupport       update_type_support_;

      DDS::Topic_var                     update_topic_;

      /**
       * Splice handle checker
       **/
      int check_handle (void * handle, const char *info);

      /**
       * Splice status checker
       **/
      int check_status (DDS::ReturnCode_t status, const char * info);
      
      /**
       * Returns error name of the specific status
       **/
      const char * get_error_name (DDS::ReturnCode_t status);
    };
  }
}

#endif
