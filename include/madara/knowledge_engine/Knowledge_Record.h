

#ifndef _KNOWLEDGE_RECORD_H_
#define _KNOWLEDGE_RECORD_H_

/**
 * @file Knowledge_Record.h
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains the Knowledge_Record class, helper typdefs and functions
 **/

#include <string>
#include <vector>
#include <map>
#include "madara/MADARA_export.h"
#include "madara/utility/stdint.h"
#include "madara/utility/Scoped_Array.h"
#include "madara/utility/Refcounter.h"
#include "ace/os_include/sys/os_types.h"
#include "madara/logger/Global_Logger.h"
#include "madara/utility/inttypes.h"

namespace Madara
{

  namespace Knowledge_Engine
  {
    class Thread_Safe_Context;
  }

  /**
   * @class Knowledge_Record
   * @brief This class encapsulates an entry in a Knowledge_Base
   **/

  class MADARA_Export Knowledge_Record
  {
  public:
    // allow Thread_Safe_Context to modify private members
    friend class Knowledge_Engine::Thread_Safe_Context;

    enum
    {
      UNCREATED = 0,
      MODIFIED = 1
    };

    enum
    {
      GLOBAL_SCOPE = 0,
      LOCAL_SCOPE = 1
    };

    enum Value_Types
    {
      UNINITIALIZED = 0,
      INTEGER = 1,
      STRING = 2,
      DOUBLE = 4,
      FLOAT = 4,
      UNKNOWN_FILE_TYPE = 8,
      XML = 16,
      TEXT_FILE = 32,
      INTEGER_ARRAY = 64,
      DOUBLE_ARRAY = 128,
      IMAGE_JPEG = 256,
      ALL_ARRAYS = INTEGER_ARRAY | DOUBLE_ARRAY,
      ALL_INTEGERS = INTEGER | INTEGER_ARRAY,
      ALL_DOUBLES = DOUBLE | DOUBLE_ARRAY,
      ALL_PRIMITIVE_TYPES = INTEGER | STRING | DOUBLE |
                            INTEGER_ARRAY | DOUBLE_ARRAY,
      ALL_FILE_TYPES = UNKNOWN_FILE_TYPE | XML | TEXT_FILE | IMAGE_JPEG,
      ALL_IMAGES = IMAGE_JPEG,
      ALL_TEXT_FORMATS = XML | TEXT_FILE | STRING,
      ALL_TYPES = ALL_PRIMITIVE_TYPES | ALL_FILE_TYPES
    };
    
    typedef  int64_t     Integer;

    /**
     * last modification time
     **/
    uint64_t clock;

    /**
     * scope (global or local)
     **/
    int32_t scope;

    /**
     * priority of the update
     **/
    uint32_t quality;

    /**
     * write priority for any local updates
     **/
    uint32_t write_quality;

  private:
    /// the logger used for any internal debugging information
    Logger::Logger * logger_;

    /**
     * status of the knowledge record
     **/
    int status_;
    
    /**
     * size of the value
     **/
    uint32_t size_;
    
    /**
     * type of variable (INTEGER, DOUBLE, STRING, FILE, IMAGE)
     **/
    uint32_t type_;
    
    /**
     * Non-array versions of double/integer. About 10x faster
     * than using the ref-counted arrays
     **/
    union
    {
      Integer int_value_;
      double double_value_;
    };

    /**
     * potential string value of the node (size int)
     **/
    Madara::Utility::Scoped_Array <Integer> int_array_;

    /**
     * potential string value of the node (size int)
     **/
    Madara::Utility::Scoped_Array <double> double_array_;

    /**
     * potential string value of the node (size int)
     **/
    Madara::Utility::Scoped_Array <char> str_value_;

    /**
     * potential file value of the node
     **/
    Madara::Utility::Scoped_Array <unsigned char> file_value_;
    
  public:

    /* default constructor */
    Knowledge_Record (
      Logger::Logger & logger = *Logger::global_logger.get ());
    
    /* Integer constructor */
    Knowledge_Record (Integer value,
      Logger::Logger & logger = *Logger::global_logger.get ());
    
