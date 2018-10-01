#ifndef _MADARA_UTILITY_H_
#define _MADARA_UTILITY_H_

#include <vector>
#include <string>
#include <cmath>
#include <limits.h>
#include <cstdlib>
#include <cstring>
#include <chrono>
#include "madara/MadaraExport.h"
#include "madara/utility/StdInt.h"
#include "madara/knowledge/WaitSettings.h"
#include "madara/utility/IntTypes.h"

namespace madara
{
namespace knowledge
{
class KnowledgeBase;
struct WaitSettings;
}

namespace utility
{
/// default clock type
typedef std::chrono::steady_clock Clock;

/// default clock duration
typedef std::chrono::nanoseconds Duration;

/// default clock duration
typedef std::chrono::duration<double> SecondsDuration;

/// time point
typedef std::chrono::time_point<Clock> TimeValue;

/**
 * Sets the thread priority in a FIFO scheme
 * @param     priority     the priority to attempt to set
 * @return    true if set call was successful
 **/
MADARA_EXPORT bool set_thread_priority(int priority = 20);

/**
 * Gets the MADARA version number
 * @return    the MADARA version number
 **/
MADARA_EXPORT std::string get_version(void);

/**
 * Converts a string version to a uint32. To use this for an
 * arbitrary version string, make sure it is in the format
 * x[.x]*. Examples include 1, 1.2, 1.2.3, 1.2.3.4. Version
 * components should not go above 255.
 * @param   string_version   the string version to convert
 * @return    the MADARA 4 byte version number
 **/
MADARA_EXPORT uint32_t get_uint_version(
    const std::string& str_version = get_version());

/**
 * Converts a MADARA uint32_t version number to human-readable
 * @param   version     the 4 byte version number
 * @return              the stringified version number
 **/
MADARA_EXPORT std::string to_string_version(uint32_t version);

/**
 * Replaces an old phrase with a new phrase within a string
 * @param   source     source string to change
 * @param   old_phrase  old phrase in the source string to replace
 * @param   new_phrase  new phrase to replace the old phrase
 * @param   replace_all if true, replace all instances of old phrase.
 *                      if false, replace first instance of old phrase.
 * @return  the number of replacements made
 **/
MADARA_EXPORT size_t string_replace(std::string& source,
    const std::string& old_phrase, const std::string& new_phrase,
    bool replace_all = true);

/**
 * Converts the string to upper. This function will modify and return the
 * input, so make a copy if needed.
 * @param     input         the string to change
 * @return    a reference to the modified input
 **/
std::string& upper(std::string& input);

/**
 * Converts the string to lower. This function will modify and return the
 * input, so make a copy if needed.
 * @param     input         the string to change
 * @return    a reference to the modified input
 **/
std::string& lower(std::string& input);

/**
 * Changes periods to underscores in compliance with OpenSplice needs.
 * This function will modify and return the input, so make a copy if
 * needed.
 * @param     input         the string to change
 * @return    a reference to the modified input
 **/
std::string& dds_topicify(std::string& input);

/**
 * Strips all whitespace characters from a string. This function will
 * modify and return the input, so make a copy if needed.
 * @param     input         the string to change
 * @return    a reference to the modified input
 **/
std::string& strip_white_space(std::string& input);

/**
 * Strips all comments (single-line and multi-line). This function will
 * modify and return the input, so make a copy if needed.
 * @param     input         the string to change
 * @return    a reference to the modified input
 **/
MADARA_EXPORT std::string& strip_comments(std::string& input);

/**
 * Strips an unwanted character. This function will
 * modify and return the input, so make a copy if needed.
 * @param     input         the string to change
 * @param     unwanted      the character value to remove from input
 * @return    a reference to the modified input
 **/
MADARA_EXPORT std::string& string_remove(std::string& input, char unwanted);

/**
 * Strips whitespace from front and end of string and also
 * condenses multiple whitespace into a single space. This function will
 * modify and return the input, so make a copy if needed.
 * @param     input         the string to change
 * @return    a reference to the modified input
 **/
MADARA_EXPORT std::string& strip_extra_white_space(std::string& input);

/**
 * Strips a prefix from an input string and returns
 * the result. The prefix should be at the front of
 * the input. This does not perform checks for
 * speed reasons.
 * @param     input         the string to change
 * @param     prefix        the start of the string
 * @return    a reference to the modified input
 **/
std::string strip_prefix(const std::string& input, const std::string& prefix);

/**
 * Splits a key of host:port into a corresponding host and port.
 * @param     key         a string containing a host:port
 * @param     host        the host in the key
 * @param     port        the port in the key
 * @return    1 if there was no ":". 0 if both host and port are set.
 **/
MADARA_EXPORT int split_hostport_identifier(
    const std::string& key, std::string& host, std::string& port);

/**
 * Merges a host and port into a host:port key
 * @param     key         a string containing a host:port
 * @param     host        the host in the key
 * @param     port        the port in the key
 * @return    0 if successful (always successful right now)
 **/
MADARA_EXPORT int merge_hostport_identifier(
    std::string& key, const std::string& host, const std::string& port);

/**
 * Merges a host and port into a host:port key
 * @param     key         a string containing a host:port
 * @param     host        the host in the key
 * @param     u_port      the port in the key
 * @return    0 if successful (always successful right now)
 **/
MADARA_EXPORT int merge_hostport_identifier(
    std::string& key, const std::string& host, unsigned short u_port);

/**
 * Splits an input string into tokens.
 * @param     input       a string to be split by splitters
 * @param     splitters   a vector of strings to look for in input
 * @param     tokens      the sections in between splitters
 * @param     pivot_list  the actual splitters that were found between
 *                        the tokens
 **/
MADARA_EXPORT void tokenizer(const std::string& input,
    const ::std::vector<std::string>& splitters,
    ::std::vector<std::string>& tokens, ::std::vector<std::string>& pivot_list);

/**
 * Reads a file into a string
 * @param     filename    name of the file to read
 * @return    contents of the file. Null string if unsuccessful.
 **/
MADARA_EXPORT std::string file_to_string(const std::string& filename);

/**
 * Extracts the path of a filename
 * @param     name        name of the file to extract path of
 * @return    the directory path of the file
 **/
MADARA_EXPORT std::string extract_path(const std::string& name);

/**
 * Extracts the file name of an absolute or relative path
 * @param     name        name of the file to extract path of
 * @return    the file name
 **/
MADARA_EXPORT std::string extract_filename(const std::string& name);

/**
 * Checks the filename for abnormal redirects such as ".."
 * @param     filename     name of the file to check
 * @return    true if filename contains "..", "//", or "~""
 **/
MADARA_EXPORT bool filename_has_redirect(const std::string& filename);

/**
 * Builds a file from fragments that have the format: filename.{}.crc.frag
 * @param   filename          name of the file to create
 * @param   crc               crc for completed file
 * @param   delete_incomplete if true, delete if crc is incorrect
 * @param   delete_fragments  if true, delete fragments if crc correct
 * @return  true if resulting file has correct crc
 **/
MADARA_EXPORT bool file_from_fragments(const std::string& filename,
    uint32_t crc, bool delete_incomplete = true, bool delete_fragments = true);

/**
 * Builds a file from fragments that have the format: filename.{}.crc.frag
 * @param   filename          name of the file to create
 * @param   crc               crc for completed file
 * @param   expected_size     expected total size of the file (bytes)
 * @param   fragment_size     max fragment size for sending
 * @return    size in bytes recieved of the fragments
 **/
MADARA_EXPORT size_t get_file_progress(const std::string& filename,
    uint32_t crc, size_t expected_size, size_t fragment_size = 60000);

/**
 * Attempts to builds a file from fragments that have the format:
 * filename.{}.crc.frag and returns any missing fragments
 * @param   filename          name of the file to create
 * @param   crc               crc for completed file
 * @param   expected_size     expected total size of the file (bytes)
 * @param   max_fragments     maximum fragments to request (-1 means all)
 * @param   fragment_size     max fragment size for sending
 * @return  the fragments that are missing
 **/
MADARA_EXPORT std::vector<int64_t> get_file_missing_fragments(
    const std::string& filename, uint32_t crc, size_t expected_size,
    int max_fragments = -1, size_t fragment_size = 60000);

/**
 * Expands environment variables referenced in the string. The environment
 * variables must be specified as $(var) and not $var.
 * @param     source      the string to expand
 * @return    a string with environment variables expanded
 **/
MADARA_EXPORT std::string expand_envs(const std::string& source);

/**
 * Helper function for madara::utility::expand_envs which retrieves an
 * environment variable.
 * @param     source      the string to expand
 * @param     cur         the current focus position of expand_vars
 * @param     end         the end of the variable
 * @return    the value of the environment variable
 **/
char* get_var(const std::string& source, size_t cur, size_t& end);

/**
 * Substitutes the appropriate directory delimiter, which may help
 * with portability between operating systems.
 * @param     target      the string to modify
 * @return    a more compliant directory path string
 **/
MADARA_EXPORT std::string clean_dir_name(const std::string& target);

/**
 * Converts a host format uint64_t into big endian. Can
 * also be used to convert back into host form.
 * @param     value      the value to convert
 * @return    the converted value
 **/
uint64_t endian_swap(uint64_t value);

/**
 * Converts a host format signed madara::knowledge::KnowledgeRecord::Integer
 * into big endian.  Can also be used to convert back into host form.
 * @param     value      the value to convert
 * @return    the converted value
 **/
int64_t endian_swap(int64_t value);

/**
 * Converts a host format uint64_t into big endian. Can
 * also be used to convert back into host form.
 * @param     value      the value to convert
 * @return    the converted value
 **/
uint32_t endian_swap(uint32_t value);

/**
 * Converts a host format signed madara::knowledge::KnowledgeRecord::Integer
 * into big endian.  Can also be used to convert back into host form.
 * @param     value      the value to convert
 * @return    the converted value
 **/
int32_t endian_swap(int32_t value);

/**
 * Converts a host format signed madara::knowledge::KnowledgeRecord::Integer
 * into big endian.  Can also be used to convert back into host form.
 * @param     value      the value to convert
 * @return    the converted value
 **/
uint16_t endian_swap(uint16_t value);

/**
 * Converts a host format signed madara::knowledge::KnowledgeRecord::Integer
 * into big endian.  Can also be used to convert back into host form.
 * @param     value      the value to convert
 * @return    the converted value
 **/
int16_t endian_swap(int16_t value);

/**
 * Converts a host format double precision into big endian. Can
 * also be used to convert back into host form.
 * @param     value      the value to convert
 * @return    the converted value
 **/
double endian_swap(double value);

/**
 * Reads a file into a provided void pointer. The void pointer will point
 * to an allocated buffer that the user will need to delete.
 * @param    filename   the name of the file
 * @param    buffer     a buffer that contains the contents of the file
 * @param    size       the size of the allocated buffer (will change)
 * @param    add_zero_char  add a zero char to the end of the buffer
 * @return              zero if successful
 **/
MADARA_EXPORT int read_file(const std::string& filename, void*& buffer,
    size_t& size, bool add_zero_char = false);

/**
 * Writes a file with provided contents.
 * @param    filename   the name of the file
 * @param    buffer     a buffer that contains the contents of the file
 * @param    size       the size of the allocated buffer
 * @return              amount of bytes written (-1 if unsuccessful)
 **/
MADARA_EXPORT ssize_t write_file(
    const std::string& filename, void* buffer, size_t size);

/**
 * Returns a time of day in nanoseconds
 * If simtime feature is enabled, this may be simulation time instead of
 * real rtime.
 *
 * @return  time of day in nanoseconds
 **/
int64_t get_time(void);

/**
 * Returns a time of day as a chrono time value
 * If simtime feature is enabled, this may be simulation time instead of
 * real rtime.
 *
 * @return  time of day as a chrono time value
 **/
TimeValue get_time_value(void);

/**
 * Returns an offset of a time by seconds in double format
 * @return  new time of day as a chrono time value
 **/
TimeValue add_seconds(const TimeValue& start, double seconds);

/**
 * Returns seconds in double format as nanosecond duration
 * @return  duration for usage with TimeValues
 **/
Duration seconds_to_duration(double seconds);

/**
 * Returns seconds in double format as seconds duration
 * @return  duration for usage with TimeValues
 **/
SecondsDuration seconds_to_seconds_duration(double seconds);

/**
 * Returns seconds in double format as nanosecond since epoch
 * @return  converted time
 **/
TimeValue seconds_to_time(double seconds);

/**
 * Returns a random double between floor and ceiling
 * @param  floor      lowest possible double
 * @param  ceiling    highest possible double
 * @param  set_seed_to_time  if true, seed the random generator with time
 * @return a double between floor and ceiling (inclusive)
 **/
MADARA_EXPORT double rand_double(
    double floor = 0.0, double ceiling = 1.0, bool set_seed_to_time = true);

/**
 * Returns a random integer between a floor and ceiling
 * @param  floor      lowest possible integer
 * @param  ceiling    highest possible integer
 * @param  set_seed_to_time  if true, seed the random generator with time
 * @return a integer between floor and ceiling (inclusive)
 **/
MADARA_EXPORT int64_t rand_int(int64_t floor = 0, int64_t ceiling = RAND_MAX,
    bool set_seed_to_time = true);

/**
 * Rounds a double to the nearest integer
 * @param  input      real number to round to integer
 * @return the nearest integer to the input
 **/
MADARA_EXPORT int64_t nearest_int(double input);

/**
 * Check if input contains prefix at the beginning
 * @param  input     string to check for prefix
 * @param  prefix    string to match
 * @return true if input begins with prefix. False otherwise.
 **/
MADARA_EXPORT bool begins_with(
    const std::string& input, const std::string& prefix);

/**
 * Check if input contains a pattern at the end
 * @param  input     string to check for prefix
 * @param  ending    string to match
 * @return true if input ends with ending. False otherwise.
 **/
MADARA_EXPORT bool ends_with(
    const std::string& input, const std::string& ending);

/**
 * Sleeps for a certain amount of time. Unlike other sleep timers
 * this will sleep for the amount of time regardless of OS interrupts
 * @param  sleep_time  time in seconds to sleep for
 **/
MADARA_EXPORT double sleep(double sleep_time);

/**
 * Sleeps for a certain amount of time. Unlike other sleep timers
 * this will sleep for the amount of time regardless of OS interrupts
 * @param  sleep_time  time to sleep for
 **/
MADARA_EXPORT SecondsDuration sleep(const SecondsDuration& sleep_time);

/**
 * Checks two doubles for approximate equality. Doubles, unlike integers,
 * are approximations of fractions. Consequently, two doubles cannot be
 * checked for equality as easily and as portably as integers. To compare
 * doubles for equality, use this function with an epsilon, which by
 * default is accurate to the 10,000th (4 decimal places)
 * @param  value1   the first value to compare, order does not matter
 * @param  value2   the second value to compare, order does not matter
 * @param  epsilon  the minimum difference between two values before
 *                  equality does not hold.
 **/
bool approx_equal(double value1, double value2, double epsilon = 0.0001);

/**
 * Checks if a file exists
 * @param filename   path and name of the file to open
 * @return true if file exists. false otherwise.
 **/
bool file_exists(const std::string& filename);

/**
 * Checks if a a path is a directory
 * @param path       path to check for directory status
 * @return  true if the path is a directory
 **/
bool is_directory(const std::string& path);

/**
 * Returns the size of a file
 * @param filename   path and name of the file to open
 * @return size of the file
 **/
unsigned int file_size(const std::string& filename);

/**
 * Returns the size of a file stream and returns the stream
 * in the same position as when called
 *
 * @param input  the file stream to check
 * @return size of the stream
 **/
size_t file_size(std::ifstream& input);

/**
 * Returns the crc of a file
 * @param filename   path and name of the file to open
 * @param max_block  maximum block size to read when processing CRC. This
 *                   can be important with large files. We provide a
 *                   reasonable block size by default, but you can adjust
 *                   as appropriate
 * @return size of the file
 **/
uint32_t file_crc(const std::string& filename, size_t max_block = 1000000);

/**
 * Waits on a knowledge record to be true without needing KaRL language
 * @param  knowledge  the knowledge base
 * @param  variable   the variable to wait on
 * @param  settings   the Wait Settings. Only uses poll_frequency and
 *                    max_wait.
 **/
MADARA_EXPORT bool wait_true(knowledge::KnowledgeBase& knowledge,
    const std::string& variable,
    const knowledge::WaitSettings& settings = knowledge::WaitSettings());

/**
 * Waits on a knowledge record to be false without needing KaRL language
 * @param  knowledge  the knowledge base
 * @param  variable   the variable to wait on
 * @param  settings   the Wait Settings. Only uses poll_frequency and
 *                    max_wait.
 **/
MADARA_EXPORT bool wait_false(knowledge::KnowledgeBase& knowledge,
    const std::string& variable,
    const knowledge::WaitSettings& settings = knowledge::WaitSettings());
/**
 * Returns true if left < right
 **/
template<typename T>
bool less_compare(const T& left, const T& right);

/**
 * Returns true if right < left
 **/
template<typename T>
bool greater_compare(const T& left, const T& right);

/**
 * Sifts elements down a heap according to a comparator
 * @param  input       the array to heapify
 * @param  start       index to start from
 * @param  end         index that signifies end of range to sift
 * @param  comparator  a function for comparing two elements
 **/
template<typename T>
void sift_down(T* input, int start, int end,
    bool (*comparator)(const T& left, const T& right) = greater_compare<T>);

/**
 * Builds a heap out of an array of elements
 * @param  input       the array to heapify
 * @param  size        the size of the array
 * @param  comparator  a function for comparing two elements
 **/
template<typename T>
void heapify(T* input, int size,
    bool (*comparator)(const T& left, const T& right) = greater_compare<T>);

/**
 * Sorts an array with heap sort
 * @param  input       the array to heapify
 * @param  size        the size of the array
 * @param  comparator  a function for comparing two elements
 **/
template<typename T>
void heap_sort(T* input, int size,
    bool (*comparator)(const T& left, const T& right) = greater_compare<T>);

/**
 * Adds values to a bit mask
 * @param   mask    the target bit mask
 * @param   values  values to bitwise add to mask
 * @return  resulting mask
 **/
template<typename T>
T bitmask_add(T mask, T values);

/**
 * Returns true if mask contains values
 * @param   mask    the bit mask to check
 * @param   values  values to check for inclusion in mask
 * @return  true if values in exist in mask, false otherwise
 **/
template<typename T>
bool bitmask_check(T mask, T values);

/**
 * Removes values from a bit mask
 * @param   mask    the target bit mask
 * @param   values  values to bitwise remove from mask
 * @return  resulting mask
 **/
template<typename T>
T bitmask_remove(T mask, T values);

std::pair<std::string, uint16_t> parse_address(std::string addr);
}
}

#include "Utility.inl"

#endif  // _MADARA_UTILITY_H_
