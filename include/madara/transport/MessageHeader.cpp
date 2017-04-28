#include "MessageHeader.h"
#include "madara/utility/Utility.h"
#include <algorithm>
#include <time.h>
#include <sstream>

madara::transport::MessageHeader::MessageHeader ()
: size (encoded_size ()),
  type (0), updates (0), quality (0), clock (0), 
  timestamp (time (NULL)), ttl (0)
{
  memcpy (madara_id, MADARA_IDENTIFIER, 7);
  madara_id[7] = 0;

  originator[0] = 0;
  domain[0] = 0;
}


madara::transport::MessageHeader::~MessageHeader ()
{
}

uint32_t
madara::transport::MessageHeader::encoded_size (void) const
{
  return sizeof (uint64_t) * 3  // size, clock, timestamp
    + sizeof (char) * (MADARA_IDENTIFIER_LENGTH + MADARA_DOMAIN_MAX_LENGTH
                        + MAX_ORIGINATOR_LENGTH + 1)
    + sizeof (uint32_t) * 3;   // type, updates, quality 
}

uint32_t
madara::transport::MessageHeader::static_encoded_size (void)
{
  return sizeof (uint64_t) * 3  // size, clock, timestamp
    + sizeof (char) * (MADARA_IDENTIFIER_LENGTH + MADARA_DOMAIN_MAX_LENGTH
                        + MAX_ORIGINATOR_LENGTH + 1)
    + sizeof (uint32_t) * 3;   // type, updates, quality 
}

const char *
madara::transport::MessageHeader::read (const char * buffer,
                                         int64_t & buffer_remaining)
{
  // Remove size field from the buffer and update accordingly
  if (buffer_remaining >= sizeof (size))
  {
    memcpy (&size, buffer, sizeof (size));
    size = madara::utility::endian_swap (size);
    buffer += sizeof (size);
  }
  buffer_remaining -= sizeof (size);

  // Remove madara_id field from the buffer and update accordingly
  if (buffer_remaining >= sizeof (char) * MADARA_IDENTIFIER_LENGTH)
  {
    strncpy (madara_id, buffer, MADARA_IDENTIFIER_LENGTH);
    buffer += sizeof (char) * MADARA_IDENTIFIER_LENGTH;
  }
  buffer_remaining -= sizeof (char) * MADARA_IDENTIFIER_LENGTH;
  
  // Remove domain field from the buffer and update accordingly
  if (buffer_remaining >= sizeof (char) * MADARA_DOMAIN_MAX_LENGTH)
  {
    strncpy (domain, buffer, MADARA_DOMAIN_MAX_LENGTH);
    buffer += sizeof (char) * MADARA_DOMAIN_MAX_LENGTH;
  }
  buffer_remaining -= sizeof (char) * MADARA_DOMAIN_MAX_LENGTH;
  
  // Remove originator from the buffer and update accordingly
  if (buffer_remaining >= sizeof (char) * MAX_ORIGINATOR_LENGTH)
  {
    strncpy (originator, buffer, MAX_ORIGINATOR_LENGTH);
    buffer += sizeof (char) * MAX_ORIGINATOR_LENGTH;
  }
  buffer_remaining -= sizeof (char) * MAX_ORIGINATOR_LENGTH;

  // Remove type field from the buffer and update accordingly
  if (buffer_remaining >= sizeof (type))
  {
    memcpy (&type, buffer, sizeof (type));
    type = madara::utility::endian_swap (type);
    buffer += sizeof (type);
  }
  buffer_remaining -= sizeof (type);
  
  // Remove updates field from the buffer and update accordingly
  if (buffer_remaining >= sizeof (updates))
  {
    memcpy (&updates, buffer, sizeof (updates));
    updates = madara::utility::endian_swap (updates);
    buffer += sizeof (updates);
  }
  buffer_remaining -= sizeof (updates);
  
  // Remove quality field from the buffer and update accordingly
  if (buffer_remaining >= sizeof (quality))
  {
    memcpy (&quality, buffer, sizeof (quality));
    quality = madara::utility::endian_swap (quality);
    buffer += sizeof (quality);
  }
  buffer_remaining -= sizeof (quality);
  
  // Remove clock field from the buffer and update accordingly
  if (buffer_remaining >= sizeof (clock))
  {
    memcpy (&clock, buffer, sizeof (clock));
    clock = madara::utility::endian_swap (clock);
    buffer += sizeof (clock);
  }
  buffer_remaining -= sizeof (clock);
  
  // Remove timestamp field from the buffer and update accordingly
  if (buffer_remaining >= sizeof (timestamp))
  {
    memcpy (&timestamp, buffer, sizeof (timestamp));
    timestamp = madara::utility::endian_swap (timestamp);
    buffer += sizeof (timestamp);
  }
  buffer_remaining -= sizeof (timestamp);

  // Remove the time to live field from the buffer
  if (buffer_remaining >= 1)
  {
    memcpy (&ttl, buffer, 1);
    buffer += 1;
  }
  buffer_remaining -= 1;

  return buffer;
}

