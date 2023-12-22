#ifndef _MADARA_FILTERS_BUFFERFILTERHEADER_INL_
#define _MADARA_FILTERS_BUFFERFILTERHEADER_INL_

/**
 * @file BufferFilterHeader.inl
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains the implementation of inline features of
 * the BufferFilterHeader class
 **/

#include <string.h>
#include <sstream>

#include "madara/utility/StdInt.h"
#include "BufferFilterHeader.h"
#include "madara/exceptions/MemoryException.h"

namespace madara
{
namespace filters
{
inline bool BufferFilterHeader::check_filter(filters::BufferFilter* filter)
{
  if (filter != 0)
  {
    madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_MINOR,
        "CheckpointSettings::check_filter: header: "
        " %s:%s, filter: %s:%s\n",
        id, utility::to_string_version(version).c_str(),
        filter->get_id().c_str(),
        utility::to_string_version(filter->get_version()).c_str());

    return filter->get_id() == id && version >= filter->get_version();
  }
  else
    return false;
}

inline void BufferFilterHeader::read(filters::BufferFilter* filter)
{
  if (filter != 0)
  {
    // the only thing not filled in is size, which comes from encode/decode
    version = filter->get_version();
    std::string temp = filter->get_id();
    memcpy(id, temp.c_str(), std::min(temp.size() + 1, (size_t)8));
  }
  else
  {
    std::stringstream buffer;
    buffer << "BufferFilterHeader::read: ";
    buffer << " filter was null\n";

    throw exceptions::MemoryException(buffer.str());
  }
}

inline const char* BufferFilterHeader::read(
    const char* buffer, int64_t& buffer_remaining)
{
  if (buffer_remaining >= 20)
  {
    memcpy(&size, buffer, 8);
    strncpy(id, buffer + 8, 7);
    memcpy(&version, buffer + 16, 4);

    size = utility::endian_swap(size);
    version = utility::endian_swap(version);

    buffer_remaining -= 20;
    buffer += 20;
  }
  else
  {
    std::stringstream local_buffer;
    local_buffer << "BufferFilterHeader::read: ";
    local_buffer << " 20 byte size encoding cannot fit in ";
    local_buffer << buffer_remaining << " byte buffer\n";

    throw exceptions::MemoryException(local_buffer.str());
  }

  return buffer;
}

inline char* BufferFilterHeader::write(char* buffer, int64_t& buffer_remaining)
{
  if (buffer_remaining >= 20)
  {
    uint64_t new_size = utility::endian_swap(size);
    uint32_t new_version = utility::endian_swap(version);

    memcpy(buffer, &new_size, 8);
    memcpy(buffer + 8, id, 8);
    memcpy(buffer + 16, &new_version, 4);

    buffer_remaining -= 20;
    buffer += 20;
  }
  else
  {
    std::stringstream local_buffer;
    local_buffer << "BufferFilterHeader::write: ";
    local_buffer << " 20 byte size encoding cannot fit in ";
    local_buffer << buffer_remaining << " byte buffer\n";

    throw exceptions::MemoryException(local_buffer.str());
  }

  return buffer;
}

}  // end filters namespace
}  // end madara namespace

#endif  // _MADARA_FILTERS_BUFFERFILTERHEADER_INL_
