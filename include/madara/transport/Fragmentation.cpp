#include "Fragmentation.h"
#include "ReducedMessageHeader.h"
#include "madara/exceptions/MemoryException.h"
#include "madara/utility/Utility.h"
#include "madara/logger/GlobalLogger.h"

#include <algorithm>
#include <time.h>
#include <sstream>

madara::transport::FragmentMessageHeader::FragmentMessageHeader()
  : MessageHeader(), update_number(0), total_size(0)
{
  memcpy(madara_id, FRAGMENTATION_MADARA_ID, 7);
  madara_id[7] = 0;

  originator[0] = 0;
  domain[0] = 0;
}

madara::transport::FragmentMessageHeader::~FragmentMessageHeader() {}

uint32_t madara::transport::FragmentMessageHeader::encoded_size(void) const
{
  return sizeof(uint64_t) * 4  // size, clock, timestamp, total_size (4)=32
         + sizeof(char) *(MADARA_IDENTIFIER_LENGTH + MADARA_DOMAIN_MAX_LENGTH +
                              MAX_ORIGINATOR_LENGTH + 1) + // 8+32+64+1=105
         sizeof(uint32_t) * 4;  // type, updates, quality, update_number(4)=16
}

uint32_t madara::transport::FragmentMessageHeader::static_encoded_size(void)
{
  return sizeof(uint64_t) * 4  // size, clock, timestamp, total_size=32
         + sizeof(char) *(MADARA_IDENTIFIER_LENGTH + MADARA_DOMAIN_MAX_LENGTH +
                              MAX_ORIGINATOR_LENGTH + 1) + // 8+32+64+1=105
         sizeof(uint32_t) * 4;  // type, updates, quality, update_number=16
}

uint32_t madara::transport::FragmentMessageHeader::get_updates(
    const char* buffer)
{
  buffer += 116;
  return(madara::utility::endian_swap(*(uint32_t*)buffer));
}

std::string madara::transport::FragmentMessageHeader::to_string(void)
{
  std::stringstream buffer;
  buffer << "frag 12: ";
  buffer << "index(4:" << update_number << "), ";
  buffer << "total_size(8:" << total_size << "), msg: ";
  buffer << MessageHeader::to_string();

  return buffer.str();
}

