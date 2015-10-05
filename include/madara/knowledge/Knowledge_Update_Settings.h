#ifndef _KNOWLEDGE_UPDATE_SETTINGS_H_
#define _KNOWLEDGE_UPDATE_SETTINGS_H_

/**
 * @file Knowledge_Base.h
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains the Knowledge_Update_Settings class, which
 * provides settings for applying knowledge updates
 */

#include "madara/MADARA_export.h"
#include "madara/utility/stdint.h"
#include "madara/knowledge/Knowledge_Reference_Settings.h"

namespace madara
{
  namespace knowledge
  {
    /**
     * Settings for applying knowledge updates
     **/
    class MADARA_Export Knowledge_Update_Settings
          : public Knowledge_Reference_Settings
    {
    public:
      /**
       * Constructor
       **/
      Knowledge_Update_Settings ()
        : Knowledge_Reference_Settings (), 
          treat_globals_as_locals (false),
          signal_changes (true),
          always_overwrite (false),
          track_local_changes (false),
          clock_increment (1)
      {
      }
  
      /**
       * Constructor
        * @param  t_treat_globals_as_locals true if global variables should
        *                                   be marked as not to be sent to the
        *                                   transport
        * @param  t_signal_changes          indicates whether to signal updates
        *                                   to a separate thread that may be
        *                                   waiting. This is only potentially
        *                                   useful to multithreaded MADARA apps
        * @param  t_always_overwrite        always overwrite, despite quality
        * @param  t_always_expand           always try to expand variable names
        * @param  t_track_local_changes     if true, keep track of modifications
        *                                   to local variables for checkpoint
        *                                   saving purposes.
        * @param  t_clock_increment         amount of clock ticks to increment
        *                                   updated records by
       **/
      Knowledge_Update_Settings (bool t_treat_globals_as_locals,
        bool t_signal_changes = true,
        bool t_always_overwrite = false,
        bool t_always_expand = true,
        bool t_track_local_changes = false,
        uint64_t t_clock_increment = 1)
        : Knowledge_Reference_Settings (t_always_expand),
          treat_globals_as_locals (t_treat_globals_as_locals),
          signal_changes (t_signal_changes),
          always_overwrite (t_always_overwrite),
          track_local_changes (t_track_local_changes),
          clock_increment (t_clock_increment)
      {
      }
  
      /**
       * Constructor
       **/
      Knowledge_Update_Settings (const Knowledge_Update_Settings & rhs)
        : Knowledge_Reference_Settings (rhs),
          treat_globals_as_locals (rhs.treat_globals_as_locals),
          signal_changes (rhs.signal_changes),
          always_overwrite (rhs.always_overwrite),
          track_local_changes (rhs.track_local_changes),
          clock_increment (rhs.clock_increment)
      {
      }


      /**
       * Destructor
       **/
      ~Knowledge_Update_Settings ()
      {
      }

      /**
      * Toggle whether updates to global variables are treated as
      * local variables and not marked as modified to the transport
      **/
      bool treat_globals_as_locals;

      /**
       * Toggle whether to signal changes have happened. Setting this
       * to false will increase performance, but in general should be
       * left to true. Setting this to false can result in problems
       * with wait statements.
       **/
      bool signal_changes;
      
      /**
      * Toggle for always overwriting records, regardless of quality,
      * clock values, etc.
      **/
      bool always_overwrite;
      
      /**
       * Toggle for keeping track of modifications to local variables.
       **/
      bool track_local_changes;

      /**
       * Default clock increment
       **/
      uint64_t  clock_increment;
    };
  }
}

#endif
