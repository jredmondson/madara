#include "EvalSettings.h"

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
*/

const EvalSettings EvalSettings::DEFAULT;
//                                                   delay   g->l   sig   ovwrt
//                                                   expand chkpt  pre post
const EvalSettings EvalSettings::CHECKPOINT(
    true, false, true, false, true, true, "", "");
const EvalSettings EvalSettings::DELAY(
    true, false, true, false, true, false, "", "");
const EvalSettings EvalSettings::DELAY_CHECKPOINT(
    true, false, true, false, true, true, "", "");
const EvalSettings EvalSettings::DELAY_EXPAND(
    true, false, true, false, true, false, "", "");
const EvalSettings EvalSettings::DELAY_NO_EXPAND(
    true, false, true, false, false, false, "", "");
const EvalSettings EvalSettings::EXPAND(
    true, false, true, false, true, false, "", "");
const EvalSettings EvalSettings::NO_EXPAND(
    true, false, true, false, false, false, "", "");
const EvalSettings EvalSettings::NO_EXPAND_CHECKPOINT(
    true, false, true, false, false, true, "", "");
const EvalSettings EvalSettings::SEND(
    false, false, true, false, true, false, "", "");
const EvalSettings EvalSettings::SEND_EXPAND(
    false, false, true, false, true, false, "", "");
const EvalSettings EvalSettings::SEND_NO_EXPAND(
    false, false, true, false, false, false, "", "");
}
}
