#ifndef   _MADARA_UTILITY_INL_
#define   _MADARA_UTILITY_INL_

#include "Utility.h"
#include "SimTime.h"

#ifdef _WIN32
  #include "madara/Boost.h"
  #include "boost/asio.hpp"
#else
  #include <pthread.h>
#endif

namespace madara { namespace utility {

inline bool
set_thread_priority (int priority)
{
  bool result = false;

  #ifdef _WIN32
    
    if (SetPriorityClass (GetCurrentProcess (), HIGH_PRIORITY_CLASS))
    {
      // in Windows, normalize for highest priority
      if (priority > THREAD_PRIORITY_HIGHEST)
        priority = THREAD_PRIORITY_HIGHEST;

      if (SetThreadPriority (GetCurrentThread (), priority))
      {
        result = true;
      }
    }
      
  #else
    // assume POSIX
    sched_param sch;
    int policy; 
    pthread_getschedparam (pthread_self (), &policy, &sch);
    sch.sched_priority = priority;
    if (0 == pthread_setschedparam (pthread_self (), SCHED_FIFO, &sch))
    {
      result = true;
    }
  #endif

  return result;
}

/// Convert string to uppercase
inline std::string
strip_prefix (
const std::string & input, const std::string & prefix)
{
  return input.substr (prefix.size ());
}


/// Convert string to uppercase
inline std::string &
upper (std::string &input)
{
  for (std::string::iterator cur = input.begin ();
       cur != input.end (); ++cur)
    *cur = toupper (*cur);

  return input;
}

/// Convert string to lowercase 
inline std::string & 
dds_topicify (std::string & input)
{
  for (std::string::iterator cur = input.begin ();
       cur != input.end (); ++cur)
  {
    // change periods to _
    if (*cur == '.')
      *cur = '_';
  }

  return input;
}

/// Convert string to lowercase 
inline std::string &
lower (std::string &input)
{
  for (std::string::iterator cur = input.begin ();
       cur != input.end (); ++cur)
    *cur = tolower (*cur);

  return input;
}

inline bool
endian_is_little ()
{
  static const auto detect_little_endian = []() {
    uint16_t tester = 1;
    char copy[2];
    static_assert(sizeof(uint16_t) == sizeof(copy), "uint16_t isn't 2 bytes!");
    memcpy(copy, (char*)&tester, sizeof(copy));
    return copy[0] != 1;
  };

  static const bool ret = detect_little_endian ();
  return ret;
}

/**
  * Converts a host format uint64_t into big endian
  **/
inline uint64_t
endian_swap (uint64_t value)
{
  // if host is little endian, then we have work to do
  if (endian_is_little ())
  {
    value = ((value << 8) & 0xFF00FF00FF00FF00ULL )
          | ((value >> 8) & 0x00FF00FF00FF00FFULL );
    value = ((value << 16) & 0xFFFF0000FFFF0000ULL )
          | ((value >> 16) & 0x0000FFFF0000FFFFULL );
    return (value << 32) | (value >> 32);
  }

  return value;
}

/**
* Converts a host format signed knowledge::KnowledgeRecord::Integer into big endian
**/
inline int64_t
endian_swap (int64_t value)
{
  // if host is little endian, then we have work to do
  if (endian_is_little ())
  {
    value = ((value << 8) & 0xFF00FF00FF00FF00ULL )
          | ((value >> 8) & 0x00FF00FF00FF00FFULL );
    value = ((value << 16) & 0xFFFF0000FFFF0000ULL )
          | ((value >> 16) & 0x0000FFFF0000FFFFULL );
    return (value << 32) | ((value >> 32) & 0xFFFFFFFFULL);
  }

  return value;
}


/**
  * Converts a host format uint64_t into big endian
  **/
inline uint32_t
endian_swap (uint32_t value)
{
  // if host is little endian, then we have work to do
  if (endian_is_little ())
  {
    value = ((value << 8) & 0xFF00FF00 ) |
            ((value >> 8) & 0xFF00FF ); 
    return (value << 16) | (value >> 16);
  }

  return value;
}

/**
* Converts a host format signed knowledge::KnowledgeRecord::Integer into big endian
**/
inline int32_t
endian_swap (int32_t value)
{
  // if host is little endian, then we have work to do
  if (endian_is_little ())
  {
    value = ((value << 8) & 0xFF00FF00) |
            ((value >> 8) & 0xFF00FF ); 
    return (value << 16) | ((value >> 16) & 0xFFFF);
  }

  return value;
}

/**
* Converts a host format signed knowledge::KnowledgeRecord::Integer into big endian
**/
inline uint16_t
endian_swap (uint16_t value)
{
  // if host is little endian, then we have work to do
  if (endian_is_little ())
  {
    return ((value << 8) & 0xFFFF) | (value >> 8);
  }

  return value;
}

/**
* Converts a host format signed knowledge::KnowledgeRecord::Integer into big endian
**/
inline int16_t
endian_swap (int16_t value)
{
  // if host is little endian, then we have work to do
  if (endian_is_little ())
  {
    return ((value << 8) & 0xFFFF) | (value >> 8);
  }

  return value;
}

/**
  * Converts a host format double into big endian
  **/
inline double
endian_swap (double orig)
{
  // if host is little endian, then we have work to do
  if (endian_is_little ())
  {
    uint64_t value;
    memcpy(&value, &orig, sizeof(value));

    value = endian_swap(value);

    double result;
    memcpy (&result, &value, sizeof(result));
    return result;
  }

  return orig;
}

static const uint64_t milli_per = 1000;
static const uint64_t micro_per = milli_per * 1000;
static const uint64_t nano_per = micro_per * 1000;

static const uint64_t simtime_min_sleep = 100 * (nano_per / milli_per);

inline TimeValue
get_time_value (void)
{
#ifndef MADARA_FEATURE_SIMTIME
  return Clock::now ();
#else
  return TimeValue (std::chrono::nanoseconds (SimTime::time ()))
#endif
}

inline int64_t
get_time (void)
{
  auto current_time = Clock::now ();
  auto epoch = current_time.time_since_epoch ();
  return std::chrono::duration_cast<Duration> (epoch).count ();
}

inline TimeValue
add_seconds (const TimeValue & start, double seconds)
{
  return start + seconds_to_duration (seconds);
}

inline Duration seconds_to_duration (double seconds)
{
  return std::chrono::duration_cast<Duration> (
    seconds_to_seconds_duration (seconds));
}

inline SecondsDuration seconds_to_seconds_duration (double seconds)
{
  return std::chrono::duration<double> (seconds);
}

inline TimeValue seconds_to_time (double seconds)
{
  return TimeValue (seconds_to_duration (seconds));
}

inline bool approx_equal (
  double value1, double value2, double epsilon)
{
  return std::abs (value1 - value2) < epsilon;
}

inline bool
file_exists (const std::string & filename)
{
  if (FILE * file = fopen (filename.c_str(), "r"))
  {
    fclose(file);
    return true;
  }
  else
  {
    return false;
  }   
}

inline unsigned int
file_size (const std::string & filename)
{
  unsigned int size = 0;
  if (FILE * file = fopen (filename.c_str(), "r"))
  {
    fseek (file, 0L, SEEK_END);
    size = ftell (file);
    fclose (file);
  }

  return size;
}

inline
bool begins_with (const std::string & input,
      const std::string & prefix)
{
  bool result = false;

  if (prefix.length () <= input.length ())
  {
    result = input.substr (0, prefix.length ()) == prefix;
  }

  return result;
}

inline
bool ends_with (const std::string & input,
  const std::string & match)
{
  bool result = false;

  if (match.length () <= input.length ())
  {
    result =
      input.substr (input.size () - match.length (), match.length ()) == match;
  }

  return result;
}


template <typename T>
T bitmask_add (T mask, T values)
{
  return mask | values;
}
    
/**
  * Returns true if mask contains values
  **/
template <typename T>
bool bitmask_check (T mask, T values)
{
  return (mask & values) > 0;
}
    
/**
  * Removes values from a bit mask
  **/
template <typename T>
T bitmask_remove (T mask, T values)
{
  return mask & ~values;
}


template <typename T>
bool
less_compare (const T & left, const T & right)
{
  return left < right;
}

template <typename T>
bool
greater_compare (const T & left, const T & right)
{
  return right < left;
}

template <typename T>
void
sift_down (T * input, int start, int end,
  bool (*comparator) (const T & left, const T & right))
{
  int root = start;
  for (int child = root * 2 + 1; child <= end; child = root * 2 + 1)
  {
    int swap = root;

    // check left child
    if (comparator (input[child], input[swap]))
      swap = child;

    // check right child
    ++child;
    if (child <= end && comparator (input[child], input[swap]))
      swap = child;

    if (swap != root)
    {
      std::swap (input[root], input[swap]);
      root = swap;
    }
    else
      break;
  }
}
    
template <typename T>
void
heapify (T * input, int size,
  bool (*comparator) (const T & left, const T & right))
{
  // start at lowest parent node and work our way back
  for (int start = (size - 2) / 2; start >= 0; --start)
  {
    sift_down (input, start, size - 1, comparator);
  }
}

template <typename T>
void
heap_sort (T * input, int size,
  bool (*comparator) (const T & left, const T & right))
{
  heapify (input, size, comparator);

  for (int end = size - 1; end > 0; --end)
  {
    std::swap (input[end], input[0]);
    sift_down (input, 0, end - 1);
  }
}

} }

#endif  // _MADARA_UTILITY_INL_

