#include "madara/transport/splice/Splice_Transport_Read_Thread.h"
#include "madara/utility/Log_Macros.h"
#include "madara/knowledge_engine/Update_Types.h"
#include "madara/utility/Utility.h"

#include <iostream>
#include <sstream>

Madara::Transport::Splice_Read_Thread::Splice_Read_Thread (
  const std::string & id,
  const Settings & settings,
  Madara::Knowledge_Engine::Thread_Safe_Context & context, 
  Knowledge::UpdateDataReader_ptr & update_reader,
  Knowledge::UpdateDataWriter_ptr & update_writer,
  Bandwidth_Monitor & send_monitor,
  Bandwidth_Monitor & receive_monitor,
  Packet_Scheduler & packet_scheduler)
  : id_ (id), settings_ (settings), context_ (context),
    update_reader_ (update_reader),
    update_writer_ (update_writer),
    barrier_ (2), terminated_ (false), 
    mutex_ (), is_not_ready_ (mutex_), is_ready_ (false),
    send_monitor_ (send_monitor),
    receive_monitor_ (receive_monitor),
    packet_scheduler_ (packet_scheduler)
{
  assignment_symbols_.push_back ("=");
  assignment_symbols_.push_back (";");
  
  qos_settings_ = dynamic_cast <const QoS_Transport_Settings *> (&settings);
  
  // setup the receive buffer
  if (settings_.queue_length > 0)
    buffer_ = new char [settings_.queue_length];

  // check for an on_data_received ruleset
  if (settings_.on_data_received_logic.length () != 0)
  {
    MADARA_DEBUG (MADARA_LOG_MAJOR_EVENT, (LM_DEBUG, 
      DLINFO
      "Splice_Read_Thread::Splice_Read_Thread:" \
      " setting rules to %s\n", 
      settings_.on_data_received_logic.c_str ()));

    Madara::Expression_Tree::Interpreter interpreter;
    on_data_received_ = context_.compile (settings_.on_data_received_logic);
  }
  else
  {
    MADARA_DEBUG (MADARA_LOG_MAJOR_EVENT, (LM_DEBUG, 
      DLINFO "Splice_Read_Thread::Splice_Read_Thread:" \
      " no permanent rules were set\n"));
  }
  
  int result = this->activate (THR_NEW_LWP | THR_DETACHED, 1);
  
  MADARA_DEBUG (MADARA_LOG_MAJOR_EVENT, (LM_DEBUG, 
    DLINFO "Splice_Read_Thread::Splice_Read_Thread:" \
    " read thread started\n"));

  if (result != -1)
  {
    MADARA_DEBUG (MADARA_LOG_MAJOR_EVENT, (LM_DEBUG, 
      DLINFO "Splice_Read_Thread::Splice_Read_Thread:" \
      " read thread started (result = %d)\n", result));
  }
  else
  {
    MADARA_DEBUG (MADARA_LOG_MAJOR_EVENT, (LM_DEBUG, 
      DLINFO "Splice_Read_Thread::Splice_Read_Thread:" \
      " failed to create thread. ERRNO = %d\n", ACE_OS::last_error ()));
  }
}

Madara::Transport::Splice_Read_Thread::~Splice_Read_Thread ()
{
  close ();
}

int
Madara::Transport::Splice_Read_Thread::close (void)
{
  terminated_ = true;
  enter_barrier ();

  return 0;
}

void
Madara::Transport::Splice_Read_Thread::wait_for_ready (void)
{
  if (!is_ready_)
    is_not_ready_.wait ();
}

int Madara::Transport::Splice_Read_Thread::enter_barrier (void)
{ 
  MADARA_DEBUG (MADARA_LOG_MAJOR_EVENT, (LM_DEBUG, 
    DLINFO "Splice_Read_Thread::enter_barrier:" \
    " beginning thread barrier for shut down of read thread\n"));

  barrier_.wait ();
  return 0;
}

#ifdef _USE_CID_
/**
 * originator == person who started the latency rounds
 * key == the person replying to the round
 * clock == the time the round was started
 * value == the step in the round (0 = initial, 1 = reply, 2 = reply to reply)
 **/

