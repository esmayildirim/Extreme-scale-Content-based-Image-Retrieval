
#include "module_roughSearching_2.h"


void para_feaDis_Omp_F_IM(result_distance_t * imgDis_temp ,
                          Mat * QueryHist,
                          Mat dataImg,
                          int iPatX,
                          int iPatY,
                          int WidthImg,
                          int HeightImg,
                          int KBINS,
                          int Index_FEA,
                          int tile_ID,
                          char * file_name,
                          int file_index){
    
    
    
    Mat * dataImgHist = new Mat[KBINS*3];
//   Mat dataImgHist[KBINS*3]; 
       for(int i = 0 ; i < KBINS * 3;i++)
         dataImgHist[i].create( 1, 256, CV_32FC1 ); 
    // calcultate data image patch histogram
    hah_module( dataImg, dataImgHist, KBINS, 256, 256,
               iPatX, iPatY, WidthImg, HeightImg, Index_FEA );
    
    // calculate dis
    float dis = HistDistance( QueryHist, dataImgHist, KBINS );
   
    // save result distance
    imgDis_temp->iPatX = iPatX;
    imgDis_temp->iPatY = iPatY;
    imgDis_temp->dis = dis;
    imgDis_temp->tile_ID = tile_ID;
    memcpy(imgDis_temp->file_name, file_name,200);
    imgDis_temp -> file_index = file_index;
    
    for(int i=0; i<KBINS*3;i++)
        dataImgHist[i].release();
    
}//END para_feaDis_Omp_F_IM()
/*
int para_feaDis_tex_Omp_F_IM(struct resultDis& imgDis_temp,
                             float* queryHist_arr_1d,
                             Mat dataImg,
                             int iPatX,
                             int iPatY,
                             int WidthImg,
                             int HeightImg,
                             int KBINS,
                             int Index_FEA,
                             int tile_ID,
                             char * file_name,
                             int file_index,
                             int D,
                             int L )
{
    
    // -->  dataSet COOC_fea
    float * dataHist_arr_1d = new float[3*KBINS*L];
    
    cout <<"[] data.hah_module_tex() start ... "<< endl;
    hah_module_tex( dataImg, dataHist_arr_1d, KBINS, 256, 256, 0, 0,
                   WidthImg, HeightImg, 1, D, L );
    cout <<"[] data.hah_module_tex() end ... "<< endl;
    
    // --> HistDistance_tex()
    cout <<"[] histDis_tex() start ... "<< endl;
    float histDis_tex = HistDistance_tex( queryHist_arr_1d, dataHist_arr_1d, KBINS, D, L );
    cout <<"[] histDis_tex() end ... "<< endl;
    cout <<"[] histDis_tex = "<< histDis_tex << endl;
    
    
    // save result distance
    imgDis_temp.iPatX = iPatX;
    imgDis_temp.iPatY = iPatY;
    imgDis_temp.dis = histDis_tex;
    imgDis_temp.tile_ID = tile_ID;
    memcpy(imgDis_temp->file_name, file_name,200);
    imgDis_temp -> file_index = file_index;
    
    free(dataHist_arr_1d);

}//END para_feaDis_tex_Omp_F_IM()

*/























