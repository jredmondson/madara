#include "madara/transport/splice/Splice_Subscriber_Listener.h"
#include "madara/utility/Log_Macros.h"
#include "madara/knowledge/Update_Types.h"
#include "madara/utility/Utility.h"

#include <iostream>
#include <sstream>

madara::transport::Splice_Subscriber_Listener::Splice_Subscriber_Listener(
  const std::string & id,
  madara::knowledge::Thread_Safe_Context & context)
: id_ (id), context_ (context)
{
}

madara::transport::Splice_Subscriber_Listener::Splice_Subscriber_Listener(
  const Splice_Subscriber_Listener &ref)
: id_ (ref.id_), context_ (ref.context_)
{
}

madara::transport::Splice_Subscriber_Listener::~Splice_Subscriber_Listener()
{}

void
madara::transport::Splice_Subscriber_Listener::on_sample_lost (
  DDS::DataReader_ptr, const DDS::SampleLostStatus &status)
{
}

void
madara::transport::Splice_Subscriber_Listener::on_sample_rejected (
  DDS::DataReader_ptr, const DDS::SampleRejectedStatus &status)
{
}

void
madara::transport::Splice_Subscriber_Listener::on_requested_incompatible_qos (
  DDS::DataReader_ptr, const DDS::RequestedIncompatibleQosStatus &status)
{
}

void
madara::transport::Splice_Subscriber_Listener::on_requested_deadline_missed (
  DDS::DataReader_ptr reader, const DDS::RequestedDeadlineMissedStatus & status)
{
}

void
madara::transport::Splice_Subscriber_Listener::on_liveliness_changed (
  DDS::DataReader_ptr reader, const DDS::LivelinessChangedStatus & status)
{
}

void
madara::transport::Splice_Subscriber_Listener::on_subscription_matched (
  DDS::DataReader_ptr reader, const DDS::SubscriptionMatchedStatus & status)
{
}

void 
madara::transport::Splice_Subscriber_Listener::on_data_available(
  DDS::DataReader_ptr reader)
{
}


void 
madara::transport::Splice_Subscriber_Listener::on_data_on_readers(
  DDS::Subscriber_ptr sub)
{
  sub->notify_datareaders ();
}