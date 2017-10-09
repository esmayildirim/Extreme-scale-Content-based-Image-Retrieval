// para_feaDis2.h

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
    
#ifndef __PROCESS__
#define __PROCESS__
/*
  #include "seedDetection.h" // pre-processing seedDetection
  #include "datasetfile_readin_module.h"
*/
//  #include "hah_module.h" // rough-searching
  #include "../cbir_common/patchDis.h"
  #include "segDis2.h" // fine-searching
///  #include "Meanshift_fun.h" // meanshift-clustering

//  #include "bwareaOpen.h"
//  #include "sort_fun.h"
//  #include "resultStore.h"
#endif


int para_feaDis2_Omp_F( result_distance_t& r2_temp, Mat * queryHist_seg, Mat* SegHistMask,
                       ///			vector<Mat>& dataImg_v, int iImg,
                       Mat dataImg,
                       int iPatX, int iPatY, int WidthImg, int HeightImg, int KBINS, float OLP );
			
			
