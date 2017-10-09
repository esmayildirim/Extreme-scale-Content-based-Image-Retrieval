
#ifndef _FEA_HAH_
#define _FEA_HAH_

#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <math.h>
#include <algorithm> // std::sort
#include <vector>

#endif

using namespace std;
using namespace cv;

#include "glcm_f.h"
void fea_HAH( Mat& Query_rgb_iChn, Mat hist_mask, Mat& hist_temp, int histSize_scalar, int histRange_scalar );

void fea_TEX_COOC( Mat& Query_rgb_iChn, Mat hist_mask, Mat& hist_temp, int histSize_scalar, int histRange_scalar );
int uint8_f(int i);
void hist_f( float* hist, int* data, int data_num, int bin_min, int bin_max );
//void fea_TEX_TFCM( Mat& Query_rgb_iChn, Mat& hist_mask, Mat& hist_temp, int tfcm_D);
void fea_TEX_TFCM( Mat& Query_rgb_iChn, Mat& hist_mask, Mat& hist_temp, int histSize_scalar, int histRange_scalar );

void fea_TEX_LBP( Mat& Query_rgb_iChn, Mat& hist_mask, Mat& hist_temp, int histSize_scalar, int histRange_scalar );

void fea_TEX_CSAC( Mat& Query_rgb_iChn, Mat& hist_mask, Mat& hist_temp, int histSize_scalar, int histRange_scalar );


