

#ifndef _KNOWLEDGE_RECORD_INL_
#define _KNOWLEDGE_RECORD_INL_

#include <algorithm>
#include <iostream>
#include <sstream>
#include <math.h>

#include "madara/utility/Utility.h"
#include "madara/exceptions/MemoryException.h"

/**
* @file KnowledgeRecord.inl
* @author James Edmondson <jedmondson@gmail.com>
*
* This file contains the methods of the knowledge::KnowledgeRecord class
**/

namespace madara { namespace knowledge {

inline
KnowledgeRecord::KnowledgeRecord (logger::Logger & logger) noexcept
: logger_ (&logger)
{
}

template<typename T,
  typename std::enable_if<std::is_integral<T>::value, void*>::type>
inline KnowledgeRecord::KnowledgeRecord (T value,
  logger::Logger & logger) noexcept
: logger_ (&logger), int_value_ ((Integer)value), type_ (INTEGER)
{
}

inline KnowledgeRecord::KnowledgeRecord (
  const std::vector <Integer> & value, logger::Logger & logger)
: logger_ (&logger)
{
  set_value (value);
}

inline KnowledgeRecord::KnowledgeRecord (
  std::vector <Integer> && value, logger::Logger & logger) noexcept
: logger_ (&logger)
{
  set_value (std::move(value));
}

inline KnowledgeRecord::KnowledgeRecord (
    std::shared_ptr<std::vector<Integer>> value,
    logger::Logger & logger) noexcept
: logger_ (&logger)
{
  set_value (std::move(value));
}

template<typename T,
  typename std::enable_if<std::is_floating_point<T>::value, void*>::type>
inline KnowledgeRecord::KnowledgeRecord (T value,
  logger::Logger & logger) noexcept
: logger_ (&logger), double_value_ ((double)value), type_ (DOUBLE)
{
}

inline KnowledgeRecord::KnowledgeRecord (
  const std::vector <double> & value,
  logger::Logger & logger)
: logger_ (&logger)
{
  set_value (value);
}

inline KnowledgeRecord::KnowledgeRecord (
  std::vector <double> && value,
  logger::Logger & logger) noexcept
: logger_ (&logger)
{
  set_value (std::move(value));
}

inline KnowledgeRecord::KnowledgeRecord (
    std::shared_ptr<std::vector<double>> value,
    logger::Logger & logger) noexcept
: logger_ (&logger)
{
  set_value (std::move(value));
}

inline KnowledgeRecord::KnowledgeRecord (const std::string & value,
  logger::Logger & logger)
: logger_ (&logger)
{
  set_value (value);
}

inline KnowledgeRecord::KnowledgeRecord (std::string && value,
  logger::Logger & logger) noexcept
: logger_ (&logger)
{
  set_value (std::move(value));
}

inline KnowledgeRecord::KnowledgeRecord (
    std::shared_ptr<std::string> value,
    logger::Logger & logger) noexcept
: logger_ (&logger)
{
  set_value (std::move(value));
}

inline KnowledgeRecord::KnowledgeRecord (const char * value,
  logger::Logger & logger)
: logger_ (&logger)
{
  set_value (std::string (value));
}

inline KnowledgeRecord::KnowledgeRecord (
    std::shared_ptr<std::vector<unsigned char>> value,
    logger::Logger & logger) noexcept
: logger_ (&logger)
{
  set_file (std::move(value));
}

inline KnowledgeRecord::KnowledgeRecord (
    const knowledge::KnowledgeRecord & rhs)
: logger_ (rhs.logger_),
  clock (rhs.clock),
  quality (rhs.quality),
  write_quality (rhs.write_quality),
  type_ (rhs.type_),
  shared_ (rhs.is_ref_counted() ? SHARED : OWNED)
{
  if      (rhs.type_ == EMPTY)
    return;

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

inline KnowledgeRecord::KnowledgeRecord (
    knowledge::KnowledgeRecord &&rhs) noexcept
: logger_ (std::move(rhs.logger_)),
  clock (rhs.clock),
  quality (rhs.quality),
  write_quality (rhs.write_quality),
  type_ (rhs.type_),
  shared_ (rhs.shared_)
{
  if      (rhs.type_ == EMPTY)
    return;

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

  rhs.type_ = EMPTY;
}

inline KnowledgeRecord::~KnowledgeRecord () noexcept
{
  clear_union();
}

inline KnowledgeRecord &
KnowledgeRecord::operator= (const knowledge::KnowledgeRecord & rhs)
{
  if (this == &rhs)
    return *this;

  // clear any dynamic memory being used on the left hand side
  clear_value ();

  if (rhs.type_ == EMPTY)
    return *this;

  // set the instance properties accordingly
  clock = rhs.clock;
  quality = rhs.quality;
  write_quality = rhs.write_quality;
  type_ = rhs.type_;
  shared_ = is_ref_counted() ? SHARED : OWNED;

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

  return *this;
}

inline KnowledgeRecord &
KnowledgeRecord::operator= (knowledge::KnowledgeRecord && rhs) noexcept
{
  if (this == &rhs)
    return *this;

  // clear any dynamic memory being used on the left hand side
  clear_value();

  if (rhs.type_ == EMPTY)
    return *this;

  // set the instance properties accordingly
  clock = rhs.clock;
  quality = rhs.quality;
  write_quality = rhs.write_quality;
  type_ = rhs.type_;
  shared_ = rhs.shared_;

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

  rhs.type_ = EMPTY;

  return *this;
}

#if 0
template<typename T,
  typename std::enable_if<std::is_integral<T>::value, void*>::type>
inline bool
KnowledgeRecord::operator== (T value) const
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

template<typename T,
  typename std::enable_if<std::is_floating_point<T>::value, void*>::type>
inline bool
KnowledgeRecord::operator== (T value) const
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
  return to_string ().compare (value) == 0;
}
#endif

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

