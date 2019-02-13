
#include "madara/knowledge/ContextGuard.h"
#include "madara/utility/Utility.h"
#include "SharedMemoryPush.h"

madara::transport::SharedMemoryPush::SharedMemoryPush (
  const std::string & id,
  madara::transport::TransportSettings & new_settings,
  knowledge::KnowledgeBase & knowledge)
: madara::transport::Base (id, new_settings, knowledge.get_context ())
{
  // populate variables like buffer_ based on transport settings
  Base::setup ();
}

madara::transport::SharedMemoryPush::~SharedMemoryPush ()
{
}

long
madara::transport::SharedMemoryPush::send_data (
  const madara::knowledge::KnowledgeMap & modifieds)
{
  /**
   * Return number of bytes sent or negative for error
   **/
  long result (0);

  const char * print_prefix = "madara::transport::SharedMemoryPush";
  
  // copy the cosnt modifieds list to a nonconst version for filtering
  knowledge::KnowledgeMap filtered_updates = modifieds;

  transport::TransportContext send_context(
      transport::TransportContext::SENDING_OPERATION,
      receive_monitor_.get_bytes_per_second(),
      send_monitor_.get_bytes_per_second(), (uint64_t)utility::get_time(),
      (uint64_t)utility::get_time(), settings_.write_domain, id_);

  /**
   * 1. Apply send filters to a master list that will be applied to all kbs
   **/

  if(settings_.get_number_of_send_aggregate_filters() > 0 &&
      filtered_updates.size() > 0)
  {
    settings_.filter_send(filtered_updates, send_context);
  }
  else
  {
    madara_logger_log(context_.get_logger(), logger::LOG_MINOR,
        "%s:"
        " No aggregate send filters were applied...\n",
        print_prefix);
  }

  /**
   * 2. Apply receive filters to each kb transfer
   **/

  if(settings_.get_number_of_receive_aggregate_filters() > 0 &&
      filtered_updates.size() > 0)
  {
    settings_.filter_receive(filtered_updates, send_context);
  }
  else
  {
    madara_logger_log(context_.get_logger(), logger::LOG_MINOR,
        "%s:"
        " No aggregate receive filters were applied...\n",
        print_prefix);
  }

  /**
   * 3. Update kbs
   **/
  for (auto kb : kbs_)
  {
    // try not to update ourself
    if (id_ == kb.get_id())
    {
      continue;
    }

    knowledge::ContextGuard guard(kb);
    knowledge::ThreadSafeContext & context = kb.get_context();

    madara_logger_log(context_.get_logger(), logger::LOG_MINOR,
        "%s:"
        " Applying updates to context.\n",
        print_prefix);

    uint64_t now = utility::get_time();
    
    for(auto i : filtered_updates)
    {
      const auto apply = [&](knowledge::KnowledgeRecord& record) {
        int result = 0;

        record.set_toi(now);
        result = record.apply(
            context, i.first, i.second.quality, i.second.clock, false);

        if(result != 1)
        {
          madara_logger_log(context.get_logger(), logger::LOG_MAJOR,
              "%s:"
              " update %s=%s was rejected\n",
              print_prefix, i.first.c_str(), record.to_string().c_str());
        }
        else
        {
          madara_logger_log(context.get_logger(), logger::LOG_MAJOR,
              "%s:"
              " update %s=%s was accepted\n",
              print_prefix, i.first.c_str(), record.to_string().c_str());
        }
      };

      apply(i.second);
    }
  }

  return result;
}
