// sort_fun.h
#include <stdlib.h>
#include <string.h>
#include <vector>
#ifndef __PDIS_DEF__
#define __PDIS_DEF__
#include "PointDis_def.h" // data structures for rough searching
#endif
using namespace std;

// For sort()_function //
bool compare_patch_distance_asc(result_distance_t A, result_distance_t B);
bool compare_patch_tile_ID_asc(result_distance_t A, result_distance_t B);
void bubblesort_patch_distance_asc(vector<result_distance_t> &list);
void insertionsort_patch_distance_asc(vector<result_distance_t> &list);
void bubblesort_tile_ID_asc(vector<result_distance_t> &list);
