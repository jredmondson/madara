#ifndef   _MADARA_UTILITY_INL_
#define   _MADARA_UTILITY_INL_

#include "Utility.h"

inline
bool Madara::Utility::begins_with (const std::string & input,
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
bool Madara::Utility::ends_with (const std::string & input,
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
T Madara::Utility::bitmask_add (T mask, T values)
{
  return mask | values;
}
    
/**
  * Returns true if mask contains values
  **/
template <typename T>
bool Madara::Utility::bitmask_check (T mask, T values)
{
  return (mask & values) > 0;
}
    
/**
  * Removes values from a bit mask
  **/
template <typename T>
T Madara::Utility::bitmask_remove (T mask, T values)
{
  return mask & ~values;
}


template <typename T>
bool
Madara::Utility::less_compare (const T & left, const T & right)
{
  return left < right;
}

template <typename T>
bool
Madara::Utility::greater_compare (const T & left, const T & right)
{
  return right < left;
}

template <typename T>
void
Madara::Utility::sift_down (T * input, int start, int end,
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
Madara::Utility::heapify (T * input, int size,
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
Madara::Utility::heap_sort (T * input, int size,
  bool (*comparator) (const T & left, const T & right))
{
  heapify (input, size, comparator);

  int end = size - 1;
  for (int end = size - 1; end > 0; --end)
  {
    std::swap (input[end], input[0]);
    sift_down (input, 0, end - 1);
  }
}

#endif  // _MADARA_UTILITY_INL_