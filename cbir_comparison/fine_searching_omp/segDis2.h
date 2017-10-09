// 2013-5-12 A

#ifndef _SEGDIS2_
#define _SEGDIS2_

#include <iostream>
#include <opencv2/opencv.hpp>
#include <stdio.h>
#include <fstream>
#include <cmath>
#include <omp.h>
#include <math.h>

#endif

using namespace std;
using namespace cv;

#include "../cbir_common/patchDis.h"



int Seg2_IsSection( int ii, int jj, int WidthImg, int HeightImg );

void segQueryHist( Mat& QueryImg, Mat * sQueryHist , Mat* SegHistMask, int KBINS );

// KBIN
float cbir_seg(Mat dataImg,
               Mat* SegHistMask,
               int iPatX,
               int iPatY,
               Mat * QueryHist,
               int WidthImg,
               int HeightImg,
               int KBINS,
               float OLP );
// KBIN, OLP
int buildFan( Mat* SegHistMask, int WidthImg, int HeightImg );





