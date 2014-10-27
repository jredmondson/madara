#ifndef _KNOWLEDGE_RECORD_CPP_
#define _KNOWLEDGE_RECORD_CPP_

#include "madara/knowledge_engine/Knowledge_Record.h"
#include "madara/knowledge_engine/Thread_Safe_Context.h"
#include "madara/utility/Log_Macros.h"
#include "madara/utility/Utility.h"
#include <sstream>
#include <algorithm>
#include <stdlib.h>
#include <iomanip>
#include <iostream>

int madara_double_precision (-1);

int
Madara::Knowledge_Record::get_precision (void)
{
  return madara_double_precision;
}

void
Madara::Knowledge_Record::set_precision (int new_precision)
{
  MADARA_DEBUG (MADARA_LOG_MINOR_EVENT, (LM_DEBUG, 
      DLINFO "Knowledge_Record::set_precision:" \
      " setting precision to %d\n", madara_double_precision));

  madara_double_precision = new_precision;
}

Madara::Knowledge_Record::Knowledge_Record ()
      : status_ (UNCREATED), clock (0), scope (LOCAL_SCOPE),
        quality (0), write_quality (0),
        int_value_ (0), type_ (INTEGER)
{
}

Madara::Knowledge_Record::Knowledge_Record (Integer value)
      : status_ (UNCREATED), clock (0), scope (LOCAL_SCOPE),
        quality (0), write_quality (0)
{
  set_value (value);
}

Madara::Knowledge_Record::Knowledge_Record (
  const std::vector <Integer> & value)
      : status_ (UNCREATED), clock (0), scope (LOCAL_SCOPE),
        quality (0), write_quality (0)
{
  set_value (value);
}

Madara::Knowledge_Record::Knowledge_Record (double value)
      : status_ (UNCREATED), clock (0), scope (LOCAL_SCOPE),
        quality (0), write_quality (0)
{
  set_value (value);
}

Madara::Knowledge_Record::Knowledge_Record (
  const std::vector <double> & value)
      : status_ (UNCREATED), clock (0), scope (LOCAL_SCOPE),
        quality (0), write_quality (0)
{
  set_value (value);
}

Madara::Knowledge_Record::Knowledge_Record (const std::string & value)
      : status_ (UNCREATED), clock (0), scope (LOCAL_SCOPE),
        quality (0), write_quality (0)
{
  set_value (value);
}

Madara::Knowledge_Record::Knowledge_Record (const char * value)
      : status_ (UNCREATED), clock (0), scope (LOCAL_SCOPE),
        quality (0), write_quality (0)
{
  set_value (std::string (value));
}

Madara::Knowledge_Record::Knowledge_Record (const Knowledge_Record & rhs)
      : status_ (rhs.status_), clock (rhs.clock),
      scope (rhs.scope), quality (rhs.quality),
      write_quality (rhs.write_quality),
      size_ (rhs.size_), type_ (rhs.type_)
{
  if      (rhs.status_ != UNCREATED)
  {
    if      (rhs.type_ == INTEGER)
      int_value_ = rhs.int_value_;
    else if (rhs.type_ == INTEGER_ARRAY)
      int_array_ = rhs.int_array_;
    else if (rhs.type_ == DOUBLE)
      double_value_ = rhs.double_value_;
    else if (rhs.type_ == DOUBLE_ARRAY)
      double_array_ = rhs.double_array_;
    else if (rhs.is_string_type ())
      str_value_ = rhs.str_value_;
    else if (rhs.is_file_type ())
      file_value_ = rhs.file_value_;
  }
}

Madara::Knowledge_Record::~Knowledge_Record ()
{
}


bool
Madara::Knowledge_Record::exists (void)
{
  return status_ != UNCREATED;
}

Madara::Knowledge_Record *
Madara::Knowledge_Record::clone (void) const
{
  Knowledge_Record * result = new Knowledge_Record ();
  
  if (is_ref_counted ())
  {
    if (this->type_ == STRING)
    {
      result->set_value (this->to_string ());
    }
    else if (this->type_ == TEXT_FILE)
    {
      std::string text = this->to_string ();
      result->set_text (text.c_str (), text.size ());
    }
    else if (this->type_ == XML)
    {
      std::string text = this->to_string ();
      result->set_xml (text.c_str (), text.size ());
    }
    else if (this->type_ == DOUBLE_ARRAY)
    {
      result->set_value (this->to_doubles ());
    }
    else if (this->type_ == INTEGER_ARRAY)
    {
      result->set_value (this->to_integers ());
    }
    else if (this->type_ == UNKNOWN_FILE_TYPE || this->type_ == IMAGE_JPEG)
    {
      size_t temp;
      result->size_ = this->size_;
      result->type_ = this->type_;
      result->file_value_ = this->to_unmanaged_buffer (temp);
    }
  }
  else
  {
    *result = *this;
  }

  return result;
}

