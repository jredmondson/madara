

#ifndef _KNOWLEDGE_RECORD_INL_
#define _KNOWLEDGE_RECORD_INL_

#include <algorithm>
#include <iostream>

#include "madara/utility/Utility.h"

#define MADARA_WITH_DELETER(val) \
  {(val), std::default_delete<typename std::decay<decltype(*val)>::type []>()}

/**
* @file KnowledgeRecord.inl
* @author James Edmondson <jedmondson@gmail.com>
*
* This file contains the methods of the knowledge::KnowledgeRecord class
**/

namespace madara { namespace knowledge {

inline
KnowledgeRecord::KnowledgeRecord (logger::Logger & logger)
: logger_ (&logger), type_ (INTEGER), int_value_ (0)
{
}

inline KnowledgeRecord::KnowledgeRecord (Integer value,
  logger::Logger & logger)
: logger_ (&logger), status_ (MODIFIED), type_ (INTEGER),
  int_value_ (value)
{
}

inline KnowledgeRecord::KnowledgeRecord (
  const std::vector <Integer> & value,
  logger::Logger & logger)
: logger_ (&logger)
{
  set_value (value);
}

inline KnowledgeRecord::KnowledgeRecord (double value,
  logger::Logger & logger)
: logger_ (&logger), status_ (MODIFIED), type_ (DOUBLE),
  double_value_ (value)
{
}

inline KnowledgeRecord::KnowledgeRecord (
  const std::vector <double> & value,
  logger::Logger & logger)
: logger_ (&logger)
{
  set_value (value);
}

inline KnowledgeRecord::KnowledgeRecord (const std::string & value,
  logger::Logger & logger)
: logger_ (&logger)
{
  set_value (value);
}

inline KnowledgeRecord::KnowledgeRecord (const char * value,
  logger::Logger & logger)
: logger_ (&logger)
{
  set_value (std::string (value));
}

inline KnowledgeRecord::KnowledgeRecord (
    const knowledge::KnowledgeRecord & rhs) noexcept
: logger_ (rhs.logger_),
  status_ (rhs.status_),
  scope (rhs.scope),
  type_ (rhs.type_),
  quality (rhs.quality),
  clock (rhs.clock),
  write_quality (rhs.write_quality)
{
  if      (rhs.status_ != UNCREATED)
  {
    if      (rhs.type_ == INTEGER)
      int_value_ = rhs.int_value_;
    else if (rhs.type_ == INTEGER_ARRAY)
      new (&int_array_) std::shared_ptr<std::vector<Integer>>(rhs.int_array_);
    else if (rhs.type_ == DOUBLE)
      double_value_ = rhs.double_value_;
    else if (rhs.type_ == DOUBLE_ARRAY)
      new (&double_array_) std::shared_ptr<std::vector<double>>(rhs.double_array_);
    else if (rhs.is_string_type ())
      new (&str_value_) std::shared_ptr<std::string>(rhs.str_value_);
    else if (rhs.is_binary_file_type ())
      new (&file_value_) std::shared_ptr<std::vector<unsigned char>>(rhs.file_value_);
  }
}

inline KnowledgeRecord::KnowledgeRecord (
    knowledge::KnowledgeRecord &&rhs) noexcept
: logger_ (std::move(rhs.logger_)),
  status_ (rhs.status_),
  scope (rhs.scope),
  type_ (rhs.type_),
  quality (rhs.quality),
  clock (rhs.clock),
  write_quality (rhs.write_quality)
{
  if      (rhs.status_ != UNCREATED)
  {
    if      (rhs.type_ == INTEGER)
      int_value_ = rhs.int_value_;
    else if (rhs.type_ == INTEGER_ARRAY)
      new (&int_array_) std::shared_ptr<std::vector<Integer>>(std::move(rhs.int_array_));
    else if (rhs.type_ == DOUBLE)
      double_value_ = rhs.double_value_;
    else if (rhs.type_ == DOUBLE_ARRAY)
      new (&double_array_) std::shared_ptr<std::vector<double>>(std::move(rhs.double_array_));
    else if (rhs.is_string_type ())
      new (&str_value_) std::shared_ptr<std::string>(std::move(rhs.str_value_));
    else if (rhs.is_binary_file_type ())
      new (&file_value_) std::shared_ptr<std::vector<unsigned char>>(std::move(rhs.file_value_));
  }
}

inline KnowledgeRecord::~KnowledgeRecord () noexcept
{
  clear_union();
}

inline KnowledgeRecord &
KnowledgeRecord::operator= (const knowledge::KnowledgeRecord & rhs) noexcept
{
  if (this == &rhs)
    return *this;

  // clear any dynamic memory being used on the left hand side
  clear_value ();

  if (rhs.status_ != UNCREATED)
  {
    // set the instance properties accordingly
    clock = rhs.clock;
    quality = rhs.quality;
    scope = rhs.scope;
    status_ = rhs.status_;
    type_ = rhs.type_;

    if      (rhs.type_ == INTEGER)
      int_value_ = rhs.int_value_;
    else if (rhs.type_ == INTEGER_ARRAY)
      new (&int_array_) std::shared_ptr<std::vector<Integer>>(rhs.int_array_);
    else if (rhs.type_ == DOUBLE)
      double_value_ = rhs.double_value_;
    else if (rhs.type_ == DOUBLE_ARRAY)
      new (&double_array_) std::shared_ptr<std::vector<double>>(rhs.double_array_);
    else if (rhs.is_string_type ())
      new (&str_value_) std::shared_ptr<std::string>(rhs.str_value_);
    else if (rhs.is_binary_file_type ())
      new (&file_value_) std::shared_ptr<std::vector<unsigned char>>(rhs.file_value_);
  }

  return *this;
}

inline KnowledgeRecord &
KnowledgeRecord::operator= (knowledge::KnowledgeRecord && rhs) noexcept
{
  if (this == &rhs)
    return *this;

  // clear any dynamic memory being used on the left hand side
  clear_union();

  if      (rhs.status_ != UNCREATED)
  {
    // set the instance properties accordingly
    clock = rhs.clock;
    quality = rhs.quality;
    scope = rhs.scope;
    status_ = rhs.status_;
    type_ = rhs.type_;

    if      (rhs.type_ == INTEGER)
      int_value_ = rhs.int_value_;
    else if (rhs.type_ == INTEGER_ARRAY)
      new (&int_array_) std::shared_ptr<std::vector<Integer>>(std::move(rhs.int_array_));
    else if (rhs.type_ == DOUBLE)
      double_value_ = rhs.double_value_;
    else if (rhs.type_ == DOUBLE_ARRAY)
      new (&double_array_) std::shared_ptr<std::vector<double>>(std::move(rhs.double_array_));
    else if (rhs.is_string_type ())
      new (&str_value_) std::shared_ptr<std::string>(std::move(rhs.str_value_));
    else if (rhs.is_binary_file_type ())
      new (&file_value_) std::shared_ptr<std::vector<unsigned char>>(std::move(rhs.file_value_));
  }

  return *this;
}

inline bool
KnowledgeRecord::operator== (double value) const
{
  // for this type of comparison, we can only be equal if we are the same
  // base type
  if (is_integer_type ())
  {
    return to_integer () == value;
  }
  else if (is_double_type () || is_string_type ())
  {
    return to_double () == value;
  }
  
  return false;
}

inline bool
KnowledgeRecord::operator== (
  const std::string & value) const
{
  return to_string () == value;
}

inline bool
KnowledgeRecord::operator== (const char * value) const
{
  return to_string () == std::string (value);
}

inline bool
KnowledgeRecord::operator!= (const knowledge::KnowledgeRecord & rhs) const
{
  return !(*this == rhs);
}

/**
  * Logical not.
  **/
inline bool
KnowledgeRecord::operator! (void) const
{
  return !is_true ();
}

/**
  * Negate.
  **/
inline KnowledgeRecord
  KnowledgeRecord::operator- (void) const
{
  KnowledgeRecord record (*this);

  if (is_integer_type ())
    record.set_value (-to_integer ());
  else if (is_double_type ())
    record.set_value (-to_double ());

  return record;
}

inline bool
KnowledgeRecord::operator== (Integer value) const
{
  // for this type of comparison, we can only be equal if we are the same
  // base type
  if (is_integer_type ())
  {
    return to_integer () == value;
  }
  else if (is_double_type () || is_string_type ())
  {
    return to_double () == value;
  }
  
  return false;
}

/**
  * In-place addition operator
  **/
inline KnowledgeRecord &
KnowledgeRecord::operator+= (const knowledge::KnowledgeRecord & rhs)
{
  if (is_integer_type ())
  {
    if (rhs.is_integer_type ())
      set_value (to_integer () + rhs.to_integer ());
    else
      set_value (to_integer () + rhs.to_double ());
  }
  else if (is_double_type ())
    set_value (to_double () + rhs.to_double ());

  else if (is_string_type ())
    set_value (to_string () + rhs.to_string ());

  return *this;
}

/**
  * In-place predecrement
  **/
inline KnowledgeRecord &
KnowledgeRecord::operator-- (void)
{
  if (is_integer_type ())
    set_value (to_integer () - 1);

  else if (is_double_type ())
    set_value (to_double () - 1);

  return *this;
}

/**
  * In-place preincrement
  **/
inline KnowledgeRecord &
KnowledgeRecord::operator++ (void)
{
  if (is_integer_type ())
    set_value (to_integer () + 1);

  else if (is_double_type ())
    set_value (to_double () + 1);


  return *this;
}

/**
  * In-place subtraction operator
  **/
inline KnowledgeRecord &
KnowledgeRecord::operator-= (const knowledge::KnowledgeRecord & rhs)
{
  if (is_integer_type ())
  {
    if (rhs.is_integer_type ())
      set_value (to_integer () - rhs.to_integer ());
    else
      set_value (to_integer () - rhs.to_double ());
  }
  else if (is_double_type () || is_string_type ())
    set_value (to_double () - rhs.to_double ());

  return *this;
}

/**
  * In-place multiplication operator
  **/
inline KnowledgeRecord &
KnowledgeRecord::operator*= (const knowledge::KnowledgeRecord & rhs)
{
  if (is_integer_type ())
  {
    if (rhs.is_integer_type ())
      set_value (to_integer () * rhs.to_integer ());
    else
      set_value (to_integer () * rhs.to_double ());
  }
  else if (is_double_type () || is_string_type ())
    set_value (to_double () * rhs.to_double ());

  return *this;
}

/**
  * In-place division operator
  **/
inline KnowledgeRecord &
KnowledgeRecord::operator/= (const knowledge::KnowledgeRecord & rhs)
{
  if (is_integer_type ())
  {
    if (rhs.is_integer_type ())
    {
      Integer denom = rhs.to_integer ();
      if (denom == 0)
        set_value ("Division by Zero");
      else
        set_value (to_integer () / denom);
    }
    else
    {
      double denom = rhs.to_double ();

      if (denom == 0)
        set_value ("Division by Zero");
      else
        set_value (to_integer () / denom);
    }
  }
  else if (is_double_type () || is_string_type ())
  {
    double denom = rhs.to_double ();

    if (denom == 0)
      set_value ("Division by Zero");
    else
      set_value (to_double () / denom);
  }

  return *this;
}
   
/**
  * In-place modulus operator
  **/
inline KnowledgeRecord &
KnowledgeRecord::operator%= (const knowledge::KnowledgeRecord & rhs)
{
  if (is_integer_type ())
  {
    if (rhs.is_integer_type ())
    {
      Integer denom = rhs.to_integer ();
      if (denom == 0)
        set_value ("Division by Zero");
      else
        set_value (to_integer () % denom);
    }
  }

  return *this;
}
 
/**
  * Times operator
  **/
inline KnowledgeRecord
KnowledgeRecord::operator* (const knowledge::KnowledgeRecord & rhs) const
{
  // copy this value to a local copy
  knowledge::KnowledgeRecord ret_value (*this);

  return ret_value *= rhs;
}

/**
  * Divides operator
  **/
inline KnowledgeRecord
KnowledgeRecord::operator/ (const knowledge::KnowledgeRecord & rhs) const
{
  // copy this value to a local copy
  knowledge::KnowledgeRecord ret_value (*this);

  return ret_value /= rhs;
}

/**
  * Modulus operator
  **/
inline KnowledgeRecord
KnowledgeRecord::operator% (const knowledge::KnowledgeRecord & rhs) const
{
  // copy this value to a local copy
  knowledge::KnowledgeRecord ret_value (*this);

  return ret_value %= rhs;
}

/**
  * Plus operator
  **/
inline KnowledgeRecord
KnowledgeRecord::operator+ (const knowledge::KnowledgeRecord & rhs) const
{
  // copy this value to a local copy
  knowledge::KnowledgeRecord ret_value (*this);

  return ret_value += rhs;
}

inline bool
KnowledgeRecord::is_false (void) const
{
  return !is_true ();
}

/**
  * output stream buffering
  **/
inline std::ostream & operator<< (std::ostream & stream,
  const KnowledgeRecord & rhs)
{
  if (rhs.type () &
        (KnowledgeRecord::INTEGER_ARRAY | 
         KnowledgeRecord::DOUBLE_ARRAY))
  {
    stream << rhs.to_string (", ");
  }
  else
    stream << rhs.to_string ();

  return stream;
}

/**
  * Minus operator
  **/
inline KnowledgeRecord
KnowledgeRecord::operator- (const knowledge::KnowledgeRecord & rhs) const
{
  // copy this value to a local copy
  knowledge::KnowledgeRecord ret_value (*this);

  return ret_value -= rhs;
}

inline KnowledgeRecord
KnowledgeRecord::deep_copy () const
{
  KnowledgeRecord ret;
  ret.deep_copy(*this);
  return ret;
}

inline bool
KnowledgeRecord::exists (void) const
{
  return status_ != UNCREATED;
}

inline int
KnowledgeRecord::status (void) const
{
  return status_;
}

/**
* sets the status to modified
**/
inline void
KnowledgeRecord::set_modified (void)
{
  status_ = MODIFIED;
}

inline uint32_t
KnowledgeRecord::size (void) const
{
  if (status_ == UNINITIALIZED) {
    return 0;
  }
  if (type_ == INTEGER || type_ == DOUBLE) {
    return 1;
  } else if (is_string_type()) {
    return str_value_->size () + 1;
  } else if (is_binary_file_type()) {
    return file_value_->size ();
  } else if (type_ == INTEGER_ARRAY) {
    return int_array_->size ();
  } else if (type_ == DOUBLE_ARRAY) {
    return double_array_->size ();
  }
  return 0;
}

inline int32_t
KnowledgeRecord::type (void) const
{
  return type_;
}

inline int64_t
KnowledgeRecord::get_encoded_size (void) const
{
  int64_t buffer_size (sizeof (uint32_t) * 2);
  if (type_ & (INTEGER | INTEGER_ARRAY))
  {
    buffer_size += sizeof (int_value_) * size ();
  }
  else if (type_ & (DOUBLE | DOUBLE_ARRAY))
  {
    buffer_size += sizeof (double) * size ();
  }
  else
  {
    buffer_size += size ();
  }

  return buffer_size;
}

inline int64_t
KnowledgeRecord::get_encoded_size (const std::string & key) const
{
  // for keyed size, add another uint32_t and the size of the key with a null char
  int64_t buffer_size (sizeof (uint32_t) * 1);
  buffer_size += (key.size () + 1);

  // and then add the default encoded size
  buffer_size += get_encoded_size ();

  return buffer_size;
}

inline bool
KnowledgeRecord::is_ref_counted (void) const
{
  return is_ref_counted (type_);
}

inline bool
KnowledgeRecord::is_ref_counted (uint32_t type)
{
  return type != INTEGER && type != DOUBLE;
}

inline bool
KnowledgeRecord::is_string_type (void) const
{
  return is_string_type (type_);
}

inline bool
KnowledgeRecord::is_string_type (uint32_t type)
{
  return type == STRING || type == XML || type == TEXT_FILE;
}

inline bool
KnowledgeRecord::is_double_type (void) const
{
  return is_double_type (type_);
}

inline bool
KnowledgeRecord::is_double_type (uint32_t type)
{
  return type == DOUBLE || type == DOUBLE_ARRAY;
}

inline bool
KnowledgeRecord::is_integer_type (void) const
{
  return is_integer_type (type_);
}


inline bool
KnowledgeRecord::is_integer_type (uint32_t type)
{
  return type == INTEGER || type == INTEGER_ARRAY;
}

inline bool
KnowledgeRecord::is_array_type (void) const
{
  return is_array_type (type_);
}

inline bool
KnowledgeRecord::is_array_type (uint32_t type)
{
  return type & ALL_ARRAYS;
}

inline bool
KnowledgeRecord::is_image_type (void) const
{
  return is_image_type (type_);
}

inline bool
KnowledgeRecord::is_image_type (uint32_t type)
{
  return type == IMAGE_JPEG;
}

inline bool
KnowledgeRecord::is_file_type (void) const
{
  return is_file_type (type_);
}

inline bool
KnowledgeRecord::is_file_type (uint32_t type)
{
  return type == TEXT_FILE || type == XML ||
         type == IMAGE_JPEG || type == UNKNOWN_FILE_TYPE;
}

inline bool
KnowledgeRecord::is_binary_file_type (void) const
{
  return is_binary_file_type (type_);
}

inline bool
KnowledgeRecord::is_binary_file_type (uint32_t type)
{
  return type == IMAGE_JPEG || type == UNKNOWN_FILE_TYPE;
}

inline uint32_t
max_quality (const knowledge::KnowledgeRecords & records)
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
max_quality (const KnowledgeMap & records)
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

template<typename T>
inline void destruct(T &x) {
  x.~T();
}

inline void
KnowledgeRecord::clear_union (void) noexcept
{
  if (status_ != UNCREATED)
  {
    if (type_ & ALL_CLEARABLES)
    {
      if (type_ == INTEGER_ARRAY)
        destruct(int_array_);
      else if (type_ == DOUBLE_ARRAY)
        destruct(double_array_);
      else if (is_string_type ())
        destruct(str_value_);
      else if (is_binary_file_type ())
        destruct(file_value_);
    }
  }
}

inline void
KnowledgeRecord::clear_value (void) noexcept
{
  clear_union ();
  if (status_ != UNCREATED)
  {
    int_value_ = 0;

    type_ = INTEGER;
    status_ = UNCREATED;
  }
}

inline const char *
KnowledgeRecord::read (const char * buffer,
  int64_t & buffer_remaining)
{
  // format is [key_size | key | type | value_size | value]

  uint32_t buff_value_size (0);
  uint32_t type = INTEGER;
  uint32_t size = 1;

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

    if (is_integer_type (type))
      buff_value_size = size * sizeof (Integer);
    else if (is_double_type (type))
      buff_value_size = size * sizeof (double);
    else
      buff_value_size = size;

    buffer += sizeof (buff_value_size);
  } else {
    buffer_remaining = -1;
    return buffer;
  }
  buffer_remaining -= sizeof (buff_value_size);

