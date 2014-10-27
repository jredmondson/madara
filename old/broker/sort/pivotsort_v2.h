#ifndef _PIVOT_SORT_H_
#define _PIVOT_SORT_H_

#include <algorithm>

/**
* Contains M Pivot Sort algorithm, first published in AMCS 2005.
* Less tendency to go O(n^2) than Quick Sort and faster due to
* reduced moves and less recursion. This implementation was written
* to support std algorithm semantics.
*
* @author James Edmondson <jedmondson@gmail.com>
**/

namespace std
{

#define PIVOTSORT_MAXPIVOTS 5
#define PIVOTSORT_INITPIVOTS 5

  inline unsigned int _change_num_pivots()
  {
    // num_pivots will be a random number between 3 and 6.

    // return num_pivots just in case the calling function needs a copy
    // for instance, in case we want to make sure that we haven't
    // randomly selected the same number of pivots.

    return (3 + rand() % 4);
  }


  template <class _RanIt> inline
    _RanIt _choose_pivots(_RanIt first, _RanIt last, unsigned num_pivots)
  {
    unsigned int section = (last - first) / (num_pivots + 1);
    unsigned int i = 0;

    for (first = first + section; i < num_pivots; ++i, first += section)
    {
      std::iter_swap (--last, first);
      std::iter_swap (--last, first + 1);
    }

    return --last;
  }

  /********************************************************************
  * function: pivotEqualsLeft (local)
  * precondition: array should contain the data to be sorted, and next_start,
  *               next_greater, and cur_pivot should all point to valid memory
  *               locations in array.
  * postcondition: elements between next_start and cur_pivot will be arranged
  *                around the value in array[cur_pivot] - those elements equal to
  *                array[cur_pivot] will be left of the position cur_pivot should be.
  *                next_greater will point to the new position of array[cur_pivot].
  *********************************************************************/

  template <class _RanIt> inline
    void _pivot_equals_left (_RanIt & next_start,
    _RanIt & next_greater, _RanIt & cur_pivot)
  {
    for(_RanIt cur_unknown = next_start; cur_unknown != cur_pivot; cur_unknown++)
    {
      if( !(*cur_unknown < *cur_pivot) && !(*cur_pivot < *cur_unknown) )
      {
        //if( next_greater != cur_unknown )
        //{
          std::iter_swap(cur_unknown, next_greater);
        //}
        ++next_greater;
      }
    }
  }

  template <class _RanIt, class _Pred> inline
    void _pivot_equals_left (_RanIt & next_start,
    _RanIt & next_greater, _RanIt & cur_pivot,
    _Pred predicate)
  {
    for (_RanIt cur_unknown = next_start; cur_unknown != cur_pivot; cur_unknown++)
    {
      if( !(predicate (*cur_unknown, *cur_pivot)) && 
        !(predicate (*cur_pivot, *cur_unknown)) )
      {
        //if( next_greater != cur_unknown )
        //{
          std::iter_swap(cur_unknown, next_greater);
        //}
        ++next_greater;
      }
    }
  }

  template <class _RanIt> inline
    _RanIt _pivot_smaller_left (_RanIt & next_start,
    _RanIt & next_greater, _RanIt & cur_pivot)
  {
    _RanIt greater = next;
    for (; next != pivot - 1; ++next)
    {
      if (*next < *pivot)
      {
        std::iter_swap (next, greater++);
      }
    }

    if (greater != pivot - 1)
    {
      std::iter_swap (greater++, pivot - 1);
      std::iter_swap (greater++, pivot);
    }

    return greater;
  }

