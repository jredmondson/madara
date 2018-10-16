#ifndef MADARA_UTILITY_SIMTIME_H_
#define MADARA_UTILITY_SIMTIME_H_

#ifdef __cplusplus
#include <mutex>
#include <madara/MadaraExport.h>

#define MADARA_EXTERN_C extern "C" MADARA_EXPORT

namespace madara
{
namespace utility
{
#else
#define MADARA_EXTERN_C
#endif

#ifdef MADARA_FEATURE_SIMTIME

typedef void (*sim_time_callback_fn)(uint64_t*, double*);

/// Set a callback for querying extern time source on-demand.
/// If set to NULL (or left at initial value), time will be
/// extrapolated from last sim_time_notify
MADARA_EXTERN_C sim_time_callback_fn set_sim_time_callback(
    sim_time_callback_fn fn);

/**
 * Tell Madara that simulation time has changed. If callbacks are
 * used, should only be necessary for rate changes.
 *
 * If `time` is -1, the value will be ignored.
 * If `rate` is NAN, the value will be ignored.
 *
 * If this and sim_time_callback have not been called, sim-time
 * equals real time. If rate is set, but not sim-time, sim-time
 * will start counting from the current real-time at the given
 * rate.
 **/
MADARA_EXTERN_C void sim_time_notify(uint64_t time, double rate);

/// Minimum non-zero rate, to avoid floating point aberations
MADARA_EXTERN_C const double minrate;

#ifdef __cplusplus
constexpr bool simtime = true;

class SimTime
{
private:
  static std::mutex mutex_;
  static sim_time_callback_fn callback_;

  static uint64_t last_realtime_;
  static uint64_t last_simtime_;
  static double last_rate_;

public:
  SimTime() = delete;

  /**
   * Get real time of last time simtime parameters were updated, either by a
   * notification, or by callback.
   **/
  static uint64_t last_realtime()
  {
    std::lock_guard<std::mutex> guard(mutex_);
    return last_realtime_;
  }

  /**
   * Get simtime of last time simtime parameters were updated, either by a
   * notification, or by callback.
   **/
  static uint64_t last_simtime()
  {
    std::lock_guard<std::mutex> guard(mutex_);
    return last_simtime_;
  }

  /**
   * Get last known simtime rate, without calling callback
   **/
  static double last_rate()
  {
    std::lock_guard<std::mutex> guard(mutex_);
    return last_rate_;
  }

  /**
   * Get current real-time, not effected by simtime settings
   **/
  static uint64_t realtime();

  /**
   * Get real-time stamp equivalent to given simtime stamp, according to
   * current simtime settings.
   **/
  static uint64_t realtime(uint64_t simtime);

  /**
   * Get simtime equivalent of current real-time
   **/
  static uint64_t time();

  /**
   * Get current simtime rate. Will call callback if set.
   **/
  static double rate();

  /**
   * Scale a duration of simtime to realtime
   **/
  static uint64_t duration(uint64_t sim_duration);

  /**
   * Calculate the realtime of an offset into the future in simtime
   **/
  static uint64_t future(uint64_t sim_offset);

  friend sim_time_callback_fn set_sim_time_callback(sim_time_callback_fn fn);

  friend void sim_time_notify(uint64_t, double);
};

#endif  // ifdef __cplusplus
#else
#ifdef __cplusplus
constexpr bool simtime = false;
#endif
#endif

#ifdef __cplusplus
}
}  // namespace madara::utility
#endif

#endif
