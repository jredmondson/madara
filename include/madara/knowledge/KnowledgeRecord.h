

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
#include "madara/MadaraExport.h"
#include "madara/utility/StlHelper.h"
#include "madara/utility/StdInt.h"
#include "madara/utility/Refcounter.h"
#include "madara/logger/GlobalLogger.h"
#include "madara/utility/IntTypes.h"
#include "madara/utility/CircularBuffer.h"
#include "madara/exceptions/IndexException.h"

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
namespace tags
{
using utility::type;

using integer_t = type<int64_t>;
constexpr integer_t integer;
constexpr integer_t int_;
using double_t = type<double>;
constexpr double_t double_;
constexpr double_t dbl;
using integers_t = type<std::vector<int64_t>>;
constexpr integers_t integers;
constexpr integers_t ints;
using doubles_t = type<std::vector<double>>;
constexpr doubles_t doubles;
constexpr doubles_t dbls;
using string_t = type<std::string>;
constexpr string_t string;
constexpr string_t str;
using binary_t = type<std::vector<unsigned char>>;
constexpr binary_t binary;

template<typename T>
struct any
{
  any() {}
};

template<typename T>
struct shared_t
{
  shared_t() {}
};

/// Used to signal in-place shared_ptr construction in KnowledgeRecord
/// Example: tags::shared(tags::integers)
template<typename T>
inline shared_t<T> shared(T)
{
  return shared_t<T>{};
}
}  // namespace tags

/**
 * @class KnowledgeRecord
 * @brief This class encapsulates an entry in a KnowledgeBase
 **/

class MADARA_EXPORT KnowledgeRecord
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

  enum ValueTypes : uint32_t
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
    ALL_PRIMITIVE_TYPES =
        INTEGER | STRING | DOUBLE | INTEGER_ARRAY | DOUBLE_ARRAY,
    ALL_FILE_TYPES = UNKNOWN_FILE_TYPE | XML | TEXT_FILE | IMAGE_JPEG,
    ALL_IMAGES = IMAGE_JPEG,
    ALL_TEXT_FORMATS = XML | TEXT_FILE | STRING,
    ALL_TYPES = ALL_PRIMITIVE_TYPES | ALL_FILE_TYPES,
    ALL_CLEARABLES = ALL_ARRAYS | ALL_TEXT_FORMATS | ALL_FILE_TYPES,
    BUFFER = (1UL << 31),
  };

  typedef int64_t Integer;

  using CircBuf = utility::CircularBuffer<KnowledgeRecord>;

private:
  /// the logger used for any internal debugging information
  logger::Logger* logger_ = logger::global_logger.get();

public:
  /**
   * last modification lamport clock time
   **/
  uint64_t clock = 0;

private:
  /**
   * last modification system clock time
   **/
  uint64_t toi_ = 0;

