#ifndef SIFT_H
#define SIFT_H

#include <vector>
using namespace std;

void buildMinHeap(vector<int>& array, int first, int last , __int64& numMoves, __int64& numCompares);
// used for prepping an array for sorting.  requires O(n).  Not as good as sift().

void sift(vector<int>& array, unsigned size, __int64 &numMoves, __int64 &numCompares);
// used for prepping an array for sorting.  requires O(n).

void advSift(vector<int>& array, unsigned size, __int64 &numMoves, __int64 &numCompares);
// used for prepping an array for sorting. advanced version of Sift Algorithm.  requires O(n).

#endif