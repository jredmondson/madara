#include "Reduced_Message_Header.h"
#include "madara/utility/Utility.h"
#include <algorithm>
#include <string.h>

Madara::Transport::Reduced_Message_Header::Reduced_Message_Header ()
: Message_Header ()
{
  memcpy (madara_id, REDUCED_MADARA_ID, 7);
  madara_id[7] = 0;
}

Madara::Transport::Reduced_Message_Header::~Reduced_Message_Header ()
{
}

uint32_t
Madara::Transport::Reduced_Message_Header::encoded_size (void) const
{
  return sizeof (uint64_t) * 3                       // size, clock, timestamp
    + sizeof (char) * (MADARA_IDENTIFIER_LENGTH + 1) // KaRL1.0 + ttl
    + sizeof (uint32_t) * 1;                         // updates
}

const char *
  Madara::Transport::Reduced_Message_Header::read (const char * buffer,
  int64_t & buffer_remaining)
{
  // Remove size field from the buffer and update accordingly
  if (buffer_remaining >= sizeof (size))
  {
    memcpy (&size, buffer, sizeof (size));
    size = Madara::Utility::endian_swap (size);
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

  // Remove updates field from the buffer and update accordingly
  if (buffer_remaining >= sizeof (updates))
  {
    memcpy (&updates, buffer, sizeof (updates));
    updates = Madara::Utility::endian_swap (updates);
    buffer += sizeof (updates);
  }
  buffer_remaining -= sizeof (updates);

  // Remove clock field from the buffer and update accordingly
  if (buffer_remaining >= sizeof (clock))
  {
    memcpy (&clock, buffer, sizeof (clock));
    clock = Madara::Utility::endian_swap (clock);
    buffer += sizeof (clock);
  }
  buffer_remaining -= sizeof (clock);
  
  // Remove timestamp field from the buffer and update accordingly
  if (buffer_remaining >= sizeof (timestamp))
  {
    memcpy (&timestamp, buffer, sizeof (timestamp));
    timestamp = Madara::Utility::endian_swap (timestamp);
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
  Madara::Transport::Reduced_Message_Header::write (char * buffer,
  int64_t & buffer_remaining)
{
  // Write size field from the buffer and update accordingly
  if (buffer_remaining >= sizeof (size))
  {
    *(uint64_t *) buffer = Madara::Utility::endian_swap (size);
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

  // Write updates field from the buffer and update accordingly
  if (buffer_remaining >= sizeof (updates))
  {
    *(uint32_t *) buffer = Madara::Utility::endian_swap (updates);
    buffer += sizeof (updates);
  }
  buffer_remaining -= sizeof (updates);

  // Write clock field from the buffer and update accordingly
  if (buffer_remaining >= sizeof (clock))
  {
    *(uint64_t *) buffer = Madara::Utility::endian_swap (clock);
    buffer += sizeof (clock);
  }
  buffer_remaining -= sizeof (clock);
  
  // Write timestamp field from the buffer and update accordingly
  if (buffer_remaining >= sizeof (timestamp))
  {
    *(uint64_t *) buffer = Madara::Utility::endian_swap (timestamp);
    buffer += sizeof (timestamp);
  }
  buffer_remaining -= sizeof (timestamp);

  // Write the time to live field
  if (buffer_remaining >= 1)
  {
    memcpy (buffer, &ttl, 1);
    buffer += 1;
  }
  buffer_remaining -= 1;

  return buffer;
}

bool
Madara::Transport::Reduced_Message_Header::equals (
  const Message_Header & other)
{
  return size == other.size &&
    updates == other.updates &&
    clock == other.clock &&
    timestamp == other.timestamp;
}

