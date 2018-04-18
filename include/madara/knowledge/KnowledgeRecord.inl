

#ifndef _KNOWLEDGE_RECORD_INL_
#define _KNOWLEDGE_RECORD_INL_

#include <algorithm>

#include "madara/utility/Utility.h"

/**
* @file KnowledgeRecord.inl
* @author James Edmondson <jedmondson@gmail.com>
*
* This file contains the methods of the knowledge::KnowledgeRecord class
**/

inline int
madara::knowledge::KnowledgeRecord::status (void) const
{
  return status_;
}

/**
* sets the status to modified
**/
inline void
madara::knowledge::KnowledgeRecord::set_modified (void)
{
  status_ = MODIFIED;
}

inline uint32_t
madara::knowledge::KnowledgeRecord::size (void) const
{
  return size_;
}

inline int32_t
madara::knowledge::KnowledgeRecord::type (void) const
{
  return type_;
}

inline int64_t
madara::knowledge::KnowledgeRecord::get_encoded_size (void) const
{
  int64_t buffer_size (sizeof (uint32_t) * 2);
  if (type_ & (INTEGER | INTEGER_ARRAY))
  {
    buffer_size += sizeof (int_value_) * size_;
  }
  else if (type_ & (DOUBLE | DOUBLE_ARRAY))
  {
    buffer_size += sizeof (double) * size_;
  }
  else
  {
    buffer_size += size_;
  }

  return buffer_size;
}

inline int64_t
madara::knowledge::KnowledgeRecord::get_encoded_size (const std::string & key) const
{
  // for keyed size, add another uint32_t and the size of the key with a null char
  int64_t buffer_size (sizeof (uint32_t) * 1);
  buffer_size += (key.size () + 1);

  // and then add the default encoded size
  buffer_size += get_encoded_size ();

  return buffer_size;
}

inline bool
madara::knowledge::KnowledgeRecord::is_ref_counted (void) const
{
  return is_ref_counted (type_);
}

inline bool
madara::knowledge::KnowledgeRecord::is_ref_counted (uint32_t type) const
{
  return type != INTEGER && type != DOUBLE;
}

inline bool
madara::knowledge::KnowledgeRecord::is_string_type (void) const
{
  return is_string_type (type_);
}

inline bool
madara::knowledge::KnowledgeRecord::is_string_type (uint32_t type) const
{
  return type == STRING || type == XML || type == TEXT_FILE;
}

inline bool
madara::knowledge::KnowledgeRecord::is_double_type (void) const
{
  return is_double_type (type_);
}

inline bool
madara::knowledge::KnowledgeRecord::is_double_type (uint32_t type) const
{
  return type == DOUBLE || type == DOUBLE_ARRAY;
}

inline bool
madara::knowledge::KnowledgeRecord::is_integer_type (void) const
{
  return is_integer_type (type_);
}


inline bool
madara::knowledge::KnowledgeRecord::is_integer_type (uint32_t type) const
{
  return type == INTEGER || type == INTEGER_ARRAY;
}

inline bool
madara::knowledge::KnowledgeRecord::is_array_type (void) const
{
  return is_array_type (type_);
}

inline bool
madara::knowledge::KnowledgeRecord::is_array_type (uint32_t type) const
{
  return type & ALL_ARRAYS;
}

inline bool
madara::knowledge::KnowledgeRecord::is_image_type (void) const
{
  return is_image_type (type_);
}

inline bool
madara::knowledge::KnowledgeRecord::is_image_type (uint32_t type) const
{
  return type == IMAGE_JPEG;
}

inline bool
madara::knowledge::KnowledgeRecord::is_file_type (void) const
{
  return is_file_type (type_);
}

inline bool
madara::knowledge::KnowledgeRecord::is_file_type (uint32_t type) const
{
  return type == TEXT_FILE || type == XML ||
         type == IMAGE_JPEG || type == UNKNOWN_FILE_TYPE;
}

