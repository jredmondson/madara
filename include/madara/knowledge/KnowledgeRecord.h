

#ifndef _MADARA_KNOWLEDGE_RECORD_H_
#define _MADARA_KNOWLEDGE_RECORD_H_

/**
 * @file KnowledgeRecord.h
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains the KnowledgeRecord class, helper typdefs and functions
 **/

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <type_traits>
#include "madara/MADARA_export.h"
#include "madara/utility/stdint.h"
#include "madara/utility/Refcounter.h"
#include "ace/os_include/sys/os_types.h"
#include "madara/logger/GlobalLogger.h"
#include "madara/utility/inttypes.h"

namespace madara
{

  namespace knowledge
  {
    class ThreadSafeContext;

    /**
     * Tags to specify what type to construct in KnowledgeRecord forwarding
     * constructors. If you give one of the values defined in this namespace
     * as the first argument to the KnowledgeRecord constructor, the remaining
     * arguments will be forwarded to construct the underlying type in-place.
     **/
    namespace tags {
      static constexpr struct integers_t {} integers;
      static constexpr struct doubles_t {} doubles;
      static constexpr struct string_t {} string;
      static constexpr struct binary_t {} binary;
    }

    /**
     * @class KnowledgeRecord
     * @brief This class encapsulates an entry in a KnowledgeBase
     **/

    class MADARA_Export KnowledgeRecord
    {
    public:
      // allow ThreadSafeContext to modify private members
      friend class ThreadSafeContext;

      enum
      {
        UNCREATED = 0,
        MODIFIED = 1
      };

      enum
      {
        OWNED = 0,
        SHARED = 1
      };

      enum ValueTypes
      {
        EMPTY = 0,
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
        ALL_TYPES = ALL_PRIMITIVE_TYPES | ALL_FILE_TYPES,
        ALL_CLEARABLES = ALL_ARRAYS | ALL_TEXT_FORMATS | ALL_FILE_TYPES
      };

      typedef  int64_t     Integer;

    private:
      /// the logger used for any internal debugging information
      logger::Logger * logger_;

    public:
      /**
       * last modification time
       **/
      uint64_t clock = 0;

      /**
       * priority of the update
       **/
      uint32_t quality = 0;

      /**
       * write priority for any local updates
       **/
      uint32_t write_quality = 0;

    private:
      /**
       * Non-array versions of double/integer. About 10x faster
       * than using the ref-counted arrays
       **/
      union
      {
        Integer int_value_ = 0;
        double double_value_;

        std::shared_ptr<std::vector<Integer>> int_array_;
        std::shared_ptr<std::vector<double>> double_array_;
        std::shared_ptr<std::string> str_value_;
        std::shared_ptr<std::vector<unsigned char>> file_value_;
      };

      /**
       * type of variable (INTEGER, DOUBLE, STRING, FILE, IMAGE)
       **/
      uint32_t type_ = EMPTY;

      /**
       * is this knowledge record's shared_ptr, if any, exposed to
       * outside holders?
       **/
      mutable bool shared_ = OWNED;

      template<typename T>
      using MemberType = std::shared_ptr<T> KnowledgeRecord::*;

      template<typename T, uint32_t Type, MemberType<T> Member, typename... Args>
      std::shared_ptr<T> &emplace_shared_val(Args&&... args) {
        clear_union();
        type_ = Type;
        return *new(&(this->*Member)) std::shared_ptr<T>(
              std::forward<Args>(args)...);
      }

      template<typename T, uint32_t Type, MemberType<T> Member, typename... Args>
      std::shared_ptr<T> &emplace_val(Args&&... args) {
        return emplace_shared_val<T, Type, Member> (std::move(
            std::make_shared<T> (
              std::forward<Args>(args)...)));
      }

      template<typename T, uint32_t Type, MemberType<std::vector<T>> Member,
               typename... Args>
      std::shared_ptr<std::vector<T>> &emplace_shared_vec(Args&&... args) {
        return emplace_shared_val<std::vector<T>, Type, Member> (
              std::forward<Args>(args)...);
      }

