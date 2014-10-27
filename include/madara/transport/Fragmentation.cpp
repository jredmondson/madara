#include "Fragmentation.h"
#include "Reduced_Message_Header.h"
#include "madara/utility/Utility.h"
#include "madara/utility/Log_Macros.h"
#include <algorithm>
#include <time.h>

Madara::Transport::Fragment_Message_Header::Fragment_Message_Header ()
: Message_Header (), update_number (0)
{
  memcpy (madara_id, FRAGMENTATION_MADARA_ID, 7);
  madara_id[7] = 0;

  originator[0] = 0;
  domain[0] = 0;
}


Madara::Transport::Fragment_Message_Header::~Fragment_Message_Header ()
{
}

uint32_t
Madara::Transport::Fragment_Message_Header::encoded_size (void) const
{
  return sizeof (uint64_t) * 3  // size, clock, timestamp
    + sizeof (char) * (MADARA_IDENTIFIER_LENGTH + MADARA_DOMAIN_MAX_LENGTH
                        + MAX_ORIGINATOR_LENGTH + 1)
    + sizeof (uint32_t) * 4;   // type, updates, quality, update_number 
}

uint32_t
Madara::Transport::Fragment_Message_Header::static_encoded_size (void)
{
  return sizeof (uint64_t) * 3  // size, clock, timestamp
    + sizeof (char) * (MADARA_IDENTIFIER_LENGTH + MADARA_DOMAIN_MAX_LENGTH
                        + MAX_ORIGINATOR_LENGTH + 1)
    + sizeof (uint32_t) * 4;   // type, updates, quality, update_number 
}

     

uint32_t
Madara::Transport::Fragment_Message_Header::get_updates (const char * buffer)
{
  buffer += 116;
  return (Madara::Utility::endian_swap (*(uint32_t *)buffer));
}

const char *
Madara::Transport::Fragment_Message_Header::read (const char * buffer,
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
    type = Madara::Utility::endian_swap (type);
    buffer += sizeof (type);
  }
  buffer_remaining -= sizeof (type);
  
  // Remove updates field from the buffer and update accordingly
  if (buffer_remaining >= sizeof (updates))
  {
    memcpy (&updates, buffer, sizeof (updates));
    updates = Madara::Utility::endian_swap (updates);
    buffer += sizeof (updates);
  }
  buffer_remaining -= sizeof (updates);
  
  // Remove quality field from the buffer and update accordingly
  if (buffer_remaining >= sizeof (quality))
  {
    memcpy (&quality, buffer, sizeof (quality));
    quality = Madara::Utility::endian_swap (quality);
    buffer += sizeof (quality);
  }
  buffer_remaining -= sizeof (quality);
  
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
  
  // Remove updates field from the buffer and update accordingly
  if (buffer_remaining >= sizeof (update_number))
  {
    memcpy (&update_number, buffer, sizeof (update_number));
    update_number = Madara::Utility::endian_swap (update_number);
    buffer += sizeof (update_number);
  }
  buffer_remaining -= sizeof (update_number);
  
  return buffer;
}

char *
Madara::Transport::Fragment_Message_Header::write (char * buffer,
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
    *(uint32_t *) buffer = Madara::Utility::endian_swap (type);
    buffer += sizeof (type);
  }
  buffer_remaining -= sizeof (type);
  
  // Write updates field from the buffer and update accordingly
  if (buffer_remaining >= sizeof (updates))
  {
    *(uint32_t *) buffer = Madara::Utility::endian_swap (updates);
    buffer += sizeof (updates);
  }
  buffer_remaining -= sizeof (updates);
  
  // Write quality field from the buffer and update accordingly
  if (buffer_remaining >= sizeof (quality))
  {
    *(uint32_t *) buffer = Madara::Utility::endian_swap (quality);
    buffer += sizeof (quality);
  }
  buffer_remaining -= sizeof (quality);
  
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

  if (buffer_remaining >= 1)
  {
    memcpy (buffer, &ttl, 1);
    buffer += 1;
  }
  buffer_remaining -= 1;
  
  // Write updates field from the buffer and update accordingly
  if (buffer_remaining >= sizeof (update_number))
  {
    *(uint32_t *) buffer = Madara::Utility::endian_swap (update_number);
    buffer += sizeof (update_number);
  }
  buffer_remaining -= sizeof (update_number);
  
  return buffer;
}

