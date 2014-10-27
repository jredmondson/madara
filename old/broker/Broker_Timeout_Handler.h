#ifndef TIMEOUT_HANDLER_H
#define TIMEOUT_HANDLER_H

#include "ace/Event_Handler.h"
#include "Broker_Context.h"

class Broker_Timeout_Handler: public ACE_Event_Handler
{
public:
  Broker_Timeout_Handler (Madara::Broker_Context& context);
  virtual int handle_timeout (const ACE_Time_Value &tv, const void *arg);
private:
  int check_key (const std::string &key);
  int random_latency_query (void);
  int random_discover (void);
  int dump_latencies (void);

  Madara::Broker_Context & context_;
};

#endif // TIMEOUT_HANDLER_H