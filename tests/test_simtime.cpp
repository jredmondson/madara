#include <madara/utility/SimTime.h>
#include <iostream>

#include "test.h"

using SimTime = madara::utility::SimTime;

int madara_fails = 0;

int main()
{
  VAL(SimTime::realtime());
  VAL(SimTime::time());
  VAL(SimTime::last_realtime());
  VAL(SimTime::last_simtime());
  VAL(SimTime::rate());
  VAL(SimTime::realtime());
  VAL(SimTime::time());
  VAL(SimTime::last_realtime());
  VAL(SimTime::last_simtime());

  madara::utility::sim_time_notify(56000000, 0.5);
  for (int i = 0; i < 10; ++i) {
    VAL(SimTime::realtime());
    VAL(SimTime::time());
    madara::utility::sleep(1);
  }

  // test doesn't really check anything for success or failure

  if (madara_fails > 0) {
    std::cerr << "OVERALL: FAIL. " << madara_fails << " tests failed.\n";
  } else {
    std::cerr << "OVERALL: SUCCESS.\n";
  }

  return madara_fails;
}
