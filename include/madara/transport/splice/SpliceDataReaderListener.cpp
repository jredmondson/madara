#include "madara/transport/splice/SpliceDataReaderListener.h"
#include "madara/utility/LogMacros.h"
#include "madara/knowledge/UpdateTypes.h"
#include "madara/utility/Utility.h"

#include <iostream>
#include <sstream>

madara::transport::SpliceDataReaderListener::SpliceDataReaderListener(
  const std::string & id,
  madara::knowledge::ThreadSafeContext & context)
: id_ (id), context_ (context)
{
}

madara::transport::SpliceDataReaderListener::SpliceDataReaderListener(
  const SpliceDataReaderListener &ref)
: id_ (ref.id_), context_ (ref.context_)
{
}

madara::transport::SpliceDataReaderListener::~SpliceDataReaderListener()
{}

void madara::transport::SpliceDataReaderListener::handle_assignment (
  Knowledge::Update & data)
{
  if (data.key.val ())
  {
    // if we aren't evaluating a message from ourselves, process it
    std::string key = data.key.val ();
    long long value = data.value;
    int result = 0;

    madara_logger_log (context_->get_logger (), logger::LOG_MAJOR,
      "SpliceDataReaderListener::handle_assignment:" \
      " waiting to process assignment\n");
      
    context_.lock ();
    unsigned long long cur_clock = context_.get_clock (key);
    unsigned long cur_quality = context_.get_quality (key);

    // if the data we are updating had a lower clock value or less quality
    // then that means this update is the latest value. Among
    // other things, this means our solution will work even
    // without FIFO channel transports

    // if the data we are updating had a lower clock value
    // then that means this update is the latest value. Among
    // other things, this means our solution will work even
    // without FIFO channel transports
    result = context_.set_if_unequal (key, value, 
                                      data.quality, data.clock, false);

    context_.unlock ();
    
    // if we actually updated the value
    if (result == 1)
    {
      madara_logger_log (context_->get_logger (), logger::LOG_MAJOR,
        "SpliceDataReaderListener::handle_assignment:" \
        " received data[%s]=%q from %s\n",
        key.c_str (), value, data.originator.val ());
    }
    // if the data was already current
    else if (result == 0)
    {
      madara_logger_log (context_->get_logger (), logger::LOG_MAJOR,
        "SpliceDataReaderListener::handle_assignment:" \
        " discarded data[%s]=%q from %s as the value was already set\n",
        key.c_str (), value, data.originator.val ());
    }
    else if (result == -1)
    {
      madara_logger_log (context_->get_logger (), logger::LOG_MAJOR,
        "SpliceDataReaderListener::handle_assignment:" \
        " discarded data due to null key\n",
        key.c_str (), value, data.originator.val ());
    }
    else if (result == -2)
    {
      madara_logger_log (context_->get_logger (), logger::LOG_MAJOR,
        "SpliceDataReaderListener::handle_assignment:" \
        " discarded data[%s]=%q due to lower quality (%u vs %u)\n",
        key.c_str (), value, cur_quality, data.quality);
    }
    else if (result == -3)
    {
      madara_logger_log (context_->get_logger (), logger::LOG_MAJOR,
        "SpliceDataReaderListener::handle_assignment:" \
        " discarded data[%s]=%q due to older timestamp (%Q vs %Q)\n",
        key.c_str (), value, cur_clock, data.clock);
    }
  }
}

void madara::transport::SpliceDataReaderListener::handle_multiassignment (
  Knowledge::Update & data)
{
  if (data.key.val ())
  {
    std::string key;
    char symbol;
    long long value;
    std::stringstream stream (data.key.val ());

    madara_logger_log (context_->get_logger (), logger::LOG_MAJOR,
      "SpliceDataReaderListener::handle_multiassignment:" \
      " waiting to process multiassignment\n");

    context_.lock ();
    
    madara_logger_log (context_->get_logger (), logger::LOG_MAJOR,
      "SpliceDataReaderListener::handle_multiassignment:" \
      " processing multiassignment (%s)\n",
      data.key.val ());

    while (!stream.eof ())
    {
      stream >> key >> symbol >> value >> symbol;

      int result = 0;
      unsigned long long cur_clock = context_.get_clock (key);
      unsigned long cur_quality = context_.get_quality (key);

      // if the data we are updating had a lower clock value
      // then that means this update is the latest value. Among
      // other things, this means our solution will work even
      // without FIFO channel transports
      result = context_.set_if_unequal (key, value, 
                                        data.quality, data.clock, false);

      // if we actually updated the value
      if (result == 1)
      {
        madara_logger_log (context_->get_logger (), logger::LOG_MAJOR,
          "SpliceDataReaderListener::handle_multiassignment:" \
          " received data[%s]=%q from %s\n",
          key.c_str (), value, data.originator.val ());
      }
      // if the data was already current
      else if (result == 0)
      {
        madara_logger_log (context_->get_logger (), logger::LOG_MAJOR,
          "SpliceDataReaderListener::handle_multiassignment:" \
          " discarded data[%s]=%q from %s as the value was already set\n",
          key.c_str (), value, data.originator.val ());
      }
      else if (result == -1)
      {
        madara_logger_log (context_->get_logger (), logger::LOG_MAJOR,
          "SpliceDataReaderListener::handle_multiassignment:" \
          " discarded data due to null key\n",
          key.c_str (), value, data.originator.val ());
      }
      else if (result == -2)
      {
        madara_logger_log (context_->get_logger (), logger::LOG_MAJOR,
          "SpliceDataReaderListener::handle_multiassignment:" \
          " discarded data[%s]=%q due to lower quality (%u vs %u)\n",
          key.c_str (), value, cur_quality, data.quality);
      }
      else if (result == -3)
      {
        madara_logger_log (context_->get_logger (), logger::LOG_MAJOR,
          "SpliceDataReaderListener::handle_multiassignment:" \
          " discarded data[%s]=%q due to older timestamp (%Q vs %Q)\n",
          key.c_str (), value, cur_clock, data.clock);
      }
    }
    
    context_.unlock ();
  }
}