      template<typename T, uint32_t Type, MemberType<std::vector<T>> Member,
               typename... Args>
      std::shared_ptr<std::vector<T>> &emplace_vec(Args&&... args) {
        return emplace_val<std::vector<T>, Type, Member> (
              std::forward<Args>(args)...);
      }

    public:
      /**
       * Construct a shared_ptr to vector of integers within this
       * KnowledgeRecord.
       *
       * If the KnowledgeRecord would modify the resulting shared_ptr,
       * a private copy will be made, and modified.
       *
       * @param args All arguments are forwarded to the shared_ptr constructor
       **/
      template<typename... Args>
      void emplace_shared_integers(Args&&... args) {
        emplace_shared_vec<Integer, INTEGER_ARRAY,
               &KnowledgeRecord::int_array_> (
                  std::forward<Args>(args)...);
        shared_ = SHARED;
      }

      /**
       * Construct a vector of integers within this KnowledgeRecord.
       *
       * @param args All arguments are forwarded to the vector constructor
       **/
      template<typename... Args>
      void emplace_integers(Args&&... args) {
        emplace_vec<Integer, INTEGER_ARRAY,
               &KnowledgeRecord::int_array_> (
                  std::forward<Args>(args)...);
      }

      /**
       * Construct a shared_ptr to vector of doubles within this
       * KnowledgeRecord.
       *
       * If the KnowledgeRecord would modify the resulting shared_ptr,
       * a private copy will be made, and modified.
       *
       * @param args All arguments are forwarded to the shared_ptr constructor
       **/
      template<typename... Args>
      void emplace_shared_doubles(Args&&... args) {
        emplace_shared_vec<double, DOUBLE_ARRAY,
               &KnowledgeRecord::double_array_> (
                  std::forward<Args>(args)...);
        shared_ = SHARED;
      }

      /**
       * Construct a vector of doubles within this KnowledgeRecord.
       *
       * @param args All arguments are forwarded to the vector constructor
       **/
      template<typename... Args>
      void emplace_doubles(Args&&... args) {
        emplace_vec<double, DOUBLE_ARRAY,
               &KnowledgeRecord::double_array_> (
                  std::forward<Args>(args)...);
      }

      /**
       * Construct a shared_ptr to a string within this KnowledgeRecord.
       *
       * If the KnowledgeRecord would modify the resulting shared_ptr,
       * a private copy will be made, and modified.
       *
       * @param args All arguments are forwarded to the shared_ptr constructor
       **/
      template<typename... Args>
      void emplace_shared_string(Args&&... args) {
        emplace_shared_val<std::string, STRING,
               &KnowledgeRecord::str_value_> (
                  std::forward<Args>(args)...);
        shared_ = SHARED;
      }

      /**
       * Construct a string within this KnowledgeRecord.
       *
       * @param args All arguments are forwarded to the string constructor
       **/
      template<typename... Args>
      void emplace_string(Args&&... args) {
        emplace_val<std::string, STRING,
               &KnowledgeRecord::str_value_> (
                  std::forward<Args>(args)...);
      }

      /**
       * Construct a shared_ptr to a file (vector of unsigned char) within
       * this KnowledgeRecord.
       *
       * If the KnowledgeRecord would modify the resulting shared_ptr,
       * a private copy will be made, and modified.
       *
       * @param args All arguments are forwarded to the shared_ptr constructor
       **/
      template<typename... Args>
      void emplace_shared_file(Args&&... args) {
        emplace_shared_vec<unsigned char, UNKNOWN_FILE_TYPE,
               &KnowledgeRecord::file_value_> (
                std::forward<Args>(args)...);
        shared_ = SHARED;
      }

      /**
       * Construct a file (vector of unsigned char) within this KnowledgeRecord.
       *
       * @param args All arguments are forwarded to the vector constructor
       **/
      template<typename... Args>
      void emplace_file(Args&&... args) {
        emplace_vec<unsigned char, UNKNOWN_FILE_TYPE,
               &KnowledgeRecord::file_value_> (
                std::forward<Args>(args)...);
      }

