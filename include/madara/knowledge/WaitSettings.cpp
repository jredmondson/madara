#include "WaitSettings.h"

namespace madara
{
namespace knowledge
{
/* Constructor Args:
      bool t_delay_sending_modifieds,
      bool t_treat_globals_as_locals,
      bool t_signal_updates,
      bool t_always_overwrite,
      bool t_always_expand,
      bool t_track_local_changes,
      std::string t_pre_print_statement,
      std::string t_post_print_statement,
      double t_poll_frequency,
      double t_max_wait_time
*/

//                                                   delay   g->l   sig   ovwrt
//                                                   expand chkpt  pre post poll
//                                                   time
const WaitSettings WaitSettings::CHECKPOINT(
    true, false, true, false, true, true, "", "", 0.100, -1.0);
const WaitSettings WaitSettings::DELAY(
    true, false, true, false, true, false, "", "", 0.100, -1.0);
const WaitSettings WaitSettings::DELAY_CHECKPOINT(
    true, false, true, false, true, true, "", "", 0.100, -1.0);
const WaitSettings WaitSettings::DELAY_EXPAND(
    true, false, true, false, true, false, "", "", 0.100, -1.0);
const WaitSettings WaitSettings::DELAY_NO_EXPAND(
    true, false, true, false, false, false, "", "", 0.100, -1.0);
const WaitSettings WaitSettings::EXPAND(
    true, false, true, false, true, false, "", "", 0.100, -1.0);
const WaitSettings WaitSettings::NO_EXPAND(
    true, false, true, false, false, false, "", "", 0.100, -1.0);
const WaitSettings WaitSettings::NO_EXPAND_CHECKPOINT(
    true, false, true, false, false, true, "", "", 0.100, -1.0);
const WaitSettings WaitSettings::SEND(
    false, false, true, false, true, false, "", "", 0.100, -1.0);
const WaitSettings WaitSettings::SEND_EXPAND(
    false, false, true, false, true, false, "", "", 0.100, -1.0);
const WaitSettings WaitSettings::SEND_NO_EXPAND(
    false, false, true, false, false, false, "", "", 0.100, -1.0);
}
}
