#ifndef QUERY_LATENCIES_H
#define QUERY_LATENCIES_H

#include <string>

#include "Broker_Context.h"

namespace Madara
{
  int query_latencies_by_key (
    Madara::Broker_Context& context, std::string key);
}

#endif