      /**
       * Forwarding constructor for integer arrays
       * Each argument past the first will be forwarded to construct a
       * std::vector<Integer> in-place within the new record.
       **/
      template<typename... Args>
      KnowledgeRecord(tags::integers_t, Args&&... args)
        : int_array_ (std::make_shared<std::vector<Integer>> (
              std::forward<Args>(args)...)),
          type_ (INTEGER_ARRAY) {}

      /**
       * Forwarding constructor for double arrays
       * Each argument past the first will be forwarded to construct a
       * std::vector<double> in-place within the new record.
       **/
      template<typename... Args>
      KnowledgeRecord(tags::doubles_t, Args&&... args)
        : double_array_ (std::make_shared<std::vector<double>> (
              std::forward<Args>(args)...)),
          type_ (DOUBLE_ARRAY) {}

      /**
       * Forwarding constructor for strings
       * Each argument past the first will be forwarded to construct a
       * std::string in-place within the new record.
       **/
      template<typename... Args>
      KnowledgeRecord(tags::string_t, Args&&... args)
        : str_value_ (std::make_shared<std::string> (
              std::forward<Args>(args)...)),
          type_ (STRING) {}

      /**
       * Forwarding constructor for binary files (blobs)
       * Each argument past the first will be forwarded to construct a
       * std::vector<unsigned char> in-place within the new record.
       **/
      template<typename... Args>
      KnowledgeRecord(tags::binary_t, Args&&... args)
        : file_value_ (std::make_shared<std::vector<unsigned char>> (
              std::forward<Args>(args)...)),
          type_ (UNKNOWN_FILE_TYPE) {}

      /* default constructor */
      KnowledgeRecord () : KnowledgeRecord (*logger::global_logger.get()) {}

      explicit KnowledgeRecord (logger::Logger & logger);

      /* Integer constructor */
      template<typename T,
        typename std::enable_if<std::is_integral<T>::value,
        void*>::type = nullptr>
      explicit KnowledgeRecord (T value,
        logger::Logger & logger = *logger::global_logger.get ());

      /* Floating point constructor */
      template<typename T,
        typename std::enable_if<std::is_floating_point<T>::value,
        void*>::type = nullptr>
      explicit KnowledgeRecord (T value,
        logger::Logger & logger = *logger::global_logger.get ());

      /* Integer array constructor */
      explicit KnowledgeRecord (const std::vector <Integer> & value,
        logger::Logger & logger = *logger::global_logger.get ());

      /* Integer array move constructor */
      explicit KnowledgeRecord (std::vector <Integer> && value,
        logger::Logger & logger = *logger::global_logger.get ());

      /* Double array constructor */
      explicit KnowledgeRecord (const std::vector <double> & value,
        logger::Logger & logger = *logger::global_logger.get ());

      /* Double array move constructor */
      explicit KnowledgeRecord (std::vector <double> && value,
        logger::Logger & logger = *logger::global_logger.get ());

      /* String constructor */
      explicit KnowledgeRecord (const std::string & value,
        logger::Logger & logger = *logger::global_logger.get ());

      /* String move constructor */
      explicit KnowledgeRecord (std::string && value,
        logger::Logger & logger = *logger::global_logger.get ());

      /* Char pointer constructor for g++ */
      explicit KnowledgeRecord (const char * value,
        logger::Logger & logger = *logger::global_logger.get ());

      /* copy constructor */
      KnowledgeRecord (const KnowledgeRecord & rhs) noexcept;

      /* move constructor */
      KnowledgeRecord (KnowledgeRecord && rhs) noexcept;

      /* destructor */
      ~KnowledgeRecord () noexcept;

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
      KnowledgeRecord retrieve_index (size_t index) const;

      /**
       * decrements the value at the index to the specified value. If the
       * record was previously not an array or if the array is not
       * large enough, a new array is created.
       * @param    index   index of the value to set
       **/
      KnowledgeRecord dec_index (size_t index);

