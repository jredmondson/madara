#ifndef   _TRANSPORT_INL_
#define   _TRANSPORT_INL_

#include "Transport.h"

inline int Madara::Transport::Base::validate_transport (void) 
{ 
  is_valid_ = true; 
  shutting_down_ = false;
  valid_setup_.broadcast ();

  context_.get_logger ().log (Logger::LOG_MINOR,
    "Transport::validate_transport: transport is ready\n");

  return 0;
}

inline int
Madara::Transport::Base::check_transport (void) 
{
  context_.get_logger ().log (Logger::LOG_DETAILED,
    "Transport::check_transport: checking for valid transport\n");

  if (!is_valid_)
    return -2;

  if (shutting_down_)
    return -1;

  return 0;
}

inline void
Madara::Transport::Base::invalidate_transport (void)
{
  is_valid_ = false;
  shutting_down_ = true;
  valid_setup_.broadcast ();

  context_.get_logger ().log (Logger::LOG_DETAILED,
    "Transport::invalidate_transport: invalidating transporty\n");
}

inline Madara::Transport::Settings &
Madara::Transport::Base::settings (void)
{
  return settings_;
}

#endif
