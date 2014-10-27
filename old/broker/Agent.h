#ifndef AGENT_H
#define AGENT_H

#include "Broker_Context.h"
#include "Client_Acceptor.h"

namespace Madara
{
  int bindToFirstAvailableWorkerPort (Client_Acceptor & peer_acceptor,
                           Broker_Context & context);
  int bindToFirstAvailableBrokerPort (Client_Acceptor & peer_acceptor,
                           Broker_Context & context);
  int discoverWorkerPortOnHost(const std::string & host,
                           Broker_Context & context);
}

#endif