#include "madara/transport/ndds/NDDS_Listener.h"
#include "madara/utility/Log_Macros.h"
#include "madara/knowledge_engine/Update_Types.h"
#include "madara/utility/Utility.h"

#include <sstream>

Madara::Transport::NDDS_Listener::NDDS_Listener(
  const Settings & settings, const std::string & id,
  Madara::Knowledge_Engine::Thread_Safe_Context & context,
  Bandwidth_Monitor & send_monitor,
  Bandwidth_Monitor & receive_monitor,
  Packet_Scheduler & packet_scheduler)
: settings_ (settings), id_ (id), context_ (context),
  send_monitor_ (send_monitor), receive_monitor_ (receive_monitor),
  packet_scheduler_ (packet_scheduler)
{
  qos_settings_ = dynamic_cast <const QoS_Transport_Settings *> (&settings);
  
  // setup the receive buffer
  if (settings_.queue_length > 0)
    buffer_ = new char [settings_.queue_length];

  // check for an on_data_received ruleset
  if (settings_.on_data_received_logic.length () != 0)
  {
    MADARA_DEBUG (MADARA_LOG_MAJOR_EVENT, (LM_DEBUG, 
      DLINFO
      "NDDS_Listener::NDDS_Listener:" \
      " setting rules to %s\n", 
      settings_.on_data_received_logic.c_str ()));

    Madara::Expression_Tree::Interpreter interpreter;
    on_data_received_ = context_.compile (settings_.on_data_received_logic);
  }
  else
  {
    MADARA_DEBUG (MADARA_LOG_MAJOR_EVENT, (LM_DEBUG, 
      DLINFO "NDDS_Listener::NDDS_Listener:" \
      " no permanent rules were set\n"));
  }
  
}

Madara::Transport::NDDS_Listener::NDDS_Listener(const NDDS_Listener &ref)
: settings_ (ref.settings_), id_ (ref.id_), context_ (ref.context_), 
  send_monitor_ (ref.send_monitor_), receive_monitor_ (ref.receive_monitor_),
  packet_scheduler_ (ref.packet_scheduler_)
{
}

Madara::Transport::NDDS_Listener::~NDDS_Listener()
{}

void
Madara::Transport::NDDS_Listener::on_subscription_matched (
  DDSDataReader* reader, const DDS_SubscriptionMatchedStatus& status)
{
  context_.set_changed ();
}

void
Madara::Transport::NDDS_Listener::rebroadcast (
  const char * print_prefix,
  Message_Header * header,
  const Knowledge_Map & records)
{
  int64_t buffer_remaining = (int64_t) settings_.queue_length;
  char * buffer = buffer_.get_ptr ();
  unsigned long result = prep_rebroadcast (buffer, buffer_remaining,
                                 *qos_settings_, print_prefix,
                                 header, records,
                                 packet_scheduler_);

  if (result > 0)
  {
    ssize_t bytes_sent (result + sizeof (NDDS_Knowledge_Update));

    MADARA_DEBUG (MADARA_LOG_MAJOR_EVENT, (LM_DEBUG, 
      DLINFO "%s:" \
      " Sent packet of size %d\n",
      print_prefix,
      bytes_sent));
      
    send_monitor_.add ((uint32_t)bytes_sent);

    MADARA_DEBUG (MADARA_LOG_MINOR_EVENT, (LM_DEBUG, 
      DLINFO "%s:" \
      " Send bandwidth = %d B/s\n",
      print_prefix,
      send_monitor_.get_bytes_per_second ()));
  }
}

void 
Madara::Transport::NDDS_Listener::on_data_available(DDSDataReader * reader)
{
  NDDS_Knowledge_UpdateSeq update_data_list;
  DDS_SampleInfoSeq info_seq;
  DDS_ReturnCode_t rc;
  
  const char * print_prefix = "NDDS_Listener::svc";

  NDDS_Knowledge_UpdateDataReader * update_reader = 
    NDDS_Knowledge_UpdateDataReader::narrow(reader);
  if (update_reader == NULL)
  {
    MADARA_ERROR (MADARA_LOG_TERMINAL_ERROR, (LM_ERROR, DLINFO
      "\n%s:" \
      " Unable to create specialized reader. Leaving callback...\n",
      print_prefix));
    return;
  }

  rc = update_reader->take(
    update_data_list, 
    info_seq, 
    DDS_LENGTH_UNLIMITED,
    DDS_ANY_SAMPLE_STATE, 
    DDS_ANY_VIEW_STATE, 
    DDS_ANY_INSTANCE_STATE);

  if (rc == DDS_RETCODE_NO_DATA)
  {
    return;
  }
  else if (rc != DDS_RETCODE_OK)
  {
    MADARA_DEBUG (MADARA_LOG_MINOR_EVENT, (LM_DEBUG, 
      DLINFO "%s:" \
      " could not take current sample.\n", print_prefix));
    return;
  }

  for (int i = 0; i < update_data_list.length(); ++i)
  {
    if (info_seq[i].valid_data)
    {
      // if we are evaluating a message from ourselves, just continue
      // to the next one. It's also possible to receive null originators
      // from what I can only guess is the ospl daemon messing up
      if (strncmp (update_data_list[i].originator, "", 1) == 0)
      {
        // if we don't check originator for null, we get phantom sends
        // when the program exits.
        MADARA_DEBUG (MADARA_LOG_EVENT_TRACE, (LM_DEBUG,
          DLINFO "%s:" \
          " discarding null originator event.\n", print_prefix));

        continue;
      }

      if (update_data_list[i].type != Madara::Transport::MULTIASSIGN)
      {
        // we do not allow any other type than multiassign
        MADARA_DEBUG (MADARA_LOG_EVENT_TRACE, (LM_DEBUG,
          DLINFO "%s:" \
          " discarding non-assignment event.\n", print_prefix));

        continue;
      }

      Knowledge_Map rebroadcast_records;
      Message_Header * header = 0;

      if (Madara::Knowledge_Engine::MULTIPLE_ASSIGNMENT == update_data_list[i].type)
      {
        MADARA_DEBUG (MADARA_LOG_MINOR_EVENT, (LM_DEBUG, 
          DLINFO "%s:" \
          " processing multassignment from %s with time %Q and quality %u.\n",
          print_prefix, update_data_list[i].originator,
          update_data_list[i].clock, update_data_list[i].quality));
        
        process_received_update ((char *)update_data_list[i].buffer.get_contiguous_buffer (),
          update_data_list[i].buffer.length (), id_, context_,
          *qos_settings_, send_monitor_, receive_monitor_, rebroadcast_records,
          on_data_received_, print_prefix,
          "", header);
      }        
    }
  }

  rc = update_reader->return_loan(update_data_list, info_seq);
  if (rc != DDS_RETCODE_OK)
  {
    MADARA_DEBUG (MADARA_LOG_MINOR_EVENT, (LM_DEBUG, 
      DLINFO "%s:" \
      " could return DDS sample instance loan.\n", print_prefix));
  }
}