inline bool
madara::knowledge::KnowledgeRecord::is_binary_file_type (void) const
{
  return is_binary_file_type (type_);
}

inline bool
madara::knowledge::KnowledgeRecord::is_binary_file_type (uint32_t type) const
{
  return type == IMAGE_JPEG || type == UNKNOWN_FILE_TYPE;
}

inline uint32_t
madara::knowledge::max_quality (const knowledge::KnowledgeRecords & records)
{
  uint32_t max = 0;

  // iterate over the list and return the max
  for (KnowledgeRecords::const_iterator i = records.begin ();
    i != records.end (); ++i)
  {
    max = std::max <uint32_t> (i->second->quality, max);
  }
  return max;
}

inline uint32_t
madara::knowledge::max_quality (const KnowledgeMap & records)
{
  uint32_t max = 0;

  // iterate over the list and return the max
  for (KnowledgeMap::const_iterator i = records.begin ();
    i != records.end (); ++i)
  {
    max = std::max <uint32_t> (i->second.quality, max);
  }
  return max;
}


inline void
madara::knowledge::KnowledgeRecord::clear_value (void)
{
  if (status_ != UNCREATED)
  {
    if (type_ & ALL_CLEARABLES)
    {
      if (type_ == INTEGER_ARRAY)
        int_array_ = 0;
      else if (type_ == DOUBLE_ARRAY)
        double_array_ = 0;
      else if (is_string_type ())
        str_value_ = 0;
      else if (is_file_type ())
        file_value_ = 0;
    }

    int_value_ = 0;

    type_ = INTEGER;
    status_ = UNCREATED;
    size_ = 0;
  }
}

inline const char *
madara::knowledge::KnowledgeRecord::read (const char * buffer,
  int64_t & buffer_remaining)
{
  // format is [key_size | key | type | value_size | value]

  uint32_t buff_value_size (0);
  uint32_t type = INTEGER;
  uint32_t size = 0;

  // Remove the type of value from the buffer
  if (buffer_remaining >= (int64_t) sizeof (type))
  {
    memcpy (&type, buffer, sizeof (type));
    type = madara::utility::endian_swap (type);
    buffer += sizeof (type);
  }
  buffer_remaining -= sizeof (type);

  // Remove the size of value from the buffer
  if (buffer_remaining >= (int64_t) sizeof (size))
  {
    memcpy (&size, buffer, sizeof (size));
    size = madara::utility::endian_swap (size);

    if (is_integer_type ())
      buff_value_size = size * sizeof (Integer);
    else if (is_double_type ())
      buff_value_size = size * sizeof (double);
    else
      buff_value_size = size;

    buffer += sizeof (buff_value_size);
  }
  buffer_remaining -= sizeof (buff_value_size);

  // Remove the value from the buffer
  if (buffer_remaining >= int64_t (buff_value_size))
  {
    if (is_string_type ())
    {
      str_value_ = new char[buff_value_size];
      strncpy (str_value_.get_ptr (), buffer, buff_value_size);
    }

    else if (type == INTEGER)
    {
      memcpy (&int_value_, buffer, sizeof (int_value_));
      int_value_ = madara::utility::endian_swap (int_value_);
    }

    else if (type == INTEGER_ARRAY)
    {
      Integer * ptr_temp = new Integer[size];

      memcpy (ptr_temp, buffer, buff_value_size);

      for (uint32_t i = 0; i < size; ++i)
      {
        ptr_temp[i] = madara::utility::endian_swap (ptr_temp[i]);
      }

      int_array_ = ptr_temp;
    }

    else if (type == DOUBLE)
    {
      memcpy (&double_value_, buffer, sizeof (double_value_));
      double_value_ = madara::utility::endian_swap (double_value_);
    }

    else if (type == DOUBLE_ARRAY)
    {
      double * ptr_temp = new double[size_];

      memcpy (ptr_temp, buffer, buff_value_size);

      for (uint32_t i = 0; i < size_; ++i)
      {
        ptr_temp[i] = madara::utility::endian_swap (ptr_temp[i]);
      }

      double_array_ = ptr_temp;
    }

    else if (is_file_type (type))
    {
      file_value_ = new unsigned char[size_];
      memcpy (file_value_.get_ptr (), buffer, size_);
    }

    buffer += buff_value_size;
    buffer_remaining -= sizeof (char) * buff_value_size;

    type_ = type;
    size_ = size;
    status_ = MODIFIED;
  }

  return buffer;
}

