#ifndef _MADARA_KNOWLEDGE_FILESTREAMER_H_
#define _MADARA_KNOWLEDGE_FILESTREAMER_H_

#include <string>
#include <fstream>
#include <vector>
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
 * @class FileStreamer
 * @brief Splits files into fragments that can be saved to and loaded from
 * a knowledge base
 */
class FileStreamer
{
public:
  /**
   * Constructor
   **/
  FileStreamer() {}

  /**
   * Constructor
   * @param key        the location in the knowledge base to save to
   * @param filename   the file to open and read from
   * @param kb         the knowledge base to stream to
   **/
  FileStreamer(
      const std::string& key, const std::string& filename, KnowledgeBase kb)
  {
    init(key, filename, kb);
  }

  /**
   * Initializes the file stream and all containers
   * @param key        the location in the knowledge base to save to
   * @param filename   the file to open and read from
   * @param kb         the knowledge base to stream to
   **/
  inline void init(
      const std::string& key, const std::string& filename, KnowledgeBase kb)
  {
    // setup containers
    file_space.set_name(key, kb);
    file_space["size"].to_container(file_size);
    file_space["crc"].to_container(file_crc);
    file_space["contents"].to_container(file_fragments);
    file_fragments.clear();

    file_crc.set_name(file_space["crc"].get_name(), kb);
    file_size.set_name(file_space["size"].get_name(), kb);

    // setup file information
    filename_ = filename;
    file_crc = (KnowledgeRecord::Integer)utility::file_crc(filename);
    file_size = (KnowledgeRecord::Integer)utility::file_size(filename);

    stream_.open(filename, std::ios::in | std::ios::binary);
  }

  /**
   * Loads a fragment into the knowledge base
   **/
  inline size_t load(size_t index, size_t frag_size = 60000)
  {
    size_t bytes_read = 0;

    if (stream_) {
      // seek to the index position in the file
      stream_.seekg((std::streampos)index * frag_size, std::ios::beg);

      // read the fragment into a local buffer
      std::vector<unsigned char> frag(frag_size);
      // char * frag = new char [frag_size];
      stream_.read((char*)frag.data(), frag_size);
      bytes_read = stream_.gcount();
      frag.resize(bytes_read);

      KnowledgeRecord record;
      record.emplace_file(std::move(frag));

      // set the file fragment into the KB
      file_fragments.set(std::to_string((unsigned long long)index), record);
      // file_fragments.emplace_file ()
    }

    return bytes_read;
  }

  /**
   * Returns the crc of the file
   * @return the crc of the file
   **/
  inline uint32_t get_crc(void)
  {
    return (uint32_t)file_crc.to_integer();
  }

  /**
   * Returns the size of the file
   * @return the size of the file
   **/
  inline size_t get_size(void)
  {
    return (size_t)file_size.to_integer();
  }

  /**
   * Returns the name of the file
   * @return the name of the file being streamed
   **/
  inline std::string get_filename(void)
  {
    return filename_;
  }

  /**
   * Clears the fragment list
   **/
  inline void clear_fragments(void)
  {
    file_fragments.clear();
  }

  /**
   * Remodifies the file size and crc
   **/
  inline void modify(void)
  {
    file_size.modify();
    file_crc.modify();
  }

  /// records that contain the file fragments
  containers::FlexMap file_space;

  /// the size of the file contents
  containers::Integer file_size;

  /// the crc of the file
  containers::Integer file_crc;

  /// the crc of the file
  containers::Map file_fragments;

private:
  /// the name of the file
  std::string filename_;

  /// the stream to read from
  std::ifstream stream_;
};
}
}

#endif  // _MADARA_KNOWLEDGE_FILESTREAMER_H_
