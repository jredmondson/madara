
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

// Handler
#include "TimeoutHandler.h"

TimeoutHandler::TimeoutHandler (Madara::AgentContext & context)
: context_ (context)
{
  
}

int 
TimeoutHandler::send_ping (const std::string & key)
{
  // necessary variables
  std::string target_host;
  std::string target_port;
  u_short u_port;

  Madara::AgentPing ping;
  Madara::AgentPing reply;

  ACE_SOCK_Stream server;
  ACE_SOCK_Connector connector;

  // split the key into host and port and setup the ACE_INET_Addr
  Madara::split_key (key, target_host, target_port);
  u_port = ACE_OS::atoi (target_port.c_str ());
  ACE_INET_Addr addr (u_port, target_host.c_str ());

  // if we are able to connect
  if (connector.connect (server, addr) != -1)
    {
      Madara::fillPingFromContext (ping, context_);

          
      // keep track of time
      ACE_hrtime_t start = ACE_OS::gethrtime ();

      if (server.send_n ( (void *)&ping, sizeof (ping) ) > 0)
        {
          // time we are willing to wait for a reply (1 ms)
          ACE_Time_Value timeout (0,5000);

          size_t ret = server.recv ( (void *)&reply, sizeof (reply), &timeout);

          if (ret > 0)
            {
              // keep track of time
              ACE_hrtime_t end = ACE_OS::gethrtime ();

              // so far so good. reply to them so they can establish a full
              // round trip latency
              server.send_n ( (void *)&ping, sizeof (ping) );

                          
              ACE_hrtime_t total = end - start;

              unsigned int milliseconds = total / 1000;

              this->context_.addLatency (key, milliseconds);

              ACE_DEBUG ((LM_DEBUG, 
                "(%P|%t) Latency round complete (%d) with client %s@%d\n", 
                milliseconds, reply.host, reply.port));

            }
        }
      server.close ();
    }

  return 0;
}


int 
TimeoutHandler::check_key (const std::string &key)
{
  std::string this_key;

  Madara::merge_key (this_key, context_.getHost (), context_.getPort ());

  return this_key != key;
}


int 
TimeoutHandler::random_discover (void)
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
TimeoutHandler::random_ping (void)
{
  int num_keys = context_.getNumKeys ();

  if (num_keys <= 0)
    return -1;

  int cur_rand = rand () % num_keys;

  std::string key = context_.getKey ((unsigned int)cur_rand);

  // we don't want to send messages to ourself
  if (!check_key (key))
    return -1;

  ACE_DEBUG ((LM_DEBUG, "(%P|%t) Pinging %s! %d of %d\n", key.c_str (), 
    cur_rand, num_keys));

  // send a message to the person @ key
  send_ping (key);

  return 0;
}

int 
TimeoutHandler::handle_timeout (const ACE_Time_Value &tv, const void *arg)
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
    default:
      random_ping ();
    }

  return 0;
}
