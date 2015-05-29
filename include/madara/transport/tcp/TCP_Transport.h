#ifndef TCP_TRANSPORT_H
#define TCP_TRANSPORT_H

#include <string>

#include "madara/transport/tcp/TCP_Transport_Read_Thread.h"
#include "madara/knowledge_engine/Thread_Safe_Context.h"
#include "madara/transport/Transport.h"

namespace Madara
{
  namespace Transport
  {
    /**
     * @class TCP_Transport
     * @brief TCP-based transport (skeleton code)
     **/
    class TCP_Transport : public Base
    {
    public:
  
      enum {
        RELIABLE = 0
      };

      enum {
        ERROR_TCP_NOT_STARTED = -1,
      };

      static const int PROFILES = 1;

      /**
       * Constructor
       * @param   id   unique identifer - usually a combination of host:port
       * @param   context  knowledge context
       * @param   config   transport configuration settings
       * @param   launch_transport  whether or not to launch this transport
       **/
      TCP_Transport (const std::string & id, 
        Madara::Knowledge_Engine::Thread_Safe_Context & context, 
        Settings & config, bool launch_transport);

      /**
       * Destructor
       **/
      virtual ~TCP_Transport ();
      
      /**
       * Sends a list of knowledge updates to listeners
       * @param   updates listing of all updates that must be sent
       * @return  result of write operation or -1 if we are shutting down
       **/
      long send_data (const Madara::Knowledge_Records & updates);
    
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
      void close (void);
      int setup (void);
    protected:
    private:
      const std::string                               id_;

      TCP_Transport_Read_Thread *               thread_;
    };
  }
}

#endif // TCP_TRANSPORT_H