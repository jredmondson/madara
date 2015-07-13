#include "Counter_Filter.h"
#include "madara/utility/Utility.h"

namespace utility = Madara::Utility;
typedef Madara::Knowledge_Record::Integer   Integer;

Madara::Filters::Counter_Filter::Counter_Filter ()
  : initialized_ (false),
  packets_ (0), first_message_ (-1), last_message_ (-1)
{
}

Madara::Filters::Counter_Filter::~Counter_Filter ()
{
}


void
Madara::Filters::Counter_Filter::filter (
  Knowledge_Map & records,
  const Transport::Transport_Context & transport_context,
  Knowledge_Engine::Variables & vars)
{
  Integer cur_time = Utility::get_time ();

  if (!initialized_)
  {
    this->initialized_ = true;
    this->first_message_ = cur_time;
  }

  ++this->packets_;
  this->last_message_ = cur_time;
}
