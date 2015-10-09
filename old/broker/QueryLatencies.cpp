

#include "ace/OS.h"
#include "ace/Log_Msg.h"
#include "ace/SOCK_Connector.h"

#include "AgentMessages.h"
#include "MadaraCommon.h"
#include "QueryLatencies.h"


int Madara::query_latencies_by_key (
  Madara::BrokerContext& context, std::string key)
{
  AgentPing query;
  AgentPing response;
  AgentLatency * latencies = 0;

  ::size_t num_latencies = 0;
  ::ssize_t result = 0;

  std::string host;
  std::string port;
  std::string target;

  // split the user provided key into a host and port
  Madara::split_key (key, host, port);
  
  u_short server_port     = ACE_OS::atoi (port.c_str ());

  // create an address that ACE can use from our host/port
  ACE_INET_Addr addr (server_port, host.c_str ());

  ACE_SOCK_Stream worker;
  ACE_SOCK_Connector connector;

  ACE_DEBUG ((LM_DEBUG, "(%P|%t) Attempting worker latency query (host %s, port %d)\n", 
    host.c_str (), server_port));
  
  if (connector.connect (worker, addr) == -1)
    {
      ACE_DEBUG ((LM_DEBUG, "%s\n", "open"));
      return result;
    }

  query.size = 0;
  query.type = Madara::BROKER_LATENCY_QUERY;

  ACE_DEBUG ((LM_DEBUG, "(%P|%t) Attempting query send (host %s, port %d)\n", 
    host.c_str (), server_port));
  
  // send a latency query to the worker at the host/port
  result = worker.send_n (&query, sizeof (query));

  ACE_DEBUG ((LM_DEBUG, "(%P|%t) Completed query send (host %s, port %d)\n", 
    host.c_str (), server_port));
  
  if (result == -1)
    {
      ACE_DEBUG ((LM_DEBUG, "Problem sending query to %s\n", key.c_str ()));
      return result;
    }

  ACE_Time_Value timeout (10, 0);

  // recv reply back

  ACE_DEBUG ((LM_DEBUG, "(%P|%t) Awaiting response (host %s, port %d)\n", 
    host.c_str (), server_port));
  
  result = worker.recv_n (&response, sizeof(response), &timeout);

  ACE_DEBUG ((LM_DEBUG, "(%P|%t) Received response (host %s, port %d)\n", 
    host.c_str (), server_port));
  
  if (result == -1)
    {
      ACE_DEBUG ((LM_DEBUG, 
        "Madara::QLBK. Never heard back from %s\n", key.c_str ()));
      return result;
    }

  if (response.type == Madara::AGENT_LATENCY_QUERY_RESPONSE)
    {
      num_latencies = response.size;
      ::size_t bytes_transferred = 0;

      latencies = (Madara::AgentLatency *)
        malloc (sizeof (AgentLatency) * num_latencies);

      ACE_DEBUG ((LM_DEBUG, 
        "(%P|%t) Received latency query reponse (host %s, port %d)\n", 
        host.c_str (), server_port));
      
      result = worker.recv_n 
        (latencies, sizeof (AgentLatency) * num_latencies, 
         &timeout, &bytes_transferred);

      ACE_DEBUG ((LM_DEBUG, 
        "(%P|%t) Received response (host %s, port %d, actual bytes = %d, latencies = %d)\n", 
        host.c_str (), server_port, bytes_transferred, latencies));
      
      if (result == -1)
        {
          ACE_DEBUG ((LM_DEBUG, 
            "Madara::QLBK. Failed while receiving latencies from %s\n", 
            key.c_str ()));
          return result;
        }

      for (::size_t i = 0; i < num_latencies; ++i)
        {
          Madara::merge_key (target, latencies[i].host, latencies[i].port);
          ACE_DEBUG ((LM_DEBUG, 
            "Madara::QLBK. Adding latency %d of %d. %s -> %s\n", 
            i, num_latencies, key.c_str (), target.c_str ()));
          context.addLatency (key, target, latencies[i].latency);
        }
    }
 
  else
    {

    ACE_DEBUG ((LM_DEBUG, "(%P|%t) ERROR: Received type %d instead of %d.\n", 
    response.type, Madara::AGENT_LATENCY_QUERY_RESPONSE));
  
    }

  return 0;
}