#ifndef MADARA_KNOWLEDGE_CHECKPOINT_STREAMER_H_
#define MADARA_KNOWLEDGE_CHECKPOINT_STREAMER_H_

#include <memory>
#include <vector>
#include <thread>
#include <atomic>
#include <chrono>

#include "madara/MadaraExport.h"
#include "madara/knowledge/CheckpointSettings.h"
#include "madara/knowledge/KnowledgeBase.h"
#include "madara/knowledge/BaseStreamer.h"

/**
 * @file CheckpointStreamer.h
 * @author David Kyle <david.kyle@shield.ai>
 *
 * This file contains the CheckpointStream class which streams knowledge updates
 * to serialized temporal knowledge (STK) checkpoints.
 **/

namespace madara { namespace knowledge {

/**
 * Implementation of BaseStreamer which writes updates to a Madara checkpoint
 * file. Updates are kept in an in-memory buffer, and written to disk at the
 * hertz rate specified in the constructor.
 **/
class MADARA_EXPORT CheckpointStreamer : public BaseStreamer
{
public:
  /**
   * Constructor.
   *
   * @param settings the CheckpointSettings used in calls to
   *   ThreadSafeContext::save_checkpoint. The variables_lister and
   *   reset_checkpoint fields given are ignored.
   * @param context ThreadSafeContext this object is attached to. This context
   *   will be locked for a short time each period.
   * @param write_hertz hertz rate for periodic write to disk.
   **/
  CheckpointStreamer(
      CheckpointSettings settings,
      ThreadSafeContext &context,
      double write_hertz = 10)
    : settings_(std::move(settings)), context_(&context),
      write_hertz_(write_hertz),
      thread_(thread_main, (keep_running_.test_and_set(), this)) {}

  /**
   * Constructor.
   *
   * @param settings the CheckpointSettings used in calls to
   *   ThreadSafeContext::save_checkpoint. The variables_lister and
   *   reset_checkpoint fields given are ignored.
   * @param kb KnoweldgeBase this object is attached to. This KnoweldgeBase
   *   will be locked for a short time each period.
   * @param write_hertz hertz rate for periodic write to disk.
   **/
  CheckpointStreamer(
      CheckpointSettings settings,
      KnowledgeBase &kb,
      double write_hertz = 10)
    : CheckpointStreamer(std::move(settings), kb.get_context(), write_hertz) {}

  /**
   * Implementation of BaseStreamer::enqueue, which stores the given parameters
   * in an in-memory buffer, for later write to disk.
   **/
  void enqueue(std::string name, KnowledgeRecord record) override;

  // This object spawns a thread which holds a pointer back to this object,
  // so it cannot be safely copied or moved.
  CheckpointStreamer(const CheckpointStreamer &) = delete;
  CheckpointStreamer(CheckpointStreamer &&) = delete;
  CheckpointStreamer &operator=(const CheckpointStreamer &) = delete;
  CheckpointStreamer &operator=(CheckpointStreamer &&) = delete;

  ~CheckpointStreamer() override;

private:
  static void thread_main(CheckpointStreamer *self);

  void terminate()
  {
    keep_running_.clear();
    if (thread_.joinable()) {
      thread_.join();
    }
  }

  CheckpointSettings settings_;
  ThreadSafeContext *context_;

  using pair_type = std::pair<std::string, KnowledgeRecord>;

  std::vector<pair_type> in_buffer;
  std::vector<pair_type> out_buffer;

  double write_hertz_ = 10;

  std::atomic_flag keep_running_;
  std::thread thread_;
};

} } // namespace madara::knowledge

#endif // MADARA_KNOWLEDGE_CHECKPOINT_STREAMER_H_
