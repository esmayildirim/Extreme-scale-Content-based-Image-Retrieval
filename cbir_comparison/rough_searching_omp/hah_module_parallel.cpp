

#include <omp.h>
#include "hah_module.h"
#include "../cbir_common/split_fun_c.h"

void hah_module( Mat& QueryImg,
                Mat *QueryHist,
                int KBINS,
                int scalar_histSize,
                int scalar_histRange,
                int iPatX,
                int iPatY,
                int WidthImg,
                int HeightImg,
                int Index_FEA ){
    
    
    Mat Query_rgb[3];
    for (int ic = 0; ic < 3; ic++){
        Query_rgb[ic].create( QueryImg.size().height, QueryImg.size().width, CV_8UC1 );
    }
    split_fun_c( QueryImg, Query_rgb );
    
      
    int histSize = scalar_histSize;
    float range[]={0, scalar_histRange};
    
    const float* histRange = { range };
    bool uniform_hist = true;
    bool accumulate_hist = false;
    
    Mat *hist_mask=new Mat[KBINS];
    for(int i=0; i< KBINS; i++)
        hist_mask[i]= Query_rgb[0].clone();
    
    int rectLx, rectLy, rectRx, rectRy;
    
    
    int WidthBinX = floor( (WidthImg-1)/(2*KBINS) );
    int WidthBinY = floor( (HeightImg-1)/(2*KBINS) );
    
    //Mat *hist_temp=new Mat[KBINS];
    rectLx = iPatX; rectLy = iPatY;
    rectRx = iPatX + WidthImg; rectRy = iPatY + HeightImg;
    Mat hist_temp;
   // omp_set_dynamic(0);
    // PARALLEL BINS
//#pragma omp parallel num_threads(KBINS)
 //   {
        
       //printf("Thread id = %d\n", omp_get_thread_num()); 
#pragma omp parallel for schedule(static) num_threads(KBINS) shared(hist_mask, QueryHist) private(hist_temp)
        for( int iKbin=KBINS; iKbin>0; iKbin-- ){
            
            // Mask ROI
            int outerLx, outerLy, outerRx, outerRy; //NEW
            outerLx = rectLx + (KBINS-iKbin)*WidthBinX; //NEW
            outerLy = rectLy + (KBINS-iKbin)*WidthBinY; //NEW
            outerRx = rectRx - (KBINS-iKbin)*WidthBinX; //NEW
            outerRy = rectRy - (KBINS-iKbin)*WidthBinY; //NEW
            
            hist_mask[iKbin-1]( Rect( 0, 0, QueryImg.size().width, QueryImg.size().height ) )= Scalar::all(0); // clear mask
            hist_mask[iKbin-1]( Rect(outerLx, outerLy, (outerRx-outerLx), (outerRy-outerLy)) )= Scalar::all(1); //NEW CODE
            
            if( iKbin != 1 ){
                
                int innerLx, innerLy, innerRx, innerRy;
                innerLx = outerLx +WidthBinX;
                innerLy = outerLy +WidthBinY;
                innerRx = outerRx -WidthBinX;
                innerRy = outerRy -WidthBinY;
                hist_mask[iKbin-1]( Rect(innerLx, innerLy, (innerRx-innerLx), (innerRy-innerLy) )) = Scalar::all(0); // set inside bound		}
            }
            
            
            for( int iChn=0; iChn<3; iChn++ ){
                hist_temp.create( 1, histSize, CV_32FC1 );
                
                // hist ROI //
                switch (Index_FEA){
                    case 1:
                        fea_TEX_COOC( Query_rgb[iChn], hist_mask[iKbin-1], hist_temp, 256, 256 );
                        break;
                    case 2:
                        fea_TEX_CSAC( Query_rgb[iChn], hist_mask[iKbin-1], hist_temp, 256, 256 );
                        break;
                    case 3:
                        fea_TEX_TFCM( Query_rgb[iChn], hist_mask[iKbin-1], hist_temp, 256, 256 );
                        break;
                    case 4:
                        fea_TEX_LBP( Query_rgb[iChn], hist_mask[iKbin-1], hist_temp, 256, 256 );
                        break;
                    default:
                        fea_HAH( Query_rgb[iChn], hist_mask[iKbin-1], hist_temp, 256, 256 );
                }
                
                
                hist_temp = hist_temp/(sum(hist_temp)[0]); // Hist Nomalize
//                #pragma omp critical 
               {
                hist_temp.copyTo(QueryHist[(iKbin-1)*3+iChn]);
               } 
               hist_temp.release();
                
            }//END_For_iChn
            hist_mask[iKbin-1].release();
        }//END_For_iKbin  
 //   }//END FOR PRAGMA OMP PARALLEL
    
}// END_hah()