const char* madara::transport::FragmentMessageHeader::read(
    const char* buffer, int64_t& buffer_remaining)
{
  // Remove size field from the buffer and update accordingly
  if((size_t)buffer_remaining >= sizeof(size))
  {
    memcpy(&size, buffer, sizeof(size));
    size = madara::utility::endian_swap(size);
    buffer += sizeof(size);
  }
  else
  {
    std::stringstream buffer;
    buffer << "FragmentMessageHeader::read: ";
    buffer << sizeof(size) << " byte size field cannot fit in ";
    buffer << buffer_remaining << " byte buffer\n";

    throw exceptions::MemoryException(buffer.str());
  }
  buffer_remaining -= sizeof(size);

  // Remove madara_id field from the buffer and update accordingly
  if((size_t)buffer_remaining >= sizeof(char) * MADARA_IDENTIFIER_LENGTH)
  {
    strncpy(madara_id, buffer, MADARA_IDENTIFIER_LENGTH - 1);
    buffer += sizeof(char) * MADARA_IDENTIFIER_LENGTH;
  }
  else
  {
    std::stringstream buffer;
    buffer << "FragmentMessageHeader::read: ";
    buffer << MADARA_IDENTIFIER_LENGTH << " byte id encoding cannot fit in ";
    buffer << buffer_remaining << " byte buffer\n";

    throw exceptions::MemoryException(buffer.str());
  }
  buffer_remaining -= sizeof(char) * MADARA_IDENTIFIER_LENGTH;

  // Remove domain field from the buffer and update accordingly
  if((size_t)buffer_remaining >= sizeof(char) * MADARA_DOMAIN_MAX_LENGTH)
  {
    strncpy(domain, buffer, MADARA_DOMAIN_MAX_LENGTH - 1);
    buffer += sizeof(char) * MADARA_DOMAIN_MAX_LENGTH;
  }
  else
  {
    std::stringstream buffer;
    buffer << "FragmentMessageHeader::read: ";
    buffer << MADARA_DOMAIN_MAX_LENGTH << " byte domain encoding cannot";
    buffer << " fit in ";
    buffer << buffer_remaining << " byte buffer\n";

    throw exceptions::MemoryException(buffer.str());
  }
  buffer_remaining -= sizeof(char) * MADARA_DOMAIN_MAX_LENGTH;

  // Remove originator from the buffer and update accordingly
  if((size_t)buffer_remaining >= sizeof(char) * MAX_ORIGINATOR_LENGTH)
  {
    strncpy(originator, buffer, MAX_ORIGINATOR_LENGTH - 1);
    buffer += sizeof(char) * MAX_ORIGINATOR_LENGTH;
  }
  else
  {
    std::stringstream buffer;
    buffer << "FragmentMessageHeader::read: ";
    buffer << MAX_ORIGINATOR_LENGTH << " byte originator encoding cannot";
    buffer << " fit in ";
    buffer << buffer_remaining << " byte buffer\n";

    throw exceptions::MemoryException(buffer.str());
  }
  buffer_remaining -= sizeof(char) * MAX_ORIGINATOR_LENGTH;

  // Remove type field from the buffer and update accordingly
  if((size_t)buffer_remaining >= sizeof(type))
  {
    memcpy(&type, buffer, sizeof(type));
    type = madara::utility::endian_swap(type);
    buffer += sizeof(type);
  }
  else
  {
    std::stringstream buffer;
    buffer << "FragmentMessageHeader::read: ";
    buffer << sizeof(type) << " byte type encoding cannot";
    buffer << " fit in ";
    buffer << buffer_remaining << " byte buffer\n";

    throw exceptions::MemoryException(buffer.str());
  }
  buffer_remaining -= sizeof(type);

  // Remove updates field from the buffer and update accordingly
  if((size_t)buffer_remaining >= sizeof(updates))
  {
    memcpy(&updates, buffer, sizeof(updates));
    updates = madara::utility::endian_swap(updates);
    buffer += sizeof(updates);
  }
  else
  {
    std::stringstream buffer;
    buffer << "FragmentMessageHeader::read: ";
    buffer << sizeof(updates) << " byte updates encoding cannot";
    buffer << " fit in ";
    buffer << buffer_remaining << " byte buffer\n";

    throw exceptions::MemoryException(buffer.str());
  }
  buffer_remaining -= sizeof(updates);

  // Remove quality field from the buffer and update accordingly
  if((size_t)buffer_remaining >= sizeof(quality))
  {
    memcpy(&quality, buffer, sizeof(quality));
    quality = madara::utility::endian_swap(quality);
    buffer += sizeof(quality);
  }
  else
  {
    std::stringstream buffer;
    buffer << "FragmentMessageHeader::read: ";
    buffer << sizeof(quality) << " byte quality encoding cannot";
    buffer << " fit in ";
    buffer << buffer_remaining << " byte buffer\n";

    throw exceptions::MemoryException(buffer.str());
  }
  buffer_remaining -= sizeof(quality);

  // Remove clock field from the buffer and update accordingly
  if((size_t)buffer_remaining >= sizeof(clock))
  {
    memcpy(&clock, buffer, sizeof(clock));
    clock = madara::utility::endian_swap(clock);
    buffer += sizeof(clock);
  }
  else
  {
    std::stringstream buffer;
    buffer << "FragmentMessageHeader::read: ";
    buffer << sizeof(clock) << " byte clock encoding cannot";
    buffer << " fit in ";
    buffer << buffer_remaining << " byte buffer\n";

    throw exceptions::MemoryException(buffer.str());
  }
  buffer_remaining -= sizeof(clock);

  // Remove timestamp field from the buffer and update accordingly
  if((size_t)buffer_remaining >= sizeof(timestamp))
  {
    memcpy(&timestamp, buffer, sizeof(timestamp));
    timestamp = madara::utility::endian_swap(timestamp);
    buffer += sizeof(timestamp);
  }
  else
  {
    std::stringstream buffer;
    buffer << "FragmentMessageHeader::read: ";
    buffer << sizeof(timestamp) << " byte timestamp encoding cannot";
    buffer << " fit in ";
    buffer << buffer_remaining << " byte buffer\n";

    throw exceptions::MemoryException(buffer.str());
  }
  buffer_remaining -= sizeof(timestamp);

  // Remove the time to live field from the buffer
  if(buffer_remaining >= 1)
  {
    memcpy(&ttl, buffer, 1);
    buffer += 1;
  }
  else
  {
    std::stringstream buffer;
    buffer << "FragmentMessageHeader::read: ";
    buffer << "1 byte ttl encoding cannot";
    buffer << " fit in ";
    buffer << buffer_remaining << " byte buffer\n";

    throw exceptions::MemoryException(buffer.str());
  }
  buffer_remaining -= 1;

  // Remove updates field from the buffer and update accordingly
  if((size_t)buffer_remaining >= sizeof(update_number))
  {
    memcpy(&update_number, buffer, sizeof(update_number));
    update_number = madara::utility::endian_swap(update_number);
    buffer += sizeof(update_number);
  }
  else
  {
    std::stringstream buffer;
    buffer << "FragmentMessageHeader::read: ";
    buffer << sizeof(update_number) << " byte update number encoding cannot";
    buffer << " fit in ";
    buffer << buffer_remaining << " byte buffer\n";

    throw exceptions::MemoryException(buffer.str());
  }
  buffer_remaining -= sizeof(update_number);

  // Remove total size field from the buffer and update accordingly
  if((size_t)buffer_remaining >= sizeof(total_size))
  {
    memcpy(&total_size, buffer, sizeof(total_size));
    total_size = madara::utility::endian_swap(total_size);
    buffer += sizeof(total_size);
  }
  else
  {
    std::stringstream buffer;
    buffer << "FragmentMessageHeader::read: ";
    buffer << sizeof(total_size) << " byte total size encoding cannot";
    buffer << " fit in ";
    buffer << buffer_remaining << " byte buffer\n";

    throw exceptions::MemoryException(buffer.str());
  }
  buffer_remaining -= sizeof(total_size);

  return buffer;
}

