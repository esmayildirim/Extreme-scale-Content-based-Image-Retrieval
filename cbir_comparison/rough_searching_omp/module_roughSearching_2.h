// module_roughSearching.h

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

#ifndef __PROCESS__
#define __PROCESS__
#include "hah_module.h" // rough-searching
#include "../cbir_common/patchDis.h"
#endif

void para_feaDis_Omp_F_IM( result_distance_t *imgDis_temp ,
                          Mat *QueryHist,
                          Mat dataImg,
                          int iPatX,
                          int iPatY,
                          int WidthImg,
                          int HeightImg,
                          int KBINS,
                          int Index_FEA,
                          int tile_ID,
                          char * file_name,
                          int file_index);





















