#include "madara/transport/splice/SpliceSubscriberListener.h"
#include "madara/utility/LogMacros.h"
#include "madara/knowledge/UpdateTypes.h"
#include "madara/utility/Utility.h"

#include <iostream>
#include <sstream>

madara::transport::SpliceSubscriberListener::SpliceSubscriberListener(
  const std::string & id,
  madara::knowledge::ThreadSafeContext & context)
: id_ (id), context_ (context)
{
}

madara::transport::SpliceSubscriberListener::SpliceSubscriberListener(
  const SpliceSubscriberListener &ref)
: id_ (ref.id_), context_ (ref.context_)
{
}

madara::transport::SpliceSubscriberListener::~SpliceSubscriberListener()
{}

void
madara::transport::SpliceSubscriberListener::on_sample_lost (
  DDS::DataReader_ptr, const DDS::SampleLostStatus &status)
{
}

void
madara::transport::SpliceSubscriberListener::on_sample_rejected (
  DDS::DataReader_ptr, const DDS::SampleRejectedStatus &status)
{
}

void
madara::transport::SpliceSubscriberListener::on_requested_incompatible_qos (
  DDS::DataReader_ptr, const DDS::RequestedIncompatibleQosStatus &status)
{
}

void
madara::transport::SpliceSubscriberListener::on_requested_deadline_missed (
  DDS::DataReader_ptr reader, const DDS::RequestedDeadlineMissedStatus & status)
{
}

void
madara::transport::SpliceSubscriberListener::on_liveliness_changed (
  DDS::DataReader_ptr reader, const DDS::LivelinessChangedStatus & status)
{
}

void
madara::transport::SpliceSubscriberListener::on_subscription_matched (
  DDS::DataReader_ptr reader, const DDS::SubscriptionMatchedStatus & status)
{
}

void 
madara::transport::SpliceSubscriberListener::on_data_available(
  DDS::DataReader_ptr reader)
{
}


void 
madara::transport::SpliceSubscriberListener::on_data_on_readers(
  DDS::Subscriber_ptr sub)
{
  sub->notify_datareaders ();
}