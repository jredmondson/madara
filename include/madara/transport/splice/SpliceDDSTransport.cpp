#include "madara/transport/splice/SpliceDDSTransport.h"
#include "madara/knowledge/UpdateTypes.h"
#include "madara/utility/Utility.h"
#include <time.h>

#include <iostream>
#include <sstream>
#include "madara/logger/GlobalLogger.h"

namespace logger = madara::logger;

const char * madara::transport::SpliceDDSTransport::topic_names_[] = {
  "MADARA_KaRL_Data",
  "MADARA_KaRL_Control"
};

/* Array to hold the names for all ReturnCodes. */
const char * madara::transport::SpliceDDSTransport::ret_code_names[] = { 
    "DDS_RETCODE_OK",
    "DDS_RETCODE_ERROR",
    "DDS_RETCODE_UNSUPPORTED",
    "DDS_RETCODE_BAD_PARAMETER",
    "DDS_RETCODE_PRECONDITION_NOT_MET",
    "DDS_RETCODE_OUT_OF_RESOURCES",
    "DDS_RETCODE_NOT_ENABLED",
    "DDS_RETCODE_IMMUTABLE_POLICY",
    "DDS_RETCODE_INCONSISTENT_POLICY",
    "DDS_RETCODE_ALREADY_DELETED",
    "DDS_RETCODE_TIMEOUT",
    "DDS_RETCODE_NO_DATA",
    "DDS_RETCODE_ILLEGAL_OPERATION" };

const char * madara::transport::SpliceDDSTransport::partition_ = "Madara_knowledge";

madara::transport::SpliceDDSTransport::SpliceDDSTransport (
  const std::string & id,
  knowledge::ThreadSafeContext & context, 
  TransportSettings & config, bool launch_transport)
  : madara::transport::Base (id, config, context), 
  domain_ (0), domain_factory_ (0), 
  domain_participant_ (0), publisher_ (0), subscriber_ (0), 
  datawriter_ (0), datareader_ (0), 
  update_writer_ (0), update_reader_ (0),
  update_topic_ (0)
{
  // create a reference to the knowledge base for threading
  knowledge_.use (context);

  // set the data plane for the read threads
  read_threads_.set_data_plane (knowledge_);

  if (launch_transport)
    setup ();
}
madara::transport::SpliceDDSTransport::~SpliceDDSTransport ()
{
  close ();
}

void
madara::transport::SpliceDDSTransport::close (void)
{
  this->invalidate_transport ();

  read_threads_.terminate ();

  read_threads_.wait ();

  //if (subscriber_.in ())
  //{
  //  subscriber_->delete_datareader (update_reader_);
  //}

  //if (publisher_.in ())
  //{
  //  publisher_->delete_datawriter (update_writer_);
  //  publisher_->delete_datawriter (latency_update_writer_);
  //}


  //if (domain_participant_.in ())
  //{
  //  domain_participant_->delete_subscriber (subscriber_);
  //  domain_participant_->delete_publisher (publisher_);
  //  domain_participant_->delete_topic (update_topic_);
  //}

  //if (domain_factory_.in ())
  //  domain_factory_->delete_participant (domain_participant_);

  update_reader_ = 0;
  update_writer_ = 0;
  update_topic_ = 0;
  subscriber_ = 0;
  publisher_ = 0;
  domain_participant_ = 0;
  domain_factory_ = 0;

  this->shutting_down_ = false;
}

int
madara::transport::SpliceDDSTransport::reliability (void) const
{
  return this->settings_.reliability;
}

int
madara::transport::SpliceDDSTransport::reliability (const int & setting)
{
  return this->settings_.reliability = setting;
}

