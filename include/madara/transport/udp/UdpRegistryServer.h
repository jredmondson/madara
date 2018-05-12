#ifndef _MADARA_UDP_REGISTRY_SERVER_H_
#define _MADARA_UDP_REGISTRY_SERVER_H_

#include <string>

#include "madara/MADARA_export.h"
#include "madara/transport/udp/UdpRegistryServerReadThread.h"
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
     * @class UdpRegistryServer
     * @brief UDP-based server that handles a registry of UDP endpoints,
     *        which makes it ideal for any NAT-protected agents
     **/
    class MADARA_Export UdpRegistryServer : public Base
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
      UdpRegistryServer (const std::string & id,
        madara::knowledge::ThreadSafeContext & context, 
        TransportSettings & config, bool launch_transport);

      /**
       * Destructor
       **/
      virtual ~UdpRegistryServer ();

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

      /// underlying socket for sending
      ACE_SOCK_Dgram                            write_socket_;

      /// The socket we are reading from
      ACE_SOCK_Dgram                            read_socket_;
    };
  }
}

#endif // _MADARA_UDP_REGISTRY_SERVER_H_
