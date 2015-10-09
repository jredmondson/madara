#ifndef AGENT_H
#define AGENT_H

#include "BrokerContext.h"
#include "ClientAcceptor.h"

namespace Madara
{
  int bindToFirstAvailableWorkerPort (ClientAcceptor & peer_acceptor,
                           BrokerContext & context);
  int bindToFirstAvailableBrokerPort (ClientAcceptor & peer_acceptor,
                           BrokerContext & context);
  int discoverWorkerPortOnHost(const std::string & host,
                           BrokerContext & context);
}

#endif