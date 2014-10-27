#ifndef _PIVOT_SORT_H_
#define _PIVOT_SORT_H_

#include <algorithm>

/**
* Contains M Pivot Sort algorithm, first published in AMCS 2005.
* Less tendency to go O(n^2) than Quick Sort and faster due to
* reduced moves and less recursion. This implementation was written
* to support std algorithm semantics. Note: this is not faster
* than STL's hybrid sort method. But I'll try to work on optimizations
* soon to see if I can match the guarded_partition function in STL but
* with more pivots.
*
* @author James Edmondson <jedmondson@gmail.com>
**/

namespace std
{


  template <class _RanIt> inline
  void _choose_pivots(_RanIt first, _RanIt last)
  {
    // pivot sort is statically set to 5 divisions and 3 pivots
    // This sets up a group of 11 pivot candidates at the end of
    // the array, to be sorted by insertion sort
    unsigned int section = (last - first + 1) / 6;
    
    _RanIt mid = first + (last - first) / 2;
    _RanIt fm2 = mid - section;
    _RanIt fm1 = fm2 - section;
    _RanIt lm1 = mid + section;
    _RanIt lm2 = lm1 + section;

    std::iter_swap (lm2 + 1, --last);
    std::iter_swap (lm2, --last);
    std::iter_swap (lm1 + 1, --last);
    std::iter_swap (lm1, --last);
    std::iter_swap (mid + 1, --last);
    std::iter_swap (mid, --last);
    std::iter_swap (mid - 1, --last);
    std::iter_swap (fm2 + 1, --last);
    std::iter_swap (fm2, --last);
    std::iter_swap (fm1 + 1, --last);
    std::iter_swap (fm1, --last);
  }



  template <class _RanIt> inline
    void insertsort (_RanIt first, _RanIt last)
  {
    _DEBUG_RANGE (first, last);
    _RanIt unsorted, location;
    _RanIt::value_type next;

    for (unsorted = first + 1; unsorted != last; ++unsorted)
    {
      location = unsorted;
      next = *unsorted;

      for( ;location != first; --location)
      {
        // if the entry at location - 1 is less than or equal to the unsorted
        if( *(location - 1) < next || !(next < *(location - 1)))
          break;

        // otherwise, keep going down the line
        *location = *(location-1);
      }

      // at the end, set the contents of the current location to the unsorted value
      *location = next;
    }
  }

  template <class _RanIt, class _Pred> inline
    void insertsort (_RanIt first, _RanIt last, _Pred predicate)
  {
    _DEBUG_RANGE (first, last);
    _RanIt unsorted, location;
    _RanIt::value_type next;

    for (unsorted = first + 1; unsorted != last; ++unsorted)
    {
      location = unsorted;
      next = *unsorted;

      for( ;location != first; --location)
      {
        // if the entry at location - 1 is less than or equal to the unsorted
        if( predicate (*(location - 1), next) ||
          !predicate (next, *(location - 1)))
          break;

        // otherwise, keep going down the line
        *location = *(location-1);
      }

      // at the end, set the contents of the current location to the unsorted value
      *location = next;
    }
  }

  template <class _RanIt, class _Pred> inline
  void _four_partition (_RanIt first, _RanIt last, _RanIt partitions [],
                        _Pred predicate)
  {
    //              first     second      third
    // last - 11     [2]        [5]        [8]
    //             last - 9  last - 6    last - 3

    // partitions [0] end of first partition
    // partitions [1] start of second partition
    // partitions [2] end of second partition
    // partitions [3] start of third partition

    _RanIt g_1 = first, g_2 = first;

  }

  template <class _RanIt> inline
    void _pivotsort(_RanIt first, _RanIt last)
  {
    unsigned int size = last - first;
    if (size > 32)
    {

    }
    else
    {
      // the array range is very small so we should just
      // defer to an algorithm that handles small arrays best.
      //std::insertsort (first, last);
	    std::make_heap (first, last);
	    std::sort_heap (first, last);
    }
  }

  template <class _RanIt, class _Pred> inline
  void _pivotsort (_RanIt first, _RanIt last, _Pred predicate)
  {
    unsigned int size = last - first;
    if (size > 32)
    {
      _RanIt partitions[4];
      // choose num_pivots * 2 + 1 candidates and sort them at the end of
      // the list
      std::_choose_pivots (first, last);

      std::insertsort (last - 11, last, predicate);

      _four_partition (first, last, partitions, predicate);


    }
    else
    {
      // the array range is very small so we should just
      // defer to an algorithm that handles small arrays best.
      //std::insertsort (first, last);
	    std::make_heap (first, last, predicate);
	    std::sort_heap (first, last, predicate);
    }
  }

  template <class _RanIt> inline
  void pivotsort (_RanIt first, _RanIt last)
  {
    _DEBUG_RANGE (first, last);
    _pivotsort (first, last);
  }

  template <class _RanIt, class _Pred> inline
  void pivotsort (_RanIt first, _RanIt last, _Pred predicate)
  {
    _DEBUG_RANGE (first, last);
    _pivotsort (first, last, predicate);
  }

}

#endif // _PIVOT_SORT_H_