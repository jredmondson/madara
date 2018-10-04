#include "PacketScheduler.h"
#include "madara/utility/Utility.h"
#include "madara/logger/GlobalLogger.h"

const uint64_t max_stride(150000000);

/**
 * <b>INTERNAL USE</b>: Task that can be added to a Stride scheduler
 **/
class StrideTask
{
public:
  /**
   * Constructor
   **/
  StrideTask(double rate, bool op) : operation(op)
  {
    set_rate(rate);
  }
  /**
   * Checks for lower pass value
   **/
  bool operator<(const StrideTask& rhs) const
  {
    return pass > rhs.pass;
  }

  /**
   * Checks for pass equality
   **/
  bool operator==(const StrideTask& rhs) const
  {
    return pass == rhs.pass;
  }

  /**
   * Checks for greater pass value
   **/
  bool operator>(const StrideTask& rhs) const
  {
    return pass < rhs.pass;
  }

  /**
   * Increases pass by stride
   **/
  void operator++(void)
  {
    pass += stride;
  }

  /**
   * Sets the ticket rate, which influences stride
   **/
  void set_rate(double rate)
  {
    tickets = (uint64_t)(1000000 * rate);

    if (tickets > 0)
      stride = max_stride / tickets;
    else
      stride = 1;

    pass = stride;
  }

  /**
   * the stride to take
   **/
  uint64_t stride;

  /**
   * the current pass which determines next schedule
   **/
  uint64_t pass;

  /**
   * the number of tickets, which influences stride
   **/
  uint64_t tickets;

  /**
   * the type of operation
   **/
  bool operation;
};

madara::transport::PacketScheduler::PacketScheduler(
    const QoSTransportSettings* settings)
  : settings_(settings),
    sent_messages_(0),
    dropped_messages_(0),
    consecutive_drops_(0)
{
}

madara::transport::PacketScheduler::PacketScheduler(const PacketScheduler& rhs)
  : settings_(rhs.settings_),
    sent_messages_(rhs.sent_messages_),
    dropped_messages_(rhs.dropped_messages_),
    consecutive_drops_(rhs.consecutive_drops_)
{
}

madara::transport::PacketScheduler::~PacketScheduler() {}

void madara::transport::PacketScheduler::operator=(const PacketScheduler& rhs)
{
  MADARA_GUARD_TYPE guard(mutex_);
  if (this != &rhs)
  {
    settings_ = rhs.settings_;
    sent_messages_ = rhs.sent_messages_;
    dropped_messages_ = rhs.dropped_messages_;
    consecutive_drops_ = rhs.consecutive_drops_;
  }
}

bool madara::transport::PacketScheduler::add(void)
{
  MADARA_GUARD_TYPE guard(mutex_);

  if (settings_)
  {
    /**
     * call the accessor functions only once
     **/
    double drop_rate = settings_->get_drop_rate();
    int drop_type = settings_->get_drop_type();
    uint64_t drop_burst = settings_->get_drop_burst();

    bool result = true;

    // if the user has specified a positive drop rate
    if (drop_rate > 0)
    {
      /**
       * if the drop rate is greater than 100% or
       * drop burst is greater than 1 and we do not have that many
       * consecutive drops
       **/
      if (drop_rate >= 100 || (consecutive_drops_ > 0 && drop_burst > 1 &&
                                  consecutive_drops_ < drop_burst))
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
          if (utility::rand_double() <= drop_rate)
          {
            result = false;
          }
        }
        else
        {
          // Reset queue if this is the first time we have used this policy
          if (queue_.empty())
          {
            reset();
          }

          // remove current operation from the priority queue
          StrideTask current = queue_.top();
          queue_.pop();

          // set the result to the current operation
          result = current.operation;

          // reinsert the operation into the queue
          ++current;
          queue_.push(current);
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
  }  // end if settings_
  else
  {
    madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_EMERGENCY,
        "PacketScheduler::add: ERROR: Invalid settings class\n");

    return true;
  }
}

void madara::transport::PacketScheduler::attach(
    const QoSTransportSettings* settings)
{
  settings_ = settings;
}

void madara::transport::PacketScheduler::clear(void)
{
  MADARA_GUARD_TYPE guard(mutex_);

  sent_messages_ = 0;
  dropped_messages_ = 0;
  consecutive_drops_ = 0;
  while (!queue_.empty())
    queue_.pop();
}

void madara::transport::PacketScheduler::reset(void)
{
  MADARA_GUARD_TYPE guard(mutex_);

  if (settings_)
  {
    double drop_rate = settings_->get_drop_rate();
    uint64_t burst = settings_->get_drop_burst();

    // if burst is greater than 1, then divide the rate by burst
    if (burst > 1)
      drop_rate = drop_rate / (burst - 1);

    StrideTask drop_message(drop_rate, false);
    StrideTask send_message(1 - drop_rate, true);

    while (!queue_.empty())
      queue_.pop();

    queue_.push(drop_message);
    queue_.push(send_message);
  }
  else
  {
    madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_EMERGENCY,
        "PacketScheduler::reset: ERROR: Invalid settings class\n");
  }
}

void madara::transport::PacketScheduler::print_status(
    unsigned int log_level, const char* prefix)
{
  MADARA_GUARD_TYPE guard(mutex_);

  madara_logger_ptr_log(logger::global_logger.get(), (int)log_level,
      "%s: %d sent, %d dropped, %d consec dropped\n", prefix, sent_messages_,
      dropped_messages_, consecutive_drops_);
}

uint64_t madara::transport::PacketScheduler::get_dropped(void)
{
  MADARA_GUARD_TYPE guard(mutex_);
  return dropped_messages_;
}

uint64_t madara::transport::PacketScheduler::get_sent(void)
{
  MADARA_GUARD_TYPE guard(mutex_);
  return sent_messages_;
}
