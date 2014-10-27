
#include "ace/Timer_Queue_Adapters.h"
#include "ace/Timer_Heap.h"
#include "ace/TP_Reactor.h"

#include "ace/Guard_T.h"
#include "ace/Recursive_Thread_Mutex.h"

#include "client_acceptor.h"
#include "Timeout_Handler.h"
#include "Madara_Common.h"

#include "Agent_Context.h"
#include "Agent.h"

typedef ACE_Thread_Timer_Queue_Adapter <ACE_Timer_Heap> ActiveTimer;
typedef ACE_Guard<ACE_Recursive_Thread_Mutex> Finished_Guard;

//static sig_atomic_t finished = 0;
static ACE_Recursive_Thread_Mutex mutex_;

extern "C" void handler (int)
{
  Finished_Guard guard (mutex_);
  Madara::finished_ = 1;
}

const u_short DEFAULT_PORT = ACE_DEFAULT_SERVER_PORT;
const std::string DEFAULT_KEY = "127.0.0.1";

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

  // create an agent context, and fill it with latencies from a hosts_file
  Madara::Agent_Context context (server_host, port_str); 
  context.read (hosts_file);

  // create a reactor with a thread pool
  ACE_TP_Reactor reactor;
  ACE_Reactor new_reactor (&reactor);
  ACE_Reactor::instance (&new_reactor);

  // create the handlers
  Client_Acceptor peer_acceptor (context);
  Timeout_Handler timeouts (context);
  ActiveTimer timer;

  // for the scheduled timer
  ACE_Time_Value initial (0);
  ACE_Time_Value interval (2,0);
  int arg1 = 0;


  ACE_Reactor::instance ()->schedule_timer (&timeouts, &arg1, 
    ACE_Time_Value::zero, interval);

  // if no port is provided, then bind to the first available
  if (port_str == "")
    Madara::bindToFirstAvailableWorkerPort (peer_acceptor, context);
  else if (peer_acceptor.open (ACE_INET_Addr (server_port), ACE_Reactor::instance ()) == -1)
    ACE_ERROR_RETURN ((LM_ERROR, "%p\n", "open"), -1);

  server_port = atoi (context.getPort ().c_str ());

  // signal handler for clean exit
  ACE_Sig_Action sa ((ACE_SignalHandler) handler, SIGINT);

  std::cerr << "Dumping context\n\n";

  context.write (std::cerr);

  ACE_DEBUG ((LM_DEBUG, "(%P|%t) starting up swarm agent (port %d)\n", server_port));

  while (!Madara::finished_)
        reactor.handle_events ();

  ACE_DEBUG ((LM_DEBUG, "(%P|%t) shutting down swarm agent\n"));
 
  std::cerr << "Dumping context\n\n";

  context.write (std::cerr);

  return 0;
} 