inline const char *
madara::knowledge::KnowledgeRecord::read (const char * buffer, std::string & key,
int64_t & buffer_remaining)
{
  // format is [key_size | key | type | value_size | value]

  uint32_t key_size (0);

  // Remove the key size from the buffer
  if (buffer_remaining >= (int64_t) sizeof (key_size))
  {
    memcpy (&key_size, buffer, sizeof (key_size));
    key_size = madara::utility::endian_swap (key_size);
    buffer += sizeof (key_size);
  }
  buffer_remaining -= sizeof (key_size);

  // Remove the key from the buffer
  if (buffer_remaining >= int64_t (sizeof (char) * int64_t (key_size)))
  {
    if (key_size > 0) {
      // don't worry about null terminator
      key.assign (buffer, key_size - 1);
    } else {
      key.clear ();
    }

    buffer += sizeof (char) * key_size;
  }
  buffer_remaining -= sizeof (char) * int64_t (key_size);

  // read the type and data
  buffer = read (buffer, buffer_remaining);

  return buffer;
}

inline const char *
madara::knowledge::KnowledgeRecord::read (const char * buffer, uint32_t & key_id,
int64_t & buffer_remaining)
{
  // format is [key_id | type | value_size | value]

  // Remove the key size from the buffer
  if (buffer_remaining >= (int64_t) sizeof (key_id))
  {
    memcpy (&key_id, buffer, sizeof (key_id));
    key_id = madara::utility::endian_swap (key_id);
    buffer += sizeof (key_id);
    buffer_remaining -= sizeof (key_id);

    // read the type and data
    buffer = read (buffer, buffer_remaining);
  }

  return buffer;
}

// reset the value_ to an integer
inline void  
madara::knowledge::KnowledgeRecord::reset_value (void)
{
  clear_value ();

  quality = 0;
  write_quality = 0;
  clock = 0;
}

// set the value_ to a string
inline void
madara::knowledge::KnowledgeRecord::set_value (const std::string & new_value)
{
  if (type_ & ALL_CLEARABLES)
    clear_value ();

  type_ = STRING;
  status_ = MODIFIED;

  // create a new char array and copy over the string
  size_ = uint32_t (new_value.length () + 1);
  char * temp = new char [size_];
  strncpy (temp, new_value.c_str (), size_ - 1);
  temp[size_ - 1] = 0;
  str_value_ = temp;
}

// set the value_ to a string
inline void
madara::knowledge::KnowledgeRecord::set_xml (const char * new_value, size_t size)
{
  if (type_ & ALL_CLEARABLES)
    clear_value ();

  type_ = XML;
  status_ = MODIFIED;

  // create a new char array and copy over the string
  size_ = uint32_t (size + 1);
  char * temp = new char [size_];
  strncpy (temp, new_value, size_ - 1);
  temp[size_ - 1] = 0;
  str_value_ = temp;
}

// set the value_ to a string
inline void
madara::knowledge::KnowledgeRecord::set_text (const char * new_value, size_t size)
{
  if (type_ & ALL_CLEARABLES)
    clear_value ();

  type_ = TEXT_FILE;
  status_ = MODIFIED;

  // create a new char array and copy over the string
  size_ = uint32_t (size);
  char * temp = new char [size_];
  strncpy (temp, new_value, size_ - 1);
  temp[size_ - 1] = 0;
  str_value_ = temp;
}