    /* Double constructor */
    Knowledge_Record (double value,
      Logger::Logger & logger = *Logger::global_logger.get ());
    
    /* Integer array constructor */
    Knowledge_Record (const std::vector <Integer> & value,
      Logger::Logger & logger = *Logger::global_logger.get ());
    
    /* Double array constructor */
    Knowledge_Record (const std::vector <double> & value,
      Logger::Logger & logger = *Logger::global_logger.get ());
    
    /* String constructor */
    Knowledge_Record (const std::string & value,
      Logger::Logger & logger = *Logger::global_logger.get ());
    
    /* Char pointer constructor for g++ */
    Knowledge_Record (const char * value,
      Logger::Logger & logger = *Logger::global_logger.get ());
    
    /* copy constructor */
    Knowledge_Record (const Knowledge_Record & rhs);
    
    /* destructor */
    ~Knowledge_Record ();
    
    /**
     * Checks if record exists (i.e., is not uncreated)
     * @return true if record exists, false otherwise
     **/
    bool exists (void) const;

    /**
     * retrieves the value at an array index. If the
     * knowledge record is not an array, false is returned.
     * @param    index    index of the array item
     * @return   value at the index
     **/
    Knowledge_Record retrieve_index (size_t index) const;
    
    /**
     * decrements the value at the index to the specified value. If the
     * record was previously not an array or if the array is not
     * large enough, a new array is created.
     * @param    index   index of the value to set
     **/
    Knowledge_Record dec_index (size_t index);
     
    /**
     * increments the value at the index to the specified value. If the
     * record was previously not an array or if the array is not
     * large enough, a new array is created.
     * @param    index   index of the value to set
     **/
    Knowledge_Record inc_index (size_t index);
     
    /**
     * sets the value at the index to the specified value. If the
     * record was previously not an array or if the array is not
     * large enough, a new array is created.
     * @param    index   index of the value to set
     * @param    value   the value to set at the specified index
     **/
    void set_index (size_t index, Integer value);
     
    /**
     * sets the value at the index to the specified value. If the
     * record was previously not an array or if the array is not
     * large enough, a new array is created.
     * @param    index   index of the value to set
     * @param    value   the value to set at the specified index
     **/
    void set_index (size_t index, double value);

    /**
     * converts the value to a string.
     * @param   delimiter   characters to insert in between
     *                      elements of an array
     * @return  the value as a string
     **/
    std::string to_string (const std::string & delimiter = ", ") const;
    
    /**
     * writes the value to a file
     * @param   filename     file location to write to
     **/
    ssize_t to_file (const std::string & filename) const;

    /**
     * clones the record
     **/
    Knowledge_Record * clone (void) const;

    /**
     * converts the value to an integer
     **/
    Integer to_integer (void) const;

    /**
     * converts the value to a float/double
     **/
    double to_double (void) const;
    
    /**
     * converts the value to a vector of integers
     * @return  a vector of integers
     **/
    std::vector <Integer> to_integers (void) const;

    /**
     * converts the value to a vector of doubles
     * @return  a vector of doubles
     **/
    std::vector <double> to_doubles (void) const;
    
    /**
     * returns an unmanaged buffer that the user will have
     * to take care of (this is a copy of the internal value).
     * If you use this function, you must explicitly delete the
     * value returned. For instance:
     *
     * unsigned char * my_value = record.to_unmanaged_buffer ();
     * ... do some work on the buffer
     * delete [] my_value;
     *
     * Failure to do the above WILL result in a memory leak
     * @param   size    size of the unmanaged buffer
     * @return  the unmanaged buffer of size bytes
     **/
    unsigned char * to_unmanaged_buffer (size_t & size) const;
    
    /**
     * sets the value to a double
     * @param    new_value   new value of the Knowledge Record
     **/
    void set_value (const Integer & new_value);
    
    /**
     * sets the value to an array of integers
     * @param    new_value   new value of the Knowledge Record
     * @param    size        num elements in the array
     **/
    void set_value (const Integer * new_value, uint32_t size);
    
    /**
     * sets the value to an array of integers
     * @param    new_value   new value of the Knowledge Record
     **/
    void set_value (const std::vector <Integer> & new_value);
    
