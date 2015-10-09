
// C++
#include <stdlib.h>

// ACE
#include "ace/OS.h"
#include "ace/Log_Msg.h"
#include "ace/SOCK_Connector.h"

// Madara
#include "MadaraCommon.h"
#include "AgentMessages.h"
#include "Agent.h"
#include "QueryLatencies.h"

// Handler
#include "BrokerTimeoutHandler.h"

BrokerTimeoutHandler::BrokerTimeoutHandler (Madara::BrokerContext & context)
: context_ (context)
{
  
}


int 
BrokerTimeoutHandler::check_key (const std::string &key)
{
  std::string this_key;

  Madara::merge_key (this_key, context_.getHost (), context_.getPort ());

  return this_key != key;
}


int 
BrokerTimeoutHandler::random_discover (void)
{
  std::string host, port, key;
  int num_keys = context_.getNumKeys ();

  //if (num_keys <= 0)
  //  return -1;

  int cur_rand = rand () % (num_keys + 1);

  if (cur_rand == num_keys)
    key = "127.0.0.1";
  else
    key = context_.getKey ((unsigned int)cur_rand);

  Madara::split_key (key, host, port);

  ACE_DEBUG ((LM_DEBUG, "(%P|%t) Attempting to discover an agent on %s\n", 
    host.c_str ()));

  return Madara::discoverWorkerPortOnHost (host, context_);
}

int 
BrokerTimeoutHandler::random_latency_query (void)
{
  int num_keys = context_.getNumKeys ();

  if (num_keys <= 0)
    return -1;

  int cur_rand = rand () % num_keys;

  std::string key = context_.getKey ((unsigned int)cur_rand);

  // we don't want to send messages to ourself
  if (!check_key (key))
    return -1;

  ACE_DEBUG ((LM_DEBUG, "(%P|%t) Querying %s! %d of %d\n", key.c_str (), 
    cur_rand, num_keys));

  // send a message to the person @ key
  Madara::query_latencies_by_key (context_, key);

  return 0;
}

int
BrokerTimeoutHandler::dump_latencies (void)
{
  context_.write (std::cerr);

  return 0;
}

int 
BrokerTimeoutHandler::handle_timeout (const ACE_Time_Value &tv, const void *arg)
{
  // The agent will spend 90% of timeouts pinging other agents.
  // It will spend 10% of timeouts attempting to discover.

  int decision = 0;

  // randomize with current time
  srand (time (NULL));

  ACE_DEBUG ((LM_DEBUG, "(%P|%t) handled a timeout!\n"));

  decision = rand () % 10;

  switch (decision)
    {
    case 0:
      random_discover ();
      break;
    case 3:
    case 4:
    case 5:
      random_latency_query ();
      break;
    case 6:
    case 7:
      dump_latencies ();
      break;
    default:
      //random_ping ();
      break;
    }

  return 0;
}
