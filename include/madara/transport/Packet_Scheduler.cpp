#include "Packet_Scheduler.h"
#include "madara/utility/Utility.h"
#include "madara/logger/Global_Logger.h"


const uint64_t max_stride (150000000);

/**
 * 
 **/
class Stride_Task
{
public:
  Stride_Task (double rate, bool op)
    : operation (op)
  {
    set_rate (rate);
  }
  
  bool operator< (const Stride_Task & rhs) const
  {
    return pass > rhs.pass;
  }

  bool operator== (const Stride_Task & rhs) const
  {
    return pass == rhs.pass;
  }

  bool operator> (const Stride_Task & rhs) const
  {
    return pass < rhs.pass;
  }

  void operator++ (void)
  {
    pass += stride;
  }

  void set_rate (double rate)
  {
    tickets = 1000000 * rate;

    if (tickets > 0)
      stride = max_stride / tickets;
    else
      stride = 1;

    pass = stride;
  }

  uint64_t stride;
  uint64_t pass;
  uint64_t tickets;
  bool operation;
};

Madara::Transport::Packet_Scheduler::Packet_Scheduler (
  const QoS_Transport_Settings * settings)
  : settings_ (settings),
    sent_messages_ (0),
    dropped_messages_ (0),
    consecutive_drops_ (0)
{
}

Madara::Transport::Packet_Scheduler::Packet_Scheduler (
  const Packet_Scheduler & rhs)
  : settings_ (rhs.settings_),
  sent_messages_ (rhs.sent_messages_),
  dropped_messages_ (rhs.dropped_messages_),
  consecutive_drops_ (rhs.consecutive_drops_)
{
}

Madara::Transport::Packet_Scheduler::~Packet_Scheduler ()
{
}

void
Madara::Transport::Packet_Scheduler::operator= (
  const Packet_Scheduler & rhs)
{
  Scheduler_Guard guard (mutex_);
  if (this != &rhs)
  {
    settings_ = rhs.settings_;
    sent_messages_ = rhs.sent_messages_;
    dropped_messages_ = rhs.dropped_messages_;
    consecutive_drops_ = rhs.consecutive_drops_;
  }
}

bool
Madara::Transport::Packet_Scheduler::add (void)
{
  Scheduler_Guard guard (mutex_);
  
  if (settings_)
  {
    /**
     * call the accessor functions only once
     **/
    double drop_rate = settings_->get_drop_rate ();
    int drop_type = settings_->get_drop_type ();
    uint64_t drop_burst = settings_->get_drop_burst ();

    bool result = true;

    // if the user has specified a positive drop rate
    if (drop_rate > 0)
    {
      // compute the actual drop rate
      double actual_drop_rate = 0;
      if (dropped_messages_ + sent_messages_ > 0)
      {
        actual_drop_rate = (double)dropped_messages_ /
          (dropped_messages_ + sent_messages_);
      }

      /**
       * if the drop rate is greater than 100% or
       * drop burst is greater than 1 and we do not have that many
       * consecutive drops
       **/
      if (drop_rate >= 100 ||
          (consecutive_drops_ > 0 &&
           drop_burst > 1 && consecutive_drops_ < drop_burst))
      {
        result = false;
      }
      else
      {
        if (drop_type == PACKET_DROP_PROBABLISTIC)
        {
          // if burst is greater than 1, then divide the rate by burst
          if (drop_burst > 1)
            drop_rate = drop_rate / (drop_burst - 1);

          // easiest drop rate policy. Just drop messages with a probability.
          if (Utility::rand_double () <= drop_rate)
          {
            result = false;
          }
        }
        else
        {
          // Reset queue if this is the first time we have used this policy
          if (queue_.empty ())
          {
            reset ();
          }

          // remove current operation from the priority queue
          Stride_Task current = queue_.top ();
          queue_.pop ();

          // set the result to the current operation
          result = current.operation;

          // reinsert the operation into the queue
          ++current;
          queue_.push (current);
        }
      }
    }

    if (result)
    {
      ++sent_messages_;
      consecutive_drops_ = 0;
    }
    else
    {
      ++dropped_messages_;
      ++consecutive_drops_;
    }

    return result;
  } // end if settings_
  else
  {
    madara_logger_ptr_log (Logger::global_logger.get(), Logger::LOG_EMERGENCY,
      "Packet_Scheduler::add: ERROR: Invalid settings class\n");

    return true;
  }
}

void
Madara::Transport::Packet_Scheduler::attach (
  const QoS_Transport_Settings * settings)
{
  settings_ = settings;
}

void
Madara::Transport::Packet_Scheduler::clear (void)
{
  Scheduler_Guard guard (mutex_);
  
  sent_messages_ = 0;
  dropped_messages_ = 0;
  consecutive_drops_ = 0;
  while (!queue_.empty ())
    queue_.pop ();
}

void
Madara::Transport::Packet_Scheduler::reset (void)
{
  Scheduler_Guard guard (mutex_);
  
  if (settings_)
  {
    double drop_rate = settings_->get_drop_rate ();
    uint64_t burst = settings_->get_drop_burst ();

    // if burst is greater than 1, then divide the rate by burst
    if (burst > 1)
      drop_rate = drop_rate / (burst - 1);

    Stride_Task drop_message (drop_rate, false);
    Stride_Task send_message (1 - drop_rate, true);

    while (!queue_.empty ())
      queue_.pop ();

    queue_.push (drop_message);
    queue_.push (send_message);
  }
  else
  {
    madara_logger_ptr_log (Logger::global_logger.get(), Logger::LOG_EMERGENCY,
      "Packet_Scheduler::reset: ERROR: Invalid settings class\n");
  }
}

void
Madara::Transport::Packet_Scheduler::print_status (
  unsigned int log_level,
  const char * prefix)
{
  Scheduler_Guard guard (mutex_);

  madara_logger_ptr_log (Logger::global_logger.get(), Logger::LOG_MINOR,
    "%s: %d sent, %d dropped, %d consec dropped\n",
    prefix, sent_messages_, dropped_messages_, consecutive_drops_);
}

uint64_t
Madara::Transport::Packet_Scheduler::get_dropped (void)
{
  Scheduler_Guard guard (mutex_);
  return dropped_messages_;
}
      
uint64_t
Madara::Transport::Packet_Scheduler::get_sent (void)
{
  Scheduler_Guard guard (mutex_);
  return sent_messages_;
}
