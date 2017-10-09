
#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

#ifndef __PDIS_DEF__
#define __PDIS_DEF__
  #include "PointDis_def.h"
#endif


float HistDistance(Mat * HistA, Mat * HistB, int KBINS );
float HistDistance_Seg(Mat * HistA, Mat * HistB, int KBINS );
float HistDistance_tex( float* HistA_1d_ptr, float* HistB_1d_ptr, int KBINS, int D, int L );





