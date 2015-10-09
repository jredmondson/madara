#ifndef TIMEOUT_HANDLER_H
#define TIMEOUT_HANDLER_H

#include "ace/Event_Handler.h"
#include "BrokerContext.h"

class BrokerTimeoutHandler: public ACE_Event_Handler
{
public:
  BrokerTimeoutHandler (Madara::BrokerContext& context);
  virtual int handle_timeout (const ACE_Time_Value &tv, const void *arg);
private:
  int check_key (const std::string &key);
  int random_latency_query (void);
  int random_discover (void);
  int dump_latencies (void);

  Madara::BrokerContext & context_;
};

#endif // TIMEOUT_HANDLER_H