// set the value_ to a string
inline void
madara::knowledge::KnowledgeRecord::set_jpeg (const unsigned char * new_value,
                                    size_t size)
{
  if (type_ & ALL_CLEARABLES)
    clear_value ();

  type_ = IMAGE_JPEG;
  status_ = MODIFIED;

  // create a new char array and copy over the string
  size_ = uint32_t (size);
  unsigned char * temp = new unsigned char [size_];
  memcpy (temp, new_value, size_);
  file_value_ = temp;
}

// set the value_ to a string
inline void
madara::knowledge::KnowledgeRecord::set_file (const unsigned char * new_value,
                                    size_t size)
{
  if (type_ & ALL_CLEARABLES)
    clear_value ();

  type_ = UNKNOWN_FILE_TYPE;
  status_ = MODIFIED;

  // create a new char array and copy over the string
  size_ = uint32_t (size);
  unsigned char * temp = new unsigned char [size_];
  memcpy (temp, new_value, size_);
  file_value_ = temp;
}

// set the value_ to an integer
inline void
madara::knowledge::KnowledgeRecord::set_value (const Integer & new_value)
{
  if (type_ & ALL_CLEARABLES)
    clear_value ();

  type_ = INTEGER;
  status_ = MODIFIED;

  size_ = 1;
  int_value_ = new_value;
}

// set the value_ to an array of doubles
inline void
madara::knowledge::KnowledgeRecord::set_value (const Integer * new_value, uint32_t size)
{
  if (type_ & ALL_CLEARABLES)
    clear_value ();

  type_ = INTEGER_ARRAY;
  status_ = MODIFIED;

  size_ = size;
  Integer * temp_array = new Integer [size];

  for (uint32_t i = 0; i < size; ++i)
    temp_array[i] = new_value[i];

  int_array_ = temp_array;
}

// set the value_ to an array of integers
inline void
madara::knowledge::KnowledgeRecord::set_value (const std::vector <Integer> & new_value)
{
  if (type_ & ALL_CLEARABLES)
    clear_value ();

  type_ = INTEGER_ARRAY;
  status_ = MODIFIED;

  size_ = uint32_t (new_value.size ());
  Integer * temp_array = new Integer [new_value.size ()];

  for (unsigned int i = 0; i < new_value.size (); ++i)
    temp_array[i] = new_value[i];

  int_array_ = temp_array;
}

// set the value_ to a double
inline void
madara::knowledge::KnowledgeRecord::set_value (const double & new_value)
{
  if (type_ & ALL_CLEARABLES)
    clear_value ();

  type_ = DOUBLE;
  status_ = MODIFIED;

  size_ = 1;
  double_value_ = new_value;
}

// set the value_ to an array of doubles
inline void
madara::knowledge::KnowledgeRecord::set_value (const double * new_value, uint32_t size)
{
  if (type_ & ALL_CLEARABLES)
    clear_value ();

  type_ = DOUBLE_ARRAY;
  status_ = MODIFIED;

  size_ = size;
  double * temp_array = new double [size];

  for (uint32_t i = 0; i < size; ++i)
    temp_array[i] = new_value[i];

  double_array_ = temp_array;
}

// set the value_ to an array of doubles
inline void
madara::knowledge::KnowledgeRecord::set_value (const std::vector <double> & new_value)
{
  if (type_ & ALL_CLEARABLES)
    clear_value ();

  type_ = DOUBLE_ARRAY;
  status_ = MODIFIED;

  size_ = uint32_t (new_value.size ());
  double * temp_array = new double [new_value.size ()];

  for (unsigned int i = 0; i < new_value.size (); ++i)
    temp_array[i] = new_value[i];

  double_array_ = temp_array;
}

