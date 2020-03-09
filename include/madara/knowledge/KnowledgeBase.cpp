
#include "madara/knowledge/KnowledgeBase.h"
#include "madara/knowledge/KnowledgeBaseImpl.h"
#include "madara/logger/Logger.h"
#include "madara/utility/Utility.h"
#include "ContextGuard.h"
#include "madara/utility/EpochEnforcer.h"

#include <sstream>
#include <iostream>

namespace utility = madara::utility;

typedef utility::EpochEnforcer<std::chrono::steady_clock> EpochEnforcer;

namespace madara
{
namespace knowledge
{
bool KnowledgeBase::has_history(
    const VariableReference& key,
	const KnowledgeReferenceSettings& settings) const
{
  if (impl_.get())
  {
    ContextGuard guard(this->get_context());
    return key.get_record_unsafe()->has_history();
  }
  else if (context_)
  {
    ContextGuard guard(*context_);
    return key.get_record_unsafe()->has_history();
  }
  throw_null_context();
}

size_t KnowledgeBase::get_history_size(const VariableReference& key,
    const KnowledgeReferenceSettings& settings) const
{
  if (impl_.get())
  {
    ContextGuard guard(this->get_context());
    return key.get_record_unsafe()->get_history_size();
  }
  else if (context_)
  {
    ContextGuard guard(*context_);
    return key.get_record_unsafe()->get_history_size();
  }
  throw_null_context();
}

size_t KnowledgeBase::get_history_capacity(const VariableReference& key,
    const KnowledgeReferenceSettings& settings) const
{
  if (impl_.get())
  {
    ContextGuard guard(this->get_context());
    return key.get_record_unsafe()->get_history_capacity();
  }
  else if (context_)
  {
    ContextGuard guard(*context_);
    return key.get_record_unsafe()->get_history_capacity();
  }
  throw_null_context();
}

void KnowledgeBase::set_history_capacity(
    const VariableReference& key, size_t size, const EvalSettings& settings)
{
  if (impl_.get())
  {
    ContextGuard guard(this->get_context());
    return key.get_record_unsafe()->set_history_capacity(size);
  }
  else if (context_)
  {
    ContextGuard guard(*context_);
    return key.get_record_unsafe()->set_history_capacity(size);
  }
  throw_null_context();
}

void KnowledgeBase::clear_history(
	const VariableReference& key, const EvalSettings& settings)
{
  if (impl_.get())
  {
    ContextGuard guard(this->get_context());
    return key.get_record_unsafe()->clear_history();
  }
  else if (context_)
  {
    ContextGuard guard(*context_);
    return key.get_record_unsafe()->clear_history();
  }
  throw_null_context();
}

KnowledgeRecord KnowledgeBase::get_oldest(const VariableReference& key,
    const KnowledgeReferenceSettings& settings) const
{
  if (impl_.get())
  {
    ContextGuard guard(this->get_context());
    return key.get_record_unsafe()->get_oldest();
  }
  else if (context_)
  {
    ContextGuard guard(*context_);
    return key.get_record_unsafe()->get_oldest();
  }
  throw_null_context();
}

KnowledgeRecord KnowledgeBase::get_newest(const VariableReference& key,
    const KnowledgeReferenceSettings& settings) const
{
  if (impl_.get())
  {
    ContextGuard guard(this->get_context());
    return key.get_record_unsafe()->get_newest();
  }
  else if (context_)
  {
    ContextGuard guard(*context_);
    return key.get_record_unsafe()->get_newest();
  }
  throw_null_context();
}

std::vector<KnowledgeRecord> KnowledgeBase::get_oldest(
    const VariableReference& key, size_t count,
    const KnowledgeReferenceSettings& settings) const
{
  if (impl_.get())
  {
    ContextGuard guard(this->get_context());
    return key.get_record_unsafe()->get_oldest(count);
  }
  else if (context_)
  {
    ContextGuard guard(*context_);
    return key.get_record_unsafe()->get_oldest(count);
  }
  throw_null_context();
}

std::vector<KnowledgeRecord> KnowledgeBase::get_newest(
    const VariableReference& key, size_t count,
    const KnowledgeReferenceSettings& settings) const
{
  if (impl_.get())
  {
    ContextGuard guard(this->get_context());
    return key.get_record_unsafe()->get_newest(count);
  }
  else if (context_)
  {
    ContextGuard guard(*context_);
    return key.get_record_unsafe()->get_newest(count);
  }
  throw_null_context();
}

std::vector<KnowledgeRecord> KnowledgeBase::get_history(
    const VariableReference& key,
	const KnowledgeReferenceSettings& settings) const
{
  if (impl_.get())
  {
    ContextGuard guard(this->get_context());
    return key.get_record_unsafe()->get_history();
  }
  else if (context_)
  {
    ContextGuard guard(*context_);
    return key.get_record_unsafe()->get_history();
  }
  throw_null_context();
}

std::vector<KnowledgeRecord> KnowledgeBase::get_history(
    const VariableReference& key, size_t index, size_t count,
    const KnowledgeReferenceSettings& settings) const
{
  if (impl_.get())
  {
    ContextGuard guard(this->get_context());
    return key.get_record_unsafe()->get_history(index, count);
  }
  else if (context_)
  {
    ContextGuard guard(*context_);
    return key.get_record_unsafe()->get_history(index, count);
  }
  throw_null_context();
}

KnowledgeRecord KnowledgeBase::get_history(const VariableReference& key,
    size_t index, const KnowledgeReferenceSettings& settings) const
{
  if (impl_.get())
  {
    ContextGuard guard(this->get_context());
    return key.get_record_unsafe()->get_history(index);
  }
  else if (context_)
  {
    ContextGuard guard(*context_);
    return key.get_record_unsafe()->get_history(index);
  }
  throw_null_context();
}
  
#ifndef _MADARA_NO_KARL_
	
KnowledgeRecord
KnowledgeBase::wait(
    CompiledExpression& expression, const WaitSettings& settings)
{
  KnowledgeRecord result;

  if (context_)
  {
    /**
     * The only situation this can be useful will be if the thread safe
     * context is being used as a shared memory structure between
     * threads. This should not be used for processes communicating
     * together because the wait statement is unable to send modifieds
     * as it has zero concept of transports. The type of knowledge
     * base handled here is a facade for another knowledge base's
     * context.
     **/

    EpochEnforcer enforcer(settings.poll_frequency, settings.max_wait_time);

    KnowledgeRecord last_value;

    // print the post statement at highest log level (cannot be masked)
    if (settings.pre_print_statement != "")
      context_->print(settings.pre_print_statement, logger::LOG_ALWAYS);

    {
      ContextGuard context_guard(*context_);

      madara_logger_log(context_->get_logger(), logger::LOG_MAJOR,
          "KnowledgeBase::wait:"
          " waiting on %s\n",
          expression.logic.c_str());

      last_value = expression.expression.evaluate(settings);

      madara_logger_log(context_->get_logger(), logger::LOG_DETAILED,
          "KnowledgeBase::wait:"
          " completed first eval to get %s\n",
          last_value.to_string().c_str());

      send_modifieds("KnowledgeBase:wait", settings);
    }

    // wait for expression to be true
    while (!last_value.to_integer() &&
           (settings.max_wait_time < 0 || !enforcer.is_done()))
    {
      madara_logger_log(context_->get_logger(), logger::LOG_DETAILED,
          "KnowledgeBase::wait:"
          " last value didn't result in success\n");

      // Unlike the other wait statements, we allow for a time based wait.
      // To do this, we allow a user to specify a
      if (settings.poll_frequency > 0)
      {
        enforcer.sleep_until_next();
      }
      else
        context_->wait_for_change(true);

      // relock - basically we need to evaluate the tree again, and
      // we can't have a bunch of people changing the variables as
      // while we're evaluating the tree.
      {
        ContextGuard context_guard(*context_);

        madara_logger_log(context_->get_logger(), logger::LOG_MAJOR,
            "KnowledgeBase::wait:"
            " waiting on %s\n",
            expression.logic.c_str());

        last_value = expression.expression.evaluate(settings);

        madara_logger_log(context_->get_logger(), logger::LOG_DETAILED,
            "KnowledgeBase::wait:"
            " completed eval to get %s\n",
            last_value.to_string().c_str());

        send_modifieds("KnowledgeBase:wait", settings);
      }

      context_->signal();
    }  // end while (!last)

    if (enforcer.is_done())
    {
      madara_logger_log(context_->get_logger(), logger::LOG_MAJOR,
          "KnowledgeBase::wait:"
          " Evaluate did not succeed. Timeout occurred\n");
    }

    // print the post statement at highest log level (cannot be masked)
    if (settings.post_print_statement != "")
      context_->print(settings.post_print_statement, logger::LOG_ALWAYS);

    return last_value;
  }
  else if (impl_.get())
  {
    result = impl_->wait(expression, settings);
  }

  return result;
}
#endif  // _MADARA_NO_KARL_
}
}  // namespace madara
