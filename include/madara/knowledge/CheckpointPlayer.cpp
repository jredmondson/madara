#include <fstream>
#include <chrono>

#include "madara/logger/GlobalLogger.h"
#include "madara/exceptions/MemoryException.h"
#include "madara/exceptions/FileException.h"
#include "madara/exceptions/FilterException.h"
#include "madara/utility/Utility.h"
#include "madara/utility/SimTime.h"

#include "madara/transport/Transport.h"

#include "CheckpointPlayer.h"

namespace sc = std::chrono;

namespace madara { namespace knowledge {

void CheckpointReader::start()
{
  if (stage != 0) {
    return;
  }

  madara_logger_ptr_log (logger_, logger::LOG_MAJOR,
    "ThreadSafeContext::load_context:" \
    " opening file %s\n", checkpoint_settings.filename.c_str ());

  file.open(checkpoint_settings.filename.c_str (),
    std::ios::in | std::ios::binary);

  if (!file)
  {
    madara_logger_ptr_log (logger_, logger::LOG_ALWAYS,
      "ThreadSafeContext::load_context:" \
      " could not open file %s for reading. "
      "Check that file exists and that permissions are appropriate.\n",
      checkpoint_settings.filename.c_str ());
    stage = 9;
    return;
  }

  max_buffer = checkpoint_settings.buffer_size;
  buffer_remaining = max_buffer;

  buffer = new char[max_buffer];
  current = buffer.get_ptr ();

  file.seekg (0, file.end);
  int length = file.tellg();
  file.seekg (0, file.beg);

  madara_logger_ptr_log (logger_, logger::LOG_MINOR,
    "ThreadSafeContext::load_context:" \
    " file contains %d bytes.\n",
    (int)length);

  if (!file.read (buffer.get (), FileHeader::encoded_size ()))
  {
    std::stringstream message;
    message << "ThreadSafeContext::load_context: ";
    message << "file ";
    message << checkpoint_settings.filename;
    message << " does not have enough room for an appropriate header";
    throw exceptions::FileException (message.str ());
  }
  total_read = file.tellg();

  buffer_remaining = (int64_t)total_read;

  madara_logger_ptr_log (logger_, logger::LOG_MINOR,
    "ThreadSafeContext::load_context:" \
    " reading file: %d bytes read.\n",
    (int)total_read);

  checkpoint_start = (size_t)FileHeader::encoded_size ();

  if (total_read < FileHeader::encoded_size () ||
    !FileHeader::file_header_test (current))
  {
    madara_logger_ptr_log (logger_, logger::LOG_MINOR,
      "ThreadSafeContext::load_context:" \
      " invalid file. No contextual change.\n");
    stage = 9;
    return;
  }

  // if there was something in the file, and it was the right header

  current = (char *)meta.read (current, buffer_remaining);

  checkpoint_settings.initial_timestamp = meta.initial_timestamp;
  checkpoint_settings.last_timestamp = meta.last_timestamp;
  checkpoint_settings.originator = meta.originator;
  checkpoint_settings.states = meta.states;
  checkpoint_settings.version = utility::to_string_version (
    meta.karl_version);

  madara_logger_ptr_log (logger_, logger::LOG_MINOR,
            "ThreadSafeContext::load_context:" \
            " read File meta. Meta.size=%d. Meta.states=%d\n",
            (int)meta.size, (int)meta.states);

  /**
  * check that there is more than one state and that the rest of
  * the file is sufficient to at least be a message header (what
  * we use as a checkpoint header
  **/
  if (meta.states == 0)
  {
    stage = 9;
    return;
  }

  stage = 1;
  state = 0;
}

std::pair<std::string, KnowledgeRecord> CheckpointReader::next()
{
  if (stage == 0) {
    start();
  }

  if (stage == 9) {
    return {};
  }

  // Outer loop for progressing through stages
  for (;;) {
    // We're iterating to next state
    if (stage == 1)
    {
      if (state >= meta.states || state > checkpoint_settings.last_state) {
        madara_logger_ptr_log (logger_, logger::LOG_MINOR,
          "ThreadSafeContext::load_context:" \
          " done at state=%d of meta.states=%d\n",
          (int)state, (int)meta.states);
        stage = 9;
        return {};
      }

      madara_logger_ptr_log (logger_, logger::LOG_MINOR,
        "ThreadSafeContext::load_context:" \
        " reading 64bit unsigned size at %d byte file offset\n",
        (int)checkpoint_start);

      // set the file pointer to the checkpoint header start
      //fseek (file, (long)checkpoint_start, SEEK_SET);
      file.seekg (checkpoint_start, file.beg);

      if (!file.read ((char *)&checkpoint_size, sizeof (checkpoint_size)))
      {
        std::stringstream message;
        message << "ThreadSafeContext::load_context: ";
        message << "file ";
        message << checkpoint_settings.filename;
        message << " does not have enough room for a checkpoint";
        throw exceptions::FileException (message.str ());
      }

      // total_read = fread (&checkpoint_size,
      //   1, sizeof (checkpoint_size), file);

      total_read = sizeof (checkpoint_size);

      checkpoint_size = utility::endian_swap (checkpoint_size);

      if (checkpoint_settings.buffer_filters.size () > 0)
      {
        checkpoint_size += filters::BufferFilterHeader::encoded_size ();
      }

      madara_logger_ptr_log (logger_, logger::LOG_MINOR,
        "ThreadSafeContext::load_context:" \
        " %d state checkpoint size is %d\n",
        (int)state, (int)checkpoint_size);

      // set the file pointer to the checkpoint header start
      file.seekg (checkpoint_start, file.beg);

      checkpoint_start += checkpoint_size;

      madara_logger_ptr_log (logger_, logger::LOG_MINOR,
        "ThreadSafeContext::load_context:" \
        " reading %d bytes for full checkpoint\n",
        (int)checkpoint_size);

      if (!file.read (buffer.get (), checkpoint_size))
      {
        std::stringstream message;
        message << "ThreadSafeContext::load_context: ";
        message << "file ";
        message << checkpoint_settings.filename;
        message << " does not have enough room for ";
        message << checkpoint_size;
        message << " bytes noted in header";
        throw exceptions::FileException (message.str ());
      }
      total_read = (int64_t)checkpoint_size;

      current = buffer.get_ptr ();

      madara_logger_ptr_log (logger_, logger::LOG_MINOR,
        "ThreadSafeContext::load_context:" \
        " read %d bytes\n",
        (int)total_read);

      buffer_remaining = (int64_t)total_read;

      madara_logger_ptr_log (logger_, logger::LOG_MINOR,
        "ThreadSafeContext::load_context:" \
        " decoding with %d buffer filters with initial size of "
        "%d bytes and total buffer of %d bytes\n",
        (int)checkpoint_settings.buffer_filters.size (),
        (int)total_read, (int)max_buffer);

      // call decode with any buffer filters
      buffer_remaining = (int64_t)checkpoint_settings.decode (current,
        (int)total_read, (int)max_buffer);

      if (buffer_remaining <= 0)
      {
        stage = 9;
        throw exceptions::FilterException (
          "ThreadSafeContext::load_context: "
          "decode () returned a negative encoding size. Bad filter/encode.");
      }

      if (buffer_remaining <= (int64_t)
        transport::MessageHeader::static_encoded_size ())
      {
        stage = 9;
        throw exceptions::MemoryException (
          "ThreadSafeContext::load_context: "
          "Not enough room in buffer for message header"
        );
      }

      madara_logger_ptr_log (logger_, logger::LOG_MINOR,
        "ThreadSafeContext::load_context:" \
        " Reading a checkpoint header with %d byte buffer remaining\n",
        (int)buffer_remaining);

      current = (char *)checkpoint_header.read (current, buffer_remaining);

      if (state == 0)
      {
        checkpoint_settings.initial_lamport_clock = checkpoint_header.clock;
      }

      if (state == meta.states - 1)
      {
        checkpoint_settings.last_lamport_clock = checkpoint_header.clock;
      }

      uint64_t updates_size = checkpoint_header.size -
        checkpoint_header.encoded_size ();

      madara_logger_ptr_log (logger_, logger::LOG_MINOR,
          "ThreadSafeContext::load_context:" \
          " read Checkpoint header. header.size=%d, updates.size=%d\n",
          (int)checkpoint_header.size, (int)updates_size);

      /**
      * What we read into the checkpoint_header will dictate our
      * max_buffer. We want to make this checkpoint_header size into
      * something reasonable.
      **/
      if (updates_size > (uint64_t)buffer_remaining)
      {
        throw exceptions::MemoryException (
          "ThreadSafeContext::load_context: "
          "Not enough room in buffer for checkpoint"
        );
      } // end if allocation is needed

      madara_logger_ptr_log (logger_, logger::LOG_MINOR,
          "ThreadSafeContext::load_context:" \
          " state=%d, initial_state=%d, last_state=%d\n",
          (int)state, (int)checkpoint_settings.initial_state,
          (int)checkpoint_settings.last_state);

      if (state <= checkpoint_settings.last_state &&
          state >= checkpoint_settings.initial_state) {
        stage = 2;
        update = 0;
      } else {
        madara_logger_ptr_log (logger_, logger::LOG_MINOR,
          "ThreadSafeContext::load_context:" \
          " not a valid state, incrementing by %d bytes.\n",
          (int)updates_size);

        current += updates_size;
      }
      ++state;
    }

    // We're iterating to next update
    if (stage == 2)
    {
      if (update >= checkpoint_header.updates) {
        stage = 1;
        continue;
      }

      std::string key;
      knowledge::KnowledgeRecord record;
      record.clock = checkpoint_header.clock;
      record.toi = checkpoint_settings.last_timestamp;
      current = (char *)record.read (current, key, buffer_remaining);

      madara_logger_ptr_log (logger_, logger::LOG_MINOR,
        "ThreadSafeContext::load_context:" \
        " read record (%d of %d): %s\n",
        (int)update, (int)checkpoint_header.updates, key.c_str ());

      // check if the prefix is allowed
      if (checkpoint_settings.prefixes.size () > 0)
      {
        bool prefix_found = false;
        for (size_t j = 0; j < checkpoint_settings.prefixes.size ()
                           && !prefix_found; ++j)
        {
          madara_logger_ptr_log (logger_, logger::LOG_MINOR,
            "ThreadSafeContext::load_context:" \
            " checking record %s against prefix %s\n",
            key.c_str (), checkpoint_settings.prefixes[j].c_str ());

          if (madara::utility::begins_with (
                key, checkpoint_settings.prefixes[j]))
          {
            madara_logger_ptr_log (logger_, logger::LOG_MINOR,
              "ThreadSafeContext::load_context:" \
              " record has the correct prefix.\n");

            prefix_found = true;
          } // end if prefix success
        } // end for all prefixes

        if (!prefix_found)
        {
          madara_logger_ptr_log (logger_, logger::LOG_MINOR,
            "ThreadSafeContext::load_context:" \
            " record does not have the correct prefix. Rejected.\n");

          ++update;
          continue;
        } // end if prefix found
      } // end if there are prefixes in the checkpoint settings

      ++update;
      return {key, record};
    } // end for all updates
  }
}

void CheckpointPlayer::thread_main(CheckpointPlayer *self)
{
  uint64_t first_toi = -1UL;
  uint64_t prev_toi = -1UL;

  while (self->keep_running_.test_and_set()) {
    auto cur = self->reader_->next();
    if (cur.first == "") {
      break;
    }

    madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_MINOR,
      "CheckpointPlayer:" \
      " record has toi %lu. prev: %lu. first: %lu\n",
      cur.second.toi, prev_toi, first_toi);

    if (first_toi == -1UL)
    {
      first_toi = cur.second.toi;
      prev_toi = first_toi;
#ifdef MADARA_FEATURE_SIMTIME
      if (self->settings_.playback_simtime) {
        utility::sim_time_notify(first_toi, NAN);
      }
#endif
    }

#ifdef MADARA_FEATURE_SIMTIME
    uint64_t wait_time = utility::SimTime::duration(cur.second.toi - prev_toi);
#else
    uint64_t wait_time = cur.second.toi - prev_toi;
#endif

    utility::SecondsDuration sec_wait = sc::nanoseconds{wait_time};

    madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_MINOR,
      "CheckpointPlayer: wait_time: %lu (%f s)\n", wait_time,
      sec_wait.count());

    utility::sleep(sec_wait);

    prev_toi = cur.second.toi;

    self->context_->update_record_from_external (cur.first, cur.second, self->update_settings_);

  }
}

} } // namespace madara::knowledge