  template <class _RanIt, class _Pred> inline
    //std::pair <RanIt, RanIt> 
    _RanIt
    _pivot_smaller_left (_RanIt next, _RanIt pivot, _Pred predicate)
  {
    _RanIt greater = next;
    for (; next != pivot - 1; ++next)
    {
      if (predicate (*next, *pivot))
      {
        std::iter_swap (next, greater++);
      }
    }

    if (greater != pivot - 1)
    {
      std::iter_swap (greater++, pivot - 1);
      std::iter_swap (greater++, pivot);
    }

    return greater;

 // _RanIt _First = next;
 // _RanIt _Last = pivot + 1;

 // _RanIt _Pfirst = pivot;
 // _RanIt _Plast = pivot + 1;


	//while (_First < _Pfirst
	//	&& !_DEBUG_LT_PRED(predicate, *(_Pfirst - 1), *_Pfirst)
	//	&& !(predicate (*_Pfirst, *(_Pfirst - 1))))
	//	--_Pfirst;
	//while (_Plast < _Last
	//	&& !_DEBUG_LT_PRED(predicate, *_Plast, *_Pfirst)
	//	&& !(predicate (*_Pfirst, *_Plast)))
	//	++_Plast;

	//_RanIt _Gfirst = _Plast;
	//_RanIt _Glast = _Pfirst;

	//for (; ; )
	//	{	// partition
	//	for (; _Gfirst < _Last; ++_Gfirst)
	//		if (_DEBUG_LT_PRED(predicate, *_Pfirst, *_Gfirst))
	//			;
	//		else if (predicate (*_Gfirst, *_Pfirst))
	//			break;
	//		else
	//			std::iter_swap(_Plast++, _Gfirst);
	//	for (; _First < _Glast; --_Glast)
	//		if (_DEBUG_LT_PRED(predicate, *(_Glast - 1), *_Pfirst))
	//			;
	//		else if (predicate (*_Pfirst, *(_Glast - 1)))
	//			break;
	//		else
	//			std::iter_swap(--_Pfirst, _Glast - 1);
	//	if (_Glast == _First && _Gfirst == _Last)
	//		return (pair<_RanIt, _RanIt>(_Pfirst, _Plast));

	//	if (_Glast == _First)
	//		{	// no room at bottom, rotate pivot upward
	//		if (_Plast != _Gfirst)
	//			std::iter_swap(_Pfirst, _Plast);
	//		++_Plast;
	//		std::iter_swap(_Pfirst++, _Gfirst++);
	//		}
	//	else if (_Gfirst == _Last)
	//		{	// no room at top, rotate pivot downward
	//		if (--_Glast != --_Pfirst)
	//			std::iter_swap(_Glast, _Pfirst);
	//		std::iter_swap(_Pfirst, --_Plast);
	//		}
	//	else
	//		std::iter_swap(_Gfirst++, --_Glast);
	//	}

  }

  /********************************************************************
  * function: insertSort
  * arguments: reference to a vector of integers and the size of the vector
  * returns: none
  * comments: sorts a vector in ascending order - not a good algorithm
  *           should not be used unless you know that only 1 or 2 items
  *           in the vector are out of place (not sorted) or already sorted
  ********************************************************************/

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

  template <class _RanIt> inline
    void _pivotsort(_RanIt first, _RanIt last, unsigned int num_pivots)
  {
    unsigned int size = last - first;
    if (size > 32)
    {
      // choose num_pivots * 2 + 1 candidates and sort them at the end of
      // the list
      _RanIt pivot = std::_choose_pivots (first, last, num_pivots);
      std::insertsort (pivot++, last);
      _RanIt last_first = first;
      
      for (unsigned int i = 0; i < num_pivots; ++i, pivot += 2)
      {
        first = _pivot_smaller_left (first, pivot);
        std::pivotsort (last_first, first - 1);
        last_first = first;

        // need to place pivot_equals_left here
      }

      std::pivotsort (first, last);
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
  void _pivotsort (_RanIt first, _RanIt last,
    unsigned int num_pivots, _Pred predicate)
  {
    unsigned int size = last - first;
    if (size > 32)
    {
      // choose num_pivots * 2 + 1 candidates and sort them at the end of
      // the list
      _RanIt pivot = std::_choose_pivots (first, last, num_pivots);
      std::insertsort (pivot++, last, predicate);
	    //std::make_heap (first, last, predicate);
	    //std::sort_heap (first, last, predicate);
      _RanIt last_first = first;
      
      for (unsigned int i = 0; i < num_pivots; ++i, pivot += 2)
      {
        first = _pivot_smaller_left (first, pivot, predicate);
        std::pivotsort (last_first, first - 1, predicate);
        last_first = first;

        // need to place pivot_equals_left here
      }

      std::pivotsort (first, last, predicate);
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
    _pivotsort (first, last, PIVOTSORT_INITPIVOTS);
  }

  template <class _RanIt, class _Pred> inline
  void pivotsort (_RanIt first, _RanIt last, _Pred predicate)
  {
    _DEBUG_RANGE (first, last);
    _pivotsort (first, last, PIVOTSORT_INITPIVOTS, predicate);
  }

}

#endif // _PIVOT_SORT_H_