char* madara::transport::FragmentMessageHeader::write(
    char* buffer, int64_t& buffer_remaining)
{
  // Write size field from the buffer and update accordingly
  if((size_t)buffer_remaining >= sizeof(size))
  {
    *(uint64_t*)buffer = madara::utility::endian_swap(size);
    buffer += sizeof(size);
  }
  else
  {
    std::stringstream buffer;
    buffer << "FragmentMessageHeader::write: ";
    buffer << sizeof(size) << " byte size encoding cannot";
    buffer << " fit in ";
    buffer << buffer_remaining << " byte buffer\n";

    throw exceptions::MemoryException(buffer.str());
  }
  buffer_remaining -= sizeof(size);

  // Write madara_id field from the buffer and update accordingly
  if((size_t)buffer_remaining >= sizeof(char) * MADARA_IDENTIFIER_LENGTH)
  {
    strncpy(buffer, madara_id, MADARA_IDENTIFIER_LENGTH - 1);
    buffer += sizeof(char) * MADARA_IDENTIFIER_LENGTH;
  }
  else
  {
    std::stringstream buffer;
    buffer << "FragmentMessageHeader::write: ";
    buffer << MADARA_IDENTIFIER_LENGTH << " byte id encoding cannot";
    buffer << " fit in ";
    buffer << buffer_remaining << " byte buffer\n";

    throw exceptions::MemoryException(buffer.str());
  }
  buffer_remaining -= sizeof(char) * MADARA_IDENTIFIER_LENGTH;

  // Write domain field from the buffer and update accordingly
  if((size_t)buffer_remaining >= sizeof(char) * MADARA_DOMAIN_MAX_LENGTH)
  {
    strncpy(buffer, domain, MADARA_DOMAIN_MAX_LENGTH - 1);
    buffer += sizeof(char) * MADARA_DOMAIN_MAX_LENGTH;
  }
  else
  {
    std::stringstream buffer;
    buffer << "FragmentMessageHeader::write: ";
    buffer << MADARA_DOMAIN_MAX_LENGTH << " byte domain encoding cannot";
    buffer << " fit in ";
    buffer << buffer_remaining << " byte buffer\n";

    throw exceptions::MemoryException(buffer.str());
  }
  buffer_remaining -= sizeof(char) * MADARA_DOMAIN_MAX_LENGTH;

  // Write originator from the buffer and update accordingly
  if((size_t)buffer_remaining >= sizeof(char) * MAX_ORIGINATOR_LENGTH)
  {
    strncpy(buffer, originator, MAX_ORIGINATOR_LENGTH - 1);
    buffer += sizeof(char) * MAX_ORIGINATOR_LENGTH;
  }
  else
  {
    std::stringstream buffer;
    buffer << "FragmentMessageHeader::write: ";
    buffer << MAX_ORIGINATOR_LENGTH << " byte originator encoding cannot";
    buffer << " fit in ";
    buffer << buffer_remaining << " byte buffer\n";

    throw exceptions::MemoryException(buffer.str());
  }
  buffer_remaining -= sizeof(char) * MAX_ORIGINATOR_LENGTH;

  // Write type field from the buffer and update accordingly
  if((size_t)buffer_remaining >= sizeof(type))
  {
    *(uint32_t*)buffer = madara::utility::endian_swap(type);
    buffer += sizeof(type);
  }
  else
  {
    std::stringstream buffer;
    buffer << "FragmentMessageHeader::write: ";
    buffer << sizeof(type) << " byte type encoding cannot";
    buffer << " fit in ";
    buffer << buffer_remaining << " byte buffer\n";

    throw exceptions::MemoryException(buffer.str());
  }
  buffer_remaining -= sizeof(type);

  // Write updates field from the buffer and update accordingly
  if((size_t)buffer_remaining >= sizeof(updates))
  {
    *(uint32_t*)buffer = madara::utility::endian_swap(updates);
    buffer += sizeof(updates);
  }
  else
  {
    std::stringstream buffer;
    buffer << "FragmentMessageHeader::write: ";
    buffer << sizeof(updates) << " byte updates encoding cannot";
    buffer << " fit in ";
    buffer << buffer_remaining << " byte buffer\n";

    throw exceptions::MemoryException(buffer.str());
  }
  buffer_remaining -= sizeof(updates);

  // Write quality field from the buffer and update accordingly
  if((size_t)buffer_remaining >= sizeof(quality))
  {
    *(uint32_t*)buffer = madara::utility::endian_swap(quality);
    buffer += sizeof(quality);
  }
  else
  {
    std::stringstream buffer;
    buffer << "FragmentMessageHeader::write: ";
    buffer << sizeof(quality) << " byte quality encoding cannot";
    buffer << " fit in ";
    buffer << buffer_remaining << " byte buffer\n";

    throw exceptions::MemoryException(buffer.str());
  }
  buffer_remaining -= sizeof(quality);

  // Write clock field from the buffer and update accordingly
  if((size_t)buffer_remaining >= sizeof(clock))
  {
    *(uint64_t*)buffer = madara::utility::endian_swap(clock);
    buffer += sizeof(clock);
  }
  else
  {
    std::stringstream buffer;
    buffer << "FragmentMessageHeader::write: ";
    buffer << sizeof(clock) << " byte clock encoding cannot";
    buffer << " fit in ";
    buffer << buffer_remaining << " byte buffer\n";

    throw exceptions::MemoryException(buffer.str());
  }
  buffer_remaining -= sizeof(clock);

  // Write timestamp field from the buffer and update accordingly
  if((size_t)buffer_remaining >= sizeof(timestamp))
  {
    *(uint64_t*)buffer = madara::utility::endian_swap(timestamp);
    buffer += sizeof(timestamp);
  }
  else
  {
    std::stringstream buffer;
    buffer << "FragmentMessageHeader::write: ";
    buffer << sizeof(timestamp) << " byte timestamp encoding cannot";
    buffer << " fit in ";
    buffer << buffer_remaining << " byte buffer\n";

    throw exceptions::MemoryException(buffer.str());
  }
  buffer_remaining -= sizeof(timestamp);

  if(buffer_remaining >= 1)
  {
    memcpy(buffer, &ttl, 1);
    buffer += 1;
  }
  else
  {
    std::stringstream buffer;
    buffer << "FragmentMessageHeader::write: ";
    buffer << "1 byte ttl encoding cannot";
    buffer << " fit in ";
    buffer << buffer_remaining << " byte buffer\n";

    throw exceptions::MemoryException(buffer.str());
  }
  buffer_remaining -= 1;

  // Write updates field from the buffer and update accordingly
  if((size_t)buffer_remaining >= sizeof(update_number))
  {
    *(uint32_t*)buffer = madara::utility::endian_swap(update_number);
    buffer += sizeof(update_number);
  }
  else
  {
    std::stringstream buffer;
    buffer << "FragmentMessageHeader::write: ";
    buffer << sizeof(update_number) << " byte update number encoding cannot";
    buffer << " fit in ";
    buffer << buffer_remaining << " byte buffer\n";

    throw exceptions::MemoryException(buffer.str());
  }
  buffer_remaining -= sizeof(update_number);

  // Write updates field from the buffer and update accordingly
  if((size_t)buffer_remaining >= sizeof(total_size))
  {
    *(uint64_t*)buffer = madara::utility::endian_swap(total_size);
    buffer += sizeof(total_size);
  }
  else
  {
    std::stringstream buffer;
    buffer << "FragmentMessageHeader::write: ";
    buffer << sizeof(total_size) << " byte total size encoding cannot";
    buffer << " fit in ";
    buffer << buffer_remaining << " byte buffer\n";

    throw exceptions::MemoryException(buffer.str());
  }
  buffer_remaining -= sizeof(total_size);

  return buffer;
}

