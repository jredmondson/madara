#ifndef _MADARA_KNOWLEDGE_CHECKPOINTSETTINGS_H_
#define _MADARA_KNOWLEDGE_CHECKPOINTSETTINGS_H_

/**
 * @file CheckpointSettings.h
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains the CheckpointSettings class, which
 * provides settings for saving/loading KnowledgeBase checkpoints
 */

#include <string>
#include <map>
#include <memory>

#include "madara/knowledge/KnowledgeRecord.h"
#include "madara/utility/Utility.h"
#include "madara/filters/BufferFilter.h"
#include "madara/filters/BufferFilterHeader.h"
#include <stdio.h>

namespace madara
{
namespace knowledge
{
class ThreadSafeContext;
class VariablesLister;

/**
 * @class CheckpointSettings
 * @brief Holds settings for checkpoints to load or save. Most of the
 *   data members are "smart" data members. For loads, many of the fields
 *   are essentially pass-by-reference and fill with the related data.
 **/
class CheckpointSettings
{
public:
  /**
   * Allow for ThreadSafeContext to update private data members
   **/
  friend ThreadSafeContext;

  /**
   * Constructor
   **/
  CheckpointSettings()
    : buffer_size(1024000),
      clear_knowledge(false),
      filename(""),
      initial_timestamp(0),
      last_timestamp(0),
      initial_lamport_clock(0),
      last_lamport_clock(0),
      states(0),
      version(utility::get_version()),
      override_timestamp(false),
      override_lamport(false),
      keep_open(false),
      initial_state(0),
      last_state(-1),
      reset_checkpoint(true),
      ignore_header_check(false)
  {
  }

  /**
   * Constructor
   * @param  t_buffer_size       size in bytes to allocate for a buffer
   * @param  t_clear_knowledge   if true, during loads, clear knowledge
   * @param  t_filename          the filename, including path to load/save
   * @param  t_originator        the originator id
   * @param  t_initial_timestamp the wallclock time the checkpoint started
   * @param  t_last_timestamp    the wallclock time the checkpoint ended
   * @param  t_initial_lamport_clock the clock when checkpoint started
   * @param  t_last_lamport_clock the clock when checkpoint ended
   * @param  t_prefixes          the prefixes to save or load (empty = all)
   * @param  t_states            number of states in checkpoint
   * @param  t_version           the version of the checkpoint
   * @param  t_override_timestamp  override the timestamps
   * @param  t_override_lamport  override the lamport clocks
   * @param  t_keep_open         attempt to keep the file open for later
   * @param  t_initial_state     the initial state to query/save
   * @param  t_last_state        the last state to query/save
   * @param  t_reset_checkpoint  reset the checkpoint modifieds
   * @param  t_ignore_header_check  if true, ignore header checks
   * @param  t_max_buffer_size   the max size in bytes for buffer growth
   * @param  t_variables_lister  a custom list of variables to print
   **/
  CheckpointSettings(size_t t_buffer_size, bool t_clear_knowledge,
      std::string t_filename = "", uint64_t t_initial_timestamp = 0,
      uint64_t t_last_timestamp = 0, uint64_t t_initial_lamport_clock = 0,
      uint64_t t_last_lamport_clock = 0, std::string t_originator = "",
      const std::vector<std::string>& t_prefixes = {}, uint64_t t_states = 0,
      std::string t_version = "", bool t_override_timestamp = false,
      bool t_override_lamport = false, bool t_keep_open = false,
      uint64_t t_initial_state = 0, uint64_t t_last_state = (uint64_t)-1,
      bool t_reset_checkpoint = true, bool t_ignore_header_check = false,
      VariablesLister* t_variables_lister = nullptr,
      size_t t_max_buffer_size = 2000000000)
    : buffer_size(t_buffer_size),
      clear_knowledge(t_clear_knowledge),
      filename(t_filename),
      initial_timestamp(t_initial_timestamp),
      last_timestamp(t_last_timestamp),
      initial_lamport_clock(t_initial_lamport_clock),
      last_lamport_clock(t_last_lamport_clock),
      originator(t_originator),
      prefixes(t_prefixes),
      states(t_states),
      version(t_version),
      override_timestamp(t_override_timestamp),
      override_lamport(t_override_lamport),
      keep_open(t_keep_open),
      initial_state(t_initial_state),
      last_state(t_last_state),
      reset_checkpoint(t_reset_checkpoint),
      ignore_header_check(t_ignore_header_check),
      variables_lister(t_variables_lister),
      max_buffer_size(t_max_buffer_size)
  {
  }

