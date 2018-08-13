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

class MADARA_EXPORT CheckpointStreamer : public BaseStreamer
{
public:
  CheckpointStreamer(
      CheckpointSettings settings,
      ThreadSafeContext &context,
      double write_hertz = 10)
    : settings_(std::move(settings)), context_(&context),
      write_hertz_(write_hertz),
      thread_(thread_main, (keep_running_.test_and_set(), this)) {}

  CheckpointStreamer(
      CheckpointSettings settings,
      KnowledgeBase &kb,
      double write_hertz = 10)
    : CheckpointStreamer(std::move(settings), kb.get_context(), write_hertz) {}

  void enqueue(std::string name, KnowledgeRecord record) override;

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
