#include <madara/utility/SimTime.h>

#include "test.h"

using SimTime = madara::utility::SimTime;

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
}