bool madara::transport::FragmentMessageHeader::equals(
    const MessageHeader& other)
{
  const FragmentMessageHeader* rhs =
      dynamic_cast<const FragmentMessageHeader*>(&other);

  return size == rhs->size && type == rhs->type && updates == rhs->updates &&
         update_number == rhs->update_number &&
         total_size == rhs->total_size &&
         quality == rhs->quality &&
         clock == rhs->clock && timestamp == rhs->timestamp &&
         strncmp(madara_id, rhs->madara_id, MADARA_IDENTIFIER_LENGTH) == 0 &&
         strncmp(domain, rhs->domain, MADARA_DOMAIN_MAX_LENGTH) == 0 &&
         strncmp(originator, rhs->originator, MAX_ORIGINATOR_LENGTH) == 0;
}

char* madara::transport::defrag(FragmentMap& map, uint64_t & total_size)
{
  char* result = 0;

  madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_MAJOR,
      "transport::defrag:"
      " defragging fragment map\n");

  FragmentMap::iterator i = map.find(0);
  if(i != map.end())
  {
    FragmentMessageHeader header;
    int64_t buffer_remaining(header.encoded_size());
    const char* buffer = header.read(i->second.get(), buffer_remaining);

    madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_DETAILED,
        "transport::defrag:"
        " inspecting frag: %s\n", header.to_string().c_str());

    // do we have enough updates to defragment?
    if(header.updates <= map.size())
    {
      madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_DETAILED,
          "transport::defrag:"
          " the map is large enough to contain updates\n");

      int64_t size = 0;
      // if(MessageHeader::message_header_test(buffer))
      // {
      //   madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_DETAILED,
      //       "transport::defrag:"
      //       " regular message header detected\n");

      //   MessageHeader contents_header;
      //   buffer_remaining = contents_header.encoded_size();
      //   contents_header.read(buffer, buffer_remaining);
      //   size = contents_header.size;
      // }
      // else if(ReducedMessageHeader::message_header_test(buffer))
      // {
      //   madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_DETAILED,
      //       "transport::defrag:"
      //       " reduced message header detected\n");

      //   ReducedMessageHeader contents_header;
      //   buffer_remaining = contents_header.encoded_size();
      //   contents_header.read(buffer, buffer_remaining);
      //   size = contents_header.size;
      // }
      size = header.total_size;

      result = new char[size];
      char* lhs = result;

      uint32_t actual_size = (uint32_t)header.size - header.encoded_size();
      buffer_remaining = actual_size;

      if(size >= 0)
      {
        madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_DETAILED,
            "transport::defrag: copying buffer to lhs\n");

        memcpy(lhs, buffer, buffer_remaining);
        buffer += actual_size;
        lhs += actual_size;
        size -= actual_size;
      }

      if(i != map.end())
        ++i;

      // if so, iterate over the fragments and copy the contents
      for(; i != map.end(); ++i)
      {
        madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_DETAILED,
            "transport::defrag: reading header of new fragment\n");

        buffer_remaining = header.encoded_size();
        buffer = header.read(i->second.get(), buffer_remaining);
        actual_size =(uint32_t)header.size - header.encoded_size();
        buffer_remaining = actual_size;

        madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_DETAILED,
            "transport::defrag:"
            " piecing together frag: %s\n", header.to_string().c_str());

        if(size >= 0)
        {
          madara_logger_ptr_log(logger::global_logger.get(),
              logger::LOG_DETAILED,
              "transport::defrag: copying buffer to lhs\n");

          memcpy(lhs, buffer, buffer_remaining);
          buffer += actual_size;
          lhs += actual_size;
          size -= actual_size;
        }
      }
    }

    total_size = header.total_size;
  }

  return result;
}