char *
madara::transport::MessageHeader::write (char * buffer,
                                         int64_t & buffer_remaining)
{
  // Write size field from the buffer and update accordingly
  if (buffer_remaining >= sizeof (size))
  {
    *(uint64_t *) buffer = madara::utility::endian_swap (size);
    buffer += sizeof (size);
  }
  buffer_remaining -= sizeof (size);

  // Write madara_id field from the buffer and update accordingly
  if (buffer_remaining >= sizeof (char) * MADARA_IDENTIFIER_LENGTH)
  {
    strncpy (buffer, madara_id, MADARA_IDENTIFIER_LENGTH);
    buffer += sizeof (char) * MADARA_IDENTIFIER_LENGTH;
  }
  buffer_remaining -= sizeof (char) * MADARA_IDENTIFIER_LENGTH;
  
  // Write domain field from the buffer and update accordingly
  if (buffer_remaining >= sizeof (char) * MADARA_DOMAIN_MAX_LENGTH)
  {
    strncpy (buffer, domain, MADARA_DOMAIN_MAX_LENGTH);
    buffer += sizeof (char) * MADARA_DOMAIN_MAX_LENGTH;
  }
  buffer_remaining -= sizeof (char) * MADARA_DOMAIN_MAX_LENGTH;
  
  // Write originator from the buffer and update accordingly
  if (buffer_remaining >= sizeof (char) * MAX_ORIGINATOR_LENGTH)
  {
    strncpy (buffer, originator, MAX_ORIGINATOR_LENGTH);
    buffer += sizeof (char) * MAX_ORIGINATOR_LENGTH;
  }
  buffer_remaining -= sizeof (char) * MAX_ORIGINATOR_LENGTH;

  // Write type field from the buffer and update accordingly
  if (buffer_remaining >= sizeof (type))
  {
    *(uint32_t *) buffer = madara::utility::endian_swap (type);
    buffer += sizeof (type);
  }
  buffer_remaining -= sizeof (type);
  
  // Write updates field from the buffer and update accordingly
  if (buffer_remaining >= sizeof (updates))
  {
    *(uint32_t *) buffer = madara::utility::endian_swap (updates);
    buffer += sizeof (updates);
  }
  buffer_remaining -= sizeof (updates);
  
  // Write quality field from the buffer and update accordingly
  if (buffer_remaining >= sizeof (quality))
  {
    *(uint32_t *) buffer = madara::utility::endian_swap (quality);
    buffer += sizeof (quality);
  }
  buffer_remaining -= sizeof (quality);
  
  // Write clock field from the buffer and update accordingly
  if (buffer_remaining >= sizeof (clock))
  {
    *(uint64_t *) buffer = madara::utility::endian_swap (clock);
    buffer += sizeof (clock);
  }
  buffer_remaining -= sizeof (clock);
  
  // Write timestamp field from the buffer and update accordingly
  if (buffer_remaining >= sizeof (timestamp))
  {
    *(uint64_t *) buffer = madara::utility::endian_swap (timestamp);
    buffer += sizeof (timestamp);
  }
  buffer_remaining -= sizeof (timestamp);

  if (buffer_remaining >= 1)
  {
    memcpy (buffer, &ttl, 1);
    buffer += 1;
  }
  buffer_remaining -= 1;

  return buffer;
}

std::string
madara::transport::MessageHeader::to_string (void)
{
  std::stringstream buffer;

  buffer << "140: size (8:" << size << "), ";
  buffer << "encoding (8:" << madara_id << "), ";
  buffer << "domain (32:" << domain << "), ";
  buffer << "orig (64:" << originator << "), ";
  buffer << "type (4:" << type << "), ";
  buffer << "numupdates (4:" << updates << "), ";
  buffer << "quality (4:" << quality << "), ";
  buffer << "clock (8:" << clock << "), ";
  buffer << "wallclock (8:" << timestamp << "), ";
  buffer << "ttl (1:" << (int)ttl << "), ";

  return buffer.str ();
}

uint64_t
madara::transport::MessageHeader::get_size (const char * buffer)
{
  return (madara::utility::endian_swap (*(uint64_t *)buffer));
}
 
bool
madara::transport::MessageHeader::equals (const MessageHeader & other)
{
  return size == other.size &&
         type == other.type &&
         updates == other.updates &&
         quality == other.quality &&
         clock == other.clock &&
         timestamp == other.timestamp &&
         strncmp (madara_id, other.madara_id, MADARA_IDENTIFIER_LENGTH) == 0 &&
         strncmp (domain, other.domain, MADARA_DOMAIN_MAX_LENGTH) == 0 &&
         strncmp (originator, other.originator, MAX_ORIGINATOR_LENGTH) == 0;
}
