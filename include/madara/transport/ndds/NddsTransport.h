#ifndef _NDDS_TRANSPORT_H_
#define _NDDS_TRANSPORT_H_

#include <string>

#include "madara/knowledge/ThreadSafeContext.h"
#include "madara/transport/Transport.h"
#include <ndds/ndds_cpp.h>
#include "madara/transport/ndds/Ndds_Knowledge_Update.h"
#include "madara/transport/ndds/Ndds_Knowledge_UpdateSupport.h"
#include "madara/transport/ndds/NddsListener.h"

namespace madara
{
namespace transport
{
/**
 * @class NddsTransport
 * @brief This class provides an interface into the NDDS
 *        dissemination transport
 */
class NddsTransport : public Base
{
public:
  /**
   * Constructor
   * @param   id   unique identifer - usually a combination of host:port
   * @param   context  knowledge context
   * @param   config   transport configuration settings
   * @param   launch_transport  whether or not to launch this transport
   **/
  NddsTransport(const std::string& id, knowledge::ThreadSafeContext& context,
      TransportSettings& config, bool launch_transport);

  /**
   * Destructor
   **/
  ~NddsTransport();

  /**
   * Sends a list of knowledge updates to listeners
   * @param   updates listing of all updates that must be sent
   * @return  result of write operation or -1 if we are shutting down
   **/
  long send_data(const knowledge::VariableReferenceMap& updates) override;

  /**
   * Accesses reliability setting
   * @return  whether we are using reliable dissemination or not
   **/
  int reliability(void) const;

  /**
   * Sets the reliability setting
   * @return  the changed setting
   **/
  int reliability(const int& setting);

  /**
   * Closes this transport
   **/
  void close(void);

  /**
   * Activates this transport
   * @return  0 for success. This function may exit the process
   *          if the OSPL daemon hasn't been started.
   **/
  int setup(void);

protected:
private:
  const static char* topic_names_[];
  const static char* partition_;

  // NDDS variables
  DDSDomainParticipant* domain_participant_;
  DDSPublisher* publisher_;
  DDSSubscriber* subscriber_;
  DDSTopic* update_topic_;
  DDSDataReader* data_reader_;
  DDSDataWriter* data_writer_;
  Ndds_Knowledge_UpdateDataWriter* update_writer_;
  NddsListener* listener_;
};
}
}

#endif