void
Madara::Knowledge_Record::deep_copy (const Knowledge_Record & source)
{
  if (source.is_ref_counted ())
  {
    if (source.type_ == STRING)
    {
      set_value (source.to_string ());
    }
    else if (source.type_ == TEXT_FILE)
    {
      std::string text = source.to_string ();
      set_text (text.c_str (), text.size ());
    }
    else if (source.type_ == XML)
    {
      std::string text = source.to_string ();
      set_xml (text.c_str (), text.size ());
    }
    else if (source.type_ == DOUBLE_ARRAY)
    {
      set_value (source.to_doubles ());
    }
    else if (source.type_ == INTEGER_ARRAY)
    {
      set_value (source.to_integers ());
    }
    else if (source.type_ == UNKNOWN_FILE_TYPE || source.type_ == IMAGE_JPEG)
    {
      size_t temp;
      size_ = source.size_;
      type_ = source.type_;
      file_value_ = source.to_unmanaged_buffer (temp);
    }
  }
  else
  {
    *this = source;
  }
}

int
Madara::Knowledge_Record::read_file (
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
  Madara::Utility::lower (extension);
  
  // do we have a text-based file
  if (is_string_type (read_as_type) || 
    extension == ".txt" || extension == ".xml")
  {
    add_zero_char = true;
  }

  // read the file into the temporary buffer
  if (Madara::Utility::read_file (filename, buffer, size, add_zero_char) == 0)
  {
    // do we have a text-based file
    if (is_string_type (read_as_type)
           || extension == ".txt" || extension == ".xml")
    {
      // change the string value and size to appropriate values
      str_value_ = (char *)buffer;
      size_ = (uint32_t)size;
      
      if (is_string_type (read_as_type))
        type_ = read_as_type;
      else if (extension == ".xml")
        type_ = XML;
      else
        type_ = TEXT_FILE;
      status_ = MODIFIED;
    }
    else
    {
      file_value_ = (unsigned char *)buffer;
      size_ = (uint32_t)size;

      if (extension == ".jpg" || read_as_type == IMAGE_JPEG)
        type_ = IMAGE_JPEG;
      else
        type_ = UNKNOWN_FILE_TYPE;
      status_ = MODIFIED;
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
Madara::Knowledge_Record::to_file (const std::string & filename) const
{
  if (is_string_type ())
  {
    return Madara::Utility::write_file (filename,
      (void *)str_value_.get_ptr (), size_);
  }
  else if (is_file_type ())
  {
    return Madara::Utility::write_file (filename,
      (void *)file_value_.get_ptr (), size_);
  }
  else
  {
    std::string buffer (to_string ());
    
    return Madara::Utility::write_file (filename,
      (void *)buffer.c_str (), buffer.size ());
  }
}


double
Madara::Knowledge_Record::to_double (void) const
{
  double value = 0;

  if (status_ != UNCREATED)
  {
    if (type_ == DOUBLE)
      value = double_value_;
    else if (type_ == DOUBLE_ARRAY)
      value = *(double_array_.get_ptr ());
    else
    {
      std::stringstream buffer;

      // read the value_ into a stringstream and then convert it to double
      if (type_ == INTEGER)
        buffer << int_value_;
      else if (type_ == INTEGER_ARRAY)
        buffer << *(int_array_.get_ptr ());
      else if (is_string_type ())
        buffer << str_value_.get_ptr ();

      buffer >> value;
    }
  }

  return value;
}

Madara::Knowledge_Record::Integer
Madara::Knowledge_Record::to_integer (void) const
{
  Integer value (0);

  if (status_ != UNCREATED)
  {
    if (type_ == INTEGER)
      value = int_value_;
    else if (type_ == INTEGER_ARRAY)
      value = *(int_array_.get_ptr ());
    else
    {
      std::stringstream buffer;

      // read the value_ into a stringstream and then convert it to double
      if (type_ == DOUBLE)
        buffer << double_value_;
      else if (type_ == DOUBLE_ARRAY)
        buffer << *(double_array_.get_ptr ());
      else if (is_string_type ())
        buffer << str_value_.get_ptr ();

      buffer >> value;
    }
  }
  
  return value;
}

std::vector <Madara::Knowledge_Record::Integer>
Madara::Knowledge_Record::to_integers (void) const
{
  std::vector <Integer> integers;

  if (status_ != UNCREATED)
  {
    unsigned int size = (unsigned int)size_;;
    integers.resize (size);

    if (type_ == INTEGER)
    {
      integers[0] = int_value_;
    }
    else if (type_ == INTEGER_ARRAY)
    {
      const Integer * ptr_temp = int_array_.get_ptr ();

      for (unsigned int i = 0; i < size; ++i)
        integers[i] = ptr_temp[i];
    }
    else if (type_ == DOUBLE)
      integers[0] = Integer (double_value_);
    else if (type_ == DOUBLE_ARRAY)
    {
      const double * ptr_temp = double_array_.get_ptr ();

      for (unsigned int i = 0; i < size; ++i)
        integers[i] = Integer (ptr_temp[i]);
    }
    else if (is_string_type ())
    {
      const char * ptr_temp = str_value_.get_ptr ();

      for (unsigned int i = 0; i < size; ++i)
        integers[i] = Integer (ptr_temp[i]);
    }
    else if (is_file_type ())
    {
      const unsigned char * ptr_temp = file_value_.get_ptr ();

      for (unsigned int i = 0; i < size; ++i)
        integers[i] = Integer (ptr_temp[i]);
    }
  }

  return integers;
}

std::vector <double>
Madara::Knowledge_Record::to_doubles (void) const
{
  std::vector <double> doubles;

  if (status_ != UNCREATED)
  {
    unsigned int size = (unsigned int)size_;;
    doubles.resize (size);

    if      (type_ == INTEGER)
      doubles[0] = double (int_value_);
    else if (type_ == INTEGER_ARRAY)
    {
      const Integer * ptr_temp = int_array_.get_ptr ();

      for (unsigned int i = 0; i < size; ++i)
        doubles[i] = double (ptr_temp[i]);
    }
    else if (type_ == DOUBLE)
      doubles[0] = double_value_;
    else if (type_ == DOUBLE_ARRAY)
    {
      const double * ptr_temp = double_array_.get_ptr ();

      for (unsigned int i = 0; i < size; ++i)
        doubles[i] = ptr_temp[i];
    }
    else if (is_string_type ())
    {
      const char * ptr_temp = str_value_.get_ptr ();

      for (unsigned int i = 0; i < size; ++i)
        doubles[i] = double (ptr_temp[i]);
    }
    else if (is_file_type ())
    {
      const unsigned char * ptr_temp = file_value_.get_ptr ();

      for (unsigned int i = 0; i < size; ++i)
        doubles[i] = double (ptr_temp[i]);
    }
  }

  return doubles;
}

// read the value_ in a string format
std::string
Madara::Knowledge_Record::to_string (const std::string & delimiter) const
{
  if (status_ != UNCREATED)
  {
    if (!is_string_type ())
    {
      MADARA_DEBUG (MADARA_LOG_MINOR_EVENT, (LM_DEBUG, 
          DLINFO "Knowledge_Record::to_string:" \
          " type_ is %d\n", type_));

      std::stringstream buffer;
    
      if      (type_ == INTEGER)
        buffer << int_value_;
      else if (type_ == INTEGER_ARRAY)
      {
        const Integer * ptr_temp = int_array_.get_ptr ();

        if (size_ >= 1)
          buffer << *ptr_temp;

        ++ptr_temp;

        for (uint32_t i = 1; i < size_; ++i, ++ptr_temp)
          buffer << delimiter << *ptr_temp; 
      }
      else if (type_ == DOUBLE)
      {
        if (madara_double_precision >= 0)
        {
          buffer << std::setprecision (madara_double_precision);
          buffer << std::fixed;

          MADARA_DEBUG (MADARA_LOG_MINOR_EVENT, (LM_DEBUG, 
              DLINFO "Knowledge_Record::to_string:" \
              " precision set to %d\n", madara_double_precision));
        }
        else
        {
          MADARA_DEBUG (MADARA_LOG_MINOR_EVENT, (LM_DEBUG, 
              DLINFO "Knowledge_Record::to_string:" \
              " precision set to default\n", madara_double_precision));
        }

        buffer << double_value_;
      }
      else if (type_ == DOUBLE_ARRAY)
      {
        if (madara_double_precision >= 0)
        {
          buffer << std::setprecision (madara_double_precision);
          buffer << std::fixed;

          MADARA_DEBUG (MADARA_LOG_MINOR_EVENT, (LM_DEBUG, 
              DLINFO "Knowledge_Record::to_string:" \
              " precision set to %d\n", madara_double_precision));
        }
        else
        {
          MADARA_DEBUG (MADARA_LOG_MINOR_EVENT, (LM_DEBUG, 
              DLINFO "Knowledge_Record::to_string:" \
              " precision set to default\n", madara_double_precision));
        }

        const double * ptr_temp = double_array_.get_ptr ();

        if (size_ >= 1)
          buffer << *ptr_temp;

        ++ptr_temp;

        for (uint32_t i = 1; i < size_; ++i, ++ptr_temp)
          buffer << delimiter << *ptr_temp; 
      }
      else if (is_binary_file_type ())
      {
        buffer << "binary:size=";
        buffer << size_; 
      }
      return buffer.str ();
    }
    else
      return std::string (str_value_.get_ptr ());
  }
  else
    return "0";
}

// read the value_ in a string format
unsigned char *
Madara::Knowledge_Record::to_unmanaged_buffer (size_t & size) const
{
  if (status_ != UNCREATED)
  {
    unsigned char * buffer = new unsigned char [size_];

    if (is_string_type ())
    {
      memcpy (buffer, str_value_.get_ptr (), size_);
    }
    else if (is_file_type ())
    {
      memcpy (buffer, file_value_.get_ptr (), size_);
    }
    else if (type_ == INTEGER)
    {
      memcpy (buffer, &int_value_, sizeof(Integer) * size_);
    }
    else if (type_ == DOUBLE)
    {
      memcpy (buffer, &double_value_, sizeof(double) * size_);
    }
    else if (type_ == INTEGER_ARRAY)
    {
      memcpy (buffer, int_array_.get_ptr (), sizeof(Integer) * size_);
    }
    else if (type_ == DOUBLE_ARRAY)
    {
      memcpy (buffer, double_array_.get_ptr (), sizeof(double) * size_);
    }

    return buffer;
  }
  else
    return 0;
}


Madara::Knowledge_Record
Madara::Knowledge_Record::fragment (unsigned int first, unsigned int last)
{
  Knowledge_Record ret;

  if (first <= last && status_ != UNCREATED)
  {
    if (is_string_type ())
    {
      // make sure last is accessible in the data type
      last = std::min <unsigned int> (last, size_ - 1);

       // Create a new buffer, copy over the elements, and add a null delimiter
      char * new_buffer = new char [last - first + 2];

      memcpy (new_buffer, str_value_.get_ptr () + first, last - first + 1);
      new_buffer[last-first + 1] = 0;

      ret.set_value (new_buffer);
    }
    else if (is_file_type ())
    {
      // make sure last is accessible in the data type
      last = std::min <unsigned int> (last, size_ - 1);

      // Unlike string types, file buffers are not ended with a null delimiter
      uint32_t size = last - first + 1;
      unsigned char * new_buffer = new unsigned char [size];

      memcpy (new_buffer, file_value_.get_ptr () + first, last - first + 1);

      // create a new record with the unsigned char buffer as contents
      ret.set_file (new_buffer, size);
    }
    else if (type_ == INTEGER_ARRAY)
    {
      // make sure last is accessible in the data type
      last = std::min <unsigned int> (last, size_ - 1);
      uint32_t size = last - first + 1;

      std::vector <Integer> integers;
      integers.resize (size);
      Integer * ptr_temp = int_array_.get_ptr ();

      for (unsigned int i = first; i <= last; ++i, ++ptr_temp)
        integers[i] = *ptr_temp;

      ret.set_value (integers);
    }
    else if (type_ == DOUBLE_ARRAY)
    {
      // make sure last is accessible in the data type
      last = std::min <unsigned int> (last, size_ - 1);
      uint32_t size = last - first + 1;

      std::vector <double> doubles;
      doubles.resize (size);
      double * ptr_temp = double_array_.get_ptr ();

      for (unsigned int i = first; i <= last; ++i, ++ptr_temp)
        doubles[i] = *ptr_temp;

      ret.set_value (doubles);
    }
  }

  return ret;
}

Madara::Knowledge_Record::Integer
Madara::Knowledge_Record::operator< (const Knowledge_Record & rhs) const
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
        strncmp (str_value_.get_ptr (), rhs.str_value_.get_ptr (), 
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

  return result;
}

Madara::Knowledge_Record::Integer
Madara::Knowledge_Record::operator<= (const Knowledge_Record & rhs) const
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
        strncmp (str_value_.get_ptr (), rhs.str_value_.get_ptr (), 
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

  return result;
}

Madara::Knowledge_Record::Integer
Madara::Knowledge_Record::operator== (const Knowledge_Record & rhs) const
{
  Integer result (0);

  // if the left hand side is an integer
  if (is_integer_type ())
  {
    Integer lhs = this->to_integer ();

    if (rhs.is_double_type () || rhs.is_string_type ())
    {
      double other = rhs.to_double ();
      
      result = lhs == other;
    }
    else if (rhs.is_integer_type ())
    {
      Integer other = rhs.to_integer ();
    
      result = lhs == other;
    }
  }

  // if the left hand side is a string
  else if (is_string_type ())
  {
    // string to string comparison
    if      (rhs.is_string_type ())
    {
      result = 
        strncmp (str_value_.get_ptr (), rhs.str_value_.get_ptr (), 
        size () >= rhs.size () ? size () : rhs.size ()) == 0;
    }
    
    // string to double comparison
    else if (rhs.is_double_type ())
    {
      // when comparing strings to anything else, convert the
      // value into a double for maximum precision
      double temp = to_double ();
      double other = rhs.to_double ();
      
      result = temp == other;
    }
    
    // default is string to integer comparison
    else if (rhs.is_integer_type ())
    {
      // when comparing strings to anything else, convert the
      // value into a double for maximum precision
      Integer temp = to_integer ();
      Integer other = rhs.to_integer ();
      
      result = temp == other;
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

  return result;
}

bool
Madara::Knowledge_Record::operator== (Integer value) const
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

bool
Madara::Knowledge_Record::operator== (double value) const
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

bool
Madara::Knowledge_Record::operator== (const std::string & value) const
{
  return to_string () == value;
}

bool
Madara::Knowledge_Record::operator== (const char * value) const
{
  return to_string () == std::string (value);
}

Madara::Knowledge_Record::Integer 
Madara::Knowledge_Record::operator!= (const Knowledge_Record & rhs) const
{
  return !(*this == rhs);
}

/**
  * Logical and.
  **/
Madara::Knowledge_Record::Integer
  Madara::Knowledge_Record::operator&& (const Knowledge_Record & rhs) const
{
  return this->is_true () && rhs.is_true ();
}

/**
  * Logical or.
  **/
Madara::Knowledge_Record::Integer
  Madara::Knowledge_Record::operator|| (const Knowledge_Record & rhs) const
{
  return this->is_true () || rhs.is_true ();
}

/**
  * Logical not.
  **/
Madara::Knowledge_Record::Integer
Madara::Knowledge_Record::operator! (void) const
{
  return !is_true ();
}

/**
  * Negate.
  **/
Madara::Knowledge_Record
  Madara::Knowledge_Record::operator- (void) const
{
  Madara::Knowledge_Record record (*this);

  if (is_integer_type ())
    record.set_value (-to_integer ());
  else if (is_double_type ())
    record.set_value (-to_double ());

  return record;
}

Madara::Knowledge_Record::Integer
Madara::Knowledge_Record::operator> (const Knowledge_Record & rhs) const
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
        strncmp (str_value_.get_ptr (), rhs.str_value_.get_ptr (), 
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

  return result;
}

Madara::Knowledge_Record::Integer 
Madara::Knowledge_Record::operator>= (const Knowledge_Record & rhs) const
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
        strncmp (str_value_.get_ptr (), rhs.str_value_.get_ptr (), 
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

  return result;
}

Madara::Knowledge_Record
Madara::Knowledge_Record::operator= (const Knowledge_Record & rhs)
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
    size_ = rhs.size_;
    type_ = rhs.type_;

    if (is_string_type ())
      str_value_ = rhs.str_value_;
    else if (type_ == INTEGER)
      int_value_ = rhs.int_value_;
    else if (type_ == INTEGER_ARRAY)
      int_array_ = rhs.int_array_;
    else if (type_ == DOUBLE)
      double_value_ = rhs.double_value_;
    else if (type_ == DOUBLE_ARRAY)
      double_array_ = rhs.double_array_;
    else if (is_file_type ())
      file_value_ = rhs.file_value_;
  }

  return *this;
}

/**
  * Atomic addition operator
  **/
Madara::Knowledge_Record
Madara::Knowledge_Record::operator+= (const Knowledge_Record & rhs)
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
  * Atomic predecrement
  **/
Madara::Knowledge_Record
Madara::Knowledge_Record::operator-- (void)
{
  if (is_integer_type ())
    set_value (to_integer () - 1);

  else if (is_double_type ())
    set_value (to_double () - 1);

  return *this;
}
   
/**
  * Atomic preincrement
  **/
Madara::Knowledge_Record
Madara::Knowledge_Record::operator++ (void)
{
  if (is_integer_type ())
    set_value (to_integer () + 1);

  else if (is_double_type ())
    set_value (to_double () + 1);


  return *this;
}
       
/**
  * Atomic subtraction operator
  **/
Madara::Knowledge_Record
Madara::Knowledge_Record::operator-= (const Knowledge_Record & rhs)
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
  * Atomic multiplication operator
  **/
Madara::Knowledge_Record
Madara::Knowledge_Record::operator*= (const Knowledge_Record & rhs)
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
  * Atomic division operator
  **/
Madara::Knowledge_Record
Madara::Knowledge_Record::operator/= (const Knowledge_Record & rhs)
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
  * Atomic division operator
  **/
Madara::Knowledge_Record
Madara::Knowledge_Record::operator%= (const Knowledge_Record & rhs)
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
Madara::Knowledge_Record
Madara::Knowledge_Record::operator* (const Knowledge_Record & rhs) const
{
  // copy this value to a local copy
  Knowledge_Record ret_value (*this);

  return ret_value *= rhs;
}

/**
  * Divides operator
  **/
Madara::Knowledge_Record
Madara::Knowledge_Record::operator/ (const Knowledge_Record & rhs) const
{
  // copy this value to a local copy
  Knowledge_Record ret_value (*this);

  return ret_value /= rhs;
}

/**
  * Modulus operator
  **/
Madara::Knowledge_Record
Madara::Knowledge_Record::operator% (const Knowledge_Record & rhs) const
{
  // copy this value to a local copy
  Knowledge_Record ret_value (*this);

  return ret_value %= rhs;
}

/**
  * Plus operator
  **/
Madara::Knowledge_Record
Madara::Knowledge_Record::operator+ (const Knowledge_Record & rhs) const
{
  // copy this value to a local copy
  Knowledge_Record ret_value (*this);

  return ret_value += rhs;
}
    
/**
  * Minus operator
  **/
Madara::Knowledge_Record
Madara::Knowledge_Record::operator- (const Knowledge_Record & rhs) const
{
  // copy this value to a local copy
  Knowledge_Record ret_value (*this);

  return ret_value -= rhs;
}
    

int64_t
Madara::Knowledge_Record::get_encoded_size (const std::string & key) const
{
  int64_t buffer_size (sizeof (uint32_t) * 3 );
  buffer_size += (key.size () + 1);
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

char *
Madara::Knowledge_Record::write (char * buffer, const std::string & key,
   int64_t & buffer_remaining) const
     
{
  // format is [key_size | key | type | value_size | value]

  uint32_t key_size = uint32_t (key.size () + 1);
  char * size_location = 0;
  uint32_t size_intermediate = 0;
  uint32_t uint32_temp;
  Integer integer_temp;
  double double_temp;

  int64_t encoded_size = get_encoded_size (key);

  if (buffer_remaining >= encoded_size)
  {
    MADARA_DEBUG (MADARA_LOG_MINOR_EVENT, (LM_DEBUG, 
        DLINFO "Knowledge_Record::write:" \
        " encoding %Q byte message\n", encoded_size));

    // Remove the key size from the buffer
    if (buffer_remaining >= sizeof (key_size))
    {
      uint32_temp = Madara::Utility::endian_swap (key_size);
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
  
    // Remove the type of value from the buffer
    if (buffer_remaining >= sizeof (type_))
    {
      uint32_temp = Madara::Utility::endian_swap (type_);
      memcpy (buffer, &uint32_temp, sizeof (uint32_temp));
      buffer += sizeof (type_);
    }
    buffer_remaining -= sizeof (type_);

    // Remove the size of value from the buffer
    if (buffer_remaining >= sizeof (size_))
    {
      // set a pointer to size, in case we need to modify it during
      // value copy (e.g. during double conversion)
      size_location = buffer;
      size_intermediate = size_;
    
      uint32_temp = Madara::Utility::endian_swap (size_);
      memcpy (buffer, &uint32_temp, sizeof (uint32_temp));

      // note that we do not encode the size yet because it may change
      // and we need the architectural-specific version for other checks

      buffer += sizeof (size_);
    }
    buffer_remaining -= sizeof (size_);
  
    // Remove the value from the buffer
    if      (is_string_type ())
    {
      // strings do not have to be converted
      if (buffer_remaining >=  int64_t (size_))
      {
        memcpy (buffer, str_value_.get_ptr (), size_);
      }
    }
    else if (type_ == INTEGER)
    {
      if (buffer_remaining >= int64_t (size_ * sizeof (Integer)))
      {
        integer_temp = Madara::Utility::endian_swap (int_value_);
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
          integer_temp = Madara::Utility::endian_swap (*ptr_temp);
          memcpy (target_buffer, &integer_temp, sizeof (Integer));
        }

        size_intermediate = size_ * sizeof (Integer);
      }
    }
    else if (type_ == DOUBLE)
    {
      if (buffer_remaining >= int64_t (size_ * sizeof (double)))
      {
        double_temp = Madara::Utility::endian_swap (double_value_);
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
          double_temp = Madara::Utility::endian_swap (*ptr_temp);
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
    MADARA_DEBUG (MADARA_LOG_MINOR_EVENT, (LM_DEBUG, 
        DLINFO "Knowledge_Record::write:" \
        " %q byte buffer cannot contain %q byte message\n",
        buffer_remaining, encoded_size));
  }
  return buffer;
}


Madara::Knowledge_Record
Madara::Knowledge_Record::retrieve_index (size_t index) const
{
  Knowledge_Record ret_value;

  if (type_ == INTEGER_ARRAY)
  {
    if (index < size_t (size_))
      ret_value.set_value (int_array_.get_ptr ()[index]);
  }
  else if (type_ == DOUBLE_ARRAY)
  {
    if (index < size_t (size_))
      ret_value.set_value (double_array_.get_ptr ()[index]);
  }

  return ret_value;
}

Madara::Knowledge_Record 
Madara::Knowledge_Record::dec_index (size_t index)
{
  if (type_ == DOUBLE_ARRAY)
  {
    if (index >= size_)
    {
      double * ptr_temp = new double [index+1];
      memcpy (ptr_temp, double_array_.get_ptr (), size_ * sizeof (double));
      
      for (size_t i = size_; i < index+1; ++i)
        ptr_temp[i] = 0;
      
      size_ = uint32_t (index+1);
      double_array_ = ptr_temp;
    }
  }
  else if (type_ == INTEGER_ARRAY)
  {
    if (index >= size_)
    {
      Integer * ptr_temp = new Integer [index+1];
      memcpy (ptr_temp, int_array_.get_ptr (), size_ * sizeof (Integer));

      for (size_t i = size_; i < index+1; ++i)
        ptr_temp[i] = 0;

      size_ = uint32_t (index+1);
      int_array_ = ptr_temp;
    }
  }
  else
  {
    Integer * ptr_temp = new Integer [index+1];
    size_ = uint32_t (index+1);

    for (size_t i = 0; i < index + 1; ++i)
      ptr_temp[i] = 0;

    int_array_ = ptr_temp;
    type_ = INTEGER_ARRAY;
  }
  
  if (status_ != MODIFIED)
    status_ = MODIFIED;

  return Knowledge_Record (--int_array_.get_ptr ()[index]);
}
 
Madara::Knowledge_Record 
Madara::Knowledge_Record::inc_index (size_t index)
{
  if (type_ == DOUBLE_ARRAY)
  {
    if (index >= size_)
    {
      double * ptr_temp = new double [index+1];
      memcpy (ptr_temp, double_array_.get_ptr (), size_ * sizeof (double));
      
      for (size_t i = size_; i < index+1; ++i)
        ptr_temp[i] = 0;
      
      size_ = uint32_t (index+1);
      double_array_ = ptr_temp;
    }
  }
  else if (type_ == INTEGER_ARRAY)
  {
    if (index >= size_)
    {
      Integer * ptr_temp = new Integer [index+1];
      memcpy (ptr_temp, int_array_.get_ptr (), size_ * sizeof (Integer));

      for (size_t i = size_; i < index+1; ++i)
        ptr_temp[i] = 0;

      size_ = uint32_t (index+1);
      int_array_ = ptr_temp;
    }
  }
  else
  {
    Integer * ptr_temp = new Integer [index+1];
    size_ = uint32_t (index+1);

    for (size_t i = 0; i < index + 1; ++i)
      ptr_temp[i] = 0;

    int_array_ = ptr_temp;
    type_ = INTEGER_ARRAY;
  }
  
  if (status_ != MODIFIED)
    status_ = MODIFIED;

  return Knowledge_Record (++int_array_.get_ptr ()[index]);
}
 
/**
  * sets the value at the index to the specified value. If the
  * record was previously not an array or if the array is not
  * large enough, a new array is created.
  **/
void
Madara::Knowledge_Record::set_index (size_t index, Integer value)
{
  if (type_ == DOUBLE_ARRAY)
  {
    // let the set_index for doubles take care of this
    set_index (index, double (value));
    return;
  }
  else if (type_ == INTEGER_ARRAY)
  {
    if (index >= size_)
    {
      Integer * ptr_temp = new Integer [index+1];
      memcpy (ptr_temp, int_array_.get_ptr (), size_ * sizeof (Integer));

      for (size_t i = size_; i < index; ++i)
        ptr_temp[i] = 0;

      size_ = uint32_t (index+1);
      int_array_ = ptr_temp;
    }
  }
  else
  {
    Integer * ptr_temp = new Integer [index+1];
    size_ = uint32_t (index+1);

    for (size_t i = 0; i < index; ++i)
      ptr_temp[i] = 0;

    int_array_ = ptr_temp;
    type_ = INTEGER_ARRAY;
  }
  
  int_array_.get_ptr ()[index] = value;

  if (status_ != MODIFIED)
    status_ = MODIFIED;
}
 
/**
  * sets the value at the index to the specified value. If the
  * record was previously not an array or if the array is not
  * large enough, a new array is created.
  **/
void
Madara::Knowledge_Record::set_index (size_t index, double value)
{
  if (type_ == INTEGER_ARRAY)
  {
    /**
     * if this was previously an integer array, then copy
     * all of the elements and update the index appropriately
     **/
    size_t size = size_ > index + 1 ? size_ : index + 1;
    double * ptr_temp = new double [size];
    Integer * source_array = int_array_.get_ptr ();

    // go through each element of the old array and copy over
    for (size_t i = 0; i < size_; ++i)
      ptr_temp[i] = source_array[i];
    
    for (size_t i = size_; i < size; ++i)
      ptr_temp[i] = 0;

    double_array_ = ptr_temp;
    size_ = uint32_t (size);
    type_ = DOUBLE_ARRAY;
  }
  else if (type_ != DOUBLE_ARRAY)
  {
    double * ptr_temp = new double [index+1];
    size_ = uint32_t (index+1);

    if (size_ > 1)
    {
      for (size_t i = 0; i < size_ - 1; ++i)
        ptr_temp[i] = 0;
    }

    double_array_ = ptr_temp;
    type_ = DOUBLE_ARRAY;
  }
  else
  {
    if (index + 1 >= size_)
    {
      size_t size = index + 1;
      double * ptr_temp = new double [size];
      double * source_array = double_array_.get_ptr ();

      // go through each element of the old array and copy over
      for (size_t i = 0; i < size_; ++i)
        ptr_temp[i] = source_array[i];

      for (size_t i = size_; i < size; ++i)
        ptr_temp[i] = 0;

      double_array_ = ptr_temp;
      size_ = uint32_t (size);
    }
  }
  
  double_array_.get_ptr ()[index] = value;

  if (status_ != MODIFIED)
    status_ = MODIFIED;
}

const char *
Madara::Knowledge_Record::read (const char * buffer, std::string & key,
                                int64_t & buffer_remaining)
{
  // format is [key_size | key | type | value_size | value]

  uint32_t key_size (0);
  uint32_t buff_value_size (0);

  // Remove the key size from the buffer
  if (buffer_remaining >= sizeof (key_size))
  {
    memcpy (&key_size, buffer, sizeof (key_size)); 
    key_size = Madara::Utility::endian_swap (key_size);
    buffer += sizeof (key_size);
  }
  buffer_remaining -= sizeof (key_size);

  // Remove the key from the buffer
  if (buffer_remaining >= int64_t (sizeof (char) * int64_t (key_size)))
  {
    // don't worry about null terminator
    key.assign (buffer, key_size - 1);

    buffer += sizeof (char) * key_size;
  }
  buffer_remaining -= sizeof (char) * int64_t (key_size);
  
  // Remove the type of value from the buffer
  if (buffer_remaining >= sizeof (type_))
  {
    memcpy (&type_, buffer, sizeof (type_)); 
    type_ = Madara::Utility::endian_swap (type_);
    buffer += sizeof (type_);
  }
  buffer_remaining -= sizeof (type_);
  
  // Remove the size of value from the buffer
  if (buffer_remaining >= sizeof (size_))
  {
    memcpy (&size_, buffer, sizeof (size_)); 
    size_ = Madara::Utility::endian_swap (size_);
    buff_value_size = size_;
    buffer += sizeof (size_);

    if (is_integer_type ())
      buff_value_size *= sizeof (Integer);
    else if (is_double_type ())
      buff_value_size *= sizeof (double);
  }
  buffer_remaining -= sizeof (size_);
  
  // Remove the value from the buffer
  if (size_ > 0 && buffer_remaining >= int64_t (buff_value_size))
  {
    if      (is_string_type ())
    {
      str_value_ = new char [size_];
      strncpy (str_value_.get_ptr (), buffer, size_);
    }
  
    else if (type_ == INTEGER)
    {
      memcpy (&int_value_, buffer, buff_value_size); 
      int_value_ = Madara::Utility::endian_swap (int_value_);
    }

    else if (type_ == INTEGER_ARRAY)
    {
      buff_value_size = size_ * sizeof (Integer);
      Integer * ptr_temp = new Integer [size_];

      memcpy (ptr_temp, buffer, buff_value_size); 

      for (uint32_t i = 0; i < size_; ++i)
      {
        ptr_temp[i] = Madara::Utility::endian_swap (ptr_temp[i]);
      }

      int_array_ = ptr_temp;
    }
    
    else if (type_ == DOUBLE)
    {
      memcpy (&double_value_, buffer, buff_value_size); 
      double_value_ = Madara::Utility::endian_swap (double_value_);
    }

    else if (type_ == DOUBLE_ARRAY)
    {
      double * ptr_temp = new double [size_];

      memcpy (ptr_temp, buffer, buff_value_size);

      for (uint32_t i = 0; i < size_; ++i)
      {
        ptr_temp[i] = Madara::Utility::endian_swap (ptr_temp[i]);
      }
      
      double_array_ = ptr_temp;
    }

    else if (is_file_type ())
    {
      file_value_ = new unsigned char [size_];
      memcpy (file_value_.get_ptr (), buffer, size_);
    }

    buffer += buff_value_size;
    buffer_remaining -= sizeof (char) * buff_value_size;

    status_ = MODIFIED;
  }

  return buffer;
}

void
Madara::Knowledge_Record::resize (size_t new_size)
{
  if (new_size > size_)
  {
    if (type_ == this->DOUBLE_ARRAY)
    {
      double zero (0.0);
      set_index (new_size - 1, zero);
    }
    else if (type_ == this->INTEGER_ARRAY)
    {
      Integer zero (0);
      set_index (new_size - 1, zero);
    }
  }
  else if (is_ref_counted () && new_size < size_)
  {
    size_ = (uint32_t)new_size;
  }
}

int
  Madara::Knowledge_Record::apply (
  Madara::Knowledge_Engine::Thread_Safe_Context & context,
  const std::string & key, unsigned int quality, uint64_t clock,
  bool perform_lock)
{
  int result = -1;

  if (key.length () > 0)
  {
    MADARA_DEBUG (MADARA_LOG_MINOR_EVENT, (LM_DEBUG, 
        DLINFO "Knowledge_Record::apply:" \
        " attempting to set %s=%s\n", key.c_str (), to_string ().c_str ()));

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
      Knowledge_Engine::Knowledge_Update_Settings (true));
          
          
    if (perform_lock)
    {
      context.unlock ();
      context.set_changed ();
    }

    // if we actually updated the value
    if (result == 1)
    {
      MADARA_DEBUG (MADARA_LOG_MAJOR_EVENT, (LM_DEBUG, 
        DLINFO "Knowledge_Record::apply:" \
        " received data[%s]=%s.\n", 
        key.c_str (), to_string ().c_str ()));
    }
    // if the data was already current
    else if (result == 0)
    {
      MADARA_DEBUG (MADARA_LOG_MAJOR_EVENT, (LM_DEBUG, 
          DLINFO "Knowledge_Record::apply:" \
          " discarded data[%s]=%s as the value was already set.\n",
          key.c_str (), to_string ().c_str ()));
    }
    else if (result == -1)
    {
      MADARA_DEBUG (MADARA_LOG_MAJOR_EVENT, (LM_DEBUG, 
        DLINFO "Knowledge_Record::apply:" \
        " discarded data due to null key.\n"));
    }
    else if (result == -2)
    {
      MADARA_DEBUG (MADARA_LOG_MAJOR_EVENT, (LM_DEBUG, 
        DLINFO "Knowledge_Record::apply:" \
        " discarded data[%s]=%q due to lower quality.\n",
        key.c_str (), to_string ().c_str ()));
    }
    else if (result == -3)
    {
      MADARA_DEBUG (MADARA_LOG_MAJOR_EVENT, (LM_DEBUG, 
        DLINFO "Knowledge_Record::apply:" \
        " discarded data[%s]=%q due to older timestamp.\n",
        key.c_str (), to_string ().c_str ()));
    }
  }
  return result;
}

bool
Madara::Knowledge_Record::is_true (void) const
{
  MADARA_DEBUG (MADARA_LOG_MAJOR_EVENT, (LM_DEBUG, 
    DLINFO "Knowledge_Record::apply:" \
    " checking if record is non-zero.\n"));

  if (is_integer_type ())
    return to_integer () != 0;
  else if (is_double_type ())
  {
    double value = to_double ();
    return value < 0 || value > 0;
  }
  else if (is_string_type ())
  {
    return size_ > 1;
  }
  else
  {
    return size_ >= 1;
  }
}

bool
Madara::Knowledge_Record::is_false (void) const
{
  return !is_true ();
}

/**
  * output stream buffering
  **/
std::ostream & operator<< (std::ostream & stream,
  const Madara::Knowledge_Record & rhs)
{
  if (rhs.type () &
        (Madara::Knowledge_Record::INTEGER_ARRAY | 
         Madara::Knowledge_Record::DOUBLE_ARRAY))
  {
    stream << rhs.to_string (", ");
  }
  else
    stream << rhs.to_string ();

  return stream;
}

  

#endif   // _KNOWLEDGE_RECORD_CPP_