public:
  uint64_t toi() const
  {
    return toi_;
  }
  void set_toi(uint64_t new_toi)
  {
    toi_ = new_toi;
    if (has_history())
    {
      buf_->back().set_toi(new_toi);
    }
  }

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
  union {
    Integer int_value_ = 0;
    double double_value_;

    std::shared_ptr<std::vector<Integer>> int_array_;
    std::shared_ptr<std::vector<double>> double_array_;
    std::shared_ptr<std::string> str_value_;
    std::shared_ptr<std::vector<unsigned char>> file_value_;

    std::shared_ptr<CircBuf> buf_;
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

public:
  /* default constructor */
  KnowledgeRecord() noexcept : KnowledgeRecord(*logger::global_logger.get()) {}

  explicit KnowledgeRecord(logger::Logger& logger) noexcept;

  /* Integer constructor */
  template<typename T,
      utility::enable_if_<utility::is_int_numeric<T>(), int> = 0>
  explicit KnowledgeRecord(
      T value, logger::Logger& logger = *logger::global_logger.get()) noexcept;

  /* Floating point constructor */
  template<typename T, typename std::enable_if<std::is_floating_point<T>::value,
                           void*>::type = nullptr>
  explicit KnowledgeRecord(
      T value, logger::Logger& logger = *logger::global_logger.get()) noexcept;

  /* Integer array constructor */
  explicit KnowledgeRecord(const std::vector<Integer>& value,
      logger::Logger& logger = *logger::global_logger.get());

  /* Integer array move constructor */
  explicit KnowledgeRecord(std::vector<Integer>&& value,
      logger::Logger& logger = *logger::global_logger.get()) noexcept;

  /* Integer array unique_ptr constructor */
  explicit KnowledgeRecord(std::unique_ptr<std::vector<Integer>> value,
      logger::Logger& logger = *logger::global_logger.get()) noexcept;

  /* Double array constructor */
  explicit KnowledgeRecord(const std::vector<double>& value,
      logger::Logger& logger = *logger::global_logger.get());

  /* Double array move constructor */
  explicit KnowledgeRecord(std::vector<double>&& value,
      logger::Logger& logger = *logger::global_logger.get()) noexcept;

  /* Double array unique_ptr constructor */
  explicit KnowledgeRecord(std::unique_ptr<std::vector<double>> value,
      logger::Logger& logger = *logger::global_logger.get()) noexcept;

  /* String constructor */
  KnowledgeRecord(const std::string& value,
      logger::Logger& logger = *logger::global_logger.get());

  /* String move constructor */
  explicit KnowledgeRecord(std::string&& value,
      logger::Logger& logger = *logger::global_logger.get()) noexcept;

  /* Double array unique_ptr constructor */
  explicit KnowledgeRecord(std::unique_ptr<std::string> value,
      logger::Logger& logger = *logger::global_logger.get()) noexcept;

  /* Char pointer constructor for g++ */
  explicit KnowledgeRecord(
      const char* value, logger::Logger& logger = *logger::global_logger.get());

  /* Binary file unique_ptr constructor */
  explicit KnowledgeRecord(std::unique_ptr<std::vector<unsigned char>> value,
      logger::Logger& logger = *logger::global_logger.get()) noexcept;

  /**
   * Construct using CircularBuffer directly. This buffer will be treated
   * as the history of this record, and used as such going forward.
   *
   * @param buffer buffer that will be copied into this record
   * @param logger the logger to use for logging
   **/
  explicit KnowledgeRecord(const CircBuf& buffer,
      logger::Logger& logger = *logger::global_logger.get());

  /**
   * Construct using CircularBuffer directly. This buffer will be treated
   * as the history of this record, and used as such going forward.
   *
   * @param buffer buffer that will be copied into this record
   * @param logger the logger to use for logging
   **/
  explicit KnowledgeRecord(CircBuf&& buffer,
      logger::Logger& logger = *logger::global_logger.get()) noexcept;

  /**
   * copy constructor
   * @param rhs   value to copy
   */
  KnowledgeRecord(const KnowledgeRecord& rhs);

  /**
   * move constructor
   * @param rhs   value to copy
   **/
  KnowledgeRecord(KnowledgeRecord&& rhs) noexcept;

  /* destructor */
  ~KnowledgeRecord() noexcept;

public:
  /**
   * Construct a vector of integers within this KnowledgeRecord.
   *
   * @param args All arguments are forwarded to the vector constructor
   **/
  template<typename... Args>
  void emplace_integers(Args&&... args)
  {
    emplace_vec<Integer, INTEGER_ARRAY, &KnowledgeRecord::int_array_>(
        std::forward<Args>(args)...);
  }

  /**
   * Construct a vector of integers within this KnowledgeRecord.
   *
   * @param args arguments forwarded to the vector constructor
   **/
  template<typename... Args>
  void emplace(tags::integers_t, Args&&... args)
  {
    emplace_integers(std::forward<Args>(args)...);
  }

  /**
   * Construct a vector of doubles within this KnowledgeRecord.
   *
   * @param args All arguments are forwarded to the vector constructor
   **/
  template<typename... Args>
  void emplace_doubles(Args&&... args)
  {
    emplace_vec<double, DOUBLE_ARRAY, &KnowledgeRecord::double_array_>(
        std::forward<Args>(args)...);
  }

  /**
   * Construct a vector of doubles within this KnowledgeRecord.
   *
   * @param args arguments forwarded to the vector constructor
   **/
  template<typename... Args>
  void emplace(tags::doubles_t, Args&&... args)
  {
    emplace_doubles(std::forward<Args>(args)...);
  }

  /**
   * Construct a string within this KnowledgeRecord.
   *
   * @param args All arguments are forwarded to the string constructor
   **/
  template<typename... Args>
  void emplace_string(Args&&... args)
  {
    emplace_val<std::string, STRING, &KnowledgeRecord::str_value_>(
        std::forward<Args>(args)...);
  }

  /**
   * Construct a string within this KnowledgeRecord.
   *
   * @param args arguments forwarded to the string constructor
   **/
  template<typename... Args>
  void emplace(tags::string_t, Args&&... args)
  {
    emplace_string(std::forward<Args>(args)...);
  }

  /**
   * Construct a file (vector of unsigned char) within this KnowledgeRecord.
   *
   * @param args All arguments are forwarded to the vector constructor
   **/
  template<typename... Args>
  void emplace_file(Args&&... args)
  {
    emplace_vec<unsigned char, UNKNOWN_FILE_TYPE,
        &KnowledgeRecord::file_value_>(std::forward<Args>(args)...);
  }

  /**
   * Construct a binary (vector of unsigned char) within this KnowledgeRecord.
   *
   * @param args arguments forwarded to the vector constructor
   **/
  template<typename... Args>
  void emplace(tags::binary_t, Args&&... args)
  {
    emplace_binary(std::forward<Args>(args)...);
  }

  /**
   * Construct a CircularBuffer within this KnowledgeRecord directly. This
   * buffer will be treated as the history of this record, and used as such
   * going forward.
   *
   * @param args arguments forwarded to the
   *               madara::utility::CircularBuffer constructor
   **/
  template<typename... Args>
  void overwrite_circular_buffer(Args&&... args)
  {
    emplace_val<CircBuf, BUFFER, &KnowledgeRecord::buf_, true>(
        std::forward<Args>(args)...);
  }

  /**
   * Forwarding constructor for integer arrays
   * Each argument past the first will be forwarded to construct a
   * std::vector<Integer> in-place within the new record.
   **/
  template<typename... Args>
  KnowledgeRecord(tags::integers_t, Args&&... args)
    : int_array_(
          std::make_shared<std::vector<Integer>>(std::forward<Args>(args)...)),
      type_(INTEGER_ARRAY)
  {
  }

  /**
   * Forwarding constructor for double arrays
   * Each argument past the first will be forwarded to construct a
   * std::vector<double> in-place within the new record.
   **/
  template<typename... Args>
  KnowledgeRecord(tags::doubles_t, Args&&... args)
    : double_array_(
          std::make_shared<std::vector<double>>(std::forward<Args>(args)...)),
      type_(DOUBLE_ARRAY)
  {
  }

  /**
   * Forwarding constructor for strings
   * Each argument past the first will be forwarded to construct a
   * std::string in-place within the new record.
   *
   * For example:
   * KnowledgeRecord rec (tags::string, "Hello World");
   **/
  template<typename... Args>
  KnowledgeRecord(tags::string_t, Args&&... args)
    : str_value_(std::make_shared<std::string>(std::forward<Args>(args)...)),
      type_(STRING)
  {
  }

  /**
   * Forwarding constructor for binary files (blobs)
   * Each argument past the first will be forwarded to construct a
   * std::vector<unsigned char> in-place within the new record.
   **/
  template<typename... Args>
  KnowledgeRecord(tags::binary_t, Args&&... args)
    : file_value_(std::make_shared<std::vector<unsigned char>>(
          std::forward<Args>(args)...)),
      type_(UNKNOWN_FILE_TYPE)
  {
  }

  /**
   * Checks if record exists (i.e., is not uncreated)
   * @return true if record exists, false otherwise
   **/
  bool exists(void) const;

  /**
   * retrieves the value at an array index. If the
   * knowledge record is not an array, false is returned.
   * @param    index    index of the array item
   * @return   value at the index
   **/
  KnowledgeRecord retrieve_index(size_t index) const;

  /**
   * decrements the value at the index to the specified value. If the
   * record was previously not an array or if the array is not
   * large enough, a new array is created.
   * @param    index   index of the value to set
   **/
  KnowledgeRecord dec_index(size_t index);

  /**
   * increments the value at the index to the specified value. If the
   * record was previously not an array or if the array is not
   * large enough, a new array is created.
   * @param    index   index of the value to set
   **/
  KnowledgeRecord inc_index(size_t index);

  /**
   * sets the value at the index to the specified value. If the
   * record was previously not an array or if the array is not
   * large enough, a new array is created.
   * @param    index   index of the value to set
   * @param    value   the value to set at the specified index
   **/
  template<typename T,
      utility::enable_if_<utility::is_int_numeric<T>(), int> = 0>
  void set_index(size_t index, T value);

  /**
   * sets the value at the index to the specified value. If the
   * record was previously not an array or if the array is not
   * large enough, a new array is created.
   * @param    index   index of the value to set
   * @param    value   the value to set at the specified index
   **/
  template<typename T, typename std::enable_if<std::is_floating_point<T>::value,
                           void*>::type = nullptr>
  void set_index(size_t index, T value);

  /**
   * converts the value to a string.
   * @param   delimiter   characters to insert in between
   *                      elements of an array
   * @return  the value as a string
   **/
  std::string to_string(const std::string& delimiter = ", ") const;

  /**
   * converts the value to a c string. Similar to unmanaged buffer except
   * the user is expected to have malloc'd, new'd, etc. the buffer beforehand.
   * Of note, this method is intended to provide an exact copy of a record's
   * contents and not the string equivalent. In other words, this will not
   * be a null-delimited string. This will be a raw string with no null ending
   * unless a 0 was an actual element of the character buffer.
   * @param   buffer     the user-managed character buffer to fill
   * @param   buf_size   the character buffer max size
   * @return  the number of characters placed in buffer
   **/
  size_t to_managed_buffer(char* buffer, size_t buf_size) const;
  
  /**
   * converts the value to a c string. This is the equivalent to calling
   * to_string and then copying it to the buffer. It just doesn't cause
   * exceptions with STL errors (e.g., in Unreal Engine plugins)
   * @param   buffer     the user-managed character buffer to fill
   * @param   buf_size   the character buffer max size
   * @return  the number of characters placed in buffer
   **/
  size_t to_managed_string(char* buffer, size_t buf_size) const;

  /**
   * @return a shared_ptr, sharing with the internal one.
   * If this record is not a string, returns NULL shared_ptr
   **/
  std::shared_ptr<const std::string> share_string() const;

  /**
   * writes the value to a file
   * @param   filename     file location to write to
   **/
  ssize_t to_file(const std::string& filename) const;

  /**
   * If this record holds a shared_ptr, make a copy of the underlying
   * value so it has an exclusive copy.
   **/
  void unshare(void);

  /**
   * clones the record. Caller must ensure returned pointer is deleted.
   **/
  KnowledgeRecord* clone(void) const;

  /**
   * converts the value to an integer. This operation tries to convert
   * as best as it can using reasonable conversions. For doubles and
   * integers, it uses the scalar conversion to integer (e.g., floor
   * for double). For int/double arrays, it will use the same scalar conversion
   * of the first element. For strings, it will try to convert with
   * a stringstream.
   **/
  Integer to_integer(void) const;

  /**
   * converts the value to a float/double. This operation tries to convert
   * as best as it can using reasonable conversions. For doubles and
   * integers, it uses the scalar conversion to double.
   * For int/double arrays, it will use the same scalar conversion
   * of the first element. For strings, it will try to convert with
   * a stringstream.
   **/
  double to_double(void) const;

  /**
   * converts the value to a vector of integers
   * @return  a vector of integers
   **/
  std::vector<Integer> to_integers(void) const;

  /**
   * @return a shared_ptr, sharing with the internal one.
   * If this record is not an int array, returns NULL shared_ptr
   **/
  std::shared_ptr<const std::vector<Integer>> share_integers() const;

  /**
   * converts the value to a vector of doubles
   * @return  a vector of doubles
   **/
  std::vector<double> to_doubles(void) const;

  /**
   * @return a shared_ptr, sharing with the internal one.
   * If this record is not a doubles array, returns NULL shared_ptr
   **/
  std::shared_ptr<const std::vector<double>> share_doubles() const;

  /**
   * @return a shared_ptr, sharing with the internal one.
   * If this record is not a binary file value, returns NULL shared_ptr
   **/
  std::shared_ptr<const std::vector<unsigned char>> share_binary() const;

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
  unsigned char* to_unmanaged_buffer(size_t& size) const;

  /**
   * Sets the value from another KnowledgeRecord,
   * does not copy toi, clock, and write_quality.
   *
   * @param    new_value   new value of the Knowledge Record
   **/
  void set_value(const KnowledgeRecord& new_value);

  /**
   * Sets the value from another KnowledgeRecord,
   * does not copy toi, clock, and write_quality.
   *
   * @param    new_value   new value of the Knowledge Record
   **/
  void set_value(KnowledgeRecord&& new_value);

  /**
   * Sets the value and meta data from another KnowledgeRecord.
   *
   * @param    new_value   new value of the Knowledge Record
   **/
  void set_full(const KnowledgeRecord& new_value);

  /**
   * Sets the value and meta data from another KnowledgeRecord.
   *
   * @param    new_value   new value of the Knowledge Record
   **/
  void set_full(KnowledgeRecord&& new_value);

  /**
   * sets the value to an integer
   * @param    new_value   new value of the Knowledge Record
   **/
  void set_value(Integer new_value);

  /**
   * sets the value to an integer
   * @param    new_value   new value of the Knowledge Record
   **/
  void set_value(int new_value);

  /**
   * sets the value to an integer
   * @param    new_value   new value of the Knowledge Record
   **/
  void set_value(size_t new_value);

  /**
   * sets the value to an array of integers
   * @param    new_value   new value of the Knowledge Record
   * @param    size        num elements in the array
   **/
  void set_value(const Integer* new_value, uint32_t size);

  /**
   * sets the value to an array of integers
   * @param    new_value   new value of the Knowledge Record
   **/
  void set_value(std::vector<Integer>&& new_value);

  /**
   * sets the value to an array of integers
   * @param    new_value   new value of the Knowledge Record
   **/
  void set_value(const std::vector<Integer>& new_value);

  /**
   * sets the value to an array of integers, without copying
   * @param    new_value   new value of the Knowledge Record
   **/
  void set_value(std::unique_ptr<std::vector<Integer>> new_value);

  /**
   * sets the value to a string, from a buffer
   * @param    new_value   new value of the Knowledge Record
   * @param    size        num elements in the buffer
   **/
  void set_value(const char* new_value, uint32_t size);

  /**
   * sets the value to a string
   * @param    new_value   new value of the Knowledge Record
   **/
  void set_value(std::string&& new_value);

  /**
   * sets the value to a string
   * @param    new_value   new value of the Knowledge Record
   **/
  void set_value(const std::string& new_value);

  /**
   * sets the value to a string. Does not copy the string.
   * @param    new_value   new value of the Knowledge Record
   **/
  void set_value(std::unique_ptr<std::string> new_value);

  /**
   * sets the value to a double
   * @param    new_value   new value of the Knowledge Record
   **/
  void set_value(double new_value);

  /**
   * sets the value to a float
   * @param    new_value   new value of the Knowledge Record
   **/
  void set_value(float new_value);

  /**
   * sets the value to an array of doubles
   * @param    new_value   new value of the Knowledge Record
   * @param    size        num elements in the array
   **/
  void set_value(const double* new_value, uint32_t size);

  /**
   * sets the value to an array of doubles
   * @param    new_value   new value of the Knowledge Record
   **/
  void set_value(std::vector<double>&& new_value);

  /**
   * sets the value to an array of doubles
   * @param    new_value   new value of the Knowledge Record
   **/
  void set_value(const std::vector<double>& new_value);

  /**
   * sets the value to an array of doubles, without copying
   * @param    new_value   new value of the Knowledge Record
   **/
  void set_value(std::unique_ptr<std::vector<double>> new_value);

  /**
   * sets the value to an xml string
   * @param    new_value   new value of the Knowledge Record
   * @param    size        size of the new_value buffer
   **/
  void set_xml(const char* new_value, size_t size);

  /**
   * sets the value to an xml string
   * @param    new_value   new value of the Knowledge Record
   **/
  void set_xml(std::string&& new_value);

  /**
   * sets the value to an xml string
   * @param    new_value   new value of the Knowledge Record
   **/
  void set_xml(const std::string& new_value);

  /**
   * sets the value to an xml string. Does not copy the string.
   * @param    new_value   new value of the Knowledge Record
   **/
  void set_xml(std::unique_ptr<std::string> new_value);

  /**
   * sets the value to a plaintext string
   * @param    new_value   new value of the Knowledge Record
   * @param    size        size of the new_value buffer
   **/
  void set_text(const char* new_value, size_t size);

  /**
   * sets the value to a plaintext string
   * @param    new_value   new value of the Knowledge Record
   **/
  void set_text(std::string&& new_value);

  /**
   * sets the value to a plaintext string
   * @param    new_value   new value of the Knowledge Record
   **/
  void set_text(const std::string& new_value);

  /**
   * sets the value to a plaintext string. Does not copy the string.
   * @param    new_value   new value of the Knowledge Record
   **/
  void set_text(std::unique_ptr<std::string> new_value);

  /**
   * Sets the double precision of a double record when using
   * to_string (). All doubles will have this fixed precision.
   * To return to default precision, set new_precision to a
   * negative number.
   * @param   new_precision  new double precision to use
   **/
  static void set_precision(int new_precision);

  /**
   * Sets the output format for doubles to std::scientific
   **/
  static void set_scientific(void);

  /**
   * Sets the output format for doubles to std::fixed
   **/
  static void set_fixed(void);

  /**
   * Gets the current double precision for double to string conversion
   * @return  the double precision for double to string conversion
   **/
  static int get_precision(void);

  /**
   * sets the value to a jpeg
   * @param    new_value   new value of the Knowledge Record
   * @param    size        size of the new_value buffer
   **/
  void set_jpeg(const unsigned char* new_value, size_t size);

  /**
   * sets the value to a jpeg, without copying
   * @param    new_value   new value of the Knowledge Record
   **/
  void set_jpeg(std::vector<unsigned char>&& new_value);

  /**
   * sets the value to a jpeg
   * @param    new_value   new value of the Knowledge Record
   **/
  void set_jpeg(const std::vector<unsigned char>& new_value);

  /**
   * sets the value to a jpeg, without copying
   * @param    new_value   new value of the Knowledge Record
   **/
  void set_jpeg(std::unique_ptr<std::vector<unsigned char>> new_value);

  /**
   * sets the value to an unknown file type
   * @param    new_value   new value of the Knowledge Record
   * @param    size        size of the new_value buffer
   **/
  void set_file(const unsigned char* new_value, size_t size);

  /**
   * sets the value to an unknown file type, without copying
   * @param    new_value   new value of the Knowledge Record
   **/
  void set_file(std::vector<unsigned char>&& new_value);

  /**
   * sets the value to an unknown file type
   * @param    new_value   new value of the Knowledge Record
   **/
  void set_file(const std::vector<unsigned char>& new_value);

  /**
   * sets the value to an unknown file type, without copying
   * @param    new_value   new value of the Knowledge Record
   **/
  void set_file(std::unique_ptr<std::vector<unsigned char>> new_value);

  /**
   * Creates a deep copy of the knowledge record. Because each
   * Knowledge Record may contain non-thread-safe ref counted values,
   * user threads that reference knowledge records will want to use
   * their own ref counts, which can be accomplished via this method.
   * @param    source   the Knowledge Record we wish to copy
   **/
  void deep_copy(const KnowledgeRecord& source);

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
  int status(void) const;

  /**
   * sets the status to modified
   **/
  void set_modified(void);

  /**
   * resizes an array to a new size
   * @param  new_size  new size of the array
   **/
  void resize(size_t new_size);

  /**
   * reads a file and sets the type appropriately according to
   * the extension
   * @param   filename         location of the file to read from
   * @param   read_as_type     force a type (XML, TEXT, IMAGE_JPEG, or
   *                           UNKNOWN_FILE_TYPE)
   * @return  size of file
   **/
  int read_file(const std::string& filename, uint32_t read_as_type = 0);

  /**
   * resets the variable to an integer
   **/
  void reset_value(void) noexcept;

private:
  void clear_union(void) noexcept;

public:
  /**
   * clears any dynamic values. This method does not attempt to set
   * the value of the Knowledge Record, as clear does, and is consequently
   * more efficient.
   **/
  void clear_value(void) noexcept;

  /**
   * returns the size of the value
   **/
  uint32_t size(void) const;

  /**
   * returns the type of the value
   **/
  uint32_t type(void) const;

  /**
   * Modify the type, but only if it's compatible with current type without
   * changing any actual data stored. I.e.,
   * STRING, TEXT_FILE, and XML can be converted between each other, and
   * UKNOWN_BINARY_FILE, IMAGE_JPEG can be converted between each other.
   *
   * @param type the new type
   *
   * @return true if conversion successfully applied, or was already type,
   *         false if not
   **/
  bool set_type(uint32_t type);

  /**
   * returns if the record has a reference-counted type
   * @return   true if the record is reference-counted
   **/
  bool is_ref_counted(void) const;

  /**
   * returns if the record has a reference-counted type
   * @param   type the type to check
   * @return   true if the record is reference-counted
   **/
  static bool is_ref_counted(uint32_t type);

  /**
   * returns true if the record is a string type (STRING, XML, TEXT_FILE)
   * @return   true if the record is a string
   **/
  bool is_string_type(void) const;

  /**
   * returns if the record is a string type (STRING, XML, TEXT_FILE)
   * @param   type the type to check
   * @return   true if the record is a string
   **/
  static bool is_string_type(uint32_t type);

  /**
   * returns if the record is a double type (DOUBLE, DOUBLE_ARRAY)
   * @return   true if the record is a double
   **/
  bool is_double_type(void) const;

  /**
   * returns if the record is a double type (DOUBLE, DOUBLE_ARRAY)
   * @param   type the type to check
   * @return   true if the record is a double
   **/
  static bool is_double_type(uint32_t type);

  /**
   * returns if the record is a integer type (INTEGER, INTEGER_ARRAY)
   * @return   true if the record is an integer
   **/
  bool is_integer_type(void) const;

  /**
   * returns if the record is a integer type (INTEGER, INTEGER_ARRAY)
   * @param   type the type to check
   * @return   true if the record is an integer
   **/
  static bool is_integer_type(uint32_t type);

  /**
   * returns if the record is an array type (DOUBLE_ARRAY, INTEGER_ARRAY)
   * @return   true if the record is an integer
   **/
  bool is_array_type(void) const;

  /**
   * returns if the record is a array type (DOUBLE_ARRAY, INTEGER_ARRAY)
   * @param   type the type to check
   * @return   true if the record is an integer
   **/
  static bool is_array_type(uint32_t type);

  /**
   * returns a record containing a fragment of the character buffer.
   * For strings, this is equivalent to substring. For files, this is
   * like an unsigned char * equivalent to substring. For other types,
   * this will return host-specific byte values up to the size of the
   * value.
   * @param    first          first index (inclusive, >= 0)
   * @param    last           last index (inclusive, < size)
   **/
  KnowledgeRecord fragment(unsigned int first, unsigned int last);

  /**
   * returns true if the knowledge record has an image type
   * @return    true if type is an image type
   **/
  bool is_image_type(void) const;

  /**
   * returns true if the knowledge record has an image type
   * @param     type   the type of the record
   * @return    true if type is an image type
   **/
  static bool is_image_type(uint32_t type);

  /**
   * returns true if the knowledge record has a file type
   * @return    true if type is a file type
   **/
  bool is_file_type(void) const;

  /**
   * returns true if the knowledge record has a file type
   * @param     type   the type of the record
   * @return    true if type is a file type
   **/
  static bool is_file_type(uint32_t type);

  /**
   * returns true if the knowledge record has a binary file type
   * @return    true if type is a file type
   **/
  bool is_binary_file_type(void) const;

  /**
   * returns true if the knowledge record has a binary file type
   * @param     type   the type of the record
   * @return    true if type is a file type
   **/
  static bool is_binary_file_type(uint32_t type);

  /**
   * Less than
   **/
  bool operator<(const KnowledgeRecord& rhs) const;

  /**
   * Less than or equal to
   **/
  bool operator<=(const KnowledgeRecord& rhs) const;

  /**
   * Equal to
   **/
  bool operator==(const KnowledgeRecord& rhs) const;

  /**
   * Unequal to
   **/
  bool operator!=(const KnowledgeRecord& rhs) const;

  /**
   * Greater than
   **/
  bool operator>(const KnowledgeRecord& rhs) const;

  /**
   * Greater than or equal to
   **/
  bool operator>=(const KnowledgeRecord& rhs) const;

  /**
   * Logical not.
   **/
  bool operator!(void) const;

  /**
   * Negate.
   */
  KnowledgeRecord operator-(void) const;

  /**
   * Assignment
   **/
  KnowledgeRecord& operator=(const KnowledgeRecord& rhs);

  /**
   * Move Assignment
   **/
  KnowledgeRecord& operator=(KnowledgeRecord&& rhs) noexcept;

  /**
   * Assigns a convertible value to the knowledge record
   **/
  template<typename T>
  auto operator=(T&& t) ->
      typename std::enable_if<!std::is_convertible<T, KnowledgeRecord>::value,
          decltype(this->set_value(std::forward<T>(t)), *this)>::type
  {
    this->set_value(std::forward<T>(t));
    return *this;
  }

  /**
   * In-place addition operator
   **/
  KnowledgeRecord& operator+=(const KnowledgeRecord& rhs);

  /**
   * In-place subtraction operator
   **/
  KnowledgeRecord& operator-=(const KnowledgeRecord& rhs);

  /**
   * In-place multiplication operator
   **/
  KnowledgeRecord& operator*=(const KnowledgeRecord& rhs);

  /**
   * In-place division operator
   **/
  KnowledgeRecord& operator/=(const KnowledgeRecord& rhs);

  /**
   * In-place modulus operator
   **/
  KnowledgeRecord& operator%=(const KnowledgeRecord& rhs);

  /**
   * Times operator
   **/
  KnowledgeRecord operator*(const KnowledgeRecord& rhs) const;

  /**
   * Divides operator
   **/
  KnowledgeRecord operator/(const KnowledgeRecord& rhs) const;

  /**
   * Modulus operator
   **/
  KnowledgeRecord operator%(const KnowledgeRecord& rhs) const;

  /**
   * Plus operator
   **/
  KnowledgeRecord operator+(const KnowledgeRecord& rhs) const;

  /**
   * Minus operator
   **/
  KnowledgeRecord operator-(const KnowledgeRecord& rhs) const;

  /**
   * Explicit bool cast
   *
   * @return the value of is_true()
   **/
  explicit operator bool(void) const;

  /**
   * Preincrement operator
   **/
  KnowledgeRecord& operator++(void);

  /**
   * Predecrement operator
   **/
  KnowledgeRecord& operator--(void);

  /**
   * Reads a KnowledgeRecord instance from a buffer and updates
   * the amount of buffer room remaining.
   * @param     buffer     the readable buffer where data is stored
   * @param     buffer_remaining  the count of bytes remaining in the
   *                              buffer to read
   * @return    current buffer position for next read
   **/
  const char* read(const char* buffer, int64_t& buffer_remaining);

  /**
   * Reads a KnowledgeRecord instance from a buffer and updates
   * the amount of buffer room remaining.
   * @param     buffer     the readable buffer where data is stored
   * @param     key        the name of the variable
   * @param     buffer_remaining  the count of bytes remaining in the
   *                              buffer to read
   * @return    current buffer position for next read
   **/
  const char* read(
      const char* buffer, std::string& key, int64_t& buffer_remaining);

  /**
   * Reads a KnowledgeRecord instance from a buffer and updates
   * the amount of buffer room remaining.
   * @param     buffer      the readable buffer where data is stored
   * @param     key_id      the keyed index for the name of a variable
   * @param     buffer_remaining  the count of bytes remaining in the
   *                              buffer to read
   * @return    current buffer position for next read
   **/
  const char* read(
      const char* buffer, uint32_t& key_id, int64_t& buffer_remaining);

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
  char* write(char* buffer, int64_t& buffer_remaining) const;

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
  char* write(
      char* buffer, const std::string& key, int64_t& buffer_remaining) const;

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
  char* write(char* buffer, uint32_t key_id, int64_t& buffer_remaining) const;

  /**
   * Apply the knowledge record to a context, given some quality and clock
   **/
  int apply(madara::knowledge::ThreadSafeContext& context,
      const std::string& key, unsigned int quality, uint64_t clock,
      bool perform_lock);

  /**
   * Checks to see if the record is true. For integers and doubles, true
   * means the value is non-zero. For strings, it means the value is non "";
   * @return  true if the record is non-zero or not-"". False otherwise.
   **/
  bool is_true(void) const;

  /**
   * Checks to see if the record is false. For integers and boules, false
   * means the value is zero. For strings, it means the value is "".
   * @return  true if the record is zero or "". False otherwise.
   **/
  bool is_false(void) const;

  /**
   * Checks to see if the record is valid. Valid records have a status
   * that is not UNCREATED.
   * @return  true if the record is valid. False otherwise.
   **/
  inline bool is_valid(void) const
  {
    return status() != UNCREATED;
  }

  /**
   * Returns the encoded size of the record. This size is what is required
   * to write the key_size, key, type, value size, and all associated
   * information in the read () and write () methods.
   **/
  int64_t get_encoded_size(const std::string& key) const;

  /**
   * Returns the encoded size of the record. This size is what is required
   * to write the type, value size, and all associated
   * information in the read () and write () methods.
   **/
  int64_t get_encoded_size(void) const;

  /**
   * Return true if this record has a circular buffer history. Use
   * set_history_capacity to add a buffer
   **/
  bool has_history() const
  {
    return type_ == BUFFER;
  }

  /**
   * Return the amount of history this record holds.
   **/
  size_t get_history_size() const
  {
    if (type_ == BUFFER)
    {
      return buf_->size();
    }

    if (exists())
    {
      return 1;
    }

    return 0;
  }

  /**
   * Return the maximum amount of history this record can hold. Use
   * set_history_capacity to adjust this.
   **/
  size_t get_history_capacity() const
  {
    if (type_ == BUFFER)
    {
      return buf_->capacity();
    }
    else
    {
      return 0;
    }
  }

  /**
   * Set the capacity of this record's history circular buffer. Every
   * modification to this record will write a new entry in this history.
   * Once the capacity is met, the oldest entry will be discarded as new
   * entries are added.
   *
   * Note that this capacity includes current value of the record. A capacity
   * of zero indicates that this record holds no buffer, whereas one indicates
   * that this record has a buffer of size 1.
   **/
  void set_history_capacity(size_t size)
  {
    if (type_ == BUFFER)
    {
      if (size == 0)
      {
        if (!buf_->empty())
        {
          *this = std::move(buf_->back());
        }
        else
        {
          reset_value();
        }
      }
      else
      {
        buf_->reserve(size);
      }
    }
    else if (size > 0)
    {
      KnowledgeRecord tmp = *this;

      new (&buf_) std::shared_ptr<CircBuf>(std::make_shared<CircBuf>(size));
      type_ = BUFFER;

      if (tmp.exists())
      {
        buf_->push_back(std::move(tmp));
      }
    }
  }

  /**
   * Clear all history for this record, keeping the current value.
   **/
  void clear_history()
  {
    set_history_capacity(0);
  }

private:
  size_t absolute_index(ssize_t index) const
  {
    if (type_ != BUFFER)
    {
      return 0;
    }
    if (index >= 0)
    {
      return buf_->front_index() + index;
    }
    return buf_->back_index() + (index + 1);
  }

public:
  /**
   * Execute a callable for each history element.
   **/
  template<typename Func>
  size_t for_history_range(Func&& func, size_t index, size_t count) const
  {
    if (type_ != BUFFER)
    {
      func(*this);
      return 1;
    }
    size_t front = buf_->front_index();
    if (index < front)
    {
      size_t diff = front - index;
      if (count > diff)
      {
        count -= diff;
      }
      else
      {
        count = 0;
      }
      index = front;
    }
    if (count > buf_->size())
    {
      count = buf_->size();
    }
    for (size_t i = index, end = index + count; i < end; ++i)
    {
      func((*buf_)[i]);
    }
    return count;
  }

  /**
   * Copy the given absolute range of history to the output iterator given.
   * Indexing is absolute, starting with the initial value of the record,
   * not relative to the circular buffer contents. You likely want to use
   * get_history instead.
   **/
  template<typename OutputIterator>
  size_t get_history_range(
      OutputIterator out, size_t index, size_t count) const;

  /**
   * Copy the @a count oldest stored history entries of this record to the
   * given output iterator, in order from oldest to newest.
   **/
  template<typename OutputIterator>
  size_t get_oldest(OutputIterator out, size_t count) const
  {
    return get_history(out, 0, count);
  }

  /**
   * Copy the oldest stored history entry of this record to the
   * given output iterator
   **/
  template<typename OutputIterator>
  auto get_oldest(OutputIterator out) const -> decltype(*out, size_t{})
  {
    return get_oldest(out, 1);
  }

  /**
   * Copy the @a count newest stored history entries of this record to the
   * given output iterator, in order from oldest to newest.
   **/
  template<typename OutputIterator>
  size_t get_newest(OutputIterator out, size_t count) const
  {
    return get_history(out, -(ssize_t)count, count);
  }

  /**
   * Copy the newest stored history entry of this record to the
   * given output iterator
   **/
  template<typename OutputIterator>
  auto get_newest(OutputIterator out) const -> decltype(*out, size_t{})
  {
    return get_newest(out, 1);
  }

  /**
   * Return the oldest stored history entry of this record
   **/
  KnowledgeRecord get_oldest() const
  {
    KnowledgeRecord ret;
    get_oldest(&ret, 1);
    return ret;
  }

  /**
   * Return the newest stored history entry of this record
   **/
  KnowledgeRecord get_newest() const
  {
    KnowledgeRecord ret;
    get_newest(&ret, 1);
    return ret;
  }

private:
  KnowledgeRecord& ref_newest()
  {
    return buf_->back();
  }

  const KnowledgeRecord& ref_newest() const
  {
    return buf_->back();
  }

public:
  /**
   * Return the @a count oldest stored history entries of this record in
   * a vector.
   **/
  std::vector<KnowledgeRecord> get_oldest(size_t count) const
  {
    std::vector<KnowledgeRecord> ret;
    get_oldest(std::back_inserter(ret), count);
    return ret;
  }

  /**
   * Return the @a count oldest stored history entries of this record in
   * a vector of the given element type, which must support knoweldge_cast<>
   * from KnowledgeRecord.
   **/
  template<typename T>
  std::vector<T> get_oldest(size_t count) const
  {
    std::vector<T> ret;
    get_oldest(std::back_inserter(ret), count);
    return ret;
  }

  /**
   * Return the @a count newest stored history entries of this record in
   * a vector.
   **/
  std::vector<KnowledgeRecord> get_newest(size_t count) const
  {
    std::vector<KnowledgeRecord> ret;
    get_newest(std::back_inserter(ret), count);
    return ret;
  }

  /**
   * Return the @a count newest stored history entries of this record in
   * a vector of the given element type, which must support knoweldge_cast<>
   * from KnowledgeRecord.
   **/
  template<typename T>
  std::vector<T> get_newest(size_t count) const
  {
    std::vector<T> ret;
    get_newest(std::back_inserter(ret), count);
    return ret;
  }

  /**
   * Get a copy of the entire stored history of this record.
   **/
  std::vector<KnowledgeRecord> get_history() const
  {
    std::vector<KnowledgeRecord> ret;
    get_history(std::back_inserter(ret));
    return ret;
  }

  /**
   * Copy the given range of history to the output iterator given. Indexing
   * starts from oldest history entry in the buffer at index 0. Negative
   * indices count from newest entries (-1 is newest).
   **/
  template<typename OutputIterator>
  size_t get_history(OutputIterator out, ssize_t index, size_t count) const
  {
    return get_history_range(out, absolute_index(index), count);
  }

  /**
   * Copy the stored history of this record to the given output iterator,
   * in order from oldest to newest.
   **/
  template<typename OutputIterator>
  auto get_history(OutputIterator out) const -> decltype(*out, size_t{})
  {
    return get_history_range(out, 0, std::numeric_limits<size_t>::max());
  }

  /**
   * Get a copy of the entire stored history of this record in a vector of
   * the given element type, which must support knoweldge_cast<>
   * from KnowledgeRecord.
   **/
  template<typename T>
  std::vector<T> get_history() const
  {
    std::vector<T> ret;
    get_history(std::back_inserter(ret));
    return ret;
  }

  /**
   * Return a copy of the given range of history in a vector. Indexing
   * starts from oldest history entry in the buffer at index 0. Negative
   * indices count from newest entries (-1 is newest).
   **/
  std::vector<KnowledgeRecord> get_history(size_t index, size_t count) const
  {
    std::vector<KnowledgeRecord> ret;
    get_history(std::back_inserter(ret), index, count);
    return ret;
  }

  /**
   * Return the given entry in this record's history. Indexing
   * starts from oldest history entry in the buffer at index 0. Negative
   * indices count from newest entries (-1 is newest).
   **/
  KnowledgeRecord get_history(size_t index) const
  {
    KnowledgeRecord ret;
    get_history(&ret, index, 1);
    return ret;
  }

  /**
   * Gets the absolute index of the newest element in stored history.
   * If this record doesn't have history capacity, throws IndexException
   **/
  size_t get_history_newest_index() const
  {
    if (!has_history())
    {
      throw exceptions::IndexException(
          "KnowledgeRecord::get_history_newest_index: "
          "record has zero capacity");
    }
    return buf_->back_index();
  }

  /**
   * Gets the absolute index of the oldest element in stored history.
   * If this record doesn't have history capacity, throws IndexException
   **/
  size_t get_history_oldest_index() const
  {
    if (!has_history())
    {
      throw exceptions::IndexException(
          "KnowledgeRecord::get_history_oldest_index: "
          "record has zero capacity");
    }
    return buf_->front_index();
  }

  /**
   * Get a shared_ptr to the history buffer inside this record. Returns
   * nullptr if this record has no history capacity.
   **/
  std::shared_ptr<CircBuf> share_circular_buffer() const;

  /**
   * Set metadata of this record equal to that of new_value, but doesn't
   * change value. Metadata is toi, clock, quality, write_quality, and
   * logger.
   **/
  void copy_metadata(const KnowledgeRecord& new_value);

  /**
   * Clears everything including history, but excluding metadata
   * (e.g., toi, clock) and copies value from new_value into this recod
   **/
  void overwrite(const KnowledgeRecord& new_value);

  /**
   * Clears everything including history, but excluding metadata
   * (e.g., toi, clock) and moves value from new_value into this recod
   **/
  void overwrite(KnowledgeRecord&& new_value);

private:
  template<typename... Args>
  KnowledgeRecord& emplace_hist(Args&&... args)
  {
    unshare();
    return buf_->emplace_back(std::forward<Args>(args)...);
  }

  template<typename T>
  using MemberType = std::shared_ptr<T> KnowledgeRecord::*;

  template<typename T, uint32_t Type, MemberType<T> Member,
      bool Overwrite = false, typename... Args>
  std::shared_ptr<const T> emplace_shared_val(Args&&... args)
  {
    if (has_history() && !Overwrite)
    {
      KnowledgeRecord tmp;
      tmp.copy_metadata(*this);
      auto ret =
          tmp.emplace_shared_val<T, Type, Member>(std::forward<Args>(args)...);
      emplace_hist(std::move(tmp));
      return ret;
    }
    clear_union();
    type_ = Type;
    return *new (&(this->*Member))
        std::shared_ptr<const T>(std::forward<Args>(args)...);
  }

  template<typename T, uint32_t Type, MemberType<T> Member,
      bool Overwrite = false, typename... Args>
  std::shared_ptr<const T> emplace_val(Args&&... args)
  {
    return emplace_shared_val<T, Type, Member, Overwrite>(
        std::move(std::make_shared<const T>(std::forward<Args>(args)...)));
  }

  template<typename T, uint32_t Type, MemberType<std::vector<T>> Member,
      bool Overwrite = false, typename... Args>
  std::shared_ptr<const std::vector<T>> emplace_shared_vec(Args&&... args)
  {
    return emplace_shared_val<std::vector<T>, Type, Member, Overwrite>(
        std::forward<Args>(args)...);
  }

  template<typename T, uint32_t Type, MemberType<std::vector<T>> Member,
      bool Overwrite = false, typename... Args>
  std::shared_ptr<const std::vector<T>> emplace_vec(Args&&... args)
  {
    return emplace_val<std::vector<T>, Type, Member, Overwrite>(
        std::forward<Args>(args)...);
  }
};

