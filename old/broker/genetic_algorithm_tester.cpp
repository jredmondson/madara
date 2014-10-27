
// C++
#include <stdlib.h>

#include <fstream>
#include <iostream>
#include <string>
#include "ace/OS.h"
#include "ace/Log_Msg.h"
#include "ace/SOCK_Connector.h"
#include "Agent_Messages.h"
#include "Madara_Common.h"
#include "Broker_Context.h"
#include "Deployment.h"

const int LATENCY_LEVELS = 3;

const int possible_latencies [] = {
  10, 100, 1000
  };

void testDeploymentSize (int size, std::ostream & output)
{
  Madara::Broker_Context context;
  Madara::Deployment::Candidate deployment;
  ACE_hrtime_t start, end;
  int total_latency = 0;
  int initial_latency = 0;
  unsigned int total;
  int i = 0, j = 0;

  std::vector <std::string> keys (size);

  keys.resize (size);

  char buf[18];
  strcpy (buf,"127.0.0.1:");

  // change condition to i < size if you need a full broadcast
  for (i = 1; i < size / 2; ++i)
    context.addRequirement (0, i);

  // comment this out if you need a full broadcast
  for (i = size / 2 + 1; i < size; ++i)
    context.addRequirement (size / 2, i);

  for (i = Madara::WORKER_PORT_START + 0, j = 0; j < size; ++i, ++j)
    {
      itoa (i, buf + 10, 10);
      keys[j] = buf;
    }

  for (i = 0; i < size; ++i)
    {
      for (j = 0; j < size; ++j)
        {
          context.addLatency (keys[i], keys[j], 
            possible_latencies[rand () % (LATENCY_LEVELS - 1)]);
        }
    }

  //context.write (output);

  deployment = context.learnDeployment ();

  // comment this out if you want to start from TNH
  //for (int i = 0; i < size; i++)
  //  {
  //    deployment[i] = keys[i];
  //  }

  initial_latency = Madara::Deployment::calculateUtility (context.getDeployment (),
                                deployment, context.getBrokerMap ());
  
  // keep track of time
  start = ACE_OS::gethrtime ();


  for (int i = 0; i < size; i++)
    {
      total_latency = context.mutate (deployment);
      output << i << " " << total_latency << std::endl;
    }
  

  // keep track of time
  end = ACE_OS::gethrtime ();
  total = (end - start) / 1000;

  //context.writeDeploymentCandidate (output, deployment);

  output << "Broadcast deployment (" << size << ") took " << total
         << " us" << std::endl;
  output << "TNH initial: " << initial_latency << " genetic final: "
    << total_latency << " gain: " << initial_latency - total_latency
    << std::endl;

}

int main (int argc, char *argv[])
{
  std::ofstream output ("test_results.txt");
  Madara::Broker_Context context;
  Madara::Deployment::Candidate deployment;
  ACE_hrtime_t start, end;
  unsigned int total;
  int i = 20;
  int count = 20;

  srand (time (NULL));
  
  for (i = 1000; i <= 1000; i += count)
    {
      std::cout << "Testing deployment of size " << i << std::endl;
      testDeploymentSize (i, output);
    }

  return 0;
}