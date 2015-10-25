#ifndef _CUSTOM_TRANSPORT_H_
#define _CUSTOM_TRANSPORT_H_

/**
 * @file CustomTransport.h
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains the CustomTransport class, which provides a
 * multicast transport for sending knowledge updates in KaRL
 **/

#include <string>

#include "madara/transport/Transport.h"
#include "CustomTransportReadThread.h"
#include "madara/knowledge/ThreadSafeContext.h"
#include "ace/SOCK_Dgram.h"
#include "madara/knowledge/KnowledgeRecord.h"
#include "madara/expression/ExpressionTree.h"

/**
  * @class CustomTransport
  * @brief Multicast-based transport for knowledge
  **/
class CustomTransport : public madara::transport::Base
{
public:
  /**
    * Constructor
    * @param   id   unique identifer - usually a combination of host:port
    * @param   context  knowledge context
    * @param   config   transport configuration settings
    * @param   launch_transport  whether or not to launch this transport
    **/
  CustomTransport (const std::string & id, 
    madara::knowledge::ThreadSafeContext & context, 
    madara::transport::TransportSettings & config, bool launch_transport);

  /**
    * Destructor
    **/
  virtual ~CustomTransport ();
      
  /**
    * Sends a list of knowledge updates to listeners
    * @param   updates listing of all updates that must be sent
    * @return  result of write operation or -1 if we are shutting down
    **/
  long send_data (const madara::knowledge::KnowledgeRecords & updates);
	  
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
      
  /// host:port identifier of this process
  const std::string                         id_;

  /// thread for reading knowledge updates
  CustomTransportReadThread *            thread_;
      
  /// indicates whether the transport is correctly configured
  bool                                      valid_setup_;

  /// holds all IP addresses we are sending to
  std::vector <ACE_INET_Addr>               addresses_;

  /// underlying socket for sending
  ACE_SOCK_Dgram                            socket_;
};

#endif // _CUSTOM_TRANSPORT_H_
