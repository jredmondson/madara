#ifndef SPLICE_DATAREADER_LISTENER_H
#define SPLICE_DATAREADER_LISTENER_H

#include <string>

#include "madara/knowledge/ThreadSafeContext.h"
#include "madara/transport/Transport.h"

#include "ccpp_dds_dcps.h"
#include "madara/transport/splice/ccpp_SpliceKnowledgeUpdate.h"

namespace madara
{
namespace transport
{
/**
 * @class SpliceDataReaderListener
 * @brief Container for DDS-related callbacks (deprecated and unused)
 **/
class SpliceDataReaderListener : public DDS::DataReaderListener
{
public:
  SpliceDataReaderListener(
      const std::string& id, knowledge::ThreadSafeContext& context);

  SpliceDataReaderListener(const SpliceDataReaderListener& ref);

  ~SpliceDataReaderListener();

  void handle_assignment(Knowledge::Update& data);
  void handle_multiassignment(Knowledge::Update& data);

public:
  /**
   * DDS callback for deadline being missed
   **/
  void on_requested_deadline_missed(
      DDS::DataReader_ptr, const DDS::RequestedDeadlineMissedStatus& status);

  /**
   * DDS callback for incompatible qos
   **/
  void on_requested_incompatible_qos(
      DDS::DataReader_ptr, const DDS::RequestedIncompatibleQosStatus& status);

  /**
   * DDS callback for sample rejected
   **/
  void on_sample_rejected(
      DDS::DataReader_ptr, const DDS::SampleRejectedStatus& status);

  /**
   * DDS callback for sample rejected
   **/
  void on_liveliness_changed(
      DDS::DataReader_ptr, const DDS::LivelinessChangedStatus& status);

  /**
   * DDS callback for sample lost
   **/
  void on_sample_lost(DDS::DataReader_ptr, const DDS::SampleLostStatus& status);

  /**
   * DDS callback for subscription matched
   **/
  void on_subscription_matched(
      DDS::DataReader_ptr reader, const DDS::SubscriptionMatchedStatus& status);

  void on_data_available(DDS::DataReader_ptr reader);

private:
  // NDDS variables
  const std::string id_;
  knowledge::ThreadSafeContext& context_;

};  // End of class NDDSListener
}
}

#endif  // SPLICE_DATAREADER_LISTENER_H