void
Madara::Transport::Splice_Read_Thread::handle_latency_aggregation (
  Knowledge::Update & data)
{
  Madara::Transport::Settings::Context_Guard guard (settings_.mutex);
  
  // if we were the aggregator, don't apply the updates to ourself.
  if (data.quality == settings_.id)
    return;

  settings_.unaggregate_latencies (data.quality, data.key.in ());
}

void
Madara::Transport::Splice_Read_Thread::handle_vote (
  Knowledge::Update & data)
{
  Madara::Transport::Settings::Context_Guard guard (settings_.mutex);
  
  unsigned long long vote_best = (unsigned long long) data.value;

  if (settings_.latencies.results.size () == 0)
  {
    settings_.latencies.results.resize (1);
    settings_.latencies.results[0].deployment = data.key.in ();
    settings_.latencies.results[0].latency = vote_best;
  }
  else
  {
    if (settings_.latencies.results[0].latency > vote_best)
    {
      settings_.latencies.results[0].deployment = data.key.in ();
      settings_.latencies.results[0].latency = vote_best;
    }
  }

  // increment 
  ++settings_.num_votes_received;
  if (settings_.num_votes_received == settings_.num_voters)
  {
    // check the best result compared to 
    Madara::Cid::reset_solution (settings_.latencies);
    unsigned long long cur_latency = 
      Madara::Cid::calculate_latency (settings_.latencies);

    // grab the current voted best
    vote_best = settings_.latencies.results[0].latency;
    double & acceptable = settings_.redeployment_percentage_allowed;
  
    MADARA_DEBUG (MADARA_LOG_MINOR_EVENT, (LM_DEBUG, 
      DLINFO "Splice_DDS_Transport::handle_vote:" \
      " cur_latency is %Q, voted_best is %Q\n", 
      cur_latency, vote_best));

    // if the voted best is worse than our current latency, then we don't
    // do anything (we're already best latency
    if (vote_best < cur_latency)
    {
      // grab the absolute difference
      double difference = (double) (cur_latency - vote_best);

      if (acceptable < difference / (double)cur_latency)
      {
        // redeployment requirement has been met
        settings_.read_solution (settings_.latencies.results[0].deployment,
          id_);
              
        MADARA_DEBUG (MADARA_LOG_MAJOR_EVENT, (LM_DEBUG, 
          DLINFO "Splice_DDS_Transport::handle_vote:" \
          " updating solution with latency of %Q\n", 
          vote_best));

      }
    }

    // vote finished
    settings_.num_votes_received = 0;
  }
}

/**
 * originator == person who started the latency rounds
 * key == the person replying to the round
 * clock == the time the round was started
 * value == the step in the round (0 = initial, 1 = reply, 2 = reply to reply)
 **/

void
Madara::Transport::Splice_Read_Thread::handle_latency_summation (
  Knowledge::Update & data)
{
  Madara::Transport::Settings::Context_Guard guard (settings_.mutex);
  
  ++settings_.num_summations;

  MADARA_DEBUG (MADARA_LOG_MINOR_EVENT, (LM_DEBUG, 
    DLINFO "Splice_DDS_Transport::handle_latency_summation:" \
    " originator=%s, key=%s, quality=%u, step=%q\n", 
    data.originator.val (), data.key.val (),
    data.quality, data.value));

  // if we were the aggregator, don't apply the updates to ourself.
  if (data.quality != settings_.id)
    settings_.unpack_summations (data.quality, data.key.in ());

  // if we've collected all summations, then sort our summations for CID
  if (settings_.num_summations == settings_.processes)
  {
    Madara::Cid::Summations_Map & summations =
      settings_.latencies.network_summations;
    for (Madara::Cid::Summations_Map::iterator i = summations.begin ();
      i != summations.end (); ++i)
    {
      std::sort (i->second.begin (), i->second.end (),
        Madara::Cid::Increasing_Latency);
    }

    settings_.num_summations = 0;
  }
}


/**
 * originator == person who started the latency rounds
 * key == the person replying to the round
 * clock == the time the round was started
 * value == the step in the round (0 = initial, 1 = reply, 2 = reply to reply)
 **/

