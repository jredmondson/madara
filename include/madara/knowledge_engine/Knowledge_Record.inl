

#ifndef _KNOWLEDGE_RECORD_INL_
#define _KNOWLEDGE_RECORD_INL_

/**
* @file Knowledge_Record.h
* @author James Edmondson <jedmondson@gmail.com>
*
* This file contains the methods of the Knowledge_Record class
**/

inline int
Madara::Knowledge_Record::status (void) const
{
  return status_;
}

/**
* sets the status to modified
**/
inline void
Madara::Knowledge_Record::set_modified (void)
{
  status_ = MODIFIED;
}

inline uint32_t
Madara::Knowledge_Record::size (void) const
{
  return size_;
}

inline int32_t
Madara::Knowledge_Record::type (void) const
{
  return type_;
}

inline bool
Madara::Knowledge_Record::is_ref_counted (void) const
{
  return is_ref_counted (type_);
}

inline bool
Madara::Knowledge_Record::is_ref_counted (uint32_t type) const
{
  return type != INTEGER && type != DOUBLE;
}

inline bool
Madara::Knowledge_Record::is_string_type (void) const
{
  return is_string_type (type_);
}

inline bool
Madara::Knowledge_Record::is_string_type (uint32_t type) const
{
  return type == STRING || type == XML || type == TEXT_FILE;
}

inline bool
Madara::Knowledge_Record::is_double_type (void) const
{
  return is_double_type (type_);
}

inline bool
Madara::Knowledge_Record::is_double_type (uint32_t type) const
{
  return type == DOUBLE || type == DOUBLE_ARRAY;
}

inline bool
Madara::Knowledge_Record::is_integer_type (void) const
{
  return is_integer_type (type_);
}


inline bool
Madara::Knowledge_Record::is_integer_type (uint32_t type) const
{
  return type == INTEGER || type == INTEGER_ARRAY;
}

inline bool
Madara::Knowledge_Record::is_image_type (void) const
{
  return is_image_type (type_);
}

inline bool
Madara::Knowledge_Record::is_image_type (uint32_t type) const
{
  return type == IMAGE_JPEG;
}

inline bool
Madara::Knowledge_Record::is_file_type (void) const
{
  return is_file_type (type_);
}

inline bool
Madara::Knowledge_Record::is_file_type (uint32_t type) const
{
  return type == TEXT_FILE || type == XML ||
         type == IMAGE_JPEG || type == UNKNOWN_FILE_TYPE;
}

inline bool
Madara::Knowledge_Record::is_binary_file_type (void) const
{
  return is_binary_file_type (type_);
}

inline bool
Madara::Knowledge_Record::is_binary_file_type (uint32_t type) const
{
  return type == IMAGE_JPEG || type == UNKNOWN_FILE_TYPE;
}

inline uint32_t
Madara::max_quality (const Knowledge_Records & records)
{
  uint32_t max = 0;

  // iterate over the list and return the max
  for (Knowledge_Records::const_iterator i = records.begin ();
    i != records.end (); ++i)
  {
    max = std::max <uint32_t> (i->second->quality, max);
  }
  return max;
}

inline uint32_t
Madara::max_quality (const Knowledge_Map & records)
{
  uint32_t max = 0;

  // iterate over the list and return the max
  for (Knowledge_Map::const_iterator i = records.begin ();
    i != records.end (); ++i)
  {
    max = std::max <uint32_t> (i->second.quality, max);
  }
  return max;
}


inline void
Madara::Knowledge_Record::clear_value (void)
{
  if (status_ != UNCREATED)
  {
    if (type_ == INTEGER_ARRAY)
      int_array_ = 0;
    else if (type_ == DOUBLE_ARRAY)
      double_array_ = 0;
    else if (is_string_type ())
      str_value_ = 0;
    else if (is_file_type ())
      file_value_ = 0;
    int_value_ = 0;

    type_ = INTEGER;
    status_ = UNCREATED;
    size_ = 0;
  }
}


// reset the value_ to an integer
inline void  
Madara::Knowledge_Record::reset_value (void)
{
  clear_value ();

  quality = 0;
  write_quality = 0;
  clock = 0;
}

// set the value_ to a string
inline void
Madara::Knowledge_Record::set_value (const std::string & new_value)
{
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
Madara::Knowledge_Record::set_xml (const char * new_value, size_t size)
{
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
Madara::Knowledge_Record::set_text (const char * new_value, size_t size)
{
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
Madara::Knowledge_Record::set_jpeg (const unsigned char * new_value,
                                    size_t size)
{
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
Madara::Knowledge_Record::set_file (const unsigned char * new_value,
                                    size_t size)
{
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
Madara::Knowledge_Record::set_value (const Integer & new_value)
{
  clear_value ();
  type_ = INTEGER;
  status_ = MODIFIED;

  size_ = 1;
  int_value_ = new_value;
}

// set the value_ to an array of doubles
inline void
Madara::Knowledge_Record::set_value (const Integer * new_value, uint32_t size)
{
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
Madara::Knowledge_Record::set_value (const std::vector <Integer> & new_value)
{
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
Madara::Knowledge_Record::set_value (const double & new_value)
{
  clear_value ();
  type_ = DOUBLE;
  status_ = MODIFIED;

  size_ = 1;
  double_value_ = new_value;
}

// set the value_ to an array of doubles
inline void
Madara::Knowledge_Record::set_value (const double * new_value, uint32_t size)
{
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
Madara::Knowledge_Record::set_value (const std::vector <double> & new_value)
{
  clear_value ();
  type_ = DOUBLE_ARRAY;
  status_ = MODIFIED;

  size_ = uint32_t (new_value.size ());
  double * temp_array = new double [new_value.size ()];

  for (unsigned int i = 0; i < new_value.size (); ++i)
    temp_array[i] = new_value[i];

  double_array_ = temp_array;
}



#endif  // _KNOWLEDGE_RECORD_INL_
