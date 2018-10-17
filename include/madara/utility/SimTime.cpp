#include "SimTime.h"

#ifdef MADARA_FEATURE_SIMTIME

#include <chrono>
#include <cmath>

namespace madara
{
namespace utility
{
const double minrate = 0.0000000001;

std::mutex SimTime::mutex_{};

sim_time_callback_fn SimTime::callback_ = nullptr;

uint64_t SimTime::last_realtime_ = SimTime::realtime();
uint64_t SimTime::last_simtime_ = -1;
double SimTime::last_rate_ = 1.0;

uint64_t SimTime::realtime()
{
  namespace sc = std::chrono;
  auto now = sc::steady_clock::now();
  auto dur = now.time_since_epoch();
  return sc::duration_cast<sc::nanoseconds>(dur).count();
}

uint64_t SimTime::time()
{
  uint64_t prt;
  uint64_t pst;
  double pr;
  uint64_t st;
  double r;
  sim_time_callback_fn callback;

  uint64_t now = realtime();
  {
    std::lock_guard<std::mutex> guard{mutex_};
    callback = callback_;
    if(callback)
    {
      callback_(&st, &r);

      last_realtime_ = now;
      last_simtime_ = st;
      last_rate_ = r;
    }
    else
    {
      prt = last_realtime_;
      pst = last_simtime_;
      pr = last_rate_;
    }
  }

  if(!callback)
  {
    if(pst == (uint64_t)-1)
    {
      return now;
    }
    if(pr == 0)
    {
      return pst;
    }

    int64_t offset = now - prt;

    if(pr < minrate)
    {
      pr = minrate;
    }

    double delta = offset * pr;
    st = pst + (int64_t)delta;
  }

  return st;
}

double SimTime::rate()
{
  double r;
  sim_time_callback_fn callback;

  {
    std::lock_guard<std::mutex> guard{mutex_};
    callback = callback_;

    if(callback)
    {
      callback(nullptr, &r);
    }
    else
    {
      r = last_rate_;
    }
  }

  return r;
}

uint64_t SimTime::duration(uint64_t sim_duration)
{
  double r = rate();

  if(r < minrate)
  {
    return -1;
  }

  return sim_duration / r;
}

uint64_t SimTime::future(uint64_t sim_offset)
{
  uint64_t now = realtime();
  uint64_t offset = duration(sim_offset);

  if(offset == (uint64_t)-1)
  {
    return -1;
  }

  return now + offset;
}

sim_time_callback_fn set_sim_time_callback(sim_time_callback_fn fn)
{
  std::lock_guard<std::mutex> guard{SimTime::mutex_};
  using std::swap;
  swap(fn, SimTime::callback_);
  return fn;
}

void sim_time_notify(uint64_t time, double rate)
{
  bool update_time = time != (uint64_t)-1;
  bool update_rate = !std::isnan(rate);

  if(!update_time && !update_rate)
  {
    return;
  }

  uint64_t now = SimTime::realtime();
  std::lock_guard<std::mutex> guard{SimTime::mutex_};

  if(update_time)
  {
    SimTime::last_realtime_ = now;
    SimTime::last_simtime_ = time;
  }
  else if(SimTime::last_simtime_ == (uint64_t)-1)
  {
    SimTime::last_realtime_ = now;
    SimTime::last_simtime_ = now;
  }

  if(update_rate)
  {
    SimTime::last_rate_ = rate;
  }
}
}
}

#endif
