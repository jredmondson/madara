#ifndef _MADARA_ZEROMQ_TRANSPORT_H_
#define _MADARA_ZEROMQ_TRANSPORT_H_

/**
 * @file ZMQTransport.h
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains the ZMQTransport class, which provides a
 * multicast transport for sending knowledge updates in KaRL
 **/

#include <string>

#include "madara/MadaraExport.h"
#include "madara/utility/ScopedArray.h"
#include "madara/transport/QoSTransportSettings.h"
#include "madara/transport/Transport.h"
#include "madara/knowledge/KnowledgeBase.h"

#include <zmq.h>

#include "madara/knowledge/KnowledgeRecord.h"
#include "madara/expression/ExpressionTree.h"
#include "madara/transport/BandwidthMonitor.h"
#include "madara/threads/Threader.h"

namespace madara
{
  namespace transport
  {
    /**
     * @class ZMQTransport
     * @brief ZMQ-based transport for knowledge. This transport currently
     * supports the following transport settings:<br />
     *        1) a single host:port pairing<br />
     *        2) the reduced message header<br />
     *        3) the normal message header<br />
     *        4) domain differentiation<br />
     *        5) on data received logic<br />
     *        6) multi-assignment of records<br />
     *        7) rebroadcasting<br />
     **/
    class MADARA_EXPORT ZMQTransport : public Base
    {
    public:
      /**
       * Constructor
       * @param   id   unique identifer - usually a combination of host:port
       * @param   context  knowledge context
       * @param   config   transport configuration settings
       * @param   launch_transport  whether or not to launch this transport
       **/
      ZMQTransport (const std::string & id, 
        madara::knowledge::ThreadSafeContext & context, 
        TransportSettings & config, bool launch_transport);

      /**
       * Destructor
       **/
      virtual ~ZMQTransport ();
      
      /**
       * Sends a list of knowledge updates to listeners
       * @param   updates listing of all updates that must be sent
       * @return  result of write operation or -1 if we are shutting down
       **/
      long send_data (const madara::knowledge::VariableReferenceMap & updates) override;
    
      /**
       * Closes the transport
       **/
      void close (void);
      
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
       * Initializes the transport
       * @return  0 if success
       **/
      int setup (void);

    private:
      
      /// knowledge base for threads to use
      knowledge::KnowledgeBase         knowledge_;
      
      /// threads for reading knowledge updates
      threads::Threader                        read_threads_;

      /// underlying socket for sending
      void * write_socket_;
    };
  }
}

#endif // _MADARA_ZEROMQ_TRANSPORT_H_