int
madara::transport::SpliceDDSTransport::setup (void)
{
  Base::setup ();
  DDS::ReturnCode_t                         status;

  this->is_valid_ = false;

  // reset the valid setup flag
  //valid_setup_ = false;

  madara_logger_log (context_.get_logger (), logger::LOG_DETAILED,
      "SpliceDDSTransport::setup:" \
      " Creating a participant for topic (%s)\n", 
      madara::utility::dds_topicify (settings_.write_domain).c_str ());

  madara_logger_log (context_.get_logger (), logger::LOG_DETAILED,
    "SpliceDDSTransport::setup:" \
    " Participant settings are being read from the OSPL_URI environment"
    " variable\n",
    madara::utility::dds_topicify (settings_.write_domain).c_str ());

  // get the domain participant factory
  domain_factory_ = DDS::DomainParticipantFactory::get_instance ();
  domain_factory_->get_default_participant_qos (part_qos_);
	domain_participant_ = domain_factory_->create_participant (
    domain_, 
    part_qos_, NULL, DDS::STATUS_MASK_NONE);

  // if dp == NULL, we've got an error
  if (domain_participant_ == NULL)
  {
    madara_logger_log (context_.get_logger (), logger::LOG_ERROR,
      "\nSpliceDDSTransport::setup:" \
      " splice daemon not running. Try 'ospl start'...\n");

    return -2;
  }

  domain_participant_->get_default_topic_qos(topic_qos_);

  if (madara::transport::RELIABLE == this->settings_.reliability)
  {
    topic_qos_.reliability.kind = DDS::RELIABLE_RELIABILITY_QOS;
    topic_qos_.history.depth = this->settings_.queue_length;
    topic_qos_.resource_limits.max_samples_per_instance = 
      this->settings_.queue_length;
    topic_qos_.resource_limits.max_samples = this->settings_.queue_length;
    topic_qos_.destination_order.kind = 
      DDS::BY_SOURCE_TIMESTAMP_DESTINATIONORDER_QOS;
    //topic_qos_.
  }
  //topic_qos_.resource_limits.max_samples_per_instance= 10;
  domain_participant_->set_default_topic_qos(topic_qos_);


  madara_logger_log (context_.get_logger (), logger::LOG_DETAILED,
    "SpliceDDSTransport::setup:" \
    " Registering type support\n");

  //  Register Update type
  status = this->update_type_support_.register_type (
    domain_participant_, "Knowledge::Update");
  check_status(status, "Knowledge::UpdateTypeSupport::register_type");

  //  Register Mutex type
  //status = this->mutex_type_support_.register_type (
  //  domain_participant_, "Knowledge::Mutex");
  //check_status(status, "Knowledge::MutexTypeSupport::register_type");

  madara_logger_log (context_.get_logger (), logger::LOG_DETAILED,
    "SpliceDDSTransport::setup:" \
    " Setting up knowledge domain via topic (%s)\n",
    madara::utility::dds_topicify (settings_.write_domain).c_str ());

  // Create Update topic
  update_topic_ = domain_participant_->create_topic (
    madara::utility::dds_topicify (settings_.write_domain).c_str (), 
    "Knowledge::Update", 
    topic_qos_, NULL, DDS::STATUS_MASK_NONE);
  check_handle(update_topic_, 
    "DDS::DomainParticipant::create_topic (KnowledgeUpdate)");

  // Get default qos for publisher
  status = domain_participant_->get_default_publisher_qos (pub_qos_);
  check_status(status, "DDS::DomainParticipant::get_default_publisher_qos");


  if (madara::transport::RELIABLE == this->settings_.reliability)
  {
    pub_qos_.presentation.access_scope = DDS::TOPIC_PRESENTATION_QOS;
    pub_qos_.presentation.coherent_access = true;
    pub_qos_.presentation.ordered_access = false;
    //topic_qos_.
  }

  madara_logger_log (context_.get_logger (), logger::LOG_DETAILED,
    "SpliceDDSTransport::setup:" \
    " Creating publisher for topic (%s)\n",
    madara::utility::dds_topicify (settings_.write_domain).c_str ());

  // Create publisher
  pub_qos_.partition.name.length (1);
  pub_qos_.partition.name[0] = DDS::string_dup (partition_);
  publisher_ = domain_participant_->create_publisher (
    pub_qos_, NULL, DDS::STATUS_MASK_NONE);
  check_handle(publisher_, "DDS::DomainParticipant::create_publisher");

  // Create subscriber
  status = domain_participant_->get_default_subscriber_qos (sub_qos_);
  check_status(status, "DDS::DomainParticipant::get_default_subscriber_qos");


  if (madara::transport::RELIABLE == this->settings_.reliability)
  {
    sub_qos_.presentation.access_scope = DDS::TOPIC_PRESENTATION_QOS;
    sub_qos_.presentation.coherent_access = true;
    sub_qos_.presentation.ordered_access = false;
  }

  madara_logger_log (context_.get_logger (), logger::LOG_DETAILED,
    "SpliceDDSTransport::setup:" \
    " Creating subscriber for topic (%s)\n",
    madara::utility::dds_topicify (settings_.write_domain).c_str ());

  sub_qos_.partition.name.length (1);
  sub_qos_.partition.name[0] = DDS::string_dup (partition_);
  subscriber_ = domain_participant_->create_subscriber (
//    sub_qos_, &sub_listener_, DDS::DATA_AVAILABLE_STATUS | DDS::DATA_ON_READERS_STATUS);
    sub_qos_, NULL, DDS::STATUS_MASK_NONE);
  check_handle(subscriber_, "DDS::DomainParticipant::create_subscriber");

  if (!subscriber_ || !publisher_)
  {
    madara_logger_log (context_.get_logger (), logger::LOG_ERROR,
      "SpliceDDSTransport::setup:" \
      " pub or sub could not be created. Try 'ospl stop; ospl start'...\n");

    return -2;
  }

  // Create datawriter
  publisher_->get_default_datawriter_qos (datawriter_qos_);
  publisher_->copy_from_topic_qos(datawriter_qos_, topic_qos_);

  if (madara::transport::RELIABLE == this->settings_.reliability)
  {
    madara_logger_log (context_.get_logger (), logger::LOG_DETAILED,
      "SpliceDDSTransport::setup:" \
      " Enabling reliable transport for (%s) datawriters\n",
      madara::utility::dds_topicify (settings_.write_domain).c_str ());

    datawriter_qos_.reliability.kind = DDS::RELIABLE_RELIABILITY_QOS;
    datawriter_qos_.history.depth = this->settings_.queue_length;
    datawriter_qos_.resource_limits.max_samples = this->settings_.queue_length;
    datawriter_qos_.resource_limits.max_samples_per_instance = 
      this->settings_.queue_length;
    datawriter_qos_.destination_order.kind = 
      DDS::BY_SOURCE_TIMESTAMP_DESTINATIONORDER_QOS;
  }
  else
  {
    madara_logger_log (context_.get_logger (), logger::LOG_DETAILED,
      "SpliceDDSTransport::setup:" \
      " Enabling unreliable transport for (%s) datawriters\n",
      madara::utility::dds_topicify (settings_.write_domain).c_str ());
  }

  madara_logger_log (context_.get_logger (), logger::LOG_DETAILED,
    "SpliceDDSTransport::setup:" \
    " Creating datawriter for topic (%s)\n",
    madara::utility::dds_topicify (settings_.write_domain).c_str ());

  // Create Update writer
  datawriter_ = publisher_->create_datawriter (update_topic_, 
    datawriter_qos_, NULL, DDS::STATUS_MASK_NONE);
  check_handle(datawriter_, "DDS::Publisher::create_datawriter (Update)");
  update_writer_ = dynamic_cast<Knowledge::UpdateDataWriter_ptr> (datawriter_.in ());
  check_handle(update_writer_, "Knowledge::UpdateDataWriter_ptr::narrow");

  // Create Latency Update writer for Read Thread
  latencywriter_ = publisher_->create_datawriter (update_topic_, 
    datawriter_qos_, NULL, DDS::STATUS_MASK_NONE);
  check_handle(latencywriter_, "DDS::Publisher::create_datawriter (Update)");
  latency_update_writer_ = dynamic_cast<Knowledge::UpdateDataWriter_ptr> (latencywriter_.in ());
  check_handle(latency_update_writer_, "Knowledge::UpdateDataWriter_ptr::narrow");


  // Create datareader
  status = subscriber_->get_default_datareader_qos (datareader_qos_);
  subscriber_->copy_from_topic_qos (datareader_qos_, topic_qos_);
  //publisher_->copy_from_topic_qos(datawriter_qos_, topic_qos_);
  check_status(status, "DDS::Subscriber::get_default_datareader_qos");

  datareader_qos_.reader_data_lifecycle.enable_invalid_samples = FALSE;

  if (madara::transport::RELIABLE == this->settings_.reliability)
  {
    madara_logger_log (context_.get_logger (), logger::LOG_DETAILED,
      "SpliceDDSTransport::setup:" \
      " Enabling reliable transport for (%s) datareaders\n",
      madara::utility::dds_topicify (settings_.write_domain).c_str ());

    datareader_qos_.reliability.kind = DDS::RELIABLE_RELIABILITY_QOS;
    datareader_qos_.history.depth = this->settings_.queue_length;
    datareader_qos_.resource_limits.max_samples = this->settings_.queue_length;
    datareader_qos_.destination_order.kind = 
      DDS::BY_SOURCE_TIMESTAMP_DESTINATIONORDER_QOS;

    // unlike the other qos, we do not set max_samples_per_instance here.
    // that shouldn't be as necessary, since we are using take on the reader
  }
  else
  {
    madara_logger_log (context_.get_logger (), logger::LOG_DETAILED,
      "SpliceDDSTransport::setup:" \
      " Enabling unreliable transport for (%s) datareaders\n",
      madara::utility::dds_topicify (settings_.write_domain).c_str ());
  }

  madara_logger_log (context_.get_logger (), logger::LOG_DETAILED,
    "SpliceDDSTransport::setup:" \
    " Creating datareader for topic (%s)\n",
    madara::utility::dds_topicify (settings_.write_domain).c_str ());

  // Create Update datareader
  datareader_ = subscriber_->create_datareader (update_topic_, 
    //datareader_qos_, &dr_listener_, DDS::STATUS_MASK_NONE);
    datareader_qos_, NULL, DDS::STATUS_MASK_NONE);

  // notes: we set the mask to none because the listener will be called
  // by the subscriber listener with notify_datareaders. This is the Splice
  // way of doing this, since we require subscription information and they
  // have so far not implemented on_subscription_matched.

  check_handle(datareader_, "DDS::Subscriber::create_datareader (Update)");
  update_reader_ = dynamic_cast<Knowledge::UpdateDataReader_ptr>(datareader_.in ());
  check_handle(update_reader_, "Knowledge::UpdateDataReader_ptr::narrow");

  if (!settings_.no_receiving)
  {
    double hertz = settings_.read_thread_hertz;
    if (hertz < 0.0)
    {
      hertz = 0.0;
    }

    madara_logger_log (context_.get_logger (), logger::LOG_MAJOR,
      "UdpTransportReadThread::setup:" \
      " starting %d threads at %f hertz\n", settings_.read_threads,
      hertz);

    for (uint32_t i = 0; i < settings_.read_threads; ++i)
    {
      std::stringstream thread_name;
      thread_name << "read";
      thread_name << i;

      read_threads_.run (hertz, thread_name.str (),
        new madara::transport::SpliceReadThread (id_, this->settings_,
        context_, update_reader_, latency_update_writer_, send_monitor_,
        receive_monitor_, packet_scheduler_));
    }
  }
  
  return this->validate_transport ();
}

