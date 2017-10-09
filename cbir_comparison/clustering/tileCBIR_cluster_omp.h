// tileCBIR_cluster_omp.h
// ... 

#ifndef __STD__
#define __STD__
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <omp.h>
using namespace std;
#include <sys/time.h> // timing
#include <unistd.h>
#endif

#ifndef __CV__
#define __CV__
#include <opencv2/opencv.hpp>
using namespace cv;
#endif

#ifndef __PDIS_DEF__
#define __PDIS_DEF__
#include "../cbir_common/PointDis_def.h" 
#endif

#include "Meanshift_fun.h"

void tileCBIR_cluster_omp( vector<vector<result_distance_t> > vectors,
                          int HeightImg,
                          int WidthImg,
                          float Beta,
                          vector<result_distance_t> &results);