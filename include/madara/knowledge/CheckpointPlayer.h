#ifndef MADARA_KNOWLEDGE_CHECKPOINT_PLAYER_H
#define MADARA_KNOWLEDGE_CHECKPOINT_PLAYER_H

#include <fstream>
#include <thread>
#include <memory>

#include "madara/MadaraExport.h"
#include "madara/utility/ScopedArray.h"
#include "madara/knowledge/CheckpointSettings.h"
#include "madara/knowledge/FileHeader.h"
#include "madara/transport/MessageHeader.h"
#include "madara/utility/StlHelper.h"

/**
 * @file CheckpointPlayer.h
 * @author David Kyle <david.kyle@shield.ai>
 *
 * This file provides checkpoint playback capabilities
 **/

namespace madara
{
namespace knowledge
{
/**
 * Class for iterating binary checkpoint files
 **/
class MADARA_EXPORT CheckpointReader
{
public:
  /**
   * Construct using the given CheckpointSettings. Ensure that the
   * referenced object outlives this one.
   **/
  CheckpointReader(CheckpointSettings& in_checkpoint_settings)
    : checkpoint_settings(in_checkpoint_settings),
      logger_(logger::global_logger.get())
  {
  }

  /**
   * Begin by reading any header information. Optional. Will be called
   * automatically by next if need be. This opens the file specified in
   * the settings based during construction. The file will be closed
   * when this object is destructed.
   **/
  void start();

  /**
   * Get the next update from the checkpoint file. Returns an empty
   * string and record if the end is reached.
   **/
  std::pair<std::string, KnowledgeRecord> next();

  /**
   * Get total number of bytes read so far during iteration.
   **/
  int64_t get_total_read() const
  {
    return total_read;
  }

  /**
   * Get the file header information. Only valid after calling start(),
   * or next(). If not valid, returns nullptr.
   **/
  const FileHeader* get_file_header() const
  {
    if (stage == 0)
    {
      return nullptr;
    }

    return &meta;
  }

  /**
   * Check if underlying file is open. This does not imply there are more
   * records left to read.
   **/
  bool is_open() const
  {
    return file.is_open();
  }

  /**
   * Returns CheckpointSettings this reader is using.
   */
  const CheckpointSettings& get_checkpoint_settings() const
  {
    return checkpoint_settings;
  }

private:
  CheckpointSettings& checkpoint_settings;

  logger::Logger* logger_;
  int stage = 0;
  std::ifstream file;
  int64_t total_read = 0;
  FileHeader meta;
  int64_t max_buffer;
  int64_t buffer_remaining;
  utility::ScopedArray<char> buffer;
  char* current;
  size_t checkpoint_start;
  uint64_t state;
  uint64_t checkpoint_size;
  transport::MessageHeader checkpoint_header;
  uint64_t update;
};

/**
 * Plays back a checkpoint over time, based on recorded TOI
 **/
class MADARA_EXPORT CheckpointPlayer
{
public:
  /**
   * Constructor
   *
   * @param context the ThreadSafeContext that will be updated
   * @param settings the CheckpointSettings that will be used to load data
   * @param update_settings the settings that will be used when updating
   *  @a context
   **/
  CheckpointPlayer(ThreadSafeContext& context, CheckpointSettings settings,
      KnowledgeUpdateSettings update_settings = {})
    : context_(&context),
      settings_(std::move(settings)),
      update_settings_(update_settings)
  {
  }

  /**
   * Constructor from CheckpointReader
   *
   * @param context the ThreadSafeContext that will be updated
   * @param reader the CheckpointReader that will be used to load data
   * @param update_settings the settings that will be used when updating
   *  @a context
   **/
  CheckpointPlayer(ThreadSafeContext& context, CheckpointReader reader,
      KnowledgeUpdateSettings update_settings = {})
    : context_(&context),
      settings_(reader.get_checkpoint_settings()),
      update_settings_(update_settings),
      reader_(utility::mk_unique<CheckpointReader>(std::move(reader)))
  {
  }

  ~CheckpointPlayer()
  {
    stop();
  }

  // This object spawns a thread which holds a pointer back to this object,
  // so it cannot be safely copied or moved.
  CheckpointPlayer(const CheckpointPlayer&) = delete;
  CheckpointPlayer(CheckpointPlayer&&) = delete;
  CheckpointPlayer& operator=(const CheckpointPlayer&) = delete;
  CheckpointPlayer& operator=(CheckpointPlayer&&) = delete;

  /**
   * Start playback. If settings.playback_simtime is set, and macro
   * MADARA_FEATURE_SIMTIME is set, current simtime will be set to the TOI
   * of the first record. Rate of playback is equal to the simtime rate, which
   * can be modified during playback.
   **/
  void start()
  {
    keep_running_.test_and_set();
    init_reader();
    thread_ = std::thread(thread_main, this);
  }

  /**
   * Stop playback, killing the playback thread.
   **/
  void stop()
  {
    keep_running_.clear();
    if (thread_.joinable())
    {
      thread_.join();
    }
    reader_.reset();
  }

  /**
   * Loads values from checkpoint until it reaches or exceeds toi given.
   * Do not call while playback is active. Call before calling start().
   *
   * @return true if target_toi is reached before hitting end of checkpoint.
   *         false otherwise.
   **/
  bool play_until(uint64_t target_toi);

private:
  static void thread_main(CheckpointPlayer* self);

  void init_reader()
  {
    if (!reader_)
    {
      reader_ = utility::mk_unique<CheckpointReader>(settings_);
    }
  }

  ThreadSafeContext* context_;
  CheckpointSettings settings_;
  KnowledgeUpdateSettings update_settings_;

  std::atomic_flag keep_running_;
  std::thread thread_;
  std::unique_ptr<CheckpointReader> reader_;
};
}
}  // namespace madara::knowledge

#endif  // MADARA_KNOWLEDGE_CHECKPOINT_PLAYER_H
