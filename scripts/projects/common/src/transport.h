
#ifndef   _TRANSPORT_MYTRANSPORT_H_
#define   _TRANSPORT_MYTRANSPORT_H_

#include "madara/transport/Transport.h"
#include "madara/threads/Threader.h"
#include "MyTransportReadThread.h"

namespace transports
{
  /**
   * Custom network transport generated by gpc.pl
   **/
  class MyTransport : public madara::transport::Base
  {
  public:
    /**
     * Constructor
     * @param   id                unique identifier(generally host:port)
     * @param   new_settings      settings to apply to the transport
     * @param   context           the knowledge record context
     **/
    MyTransport(const std::string & id,
      madara::transport::TransportSettings & new_settings,
      madara::knowledge::KnowledgeBase & context);

    /**
     * Destructor
     **/
    virtual ~MyTransport();
    
    /**
     * Sends a list of updates to the domain. This function must be
     * implemented by your transport
     * @param  modifieds  a list of keys to values of all records that have
     *          been updated and could be sent.
     * @return  result of operation or -1 if we are shutting down
     **/
    long send_data(const madara::knowledge::KnowledgeMap & modifieds) override;
    
  protected:
    /// threads for monitoring knowledge updates
    madara::threads::Threader read_threads_;
  };
}

#endif // _TRANSPORT_MYTRANSPORT_H_