#ifndef _MADARA_UTILITY_TIMER_H_
#define _MADARA_UTILITY_TIMER_H_

#include <chrono>
#include "IntTypes.h"

namespace madara
{
namespace utility
{
/**
 * @class Timer
 * @brief Manages a timer for duration testing
 */
template<typename CLOCK = std::chrono::steady_clock>
class Timer
{
public:
  /**
   * Starts the timer. Affects future duration calls.
   **/
  inline void start(void)
  {
    start_ = CLOCK::now();
  }

  /**
   * Stops the timer. Affects future duration calls.
   **/
  inline void stop(void)
  {
    end_ = CLOCK::now();
  }

  /**
   * Returns a duration in nanoseconds
   * @return the duration of the timer in nanoseconds
   **/
  inline uint64_t duration_ns(void) const
  {
    auto dur =
        std::chrono::duration_cast<std::chrono::nanoseconds>(end_ - start_);

    return (uint64_t)dur.count();
  }

  /**
   * Returns a duration in seconds
   * @return the duration of the timer in seconds
   **/
  inline uint64_t duration_s(void) const
  {
    auto dur = std::chrono::duration_cast<std::chrono::seconds>(end_ - start_);

    return (uint64_t)dur.count();
  }

  /**
   * Returns a duration in seconds (double format)
   * @return the duration of the timer in seconds
   **/
  inline double duration_ds(void) const
  {
    auto dur = std::chrono::duration_cast<std::chrono::duration<double>>(
        end_ - start_);

    return (double)dur.count();
  }

  /**
   * Returns a duration in user-specified period
   * @return the duration of the timer in period
   **/
  template<typename PERIOD = std::chrono::nanoseconds>
  inline uint64_t duration(void) const
  {
    auto dur = std::chrono::duration_cast<PERIOD>(end_ - start_);

    return (uint64_t)dur.count();
  }

private:
  /// the start of the timer duration
  std::chrono::time_point<CLOCK> start_;

  /// the end of the timer duration
  std::chrono::time_point<CLOCK> end_;
};

typedef Timer<std::chrono::steady_clock> TimerSteady;
}
}

#endif  // _MADARA_UTILITY_TIMER_H_