bool
Madara::Transport::Fragment_Message_Header::equals (
  const Message_Header & other)
{
  const Fragment_Message_Header * rhs = dynamic_cast <
    const Fragment_Message_Header *> (&other);

  return size == rhs->size &&
         type == rhs->type &&
         updates == rhs->updates &&
         update_number == rhs->update_number &&
         quality == rhs->quality &&
         clock == rhs->clock &&
         timestamp == rhs->timestamp &&
         strncmp (madara_id, rhs->madara_id, MADARA_IDENTIFIER_LENGTH) == 0 &&
         strncmp (domain, rhs->domain, MADARA_DOMAIN_MAX_LENGTH) == 0 &&
         strncmp (originator, rhs->originator, MAX_ORIGINATOR_LENGTH) == 0;
}


char * Madara::Transport::defrag (Fragment_Map & map)
{
  char * result = 0;
  
  MADARA_DEBUG (MADARA_LOG_DETAILED_TRACE, (LM_DEBUG, 
    "Transport::defrag:" \
    " defragging fragment map.\n"));

  Fragment_Map::iterator i = map.find (0);
  if (i != map.end ())
  {
    Fragment_Message_Header header;
    int64_t buffer_remaining (header.encoded_size ());
    const char * buffer = header.read (i->second, buffer_remaining);

    // do we have enough updates to defragment?
    if (header.updates <= map.size ())
    {
      MADARA_DEBUG (MADARA_LOG_DETAILED_TRACE, (LM_DEBUG, 
        "Transport::defrag:" \
        " the map is large enough to contain updates.\n"));

      int64_t size = 0;
      if (Message_Header::message_header_test (buffer))
      {
        MADARA_DEBUG (MADARA_LOG_DETAILED_TRACE, (LM_DEBUG, 
          "Transport::defrag:" \
          " regular message header detected.\n"));

        Message_Header contents_header;
        buffer_remaining = contents_header.encoded_size ();
        contents_header.read (buffer, buffer_remaining);
        size = contents_header.size;
      }
      else if (Reduced_Message_Header::message_header_test (buffer))
      {
        MADARA_DEBUG (MADARA_LOG_DETAILED_TRACE, (LM_DEBUG, 
          "Transport::defrag:" \
          " reduced message header detected.\n"));

        Reduced_Message_Header contents_header;
        buffer_remaining = contents_header.encoded_size ();
        contents_header.read (buffer, buffer_remaining);
        size = contents_header.size;
      }

      result = new char [size];
      char * lhs = result;
      
      uint32_t actual_size = header.size - header.encoded_size ();
      buffer_remaining = actual_size;

      if (size >= 0)
      {
        MADARA_DEBUG (MADARA_LOG_DETAILED_TRACE, (LM_DEBUG, 
          "Transport::defrag:" \
          " copying buffer to lhs.\n"));

        memcpy (lhs, buffer, buffer_remaining);
        buffer += actual_size;
        lhs += actual_size;
        size -= actual_size;
      }

      // if so, iterate over the fragments and copy the contents
      for (++i ;i != map.end (); ++i)
      {
        MADARA_DEBUG (MADARA_LOG_DETAILED_TRACE, (LM_DEBUG, 
          "Transport::defrag:" \
          " reading header of new fragment.\n"));

        buffer_remaining  = header.encoded_size ();
        buffer = header.read (i->second, buffer_remaining);
        actual_size = header.size - header.encoded_size ();
        buffer_remaining = actual_size;
        
        if (size >= 0)
        {
          MADARA_DEBUG (MADARA_LOG_DETAILED_TRACE, (LM_DEBUG, 
            "Transport::defrag:" \
            " copying buffer to lhs.\n"));

          memcpy (lhs, buffer, buffer_remaining);
          buffer += actual_size;
          lhs += actual_size;
          size -= actual_size;
        }
      }
    }
  }

  return result;
}

void
Madara::Transport::delete_fragments (Fragment_Map & map)
{
  for (Fragment_Map::iterator i = map.begin (); i != map.end (); ++i)
  {
    delete [] i->second;
  }
  map.clear ();
}

