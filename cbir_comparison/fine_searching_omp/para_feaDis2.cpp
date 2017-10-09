// para_feaDis2.cpp

#include "para_feaDis2.h"

int para_feaDis2_Omp_F( result_distance_t& r2_temp,
                       Mat * queryHist_seg,
                       Mat* SegHistMask,
                       Mat dataImg,
                       int iPatX,
                       int iPatY,
                       int WidthImg,
                       int HeightImg,
                       int KBINS,
                       float OLP )
{
    

    float dis;
    dis = cbir_seg(dataImg, SegHistMask, iPatX, iPatY, queryHist_seg, WidthImg, HeightImg, KBINS, OLP);
   

    r2_temp.iPatX = iPatX;
    r2_temp.iPatY = iPatY;
    r2_temp.dis = dis;

    return 1;
}// END para_feaDis2_Omp_F()


