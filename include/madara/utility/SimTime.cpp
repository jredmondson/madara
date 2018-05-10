#include "SimTime.h"

#ifdef MADARA_FEATURE_SIMTIME

#include <chrono>

namespace madara { namespace utility {

const double minrate = 0.0000000001;

std::mutex SimTime::mutex_{};

sim_time_callback_fn SimTime::callback_ = nullptr;

uint64_t SimTime::last_realtime_ = SimTime::realtime();
uint64_t SimTime::last_simtime_ = 0;
double SimTime::last_rate_ = 1.0;

uint64_t SimTime::realtime() {
  namespace sc = std::chrono;
  auto now = sc::system_clock::now();
  auto dur = now.time_since_epoch();
  return sc::duration_cast<sc::nanoseconds>(dur).count();
}

uint64_t SimTime::time() {
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
    if (callback) {
      callback_(&st, &r);

      last_realtime_ = now;
      last_simtime_ = st;
      last_rate_ = r;
    } else {
      prt = last_realtime_;
      pst = last_simtime_;
      pr = last_rate_;
    }
  }

  if (!callback) {
    if (pr == 0) {
      return pst;
    }

    int64_t offset = now - prt;

    if (pr < minrate) {
      pr = minrate;
    }

    double delta = offset * pr;
    st = pst + (int64_t)delta;
  }

  return st;
}

double SimTime::rate() {
  double r;
  sim_time_callback_fn callback;

  {
    std::lock_guard<std::mutex> guard{mutex_};
    callback = callback_;

    if (callback) {
      callback(nullptr, &r);
    } else {
      r = last_rate_;
    }
  }

  return r;
}

uint64_t SimTime::duration(uint64_t sim_duration) {
  double r = rate();

  if (r < minrate) {
    return -1;
  }

  return sim_duration / r;
}

uint64_t SimTime::future(uint64_t sim_offset) {
  uint64_t now = realtime();
  uint64_t offset = duration(sim_offset);

  if (offset == -1) {
    return -1;
  }

  return now + offset;
}

sim_time_callback_fn set_sim_time_callback(sim_time_callback_fn fn) {
  std::lock_guard<std::mutex> guard{SimTime::mutex_};
  sim_time_callback_fn ret = SimTime::callback_;
  SimTime::callback_ = fn;
  return ret;
}

void sim_time_notify(uint64_t time, double rate) {
  uint64_t now = SimTime::realtime();

  std::lock_guard<std::mutex> guard{SimTime::mutex_};

  SimTime::last_realtime_ = now;
  SimTime::last_simtime_ = time;
  SimTime::last_rate_ = rate;
}

} }

#endif
