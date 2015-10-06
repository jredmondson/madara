#ifndef _MADARA_UDP_TRANSPORT_READ_THREAD_H_
#define _MADARA_UDP_TRANSPORT_READ_THREAD_H_

#include <string>

#include "madara/utility/Scoped_Array.h"
#include "madara/knowledge/Thread_Safe_Context.h"
#include "madara/transport/Bandwidth_Monitor.h"
#include "madara/transport/QoS_Transport_Settings.h"
#include "madara/expression/Expression_Tree.h"
#include "madara/transport/Transport.h"
#include "madara/transport/Message_Header.h"
#include "madara/threads/Base_Thread.h"

#include "ace/Task.h"
#include "ace/Mutex.h"
#include "ace/Barrier.h"
#include "ace/Atomic_Op_T.h"
#include "ace/Thread_Mutex.h"

#include "ace/Synch.h"
#include "ace/INET_Addr.h"
#include "ace/SOCK_Dgram.h"

namespace madara
{
  namespace transport
  {
    /**
     * @class UDP_Transport_Read_Thread
     * @brief Thread for reading knowledge updates through a UDP socket
     **/
    class UDP_Transport_Read_Thread : public threads::Base_Thread
    {
    public:
      /**
       * Constructor
       * @param    settings   Transport settings
       * @param    id      host:port identifier of this process, to allow for 
       *                   rejection of duplicates
       * @param    addresses    the ACE socket addresses to communicate with 
       * @param    write_socket    socket for sending
       * @param    read_socket    socket for receiving
       * @param    send_monitor    bandwidth monitor for enforcing send limits
       * @param    receive_monitor    bandwidth monitor for enforcing
       *                              receive limits
       * @param    packet_scheduler scheduler for mimicking network conditions
       **/
      UDP_Transport_Read_Thread (
        const Settings & settings,
        const std::string & id,
        std::map <std::string, ACE_INET_Addr> & addresses,
        ACE_SOCK_Dgram & write_socket,
        ACE_SOCK_Dgram & read_socket,
        Bandwidth_Monitor & send_monitor,
        Bandwidth_Monitor & receive_monitor,
        Packet_Scheduler & packet_scheduler);
      
      /**
       * Initializes MADARA context-related items
       * @param   knowledge   context for querying current program state
       **/
      void init (knowledge::Knowledge_Base & knowledge);

      /**
       * Cleanup function called by thread manager
       **/
      void cleanup (void);

      /**
       * The main loop internals for the read thread
       **/
      void run (void);

      /**
       * Sends a rebroadcast packet to all peers.
       * @param  print_prefix     prefix to include before every log message,
       *                          e.g., "My_Transport::svc"
       * @param   header   header for the rebroadcasted packet
       * @param   records  records to rebroadcast (already filtered for
       *                   rebroadcast)
       **/
      void rebroadcast (
        const char * print_prefix,
        Message_Header * header,
        const Knowledge_Map & records);

    private:
      /// Transport settings
      const QoS_Transport_Settings settings_;

      /// host:port identifier of this process
      const std::string                                 id_;
      
      /// knowledge context
      knowledge::Thread_Safe_Context * context_;
      
      /// internet addresses of our peers
      std::map <std::string, ACE_INET_Addr> & addresses_;
      
      /// The socket we are writing to
      ACE_SOCK_Dgram       &             write_socket_;
      
      /// The socket we are reading from
      ACE_SOCK_Dgram       &             read_socket_;
      
#ifndef _MADARA_NO_KARL_
      /// data received rules, defined in Transport settings
      madara::knowledge::Compiled_Expression  on_data_received_;
#endif // _MADARA_NO_KARL_
      
      /// buffer for sending
      madara::utility::Scoped_Array <char>      buffer_;

      /// monitor for sending bandwidth usage
      Bandwidth_Monitor   &   send_monitor_;
      
      /// monitor for receiving bandwidth usage
      Bandwidth_Monitor   &   receive_monitor_;

      /// scheduler for mimicking target network conditions
      Packet_Scheduler    &   packet_scheduler_;
    };
  }
}

#endif // _MADARA_UDP_TRANSPORT_READ_THREAD_H_
