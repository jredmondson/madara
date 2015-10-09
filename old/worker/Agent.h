#ifndef AGENT_H
#define AGENT_H

#include "AgentContext.h"
#include "ClientAcceptor.h"

namespace Madara
{
  int bindToFirstAvailableWorkerPort (ClientAcceptor & peer_acceptor,
                           AgentContext & context);
  int bindToFirstAvailableBrokerPort (ClientAcceptor & peer_acceptor,
                           AgentContext & context);
  int discoverWorkerPortOnHost(const std::string & host,
                           AgentContext & context);
}

#endif