
#include "madara/knowledge/KnowledgeBase.h"
#include "madara/knowledge/KnowledgeBaseImpl.h"
#include "madara/logger/Logger.h"


#include <sstream>
#include <iostream>

#include "ace/OS_NS_Thread.h"
#include "ace/High_Res_Timer.h"
#include "ace/OS_NS_sys_socket.h"

namespace madara { namespace knowledge {

#ifndef _MADARA_NO_KARL_

KnowledgeRecord
KnowledgeBase::wait (
  CompiledExpression & expression,
  const WaitSettings & settings)
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

    ACE_Time_Value current = ACE_High_Res_Timer::gettimeofday ();
    ACE_Time_Value max_wait, sleep_time, next_epoch;
    ACE_Time_Value poll_frequency, last = current;

    if (settings.poll_frequency >= 0)
    {
      max_wait.set (settings.max_wait_time);
      max_wait = current + max_wait;

      poll_frequency.set (settings.poll_frequency);
      next_epoch = current + poll_frequency;
    }

    // print the post statement at highest log level (cannot be masked)
    if (settings.pre_print_statement != "")
      context_->print (settings.pre_print_statement, logger::LOG_ALWAYS);

    // lock the context
    context_->lock ();

    madara_logger_log (context_->get_logger (), logger::LOG_MAJOR,
      "KnowledgeBase::wait:" \
      " waiting on %s\n", expression.logic.c_str ());

    KnowledgeRecord last_value = expression.expression.evaluate (settings);

    madara_logger_log (context_->get_logger (), logger::LOG_DETAILED,
      "KnowledgeBase::wait:" \
      " completed first eval to get %s\n",
      last_value.to_string ().c_str ());

    send_modifieds ("KnowledgeBase:wait", settings);

    context_->unlock ();

    current = ACE_High_Res_Timer::gettimeofday ();

    // wait for expression to be true
    while (!last_value.to_integer () &&
      (settings.max_wait_time < 0 || current < max_wait))
    {
      madara_logger_log (context_->get_logger (), logger::LOG_DETAILED,
        "KnowledgeBase::wait:" \
        " current is %llu.%llu and max is %llu.%llu (poll freq is %f)\n",
        (unsigned long long)current.sec (),
        (unsigned long long)current.usec (),
        (unsigned long long)max_wait.sec (),
        (unsigned long long)max_wait.usec (),
        settings.poll_frequency);

      madara_logger_log (context_->get_logger (), logger::LOG_DETAILED,
        "KnowledgeBase::wait:" \
        " last value didn't result in success\n");

      // Unlike the other wait statements, we allow for a time based wait.
      // To do this, we allow a user to specify a
      if (settings.poll_frequency > 0)
      {
        if (current < next_epoch)
        {
          sleep_time = next_epoch - current;
          madara::utility::sleep (sleep_time);
        }

        next_epoch = next_epoch + poll_frequency;
      }
      else
        context_->wait_for_change (true);

      // relock - basically we need to evaluate the tree again, and
      // we can't have a bunch of people changing the variables as
      // while we're evaluating the tree.
      context_->lock ();


      madara_logger_log (context_->get_logger (), logger::LOG_MAJOR,
        "KnowledgeBase::wait:" \
        " waiting on %s\n", expression.logic.c_str ());

      last_value = expression.expression.evaluate (settings);

      madara_logger_log (context_->get_logger (), logger::LOG_DETAILED,
        "KnowledgeBase::wait:" \
        " completed eval to get %s\n",
        last_value.to_string ().c_str ());

      send_modifieds ("KnowledgeBase:wait", settings);

      context_->unlock ();
      context_->signal ();

      // get current time
      current = ACE_High_Res_Timer::gettimeofday ();

    } // end while (!last)

    if (current >= max_wait)
    {
      madara_logger_log (context_->get_logger (), logger::LOG_MAJOR,
        "KnowledgeBase::wait:" \
        " Evaluate did not succeed. Timeout occurred\n");
    }

    // print the post statement at highest log level (cannot be masked)
    if (settings.post_print_statement != "")
      context_->print (settings.post_print_statement, logger::LOG_ALWAYS);

    return last_value;
  }
  else if (impl_.get ())
  {
    result = impl_->wait (expression, settings);
  }

  return result;
}
#endif // _MADARA_NO_KARL_

} }
