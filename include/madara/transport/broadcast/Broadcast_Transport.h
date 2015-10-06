#ifndef _MADARA_BROADCAST_TRANSPORT_H_
#define _MADARA_BROADCAST_TRANSPORT_H_

/**
 * @file Broadcast_Transport.h
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains the Broadcast_Transport class, which provides a
 * broadcast transport for sending knowledge updates in KaRL
 **/

#include <string>

#include "madara/MADARA_export.h"
#include "madara/utility/Scoped_Array.h"
#include "madara/transport/broadcast/Broadcast_Transport_Read_Thread.h"
#include "madara/knowledge/Thread_Safe_Context.h"
#include "madara/transport/Bandwidth_Monitor.h"
#include "ace/SOCK_Dgram_Bcast.h"
#include "madara/utility/stdint.h"
#include "madara/expression/Expression_Tree.h"
#include "madara/threads/Threader.h"


namespace madara
{
  namespace transport
  {
    /**
     * @class Broadcast_Transport
     * @brief Multicast-based transport for knowledge. This transport currently
     * supports the following transport settings:<br />
     *        1) a single host:port pairing<br />
     *        2) the reduced message header<br />
     *        3) the normal message header<br />
     *        4) domain differentiation<br />
     *        5) on data received logic<br />
     *        6) multi-assignment of records<br />
     *        7) rebroadcasting<br />
     **/
    class MADARA_Export Broadcast_Transport : public Base
    {
    public:
      /**
       * Constructor
       * @param   id   unique identifer - usually a combination of host:port
       * @param   context  knowledge context
       * @param   config   transport configuration settings
       * @param   launch_transport  whether or not to launch this transport
       **/
      Broadcast_Transport (const std::string & id, 
        madara::knowledge::Thread_Safe_Context & context, 
        Settings & config, bool launch_transport);

      /**
       * Destructor
       **/
      virtual ~Broadcast_Transport ();

      /**
       * Sends a list of knowledge updates to listeners
       * @param   updates listing of all updates that must be sent
       * @return  result of write operation or -1 if we are shutting down
       **/
      long send_data (const madara::Knowledge_Records & updates);
      
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
      knowledge::Knowledge_Base         knowledge_;
      
      /// threads for reading knowledge updates
      threads::Threader                        read_threads_;

      /// holds all multicast addresses we are sending to
      std::vector <ACE_INET_Addr>               addresses_;

      /// holds splitters for knowledge multiassignment expression for speed
      std::vector <std::string>                 splitters_;

      /// underlying socket for sending
      ACE_SOCK_Dgram_Bcast                      socket_;
    };
  }
}

#endif // _MADARA_BROADCAST_TRANSPORT_H_