  if (type_ == INTEGER)
  {
    record.set_value (-int_value_);
  }
  else if (type_ == DOUBLE)
  {
    record.set_value (-double_value_);
  }

  return record;
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
        set_value (NAN);
      else
        set_value (to_integer () / denom);
    }
    else
    {
      double denom = rhs.to_double ();

      if (denom == 0)
        set_value (NAN);
      else
        set_value (to_integer () / denom);
    }
  }
  else if (is_double_type () || is_string_type ())
  {
    double denom = rhs.to_double ();

    if (denom == 0)
      set_value (NAN);
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
        set_value (NAN);
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

inline void
KnowledgeRecord::unshare (void)
{
  if (shared_ != SHARED) {
    return;
  }

  if (is_ref_counted ())
  {
    if (is_string_type()) {
      emplace_string (*str_value_);
    } else if (is_binary_file_type()) {
      emplace_file (*file_value_);
    } else if (type_ == INTEGER_ARRAY) {
      emplace_integers (*int_array_);
    } else if (type_ == DOUBLE_ARRAY) {
      emplace_doubles (*double_array_);
    }
  }
  shared_ = OWNED;
}

inline KnowledgeRecord *
KnowledgeRecord::clone (void) const
{
  knowledge::KnowledgeRecord *result = new knowledge::KnowledgeRecord (*this);

  result->unshare();

  return result;
}

inline void
KnowledgeRecord::deep_copy (const knowledge::KnowledgeRecord & source)
{
  *this = source;
  unshare();
}

inline KnowledgeRecord
KnowledgeRecord::deep_copy () const
{
  KnowledgeRecord ret (*this);
  ret.unshare();
  return ret;
}

inline bool
KnowledgeRecord::exists (void) const
{
  return type_ != EMPTY;
}

inline int
KnowledgeRecord::status (void) const
{
  return type_ == EMPTY ? UNCREATED : MODIFIED;
}

/**
* sets the status to modified
**/
inline void
KnowledgeRecord::set_modified (void)
{
  if (!exists()) {
    set_value ((Integer)0);
  }
}

inline uint32_t
KnowledgeRecord::size (void) const
{
  if (type_ == INTEGER || type_ == DOUBLE) {
    return 1;
  } else if (is_string_type()) {
    return (uint32_t)str_value_->size () + 1;
  } else if (is_binary_file_type()) {
    return (uint32_t)file_value_->size ();
  } else if (type_ == INTEGER_ARRAY) {
    return (uint32_t)int_array_->size ();
  } else if (type_ == DOUBLE_ARRAY) {
    return (uint32_t)double_array_->size ();
  }
  return 1;
}

inline int32_t
KnowledgeRecord::type (void) const
{
  return type_;
}

inline bool
KnowledgeRecord::set_type (int32_t type)
{
  if ((uint32_t)type == type_ ||
      (is_string_type() && is_string_type(type)) ||
      (is_binary_file_type() && is_binary_file_type(type))
  ) {
    type_ = type;
    return true;
  }
  return false;
}

inline int64_t
KnowledgeRecord::get_encoded_size (void) const
{
  int64_t buffer_size (sizeof (uint32_t) * 2);
  if (type_ == INTEGER)
  {
    buffer_size += sizeof (Integer);
  }
  else if (type_ == DOUBLE)
  {
    buffer_size += sizeof (double);
  }
  else if (type_ == INTEGER_ARRAY)
  {
    buffer_size += sizeof (Integer) * double_array_->size();
  }
  else if (type_ == DOUBLE_ARRAY)
  {
    buffer_size += sizeof (double) * double_array_->size();
  }
  else if (is_string_type ())
  {
    buffer_size += str_value_->size () + 1;
  }
  else if (is_binary_file_type ())
  {
    buffer_size += file_value_->size ();
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
  return type == EMPTY || type == INTEGER || type == INTEGER_ARRAY;
}

inline bool
KnowledgeRecord::is_array_type (void) const
{
  return is_array_type (type_);
}

inline bool
KnowledgeRecord::is_array_type (uint32_t type)
{
  return (type & ALL_ARRAYS) != 0;
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
    shared_ = OWNED;
  }
}

inline void
KnowledgeRecord::clear_value (void) noexcept
{
  clear_union ();

  type_ = EMPTY;
}

inline const char *
KnowledgeRecord::read (const char * buffer,
  int64_t & buffer_remaining)
{
  // format is [key_size | key | type | value_size | value]

  uint32_t buff_value_size (0);
  decltype(type_) type = INTEGER;
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
      if (buff_value_size >= 1) {
        emplace_string (buffer, buff_value_size - 1);
      } else {
        emplace_string ();
      }
    }

    else if (type == INTEGER)
    {
      Integer tmp;
      memcpy (&tmp, buffer, sizeof (tmp));
      set_value (madara::utility::endian_swap (tmp));
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

      emplace_integers (std::move(tmp));
    }

    else if (type == DOUBLE)
    {
      double tmp;
      memcpy (&tmp, buffer, sizeof (tmp));
      set_value (madara::utility::endian_swap (tmp));
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

      emplace_doubles (std::move(tmp));
    }

    else if (is_binary_file_type (type))
    {
      const unsigned char *b = (const unsigned char *)buffer;
      emplace_file (b, b + size);
    }

    else {
      buffer_remaining = -1;
      return buffer;
    }

    buffer += buff_value_size;
    buffer_remaining -= sizeof (char) * buff_value_size;

    type_ = type;
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

inline void
KnowledgeRecord::set_value (const KnowledgeRecord &new_value)
{
  uint64_t cur_clock = this->clock;
  uint32_t cur_quality = this->write_quality;
  uint32_t cur_write_quality = this->write_quality;
  *this = new_value;
  this->clock = cur_clock;
  this->quality = cur_quality;
  this->write_quality = cur_write_quality;
}

inline void
KnowledgeRecord::set_value (KnowledgeRecord &&new_value)
{
  uint64_t cur_clock = this->clock;
  uint32_t cur_quality = this->write_quality;
  uint32_t cur_write_quality = this->write_quality;
  *this = std::move(new_value);
  this->clock = cur_clock;
  this->quality = cur_quality;
  this->write_quality = cur_write_quality;
}

// set the value_ to a string
inline void
KnowledgeRecord::set_value (std::string && new_value)
{
  emplace_string (std::move(new_value));
  type_ = STRING;
}

// set the value_ to a string
inline void
KnowledgeRecord::set_value (const std::string & new_value)
{
  emplace_string (new_value);
  type_ = STRING;
}

// set the value_ to a string
inline void
KnowledgeRecord::set_value (std::shared_ptr<std::string> new_value)
{
  emplace_shared_string (std::move(new_value));
  type_ = STRING;
}

// set the value_ to a string
inline void
KnowledgeRecord::set_value (const char * new_value, uint32_t size)
{
  emplace_string (new_value, size);
  type_ = STRING;
}

// set the value_ to a string
inline void
KnowledgeRecord::set_xml (const char * new_value, size_t size)
{
  emplace_string (new_value, size);
  type_ = XML;
}

// set the value_ to a string
inline void
KnowledgeRecord::set_xml (std::string && new_value)
{
  emplace_string (std::move(new_value));
  type_ = XML;
}

// set the value_ to a string
inline void
KnowledgeRecord::set_xml (const std::string & new_value)
{
  emplace_string (new_value);
  type_ = XML;
}

// set the value_ to a string
inline void
KnowledgeRecord::set_xml (std::shared_ptr<std::string> new_value)
{
  emplace_shared_string (std::move(new_value));
  type_ = XML;
}

// set the value_ to a string
inline void
KnowledgeRecord::set_text (const char * new_value, size_t size)
{
  emplace_string (new_value, size);
  type_ = TEXT_FILE;
}

// set the value_ to a string
inline void
KnowledgeRecord::set_text (std::string && new_value)
{
  emplace_string (std::move(new_value));
  type_ = TEXT_FILE;
}

// set the value_ to a string
inline void
KnowledgeRecord::set_text (const std::string & new_value)
{
  emplace_string (new_value);
  type_ = TEXT_FILE;
}

// set the value_ to a string
inline void
KnowledgeRecord::set_text (std::shared_ptr<std::string> new_value)
{
  emplace_shared_string (std::move(new_value));
  type_ = TEXT_FILE;
}

// set the value_ to a string
inline void
KnowledgeRecord::set_jpeg (const unsigned char * new_value,
                                    size_t size)
{
  emplace_file (new_value, new_value + size);
  type_ = IMAGE_JPEG;
}

inline void
KnowledgeRecord::set_jpeg (std::vector <unsigned char> && new_value)
{
  emplace_file (std::move(new_value));
  type_ = IMAGE_JPEG;
}

inline void
KnowledgeRecord::set_jpeg (const std::vector <unsigned char> & new_value)
{
  emplace_file (new_value);
  type_ = IMAGE_JPEG;
}

inline void
KnowledgeRecord::set_jpeg (
    std::shared_ptr<std::vector <unsigned char>> new_value)
{
  emplace_shared_file (std::move(new_value));
  type_ = IMAGE_JPEG;
}

// set the value_ to a string
inline void
KnowledgeRecord::set_file (const unsigned char * new_value,
                                    size_t size)
{
  emplace_file (new_value, new_value + size);
  type_ = UNKNOWN_FILE_TYPE;
}

inline void
KnowledgeRecord::set_file (std::vector <unsigned char> && new_value)
{
  emplace_file (std::move(new_value));
  type_ = UNKNOWN_FILE_TYPE;
}

inline void
KnowledgeRecord::set_file (const std::vector <unsigned char> & new_value)
{
  emplace_file (new_value);
  type_ = UNKNOWN_FILE_TYPE;
}

inline void
KnowledgeRecord::set_file (
    std::shared_ptr<std::vector <unsigned char>> new_value)
{
  emplace_shared_file (std::move(new_value));
  type_ = UNKNOWN_FILE_TYPE;
}

// set the value_ to an integer
template<typename T,
  typename std::enable_if<std::is_integral<T>::value, void*>::type>
inline void
KnowledgeRecord::set_value (T new_value)
{
  if (type_ != INTEGER) {
    clear_union();
    type_ = INTEGER;
  }
  int_value_ = new_value;
}

// set the value_ to an array of doubles
inline void
KnowledgeRecord::set_value (const Integer * new_value, uint32_t size)
{
  emplace_integers (new_value, new_value + size);
}

// set the value_ to an array of integers
inline void
KnowledgeRecord::set_value (std::vector <Integer> && new_value)
{
  emplace_integers (std::move(new_value));
}

// set the value_ to an array of integers
inline void
KnowledgeRecord::set_value (const std::vector <Integer> & new_value)
{
  emplace_integers (new_value);
}

// set the value_ to an array of integers
inline void
KnowledgeRecord::set_value (std::shared_ptr<std::vector <Integer>> new_value)
{
  emplace_shared_integers (std::move(new_value));
}

// set the value_ to a double
template<typename T,
  typename std::enable_if<std::is_floating_point<T>::value, void*>::type>
inline void
KnowledgeRecord::set_value (T new_value)
{
  if (type_ != DOUBLE) {
    clear_union();
    type_ = DOUBLE;
  }
  double_value_ = new_value;
}

// set the value_ to an array of doubles
inline void
KnowledgeRecord::set_value (const double * new_value, uint32_t size)
{
  emplace_doubles (new_value, new_value + size);
}

// set the value_ to an array of doubles
inline void
KnowledgeRecord::set_value (std::vector <double> && new_value)
{
  emplace_doubles (std::move(new_value));
}

// set the value_ to an array of doubles
inline void
KnowledgeRecord::set_value (const std::vector <double> & new_value)
{
  emplace_doubles (new_value);
}

// set the value_ to an array of doubles
inline void
KnowledgeRecord::set_value (std::shared_ptr<std::vector <double>> new_value)
{
  emplace_shared_doubles (std::move(new_value));
}

/**
  * sets the value at the index to the specified value. If the
  * record was previously not an array or if the array is not
  * large enough, a new array is created.
  **/
template<typename T,
  typename std::enable_if<std::is_integral<T>::value, void*>::type>
inline void
KnowledgeRecord::set_index (size_t index, T value)
{
  if (type_ == DOUBLE_ARRAY)
  {
    // let the set_index for doubles take care of this
    set_index (index, double (value));
    return;
  }
  else if (type_ == INTEGER_ARRAY)
  {
    unshare();

    if (index >= int_array_->size ())
    {
      int_array_->resize(index + 1);
    }
  }
  else
  {
    emplace_integers (index + 1);
  }

  int_array_->at (index) = value;
}

/**
  * sets the value at the index to the specified value. If the
  * record was previously not an array or if the array is not
  * large enough, a new array is created.
  **/
template<typename T,
  typename std::enable_if<std::is_floating_point<T>::value, void*>::type>
inline void
KnowledgeRecord::set_index (size_t index, T value)
{
  if (type_ == INTEGER_ARRAY)
  {
    std::vector<double> tmp (int_array_->begin (), int_array_->end ());
    emplace_doubles (std::move(tmp));
  }
  else if (type_ != DOUBLE_ARRAY)
  {
    emplace_doubles (index + 1);
  }
  else
  {
    unshare();

    if (index >= double_array_->size ())
    {
      double_array_->resize (index + 1);
    }
  }

  double_array_->at (index) = value;
}

inline std::shared_ptr<std::string>
KnowledgeRecord::share_string() const
{
  if (is_string_type()) {
    shared_ = SHARED;
    return str_value_;
  }
  return nullptr;
}

inline std::shared_ptr<std::string>
KnowledgeRecord::take_string()
{
  if (is_string_type()) {
    std::shared_ptr<std::string> ret;

    using std::swap;
    swap(ret, str_value_);

    reset_value();

    return ret;
  }
  return nullptr;
}

inline std::shared_ptr<std::vector<KnowledgeRecord::Integer>>
KnowledgeRecord::share_integers() const
{
  if (type_ == INTEGER_ARRAY) {
    shared_ = SHARED;
    return int_array_;
  }
  return nullptr;
}

inline std::shared_ptr<std::vector<KnowledgeRecord::Integer>>
KnowledgeRecord::take_integers()
{
  if (type_ == INTEGER_ARRAY) {
    std::shared_ptr<std::vector<Integer>> ret;

    using std::swap;
    swap(ret, int_array_);

    reset_value();

    return ret;
  }
  return nullptr;
}

inline std::shared_ptr<std::vector<double>>
KnowledgeRecord::share_doubles() const
{
  if (type_ == DOUBLE_ARRAY) {
    shared_ = SHARED;
    return double_array_;
  }
  return nullptr;
}

inline std::shared_ptr<std::vector<double>>
KnowledgeRecord::take_doubles()
{
  if (type_ == DOUBLE_ARRAY) {
    std::shared_ptr<std::vector<double>> ret;

    using std::swap;
    swap(ret, double_array_);

    reset_value();

    return ret;
  }
  return nullptr;
}

inline std::shared_ptr<std::vector<unsigned char>>
KnowledgeRecord::share_binary() const
{
  if (is_binary_file_type()) {
    shared_ = SHARED;
    return file_value_;
  }
  return nullptr;
}

inline std::shared_ptr<std::vector<unsigned char>>
KnowledgeRecord::take_binary()
{
  if (is_binary_file_type()) {
    std::shared_ptr<std::vector<unsigned char>> ret;

    using std::swap;
    swap(ret, file_value_);

    reset_value();

    return ret;
  }
  return nullptr;
}

inline
KnowledgeRecord::operator bool (void) const
{
  return is_true();
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
      decltype(type_) tmp = madara::utility::endian_swap (type_);
      memcpy (buffer, &tmp, sizeof (tmp));
      buffer += sizeof (tmp);
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
        memcpy (buffer, &(*str_value_)[0], size);
      }
    }
    else if (type_ == INTEGER)
    {
      if (buffer_remaining >= int64_t (sizeof (Integer)))
      {
        integer_temp = madara::utility::endian_swap (int_value_);
        memcpy (buffer, &integer_temp, sizeof (integer_temp));

        size_intermediate = sizeof (Integer);
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
      if (buffer_remaining >= int64_t (sizeof (double)))
      {
        double_temp = madara::utility::endian_swap (double_value_);
        memcpy (buffer, &double_temp, sizeof (double));

        size_intermediate = sizeof (double);
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
    std::stringstream buffer;
    buffer << "KnowledgeRecord::write: ";
    buffer << encoded_size << " byte encoding cannot fit in ";
    buffer << buffer_remaining << " byte buffer\n";
    
    madara_logger_ptr_log (logger_, logger::LOG_ERROR,
      buffer.str ().c_str ());

    throw exceptions::MemoryException (buffer.str ());
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
    std::stringstream buffer;
    buffer << "KnowledgeRecord::write: ";
    buffer << encoded_size << " byte encoding cannot fit in ";
    buffer << buffer_remaining << " byte buffer\n";
    
    madara_logger_ptr_log (logger_, logger::LOG_ERROR,
      buffer.str ().c_str ());

    throw exceptions::MemoryException (buffer.str ());
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
    std::stringstream buffer;
    buffer << "KnowledgeRecord::write: ";
    buffer << encoded_size << " byte encoding cannot fit in ";
    buffer << buffer_remaining << " byte buffer\n";
    
    madara_logger_ptr_log (logger_, logger::LOG_ERROR,
      buffer.str ().c_str ());

    throw exceptions::MemoryException (buffer.str ());
  }
  return buffer;
}

} }

#endif  // _KNOWLEDGE_RECORD_INL_