    /**
     * sets the value to a string
     * @param    new_value   new value of the Knowledge Record
     **/
    void set_value (const std::string & new_value);
    
    /**
     * sets the value to a double
     * @param    new_value   new value of the Knowledge Record
     **/
    void set_value (const double & new_value);
    
    /**
     * sets the value to an array of doubles
     * @param    new_value   new value of the Knowledge Record
     * @param    size        num elements in the array
     **/
    void set_value (const double * new_value, uint32_t size);
    
    /**
     * sets the value to an array of doubles
     * @param    new_value   new value of the Knowledge Record
     **/
    void set_value (const std::vector <double> & new_value);
    
    /**
     * sets the value to an xml string
     * @param    new_value   new value of the Knowledge Record
     * @param    size        size of the new_value buffer
     **/
    void set_xml (const char * new_value, size_t size);
    
    /**
     * sets the value to an xml string
     * @param    new_value   new value of the Knowledge Record
     * @param    size        size of the new_value buffer
     **/
    void set_text (const char * new_value, size_t size);
    
    /**
     * Sets the double precision of a double record when using
     * to_string (). All doubles will have this fixed precision.
     * To return to default precision, set new_precision to a
     * negative number.
     * @param   new_precision  new double precision to use
     **/
    static void set_precision (int new_precision);
    
    /**
     * Gets the current double precision for double to string conversion
     * @return  the double precision for double to string conversion
     **/
    static int get_precision (void);

    /**
     * sets the value to an xml string
     * @param    new_value   new value of the Knowledge Record
     * @param    size        size of the new_value buffer
     **/
    void set_jpeg (const unsigned char * new_value, size_t size);
    
    /**
     * sets the value to an unknown file type
     * @param    new_value   new value of the Knowledge Record
     * @param    size        size of the new_value buffer
     **/
    void set_file (const unsigned char * new_value, size_t size);

    /**
     * Creates a deep copy of the knowledge record. Because each
     * Knowledge Record may contain non-thread-safe ref counted values,
     * user threads that reference knowledge records will want to use
     * their own ref counts, which can be accomplished via this method.
     * @param    source   the Knowledge Record we wish to copy
     **/
    void deep_copy (const Knowledge_Record & source);

    /**
     * returns the status of the record.
     * @return   1 if modified, 0 if uncreated
     **/
    int status (void) const;

    /**
     * sets the status to modified
     **/
    void set_modified (void);

    /**
     * resizes an array to a new size
     * @param  new_size  new size of the array
     **/
    void resize (size_t new_size);

    /**
     * reads a file and sets the type appropriately according to
     * the extension
     * @param   filename         location of the file to read from
     * @param   read_as_type     force a type (XML, TEXT, IMAGE_JPEG, or
     *                           UNKNOWN_FILE_TYPE)
     * @return  size of file
     **/
    int read_file (const std::string & filename, uint32_t read_as_type = 0);
    
    /**
     * resets the variable to an integer
     **/
    void reset_value (void);

    /**
     * clears any dynamic values. This method does not attempt to set
     * the value of the Knowledge Record, as clear does, and is consequently
     * more efficient.
     **/
    void clear_value (void);
    
    /**
     * returns the size of the value
     **/
    uint32_t size (void) const;
    
    /**
     * returns the size of the value
     **/
    int32_t type (void) const;
    
    /**
     * returns if the record has a reference-counted type
     * @return   true if the record is reference-counted
     **/
    bool is_ref_counted (void) const;
    
    /**
     * returns if the record has a reference-counted type
     * @param   type the type to check
     * @return   true if the record is reference-counted
     **/
    bool is_ref_counted (uint32_t type) const;
    
    /**
     * returns true if the record is a string type (STRING, XML, TEXT_FILE)
     * @return   true if the record is a string
     **/
    bool is_string_type (void) const;
    
    /**
     * returns if the record is a string type (STRING, XML, TEXT_FILE)
     * @param   type the type to check
     * @return   true if the record is a string
     **/
    bool is_string_type (uint32_t type) const;
    