void
madara::transport::SpliceDataReaderListener::on_sample_lost (
  DDS::DataReader_ptr, const DDS::SampleLostStatus &status)
{
}

void
madara::transport::SpliceDataReaderListener::on_sample_rejected (
  DDS::DataReader_ptr, const DDS::SampleRejectedStatus &status)
{
}

void
madara::transport::SpliceDataReaderListener::on_requested_incompatible_qos (
  DDS::DataReader_ptr, const DDS::RequestedIncompatibleQosStatus &status)
{
}

void
madara::transport::SpliceDataReaderListener::on_requested_deadline_missed (
  DDS::DataReader_ptr reader, const DDS::RequestedDeadlineMissedStatus & status)
{
}

void
madara::transport::SpliceDataReaderListener::on_liveliness_changed (
  DDS::DataReader_ptr reader, const DDS::LivelinessChangedStatus & status)
{
}

void
madara::transport::SpliceDataReaderListener::on_subscription_matched (
  DDS::DataReader_ptr reader, const DDS::SubscriptionMatchedStatus & status)
{
  context_.set_changed ();
}

void 
madara::transport::SpliceDataReaderListener::on_data_available(
  DDS::DataReader_ptr reader)
{
  DDS::SampleInfoSeq_var infoList = new DDS::SampleInfoSeq;
  DDS::ReturnCode_t      dds_result;
  int                    amount;
  DDS::Boolean           result = false;
  Knowledge::UpdateSeq_var update_data_list_ = new Knowledge::UpdateSeq;

  Knowledge::UpdateDataReader_ptr update_reader = 
    dynamic_cast<Knowledge::UpdateDataReader_ptr> (reader);

  if (update_reader == 0)
  {
    madara_logger_log (context_->get_logger (), logger::LOG_ERROR,
      "SpliceDataReaderListener::on_data_available:" \
      " Unable to create specialized reader. Leaving callback...\n");

    return;
  }

  dds_result = update_reader->take (update_data_list_, infoList, 1, 
    DDS::ANY_SAMPLE_STATE, DDS::ANY_VIEW_STATE, DDS::ANY_INSTANCE_STATE);

  amount = update_data_list_->length ();

  if (amount != 0)
  {
    for (int i = 0; i < amount; ++i)
    {
      // if we are evaluating a message from ourselves, just continue
      // to the next one. It's also possible to receive null originators
      // from what I can only guess is the ospl daemon messing up
      if (!update_data_list_[i].originator.val () || 
        id_ == update_data_list_[i].originator.val ())
      {
        madara_logger_log (context_->get_logger (), logger::LOG_MINOR,
          "SpliceDataReaderListener::on_data_available:" \
          " discarding null originator event\n");

        continue;
      }

      if (madara::knowledge::ASSIGNMENT == update_data_list_[i].type)
      {
        madara_logger_log (context_->get_logger (), logger::LOG_MINOR,
          "SpliceDataReaderListener::on_data_available:" \
          " processing %s=%q from %s with time %Q and quality %u\n", 
          update_data_list_[i].key.val (), update_data_list_[i].value, 
          update_data_list_[i].originator.val (),
          update_data_list_[i].clock, update_data_list_[i].quality);

        handle_assignment (update_data_list_[i]);
      }
      else if (madara::knowledge::MULTIPLE_ASSIGNMENT == update_data_list_[i].type)
      {
        madara_logger_log (context_->get_logger (), logger::LOG_MINOR,
          "SpliceDataReaderListener::on_data_available:" \
          " processing multassignment from %s with time %Q and quality %u\n",
          update_data_list_[i].originator.val (),
          update_data_list_[i].clock, update_data_list_[i].quality);

        handle_multiassignment (update_data_list_[i]);
      }

      // otherwise the key was null, which is unusable
    }
  }
  dds_result = update_reader->return_loan (update_data_list_, infoList);

}