  // Remove the value from the buffer
  if (buffer_remaining >= int64_t (buff_value_size))
  {
    if (is_string_type (type))
    {
      str_value() = std::make_shared<std::string> (buffer, buff_value_size);
    }

    else if (type == INTEGER)
    {
      Integer tmp;
      memcpy (&tmp, buffer, sizeof (tmp));
      int_value() = madara::utility::endian_swap (tmp);
    }

    else if (type == INTEGER_ARRAY)
    {
      std::vector<Integer> tmp;
      tmp.reserve(size);

      for (uint32_t i = 0; i < size; ++i)
      {
        Integer cur;
        memcpy (&cur, buffer + i * sizeof(cur), sizeof(cur));
        tmp.emplace_back(madara::utility::endian_swap (cur));
      }

      make_int_array (std::move(tmp));
    }

    else if (type == DOUBLE)
    {
      double tmp;
      memcpy (&tmp, buffer, sizeof (tmp));
      double_value() = madara::utility::endian_swap (tmp);
    }

    else if (type == DOUBLE_ARRAY)
    {
      std::vector<double> tmp;
      tmp.reserve(size);

      for (uint32_t i = 0; i < size; ++i)
      {
        double cur;
        memcpy (&cur, buffer + i * sizeof(cur), sizeof(cur));
        tmp.emplace_back(madara::utility::endian_swap (cur));
      }

      make_double_array (std::move(tmp));
    }

    else if (is_binary_file_type (type))
    {
      const unsigned char *b = (const unsigned char *)buffer;
      file_value() = std::make_shared<std::vector<unsigned char>> (b, b + size);
    }

    else {
      buffer_remaining = -1;
      return buffer;
    }

    buffer += buff_value_size;
    buffer_remaining -= sizeof (char) * buff_value_size;

    type_ = type;
    status_ = MODIFIED;
  }