void madara::transport::delete_fragments(FragmentMap& map)
{
  for(FragmentMap::iterator i = map.begin(); i != map.end(); ++i)
  {
    i->second = (const char*)0;
  }
  map.clear();
}

char* madara::transport::add_fragment(const char* originator, uint64_t clock,
    uint32_t update_number, const char* fragment, uint32_t queue_length,
    OriginatorFragmentMap& map, uint64_t & total_size, bool clear)
{
  char* result = 0;
  total_size = 0;

  /**
   * add a fragment to the map in the following conditions
   * 1) originator does not exist yet in the map
   * 2) clock is newer than existing fragments
   * 3) fragment update does not exist yet in an existing clock entry
   *
   * clear a fragment map and return a buffer in the following situations
   * 4) if we have all fragments
   **/

  madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_MAJOR,
      "transport::add_fragment: adding fragment\n");

  utility::ScopedArray<const char> new_fragment = 0;
  const char* cur_buffer = fragment;
  FragmentMessageHeader header;
  int64_t buffer_remaining = header.encoded_size();

  madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_DETAILED,
      "transport::add_fragment:"
      " reading header from buffer\n");

  header.read(cur_buffer, buffer_remaining);

  madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_DETAILED,
      "transport::add_fragment:"
      " inspecting frag: %s\n", header.to_string().c_str());

  if(header.size > 0)
  {
    madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_DETAILED,
        "transport::add_fragment:"
        " reading fragment of size %" PRIu64 " into new buffer\n",
        header.size);

    char * new_buffer = new char[header.size];
    memcpy(new_buffer, fragment, header.size);
    new_fragment = new_buffer;
  }
  else
  {
    return 0;
  }

  madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_DETAILED,
      "transport::add_fragment:"
      " searching for originator %s.\n",
      originator);

  OriginatorFragmentMap::iterator orig_map = map.find(originator);
  if(orig_map == map.end())
  {
    madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_DETAILED,
        "transport::add_fragment:"
        " creating entry for originator %s.\n",
        originator);

    // originator does not exist(1)
    map[originator][clock][update_number] = new_fragment;
  }
  else
  {
    madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_DETAILED,
        "transport::add_fragment:"
        " originator %s exists in fragment map.\n",
        originator);

    ClockFragmentMap& clock_map(orig_map->second);
    ClockFragmentMap::iterator clock_found = clock_map.find(clock);

    if(clock_found != clock_map.end())
    {
      // we have found the clock entry
      if(clock_found->second.find(update_number) == clock_found->second.end())
      {
        if(clock_found->second.size() != 0)
        {
          madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_MINOR,
              "transport::add_fragment:"
              " %s:%d is being added.\n",
              originator, update_number);

          // the fragment does not exist yet
          clock_found->second[update_number] = new_fragment;

          // check for a new buffer
          result = defrag(clock_found->second, total_size);

          if(result && clear)
          {
            madara_logger_ptr_log(logger::global_logger.get(),
                logger::LOG_MINOR,
                "transport::add_fragment:"
                " %s:%d is complete. Deleting fragments.\n",
                originator, update_number);

            delete_fragments(clock_found->second);
            clock_map.erase(clock);
          }
          else if(result)
          {
            madara_logger_ptr_log(logger::global_logger.get(),
                logger::LOG_MINOR,
                "transport::add_fragment:"
                " %s:%d is complete. Not deleting fragments.\n",
                originator, update_number);
          }
          else
          {
            madara_logger_ptr_log(logger::global_logger.get(),
                logger::LOG_MINOR,
                "transport::add_fragment:"
                " %s:%d is incomplete. %d size.\n",
                originator, update_number, (int)clock_found->second.size());
          }
        }
        else
        {
          // if we get here, the message has already been defragged
          madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_MINOR,
              "transport::add_fragment:"
              " %s:%d has been previously defragged and fragments deleted.\n",
              originator, update_number);
        }
      }
    }

    else if(clock_map.size() < queue_length)
    {
      // if we get here, the message has already been defragged
      madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_MINOR,
          "transport::add_fragment:"
          " %s:%d is being added to queue.\n",
          originator, update_number);

      // clock queue has not been exhausted(2)
      clock_map[clock][update_number] = new_fragment;
    }
    else
    {
      madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_MINOR,
          "transport::add_fragment:"
          " %s:%d is being added to queue after a deletion\n",
          originator, update_number);

      uint32_t oldest =(uint32_t)clock_map.begin()->first;

      if(oldest < clock)
      {
        madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_MINOR,
            "transport::add_fragment:"
            " deleting fragments.\n",
            originator, update_number);

        FragmentMap& fragments = clock_map[oldest];

        // delete all fragments in the clock entry
        for(FragmentMap::iterator i = fragments.begin(); i != fragments.end();
             ++i)
        {
          i->second = (const char*)0;
        }

        madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_DETAILED,
            "transport::add_fragment:"
            " erasing old clock.\n",
            originator, update_number);

        // erase the oldest clock fragment map
        clock_map.erase(oldest);

        // replace it in the logical queue with the new clock fragment
        clock_map[clock][update_number] = new_fragment;
      }
    }
  }

  return result;
}