long
madara::transport::SpliceDDSTransport::send_data (
  const knowledge::VariableReferenceMap & updates)
{
  long result = 0;

  if (!settings_.no_sending)
  {
    result = prep_send (updates, "SpliceDDSTransport::send_data:");

    // get the maximum quality from the updates
    uint32_t quality = knowledge::max_quality (updates);

    /// get current lamport clock. 
    unsigned long long cur_clock = context_.get_clock ();

    DDS::ReturnCode_t      dds_result;
    DDS::InstanceHandle_t  handle;

    Knowledge::Update data;

    data.buffer = Knowledge::seq_oct (result, result, (unsigned char *)buffer_.get_ptr ());
    data.clock = cur_clock;
    data.quality = quality;
    data.updates = DDS::ULong (updates.size ());
    data.originator = DDS::string_dup (id_.c_str ());
    data.type = madara::transport::MULTIASSIGN;
    data.ttl = settings_.get_rebroadcast_ttl ();
    data.timestamp = utility::get_time ();
    data.madara_id = DDS::string_dup (MADARA_IDENTIFIER);

    madara_logger_log (context_.get_logger (), logger::LOG_MAJOR,
      "SpliceDDSTransport::send:" \
      " sending multiassignment: %d updates, time=llu, quality=%d\n",
      data.updates, cur_clock, quality);

    handle = update_writer_->register_instance (data);
    dds_result = update_writer_->write (data, handle);
    result = (long)dds_result;
    //update_writer_->unregister_instance (data, handle);
  }

  return result;
}

void
madara::transport::SpliceDDSTransport::check_handle (void * handle, 
                                                      const char * info)
{
  if (!handle)
  {
    madara_logger_log (context_.get_logger (), logger::LOG_ERROR,
      "SpliceDDSTransport::check_handle:" \
      " error in %s: Creation failed: invalid handle\n", info);

    return -2;
  }
}

void
madara::transport::SpliceDDSTransport::check_status (DDS::ReturnCode_t status,
                                                       const char * info)
{
  // if the status is okay, then return without issue
  if ((status == DDS::RETCODE_OK) || (status == DDS::RETCODE_NO_DATA)) 
    return;

  madara_logger_log (context_.get_logger (), logger::LOG_ERROR,
    "SpliceDDSTransport::check_status:" \
    " error in %s: Creation failed: %s\n",
    info, get_error_name (status));

  return -2;
}

const char *
madara::transport::SpliceDDSTransport::get_error_name(DDS::ReturnCode_t status)
{
    return ret_code_names[status];
}