      /**
       * increments the value at the index to the specified value. If the
       * record was previously not an array or if the array is not
       * large enough, a new array is created.
       * @param    index   index of the value to set
       **/
      KnowledgeRecord inc_index (size_t index);

      /**
       * sets the value at the index to the specified value. If the
       * record was previously not an array or if the array is not
       * large enough, a new array is created.
       * @param    index   index of the value to set
       * @param    value   the value to set at the specified index
       **/
      template<typename T,
        typename std::enable_if<std::is_integral<T>::value,
        void*>::type = nullptr>
      void set_index (size_t index, T value);

      /**
       * sets the value at the index to the specified value. If the
       * record was previously not an array or if the array is not
       * large enough, a new array is created.
       * @param    index   index of the value to set
       * @param    value   the value to set at the specified index
       **/
      template<typename T,
        typename std::enable_if<std::is_floating_point<T>::value,
        void*>::type = nullptr>
      void set_index (size_t index, T value);

      /**
       * converts the value to a string.
       * @param   delimiter   characters to insert in between
       *                      elements of an array
       * @return  the value as a string
       **/
      std::string to_string (const std::string & delimiter = ", ") const;

      /**
       * Returns a shared_ptr, sharing with the internal one.
       * If this record is not a string, returns NULL shared_ptr
       **/
      std::shared_ptr<std::string> share_string() const;

      /**
       * Returns a shared_ptr, while resetting this record to empty.
       * If this record is not a string, returns NULL shared_ptr
       **/
      std::shared_ptr<std::string> take_string();

      /**
       * writes the value to a file
       * @param   filename     file location to write to
       **/
      ssize_t to_file (const std::string & filename) const;

      /**
       * If this record holds a shared_ptr, make a copy of the underlying
       * value so it has an exclusive copy.
       **/
      void unshare (void);

      /**
       * clones the record. Caller must ensure returned pointer is deleted.
       **/
      KnowledgeRecord * clone (void) const;

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
       * Returns a shared_ptr, sharing with the internal one.
       * If this record is not an int array, returns NULL shared_ptr
       **/
      std::shared_ptr<std::vector<Integer>> share_integers() const;

      /**
       * Returns a shared_ptr, while resetting this record to empty.
       * If this record is not an int array, returns NULL shared_ptr
       **/
      std::shared_ptr<std::vector<Integer>> take_integers();

      /**
       * converts the value to a vector of doubles
       * @return  a vector of doubles
       **/
      std::vector <double> to_doubles (void) const;

      /**
       * Returns a shared_ptr, sharing with the internal one.
       * If this record is not a doubles array, returns NULL shared_ptr
       **/
      std::shared_ptr<std::vector<double>> share_doubles() const;

      /**
       * Returns a shared_ptr, while resetting this record to empty.
       * If this record is not doubles array, returns NULL shared_ptr
       **/
      std::shared_ptr<std::vector<double>> take_doubles();

      /**
       * Returns a shared_ptr, sharing with the internal one.
       * If this record is not a binary file value, returns NULL shared_ptr
       **/
      std::shared_ptr<std::vector<unsigned char>> share_binary() const;

      /**
       * Returns a shared_ptr, while resetting this record to empty.
       * If this record is not a binary file value, returns NULL shared_ptr
       **/
      std::shared_ptr<std::vector<unsigned char>> take_binary();

      /**
       * returns an unmanaged buffer that the user will have
       * to take care of (this is a copy of the internal value).
       * If you use this function, you must explicitly delete the
       * value returned. For instance:
       *
       * char * my_value = record.to_unmanaged_buffer ();
       * ... do some work on the buffer
       * delete [] my_value;
       *
       * Failure to do the above WILL result in a memory leak
       * @param   size    size of the unmanaged buffer
       * @return  the unmanaged buffer of size bytes
       **/
      unsigned char * to_unmanaged_buffer (size_t & size) const;

