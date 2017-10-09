// tileCBIR_roughS_omp.cpp
//
// Todo: OMP version of CBIR roughSearching
//

#include "tileCBIR_fineS_omp.h"

int tileCBIR_fineS_omp( vector<result_distance_t>& disPatch2_ptr,
                       long res_num,
                       result_distance_t* imgDis,
                       Mat & DataImg,
                       int tile_imH,
                       int tile_imW,
                       Mat & QueryImg,
                       int query_imH,
                       int query_imW,
                       int iImg_id,
                       float OLP,
                       int KBINS,
                       int Index_FEA )
{
    
    
    // -2. QueryHist
    int WidthImg, HeightImg;
    WidthImg = QueryImg.size().width;
    HeightImg = QueryImg.size().height;
    
    int HeightDataImg, WidthDataImg;
    HeightDataImg = DataImg.size().height;
    WidthDataImg = DataImg.size().width;
    
    
    // 3.0 Build fan mask
    Mat * queryHist_seg = new Mat[3*KBINS*8];
    for(int i=0; i< KBINS * 8 * 3; i++)
        queryHist_seg[i].create( 1, 256, CV_32FC1 );
    
    Mat SegHistMask[8];
    buildFan( SegHistMask, WidthImg, HeightImg );
    
    segQueryHist(QueryImg, queryHist_seg, SegHistMask, KBINS);
    
    
    int step2_can_num = res_num;
    
    omp_set_dynamic(0);
#pragma omp parallel for num_threads(3)
    for(int is2=0; is2< step2_can_num; is2++ ){
        result_distance_t r2_temp;
        para_feaDis2_Omp_F(r2_temp,
                           queryHist_seg,
                           SegHistMask,
                           DataImg,
                           imgDis[is2].iPatX,
                           imgDis[is2].iPatY,
                           WidthImg,
                           HeightImg,
                           KBINS,
                           OLP );
        
        r2_temp.tile_ID = imgDis[is2].tile_ID;
        r2_temp.tile_x = imgDis[is2].tile_x;
        r2_temp.tile_y = imgDis[is2].tile_y;
        r2_temp.tile_width = imgDis[is2].tile_width;
        r2_temp.tile_height = imgDis[is2].tile_height;
        memcpy( r2_temp.file_name, imgDis[is2].file_name, 200 );
        r2_temp.file_index = imgDis[is2].file_index;
#pragma omp critical
        {
            disPatch2_ptr.push_back(r2_temp);
        }//END critical
    }//END_FOR_is2
    
    for(int i=0; i< KBINS * 8 * 3; i++)
        queryHist_seg[i].release();
    for(int i=0; i<8; i++)
        SegHistMask[i].release();
    
    return 0;
    
}//END tileCBIR_omp()

















