
#ifndef _MEANSHIFT_FUN_
#define _MEANSHIFT_FUN_

#include <vector>
#include <math.h>
using namespace std;

#ifndef __PDIS_DEF__
#define __PDIS_DEF__
#include "../cbir_common/PointDis_def.h"
#endif


#endif


int g_fun(float x);

float norm_fun(float x1, float x2);

void UniqueIndex( vector<result_distance_t>& r2,
                 vector<int>& iImg_unique,
                 int can_num );

void Meanshift_fun(vector<result_distance_t>& result2,
                   vector<result_distance_t>&  pNew,
                   int BandWidth );

void Meanshift_cluster(vector<result_distance_t>& pNew,
                       vector<result_distance_t>& r2,
                       vector<int>& iImg_unique,
                       int can_num,
                       int BandWidth );

void Meanshift_fun_detail(vector<result_distance_t>& result2,
                          int BandWidth,
                          vector<result_distance_t>& pNew,
                          vector<int>& pNew_num,
                          vector<int>& pNew_index );