typedef ::std::map<std::string, KnowledgeRecord> KnowledgeMap;
typedef ::std::multimap<std::string, KnowledgeRecord> KnowledgeMultiMap;
typedef ::std::vector<KnowledgeRecord> KnowledgeVector;
typedef ::std::vector<std::string> KnowledgeRules;
typedef ::std::vector<std::string> StringVector;
typedef ::std::map<std::string, KnowledgeRecord*> KnowledgeRecords;

typedef std::string KnowledgeKey;
typedef KnowledgeRecord KnowledgeValue;

MADARA_EXPORT void safe_clear(KnowledgeMap& map);

/**
 * Returns the maximum quality within the records
 * @param    records     the list of records to gauge quality of
 * @return   the maximum quality within the list of records
 **/
uint32_t max_quality(const KnowledgeRecords& records);

/**
 * Returns the maximum quality within the records
 * @param    records     the list of records to gauge quality of
 * @return   the maximum quality within the list of records
 **/
uint32_t max_quality(const KnowledgeMap& records);

}  // namespace knowledge
}  // namespace madara

/**
 * output stream buffering
 **/
// inline std::ostream & operator<< (std::ostream & stream,
// const madara::knowledge::KnowledgeRecord & rhs);

#include "KnowledgeRecord.inl"

#endif  // _MADARA_KNOWLEDGE_RECORD_H_

#include "KnowledgeCast.h"