void madara::transport::FragmentMessageHeader::operator=(MessageHeader& header)
{
  if(this != &header)
  {
    clock = header.clock;
    strncpy(domain, header.domain, MADARA_DOMAIN_MAX_LENGTH - 1);
    strncpy(originator, header.originator, MAX_ORIGINATOR_LENGTH - 1);
    quality = header.quality;
    size = header.size;
    timestamp = header.timestamp;
    ttl = header.ttl;
    type = header.type;
    updates = header.updates;
  }
}

void madara::transport::frag(
    const char* source, uint64_t total_size,
    const char* originator, const char* domain,
    uint64_t clock,
    uint64_t timestamp,
    uint32_t quality, unsigned char ttl,
    uint64_t fragment_size, FragmentMap& map)
{
  if(fragment_size > 0)
  {
    madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_MAJOR,
        "transport::frag:"
        " fragmenting character stream into %d byte packets.\n",
        fragment_size);

    uint64_t data_per_packet = fragment_size;
//        fragment_size - FragmentMessageHeader::static_encoded_size();

    const char* buffer = source;
    FragmentMessageHeader header;
    header.size = total_size;
    header.total_size = total_size;
    strncpy(header.originator, originator, MAX_ORIGINATOR_LENGTH - 1);
    strncpy(header.domain, domain, MADARA_DOMAIN_MAX_LENGTH - 1);
    header.clock = clock;
    header.timestamp = timestamp;
    header.quality = quality;
    header.ttl = ttl;

    // if(MessageHeader::message_header_test(source))
    // {
    //   madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_DETAILED,
    //       "transport::frag:"
    //       " regular message header detected\n",
    //       fragment_size);

    //   MessageHeader contents_header;
    //   int64_t buffer_remaining = contents_header.encoded_size();
    //   contents_header.read(source, buffer_remaining);
    //   header = contents_header;
    // }
    // else if(ReducedMessageHeader::message_header_test(source))
    // {
    //   madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_DETAILED,
    //       "transport::frag:"
    //       " reduced message header detected\n");

    //   ReducedMessageHeader contents_header;
    //   int64_t buffer_remaining = contents_header.encoded_size();
    //   contents_header.read(source, buffer_remaining);
    //   header = contents_header;
    // }

//    total_size = header.size;
    header.updates =(uint32_t)(total_size / data_per_packet);
    uint64_t last_size = total_size % data_per_packet;

    if(last_size != 0)
    {
      ++header.updates;
    }

    madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_DETAILED,
        "transport::frag:"
        " inspecting frag: %s\n", header.to_string().c_str());

    madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_DETAILED,
        "transport::frag:"
        " iterating over %d updates. last_size=%d\n",
        (int)header.updates, (int)last_size);

    for(uint32_t i = 0; i < header.updates; ++i)
    {
      char* new_frag;
      size_t cur_size;
      int64_t buffer_remaining;
      uint64_t actual_data_size;

      if(i == header.updates - 1 && last_size != 0)
      {
        cur_size = (size_t)last_size + FragmentMessageHeader::static_encoded_size();
      }
      else
      {
        cur_size = (size_t)fragment_size + FragmentMessageHeader::static_encoded_size();
      }

      buffer_remaining = cur_size;
      actual_data_size =
          cur_size - FragmentMessageHeader::static_encoded_size();
      new_frag = new char[cur_size];

      map[i] = new_frag;

      madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_DETAILED,
          "transport::frag:"
          " writing %d packet of size %d (non-header:%d).\n",
          (int)i, (int)cur_size, (int)actual_data_size);

      header.update_number = i;
      header.size = cur_size;

      madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_DETAILED,
          "transport::frag:"
          " inspecting frag: %s\n", header.to_string().c_str());


      new_frag = header.write(new_frag, buffer_remaining);
      memcpy(new_frag, buffer, (size_t)actual_data_size);
      buffer += actual_data_size;
      total_size -= actual_data_size;
    }
  }
}