  /**
   * Copy constructor
   * @param   rhs   settings instance to copy
   **/
  CheckpointSettings(const CheckpointSettings& rhs) = default;
  /*
    : buffer_size (rhs.buffer_size),
      clear_knowledge (rhs.clear_knowledge),
      filename (rhs.filename),
      initial_timestamp (rhs.initial_timestamp),
      last_timestamp (rhs.last_timestamp),
      initial_lamport_clock (rhs.initial_lamport_clock),
      last_lamport_clock (rhs.last_lamport_clock),
      originator (rhs.originator),
      prefixes (rhs.prefixes),
      states (rhs.states),
      version (rhs.version),
      override_timestamp (rhs.override_timestamp),
      override_lamport (rhs.override_lamport),
      buffer_filters (rhs.buffer_filters),
      keep_open (rhs.keep_open),
      initial_state (rhs.initial_state),
      last_state (rhs.last_state),
      reset_checkpoint (rhs.reset_checkpoint),
      ignore_header_check (rhs.ignore_header_check),
      checkpoint_file (rhs.checkpoint_file)
  {
  }*/

  /**
   * Destructor
   **/
  ~CheckpointSettings() = default;

  /**
   * Calls encode on the the buffer filter chain
   * @param   source           the source and destination buffer
   * @param   size             the amount of data in the buffer in bytes
   * @param   max_size         the amount of bytes the buffer can hold
   * @return  the new size after encoding
   **/
  int encode(char* source, int size, int max_size) const
  {
    // encode from front to back
    for (filters::BufferFilters::const_iterator i = buffer_filters.begin();
         i != buffer_filters.end(); ++i)
    {
      madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_MINOR,
          "CheckpointSettings::encode: size before encode: "
          " %d of %d\n",
          size, max_size);

      size = (*i)->encode(source, size, max_size);

      madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_MINOR,
          "CheckpointSettings::encode: size after encode: "
          " %d of %d\n",
          size, max_size);

      if (max_size > size + 20)
      {
        memmove(source + 20, source, size);

        filters::BufferFilterHeader header;
        header.read(*i);
        header.size = (uint64_t)size;

        int64_t buffer_remaining = 20;

        header.write((char*)source, buffer_remaining);

        size += (int)filters::BufferFilterHeader::encoded_size();

        madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_MAJOR,
            "CheckpointSettings::encode: header: "
            "%s:%s within size %d\n",
            header.id, utility::to_string_version(header.version).c_str(),
            size);
      }
      else
      {
        std::stringstream buffer;
        buffer << "CheckpointSettings::encode: ";
        buffer << (size + 20) << " 20 byte size encoding cannot fit in ";
        buffer << max_size << " byte buffer\n";

        throw exceptions::MemoryException(buffer.str());
      }
    }