inline
madara::knowledge::KnowledgeRecord::operator bool_type (void) const
{
  return is_true() ?
    &KnowledgeRecord::KnowledgeRecord_does_not_implicitly_cast_to_bool :
    0;
}

inline char *
madara::knowledge::KnowledgeRecord::write (char * buffer,
  int64_t & buffer_remaining) const
{
  // format is [type | value_size | value]

  char * size_location = 0;
  uint32_t size_intermediate = 0;
  uint32_t uint32_temp;
  Integer integer_temp;
  double double_temp;

  int64_t encoded_size = get_encoded_size ();

  if (buffer_remaining >= encoded_size)
  {
    madara_logger_ptr_log (logger_, logger::LOG_MINOR, "KnowledgeRecord::write:" \
      " encoding %" PRId64 " byte message\n", encoded_size);

    // Remove the type of value from the buffer
    if (buffer_remaining >= (int64_t) sizeof (type_))
    {
      uint32_temp = madara::utility::endian_swap (type_);
      memcpy (buffer, &uint32_temp, sizeof (uint32_temp));
      buffer += sizeof (type_);
    }
    buffer_remaining -= sizeof (type_);

    // Remove the size of value from the buffer
    if (buffer_remaining >= (int64_t) sizeof (size_))
    {
      // set a pointer to size, in case we need to modify it during
      // value copy (e.g. during double conversion)
      size_location = buffer;
      size_intermediate = size_;

      uint32_temp = madara::utility::endian_swap (size_);
      memcpy (buffer, &uint32_temp, sizeof (uint32_temp));

      // note that we do not encode the size yet because it may change
      // and we need the architectural-specific version for other checks

      buffer += sizeof (size_);
    }
    buffer_remaining -= sizeof (size_);

    // Remove the value from the buffer
    if (is_string_type ())
    {
      // strings do not have to be converted
      if (buffer_remaining >= int64_t (size_))
      {
        memcpy (buffer, str_value_.get_ptr (), size_);
      }
    }
    else if (type_ == INTEGER)
    {
      if (buffer_remaining >= int64_t (size_ * sizeof (Integer)))
      {
        integer_temp = madara::utility::endian_swap (int_value_);
        memcpy (buffer, &integer_temp, sizeof (integer_temp));

        size_intermediate = size_ * sizeof (Integer);
      }
    }
    else if (type_ == INTEGER_ARRAY)
    {
      if (buffer_remaining >= int64_t (size_ * sizeof (Integer)))
      {
        // convert integers to network byte order
        const Integer * ptr_temp = int_array_.get_ptr ();
        Integer * target_buffer = (Integer *)buffer;

        for (uint32_t i = 0; i < size_; ++i, ++ptr_temp, ++target_buffer)
        {
          integer_temp = madara::utility::endian_swap (*ptr_temp);
          memcpy (target_buffer, &integer_temp, sizeof (Integer));
        }

        size_intermediate = size_ * sizeof (Integer);
      }
    }
    else if (type_ == DOUBLE)
    {
      if (buffer_remaining >= int64_t (size_ * sizeof (double)))
      {
        double_temp = madara::utility::endian_swap (double_value_);
        memcpy (buffer, &double_temp, sizeof (double));

        size_intermediate = size_ * sizeof (double);
      }
    }
    else if (type_ == DOUBLE_ARRAY)
    {
      if (buffer_remaining >= int64_t (size_ * sizeof (double)))
      {
        // convert integers to network byte order
        const double * ptr_temp = double_array_.get_ptr ();
        double * target_buffer = (double *)buffer;

        for (uint32_t i = 0; i < size_; ++i, ++ptr_temp, ++target_buffer)
        {
          double_temp = madara::utility::endian_swap (*ptr_temp);
          memcpy (target_buffer, &double_temp, sizeof (double_temp));
        }

        size_intermediate = size_ * sizeof (double);

        /**
        * note that we once converted doubles into strings to attempt
        * portability, but we are now just assuming that the floating
        * point units in the architecture are the same endianness as
        * the integers. This is true of ARM, Intel/AMD, and most architectures.
        * We are essentially no longer supporting an architecture that
        * mixes and matches. Persons using such architectures should
        * perform their own conversions on the knowledge records before
        * using them.
        **/
      }
    }
    else if (is_binary_file_type ())
    {
      // strings do not have to be converted
      if (buffer_remaining >= size_)
      {
        memcpy (buffer, file_value_.get_ptr (), size_);
      }
    }

    if (size_location)
    {
      buffer_remaining -= size_intermediate;
      buffer += size_intermediate;
    }
  }
  else
  {
    madara_logger_ptr_log (logger_, logger::LOG_MINOR, "KnowledgeRecord::write:" \
      " %" PRId64 " byte buffer cannot contain %" PRId64 " byte message\n",
      buffer_remaining, encoded_size);
  }
  return buffer;
}

