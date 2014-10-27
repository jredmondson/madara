#ifndef _MULTICAST_TRANSPORT_READ_THREAD_H_
#define _MULTICAST_TRANSPORT_READ_THREAD_H_

/**
 * @file Multicast_Transport_Read_Thread.h
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains the Multicast_Transport_Read_Thread class, which provides a
 * multicast transport for reading knowledge updates in KaRL
 **/

#include <string>

#include "madara/utility/Scoped_Array.h"
#include "madara/knowledge_engine/Thread_Safe_Context.h"
#include "madara/transport/Bandwidth_Monitor.h"
#include "madara/transport/QoS_Transport_Settings.h"
#include "madara/expression_tree/Expression_Tree.h"
#include "madara/transport/Transport.h"
#include "madara/transport/Message_Header.h"

#include "ace/Task.h"
#include "ace/Mutex.h"
#include "ace/Barrier.h"
#include "ace/Atomic_Op_T.h"
#include "ace/Thread_Mutex.h"
#include "ace/SOCK_Dgram_Mcast.h"

#include "ace/Synch.h"
 
namespace Madara
{
  namespace Transport
  {
    /**
     * @class Multicast_Transport_Read_Thread
     * @brief Thread for reading knowledge updates through a Multicast
     *        datagram socket
     **/
    class Multicast_Transport_Read_Thread : public ACE_Task<ACE_NULL_SYNCH>
    {
    public:
      /**
       * Constructor
       * @param    settings   Transport settings
       * @param    id      host:port identifier of this process, to allow for 
       *                   rejection of duplicates
       * @param    context    the knowledge variables to update
       * @param    address    the multicast address we will read from
       * @param    socket     socket for sending
       * @param    send_monitor    bandwidth monitor for enforcing send limits
       * @param    receive_monitor    bandwidth monitor for enforcing
       *                              receive limits
       * @param    packet_scheduler scheduler for mimicking network conditions
       **/
      Multicast_Transport_Read_Thread (
        const Settings & settings,
        const std::string & id,
        Knowledge_Engine::Thread_Safe_Context & context,
        const ACE_INET_Addr & address,
        ACE_SOCK_Dgram & socket,
        Bandwidth_Monitor & send_monitor,
        Bandwidth_Monitor & receive_monitor,
        Packet_Scheduler & packet_scheduler);
      
      /**
      * Destructor
      **/
      ~Multicast_Transport_Read_Thread ();
      
      /**
      * Signals the read thread to terminate
      **/
      int enter_barrier (void);
      
      /**
      * Closes the reading socket and clean up the thread
      **/
      int close (void);

      /**
      * Reads messages from a socket
      **/
      int svc (void);
      
      /**
       * Sends a rebroadcast packet.
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

      /**
      * Wait for the transport to be ready
      **/
      void wait_for_ready (void);

    private:
      /// quality-of-service transport settings
      const Settings  &          settings_;

      /// host:port identifier of this process
      const std::string                                 id_;

      /// knowledge context
      ::Madara::Knowledge_Engine::Thread_Safe_Context & context_;
      
      /// barrier for closing and waiting on the read thread
      ACE_Barrier barrier_;

      /// atomic variable that signals termination
      ACE_Atomic_Op<ACE_Mutex, bool>     terminated_;
      
      /// Mutex for use with terminated_
      ACE_Thread_Mutex                   mutex_;
      
      /// Condition that is waited on my calling transport on startup
      Madara::Transport::Condition       is_not_ready_;

      /// Indicates whether the read thread is ready to accept messages
      bool                               is_ready_;

      /// The multicast address we are subscribing to
      ACE_INET_Addr                      address_;

      /// The multicast socket we are reading from
      ACE_SOCK_Dgram_Mcast               read_socket_;
      
      /// underlying socket for sending
      ACE_SOCK_Dgram      &              write_socket_;

      /// data received rules, defined in Transport settings
      Madara::Knowledge_Engine::Compiled_Expression  on_data_received_;

      /// buffer for receiving
      Madara::Utility::Scoped_Array <char>      buffer_;

      /// pointer to qos_settings (if applicable)
      const QoS_Transport_Settings *      qos_settings_;
      
      /// monitor for sending bandwidth usage
      Bandwidth_Monitor   &   send_monitor_;
      
      /// monitor for receiving bandwidth usage
      Bandwidth_Monitor   &   receive_monitor_;

      /// scheduler for mimicking target network conditions
      Packet_Scheduler    &   packet_scheduler_;
    };
  }
}

#endif // _MULTICAST_TRANSPORT_READ_THREAD_H_