bool madara::transport::is_complete(
    const char* originator, uint64_t clock, OriginatorFragmentMap& map)
{
  bool result = false;

  OriginatorFragmentMap::iterator orig_map = map.find(originator);

  if(orig_map != map.end())
  {
    madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_MINOR,
        "transport::is_complete:"
        " %s was found.\n",
        originator);

    ClockFragmentMap& clock_map(orig_map->second);
    ClockFragmentMap::iterator clock_found = clock_map.find(clock);

    if(clock_found != clock_map.end())
    {
      madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_MINOR,
          "transport::is_complete:"
          " %s:%" PRIu64 " was found.\n",
          originator, clock);

      uint64_t size = clock_found->second.size();
      FragmentMap::iterator i = clock_found->second.find(0);

      if(i != clock_found->second.end())
      {
        madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_MINOR,
            "transport::is_complete:"
            " %s:%" PRIu64 ": 0 == fragmap.end.\n",
            originator, clock);

        if(FragmentMessageHeader::get_updates(i->second.get()) == size)
        {
          madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_MAJOR,
              "transport::is_complete:"
              " %s:%" PRIu64 ": size == %" PRIu64 ", updates == %" PRIu32
              ". COMPLETE\n",
              originator, clock, size,
              FragmentMessageHeader::get_updates(i->second.get()));

          result = true;
        }
        else
        {
          madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_MAJOR,
              "transport::is_complete:"
              " %s:%" PRIu64 ": size == %" PRIu64 ", updates == %" PRIu32
              ". INCOMPLETE\n",
              originator, clock, size,
              FragmentMessageHeader::get_updates(i->second.get()));
        }
      }
      else if(size == 0)
      {
        madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_MAJOR,
            "transport::is_complete:"
            " %s:%" PRIu64 ": size == 0  and i == 0. COMPLETE\n",
            originator, clock);

        result = true;
      }
    }
    else
    {
      madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_MAJOR,
          "transport::is_complete:"
          " %s:%" PRIu64 " was not found.\n",
          originator, clock);
    }
  }
  else
  {
    madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_MAJOR,
        "transport::is_complete:"
        " %s was not found.\n",
        originator);
  }

  return result;
}

bool madara::transport::exists(const char* originator, uint64_t clock,
    uint32_t update_number, OriginatorFragmentMap& map)
{
  bool result = false;

  OriginatorFragmentMap::iterator orig_map = map.find(originator);

  if(orig_map != map.end())
  {
    ClockFragmentMap& clock_map(orig_map->second);
    ClockFragmentMap::iterator clock_found = clock_map.find(clock);

    if(clock_found != clock_map.end())
    {
      if(clock_found->second.find(update_number) != clock_found->second.end())
      {
        result = true;
      }
    }
  }

  return result;
}
