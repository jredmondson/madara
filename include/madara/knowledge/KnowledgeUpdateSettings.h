#ifndef _MADARA_KNOWLEDGE_UPDATE_SETTINGS_H_
#define _MADARA_KNOWLEDGE_UPDATE_SETTINGS_H_

/**
 * @file KnowledgeBase.h
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains the KnowledgeUpdateSettings class, which
 * provides settings for applying knowledge updates
 */

#include "madara/MadaraExport.h"
#include "madara/utility/StdInt.h"
#include "madara/knowledge/KnowledgeReferenceSettings.h"

namespace madara
{
namespace knowledge
{
/**
 * Settings for applying knowledge updates
 **/
class MADARA_EXPORT KnowledgeUpdateSettings : public KnowledgeReferenceSettings
{
public:
  /* Convenient pre-defined settings objects */
  static const KnowledgeUpdateSettings DEFAULT;
  static const KnowledgeUpdateSettings NO_EXPAND;
  static const KnowledgeUpdateSettings GLOBAL_AS_LOCAL;
  static const KnowledgeUpdateSettings GLOBAL_AS_LOCAL_NO_EXPAND;

  /**
   * Constructor
   **/
  KnowledgeUpdateSettings() = default;

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
   * @param  t_treat_locals_as_globals true if local variable changes should
   *                                   be sent over the network (dangerous).
   *                                   @see treat_locals_as_globals
   * @param  t_stream_changes          true if changes must be streamed
   * @param  t_exceptions_on_unitialized true if exceptions must be thrown
   *                                   when reading uninitialized variables
   **/
  KnowledgeUpdateSettings(bool t_treat_globals_as_locals,
      bool t_signal_changes = true, bool t_always_overwrite = false,
      bool t_always_expand = true, bool t_track_local_changes = true,
      uint64_t t_clock_increment = 1, bool t_treat_locals_as_globals = false,
      bool t_stream_changes = true, bool t_exceptions_on_unitialized = false)
    : KnowledgeReferenceSettings(t_always_expand, t_exceptions_on_unitialized),
      treat_globals_as_locals(t_treat_globals_as_locals),
      signal_changes(t_signal_changes),
      always_overwrite(t_always_overwrite),
      track_local_changes(t_track_local_changes),
      clock_increment(t_clock_increment),
      treat_locals_as_globals(t_treat_locals_as_globals),
      stream_changes(t_stream_changes)
  {
  }

  /**
   * Constructor
   **/
  KnowledgeUpdateSettings(const KnowledgeUpdateSettings& rhs) = default;
  /*
    : KnowledgeReferenceSettings (rhs),
      treat_globals_as_locals (rhs.treat_globals_as_locals),
      signal_changes (rhs.signal_changes),
      always_overwrite (rhs.always_overwrite),
      track_local_changes (rhs.track_local_changes),
      clock_increment (rhs.clock_increment),
      treat_locals_as_globals (rhs.treat_locals_as_globals)
  {
  }*/

  /**
   * Destructor
   **/
  ~KnowledgeUpdateSettings() = default;

  /**
   * Toggle whether updates to global variables are treated as
   * local variables and not marked as modified to the transport
   **/
  bool treat_globals_as_locals = false;

  /**
   * Toggle whether to signal changes have happened. Setting this
   * to false will increase performance, but in general should be
   * left to true. Setting this to false can result in problems
   * with wait statements.
   **/
  bool signal_changes = true;

  /**
   * Toggle for always overwriting records, regardless of quality,
   * clock values, etc.
   **/
  bool always_overwrite = false;

  /**
   * Toggle for checkpointing support. If this is true, all changes
   * will be added to the local changes map in the knowledge base,
   * which is used by save_checkpoint to create diffs of knowledge
   **/
  bool track_local_changes = true;

  /**
   * Default clock increment
   **/
  uint64_t clock_increment = 1;

  /**
   * Toggle whether updates to local variables are treated as
   * global variables that should be sent over the transport. It
   * should be stressed that this is dangerous and should only
   * be used for debugging. If you toggle this to true, all local
   * variables will be sent over the network where they will
   * overwrite local variables in remote systems, unless the
   * remote system filters out the local variable changes with
   * an on-receive filter
   **/
  bool treat_locals_as_globals = false;

  /**
   * Toggle for streaming support. If this is true, all changes
   * will be streamed to the attached streamer, if any.
   **/
  bool stream_changes = true;
};
}
}

#endif  // _MADARA_KNOWLEDGE_UPDATE_SETTINGS_H_
