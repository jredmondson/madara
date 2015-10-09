#include "FileHeader.h"
#include "madara/utility/Utility.h"
#include <algorithm>
#include <time.h>

madara::knowledge::FileHeader::FileHeader ()
: size (encoded_size ()),
  states (0), initial_timestamp (time (NULL)),
  last_timestamp (0),
  karl_version (madara::utility::get_uint_version ())
{
  memcpy (file_type, "KaRL", 4);
  file_type[4] = 0;
  file_type[5] = 0;
  file_type[6] = 0;
  file_type[7] = 0;

  originator[0] = 0;

  last_timestamp = initial_timestamp;
}


madara::knowledge::FileHeader::~FileHeader ()
{
}

uint32_t
madara::knowledge::FileHeader::encoded_size (void)
{
  return sizeof (uint64_t) * 4
    + sizeof (char) * (64 + 8)
    + sizeof (uint32_t);
}

const char *
madara::knowledge::FileHeader::read (const char * buffer,
                                         int64_t & buffer_remaining)
{
  // Remove size field from the buffer and update accordingly
  if (buffer_remaining >= sizeof (size))
  {
    size = madara::utility::endian_swap (*(uint64_t *)buffer);
    buffer += sizeof (size);
  }
  buffer_remaining -= sizeof (size);
  
  // Remove states field from the buffer and update accordingly
  if (buffer_remaining >= sizeof (states))
  {
    states = madara::utility::endian_swap (*(uint64_t *)buffer);
    buffer += sizeof (states);
  }
  buffer_remaining -= sizeof (states);
  
  // Remove initial_timestamp field from the buffer and update accordingly
  if (buffer_remaining >= sizeof (initial_timestamp))
  {
    initial_timestamp = madara::utility::endian_swap (*(uint64_t *)buffer);
    buffer += sizeof (initial_timestamp);
  }
  buffer_remaining -= sizeof (initial_timestamp);
  
  // Remove initial_timestamp field from the buffer and update accordingly
  if (buffer_remaining >= sizeof (last_timestamp))
  {
    last_timestamp = madara::utility::endian_swap (*(uint64_t *)buffer);
    buffer += sizeof (last_timestamp);
  }
  buffer_remaining -= sizeof (last_timestamp);
  
  // Remove file_type from the buffer and update accordingly
  if (buffer_remaining >= sizeof (char) * 8)
  {
    strncpy (file_type, buffer, 8);
    buffer += sizeof (char) * 8;
  }
  buffer_remaining -= sizeof (char) * 8;
  
  // Remove karl_version field from the buffer and update accordingly
  if (buffer_remaining >= sizeof (karl_version))
  {
    karl_version = madara::utility::endian_swap (*(uint32_t *)buffer);
    buffer += sizeof (karl_version);
  }
  buffer_remaining -= sizeof (karl_version);
  
  // Remove originator from the buffer and update accordingly
  if (buffer_remaining >= sizeof (char) * 64)
  {
    strncpy (originator, buffer, 64);
    buffer += sizeof (char) * 64;
  }
  buffer_remaining -= sizeof (char) * 64;

  return buffer;
}

char *
madara::knowledge::FileHeader::write (char * buffer,
                                         int64_t & buffer_remaining)
{
  // Write size field to the buffer and update accordingly
  if (buffer_remaining >= sizeof (size))
  {
    *(uint64_t *) buffer = madara::utility::endian_swap (size);
    buffer += sizeof (size);
  }
  buffer_remaining -= sizeof (size);
  
  // Write states field to the buffer and update accordingly
  if (buffer_remaining >= sizeof (states))
  {
    *(uint64_t *) buffer = madara::utility::endian_swap (states);
    buffer += sizeof (states);
  }
  buffer_remaining -= sizeof (states);
  
  // Write initial_timestamp field to the buffer and update accordingly
  if (buffer_remaining >= sizeof (initial_timestamp))
  {
    *(uint64_t *) buffer = madara::utility::endian_swap (initial_timestamp);
    buffer += sizeof (initial_timestamp);
  }
  buffer_remaining -= sizeof (initial_timestamp);
  
  // Write last_timestamp field to the buffer and update accordingly
  if (buffer_remaining >= sizeof (last_timestamp))
  {
    *(uint64_t *) buffer = madara::utility::endian_swap (last_timestamp);
    buffer += sizeof (last_timestamp);
  }
  buffer_remaining -= sizeof (last_timestamp);
  
  // Write file_type field from the buffer and update accordingly
  if (buffer_remaining >= sizeof (char) * 8)
  {
    strncpy (buffer, file_type, 8);
    buffer += sizeof (char) * 8;
  }
  buffer_remaining -= sizeof (char) * 8;
  
  // Write karl_version field to the buffer and update accordingly
  if (buffer_remaining >= sizeof (karl_version))
  {
    *(uint32_t *) buffer = madara::utility::endian_swap (karl_version);
    buffer += sizeof (karl_version);
  }
  buffer_remaining -= sizeof (karl_version);


  // Write originator field from the buffer and update accordingly
  if (buffer_remaining >= sizeof (char) * 64)
  {
    strncpy (buffer, originator, 64);
    buffer += sizeof (char) * 64;
  }
  buffer_remaining -= sizeof (char) * 64;
  
  return buffer;
}

bool
madara::knowledge::FileHeader::equals (const FileHeader & other)
{
  return size == other.size &&
         states == other.states &&
         initial_timestamp == other.initial_timestamp &&
         last_timestamp == other.last_timestamp &&
         karl_version == other.karl_version &&
         strncmp (file_type, other.file_type, 4) == 0 &&
         strncmp (originator, other.originator, 64) == 0;
}
