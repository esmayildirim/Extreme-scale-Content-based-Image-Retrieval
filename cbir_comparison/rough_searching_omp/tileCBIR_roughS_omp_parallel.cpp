// tileCBIR_roughS_omp.cpp
//
// Todo: OMP version of CBIR roughSearching
//

#ifndef __CV__
#define __CV__
#endif
#include "tileCBIR_roughS_omp.h"
#include <opencv2/opencv.hpp>

using namespace cv;
int tileCBIR_roughS_omp(vector<result_distance_t>& disPatch_v,
                        Mat & DataImg,
                        int tile_imH,
                        int tile_imW,
                        Mat & QueryImg,
                        int query_imH,
                        int query_imW,
                        int tile_ID,
                        float OLP,
                        int KBINS,
                        int Index_FEA,
                        char * file_name,
                        int file_index)
{
    
    
    // -2. QueryHist
    int WidthImg, HeightImg;
    WidthImg = QueryImg.size().width;
    HeightImg = QueryImg.size().height;
    
    int HeightDataImg, WidthDataImg;
    HeightDataImg = DataImg.size().height;
    WidthDataImg = DataImg.size().width;
    
    
//    Mat query_histograms[KBINS*3];
    Mat * query_histograms = new Mat[KBINS*3];
    for(int i = 0 ; i < KBINS * 3; i++)
         query_histograms[i].create( 1, 256, CV_32FC1 );//3 is number of channels
    omp_set_dynamic(0);    
    hah_module(QueryImg, query_histograms, KBINS, 256, 256, 0, 0, WidthImg, HeightImg, Index_FEA );
    
    
    
    // -3. patch distance - roughS
    float OverlapR = round( 1/(1-OLP) );
    int SlideStep[2] = { round( (WidthImg-1)/OverlapR ), round( (HeightImg-1)/OverlapR ) };
    
    
    
    int iPatX,iPatY;
    iPatX = iPatY = 0;
    int h_MAX_counter, w_MAX_counter;
    h_MAX_counter = floor(HeightDataImg/(1.0*SlideStep[1]));
    w_MAX_counter = floor(WidthDataImg/(1.0*SlideStep[0]));
    
//    omp_set_dynamic(0);
#pragma omp parallel for num_threads(4)  shared(disPatch_v) private(iPatX,iPatY)
    for(int iPatY_invariant = 0; iPatY_invariant < h_MAX_counter; iPatY_invariant++){
        for(int iPatX_invariant = 0; iPatX_invariant < w_MAX_counter; iPatX_invariant++ ){
            result_distance_t distance_patch_temp;
            iPatY = SlideStep[1]*iPatY_invariant;
            iPatX = SlideStep[0]*iPatX_invariant;
            if( iPatY>=0 && (iPatY + HeightImg)<= HeightDataImg
               && iPatY>=0 && (iPatX + WidthImg)<= WidthDataImg )
            {
                para_feaDis_Omp_F_IM( &distance_patch_temp, query_histograms, DataImg,
                                     iPatX, iPatY, WidthImg, HeightImg,
                                   KBINS, Index_FEA, tile_ID, file_name, file_index );
#pragma omp critical
                {
                    disPatch_v.push_back( distance_patch_temp );
                } //END OF PRAGMA OMP CRITICAL
            }// END_if
        }//END_for_iPatY
    }// END_for_iPatX    
    for(int i = 0; i < KBINS*3; i++)
        query_histograms[i].release();
    
    return 0;
    
}//END tileCBIR_omp()

















