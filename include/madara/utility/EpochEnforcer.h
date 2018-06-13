#ifndef _MADARA_UTILITY_EPOCHENFORCER_H_
#define _MADARA_UTILITY_EPOCHENFORCER_H_

#include <thread>
#include <chrono>
#include "IntTypes.h"

namespace madara
{
  namespace utility
  {
    /**
     * @class EpochEnforcer
     * @brief Enforces a periodic epoch
     */
    template <typename CLOCK=std::chrono::steady_clock>
    class EpochEnforcer
    {
    public:
      /**
       * Constructor
       * @param  period       the period between epochs in seconds
       * @param  max_duration  the maximum duration in seconds
       **/
      EpochEnforcer (double period, double max_duration)
      {
        start ();
        set_period (period);
        set_duration (max_duration);
      }

      /**
       * Starts the timer. Affects future duration calls.
       **/
      inline void start (void)
      {
        start_ = CLOCK::now ();
        next_ = start_;
      }

      /**
       * Sets the maximum duration for the enforcer.
       * @param  duration   the maximum duration of the period epochs
       **/
      inline void set_duration (double duration)
      {
        std::chrono::duration<double> temp_duration (duration);
        auto duration_ns = std::chrono::duration_cast<
          std::chrono::nanoseconds> (temp_duration);

        max_ = start_ + duration_ns;
      }

      /**
       * Sets the period for the enforcer.
       * @param  period   the period of the epochs
       **/
      inline void set_period (double period)
      {
        std::chrono::duration<double> temp_period (period);
        period_ = std::chrono::duration_cast<
          std::chrono::nanoseconds> (temp_period);

        next_ = start_ + period_;
      }

      /**
       * Advance next
       **/
      inline void advance_next (void)
      {
        auto current_time = CLOCK::now ();

        while (next_ <= current_time)
          next_ += period_;
      }

      /**
       * Checks if current time is at or past next
       * @return true if current time is greater than or equal to next epoch
       **/
      inline bool has_reached_next (void) const
      {
        auto current_time = CLOCK::now ();

        return current_time >= next_;
      }

      /**
       * Sleeps until the next epoch
       **/
      inline void sleep_until_next (void)
      {
        std::this_thread::sleep_until (next_);

        advance_next ();
      }

      /**
       * Checks to see if max duration is finished
       * @return  true if max duration is exceeded, false otherwise
       **/
      inline bool is_done (void) const
      {
        return CLOCK::now () >= max_;
      }

      /**
       * Returns a duration in nanoseconds
       * @return the duration of the timer in nanoseconds
       **/
      inline uint64_t duration_ns (void) const
      {
        auto dur = std::chrono::duration_cast<std::chrono::nanoseconds> (
          CLOCK::now () - start_);

        return (uint64_t) dur.count ();
      }

      /**
       * Returns a duration in seconds
       * @return the duration of the timer in seconds
       **/
      inline uint64_t duration_s (void) const
      {
        auto dur = std::chrono::duration_cast<std::chrono::seconds> (
          CLOCK::now () - start_);

        return (uint64_t) dur.count ();
      }

      /**
       * Returns a duration in seconds (double format)
       * @return the duration of the timer in seconds
       **/
      inline double duration_ds (void) const
      {
        auto dur = std::chrono::duration_cast<std::chrono::duration <double>> (
          CLOCK::now () - start_);

        return (double) dur.count ();
      }

      /**
       * Returns a duration in user-specified period
       * @return the duration of the timer in period
       **/
      template <typename PERIOD=std::chrono::nanoseconds>
      inline uint64_t duration (void) const
      {
        auto dur = std::chrono::duration_cast<PERIOD> (
          CLOCK::now () - start_);

        return (uint64_t) dur.count ();
      }

    private:
      /// the start of the timer duration
      std::chrono::time_point<CLOCK> start_;

      /// the end of the timer duration
      std::chrono::time_point<CLOCK> max_;

      /// the next epoch to sleep to
      std::chrono::time_point<CLOCK> next_;

      /// the period of the epochs
      std::chrono::nanoseconds period_;

    };
  }
}

#endif // _MADARA_UTILITY_EPOCHENFORCER_H_
