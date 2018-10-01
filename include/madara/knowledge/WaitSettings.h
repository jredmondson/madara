#ifndef _MADARA_WAIT_SETTINGS_
#define _MADARA_WAIT_SETTINGS_

/**
 * @file WaitSettings.h
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains the WaitSettings class, which
 * provides settings for the wait mechanism in the Knowledge Engine
 */

#include <string>
#include "madara/knowledge/EvalSettings.h"

namespace madara
{
namespace knowledge
{
/**
 * @class WaitSettings
 * @brief Encapsulates settings for a wait statement
 **/
struct WaitSettings : public EvalSettings
{
  /* Convenient pre-defined settings objects for common use cases */

  /** Settings to use whatever defaults there are */
  static const WaitSettings DEFAULT;

  /** Settings to enable checkpoints */
  static const WaitSettings CHECKPOINT;

  /** Settings to delay sending modifieds and expand variables */
  static const WaitSettings DELAY;

  /** Settings to delay sending modifieds and enable checkpoints */
  static const WaitSettings DELAY_CHECKPOINT;

  /** Settings to delay sending modifieds and expand variables */
  static const WaitSettings EXPAND;

  /** Settings to delay sending modifieds and expand variables */
  static const WaitSettings DELAY_EXPAND;

  /** Settings to delay send modifieds and not expand variables */
  static const WaitSettings DELAY_NO_EXPAND;

  /** Settings to delay sending modifieds and expand variables */
  static const WaitSettings NO_EXPAND;

  /** Settings to not expand odifieds and enable checkpoints */
  static const WaitSettings NO_EXPAND_CHECKPOINT;

  /** Settings to immediately send modifieds and expand variables */
  static const WaitSettings SEND;

  /** Settings to immediately send modifieds and expand variables */
  static const WaitSettings SEND_EXPAND;

  /** Settings to immediately send modifieds and not expand variables */
  static const WaitSettings SEND_NO_EXPAND;

  /**
   * Constructor
   **/
  WaitSettings() : EvalSettings(), poll_frequency(0.100), max_wait_time(-1.0) {}

  /**
   * Constructor
   * @param  t_delay_sending_modifieds true to send modifieds
   * @param  t_treat_globals_as_locals true if global variables should
   *                                   be marked as not to be sent to the
   *                                   transport
   * @param  t_signal_updates          indicates whether to signal updates
   *                                   to a separate thread that may be
   *                                   waiting. This is only potentially
   *                                   useful to multithreaded MADARA apps
   * @param  t_always_overwrite        always overwrite, despite quality
   * @param  t_always_expand           always try to expand variable names
   * @param  t_track_local_changes     if true, keep track of modifications
   *                                   to local variables for checkpoint
   *                                   saving purposes.
   * @param  t_pre_print_statement     statement to print out before
   *                                   evaluations
   * @param  t_post_print_statement    statement to print out after
   *                                   evaluations
   * @param  t_poll_frequency          the frequency to poll for changes
   *                                   (in seconds)
   * @param  t_max_wait_time           the maximum time to wait for truth
   *                                   (in seconds)
   * @param  t_clock_increment         amount of clock ticks to increment
   *                                   updated records by
   * @param  t_treat_locals_as_globals true if local variable changes should
   *                                   be sent over the network (dangerous).
   *                                   @see treat_locals_as_globals
   **/
  WaitSettings(bool t_delay_sending_modifieds, bool t_treat_globals_as_locals,
      bool t_signal_updates, bool t_always_overwrite, bool t_always_expand,
      bool t_track_local_changes, std::string t_pre_print_statement,
      std::string t_post_print_statement, double t_poll_frequency,
      double t_max_wait_time, uint64_t t_clock_increment = 1,
      bool t_treat_locals_as_globals = false)
    : EvalSettings(t_delay_sending_modifieds, t_treat_globals_as_locals,
          t_signal_updates, t_always_overwrite, t_always_expand,
          t_track_local_changes, t_pre_print_statement, t_post_print_statement,
          t_clock_increment, t_treat_locals_as_globals),
      poll_frequency(t_poll_frequency),
      max_wait_time(t_max_wait_time)
  {
  }

  /**
   * Copy constructor
   **/
  WaitSettings(const WaitSettings& rhs)
    : EvalSettings(rhs),
      poll_frequency(rhs.poll_frequency),
      max_wait_time(rhs.max_wait_time)
  {
  }

  /**
   * Frequency to poll an expression for truth (in seconds)
   **/
  double poll_frequency;

  /**
   * Maximum time to wait for an expression to become true (in seconds)
   **/
  double max_wait_time;
};
}
}
#endif  // _MADARA_WAIT_SETTINGS_
