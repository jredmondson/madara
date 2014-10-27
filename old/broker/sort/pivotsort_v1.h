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

#define PIVOTSORT_MAXPIVOTS 3
#define PIVOTSORT_INITPIVOTS 3

  inline unsigned int _change_num_pivots()
  {
    // num_pivots will be a random number between 3 and 6.

    // return num_pivots just in case the calling function needs a copy
    // for instance, in case we want to make sure that we haven't
    // randomly selected the same number of pivots.

    return (3 + rand() % 4);
  }


  template <class _RanIt> inline
    void _choose_pivots(_RanIt first, _RanIt last,
    _RanIt pivots [], unsigned num_pivots)
  {

    // the base choice is the base array location we'll grab for sorting
    // in our subset of choice pivot locations.

    // steps is the amount we'll step from the base choice to mark a possible
    // pivot

    unsigned int baseChoice, steps, i;
    unsigned int size = last - first + 1;
    int j = num_pivots - 1;

    // baseChoice marks the spot at which a partition point is logically made.
    // the partition point baseChoice is the size of the array divided by the
    // number of partitions (num_pivots+1) minus 1, to account for array indexing.

    baseChoice = size / (num_pivots + 1) - 1;

    // if baseChoice is greater than 2 then step that plus 1.  Otherwise, step 2 because
    // we'll be grabbing num_pivots * 2 items from the array to sort

    steps = ( baseChoice > 2 ? baseChoice + 1 : 2 );

    // update baseChoice to not point logically but absolutely in the array.

    //baseChoice += first + 0;

    // fill locations with addresses of the chosen possible pivots

    pivots[0] = first + baseChoice;

    for (i = 1; i < num_pivots; i++)
      pivots[i] = pivots[i-1] + steps;

    // now, go through the pivot locations and copy them to the end
    // of the array, starting from last possible pivot choice to first
    // update the pivot index for later usage

    // notice that we put the pivots at the end of the array.  This is
    // not necessary, and other implementations of M Pivot Sort have been
    // done with the pivots placed at the front of the array.

    --last;
    for( j; j >= 0; j-- )
    {
      std::swap(*(pivots[j]+1),*last);
      --last;
      swap(*(pivots[j]),*last);
      pivots[j] = last;
      --last;
    }
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
    void _pivot_smaller_left (_RanIt & next_start,
    _RanIt & next_greater, _RanIt & cur_pivot)
  {
    for (_RanIt cur_unknown = next_start; cur_unknown < cur_pivot; ++cur_unknown)
    {
      if (*cur_unknown < *cur_pivot)
      {
        //if (next_greater != cur_unknown )
        //{
          std::iter_swap(cur_unknown, next_greater);
        //}
        ++next_greater;
      }
    }
  }

  template <class _RanIt, class _Pred> inline
    void _pivot_smaller_left (_RanIt & next_start,
    _RanIt & next_greater, _RanIt & cur_pivot,
    _Pred predicate)
  {
    for (_RanIt cur_unknown = next_start; cur_unknown < cur_pivot; ++cur_unknown)
    {
      if (predicate (*cur_unknown, *cur_pivot))
      {
        //if (next_greater != cur_unknown )
        //{
          std::iter_swap(cur_unknown, next_greater);
        //}
        ++next_greater;
      }
    }
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
    _RanIt cur_pivot, next_start, next_greater;
    _RanIt pivots[PIVOTSORT_MAXPIVOTS];
    unsigned int i;

    if (first != last)
    {
      // if array is big enough, start pivoting
      if (last - first > 32)
      {

        // choose our pivots.  Pick from the quartiles in a 
        // 3 pivot sort to guard against ascending and descending
        // lists
        std::_choose_pivots (first, last, pivots);

        // sort the pivot candidates
        std::insertsort (pivots[0]-1, last);

        // start from beginning of array range
        next_start = first;

        // let's begin by pivoting around the smallest of the pivots: array[last-offset]
        // where offset is (num_pivots * 2) - 1
        for (i = 0; i < num_pivots; ++i)
        {
          cur_pivot = pivots[i];
          next_greater = next_start;

          if (next_start != first && (
            !(pivots[i-1] < pivots[i]) && !(pivots[i] < pivots[i-1])))
          {
            // this is not the first pivot, and this pivot and its predecessor
            // are equal.  Consequently, we need to check if there are duplicates
            // other than the last pivot, the element between last and this pivot, and
            // this pivot.  If this situation exists, where 3 of the pivot candidates
            // were equal, then more often than not, we have stopped a potential n^2 problem.
            std::_pivot_equals_left (next_start, next_greater, cur_pivot);

            // check the other pivot candidates for duplicates of this element

            while( i < num_pivots && (
              !(pivots[i-1] < pivots[i]) && !(pivots[i] < pivots[i-1])))
            {
              // we've reached the end of the unknowns, and
              // it's time to copy the pivot.
              std::iter_swap (next_greater, cur_pivot);

              // we can also copy the element next to the pivot,
              // because we know it is less than the element in pivot2
              std::iter_swap (next_greater+1, cur_pivot+1);

              // update the pivot entry (we need to know where each
              // pivot took place)
              pivots[i] = next_greater;

              // update the next greater index
              next_start = next_greater + 2;

              i++;
              cur_pivot = pivots[i];
              next_greater = next_start;
            }

            i--;
          }
          else
          {
            // this is either the first pivot we look at, or this pivot does
            // not equal the last pivot, so we can look for what is smaller
            // than cur_pivot
            std::_pivot_smaller_left (next_start, next_greater, cur_pivot);

            // we've reached the end of the unknowns, and
            // it's time to copy the pivot.
            std::iter_swap (next_greater, cur_pivot);

            // we can also copy the element next to the pivot,
            // because we know it is less than the element in pivot2
            std::iter_swap (next_greater+1, cur_pivot+1);

            // update the pivot entry (we need to know where each pivot took place)
            pivots[i] = next_greater;

            // we've pivoted, now sort the sublist
            if (next_start == first && pivots[i] > next_start + 1)
              std::_pivotsort (next_start, pivots[i], num_pivots);
            if( next_start != first && pivots[i] > pivots[i-1] + 2 )
              std::_pivotsort (pivots[i-1]+1, pivots[i], num_pivots);

            // update the next greater starting index
            next_start = next_greater + 2;
          }
        }

        // sort the array elements between last pivot and last element of array 
        if(last > pivots[num_pivots-1] + 1)
        {
          int oldnum_pivots = num_pivots;
          //if (last == pivots[num_pivots-1] + 1
          //  || pivots[num_pivots-1] < first + ( num_pivots * 2 ) + 1)
          //{
          //  // change the num_pivots if we had a really bad run
          //  num_pivots = _change_num_pivots();
          //}

          std::_pivotsort (pivots[oldnum_pivots-1]+1, last, num_pivots);
        }
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
  }

  template <class _RanIt, class _Pred> inline
  void _pivotsort (_RanIt first, _RanIt last,
    unsigned int num_pivots, _Pred predicate)
  {
    _RanIt cur_pivot, next_start, next_greater;
    //std::vector<_RanIt> pivots;
    //pivots.resize (num_pivots);
    _RanIt pivots[PIVOTSORT_MAXPIVOTS];
    unsigned int i;

    if (first != last)
    {
      // if array is big enough, start pivoting
      if (last - first > 16)
      {

        // choose our pivots.  Pick from the quartiles in a 
        // 3 pivot sort to guard against ascending and descending
        // lists
        std::_choose_pivots (first, last, pivots, num_pivots);

        // sort the pivot candidates
        std::insertsort (pivots[0]-1, last, predicate);
		    //std::make_heap(pivots[0]-1, last, predicate);
		    //std::sort_heap(pivots[0]-1, last, predicate);

        // start from beginning of array range
        next_start = first;

        // let's begin by pivoting around the smallest of the pivots: array[last-offset]
        // where offset is (num_pivots * 2) - 1

        for (i = 0; i < num_pivots; ++i)
        {
          cur_pivot = pivots[i];
          next_greater = next_start;

          if (next_start != first && (
            !(pivots[i-1] < pivots[i]) && !(pivots[i] < pivots[i-1])))
          {
            // this is not the first pivot, and this pivot and its predecessor
            // are equal.  Consequently, we need to check if there are duplicates
            // other than the last pivot, the element between last and this pivot, and
            // this pivot.  If this situation exists, where 3 of the pivot candidates
            // were equal, then more often than not, we have stopped a potential n^2 
            // problem.
            std::_pivot_equals_left (next_start, next_greater, 
              cur_pivot, predicate);

            // check the other pivot candidates for duplicates of this element

            while (i < num_pivots && (
              !(pivots[i-1] < pivots[i]) && !(pivots[i] < pivots[i-1])))
            {
              // we've reached the end of the unknowns, and
              // it's time to copy the pivot.
              std::iter_swap (next_greater, cur_pivot);

              // we can also copy the element next to the pivot,
              // because we know it is less than the element in pivot2
              std::iter_swap (next_greater+1, cur_pivot+1);

              // update the pivot entry (we need to know where each
              // pivot took place)
              pivots[i] = next_greater;

              // update the next greater index
              next_start = next_greater + 2;

              i++;
              cur_pivot = pivots[i];
              next_greater = next_start;
            }

            i--;
          }
          else
          {
            // this is either the first pivot we look at, or this pivot does
            // not equal the last pivot, so we can look for what is smaller
            // than cur_pivot
            std::_pivot_smaller_left (next_start, next_greater,
              cur_pivot, predicate);

            // we've reached the end of the unknowns, and
            // it's time to copy the pivot.
            std::iter_swap (next_greater, cur_pivot);

            // we can also copy the element next to the pivot,
            // because we know it is less than the element in pivot2
            std::iter_swap (next_greater+1, cur_pivot+1);

            // update the pivot entry (we need to know where each pivot took place)
            pivots[i] = next_greater;

            // we've pivoted, now sort the sublist
            if (next_start == first && pivots[i] > next_start + 1)
              std::_pivotsort (next_start, pivots[i], num_pivots, predicate);
            if (next_start != first && pivots[i] > pivots[i-1] + 2)
              std::_pivotsort (pivots[i-1]+1, pivots[i], num_pivots, predicate);

            // update the next greater starting index
            next_start = next_greater + 2;
          }
        }

        // sort the array elements between last pivot and last element of array 
        if (last > pivots[num_pivots-1] + 1)
        {
          int oldnum_pivots = num_pivots;
          //if (last == pivots[num_pivots-1] + 1
          //  || pivots[num_pivots-1] < first + ( num_pivots * 2 ) + 1)
          //{
          //  // change the num_pivots if we had a really bad run
          //  num_pivots = _change_num_pivots();
          //}

          std::_pivotsort (pivots[oldnum_pivots-1]+1, last,
            num_pivots, predicate);
        }
      }
      else
      {
        // the array range is very small so we should just
        // defer to an algorithm that handles small arrays best.
        std::insertsort (first, last, predicate);
		    //std::make_heap (first, last, predicate);
		    //std::sort_heap (first, last, predicate);
      }
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