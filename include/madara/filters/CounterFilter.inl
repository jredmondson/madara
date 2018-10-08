#ifndef _MADARA_FILTERS_COUNTER_FILTER_INL_
#define _MADARA_FILTERS_COUNTER_FILTER_INL_

#include "CounterFilter.h"

madara::knowledge::KnowledgeRecord::Integer
madara::filters::CounterFilter::get_count()
{
  return packets_;
}

madara::knowledge::KnowledgeRecord::Integer
madara::filters::CounterFilter::get_elapsed()
{
  madara::knowledge::KnowledgeRecord::Integer result(0);

  if (initialized_)
  {
    result = last_message_ - first_message_;
  }

  return result;
}

double madara::filters::CounterFilter::get_throughput()
{
  double result(0);

  if (initialized_)
  {
    // convert elapsed time into seconds
    double elapsed = (double)(last_message_ - first_message_);
    // elapsed *= 0.000000001;

    // divide packets by time elapsed in seconds
    result = packets_ / elapsed;
  }

  return result;
}

#endif  // _MADARA_FILTERS_COUNTER_FILTER_INL_
