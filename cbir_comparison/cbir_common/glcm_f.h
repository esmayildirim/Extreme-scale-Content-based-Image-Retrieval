// glcm_f.h
// 12-16-2013 A.

#include <iostream>
#include <stdio.h>
using namespace std;

#include <opencv2/opencv.hpp>
using namespace cv;

void glcm_f( Mat& im_SI, Mat* im_glcm, Mat& Query_rgb_iChn, int cooc_L, int cooc_D );