    return size;
  }

  /**
   * Calls decode on the the buffer filter chain
   * @param   source           the source and destination buffer
   * @param   size             the amount of data in the buffer in bytes
   * @param   max_size         the amount of bytes the buffer can hold
   * @return  the new size after encoding
   **/
  int decode(char* source, int size, int max_size) const
  {
    if (buffer_filters.size() == 0 && !ignore_header_check)
    {
      // if we don't have buffer filters, do a check to see if we should
      filters::BufferFilterHeader header;
      int64_t local_buffer_size =
          (int64_t)filters::BufferFilterHeader::encoded_size();

      header.read((char*)source, local_buffer_size);

      header.id[4] = 0;

      std::string header_id(header.id);

      // id is either karl or KaRL. If it's anything else, then error
      if (header_id == "karl" || header_id == "KaRL")
      {
        madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_MAJOR,
            "CheckpointSettings::decode: header: "
            " Detected %s\n",
            header.id);
      }
      else
      {
        madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_MAJOR,
            "CheckpointSettings::decode: header: "
            " Detected %s, which is not a message or checkpoint header\n",
            header.id);

        return 0;
      }
    }

    // decode from back to front
    for (filters::BufferFilters::const_reverse_iterator i =
             buffer_filters.rbegin();
         i != buffer_filters.rend(); ++i)
    {
      if (size > (int)filters::BufferFilterHeader::encoded_size())
      {
        filters::BufferFilterHeader header;
        int64_t local_buffer_size =
            (int64_t)filters::BufferFilterHeader::encoded_size();

        header.read((char*)source, local_buffer_size);

        if (header.size > (uint64_t)max_size)
        {
          std::stringstream buffer;
          buffer << "CheckpointSettings::decode: ";
          buffer << header.size << " byte size encoding cannot fit in ";
          buffer << max_size << " byte buffer\n";

          throw exceptions::MemoryException(buffer.str());
        }

        madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_MAJOR,
            "CheckpointSettings::decode: header: "
            " %s:%s\n",
            header.id, utility::to_string_version(header.version).c_str());

        if (*i == 0)
        {
          madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_ERROR,
              "CheckpointSettings::decode: filter is null somehow\n");

          return 0;
        }
        else
        {
          madara_logger_ptr_log(logger::global_logger.get(),
              logger::LOG_DETAILED,
              "CheckpointSettings::decode: filter is not null\n");
        }

        if (ignore_header_check || header.check_filter(*i))
        {
          madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_MAJOR,
              "CheckpointSettings::decode: buffer filter %s is a match\n",
              header.id);
        }
        else
        {
          madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_ERROR,
              "CheckpointSettings::decode: buffer filter %s doesn't match."
              " Returning 0.\n",
              header.id);

          return 0;
        }

        madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_MINOR,
            "CheckpointSettings::decode: size before decode: "
            " %d of %d (header.size=%d)\n",
            size, max_size, (int)header.size);

        size =
            (*i)->decode(source + filters::BufferFilterHeader::encoded_size(),
                (int)header.size, max_size);

        madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_MINOR,
            "CheckpointSettings::decode: size after decode: "
            " %d of %d (header.size=%d)\n",
            size, max_size, (int)header.size);

        if (size > 0)
        {
          memmove(source, source + filters::BufferFilterHeader::encoded_size(),
              size);
        }
      }
      else
      {
        std::stringstream buffer;
        buffer << "CheckpointSettings::decode: ";
        buffer << size << " byte size encoding cannot fit in ";
        buffer << max_size << " byte buffer\n";

        throw exceptions::MemoryException(buffer.str());
      }
    }

    return size;
  }

  /**
   * the size of the buffer needed for the checkpoint
   **/
  size_t buffer_size;

  /**
   * If true, during loads, clear the KnowledgeBase first
   **/
  bool clear_knowledge;

  /**
   * path to files
   **/
  std::string filename;

  /**
   * initial wallclock time saved in the checkpoint
   **/
  uint64_t initial_timestamp;

  /**
   * final wallclock time saved in the checkpoint
   **/
  uint64_t last_timestamp;

  /**
   * initial lamport clock saved in the checkpoint
   **/
  uint64_t initial_lamport_clock;

  /**
   * final lamport clock saved in the checkpoint
   **/
  uint64_t last_lamport_clock;

  /**
   * the originator id of the checkpoint
   **/
  std::string originator;

  /**
   * A list of prefixes to save/load. If empty, all prefixes are valid.
   **/
  std::vector<std::string> prefixes;

  /**
   * the number of states checkpointed in the file stream
   **/
  uint64_t states;

  /**
   * the MADARA version
   **/
  std::string version;

  /**
   * use the timestamps in this class instead of current wallclock time
   * when writing context or checkpoints
   **/
  bool override_timestamp;

  /**
   * use the lamport clocks in this class instead of KB clock when writing
   * context or checkpoints
   **/
  bool override_lamport;

  /**
   * buffer filters. Note that the user must clean up memory of all filters
   **/
  filters::BufferFilters buffer_filters;

  /**
   * if true, keep the file open to avoid open/close overhead when
   * programmatically iterating through checkpoints. This is sort of
   * useful with save_checkpoint
   **/
  bool keep_open;

  /**
   * the initial state number of interest (useful for loading ranges of
   * checkpoint states). This is an inclusive identifier, so 0 means to
   * load from initial context/checkpoint save
   **/
  uint64_t initial_state;

  /**
   * the last state number of interest (useful for loading ranges of
   * checkpoint states. This is an inclusive identifier. If last_state
   * >= states, it will essentially indicate the last valid state.
   **/
  uint64_t last_state;

  /**
   * If true, resets the checkpoint to start a new diff from this point
   * forward.
   **/
  bool reset_checkpoint;

  /**
   * If true, do not perform a header check. This is useful if you are
   * loading an arbitrary text file with no buffer filters
   **/
  bool ignore_header_check;

  /**
   * If true, update simtime during playback to match recorded TOI. Only
   * operable if MADARA_FEATURE_SIMTIME macro is defined.
   **/
  bool playback_simtime = false;

  /**
   * Object which will be used to extract variables for checkpoint saving.
   * By default (if left nullptr), use a default implementation which uses
   * ThreadSaveContext::get_local_modified
   *
   * This object is not owned, so must exist as long as this settings
   * obejct exists
   **/
  VariablesLister* variables_lister = nullptr;

  /**
   * the max size the buffer can grow to
   **/
  size_t max_buffer_size = 2000000000;

private:
  /**
   * a thread-safe ref-counted file handle for quick access to an open
   * checkpoint binary file
   **/
  std::shared_ptr<FILE> checkpoint_file;
};

class VariablesLister
{
public:
  virtual ~VariablesLister() = default;

  virtual void start(const CheckpointSettings& settings) = 0;
  virtual std::pair<const char*, const KnowledgeRecord*> next() = 0;
};
}
}
#endif  //_MADARA_KNOWLEDGE_CHECKPOINTSETTINGS_H_