    /**
     * returns if the record is a double type (DOUBLE, DOUBLE_ARRAY)
     * @return   true if the record is a double
     **/
    bool is_double_type (void) const;
     
    /**
     * returns if the record is a double type (DOUBLE, DOUBLE_ARRAY)
     * @param   type the type to check
     * @return   true if the record is a double
     **/
    bool is_double_type (uint32_t type) const;
     
    /**
     * returns if the record is a integer type (INTEGER, INTEGER_ARRAY)
     * @return   true if the record is an integer
     **/
    bool is_integer_type (void) const;
    
    /**
     * returns if the record is a integer type (INTEGER, INTEGER_ARRAY)
     * @param   type the type to check
     * @return   true if the record is an integer
     **/
    bool is_integer_type (uint32_t type) const;
     
    
    /**
     * returns a record containing a fragment of the character buffer.
     * For strings, this is equivalent to substring. For files, this is
     * like an unsigned char * equivalent to substring. For other types,
     * this will return host-specific byte values up to the size of the
     * value.
     * @param    first          first index (inclusive, >= 0)
     * @param    last           last index (inclusive, < size)
     **/
    Knowledge_Record fragment (unsigned int first, unsigned int last);

    /**
     * returns true if the knowledge record has an image type
     * @return    true if type is an image type
     **/
    bool is_image_type (void) const;
    
    /**
     * returns true if the knowledge record has an image type
     * @param     type   the type of the record
     * @return    true if type is an image type
     **/
    bool is_image_type (uint32_t type) const;
    
    /**
     * returns true if the knowledge record has a file type
     * @return    true if type is a file type
     **/
    bool is_file_type (void) const;
    
    /**
     * returns true if the knowledge record has a file type
     * @param     type   the type of the record
     * @return    true if type is a file type
     **/
    bool is_file_type (uint32_t type) const;
     
    /**
     * returns true if the knowledge record has a binary file type
     * @return    true if type is a file type
     **/
    bool is_binary_file_type (void) const;
    
    /**
     * returns true if the knowledge record has a binary file type
     * @param     type   the type of the record
     * @return    true if type is a file type
     **/
    bool is_binary_file_type (uint32_t type) const;
    
    /**
     * Less than
     **/
    Integer operator< (const Knowledge_Record & rhs) const;
    
    /**
     * Less than or equal to
     **/
    Integer operator<= (const Knowledge_Record & rhs) const;
    
    /**
     * Equal to
     **/
    Integer operator== (const Knowledge_Record & rhs) const;
    
    /**
     * Equal to
     **/
    bool operator== (Integer value) const;
    
    /**
     * Equal to
     **/
    bool operator== (double value) const;
    
    /**
     * Equal to
     **/
    bool operator== (const std::string & value) const;
     
    /**
     * Equal to
     **/
    bool operator== (const char * value) const;
    
    /**
     * Unequal to
     **/
    Integer operator!= (const Knowledge_Record & rhs) const;
    
    /**
     * Greater than
     **/
    Integer operator> (const Knowledge_Record & rhs) const;
    
    /**
     * Greater than or equal to
     **/
    Integer operator>= (const Knowledge_Record & rhs) const;
    
    /**
     * Logical not.
     **/
    Integer operator! (void) const;

    /**
     * Negate.
     **/
    Knowledge_Record operator- (void) const;

    /**
     * Assignment
     **/
    Knowledge_Record operator= (const Knowledge_Record & rhs);
    
    /**
     * Atomic addition operator
     **/
    Knowledge_Record operator+= (const Knowledge_Record & rhs);
    
    /**
     * Atomic subtraction operator
     **/
    Knowledge_Record operator-= (const Knowledge_Record & rhs);
    
    /**
     * Atomic multiplication operator
     **/
    Knowledge_Record operator*= (const Knowledge_Record & rhs);
    
    /**
     * Atomic division operator
     **/
    Knowledge_Record operator/= (const Knowledge_Record & rhs);
    
    /**
     * Atomic modulus operator
     **/
    Knowledge_Record operator%= (const Knowledge_Record & rhs);
    
    /**
     * Times operator
     **/
    Knowledge_Record operator* (const Knowledge_Record & rhs) const;
    
