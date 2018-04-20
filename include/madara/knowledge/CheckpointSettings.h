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
#include "madara/knowledge/KnowledgeRecord.h"
#include "madara/utility/Utility.h"
#include "madara/filters/BufferFilter.h"

namespace madara
{

  namespace knowledge
  {

    /**
     * @class CheckpointSettings
     * @brief Holds settings for checkpoints to load or save
     **/
    struct CheckpointSettings
    {
      /**
       * Constructor
       **/
      CheckpointSettings ()
        : buffer_size (1024000),
          clear_knowledge (false), initial_timestamp (0), last_timestamp (0),
          initial_lamport_clock (0), last_lamport_clock (0), states (0),
          version (utility::get_version ()),
          override_timestamp (false), override_lamport (false)

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
       **/
      CheckpointSettings (bool t_buffer_size,
        bool t_clear_knowledge,
        std::string t_filename = "",
        uint64_t t_initial_timestamp = 0,
        uint64_t t_last_timestamp = 0,
        uint64_t t_initial_lamport_clock = 0,
        uint64_t t_last_lamport_clock = 0,
        std::string t_originator = "",
        const std::vector <std::string> & t_prefixes = {},
        uint64_t t_states = 0,
        std::string t_version = "",
        bool t_override_timestamp = false,
        bool t_override_lamport = false)
        : buffer_size (t_buffer_size),
          clear_knowledge (t_clear_knowledge),
          filename (t_filename),
          initial_timestamp (t_initial_timestamp),
          last_timestamp (t_last_timestamp),
          initial_lamport_clock (t_initial_lamport_clock),
          last_lamport_clock (t_last_lamport_clock),
          originator (t_originator),
          prefixes (t_prefixes),
          states (t_states),
          version (t_version),
          override_timestamp (t_override_timestamp),
          override_lamport (t_override_lamport)
      {
      }
       
      /**
       * Copy constructor
       * @param   rhs   settings instance to copy
       **/
      CheckpointSettings (const CheckpointSettings & rhs)
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
          buffer_filters (rhs.buffer_filters)
      {
      }

      /**
      * Calls encode on the the buffer filter chain
      * @param   source           the source and destination buffer
      * @param   size             the amount of data in the buffer in bytes
      * @param   max_size         the amount of bytes the buffer can hold
      * @return  the new size after encoding
      **/
      int encode (
        unsigned char * source, int size, int max_size) const
      {
        // encode from front to back
        for (filters::BufferFilters::const_iterator i = buffer_filters.begin ();
          i != buffer_filters.end (); ++i)
        {
          size = (*i)->encode (source, size, max_size);
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
      int decode (
        unsigned char * source, int size, int max_size) const
      {
        // decode from back to front
        for (filters::BufferFilters::const_reverse_iterator i = buffer_filters.rbegin ();
          i != buffer_filters.rend (); ++i)
        {
          size = (*i)->decode (source, size, max_size);
        }

        return size;
      }


      /**
       * the size of the buffer needed for the checkpoint
       **/
      size_t      buffer_size;

      /**
       * If true, during loads, clear the KnowledgeBase first
       **/
      bool        clear_knowledge;

      /**
       * path to files
       **/
      std::string filename;

      /**
       * initial wallclock time saved in the checkpoint
       **/
      uint64_t   initial_timestamp;

      /**
       * final wallclock time saved in the checkpoint
       **/
      uint64_t   last_timestamp;

      /**
       * initial lamport clock saved in the checkpoint
       **/
      uint64_t   initial_lamport_clock;

      /**
       * final lamport clock saved in the checkpoint
       **/
      uint64_t   last_lamport_clock;

      /**
       * the originator id of the checkpoint
       **/
      std::string originator;

      /**
       * A list of prefixes to save/load. If empty, all prefixes are valid.
       **/
      std::vector <std::string> prefixes;

      /**
       * the number of states checkpointed in the file stream
       **/
      uint64_t   states;

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
      filters::BufferFilters   buffer_filters;
    };
  }
}
#endif //_MADARA_KNOWLEDGE_CHECKPOINTSETTINGS_H_
