#ifndef QUERY_LATENCIES_H
#define QUERY_LATENCIES_H

#include <string>

#include "BrokerContext.h"

namespace Madara
{
  int query_latencies_by_key (
    Madara::BrokerContext& context, std::string key);
}

#endif