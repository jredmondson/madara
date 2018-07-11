#ifndef _KNOWLEDGE_RECORD_CPP_
#define _KNOWLEDGE_RECORD_CPP_

#include "madara/knowledge/KnowledgeRecord.h"
#include "madara/knowledge/ThreadSafeContext.h"

#include "madara/utility/Utility.h"
#include <sstream>
#include <algorithm>
#include <stdlib.h>
#include <iomanip>
#include <iostream>
#include "madara/utility/IntTypes.h"

namespace {
  int madara_double_precision (-1);

  bool madara_use_scientific (false);
}

namespace madara { namespace knowledge {

int
KnowledgeRecord::get_precision (void)
{
  return madara_double_precision;
}

void
KnowledgeRecord::set_precision (int new_precision)
{
  madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_MINOR,
    "KnowledgeRecord::set_precision:" \
    " setting precision to %d\n", madara_double_precision);

  madara_double_precision = new_precision;
}

void
KnowledgeRecord::set_fixed (void)
{
  madara_logger_ptr_log (logger::global_logger.get_ptr (), logger::LOG_MINOR,
    "KnowledgeRecord::set_fixed:" \
    " setting output format to std::fixed\n");

  madara_use_scientific = false;
}

void
KnowledgeRecord::set_scientific (void)
{
  madara_logger_ptr_log (logger::global_logger.get_ptr (), logger::LOG_MINOR,
    "KnowledgeRecord::set_scientific:" \
    " setting output format to std::scientific\n");

  madara_use_scientific = true;
}

int
KnowledgeRecord::read_file (
  const std::string & filename, uint32_t read_as_type)
{
  void * buffer;
  size_t size;
  bool add_zero_char = false;

  // clear the old value
  clear_value ();

  std::string::size_type position = filename.rfind ('.');
  std::string extension = filename.substr (position,
    filename.size () - position);
  madara::utility::lower (extension);

  // do we have a text-based file
  if (is_string_type (read_as_type) || 
    extension == ".txt" || extension == ".xml")
  {
    add_zero_char = true;
  }

  // read the file into the temporary buffer
  if (madara::utility::read_file (filename, buffer, size, add_zero_char) == 0)
  {
    // do we have a text-based file
    if (is_string_type (read_as_type)
           || extension == ".txt" || extension == ".xml")
    {
      // change the string value and size to appropriate values
      str_value_ = std::make_shared<std::string> ((char *)buffer, size-1);

      if (is_string_type (read_as_type))
        type_ = read_as_type;
      else if (extension == ".xml")
        type_ = XML;
      else
        type_ = TEXT_FILE;
    }
    else
    {
      unsigned char *ucbuf = (unsigned char *)buffer;
      emplace_file (ucbuf, ucbuf + size);

      if (extension == ".jpg" || read_as_type == IMAGE_JPEG)
        type_ = IMAGE_JPEG;
      else
        type_ = UNKNOWN_FILE_TYPE;
    }

    return 0;
  }
  else
    return -1;
}

/**
  * writes the value to a file
  **/
ssize_t
KnowledgeRecord::to_file (const std::string & filename) const
{
  if (is_string_type ())
  {
    return madara::utility::write_file (filename,
      (void *)str_value_->c_str (), str_value_->size ());
  }
  else if (is_binary_file_type ())
  {
    return madara::utility::write_file (filename,
      (void *)&file_value_->at(0), file_value_->size ());
  }
  else
  {
    std::string buffer (to_string ());

    return madara::utility::write_file (filename,
      (void *)buffer.c_str (), buffer.size ());
  }
}


double
KnowledgeRecord::to_double (void) const
{
  double value = 0;

  if (type_ == DOUBLE)
    value = double_value_;
  else if (type_ == DOUBLE_ARRAY)
    value = double_array_->size () == 0 ? 0 : double_array_->at(0);
  else if (type_ != EMPTY)
  {
    std::stringstream buffer;

    // read the value_ into a stringstream and then convert it to double
    if (type_ == INTEGER)
      buffer << int_value_;
    else if (type_ == INTEGER_ARRAY)
      buffer << (int_array_->size () == 0 ? 0 : int_array_->at(0));
    else if (is_string_type ())
      buffer << str_value_->c_str ();

    buffer >> value;
  }

  return value;
}

KnowledgeRecord::Integer
KnowledgeRecord::to_integer (void) const
{
  Integer value (0);

  if (type_ == INTEGER)
    value = int_value_;
  else if (type_ == INTEGER_ARRAY)
    value = int_array_->size () == 0 ? 0 : int_array_->at(0);
  else if (type_ != EMPTY)
  {
    std::stringstream buffer;

    // read the value_ into a stringstream and then convert it to double
    if (type_ == DOUBLE)
      buffer << double_value_;
    else if (type_ == DOUBLE_ARRAY)
      buffer << (double_array_->size () == 0 ? 0 : double_array_->at(0));
    else if (is_string_type ())
      buffer << str_value_->c_str();

    buffer >> value;
  }

  return value;
}

std::vector <KnowledgeRecord::Integer>
KnowledgeRecord::to_integers (void) const
{
  std::vector <Integer> integers;

  if (type_ == EMPTY) {
    return integers;
  }

  unsigned int size = (unsigned int)this->size ();
  integers.resize (size);

  if (type_ == INTEGER)
  {
    integers[0] = int_value_;
  }
  else if (type_ == INTEGER_ARRAY)
  {
    const Integer * ptr_temp = &(*int_array_)[0];

    for (unsigned int i = 0; i < size; ++i)
      integers[i] = ptr_temp[i];
  }
  else if (type_ == DOUBLE)
    integers[0] = Integer (double_value_);
  else if (type_ == DOUBLE_ARRAY)
  {
    const double * ptr_temp = &(*double_array_)[0];

    for (unsigned int i = 0; i < size; ++i)
      integers[i] = Integer (ptr_temp[i]);
  }
  else if (is_string_type ())
  {
    const char * ptr_temp = str_value_->c_str ();

    for (unsigned int i = 0; i < size; ++i)
      integers[i] = Integer (ptr_temp[i]);
  }
  else if (is_binary_file_type ())
  {
    const unsigned char * ptr_temp = &(*file_value_)[0];

    for (unsigned int i = 0; i < size; ++i)
      integers[i] = Integer (ptr_temp[i]);
  }

  return integers;
}

std::vector <double>
KnowledgeRecord::to_doubles (void) const
{
  std::vector <double> doubles;

  if (type_ == EMPTY) {
    return doubles;
  }

  unsigned int size = (unsigned int)this->size ();
  doubles.resize (size);

  if      (type_ == INTEGER)
    doubles[0] = double (int_value_);
  else if (type_ == INTEGER_ARRAY)
  {
    const Integer * ptr_temp = &(*int_array_)[0];

    for (unsigned int i = 0; i < size; ++i)
      doubles[i] = double (ptr_temp[i]);
  }
  else if (type_ == DOUBLE)
    doubles[0] = double_value_;
  else if (type_ == DOUBLE_ARRAY)
  {
    const double * ptr_temp = &(*double_array_)[0];

    for (unsigned int i = 0; i < size; ++i)
      doubles[i] = ptr_temp[i];
  }
  else if (is_string_type ())
  {
    const char * ptr_temp = str_value_->c_str ();

    for (unsigned int i = 0; i < size; ++i)
      doubles[i] = double (ptr_temp[i]);
  }
  else if (is_binary_file_type ())
  {
    const unsigned char * ptr_temp = &(*file_value_)[0];

    for (unsigned int i = 0; i < size; ++i)
      doubles[i] = double (ptr_temp[i]);
  }

  return doubles;
}

// read the value_ in a string format
std::string
KnowledgeRecord::to_string (const std::string & delimiter) const
{
  if (type_ == EMPTY) {
    return "";
  }

  if (type_ == ANY) {
    return "<Any type>";
  }

  if (!is_string_type ())
  {
    madara_logger_ptr_log (logger_, logger::LOG_DETAILED, "KnowledgeRecord::to_string:" \
      " type_ is %d\n", type_);

    std::stringstream buffer;

    if      (type_ == INTEGER)
      buffer << int_value_;
    else if (type_ == INTEGER_ARRAY)
    {
      const Integer * ptr_temp = &(*int_array_)[0];
      uint32_t size = this->size ();

      if (size >= 1)
        buffer << *ptr_temp;

      ++ptr_temp;

      for (uint32_t i = 1; i < size; ++i, ++ptr_temp)
        buffer << delimiter << *ptr_temp;
    }
    else if (type_ == DOUBLE)
    {
      // set fixed or scientific
      if (!madara_use_scientific)
      {
        madara_logger_ptr_log (logger_, logger::LOG_DETAILED,
          "KnowledgeRecord::to_string: using fixed format\n");

        buffer << std::fixed;
      }
      else
      {
        madara_logger_ptr_log (logger_, logger::LOG_DETAILED,
          "KnowledgeRecord::to_string: using scientific format\n");

        buffer << std::scientific;
      }

      if (madara_double_precision >= 0)
      {
        // set the precision of double output
        buffer << std::setprecision (madara_double_precision);

        madara_logger_ptr_log (logger_, logger::LOG_DETAILED, "KnowledgeRecord::to_string:" \
          " precision set to %d\n", madara_double_precision);
      }
      else
      {
        madara_logger_ptr_log (logger_, logger::LOG_DETAILED, "KnowledgeRecord::to_string:" \
          " precision set to default\n", madara_double_precision);
      }

      buffer << double_value_;
    }
    else if (type_ == DOUBLE_ARRAY)
    {
      // set fixed or scientific
      if (!madara_use_scientific)
      {
        madara_logger_ptr_log (logger_, logger::LOG_DETAILED,
          "KnowledgeRecord::to_string: using fixed format\n");

        buffer << std::fixed;
      }
      else
      {
        madara_logger_ptr_log (logger_, logger::LOG_DETAILED,
          "KnowledgeRecord::to_string: using scientific format\n");

        buffer << std::scientific;
      }

      if (madara_double_precision >= 0)
      {
        buffer << std::setprecision (madara_double_precision);

        madara_logger_ptr_log (logger_, logger::LOG_DETAILED, "KnowledgeRecord::to_string:" \
          " precision set to %d\n", madara_double_precision);
      }
      else
      {
        madara_logger_ptr_log (logger_, logger::LOG_DETAILED, "KnowledgeRecord::to_string:" \
          " precision set to default\n", madara_double_precision);
      }

      const double * ptr_temp = &(*double_array_)[0];
      uint32_t size = this->size ();

      if (size >= 1)
        buffer << *ptr_temp;

      ++ptr_temp;

      for (uint32_t i = 1; i < size; ++i, ++ptr_temp)
        buffer << delimiter << *ptr_temp; 
    }
    else if (is_binary_file_type ())
    {
      buffer << "binary:size=";
      buffer << size (); 
    }
    return buffer.str ();
  }
  else
    return std::string (*str_value_);
}

// read the value_ in a string format
unsigned char *
KnowledgeRecord::to_unmanaged_buffer (size_t & size) const
{
  char * buffer;

  if (is_string_type ())
  {
    size = str_value_->size ();
    buffer = new char [size];
    memcpy (buffer, str_value_->c_str (), size);
  }
  else if (is_binary_file_type ())
  {
    size = file_value_-> size();
    buffer = new char [size];
    memcpy (buffer, &(*file_value_)[0], size);
  }
  else if (type_ == INTEGER)
  {
    size = sizeof(Integer);
    buffer = new char [size];
    memcpy (buffer, &int_value_, size);
  }
  else if (type_ == DOUBLE)
  {
    size = sizeof(double);
    buffer = new char [size];
    memcpy (buffer, &double_value_, size);
  }
  else if (type_ == INTEGER_ARRAY)
  {
    size = sizeof(Integer) * int_array_->size ();
    buffer = new char [size];
    memcpy (buffer, &(*int_array_)[0], size);
  }
  else if (type_ == DOUBLE_ARRAY)
  {
    size = sizeof(double) * double_array_->size () ;
    buffer = new char [size];
    memcpy (buffer, &(*double_array_)[0], size);
  } else {
    buffer = nullptr;
    size = 0;
  }

  return (unsigned char *)buffer;
}


KnowledgeRecord
KnowledgeRecord::fragment (unsigned int first, unsigned int last)
{
  knowledge::KnowledgeRecord ret;

  if (is_string_type ())
  {
    unsigned int size = (unsigned int)str_value_->size ();

    // make sure last is accessible in the data type
    last = std::min <unsigned int> (last, size - 1);

     // Create a new buffer, copy over the elements, and add a null delimiter
    char * new_buffer = new char [last - first + 2];

    memcpy (new_buffer, str_value_->c_str () + first, last - first + 1);
    new_buffer[last-first + 1] = 0;

    ret.set_value (new_buffer);
  }
  else if (is_binary_file_type ())
  {
    unsigned int size = (unsigned int)file_value_->size ();

    // make sure last is accessible in the data type
    last = std::min <unsigned int> (last, (unsigned int)size - 1);

    // Unlike string types, file buffers are not ended with a null delimiter
    uint32_t bufsize = last - first + 1;
    unsigned char * new_buffer = new unsigned char [bufsize];

    memcpy (new_buffer, &(*file_value_)[0] + first, last - first + 1);

    // create a new record with the unsigned char buffer as contents
    ret.set_file (new_buffer, bufsize);
  }
  else if (type_ == INTEGER_ARRAY)
  {
    unsigned int size = (unsigned int)int_array_->size ();

    // make sure last is accessible in the data type
    last = std::min <unsigned int> (last, size - 1);
    uint32_t bufsize = last - first + 1;

    std::vector <Integer> integers;
    integers.resize (bufsize);
    Integer * ptr_temp = &(*int_array_)[0];

    for (unsigned int i = first; i <= last; ++i, ++ptr_temp)
      integers[i] = *ptr_temp;

    ret.set_value (integers);
  }
  else if (type_ == DOUBLE_ARRAY)
  {
    unsigned int size = (unsigned int) double_array_->size ();

    // make sure last is accessible in the data type
    last = std::min <unsigned int> (last, size - 1);
    uint32_t bufsize = last - first + 1;

    std::vector <double> doubles;
    doubles.resize (bufsize);
    double * ptr_temp = &(*double_array_)[0];

    for (unsigned int i = first; i <= last; ++i, ++ptr_temp)
      doubles[i] = *ptr_temp;

    ret.set_value (doubles);
  }

  return ret;
}

bool
KnowledgeRecord::operator< (
  const knowledge::KnowledgeRecord & rhs) const
{
  Integer result (0);

  // if the left hand side is an integer
  if (is_integer_type ())
  {
    Integer lhs = this->to_integer ();

    if (rhs.is_double_type () || rhs.is_string_type ())
    {
      double other = rhs.to_double ();

      result = lhs < other;
    }
    else if (rhs.is_integer_type ())
    {
      Integer other = rhs.to_integer ();

      result = lhs < other;
    }
  }

  // if the left hand side is a string
  else if (is_string_type ())
  {
    // string to string comparison
    if      (rhs.is_string_type ())
    {
      result =
        strncmp (str_value_->c_str (), rhs.str_value_->c_str (), 
        size () >= rhs.size () ? size () : rhs.size ()) < 0;
    }

    // string to double comparison
    else if (rhs.is_double_type ())
    {
      // when comparing strings to anything else, convert the
      // value into a double for maximum precision
      double temp = to_double ();
      double other = rhs.to_double ();

      result = temp < other;
    }

    // default is string to integer comparison
    else if (rhs.is_integer_type ())
    {
      // when comparing strings to anything else, convert the
      // value into a double for maximum precision
      Integer temp = to_integer ();
      Integer other = rhs.to_integer ();

      result = temp < other;
    }
  }

  // if the left hand side is a double
  else if (is_double_type ())
  {
    double lhs = to_double ();

    // string to string comparison
    if      (rhs.is_string_type () || rhs.is_double_type ())
    {
      // when comparing strings to anything else, convert the
      // value into a double for maximum precision

      double other = rhs.to_double ();

      result = lhs < other;
    }

    // default is string to integer comparison
    else if (rhs.is_integer_type ())
    {
      Integer other = rhs.to_integer ();
      result = lhs < other;
    }
  }

  return result != 0;
}

bool
KnowledgeRecord::operator<= (
  const knowledge::KnowledgeRecord & rhs) const
{
  Integer result (0);

  // if the left hand side is an integer
  if (is_integer_type ())
  {
    Integer lhs = this->to_integer ();

    if (rhs.is_double_type () || rhs.is_string_type ())
    {
      double other = rhs.to_double ();

      result = lhs <= other;
    }
    else if (rhs.is_integer_type ())
    {
      Integer other = rhs.to_integer ();

      result = lhs <= other;
    }
  }

  // if the left hand side is a string
  else if (is_string_type ())
  {
    // string to string comparison
    if      (rhs.is_string_type ())
    {
      result = 
        strncmp (str_value_->c_str (), rhs.str_value_->c_str (), 
        size () >= rhs.size () ? size () : rhs.size ()) <= 0;
    }

    // string to double comparison
    else if (rhs.is_double_type ())
    {
      // when comparing strings to anything else, convert the
      // value into a double for maximum precision
      double temp = to_double ();
      double other = rhs.to_double ();

      result = temp <= other;
    }

    // default is string to integer comparison
    else if (rhs.is_integer_type ())
    {
      // when comparing strings to anything else, convert the
      // value into a double for maximum precision
      Integer temp = to_integer ();
      Integer other = rhs.to_integer ();

      result = temp <= other;
    }
  }

  // if the left hand side is a double
  else if (is_double_type ())
  {
    double lhs = to_double ();

    // string to string comparison
    if      (rhs.is_string_type () || rhs.is_double_type ())
    {
      // when comparing strings to anything else, convert the
      // value into a double for maximum precision

      double other = rhs.to_double ();

      result = lhs <= other;
    }

    // default is string to integer comparison
    else if (rhs.is_integer_type ())
    {
      Integer other = rhs.to_integer ();
      result = lhs <= other;
    }
  }

  return result != 0;
}

bool
KnowledgeRecord::operator== (
  const knowledge::KnowledgeRecord & rhs) const
{
  Integer result (0);

  // if left hand side does 
  if (!exists ())
  {
    if (!rhs.exists () || rhs.is_false ())
    {
      result = 1;
    }
  }

  // if the left hand side is an integer
  else if (is_integer_type ())
  {
    if (rhs.is_double_type ())
    {
      result = to_double () == rhs.to_double ();
    }
    else if (rhs.is_integer_type ())
    {
      result = to_integer () == rhs.to_integer ();
    }
    else if (rhs.is_string_type ())
    {
      if (rhs.size () > 0 && rhs.str_value_->at (0) >= '0' &&
        rhs.str_value_->at (0) <= '9')
      {
        result = to_double () == rhs.to_double ();
      }
      else result = 0;
    }
  }

  // if the left hand side is a string
  else if (is_string_type ())
  {
    // string to string comparison
    if      (rhs.is_string_type ())
    {
      result = 
        strncmp (str_value_->c_str (), rhs.str_value_->c_str (), 
        size () >= rhs.size () ? size () : rhs.size ()) == 0;
    }

    // string to double comparison
    else if (rhs.is_double_type ())
    {
      // when comparing strings to anything else, convert the
      // value into a double for maximum precision
      result = to_double () == rhs.to_double ();
    }

    // check if right hand side is uncreated
    else if (!rhs.exists ())
    {
      result = is_false ();
    }

    // default is string to integer comparison
    else if (rhs.is_integer_type ())
    {
      if (size () > 0 && this->str_value_->at (0) >= '0' &&
        this->str_value_->at (0) <= '9')
      {
        result = to_double () == rhs.to_double ();
      }
      else
      {
        result = 0;
      }
    }
  }

  // if the left hand side is a double
  else if (is_double_type ())
  {
    double lhs = to_double ();

    // string to string comparison
    if      (rhs.is_string_type () || rhs.is_double_type ())
    {
      // when comparing strings to anything else, convert the
      // value into a double for maximum precision

      double other = rhs.to_double ();

      result = lhs == other;
    }

    // default is string to integer comparison
    else if (rhs.is_integer_type ())
    {
      Integer other = rhs.to_integer ();

      result = lhs == other;
    }
  }

  return result != 0;
}

bool
KnowledgeRecord::operator> (const knowledge::KnowledgeRecord & rhs) const
{
  Integer result (0);

  // if the left hand side is an integer
  if (is_integer_type ())
  {
    Integer lhs = this->to_integer ();

    if (rhs.is_double_type () || rhs.is_string_type ())
    {
      double other = rhs.to_double ();

      result = lhs > other;
    }
    else if (rhs.is_integer_type ())
    {
      Integer other = rhs.to_integer ();

      result = lhs > other;
    }
  }


  // if the left hand side is a string
  else if (is_string_type ())
  {
    // string to string comparison
    if      (rhs.is_string_type ())
    {
      result = 
        strncmp (str_value_->c_str (), rhs.str_value_->c_str (), 
        size () >= rhs.size () ? size () : rhs.size ()) > 0;
    }

    // string to double comparison
    else if (rhs.is_double_type ())
    {
      // when comparing strings to anything else, convert the
      // value into a double for maximum precision
      double lhs = to_double ();
      double other = rhs.to_double ();

      result = lhs > other;
    }

    // default is string to integer comparison
    else if (rhs.is_integer_type ())
    {
      // when comparing strings to anything else, convert the
      // value into a double for maximum precision
      Integer lhs = to_integer ();
      Integer other = rhs.to_integer ();

      result = lhs > other;
    }
  }

  // if the left hand side is a double
  else if (is_double_type ())
  {
    double lhs = to_double ();

    // string to string comparison
    if      (rhs.is_string_type () || rhs.is_double_type ())
    {
      // when comparing strings to anything else, convert the
      // value into a double for maximum precision

      double other = rhs.to_double ();

      result = lhs > other;
    }

    // default is string to integer comparison
    else if (rhs.is_integer_type ())
    {
      Integer other = rhs.to_integer ();

      result = lhs > other;
    }
  }

  return result != 0;
}

bool
KnowledgeRecord::operator>= (const knowledge::KnowledgeRecord & rhs) const
{
  Integer result (0);

  // if the left hand side is an integer
  if (is_integer_type ())
  {
    Integer lhs = this->to_integer ();

    if (rhs.is_double_type () || rhs.is_string_type ())
    {
      double other = rhs.to_double ();

      result = lhs >= other;
    }
    else if (rhs.is_integer_type ())
    {
      Integer other = rhs.to_integer ();

      result = lhs >= other;
    }
  }


  // if the left hand side is a string
  else if (is_string_type ())
  {
    // string to string comparison
    if      (rhs.is_string_type ())
    {
      result =
        strncmp (str_value_->c_str (), rhs.str_value_->c_str (), 
        size () >= rhs.size () ? size () : rhs.size ()) >= 0;
    }

    // string to double comparison
    else if (rhs.is_double_type ())
    {
      // when comparing strings to anything else, convert the
      // value into a double for maximum precision
      double lhs = to_double ();
      double other = rhs.to_double ();

      result = lhs >= other;
    }

    // default is string to integer comparison
    else if (rhs.is_integer_type ())
    {
      // when comparing strings to anything else, convert the
      // value into a double for maximum precision
      Integer lhs = to_integer ();
      Integer other = rhs.to_integer ();

      result = lhs >= other;
    }
  }

  // if the left hand side is a double
  else if (is_double_type ())
  {
    double lhs = to_double ();

    // string to string comparison
    if      (rhs.is_string_type () || rhs.is_double_type ())
    {
      // when comparing strings to anything else, convert the
      // value into a double for maximum precision

      double other = rhs.to_double ();

      result = lhs >= other;
    }

    // default is string to integer comparison
    else if (rhs.is_integer_type ())
    {
      Integer other = rhs.to_integer ();

      result = lhs >= other;
    }
  }

  return result != 0;
}


KnowledgeRecord
KnowledgeRecord::retrieve_index (size_t index) const
{
  knowledge::KnowledgeRecord ret_value;

  if (type_ == INTEGER_ARRAY)
  {
    if (index < size_t (int_array_->size ()))
      ret_value.set_value (int_array_->at (index));
  }
  else if (type_ == DOUBLE_ARRAY)
  {
    if (index < size_t (double_array_-> size ()))
      ret_value.set_value (double_array_->at (index));
  }

  return ret_value;
}

KnowledgeRecord 
KnowledgeRecord::dec_index (size_t index)
{
  if (type_ == DOUBLE_ARRAY)
  {
    unshare();

    if (double_array_->size () <= index) {
      double_array_->resize (index + 1);
    }
    return knowledge::KnowledgeRecord(--double_array_->at (index));
  }
  else if (type_ == INTEGER_ARRAY)
  {
    unshare();

    if (int_array_->size () <= index) {
      int_array_->resize (index + 1);
    }
    return knowledge::KnowledgeRecord(--int_array_->at (index));
  }
  std::vector<Integer> tmp(index + 1);
  emplace_integers (std::move(tmp));
  return knowledge::KnowledgeRecord(--int_array_->at (index));
}

KnowledgeRecord 
KnowledgeRecord::inc_index (size_t index)
{
  if (type_ == DOUBLE_ARRAY)
  {
    unshare();

    if (double_array_->size () <= index) {
      double_array_->resize (index + 1);
    }
    return knowledge::KnowledgeRecord(++double_array_->at (index));
  }
  else if (type_ == INTEGER_ARRAY)
  {
    unshare();

    if (int_array_->size () <= index) {
      int_array_->resize (index + 1);
    }
    return knowledge::KnowledgeRecord(++int_array_->at (index));
  }
  std::vector<Integer> tmp(index + 1);
  emplace_integers (std::move(tmp));
  return knowledge::KnowledgeRecord(++int_array_->at (index));
}

void
KnowledgeRecord::resize (size_t new_size)
{
  size_t cur_size = size ();

  if (cur_size == new_size) {
    return;
  }

  unshare();

  if (new_size > cur_size)
  {
    if (type_ == EMPTY ||
        type_ == INTEGER)
    {
      Integer zero (0);
      set_index (new_size - 1, zero);
      return;
    }
    else if (type_ == DOUBLE)
    {
      double zero (0.0);
      set_index (new_size - 1, zero);
      return;
    }
  }
  if (type_ == INTEGER_ARRAY) {
    int_array_->resize (new_size);
  } else if (type_ == DOUBLE_ARRAY) {
    double_array_->resize (new_size);
  } else if (is_string_type (type_)) {
    str_value_->resize (new_size);
  } else if (is_binary_file_type (type_)) {
    file_value_->resize (new_size);
  }
}

int
  KnowledgeRecord::apply (
  ThreadSafeContext & context,
  const std::string & key, unsigned int /*quality*/, uint64_t /*clock*/,
  bool perform_lock)
{
  int result = -1;

  if (key.length () > 0)
  {
    madara_logger_ptr_log (logger_, logger::LOG_MINOR, "KnowledgeRecord::apply:" \
      " attempting to set %s=%s\n", key.c_str (), to_string ().c_str ());

    if (perform_lock)
      context.lock ();

    // if the data we are updating had a lower clock value or less quality
    // then that means this update is the latest value. Among
    // other things, this means our solution will work even
    // without FIFO channel transports

    // if the data we are updating had a lower clock value
    // then that means this update is the latest value. Among
    // other things, this means our solution will work even
    // without FIFO channel transports
    result = context.update_record_from_external (key, *this, 
      knowledge::KnowledgeUpdateSettings::GLOBAL_AS_LOCAL_NO_EXPAND);

    if (perform_lock)
    {
      context.unlock ();
      context.set_changed ();
    }

    // if we actually updated the value
    if (result == 1)
    {
      madara_logger_ptr_log (logger_, logger::LOG_MINOR, "KnowledgeRecord::apply:" \
        " received data[%s]=%s.\n",
        key.c_str (), to_string ().c_str ());
    }
    // if the data was already current
    else if (result == 0)
    {
      madara_logger_ptr_log (logger_, logger::LOG_MINOR, "KnowledgeRecord::apply:" \
        " discarded data[%s]=%s as the value was already set.\n",
        key.c_str (), to_string ().c_str ());
    }
    else if (result == -1)
    {
      madara_logger_ptr_log (logger_, logger::LOG_MINOR, "KnowledgeRecord::apply:" \
        " discarded data due to null key.\n");
    }
    else if (result == -2)
    {
      madara_logger_ptr_log (logger_, logger::LOG_MINOR, "KnowledgeRecord::apply:" \
        " discarded data[%s]=%s due to lower quality.\n",
        key.c_str (), to_string ().c_str ());
    }
    else if (result == -3)
    {
      madara_logger_ptr_log (logger_, logger::LOG_MINOR, "KnowledgeRecord::apply:" \
        " discarded data[%s]=%" PRId64 " due to older timestamp.\n",
        key.c_str (), to_string ().c_str ());
    }
  }
  return result;
}

bool
KnowledgeRecord::is_true (void) const
{
  madara_logger_ptr_log (logger_, logger::LOG_MAJOR, "KnowledgeRecord::apply:" \
    " checking if record is non-zero.\n");

  if (is_integer_type ())
    return to_integer () != 0;
  else if (is_double_type ())
  {
    double value = to_double ();
    return value < 0 || value > 0;
  }
  else if (is_string_type ())
  {
    return str_value_->size () >= 1;
  }
  else if (is_binary_file_type ())
  {
    return file_value_->size () >= 1;
  }
  else if (is_any_type ())
  {
    return !any_value_->empty ();
  }
  else
  {
    return false;
  }
}

} }

#endif   // _KNOWLEDGE_RECORD_CPP_
