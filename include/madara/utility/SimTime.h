#ifndef MADARA_UTILITY_SIMTIME_H_
#define MADARA_UTILITY_SIMTIME_H_

#ifdef __cplusplus
#include <mutex>
#include <madara/MADARA_export.h>

#define MADARA_EXTERN_C extern "C" MADARA_Export

namespace madara { namespace utility {
#else
#define MADARA_EXTERN_C
#endif

#if defined(MADARA_FEATURE_SIMTIME) && MADARA_FEATURE_SIMTIME==1

typedef void (*sim_time_callback_fn)(uint64_t*, double*);

/// Set a callback for querying extern time source on-demand.
/// If set to NULL (or left at initial value), time will be
/// extrapolated from last sim_time_notify
MADARA_EXTERN_C sim_time_callback_fn set_sim_time_callback(
    sim_time_callback_fn fn);

/// Tell Madara that simulation time has changed. If callbacks are
/// used, should only be necessary for rate changes.
/// At startup, the equivalent of sim_time_notify(0, 1.0) is used.
MADARA_EXTERN_C void sim_time_notify(uint64_t, double);

/// Minimum non-zero rate, to avoid floating point aberations
MADARA_EXTERN_C const double minrate;

#ifdef __cplusplus
constexpr bool simtime = true;

class SimTime {
private:
  static std::mutex mutex_;
  static sim_time_callback_fn callback_;

  static uint64_t last_realtime_;
  static uint64_t last_simtime_;
  static double last_rate_;

public:
  SimTime() = delete;

  static uint64_t last_realtime() {
    std::lock_guard<std::mutex> guard(mutex_);
    return last_realtime_;
  }

  static uint64_t last_simtime() {
    std::lock_guard<std::mutex> guard(mutex_);
    return last_simtime_;
  }

  static double last_rate() {
    std::lock_guard<std::mutex> guard(mutex_);
    return last_rate_;
  }

  static uint64_t realtime();
  static uint64_t time();
  static double rate();
  static uint64_t duration(uint64_t sim_duration);
  static uint64_t future(uint64_t sim_offset);

  friend sim_time_callback_fn set_sim_time_callback(
      sim_time_callback_fn fn);

  friend void sim_time_notify(uint64_t, double);
};

#endif // ifdef __cplusplus
#else
#ifdef __cplusplus
constexpr bool simtime = false;
#endif
#endif

#ifdef __cplusplus
} } // namespace madara::utility
#endif

#endif
