#ifndef _MADARA_UDP_REGISTRY_CLIENT_TRANSPORT_H_
#define _MADARA_UDP_REGISTRY_CLIENT_TRANSPORT_H_

#include <string>

#include "madara/MADARA_export.h"
#include "madara/transport/udp/UdpRegistryClientReadThread.h"
#include "madara/transport/Transport.h"
#include "madara/threads/Threader.h"
#include "madara/knowledge/containers/Map.h"

#include <string>
#include <map>

#include "ace/INET_Addr.h"
#include "ace/SOCK_Dgram.h"


namespace madara
{
  namespace transport
  {
    /**
     * @class UdpRegistryClient
     * @brief UDP-based transport for knowledge. This transport currently
     * supports the following transport settings:<br />
     *        1) multiple host:port pairing with self first in vector<br />
     *        2) the reduced message header<br />
     *        3) the normal message header<br />
     *        4) domain differentiation<br />
     *        5) on data received logic<br />
     *        6) multi-assignment of records<br />
     *        7) rebroadcasting<br />
     **/
    class MADARA_Export UdpRegistryClient : public Base
    {
    public:
  
      enum {
        RELIABLE = 0
      };

      enum {
        ERROR_UDP_NOT_STARTED = -1,
      };

      static const int PROFILES = 1;

      /**
       * Constructor
       * @param   id   unique identifer - usually a combination of host:port
       * @param   context  knowledge context
       * @param   config   transport configuration settings
       * @param   launch_transport  whether or not to launch this transport
       **/
      UdpRegistryClient (const std::string & id, 
        madara::knowledge::ThreadSafeContext & context, 
        TransportSettings & config, bool launch_transport);

      /**
       * Destructor
       **/
      virtual ~UdpRegistryClient ();


      /**
      * Sends register messages to all servers
      **/
      void send_register (void);

      /**
       * Sends a list of knowledge updates to listeners
       * @param   updates listing of all updates that must be sent
       * @return  result of write operation or -1 if we are shutting down
       **/
      long send_data (const madara::knowledge::VariableReferenceMap & updates) override;
    
      /**
       * Accesses reliability setting. If this returns zero, it doesn't
       * make much sense.
       * @return  whether we are using reliable dissemination or not
       **/
      int reliability (void) const;

      /**
       * Accesses reliability setting. If this returns zero, it doesn't
       * make much sense.
       * @return  whether we are using reliable dissemination or not
       **/
      int reliability (const int & setting);
      long read (void);
      void close (void) override;
      int setup (void) override;
    protected:
    private:
      /// knowledge base for threads to use
      knowledge::KnowledgeBase          knowledge_;
      
      /// threads for reading knowledge updates
      threads::Threader                         read_threads_;
      
      /// registry servers
      std::map <std::string, ACE_INET_Addr>     servers_;

      /// registry clients
      std::map <std::string, ACE_INET_Addr>     clients_;

      knowledge::containers::Map   endpoints_;

      /// underlying socket for sending and receiving
      ACE_SOCK_Dgram                            socket_;
    };
  }
}

#endif // _MADARA_UDP_REGISTRY_CLIENT_TRANSPORT_H_