      /**
       * Sets the value from another KnowledgeRecord,
       * does not copy clock and write_quality.
       * Will check that write_quality of new value is >= quality
       * of this record, and ignore update if not. Otherwise,
       * this records quality will be set to new_value.write_quality.
       *
       * @param    new_value   new value of the Knowledge Record
       * @param    size        num elements in the array
       **/
      void set_value (const KnowledgeRecord &new_value);

      /**
       * Sets the value from another KnowledgeRecord,
       * does not copy clock and write_quality.
       * Will check that write_quality of new value is >= quality
       * of this record, and ignore update if not. Otherwise,
       * this records quality will be set to new_value.write_quality.
       *
       * @param    new_value   new value of the Knowledge Record
       * @param    size        num elements in the array
       **/
      void set_value (KnowledgeRecord &&new_value);

      /**
       * sets the value to a double
       * @param    new_value   new value of the Knowledge Record
       **/
      template<typename T,
        typename std::enable_if<std::is_integral<T>::value,
        void*>::type = nullptr>
      void set_value (T new_value);

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
      void set_value (std::vector <Integer> && new_value);

      /**
       * sets the value to an array of integers
       * @param    new_value   new value of the Knowledge Record
       **/
      void set_value (const std::vector <Integer> & new_value);

      /**
       * sets the value to an array of integers, without copying
       * @param    new_value   new value of the Knowledge Record
       **/
      void set_value (std::shared_ptr<std::vector <Integer>> new_value);

      /**
       * sets the value to a string, from a buffer
       * @param    new_value   new value of the Knowledge Record
       * @param    size        num elements in the buffer
       **/
      void set_value (const char * new_value, uint32_t size);

      /**
       * sets the value to a string
       * @param    new_value   new value of the Knowledge Record
       **/
      void set_value (std::string && new_value);

      /**
       * sets the value to a string
       * @param    new_value   new value of the Knowledge Record
       **/
      void set_value (const std::string & new_value);

      /**
       * sets the value to a string. Does not copy the string.
       * @param    new_value   new value of the Knowledge Record
       **/
      void set_value (std::shared_ptr<std::string> new_value);

      /**
       * sets the value to a floating point number
       * @param    new_value   new value of the Knowledge Record
       **/
      template<typename T,
        typename std::enable_if<std::is_floating_point<T>::value,
        void*>::type = nullptr>
      void set_value (T new_value);

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
      void set_value (std::vector <double> && new_value);

      /**
       * sets the value to an array of doubles
       * @param    new_value   new value of the Knowledge Record
       **/
      void set_value (const std::vector <double> & new_value);

      /**
       * sets the value to an array of doubles, without copying
       * @param    new_value   new value of the Knowledge Record
       **/
      void set_value (std::shared_ptr<std::vector <double>> new_value);

      /**
       * sets the value to an xml string
       * @param    new_value   new value of the Knowledge Record
       * @param    size        size of the new_value buffer
       **/
      void set_xml (const char * new_value, size_t size);

      /**
       * sets the value to an xml string
       * @param    new_value   new value of the Knowledge Record
       **/
      void set_xml (std::string && new_value);

      /**
       * sets the value to an xml string
       * @param    new_value   new value of the Knowledge Record
       **/
      void set_xml (const std::string & new_value);

      /**
       * sets the value to an xml string. Does not copy the string.
       * @param    new_value   new value of the Knowledge Record
       **/
      void set_xml (std::shared_ptr<std::string> new_value);

      /**
       * sets the value to a plaintext string
       * @param    new_value   new value of the Knowledge Record
       * @param    size        size of the new_value buffer
       **/
      void set_text (const char * new_value, size_t size);

      /**
       * sets the value to a plaintext string
       * @param    new_value   new value of the Knowledge Record
       **/
      void set_text (std::string && new_value);

      /**
       * sets the value to a plaintext string
       * @param    new_value   new value of the Knowledge Record
       **/
      void set_text (const std::string & new_value);

      /**
       * sets the value to a plaintext string. Does not copy the string.
       * @param    new_value   new value of the Knowledge Record
       **/
      void set_text (std::shared_ptr<std::string> new_value);