    /**
     * Divides operator
     **/
    Knowledge_Record operator/ (const Knowledge_Record & rhs) const;
    
    /**
     * Modulus operator
     **/
    Knowledge_Record operator% (const Knowledge_Record & rhs) const;
    
    /**
     * Plus operator
     **/
    Knowledge_Record operator+ (const Knowledge_Record & rhs) const;
    
    /**
     * Minus operator
     **/
    Knowledge_Record operator- (const Knowledge_Record & rhs) const;
    
    /**
     * And operator
     **/
    Integer operator&& (const Knowledge_Record & rhs) const;
    
    /**
     * Or operator
     **/
    Integer operator|| (const Knowledge_Record & rhs) const;
    
    /**
     * Preincrement operator
     **/
    Knowledge_Record operator++ (void);
    
    /**
     * Predecrement operator
     **/
    Knowledge_Record operator-- (void);
    
    /**
      * Reads a Knowledge_Record instance from a buffer and updates
      * the amount of buffer room remaining.
      * @param     buffer     the readable buffer where data is stored
      * @param     key        the name of the variable
      * @param     buffer_remaining  the count of bytes remaining in the
      *                              buffer to read
      * @return    current buffer position for next read
      **/
    const char * read (const char * buffer, std::string & key,
                 int64_t & buffer_remaining);
      
    /**
      * Writes a Knowledge_Record instance to a buffer and updates
      * the amount of buffer room remaining.
      * @param     buffer     the readable buffer where data is stored
      * @param     key        the name of the variable
      * @param     buffer_remaining  the count of bytes remaining in the
      *                              buffer to read
      * @return    current buffer position for next write
      **/
    char * write (char * buffer, const std::string & key,
                  int64_t & buffer_remaining) const;
     
    /**
     * Apply the knowledge record to a context, given some quality and clock
     **/
    int apply (Madara::Knowledge_Engine::Thread_Safe_Context & context,
               const std::string & key, unsigned int quality, uint64_t clock,
               bool perform_lock);

    /**
     * Checks to see if the record is true. For integers and doubles, true
     * means the value is non-zero. For strings, it means the value is non "";
     * @return  true if the record is non-zero or not-"". False otherwise.
     **/
    bool is_true (void) const;

    /**
     * Checks to see if the record is false. For integers and boules, false
     * means the value is zero. For strings, it means the value is "".
     * @return  true if the record is zero or "". False otherwise.
     **/
    bool is_false (void) const;
    
    /**
     * Checks to see if the record is valid. Valid records have a status
     * that is not UNCREATED.
     * @return  true if the record is valid. False otherwise.
     **/
    inline bool is_valid (void) const
    {
      return status_ != UNCREATED;
    }

    /**
     * Returns the encoded size of the record. This size is what is required
     * to write the key_size, key, type, value size, and all associated
     * information in the read () and write () methods.
     **/
    int64_t get_encoded_size (const std::string & key) const;
  };
  
  typedef ::std::map < std::string, Knowledge_Record>   Knowledge_Map;
  typedef ::std::vector < Knowledge_Record>             Knowledge_Vector;
  typedef ::std::vector < std::string>                  Knowledge_Rules;
  typedef ::std::vector < std::string>                  String_Vector;
  typedef ::std::map < std::string, Knowledge_Record *> Knowledge_Records;

  typedef std::string     Knowledge_Key;
  typedef Knowledge_Record              Knowledge_Value;
  
  /**
   * Returns the maximum quality within the records
   * @param    records     the list of records to gauge quality of
   * @return   the maximum quality within the list of records
   **/
  uint32_t max_quality (const Knowledge_Records & records);
  
  /**
   * Returns the maximum quality within the records
   * @param    records     the list of records to gauge quality of
   * @return   the maximum quality within the list of records
   **/
  uint32_t max_quality (const Knowledge_Map & records);
}


/**
  * output stream buffering
  **/
MADARA_Export std::ostream & operator<< (std::ostream & stream,
  const Madara::Knowledge_Record & rhs);

#include "Knowledge_Record.inl"


#endif  // _KNOWLEDGE_RECORD_H_
