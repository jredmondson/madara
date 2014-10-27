#ifndef SIFT_CPP
#define SIFT_CPP

#include <iostream>
#include <vector>
#include "Sift.h"
using namespace std;

/********************************************************************
// author: James Edmondson
// date: 3/7/2005
// purpose: contains functions for implementing the Sift Algorithm
// functions: sift(vector<> array, unsigned size)
//            buildMinHeap(vector<> array, int first, int last)
//            siftAscending(vector<> array, int first, int last)
//            buildReverseMaxHeap(vector<> array, int first, int last)
//*******************************************************************/

/********************************************************************
* function: swap (local)
* precondition: x and y should be initialized for with the class constructor.
* postcondition: x and y will be exchanged.
*********************************************************************/

template< class T >
void swap(T& x, T& y, __int64& numMoves)
{
	T temp = x;
	x = y;
	y = temp;

	// add three to the number of swaps

	numMoves+=3;

	return;
}

/********************************************************************
* function: buildMinHeap (local)
* precondition: first and last should be valid locations in array
* postcondition: array will be arranged into a MinHeap. Takes O(n).
*********************************************************************/

void buildMinHeap(vector<int>& array, int first, int last , __int64& numMoves, __int64& numCompares)
{
	int size = last - first + 1;
	int largerChildPosition, position;
	for( int i = (size >> 1) - 1; i >= 0; i-- )
	{
		// loop indefinitely until we establish a heap
		position = i;
		while( 1 )
		{
			largerChildPosition = (position << 1) + 1;  // position * 2 + 1

			// the parent is childless, get out

			if( largerChildPosition > size - 1 ) break;

			// if largerChild position is less than last (if it were equal the second part
			// couldn't be evaluated)and the rightmost child is actually larger than the 
			// leftmost child, point largestChild to the rightmost child position

			++numCompares;
			if( largerChildPosition < size - 1 && array[first+largerChildPosition + 1] < array[first+largerChildPosition] )
				largerChildPosition = largerChildPosition + 1;

			// if we've already got a heap for this level and side, get out

			++numCompares;
			if( array[first+position] < array[first+largerChildPosition] ) break;

			// otherwise swap the larger child with the currently smaller parent
			// to form a heap

			swap(array[first+position],array[first+largerChildPosition],numMoves);
			position = largerChildPosition;
		}
	}

	return;
}

template < class T >
void siftAscending(vector<T>& array, int first, int last, __int64& numMoves, __int64& numCompares)
{
	int size = last - first + 1;
	int middle = size / 2;
	int quarter = middle / 2;
	int left = first + 1;
	int right = size - 1;

	for( ; left < right; left++, right-- )
	{
		if( array[right] < array[left] )
		{
			swap(array[right],array[left],numMoves);
		}
	}

	left = first + 1;
	right = quarter;

	for( ; right <= size - 1; left++, right++ )
	{
		if( array[right] < array[left] )
		{
			swap(array[right],array[left],numMoves);
		}
	}
}

/********************************************************************
* function: buildReverseMaxHeap (local)
* precondition: first and last should be valid locations in array
* postcondition: array will be arranged into a Reverse Maxheap. Takes O(n).
*********************************************************************/

template <class T>
void buildReverseMaxHeap(vector<T>& array, int first, int last , __int64& numMoves, __int64& numCompares)
{
	int size = last - first + 1;
	int largerChildPosition, position;
	for( int i = (size >> 1) - 1; i >= 0; i-- )
	{
		// loop indefinitely until we establish a heap
		position = i;
		while( 1 )
		{
			largerChildPosition = (position << 1) + 1;  // position * 2 + 1

			// the parent is childless, get out

			if( largerChildPosition > size - 1 ) break;

			// if largerChild position is less than last (if it were equal the second part
			// couldn't be evaluated)and the rightmost child is actually larger than the 
			// leftmost child, point largestChild to the rightmost child position

			++numCompares;
			if( largerChildPosition < size - 1 && array[last - (largerChildPosition + 1)] > array[last-largerChildPosition] )
				largerChildPosition = largerChildPosition + 1;

			// if we've already got a heap for this level and side, get out

			++numCompares;
			if( array[last-largerChildPosition] > array[last-position] )
				swap(array[last-position],array[last-largerChildPosition],numMoves);
			else break;

			// otherwise swap the larger child with the currently smaller parent
			// to form a heap

			position = largerChildPosition;
		}
	}

	return;
}

/********************************************************************
* function: sift (global)
* precondition: array and size should be valid.
* postcondition: array will be arranged with the Sift Algorithm. Takes O(n).
*********************************************************************/

void sift(vector<int>& array, unsigned size,
				__int64& numMoves, __int64& numCompares)
{
	buildMinHeap(array,0,size-1,numMoves,numCompares);
	siftAscending(array,0,size-1,numMoves,numCompares);
	buildReverseMaxHeap(array,1,size-1,numMoves,numCompares);
	return;
}

/********************************************************************
* function: sift (global)
* precondition: array and size should be valid.
* postcondition: array will be arranged with the Sift Algorithm. Takes O(n).
*********************************************************************/

void advSift(vector<int>& array, unsigned size,
				__int64& numMoves, __int64& numCompares)
{
	buildMinHeap(array,0,size-1,numMoves,numCompares);
	buildReverseMaxHeap(array,1,size-1,numMoves,numCompares);
	if( size > 50 )
	{
		unsigned middle = size >> 1;
		unsigned quarter = size >> 2;
		unsigned eighth = size >> 3;
		unsigned sixteenth = size >> 4;
		unsigned left = 0, right = 0;

		buildMinHeap(array,sixteenth,size-sixteenth-1,numMoves,numCompares);
		buildReverseMaxHeap(array,sixteenth,size-sixteenth-1,numMoves,numCompares);

		buildMinHeap(array,eighth,size-sixteenth-1,numMoves,numCompares);
		buildReverseMaxHeap(array,sixteenth,size-eighth-1,numMoves,numCompares);

		buildMinHeap(array,quarter,size-quarter-1,numMoves,numCompares);
		buildReverseMaxHeap(array,quarter,size-quarter-1,numMoves,numCompares);

		buildMinHeap(array,1,middle,numMoves,numCompares);
		buildReverseMaxHeap(array,middle-1,size-2,numMoves,numCompares);
		
		if( size > 500 )
		{
			for( left = 0, right = sixteenth; right <= size - 1; left+= sixteenth, right+= sixteenth )
			{
				buildMinHeap(array,left,right,numMoves,numCompares);
				buildReverseMaxHeap(array,left,right,numMoves,numCompares);
			}
			if( left < size - 1 )
			{
				buildMinHeap(array,left,size-1,numMoves,numCompares);
				buildReverseMaxHeap(array,left,size-1,numMoves,numCompares);
			}
		}

	}
	return;
}

#endif