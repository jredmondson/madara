#ifndef TIMEOUT_HANDLER_H
#define TIMEOUT_HANDLER_H

#include "ace/Event_Handler.h"
#include "AgentContext.h"

class TimeoutHandler: public ACE_Event_Handler
{
public:
  TimeoutHandler (Madara::AgentContext& context);
  virtual int handle_timeout (const ACE_Time_Value &tv, const void *arg);
private:
  int check_key (const std::string &key);
  int random_ping (void);
  int random_discover (void);
  int send_ping (const std::string &key);
  Madara::AgentContext & context_;
};

#endif // TIMEOUT_HANDLER_H