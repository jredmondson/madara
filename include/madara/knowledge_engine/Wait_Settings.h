#ifndef _WAIT_SETTINGS_
#define _WAIT_SETTINGS_

/**
 * @file Wait_Settings.h
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains the Wait_Settings class, which
 * provides settings for the wait mechanism in the Knowledge Engine
 */

#include <string>
#include "madara/knowledge_engine/Eval_Settings.h"

namespace Madara
{
  namespace Knowledge_Engine
  {
    /**
     * @class Wait_Settings
     * @brief Encapsulates settings for a wait statement
     **/
    struct Wait_Settings : public Eval_Settings
     {
       /**
        * Constructor
        **/
       Wait_Settings ()
         : Eval_Settings (), poll_frequency (0.100), max_wait_time (-1.0)
       {
       }

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
        **/
       Wait_Settings (bool t_delay_sending_modifieds,
         bool t_treat_globals_as_locals,
         bool t_signal_updates,
         bool t_always_overwrite,
         bool t_always_expand,
         bool t_track_local_changes,
         std::string t_pre_print_statement,
         std::string t_post_print_statement,
         double t_poll_frequency, double t_max_wait_time)
         : Eval_Settings (t_delay_sending_modifieds,
              t_treat_globals_as_locals, t_signal_updates,
              t_always_overwrite, t_always_expand, t_track_local_changes,
              t_pre_print_statement, t_post_print_statement),
           poll_frequency (t_poll_frequency), max_wait_time (t_max_wait_time)
       {
       }
       
       /**
        * Copy constructor
        **/
       Wait_Settings (const Wait_Settings & rhs)
         : Eval_Settings (rhs),
         poll_frequency (rhs.poll_frequency), max_wait_time (rhs.max_wait_time)
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
#endif