      /**
       * Sets the double precision of a double record when using
       * to_string (). All doubles will have this fixed precision.
       * To return to default precision, set new_precision to a
       * negative number.
       * @param   new_precision  new double precision to use
       **/
      static void set_precision (int new_precision);

      /**
      * Sets the output format for doubles to std::scientific
      **/
      static void set_scientific (void);

      /**
      * Sets the output format for doubles to std::fixed
      **/
      static void set_fixed (void);

      /**
       * Gets the current double precision for double to string conversion
       * @return  the double precision for double to string conversion
       **/
      static int get_precision (void);

      /**
       * sets the value to a jpeg
       * @param    new_value   new value of the Knowledge Record
       * @param    size        size of the new_value buffer
       **/
      void set_jpeg (const unsigned char * new_value, size_t size);

      /**
       * sets the value to a jpeg
       * @param    new_value   new value of the Knowledge Record
       **/
      void set_jpeg (std::vector <unsigned char> && new_value);

      /**
       * sets the value to a jpeg
       * @param    new_value   new value of the Knowledge Record
       **/
      void set_jpeg (const std::vector <unsigned char> & new_value);

      /**
       * sets the value to a jpeg, without copying
       * @param    new_value   new value of the Knowledge Record
       **/
      void set_jpeg (std::shared_ptr<std::vector <unsigned char>> new_value);

      /**
       * sets the value to an unknown file type
       * @param    new_value   new value of the Knowledge Record
       * @param    size        size of the new_value buffer
       **/
      void set_file (const unsigned char * new_value, size_t size);

      /**
       * sets the value to an unknown file type
       * @param    new_value   new value of the Knowledge Record
       **/
      void set_file (std::vector <unsigned char> && new_value);

      /**
       * sets the value to an unknown file type
       * @param    new_value   new value of the Knowledge Record
       **/
      void set_file (const std::vector <unsigned char> & new_value);

      /**
       * sets the value to an unknown file type, without copying
       * @param    new_value   new value of the Knowledge Record
       **/
      void set_file (std::shared_ptr<std::vector <unsigned char>> new_value);

      /**
       * Creates a deep copy of the knowledge record. Because each
       * Knowledge Record may contain non-thread-safe ref counted values,
       * user threads that reference knowledge records will want to use
       * their own ref counts, which can be accomplished via this method.
       * @param    source   the Knowledge Record we wish to copy
       **/
      void deep_copy (const KnowledgeRecord & source);

      /**
       * Creates a deep copy of this knowledge record. Because each
       * Knowledge Record may contain non-thread-safe ref counted values,
       * user threads that reference knowledge records will want to use
       * their own ref counts, which can be accomplished via this method.
       * @return   a deep copy of this knowledge record
       **/
      KnowledgeRecord deep_copy() const;

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
      void reset_value (void) noexcept;

    private:
      void clear_union (void) noexcept;

    public:
      /**
       * clears any dynamic values. This method does not attempt to set
       * the value of the Knowledge Record, as clear does, and is consequently
       * more efficient.
       **/
      void clear_value (void) noexcept;

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
      static bool is_ref_counted (uint32_t type);

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
      static bool is_string_type (uint32_t type);

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
      static bool is_double_type (uint32_t type);

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
      static bool is_integer_type (uint32_t type);


      /**
       * returns if the record is an array type (DOUBLE_ARRAY, INTEGER_ARRAY)
       * @return   true if the record is an integer
       **/
      bool is_array_type (void) const;

      /**
       * returns if the record is a array type (DOUBLE_ARRAY, INTEGER_ARRAY)
       * @param   type the type to check
       * @return   true if the record is an integer
       **/
      static bool is_array_type (uint32_t type);

      /**
       * returns a record containing a fragment of the character buffer.
       * For strings, this is equivalent to substring. For files, this is
       * like an unsigned char * equivalent to substring. For other types,
       * this will return host-specific byte values up to the size of the
       * value.
       * @param    first          first index (inclusive, >= 0)
       * @param    last           last index (inclusive, < size)
       **/
      KnowledgeRecord fragment (unsigned int first, unsigned int last);

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
      static bool is_image_type (uint32_t type);

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
      static bool is_file_type (uint32_t type);

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
      static bool is_binary_file_type (uint32_t type);