char *
Madara::Transport::add_fragment (const char * originator, uint64_t clock,
      uint32_t update_number, const char * fragment,
      uint32_t queue_length,
      Originator_Fragment_Map & map, bool clear)
{
  char * result = 0;

  /**
  * add a fragment to the map in the following conditions
  * 1) originator does not exist yet in the map
  * 2) clock is newer than existing fragments
  * 3) fragment update does not exist yet in an existing clock entry
  *
  * clear a fragment map and return a buffer in the following situations
  * 4) if we have all fragments
  **/
  
  MADARA_DEBUG (MADARA_LOG_DETAILED_TRACE, (LM_DEBUG, 
    "Transport::add_fragment:" \
    " adding fragment.\n"));

  char * new_fragment = 0;
  Fragment_Message_Header header;
  int64_t buffer_remaining = header.encoded_size ();

  
  MADARA_DEBUG (MADARA_LOG_DETAILED_TRACE, (LM_DEBUG, 
    "Transport::add_fragment:" \
    " reading header from buffer.\n"));

  header.read (fragment, buffer_remaining);

  if (header.size > 0)
  {
    MADARA_DEBUG (MADARA_LOG_DETAILED_TRACE, (LM_DEBUG, 
      "Transport::add_fragment:" \
      " creating buffer to hold fragment.\n"));

    new_fragment = new char [header.size];
    memcpy (new_fragment, fragment, header.size);
  }
  else
    return 0;
  
  MADARA_DEBUG (MADARA_LOG_DETAILED_TRACE, (LM_DEBUG, 
    "Transport::add_fragment:" \
    " searching for originator %s.\n", originator));

  Originator_Fragment_Map::iterator orig_map = map.find (originator);
  if (orig_map == map.end ())
  {
    MADARA_DEBUG (MADARA_LOG_DETAILED_TRACE, (LM_DEBUG, 
      "Transport::add_fragment:" \
      " creating entry for originator %s.\n", originator));

    // originator does not exist (1)
    map[originator][clock][update_number] = new_fragment;
  }

  else
  {
    MADARA_DEBUG (MADARA_LOG_DETAILED_TRACE, (LM_DEBUG, 
      "Transport::add_fragment:" \
      " originator %s exists in fragment map.\n", originator));

    Clock_Fragment_Map & clock_map (orig_map->second);
    Clock_Fragment_Map::iterator clock_found = clock_map.find (clock);
    
    if (clock_found != clock_map.end ())
    {
      // we have found the clock entry
      if (clock_found->second.find (update_number)
            == clock_found->second.end ())
      {
        if (clock_found->second.size () != 0)
        {
          MADARA_DEBUG (MADARA_LOG_DETAILED_TRACE, (LM_DEBUG, 
            "Transport::add_fragment:" \
            " %s:%d is being added.\n", originator, update_number));

          // the fragment does not exist yet
          clock_found->second [update_number] = new_fragment;

          // check for a new buffer
          result = defrag (clock_found->second);

          if (result && clear)
          {
            MADARA_DEBUG (MADARA_LOG_DETAILED_TRACE, (LM_DEBUG, 
              "Transport::add_fragment:" \
              " %s:%d is complete. Deleting fragments.\n",
              originator, update_number));

            delete_fragments (clock_found->second);
            clock_map.erase (clock);
          }
          else
          {
            MADARA_DEBUG (MADARA_LOG_DETAILED_TRACE, (LM_DEBUG, 
              "Transport::add_fragment:" \
              " %s:%d is incomplete. Need more fragments.\n",
              originator, update_number));
          }
        }
        else
        {
          // if we get here, the message has already been defragged
            MADARA_DEBUG (MADARA_LOG_DETAILED_TRACE, (LM_DEBUG, 
              "Transport::add_fragment:" \
              " %s:%d has been previously defragged and fragments deleted.\n",
              originator, update_number));

        }
      }
    }

    else if (clock_map.size () < queue_length)
    {
      MADARA_DEBUG (MADARA_LOG_DETAILED_TRACE, (LM_DEBUG, 
        "Transport::add_fragment:" \
        " %s:%d is being added to queue.\n",
        originator, update_number));

      // clock queue has not been exhausted (2)
      clock_map [clock][update_number] = new_fragment;
    }
    else
    {
      MADARA_DEBUG (MADARA_LOG_DETAILED_TRACE, (LM_DEBUG, 
        "Transport::add_fragment:" \
        " %s:%d is being added to queue after a deletion.\n",
        originator, update_number));

      uint32_t oldest = clock_map.begin ()->first;

      if (oldest < clock)
      {
        MADARA_DEBUG (MADARA_LOG_DETAILED_TRACE, (LM_DEBUG, 
          "Transport::add_fragment:" \
          " deleting fragments.\n",
          originator, update_number));

        Fragment_Map & fragments = clock_map [oldest];

        // delete all fragments in the clock entry
        for (Fragment_Map::iterator i = fragments.begin ();
             i != fragments.end (); ++i)
        {
          delete [] i->second;
        }
        
        MADARA_DEBUG (MADARA_LOG_DETAILED_TRACE, (LM_DEBUG, 
          "Transport::add_fragment:" \
          " erasing old clock.\n",
          originator, update_number));

        // erase the oldest clock fragment map
        clock_map.erase (oldest);

        // replace it in the logical queue with the new clock fragment
        clock_map [clock][update_number] = new_fragment;
      }
    }
  }

  return result;
}

