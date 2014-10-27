#include "madara/transport/splice/Splice_Subscriber_Listener.h"
#include "madara/utility/Log_Macros.h"
#include "madara/knowledge_engine/Update_Types.h"
#include "madara/utility/Utility.h"

#include <iostream>
#include <sstream>

Madara::Transport::Splice_Subscriber_Listener::Splice_Subscriber_Listener(
  const std::string & id,
  Madara::Knowledge_Engine::Thread_Safe_Context & context)
: id_ (id), context_ (context)
{
}

Madara::Transport::Splice_Subscriber_Listener::Splice_Subscriber_Listener(
  const Splice_Subscriber_Listener &ref)
: id_ (ref.id_), context_ (ref.context_)
{
}

Madara::Transport::Splice_Subscriber_Listener::~Splice_Subscriber_Listener()
{}

void
Madara::Transport::Splice_Subscriber_Listener::on_sample_lost (
  DDS::DataReader_ptr, const DDS::SampleLostStatus &status)
{
}

void
Madara::Transport::Splice_Subscriber_Listener::on_sample_rejected (
  DDS::DataReader_ptr, const DDS::SampleRejectedStatus &status)
{
}

void
Madara::Transport::Splice_Subscriber_Listener::on_requested_incompatible_qos (
  DDS::DataReader_ptr, const DDS::RequestedIncompatibleQosStatus &status)
{
}

void
Madara::Transport::Splice_Subscriber_Listener::on_requested_deadline_missed (
  DDS::DataReader_ptr reader, const DDS::RequestedDeadlineMissedStatus & status)
{
}

void
Madara::Transport::Splice_Subscriber_Listener::on_liveliness_changed (
  DDS::DataReader_ptr reader, const DDS::LivelinessChangedStatus & status)
{
}

void
Madara::Transport::Splice_Subscriber_Listener::on_subscription_matched (
  DDS::DataReader_ptr reader, const DDS::SubscriptionMatchedStatus & status)
{
}

void 
Madara::Transport::Splice_Subscriber_Listener::on_data_available(
  DDS::DataReader_ptr reader)
{
}


void 
Madara::Transport::Splice_Subscriber_Listener::on_data_on_readers(
  DDS::Subscriber_ptr sub)
{
  sub->notify_datareaders ();
}