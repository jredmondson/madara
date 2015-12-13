#ifndef _MADARA_KNOWLEDGE_UPDATE_SETTINGS_H_
#define _MADARA_KNOWLEDGE_UPDATE_SETTINGS_H_

/**
 * @file KnowledgeBase.h
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains the KnowledgeUpdateSettings class, which
 * provides settings for applying knowledge updates
 */

#include "madara/MADARA_export.h"
#include "madara/utility/stdint.h"
#include "madara/knowledge/KnowledgeReferenceSettings.h"

namespace madara
{
  namespace knowledge
  {
    /**
     * Settings for applying knowledge updates
     **/
    class MADARA_Export KnowledgeUpdateSettings
          : public KnowledgeReferenceSettings
    {
    public:
      /**
       * Constructor
       **/
      KnowledgeUpdateSettings ()
        : KnowledgeReferenceSettings (), 
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
      KnowledgeUpdateSettings (bool t_treat_globals_as_locals,
        bool t_signal_changes = true,
        bool t_always_overwrite = false,
        bool t_always_expand = true,
        bool t_track_local_changes = false,
        uint64_t t_clock_increment = 1)
        : KnowledgeReferenceSettings (t_always_expand),
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
      KnowledgeUpdateSettings (const KnowledgeUpdateSettings & rhs)
        : KnowledgeReferenceSettings (rhs),
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
      ~KnowledgeUpdateSettings ()
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

#endif // _MADARA_KNOWLEDGE_UPDATE_SETTINGS_H_