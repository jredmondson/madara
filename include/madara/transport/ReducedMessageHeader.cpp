
#include <algorithm>
#include <string.h>
#include <sstream>

#include "madara/exceptions/MemoryException.h"
#include "ReducedMessageHeader.h"
#include "madara/utility/Utility.h"

madara::transport::ReducedMessageHeader::ReducedMessageHeader()
  : MessageHeader()
{
  memcpy(madara_id, REDUCED_MADARA_ID, 7);
  madara_id[7] = 0;
}

madara::transport::ReducedMessageHeader::~ReducedMessageHeader() {}

uint32_t madara::transport::ReducedMessageHeader::encoded_size(void) const
{
  return sizeof(uint64_t) * 3  // size, clock, timestamp
         + sizeof(char) * (MADARA_IDENTIFIER_LENGTH + 1)  // KaRL1.0 + ttl
         + sizeof(uint32_t) * 1;                          // updates
}

const char* madara::transport::ReducedMessageHeader::read(
    const char* buffer, int64_t& buffer_remaining)
{
  // Remove size field from the buffer and update accordingly
  if ((size_t)buffer_remaining >= sizeof(size))
  {
    memcpy(&size, buffer, sizeof(size));
    size = madara::utility::endian_swap(size);
    buffer += sizeof(size);
  }
  else
  {
    std::stringstream buffer;
    buffer << "ReducedMessageHeader::read: ";
    buffer << sizeof(size) << " byte size encoding cannot";
    buffer << " fit in ";
    buffer << buffer_remaining << " byte buffer\n";

    throw exceptions::MemoryException(buffer.str());
  }
  buffer_remaining -= sizeof(size);

  // Remove madara_id field from the buffer and update accordingly
  if ((size_t)buffer_remaining >= sizeof(char) * MADARA_IDENTIFIER_LENGTH)
  {
    strncpy(madara_id, buffer, MADARA_IDENTIFIER_LENGTH);
    buffer += sizeof(char) * MADARA_IDENTIFIER_LENGTH;
  }
  else
  {
    std::stringstream buffer;
    buffer << "ReducedMessageHeader::read: ";
    buffer << MADARA_IDENTIFIER_LENGTH << " byte id encoding cannot";
    buffer << " fit in ";
    buffer << buffer_remaining << " byte buffer\n";

    throw exceptions::MemoryException(buffer.str());
  }
  buffer_remaining -= sizeof(char) * MADARA_IDENTIFIER_LENGTH;

  // Remove updates field from the buffer and update accordingly
  if ((size_t)buffer_remaining >= sizeof(updates))
  {
    memcpy(&updates, buffer, sizeof(updates));
    updates = madara::utility::endian_swap(updates);
    buffer += sizeof(updates);
  }
  else
  {
    std::stringstream buffer;
    buffer << "ReducedMessageHeader::read: ";
    buffer << sizeof(updates) << " byte updates encoding cannot";
    buffer << " fit in ";
    buffer << buffer_remaining << " byte buffer\n";

    throw exceptions::MemoryException(buffer.str());
  }
  buffer_remaining -= sizeof(updates);

  // Remove clock field from the buffer and update accordingly
  if ((size_t)buffer_remaining >= sizeof(clock))
  {
    memcpy(&clock, buffer, sizeof(clock));
    clock = madara::utility::endian_swap(clock);
    buffer += sizeof(clock);
  }
  else
  {
    std::stringstream buffer;
    buffer << "ReducedMessageHeader::read: ";
    buffer << sizeof(clock) << " byte clock encoding cannot";
    buffer << " fit in ";
    buffer << buffer_remaining << " byte buffer\n";

    throw exceptions::MemoryException(buffer.str());
  }
  buffer_remaining -= sizeof(clock);

  // Remove timestamp field from the buffer and update accordingly
  if ((size_t)buffer_remaining >= sizeof(timestamp))
  {
    memcpy(&timestamp, buffer, sizeof(timestamp));
    timestamp = madara::utility::endian_swap(timestamp);
    buffer += sizeof(timestamp);
  }
  else
  {
    std::stringstream buffer;
    buffer << "ReducedMessageHeader::read: ";
    buffer << sizeof(timestamp) << " byte timestamp encoding cannot";
    buffer << " fit in ";
    buffer << buffer_remaining << " byte buffer\n";

    throw exceptions::MemoryException(buffer.str());
  }
  buffer_remaining -= sizeof(timestamp);

  // Remove the time to live field from the buffer
  if (buffer_remaining >= 1)
  {
    memcpy(&ttl, buffer, 1);
    buffer += 1;
  }
  buffer_remaining -= 1;

  return buffer;
}

