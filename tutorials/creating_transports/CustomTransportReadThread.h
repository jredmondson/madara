#ifndef _CUSTOM_TRANSPORT_READ_THREAD_H_
#define _CUSTOM_TRANSPORT_READ_THREAD_H_

/**
 * @file CustomTransportReadThread.h
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains the CustomTransportReadThread class, which provides a
 * multicast transport for reading knowledge updates in KaRL
 **/

#include <string>

#include "madara/knowledge_engine/ThreadSafeContext.h"
#include "madara/transport/Transport.h"
#include "madara/expression_tree/ExpressionTree.h"

#include "ace/Task.h"
#include "ace/Mutex.h"
#include "ace/Barrier.h"
#include "ace/Atomic_Op_T.h"
#include "ace/Thread_Mutex.h"
#include "ace/SOCK_Dgram_Mcast.h"

#include "ace/Synch.h"

/**
  * @class CustomTransportReadThread
  * @brief Thread for reading knowledge updates through a Multicast
  *        datagram socket
  **/
class CustomTransportReadThread : public ACE_Task<ACE_MT_SYNCH>
{
public:
  /**
    * Constructor
    * @param    settings   Transport settings
    * @param    id      host:port identifier of this process, to allow for 
    *                   rejection of duplicates
    * @param    context    the knowledge variables to update
    * @param    address    the multicast address we will read from
    **/
  CustomTransportReadThread (
    const Madara::Transport::Settings & settings,
    const std::string & id,
    Madara::KnowledgeEngine::ThreadSafeContext & context,
    const ACE_INET_Addr & address);
      
  /**
  * Destructor
  **/
  ~CustomTransportReadThread ();
      
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
  * Wait for the transport to be ready
  **/
  void wait_for_ready (void);
private:
  /// Transport settings
  const Madara::Transport::Settings & settings_;

  /// host:port identifier of this process
  const std::string                                 id_;

  /// knowledge context
  ::Madara::KnowledgeEngine::ThreadSafeContext & context_;
      
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
  ACE_SOCK_Dgram_Mcast               socket_;
};

#endif // _CUSTOM_TRANSPORT_READ_THREAD_H_