      /**
       * Less than
       **/
      bool operator< (const KnowledgeRecord & rhs) const;

      /**
       * Less than or equal to
       **/
      bool operator<= (const KnowledgeRecord & rhs) const;

      /**
       * Equal to
       **/
      bool operator== (const KnowledgeRecord & rhs) const;

      /**
       * Equal to
       **/
      template<typename T,
        typename std::enable_if<std::is_integral<T>::value,
        void*>::type = nullptr>
      bool operator== (T value) const;

      /**
       * Equal to
       **/
      template<typename T,
        typename std::enable_if<std::is_floating_point<T>::value,
        void*>::type = nullptr>
      bool operator== (T value) const;

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
      bool operator!= (const KnowledgeRecord & rhs) const;

      /**
       * Greater than
       **/
      bool operator> (const KnowledgeRecord & rhs) const;

      /**
       * Greater than or equal to
       **/
      bool operator>= (const KnowledgeRecord & rhs) const;

      /**
       * Logical not.
       **/
      bool operator! (void) const;

      /**
       * Negate.
       */
      KnowledgeRecord operator- (void) const;

      /**
       * Assignment
       **/
      KnowledgeRecord & operator= (const KnowledgeRecord & rhs) noexcept;

      /**
       * Move Assignment
       **/
      KnowledgeRecord & operator= (KnowledgeRecord && rhs) noexcept;

      /**
       * In-place addition operator
       **/
      KnowledgeRecord & operator+= (const KnowledgeRecord & rhs);

      /**
       * In-place subtraction operator
       **/
      KnowledgeRecord & operator-= (const KnowledgeRecord & rhs);

      /**
       * In-place multiplication operator
       **/
      KnowledgeRecord & operator*= (const KnowledgeRecord & rhs);

      /**
       * In-place division operator
       **/
      KnowledgeRecord & operator/= (const KnowledgeRecord & rhs);

      /**
       * In-place modulus operator
       **/
      KnowledgeRecord & operator%= (const KnowledgeRecord & rhs);

      /**
       * Times operator
       **/
      KnowledgeRecord operator* (const KnowledgeRecord & rhs) const;

      /**
       * Divides operator
       **/
      KnowledgeRecord operator/ (const KnowledgeRecord & rhs) const;

      /**
       * Modulus operator
       **/
      KnowledgeRecord operator% (const KnowledgeRecord & rhs) const;

      /**
       * Plus operator
       **/
      KnowledgeRecord operator+ (const KnowledgeRecord & rhs) const;

      /**
       * Minus operator
       **/
      KnowledgeRecord operator- (const KnowledgeRecord & rhs) const;

      /**
       * Explicit bool cast
       *
       * @return the value of is_true()
       **/
      explicit operator bool (void) const;

      /**
       * Preincrement operator
       **/
      KnowledgeRecord & operator++ (void);

      /**
       * Predecrement operator
       **/
      KnowledgeRecord & operator-- (void);

      /**
      * Reads a KnowledgeRecord instance from a buffer and updates
      * the amount of buffer room remaining.
      * @param     buffer     the readable buffer where data is stored
      * @param     buffer_remaining  the count of bytes remaining in the
      *                              buffer to read
      * @return    current buffer position for next read
      **/
      const char * read (const char * buffer, int64_t & buffer_remaining);

      /**
        * Reads a KnowledgeRecord instance from a buffer and updates
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
      * Reads a KnowledgeRecord instance from a buffer and updates
      * the amount of buffer room remaining.
      * @param     buffer      the readable buffer where data is stored
      * @param     key_id      the keyed index for the name of a variable
      * @param     buffer_remaining  the count of bytes remaining in the
      *                              buffer to read
      * @return    current buffer position for next read
      **/
      const char * read (const char * buffer, uint32_t & key_id,
        int64_t & buffer_remaining);