char* madara::transport::ReducedMessageHeader::write(
    char* buffer, int64_t& buffer_remaining)
{
  // Write size field from the buffer and update accordingly
  if ((size_t)buffer_remaining >= sizeof(size))
  {
    *(uint64_t*)buffer = madara::utility::endian_swap(size);
    buffer += sizeof(size);
  }
  else
  {
    std::stringstream buffer;
    buffer << "ReducedMessageHeader::write: ";
    buffer << sizeof(size) << " byte size encoding cannot";
    buffer << " fit in ";
    buffer << buffer_remaining << " byte buffer\n";

    throw exceptions::MemoryException(buffer.str());
  }
  buffer_remaining -= sizeof(size);

  // Write madara_id field from the buffer and update accordingly
  if ((size_t)buffer_remaining >= sizeof(char) * MADARA_IDENTIFIER_LENGTH)
  {
    strncpy(buffer, madara_id, MADARA_IDENTIFIER_LENGTH);
    buffer += sizeof(char) * MADARA_IDENTIFIER_LENGTH;
  }
  else
  {
    std::stringstream buffer;
    buffer << "ReducedMessageHeader::write: ";
    buffer << MADARA_IDENTIFIER_LENGTH << " byte id encoding cannot";
    buffer << " fit in ";
    buffer << buffer_remaining << " byte buffer\n";

    throw exceptions::MemoryException(buffer.str());
  }
  buffer_remaining -= sizeof(char) * MADARA_IDENTIFIER_LENGTH;

  // Write updates field from the buffer and update accordingly
  if ((size_t)buffer_remaining >= sizeof(updates))
  {
    *(uint32_t*)buffer = madara::utility::endian_swap(updates);
    buffer += sizeof(updates);
  }
  else
  {
    std::stringstream buffer;
    buffer << "ReducedMessageHeader::write: ";
    buffer << sizeof(updates) << " byte updates encoding cannot";
    buffer << " fit in ";
    buffer << buffer_remaining << " byte buffer\n";

    throw exceptions::MemoryException(buffer.str());
  }
  buffer_remaining -= sizeof(updates);

  // Write clock field from the buffer and update accordingly
  if ((size_t)buffer_remaining >= sizeof(clock))
  {
    *(uint64_t*)buffer = madara::utility::endian_swap(clock);
    buffer += sizeof(clock);
  }
  else
  {
    std::stringstream buffer;
    buffer << "ReducedMessageHeader::write: ";
    buffer << sizeof(clock) << " byte clock encoding cannot";
    buffer << " fit in ";
    buffer << buffer_remaining << " byte buffer\n";

    throw exceptions::MemoryException(buffer.str());
  }
  buffer_remaining -= sizeof(clock);

  // Write timestamp field from the buffer and update accordingly
  if ((size_t)buffer_remaining >= sizeof(timestamp))
  {
    *(uint64_t*)buffer = madara::utility::endian_swap(timestamp);
    buffer += sizeof(timestamp);
  }
  else
  {
    std::stringstream buffer;
    buffer << "ReducedMessageHeader::write: ";
    buffer << sizeof(timestamp) << " byte timestamp encoding cannot";
    buffer << " fit in ";
    buffer << buffer_remaining << " byte buffer\n";

    throw exceptions::MemoryException(buffer.str());
  }
  buffer_remaining -= sizeof(timestamp);

  // Write the time to live field
  if (buffer_remaining >= 1)
  {
    memcpy(buffer, &ttl, 1);
    buffer += 1;
  }
  buffer_remaining -= 1;

  return buffer;
}

std::string madara::transport::ReducedMessageHeader::to_string(void)
{
  std::stringstream buffer;
  buffer << "29: size (8:" << size << "), ";
  buffer << "encoding (8:" << madara_id << "), ";
  buffer << "numupdates (4:" << updates << "), ";
  buffer << "clock (8:" << clock << "), ";
  buffer << "ttl (1:" << ttl << "), ";

  return buffer.str();
}

bool madara::transport::ReducedMessageHeader::equals(const MessageHeader& other)
{
  return size == other.size && updates == other.updates &&
         clock == other.clock && timestamp == other.timestamp;
}
