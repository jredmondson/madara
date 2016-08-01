#include "KnowledgeUpdateSettings.h"

namespace madara
{
  namespace knowledge
  {
    /*
      Constructor args:
        bool t_treat_globals_as_locals,
        bool t_signal_changes = true,
        bool t_always_overwrite = false,
        bool t_always_expand = true,
        bool t_track_local_changes = false,
        uint64_t t_clock_increment = 1
     */
    const KnowledgeUpdateSettings KnowledgeUpdateSettings::DEFAULT(false);
    const KnowledgeUpdateSettings KnowledgeUpdateSettings::GLOBAL_AS_LOCAL(true);
    const KnowledgeUpdateSettings KnowledgeUpdateSettings::NO_EXPAND(false, true, false, false);
    const KnowledgeUpdateSettings KnowledgeUpdateSettings::GLOBAL_AS_LOCAL_NO_EXPAND(true, true, false, false);
  }
}