  return buffer;
}

inline const char *
KnowledgeRecord::read (const char * buffer, std::string & key,
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
KnowledgeRecord::read (const char * buffer, uint32_t & key_id,
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
KnowledgeRecord::reset_value (void) noexcept
{
  clear_value ();

  quality = 0;
  write_quality = 0;
  clock = 0;
}

// set the value_ to a string
inline void
KnowledgeRecord::set_value (const std::string & new_value)
{
  str_value() = std::make_shared<std::string> (new_value);
  type_ = STRING;
  status_ = MODIFIED;
}

// set the value_ to a string
inline void
KnowledgeRecord::set_xml (const char * new_value, size_t size)
{
  str_value() = std::make_shared<std::string> (new_value, size);
  type_ = XML;
  status_ = MODIFIED;
}

// set the value_ to a string
inline void
KnowledgeRecord::set_text (const char * new_value, size_t size)
{
  str_value() = std::make_shared<std::string> (new_value, size);
  type_ = TEXT_FILE;
  status_ = MODIFIED;
}

// set the value_ to a string
inline void
KnowledgeRecord::set_jpeg (const unsigned char * new_value,
                                    size_t size)
{
  file_value() = std::make_shared<std::vector<unsigned char>> (new_value, new_value + size);
  type_ = IMAGE_JPEG;
  status_ = MODIFIED;
}

// set the value_ to a string
inline void
KnowledgeRecord::set_file (const unsigned char * new_value,
                                    size_t size)
{
  file_value() = std::make_shared<std::vector<unsigned char>> (new_value, new_value + size);
  type_ = UNKNOWN_FILE_TYPE;
  status_ = MODIFIED;
}

// set the value_ to an integer
inline void
KnowledgeRecord::set_value (Integer new_value)
{
  int_value() = new_value;
  status_ = MODIFIED;
}

// set the value_ to an array of doubles
inline void
KnowledgeRecord::set_value (const Integer * new_value, uint32_t size)
{
  make_int_array (new_value, new_value + size);
  status_ = MODIFIED;
}

// set the value_ to an array of integers
inline void
KnowledgeRecord::set_value (const std::vector <Integer> & new_value)
{
  make_int_array (new_value);
  status_ = MODIFIED;
}

// set the value_ to a double
inline void
KnowledgeRecord::set_value (double new_value)
{
  double_value() = new_value;
  status_ = MODIFIED;
}

// set the value_ to an array of doubles
inline void
KnowledgeRecord::set_value (const double * new_value, uint32_t size)
{
  make_double_array (new_value, new_value + size);
  status_ = MODIFIED;
}

// set the value_ to an array of doubles
inline void
KnowledgeRecord::set_value (const std::vector <double> & new_value)
{
  make_double_array (new_value);
  status_ = MODIFIED;
}

inline
KnowledgeRecord::operator bool_type (void) const
{
  return is_true() ?
    &KnowledgeRecord::KnowledgeRecord_does_not_implicitly_cast_to_bool :
    0;
}

inline char *
KnowledgeRecord::write (char * buffer,
  int64_t & buffer_remaining) const
{
  // format is [type | value_size | value]

  char * size_location = 0;
  uint32_t size_intermediate = 0;
  uint32_t uint32_temp;
  Integer integer_temp;
  double double_temp;
  uint32_t size = this->size ();

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
    if (buffer_remaining >= (int64_t) sizeof (size))
    {
      // set a pointer to size, in case we need to modify it during
      // value copy (e.g. during double conversion)
      size_location = buffer;
      size_intermediate = size;

      uint32_temp = madara::utility::endian_swap (size);
      memcpy (buffer, &uint32_temp, sizeof (uint32_temp));

      // note that we do not encode the size yet because it may change
      // and we need the architectural-specific version for other checks

      buffer += sizeof (size);
    }
    buffer_remaining -= sizeof (size);

    // Remove the value from the buffer
    if (is_string_type ())
    {
      // strings do not have to be converted
      if (buffer_remaining >= int64_t (size))
      {
        memcpy (buffer, &file_value_->at (0), size);
      }
    }
    else if (type_ == INTEGER)
    {
      if (buffer_remaining >= int64_t (size * sizeof (Integer)))
      {
        integer_temp = madara::utility::endian_swap (int_value_);
        memcpy (buffer, &integer_temp, sizeof (integer_temp));

        size_intermediate = size * sizeof (Integer);
      }
    }
    else if (type_ == INTEGER_ARRAY)
    {
      if (buffer_remaining >= int64_t (size * sizeof (Integer)))
      {
        // convert integers to network byte order
        const Integer * ptr_temp = &(*int_array_)[0];
        Integer * target_buffer = (Integer *)buffer;

        for (uint32_t i = 0; i < size; ++i, ++ptr_temp, ++target_buffer)
        {
          integer_temp = madara::utility::endian_swap (*ptr_temp);
          memcpy (target_buffer, &integer_temp, sizeof (Integer));
        }

        size_intermediate = size * sizeof (Integer);
      }
    }
    else if (type_ == DOUBLE)
    {
      if (buffer_remaining >= int64_t (size * sizeof (double)))
      {
        double_temp = madara::utility::endian_swap (double_value_);
        memcpy (buffer, &double_temp, sizeof (double));

        size_intermediate = size * sizeof (double);
      }
    }
    else if (type_ == DOUBLE_ARRAY)
    {
      if (buffer_remaining >= int64_t (size * sizeof (double)))
      {
        // convert integers to network byte order
        const double * ptr_temp = &(*double_array_)[0];
        double * target_buffer = (double *)buffer;

        for (uint32_t i = 0; i < size; ++i, ++ptr_temp, ++target_buffer)
        {
          double_temp = madara::utility::endian_swap (*ptr_temp);
          memcpy (target_buffer, &double_temp, sizeof (double_temp));
        }

        size_intermediate = size * sizeof (double);

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
      if (buffer_remaining >= size)
      {
        memcpy (buffer, &(*file_value_)[0], size);
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
KnowledgeRecord::write (char * buffer, const std::string & key,
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
KnowledgeRecord::write (char * buffer, uint32_t key_id,
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

} }

#endif  // _KNOWLEDGE_RECORD_INL_
