#ifndef _MADARA_KNOWLEDGE_FILE_REQUESTER_H_
#define _MADARA_KNOWLEDGE_FILE_REQUESTER_H_

#include <string>
#include <fstream>
#include <vector>
#include <stdio.h>
#include "madara/knowledge/KnowledgeUpdateSettings.h"
#include "madara/knowledge/KnowledgeRecord.h"
#include "madara/utility/Utility.h"
#include "madara/utility/ScopedArray.h"
#include "madara/knowledge/containers/FlexMap.h"
#include "madara/logger/GlobalLogger.h"

namespace madara
{
  namespace knowledge
  {
    /**
     * @class FileRequester
     * @brief A helper class that can reconstruct files or request files
     * be transferred in fragments from a Madara File Service (MFS)
     */
    class FileRequester
    {
    public:
      /**
       * Constructor
       **/
      FileRequester ()
      {
      }

      /**
       * Constructor
       * @param prefix     the prefix in the knowledge base that precedes .crc
       *                   and .size keys (e.g. from MFS)
       * @param sync_key   the key in the knowledge base that is used to send
       *                   sync requests (e.g. to MFS)
       * @param filename   the file that should be recreated from fragments
       * @param kb         the knowledge base where the sync key is located at
       * @param max_request_fragments  the most fragments that can be requested
       *                   in a single send_modifieds
       **/
      FileRequester (
        const std::string & prefix,
        const std::string & sync_key,
        const std::string & filename,
        KnowledgeBase kb, int max_request_fragments = -1)
      {
        init (prefix, sync_key, filename, kb, max_request_fragments);
      }

      /**
       * Initializes the requester
       * @param prefix     the prefix in the knowledge base that precedes .crc
       *                   and .size keys (e.g. from MFS)
       * @param sync_key   the key in the knowledge base that is used to send
       *                   sync requests (e.g. to MFS)
       * @param filename   the file to open and read from
       * @param kb         the knowledge base to stream to
       * @param max_request_fragments  the most fragments that can be requested
       *                   in a single send_modifieds
       **/
      inline void init (
        const std::string & prefix,
        const std::string & sync_key, const std::string & filename,
        KnowledgeBase kb, int max_request_fragments = -1)
      {
        // set the sync key
        sync_.set_name (sync_key, kb);

        // records that contain the file fragments
        containers::FlexMap file_space;

        // setup containers
        file_space.set_name (prefix, kb);
        file_space["size"].to_container (file_size_);
        file_space["crc"].to_container (file_crc_);

        // set the filename
        filename_ = filename;

        // set the max fragments
        max_fragments = max_request_fragments;

        kb_ = kb;
      }

      /**
       * Returns the crc of the file
       * @return the crc of the file
       **/
      inline uint32_t get_crc (void)
      {
        return (uint32_t) file_crc_.to_integer ();
      }

      /**
       * Returns the size of the file
       * @return the size of the file
       **/
      inline size_t get_size (void)
      {
        return (size_t) file_size_.to_integer ();
      }

      /**
       * Returns the name of the file
       * @return the name of the file being streamed
       **/
      inline std::string get_filename (void)
      {
        return filename_;
      }

      /**
       * Returns the percentage of transfer that is completed
       * @return the transfer completion percentage
       **/
      inline double get_percent_complete (void)
      {
        double result = 0;

        size_t expected_size = get_size ();
        size_t num_fragments = expected_size / 60000;

        if (expected_size % 60000 > 0)
        {
          ++num_fragments;
        }

        if (num_fragments > 0)
        {
          result = (double)utility::get_file_progress (
            filename_, get_crc (), get_size (), max_fragments)
            / (double)get_size ();

          result *= 100;
        }

        return result;
      }

      /**
       * Builds fragment request to send 
       * @return the list of missing fragments for the file
       **/
      inline std::vector<int64_t> build_fragment_request (void)
      {
        return utility::get_file_missing_fragments (
          filename_, get_crc (), get_size (), max_fragments);
      }

      /**
       * Builds fragment request to send. If the return is true, call
       * send_modifieds on the KB to send the request.
       * @return true if a request is necessary, false if file is complete
       **/
      inline bool needs_request (void)
      {
        if (get_crc () != 0 && get_size () != 0)
        {
          std::vector<int64_t> fragments = build_fragment_request ();
          sync_.set (fragments);

          return fragments.size () != 0;
        }
        else
        {
          std::vector<int64_t> fragments;

          if(max_fragments > 0)
          {
            fragments.resize (max_fragments);

            for (size_t i = 0; i < fragments.size (); ++i)
            {
              fragments[i] = i;
            }
          }

          sync_.set (fragments);
  
          return true;
        }
      }

       /**
       * Clears any lingering file fragments
       **/
      inline void clear_fragments (void)
      {
        std::string str_crc = std::to_string ((unsigned long)get_crc ());
        std::string frag_suffix = "." + str_crc + ".frag";
        std::string frag_file = filename_ + ".0" + frag_suffix;

        size_t expected_size = get_size ();
        size_t num_fragments = expected_size / 60000;

        if (expected_size % 60000 > 0)
        {
          ++num_fragments;
        }

        for (int i = 0; i < (int)num_fragments; ++i,
          frag_file = filename_ + "." + std::to_string (i) + frag_suffix)
        {
          if (utility::file_exists (frag_file))
          {
            madara_logger_ptr_log (logger::global_logger.get (),
              logger::LOG_MAJOR,
              "FileRequester::clear_fragments: removing %s\n",
              frag_file.c_str ());

            remove (frag_file.c_str ());
          }
        }
      }

       /**
       * Remodifies the file size and crc
       **/
      inline void modify (void)
      {
        sync_.modify ();
      }

      /// the maximum fragments allowed in a resend request
      int max_fragments;

    private:

      /**
       * The key that is being synced to
       **/
      containers::NativeIntegerVector sync_;

      /// the size of the file contents
      containers::Integer file_size_;

      /// the crc of the file
      containers::Integer file_crc_;

      /// the name of the file on the hard drive being reconstructed
      std::string filename_;

      /// saves the kb for general usage
      knowledge::KnowledgeBase kb_;
    };
  }
}

#endif // _MADARA_KNOWLEDGE_FILE_REQUESTER_H_
