
// C++
#include <stdlib.h>

#include <fstream>
#include <iostream>
#include <string>
#include "ace/OS.h"
#include "ace/Log_Msg.h"
#include "ace/SOCK_Connector.h"

#include "ace/Timer_Queue_Adapters.h"
#include "ace/Timer_Heap.h"
#include "ace/TP_Reactor.h"

#include "ace/Guard_T.h"
#include "ace/Recursive_Thread_Mutex.h"


#include "AgentMessages.h"
#include "BrokerContext.h"
#include "MadaraCommon.h"
#include "BrokerTimeoutHandler.h"
#include "Agent.h"

typedef ACE_Thread_Timer_Queue_Adapter <ACE_Timer_Heap> ActiveTimer;
typedef ACE_Guard<ACE_Recursive_Thread_Mutex> FinishedGuard;

const std::string DEFAULT_KEY = "127.0.0.1";
const u_short DEFAULT_PORT = ACE_DEFAULT_SERVER_PORT;

//static sig_atomic_t finished = 0;
static ACE_Recursive_Thread_Mutex mutex_;

extern "C" void handler (int)
{
  FinishedGuard guard (mutex_);
  Madara::finished_ = 1;
}

void testDeploymentSize (int size, std::ostream & output)
{
  Madara::BrokerContext context;
  Madara::Deployment::Candidate deployment;
  ACE_hrtime_t start, end;
  unsigned int total;
  int i = 0, j = 0;

  std::vector <std::string> keys (size);

  keys.resize (size);

  char buf[18];
  strcpy (buf,"127.0.0.1:");

  for (i = 1; i < size; ++i)
    context.addRequirement (0, i);

  for (i = Madara::WORKER_PORT_START + 0, j = 0; j < size; ++i, ++j)
    {
      itoa (i, buf + 10, 10);
      keys[j] = buf;
    }

  for (i = 0; i < size; ++i)
    {
      for (j = 0; j < size; ++j)
        {
          context.addLatency (keys[i], keys[j], rand () % 100 + 10);
        }
    }

  //context.write (output);

  // keep track of time
  start = ACE_OS::gethrtime ();

  deployment = context.learnDeployment ();

  // keep track of time
  end = ACE_OS::gethrtime ();
  total = (end - start) / 1000;

  context.writeDeploymentCandidate (output, deployment);

  output << "Broadcast deployment (" << size << ") took " << total
         << " us" << std::endl;

}

int main (int argc, char *argv[])
{
  // user may provide a host@port string and a hosts_file
  std::string key = argc > 1 ? argv[1] : DEFAULT_KEY;
  std::string hosts_file = argc > 2 ? argv[2] : "";

  std::string server_host;
  std::string port_str;

  // split the host@port
  Madara::split_key (key, server_host, port_str);

  // create a u_short port for the acceptor
  u_short server_port = argc > 1 ? 
    ACE_OS::atoi (port_str.c_str ()) : DEFAULT_PORT;

  Madara::BrokerContext context;

  // create a reactor with a thread pool
  ACE_TP_Reactor reactor;
  ACE_Reactor new_reactor (&reactor);
  ACE_Reactor::instance (&new_reactor);

  // create the handlers
  ClientAcceptor peer_acceptor (context);
  BrokerTimeoutHandler timeouts (context);
  ActiveTimer timer;

  // for the scheduled timer
  ACE_Time_Value initial (0);
  ACE_Time_Value interval (2,0);
  int arg1 = 0;


  ACE_Reactor::instance ()->schedule_timer (&timeouts, &arg1, 
    ACE_Time_Value::zero, interval);

  // if no port is provided, then bind to the first available
  if (port_str == "")
    Madara::bindToFirstAvailableBrokerPort (peer_acceptor, context);
  else if (peer_acceptor.open (ACE_INET_Addr (server_port), ACE_Reactor::instance ()) == -1)
    ACE_ERROR_RETURN ((LM_ERROR, "%p\n", "open"), -1);

  server_port = atoi (context.getPort ().c_str ());

  // signal handler for clean exit
  ACE_Sig_Action sa ((ACE_SignalHandler) handler, SIGINT);

  std::cerr << "Dumping context\n\n";

  context.write (std::cerr);

  ACE_DEBUG ((LM_DEBUG, "(%P|%t) starting up broker agent (port %d)\n", server_port));

  while (!Madara::finished_)
        reactor.handle_events ();

  ACE_DEBUG ((LM_DEBUG, "(%P|%t) shutting down swarm agent\n"));
 
  std::cerr << "Dumping context\n\n";

  context.write (std::cerr);


/*  std::ofstream output ("test_results.txt");
  Madara::BrokerContext context;
  Madara::Deployment::Candidate deployment;
  ACE_hrtime_t start, end;
  unsigned int total;
  int i = 20;
  int count = 20;

  srand (time (NULL));
  
  for (i = 20; i <= 200; i += count)
    {
      std::cout << "Testing deployment of size " << i << std::endl;
      testDeploymentSize (i, std::cerr);
    }
*/
  return 0;
}