void
Madara::Transport::Splice_Read_Thread::handle_latency (
  Knowledge::Update & data)
{
//  DDS::InstanceHandle_t  handle;
  Madara::Transport::Settings::Context_Guard guard (settings_.mutex);
  /**
   * During value == 0 (initial), the originator called start_latency.
   **/
  if (data.value == 0)
  {
    if (settings_.latencies.ids[data.quality] != data.originator.val ())
      settings_.latencies.ids[data.quality] = data.originator.val ();

    // originator doesn't respond to itself
    if (data.originator.val () == id_)
    {
      settings_.num_responses = 1;
      settings_.num_summations = 0;
      settings_.stop_timer (data.quality, false);

      if (settings_.num_responses == settings_.processes)
        settings_.pack_summations ();

      return;
    }
    else
    {
      settings_.num_responses = 0;
      settings_.num_summations = 0;
    }

    // everyone else needs to respond though

    Knowledge::Update reply_data;
    reply_data.key = data.originator.val ();
    reply_data.value = 1;
    reply_data.clock = data.clock;
    reply_data.quality = this->settings_.id;
    reply_data.originator = id_.c_str ();
    reply_data.type = Madara::Transport::LATENCY;

    MADARA_DEBUG (MADARA_LOG_MINOR_EVENT, (LM_DEBUG, 
      DLINFO "Splice_DDS_Transport::handle_latency:" \
      " originator=%s, key=%s, quality=%u, step=%q\n", 
      reply_data.originator.val (), reply_data.key.val (),
      reply_data.quality, reply_data.value));

    settings_.reset_timers ();
    settings_.start_all_timers ();

    //handle = update_writer_->register_instance (data);
    update_writer_->write (reply_data, 0); 
  }

  /**
   * During value == 1 (reply), the other participants are replying to the orig
   **/
  else if (data.value == 1)
  {
    if (settings_.latencies.ids[data.quality] != data.originator.val ())
      settings_.latencies.ids[data.quality] = data.originator.val ();

    // key doesn't respond to itself
    if (data.originator.val () == id_)
    {
      settings_.stop_timer (data.quality, false);
      ++settings_.num_responses;

      if (settings_.num_responses == settings_.processes)
        settings_.pack_summations ();

      return;
    }

    // if we're the originator, send a reply then record our latency
    if (data.key.val () == id_)
    {
      // stop stored timer
      settings_.stop_timer (data.quality);
      ++settings_.num_responses;


      if (settings_.num_responses == settings_.processes)
      {
        // everyone sends their aggregation once it's complete
        Knowledge::Update reply_data;
        reply_data.key = settings_.aggregate_latencies ().c_str ();
        reply_data.value = settings_.processes;
        reply_data.clock = data.clock;
        reply_data.quality = this->settings_.id;
        reply_data.originator = id_.c_str ();
        reply_data.type = Madara::Transport::LATENCY_AGGREGATE;

        update_writer_->write (reply_data, 0);

        MADARA_DEBUG (MADARA_LOG_MINOR_EVENT, (LM_DEBUG, 
          DLINFO "Splice_DDS_Transport::handle_latency::aggregate:" \
          " originator=%s, key=%s, quality=%u, step=%q\n", 
          reply_data.originator.val (), reply_data.key.val (),
          reply_data.quality, reply_data.value));

        Knowledge::Update summation;
        summation.value = settings_.processes;
        summation.clock = data.clock;
        summation.quality = this->settings_.id;
        summation.originator = id_.c_str ();
        summation.key = settings_.pack_summations ().c_str ();
        summation.type = Madara::Transport::LATENCY_SUMMATION;

        //handle = update_writer_->register_instance (data);
        update_writer_->write (summation, 0);

        MADARA_DEBUG (MADARA_LOG_MINOR_EVENT, (LM_DEBUG, 
          DLINFO "Splice_DDS_Transport::handle_latency::summation:" \
          " originator=%s, key=%s, quality=%u, step=%q\n", 
          summation.originator.val (), summation.key.val (),
          summation.quality, summation.value));

      }
    }

    // everyone sends a response to the key as the orig
    Knowledge::Update reply_data;
    reply_data.key = data.originator.val ();
    reply_data.value = 2;
    reply_data.clock = data.clock;
    reply_data.quality = this->settings_.id;
    reply_data.originator = id_.c_str ();
    reply_data.type = Madara::Transport::LATENCY;

    MADARA_DEBUG (MADARA_LOG_MINOR_EVENT, (LM_DEBUG, 
      DLINFO "Splice_DDS_Transport::handle_latency:" \
      " originator=%s, key=%s, quality=%u, step=%q\n", 
      reply_data.originator.val (), reply_data.key.val (),
      reply_data.quality, reply_data.value));

    //handle = update_writer_->register_instance (data);
    update_writer_->write (reply_data, 0); 
  }

  /**
   * During value == 2 (reply-reply), people are replying to a reply
   * The only person who cares about this is the originator in the data
   **/
  else if (data.value == 2 && data.key.val () == id_)
  {
    // stop the timer and store the latency
    settings_.stop_timer (data.quality);
    ++settings_.num_responses;

    if (settings_.latencies.ids[data.quality] != data.originator.val ())
      settings_.latencies.ids[data.quality] = data.originator.val ();

    
    if (settings_.num_responses == settings_.processes)
    {
      // everyone sends their aggregation once it's complete
      Knowledge::Update reply_data;
      reply_data.key = settings_.aggregate_latencies ().c_str ();
      reply_data.value = settings_.processes;
      reply_data.clock = data.clock;
      reply_data.quality = this->settings_.id;
      reply_data.originator = id_.c_str ();
      reply_data.type = Madara::Transport::LATENCY_AGGREGATE;

      //handle = update_writer_->register_instance (data);
      update_writer_->write (reply_data, 0);

      MADARA_DEBUG (MADARA_LOG_MINOR_EVENT, (LM_DEBUG, 
        DLINFO "Splice_DDS_Transport::handle_latency::aggregate:" \
        " originator=%s, key=%s, quality=%u, step=%q\n", 
        reply_data.originator.val (), reply_data.key.val (),
        reply_data.quality, reply_data.value));

      Knowledge::Update summation;
      summation.value = settings_.processes;
      summation.clock = data.clock;
      summation.quality = this->settings_.id;
      summation.originator = id_.c_str ();
      summation.key = settings_.pack_summations ().c_str ();
      summation.type = Madara::Transport::LATENCY_SUMMATION;

      //handle = update_writer_->register_instance (data);
      update_writer_->write (summation, 0);

      MADARA_DEBUG (MADARA_LOG_MINOR_EVENT, (LM_DEBUG, 
        DLINFO "Splice_DDS_Transport::handle_latency::summation:" \
        " originator=%s, key=%s, quality=%u, step=%q\n", 
        summation.originator.val (), summation.key.val (),
        summation.quality, summation.value));

    }
  }
}

