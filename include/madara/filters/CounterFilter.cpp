#include "CounterFilter.h"
#include "madara/utility/Utility.h"

namespace utility = madara::utility;
typedef madara::knowledge::KnowledgeRecord::Integer   Integer;

madara::filters::CounterFilter::CounterFilter ()
  : initialized_ (false),
  packets_ (0), first_message_ (-1), last_message_ (-1)
{
}

madara::filters::CounterFilter::~CounterFilter ()
{
}


void
madara::filters::CounterFilter::filter (
  knowledge::KnowledgeMap & /*records*/,
  const transport::TransportContext & transport_context,
  knowledge::Variables & /*vars*/)
{
  Integer cur_time = (Integer) transport_context.get_current_time ();

  if (!initialized_)
  {
    this->initialized_ = true;
    this->first_message_ = cur_time;
  }

  ++this->packets_;
  this->last_message_ = cur_time;
}