      /**
      * Writes a KnowledgeRecord instance to a buffer and updates
      * the amount of buffer room remaining.
      *
      * Output Format:
      *
      * [type | value_size | value]<br />
      * type = 32 bit unsigned integer, the type of value <br />
      * size = 32 bit unsigned integer, number of elements of the type<br />
      * value = the stored value
      *
      * @param     buffer     the readable buffer where data is stored
      * @param     buffer_remaining  the count of bytes remaining in the
      *                              buffer to read
      * @return    current buffer position for next write
      **/
      char * write (char * buffer, int64_t & buffer_remaining) const;

      /**
        * Writes a KnowledgeRecord instance to a buffer and updates
        * the amount of buffer room remaining.
        *        
        * Output Format:
        *
        * [key_size | key | type | value_size | value]<br />
        * key_size = 32 bit unsigned integer, length of the key<br />
        * key = key_size characters, the ASCII key<br />
        * type = 32 bit unsigned integer, the type of value <br />
        * size = 32 bit unsigned integer, number of elements of the type<br />
        * value = the stored value
        * 
        * @param     buffer     the readable buffer where data is stored
        * @param     key        the name of the variable
        * @param     buffer_remaining  the count of bytes remaining in the
        *                              buffer to read
        * @return    current buffer position for next write
        **/
      char * write (char * buffer, const std::string & key,
                    int64_t & buffer_remaining) const;

      /**
      * Writes a KnowledgeRecord instance to a buffer and updates
      * the amount of buffer room remaining. This is a write method
      * intended for keyed indexes for variables, generally for
      * perfect hashes
      *
      * Output Format:
      *
      * [key_size | key | type | value_size | value]<br />
      * key_size = 32 bit unsigned integer, length of the key<br />
      * key = key_size characters, the ASCII key<br />
      * type = 32 bit unsigned integer, the type of value <br />
      * size = 32 bit unsigned integer, number of elements of the type<br />
      * value = the stored value
      *
      * @param     buffer     the readable buffer where data is stored
      * @param     key_id     the id of the variable
      * @param     buffer_remaining  the count of bytes remaining in the
      *                              buffer to read
      * @return    current buffer position for next write
      **/
      char * write (char * buffer, uint32_t key_id,
        int64_t & buffer_remaining) const;

      /**
       * Apply the knowledge record to a context, given some quality and clock
       **/
      int apply (madara::knowledge::ThreadSafeContext & context,
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
        return status() != UNCREATED;
      }

      /**
       * Returns the encoded size of the record. This size is what is required
       * to write the key_size, key, type, value size, and all associated
       * information in the read () and write () methods.
       **/
      int64_t get_encoded_size (const std::string & key) const;

      /**
      * Returns the encoded size of the record. This size is what is required
      * to write the type, value size, and all associated
      * information in the read () and write () methods.
      **/
      int64_t get_encoded_size (void) const;
    };

    typedef ::std::map < std::string, KnowledgeRecord>   KnowledgeMap;
    typedef ::std::vector < KnowledgeRecord>             KnowledgeVector;
    typedef ::std::vector < std::string>                  KnowledgeRules;
    typedef ::std::vector < std::string>                  StringVector;
    typedef ::std::map < std::string, KnowledgeRecord *> KnowledgeRecords;

    typedef std::string     KnowledgeKey;
    typedef KnowledgeRecord              KnowledgeValue;

    /**
     * Returns the maximum quality within the records
     * @param    records     the list of records to gauge quality of
     * @return   the maximum quality within the list of records
     **/
    uint32_t max_quality (const KnowledgeRecords & records);

    /**
     * Returns the maximum quality within the records
     * @param    records     the list of records to gauge quality of
     * @return   the maximum quality within the list of records
     **/
    uint32_t max_quality (const KnowledgeMap & records);
  }
}


/**
  * output stream buffering
  **/
//inline std::ostream & operator<< (std::ostream & stream,
  //const madara::knowledge::KnowledgeRecord & rhs);

#include "KnowledgeRecord.inl"


#endif  // _MADARA_KNOWLEDGE_RECORD_H_
