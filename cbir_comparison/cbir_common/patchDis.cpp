
#include "patchDis.h"


float HistDistance(Mat * HistA, Mat * HistB , int KBINS){
   
    float *Weight = (float*)malloc( KBINS*sizeof(float) );
    
    for(int i=1; i<=KBINS; i++ ){
        Weight[KBINS-i] = i * 1.0/KBINS;
    }
    
    float dis_fro = 0;
    float dis_temp = 0;
  
    for(int iv=0; iv<KBINS; iv++ ){
        dis_temp = 0;
        
        for(int iChn=0; iChn<3; iChn++){
            for(int ib=0; ib<256; ib++){
               
                dis_temp += pow( Weight[iv] * abs( HistA[3*iv+iChn].at<float>(ib) - HistB[3*iv+iChn].at<float>(ib) ), 2);
            }
        }
        dis_fro += dis_temp;
    }// iKbin
    
    dis_fro = sqrt(dis_fro);
    
    free(Weight);
    return dis_fro;
}


float HistDistance_Seg(Mat * HistA, Mat * HistB, int KBINS ){
    
    float *Weight = (float*)malloc( KBINS*sizeof(float) );
    for(int i=1; i<=KBINS; i++ ){
        Weight[KBINS-i] = i * 1.0/KBINS;
    }
    
    
    double dis_fro = 0;
    double dis_temp = 0;
    
    for(int iSeg=0; iSeg<8; iSeg++){
        for(int iKbin=0; iKbin<KBINS; iKbin++){
            dis_temp = 0;
            for(int iChn=0; iChn<3; iChn++){
                for(int ib=0; ib<256; ib++){
                    dis_temp += pow( Weight[iKbin] * abs( HistA[iSeg*3*KBINS+iKbin*3+iChn].at<float>(ib) - HistB[iSeg*3*KBINS+iKbin*3+iChn].at<float>(ib) ), 2);
                }
            }//iChn
            dis_fro += dis_temp;
        }// ikbin
    }//iseg
    
    dis_fro = sqrt(dis_fro);
    
    free(Weight);
    return dis_fro;
}

float HistDistance_tex( float* HistA_1d_ptr, float* HistB_1d_ptr, int KBINS, int D, int L ){
   
    float *weight = (float *) malloc( KBINS * sizeof(float) );
    for(int i = 1; i <= KBINS; i++ ){
        weight[KBINS-i] = i * 1.0/KBINS;
    }
    
    float dis_fro = 0;
    float dis_temp = 0;
    for(int iv = 0; iv < KBINS; iv++ ){
        dis_temp = 0;
        for(int iChn = 0; iChn < 3; iChn++){
            for(int ib = 0; ib < L ; ib++){
               
                dis_temp += pow( weight[iv] * abs( HistA_1d_ptr[(3*iv+iChn)*L+ib] - HistB_1d_ptr[(3*iv+iChn)*L+ib] ), 2);
            }
        }
        dis_fro += dis_temp;
    }// iKbin
    dis_fro = sqrt(dis_fro);
    free(weight);
    return dis_fro;
}//END HistDistance_tex()