#endif   // USE_CID

void
Madara::Transport::Splice_Read_Thread::rebroadcast (
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
    ssize_t bytes_sent (result + sizeof (Knowledge::Update));
    DDS::ReturnCode_t dds_result;
    DDS::InstanceHandle_t handle;
    
    // get the maximum quality from the updates
    uint32_t quality = Madara::max_quality (records);

    /// get current lamport clock. 
    unsigned long long cur_clock = context_.get_clock ();

    Knowledge::Update data;
  
    data.buffer = Knowledge::seq_oct (result, result,
      (unsigned char *)buffer_.get_ptr ());
    data.clock = cur_clock;
    data.quality = quality;
    data.updates = DDS::ULong (records.size ());
    data.originator = DDS::string_dup(id_.c_str ());
    data.type = Madara::Transport::MULTIASSIGN;
    data.ttl = qos_settings_->get_rebroadcast_ttl ();
    data.timestamp = time (NULL);
    data.madara_id = DDS::string_dup(MADARA_IDENTIFIER);
    
    handle = update_writer_->register_instance (data);
    dds_result = update_writer_->write (data, handle); 

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

int
Madara::Transport::Splice_Read_Thread::svc (void)
{
  DDS::SampleInfoSeq_var infoList = new DDS::SampleInfoSeq;
  DDS::ReturnCode_t      dds_result;
  int                    amount;
  DDS::Boolean           result = false;
  const char * print_prefix = "Splice_Read_Thread::svc";
  Knowledge::UpdateSeq_var update_data_list_ = new Knowledge::UpdateSeq;

  DDS::WaitSet                   waitset_;
  DDS::StatusCondition_ptr           condition_;
  // Add update datareader statuscondition to waitset
  condition_ = update_reader_->get_statuscondition ();
  condition_->set_enabled_statuses (DDS::DATA_AVAILABLE_STATUS);
  waitset_.attach_condition (condition_);

  ::DDS::Duration_t wait_time;
  wait_time.sec = 3;
  wait_time.nanosec = 0;

  // if we don't check originator for null, we get phantom sends
  // when the program exits.
  MADARA_DEBUG (MADARA_LOG_MAJOR_EVENT, (LM_DEBUG, 
    DLINFO "%s:" \
    " entering processing loop.\n", print_prefix));

  while (false == terminated_.value ())
  {
    // by using conditionals, we can wait for a message for a specific time limit
    // the conditionList would tell us which particular conditions were met, but
    // since we've only set up the wait
    DDS::ConditionSeq_var conditionList = new DDS::ConditionSeq();
    result = waitset_.wait (conditionList.inout (), wait_time);

    if (!is_ready_)
    {
      MADARA_DEBUG (MADARA_LOG_TRACE, (LM_DEBUG, 
        DLINFO "%s:" \
        " waking up anyone waiting for read thread.\n", print_prefix));

      is_ready_ = true;
      is_not_ready_.broadcast ();
    }
    //ACE_DEBUG ((LM_DEBUG, "(%P|%t) Read thread take.\n"));

    MADARA_DEBUG (MADARA_LOG_TRACE, (LM_DEBUG, 
      DLINFO "%s:" \
      " entering a take on the DDS reader.\n", print_prefix));

    dds_result = update_reader_->take (update_data_list_, infoList, 20, 
      DDS::ANY_SAMPLE_STATE, DDS::ANY_VIEW_STATE, DDS::ANY_INSTANCE_STATE);

    amount = update_data_list_->length ();
    
    //ACE_DEBUG ((LM_DEBUG, "(%P|%t) Returning from take with %d items.\n",  
    //        amount));

    if (amount != 0)
    {
      for (int i = 0; i < amount; ++i)
      {
        // if we are evaluating a message from ourselves, just continue
        // to the next one. It's also possible to receive null originators
        // from what I can only guess is the ospl daemon messing up
        if (!update_data_list_[i].originator.val ())
        {
          // if we don't check originator for null, we get phantom sends
          // when the program exits.
          MADARA_DEBUG (MADARA_LOG_EVENT_TRACE, (LM_DEBUG,
            DLINFO "%s:" \
            " discarding null originator event.\n", print_prefix));

          continue;
        }

        if (update_data_list_[i].type != Madara::Transport::MULTIASSIGN)
        {
          // we do not allow any other type than multiassign
          MADARA_DEBUG (MADARA_LOG_EVENT_TRACE, (LM_DEBUG,
            DLINFO "%s:" \
            " discarding non-assignment event.\n", print_prefix));

          continue;
        }

        Knowledge_Map rebroadcast_records;
        Message_Header * header = 0;

        process_received_update ((char *)update_data_list_[i].buffer.get_buffer (),
          update_data_list_[i].buffer.length (), id_, context_,
          *qos_settings_, send_monitor_, receive_monitor_, rebroadcast_records,
          on_data_received_, print_prefix,
          "", header);

        if (header)
        {
          if (header->ttl > 0 && rebroadcast_records.size () > 0 &&
              qos_settings_ && qos_settings_->get_participant_ttl () > 0)
          {
            --header->ttl;
            header->ttl = std::min (
              qos_settings_->get_participant_ttl (), header->ttl);

            rebroadcast (print_prefix, header, rebroadcast_records);
          }

          // delete header
          delete header;
        }
      
      }
    }

    dds_result = update_reader_->return_loan (update_data_list_, infoList);
  }

  enter_barrier ();
  return 0;
}
