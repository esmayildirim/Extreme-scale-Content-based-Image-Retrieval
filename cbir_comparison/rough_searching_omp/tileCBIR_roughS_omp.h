// tileCBIR_roughS_omp.h

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

#include "hah_module.h" // rough-searching 

#include "module_roughSearching_2.h"


int tileCBIR_roughS_omp( vector<result_distance_t>& disPatch_v,
                        Mat & data_image,
                        int tile_imH,
                        int tile_imW,
                        Mat & query_image,
                        int query_imH,
                        int query_imW,
                        int tile_ID,
                        float OLP,
                        int KBINS,
                        int Index_FEA,
                        char *file_name ,
                        int file_index);