inline char *
madara::knowledge::KnowledgeRecord::write (char * buffer, const std::string & key,
int64_t & buffer_remaining) const

{
  // format is [key_size | key | type | value_size | value]

  uint32_t key_size = uint32_t (key.size () + 1);
  uint32_t uint32_temp;

  int64_t encoded_size = get_encoded_size (key);

  if (buffer_remaining >= encoded_size)
  {
    madara_logger_ptr_log (logger_, logger::LOG_MINOR, "KnowledgeRecord::write:" \
      " encoding %" PRId64 " byte message\n", encoded_size);

    // Remove the key size from the buffer
    if (buffer_remaining >= (int64_t) sizeof (key_size))
    {
      uint32_temp = madara::utility::endian_swap (key_size);
      memcpy (buffer, &uint32_temp, sizeof (uint32_temp));
      buffer += sizeof (key_size);
    }
    buffer_remaining -= sizeof (key_size);

    // Remove the key from the buffer
    if (buffer_remaining >= (int64_t) sizeof (char) * key_size)
    {
      // copy the string and set null terminator in buffer
      strncpy (buffer, key.c_str (), key_size - 1);
      buffer[key_size - 1] = 0;

      buffer += sizeof (char) * key_size;
    }
    buffer_remaining -= sizeof (char) * key_size;

    // write the type and value of the record
    buffer = write (buffer, buffer_remaining);
  }
  else
  {
    madara_logger_ptr_log (logger_, logger::LOG_MINOR, "KnowledgeRecord::write:" \
      " %" PRId64 " byte buffer cannot contain %" PRId64 " byte message\n",
      buffer_remaining, encoded_size);
  }
  return buffer;
}

inline char *
madara::knowledge::KnowledgeRecord::write (char * buffer, uint32_t key_id,
  int64_t & buffer_remaining) const
{
  // format is [key_id | type | value_size | value]

  uint32_t uint32_temp;

  int64_t encoded_size = get_encoded_size () + sizeof (key_id);

  if (buffer_remaining >= encoded_size)
  {
    madara_logger_ptr_log (logger_, logger::LOG_MINOR, "KnowledgeRecord::write:" \
      " encoding %" PRId64 " byte message\n", encoded_size);

    // write the key id to the buffer
    if (buffer_remaining >= (int64_t)sizeof (key_id))
    {
      uint32_temp = madara::utility::endian_swap (key_id);
      memcpy (buffer, &uint32_temp, sizeof (uint32_temp));
      buffer += sizeof (key_id);
    }
    buffer_remaining -= sizeof (key_id);

    // write the type and value of the record
    buffer = write (buffer, buffer_remaining);
  }
  else
  {
    madara_logger_ptr_log (logger_, logger::LOG_MINOR, "KnowledgeRecord::write:" \
      " %" PRId64 " byte buffer cannot contain %" PRId64 " byte message\n",
      buffer_remaining, encoded_size);
  }
  return buffer;
}




#endif  // _KNOWLEDGE_RECORD_INL_
