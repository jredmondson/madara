#ifndef   _TRANSPORT_INL_
#define   _TRANSPORT_INL_

#include "Transport.h"

inline int madara::transport::Base::validate_transport (void) 
{ 
  is_valid_ = true; 
  shutting_down_ = false;
  valid_setup_.broadcast ();

  madara_logger_log (context_.get_logger (), logger::LOG_MINOR,
    "transport::validate_transport: transport is ready\n");

  return 0;
}

inline int
madara::transport::Base::check_transport (void) 
{
  madara_logger_log (context_.get_logger (), logger::LOG_DETAILED,
    "transport::check_transport: checking for valid transport\n");

  if (!is_valid_)
    return -2;

  if (shutting_down_)
    return -1;

  return 0;
}

inline void
madara::transport::Base::invalidate_transport (void)
{
  is_valid_ = false;
  shutting_down_ = true;
  valid_setup_.broadcast ();

  madara_logger_log (context_.get_logger (), logger::LOG_DETAILED,
    "transport::invalidate_transport: invalidating transport\n");
}

inline madara::transport::TransportSettings &
madara::transport::Base::settings (void)
{
  return settings_;
}

#endif
