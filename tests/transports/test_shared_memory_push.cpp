#include <iostream>
#include "madara/transport/SharedMemoryPush.h"
#include "madara/utility/Utility.h"

namespace knowledge = madara::knowledge;
namespace transport = madara::transport;
namespace logger = madara::logger;

int madara_fails = 0;

int main (int, char **)
{
  transport::QoSTransportSettings settings;

  // create 3 KBs to push to
  std::vector <knowledge::KnowledgeBase> kbs;
  kbs.resize(3);

  // create a shared memory push transport
  std::vector <transport::SharedMemoryPush *> transports;
  transports.resize(kbs.size());

  // attach transports
  for (size_t i = 0; i < kbs.size(); ++i)
  {
    transports[i] = new transport::SharedMemoryPush(
      kbs[i].get_id(), settings, kbs[i]);

    transports[i]->set(kbs);

    kbs[i].attach_transport(transports[i]);
  }

  kbs[0].set("agent.0.name", "Sam Brandy", knowledge::EvalSettings::SEND);

  std::cerr << "Test 1: from agent.0: agent.0.name == Sam Brandy: ";

  if (kbs[1].get("agent.0.name") == "Sam Brandy" && 
      kbs[2].get("agent.0.name") == "Sam Brandy")
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n";
    madara_fails++;

    for (auto kb : kbs)
    {
      kb.print();
    }
  }

  kbs[1].set("agent.1.name", "Bob Franklin", knowledge::EvalSettings::SEND);

  std::cerr << "Test 2: from agent.1: agent.1.name == Bob Franklin: ";

  if (kbs[0].get("agent.1.name") == "Bob Franklin" && 
      kbs[2].get("agent.1.name") == "Bob Franklin")
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n";
    madara_fails++;

    for (auto kb : kbs)
    {
      kb.print();
    }
  }

  kbs[2].set("agent.2.name", "Sara Thompkins", knowledge::EvalSettings::SEND);

  std::cerr << "Test 3: from agent.2: agent.2.name == Sara Thompkins: ";

  if (kbs[0].get("agent.2.name") == "Sara Thompkins" && 
      kbs[1].get("agent.2.name") == "Sara Thompkins")
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n";
    madara_fails++;

    for (auto kb : kbs)
    {
      kb.print();
    }
  }


  if (madara_fails > 0)
  {
    std::cerr << "OVERALL: FAIL. " << madara_fails << " tests failed.\n";
  }
  else
  {
    std::cerr << "OVERALL: SUCCESS.\n";
  }

  return madara_fails;
}