void
Madara::Transport::Fragment_Message_Header::operator= (
  Message_Header & header)
{
  if (this != &header)
  {
    clock = header.clock;
    strncpy (domain, header.domain, MADARA_DOMAIN_MAX_LENGTH);
    strncpy (originator, header.originator, MAX_ORIGINATOR_LENGTH);
    quality = header.quality;
    size = header.size;
    timestamp = header.timestamp;
    ttl = header.ttl;
    type = header.type;
    updates = header.updates;
  }
}


void
Madara::Transport::frag (char * source,
  uint32_t fragment_size, Fragment_Map & map)
{
  if (fragment_size > 0)
  {
    MADARA_DEBUG (MADARA_LOG_DETAILED_TRACE, (LM_DEBUG, 
      "Transport::frag:" \
      " fragmenting character stream into %d byte packets.\n",
      fragment_size));

    uint32_t data_per_packet = fragment_size -
      Fragment_Message_Header::static_encoded_size ();

    const char * buffer = source;
    uint64_t total_size;
    Fragment_Message_Header header;
    if (Message_Header::message_header_test (source))
    {
      MADARA_DEBUG (MADARA_LOG_DETAILED_TRACE, (LM_DEBUG, 
        "Transport::frag:" \
        " regular message header detected.\n"));

      Message_Header contents_header;
      int64_t buffer_remaining = contents_header.encoded_size ();
      contents_header.read (source, buffer_remaining);
      header = contents_header;
    }
    else if (Reduced_Message_Header::message_header_test (source))
    {
      MADARA_DEBUG (MADARA_LOG_DETAILED_TRACE, (LM_DEBUG, 
        "Transport::frag:" \
        " reduced message header detected.\n"));

      Reduced_Message_Header contents_header;
      int64_t buffer_remaining = contents_header.encoded_size ();
      contents_header.read (source, buffer_remaining);
      header = contents_header;
    }

    total_size = header.size;
    header.updates = header.size / data_per_packet;

    if (header.size % data_per_packet != 0)
      ++header.updates;
    
    MADARA_DEBUG (MADARA_LOG_DETAILED_TRACE, (LM_DEBUG, 
      "Transport::frag:" \
      " iterating over %d updates.\n",
      header.updates));

    for (uint32_t i = 0; i < header.updates; ++i)
    {
      char * new_frag;
      size_t cur_size;
      int64_t buffer_remaining;
      uint64_t actual_data_size; 

      if (i == header.updates - 1)
        cur_size = (size_t) total_size +
          Fragment_Message_Header::static_encoded_size ();
      else
        cur_size = (size_t) fragment_size;

      buffer_remaining = cur_size;
      actual_data_size = cur_size -
        Fragment_Message_Header::static_encoded_size ();
      new_frag = new char [cur_size];

      map[i] = new_frag;
      
      MADARA_DEBUG (MADARA_LOG_DETAILED_TRACE, (LM_DEBUG, 
        "Transport::frag:" \
        " writing %d packet of size %d.\n",
        i, cur_size));

      header.update_number = i;
      header.size = cur_size;
      new_frag = header.write (new_frag, buffer_remaining);
      memcpy (new_frag, buffer, (size_t)actual_data_size);
      buffer += actual_data_size;
      total_size -= actual_data_size;
    }
  }
}

bool
Madara::Transport::is_complete (const char * originator, uint64_t clock,
      Originator_Fragment_Map & map)
{
  bool result = false;

  Originator_Fragment_Map::iterator orig_map = map.find (originator);

  if (orig_map != map.end ())
  {
    Clock_Fragment_Map & clock_map (orig_map->second);
    Clock_Fragment_Map::iterator clock_found = clock_map.find (clock);

    if (clock_found != clock_map.end ())
    {
      uint64_t size = clock_found->second.size ();
      Fragment_Map::iterator i = clock_found->second.find (0);

      if (i != clock_found->second.end ())
      {
        if (Fragment_Message_Header::get_updates (i->second) == size)
        {
          result = true;
        }
      }
      else if (size == 0)
      {
        result = true;
      }
    }
  }

  return result;
}

bool
Madara::Transport::exists (const char * originator, uint64_t clock,
  uint32_t update_number, Originator_Fragment_Map & map)
{
  bool result = false;

  Originator_Fragment_Map::iterator orig_map = map.find (originator);

  if (orig_map != map.end ())
  {
    Clock_Fragment_Map & clock_map (orig_map->second);
    Clock_Fragment_Map::iterator clock_found = clock_map.find (clock);

    if (clock_found != clock_map.end ())
    {
      uint64_t size = clock_found->second.size ();

      if (clock_found->second.find (update_number)
          != clock_found->second.end ())
      {
        result = true;
      }
    }
  }

  return result; 
}
