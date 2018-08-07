#ifndef _MADARA_UTILITY_FILEFRAGMENTER_H_
#define _MADARA_UTILITY_FILEFRAGMENTER_H_

#include <string>
#include <vector>
#include "madara/knowledge/KnowledgeRecord.h"
#include "madara/utility/Utility.h"

namespace madara
{
  namespace utility
  {
    /**
     * @class FileFragmenter
     * @brief Enforces a periodic epoch
     */
    class FileFragmenter
    {
    public:
      /**
       * Constructor
       * @param  buffer     the file contents
       * @param  size       the size of the buffer in bytes
       * @param  frag_size  the max size of fragments to create
       **/
      FileFragmenter (char * buffer, size_t size, size_t frag_size = 600000)
      {
        fragment_buffer (buffer, size, frag_size);
      }

      /**
       * Constructor
       * @param  filename   the file to read in and fragment
       * @param  frag_size  the max size of fragments to create
       **/
      FileFragmenter (const std::string & filename, size_t frag_size = 600000)
      {
        fragment_file (filename, frag_size);
      }

      /**
       * Fragments a file into smaller buffers up to a specified size
       * @param  filename   the file to read in and fragment
       * @param  frag_size  the max size of fragments to create
       * @return number of fragments that resulted
       **/
      inline size_t fragment_file (
        const std::string & filename, size_t frag_size = 600000)
      {
        void * buffer;

        if (utility::read_file (filename, buffer, file_size))
        {
          file_contents = <char *>buffer;
          return fragment_buffer (file_contents.get (), file_size, frag_size);
        }

        return 0;
      }

      /**
       * Fragments a file into smaller buffers up to a specified size
       * @param  buffer     the file contents
       * @param  size       the size of the buffer in bytes
       * @param  frag_size  the max size of fragments to create
       **/
      inline size_t fragment_buffer (char * buffer, size_t size,
        size_t frag_size = 600000)
      {
        size_t num_frags = size / frag_size;

        if (size % frag_size != 0)
          ++num_frags;

        records.resize (num_frags);

        // need to then emplace each buffer into the records
        
      }

      /// records that contain the file fragments
      std::vector <knowledge::KnowledgeRecord> records;

      /// the size of the file contents
      size_t file_size;

      /// the buffer that holds the file contents
      utility::ScopedArray<char> file_contents;
    };
  }
}

#endif // _MADARA_UTILITY_FILEFRAGMENTER_H_
