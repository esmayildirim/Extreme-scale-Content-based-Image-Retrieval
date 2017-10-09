
#include "segDis2.h"
#include "../cbir_common/fea_HAH.h"

///#include "split_fun.h"
#include "../cbir_common/split_fun_c.h"



// Fun_1
int Seg2_IsSection( int ii, int jj, int WidthImg, int HeightImg ){
    
    int secNum;
    if( jj < round( (float)HeightImg/2) ){ // sec0 - sec3
        if( ii < round((float)WidthImg/2) ){ // sec0, sec1
            if( jj < (float)HeightImg/WidthImg * ii )
                secNum=1;
            else
                secNum=0;
        }
        else{ // sec2, sec3
            if( jj < -(float)HeightImg/WidthImg * ii + (float)HeightImg )
                secNum=2;
            else
                secNum=3;
        }
    }
    else{// sec4 - sec7
        if( ii < round((float)WidthImg/2) ){ // sec4,5
            if( jj < -(float)HeightImg/WidthImg * ii + (float)HeightImg )
                secNum=4;
            else
                secNum=5;
        }
        else{ // sec6,7
            if( jj < (float)HeightImg/WidthImg * ii )
                secNum=7;
            else
                secNum=6;
        }
    }
    
    return secNum;
    
}//END_Seg2_IsSection()



// Fun_2

void segQueryHist( Mat& QueryImg, Mat * sQueryHist , Mat* SegHistMask, int KBINS ){
    // split into RGB planes
    int WidthImg = QueryImg.size().width;
    int HeightImg = QueryImg.size().height;
    
    Mat Query_rgb[3];
    for (int ic = 0; ic < 3; ic++){
        Query_rgb[ic].create( QueryImg.size().height, QueryImg.size().width, CV_8UC1 );
    }
    split_fun_c( QueryImg, Query_rgb );
    
    
    // cal hist
    Mat r_hist, g_hist, b_hist;
    int histSize = 256;
    float range[]={0, 256};
    const float* histRange = { range };
    bool uniform_hist = true;
    bool accumulate_hist = false;
    
    Mat hist_mask = Query_rgb[0].clone();
    const CvMat cvhist_mask = hist_mask;
    
    int rectLx, rectLy, rectRx, rectRy;
    fea_HAH( Query_rgb[0], hist_mask, r_hist, 256, 256 );
    fea_HAH( Query_rgb[1], hist_mask, g_hist, 256, 256 );
    fea_HAH( Query_rgb[2], hist_mask, b_hist, 256, 256 );
    
    Scalar hist_sum;
    hist_sum = sum( r_hist );
    
    // uniformation hist
    r_hist = r_hist/(QueryImg.size().width * QueryImg.size().height );
    
    Scalar r_sum = sum( r_hist );
    
    int WidthBinX = floor( (WidthImg-1)/(2*KBINS) );
    int WidthBinY = floor( (HeightImg-1)/(2*KBINS) );
    
    
    Mat hist_temp;
    hist_temp.create( 1, histSize, CV_32FC1 );
    
    rectLx=0; rectLy=0; rectRx=WidthImg; rectRy=HeightImg;
    
    
    Mat hist_temp_kbin;
    hist_temp_kbin.create( 1, 3 * histSize, CV_32FC1 );
    
    
    Mat QueryImgshow = QueryImg.clone();
    
    Mat hist_mask_seg = Query_rgb[0].clone();
    Mat hist_mask_and = Query_rgb[0].clone();
    Mat hist_mask_final = Query_rgb[0].clone();
    
    // const CvMat cvhist_mask_seg = hist_mask_seg;
    const CvMat cvhist_mask_and = hist_mask_and;
    CvMat cvhist_mask_final = hist_mask_final;
    
    // Polygon array
    int WidthSegA = round( WidthImg/2 );
    int HeightSegA = round( HeightImg/2 );
    
    // Hist Calculation
    int histSum=0;
    for(int iSeg=0; iSeg<8; iSeg++){
        
        rectLx=0; rectLy=0; rectRx=WidthImg; rectRy=HeightImg;
        
        for( int iKbin=KBINS; iKbin>0; iKbin-- ){
            
            
            unsigned char* p_hist_temp_kbin = hist_temp_kbin.data;
            
            // Mask ROI
            hist_mask( Rect( 0, 0, WidthImg, HeightImg) )= Scalar::all(0); // clear mask
            hist_mask( Rect(rectLx, rectLy, (rectRx-rectLx), (rectRy-rectLy)) )= Scalar::all(255); // set outside bound
            
            
            
            if( iKbin != 1 ){
                rectLx += WidthBinX; rectLy += WidthBinY;
                rectRx -= WidthBinX; rectRy -= WidthBinY;
                hist_mask( Rect(rectLx, rectLy, (rectRx-rectLx), (rectRy-rectLy) )) = Scalar::all(0); // set inside bound
            }
            
            
            
            hist_mask_seg( Rect( 0, 0, hist_mask_and.size().width, hist_mask_and.size().height ) ) = Scalar::all(0);
            hist_mask_and( Rect( 0, 0, hist_mask_and.size().width, hist_mask_and.size().height ) ) = Scalar::all(255);
            hist_mask_final( Rect( 0, 0, hist_mask_and.size().width, hist_mask_and.size().height ) ) = Scalar::all(0);
            
            CvMat cvhist_mask_seg = SegHistMask[iSeg];
            cvAnd( &cvhist_mask, &cvhist_mask_seg, &cvhist_mask_final, &cvhist_mask_and );
            
            
            for( int iChn=0; iChn<3; iChn++ ){
                hist_temp.create( 1, histSize, CV_32FC1 );
                fea_HAH( Query_rgb[iChn], hist_mask_final, hist_temp, 256, 256 );
                
                if(iChn==0 && iSeg==0)
                    histSum += sum(hist_temp)[0];
                
                
                hist_temp = hist_temp/(sum(hist_temp)[0]); // Hist Nomalize
                
                hist_temp.copyTo(sQueryHist[iSeg * 3 * KBINS + (iKbin-1)*3 + iChn]); // vector_num = 3 * Kbins
                hist_temp.release();
            }//END_For_iChn
            
        }//END_For_iKbin
        
    }//END_For_iSeg
    
    hist_mask_seg.release();
    hist_mask_and.release();
    hist_mask_final.release();
    hist_temp_kbin.release();
    ////-- Hist result check
    
}// END_segQueryHist()




// fun_3
float cbir_seg( Mat dataImg,
               Mat* SegHistMask,
               int iPatX,
               int iPatY,
               Mat * QueryHist,
               int WidthImg,
               int HeightImg,
               int KBINS,
               float OLP )
{
    
    
    float OverlapR = round( 1/(1-OLP) );
    int SlideStep[2] = { round( (WidthImg-1)/OverlapR ), round( (HeightImg-1)/OverlapR ) };
    int histSize = 256;
    float range[]={0, 256};
    const float* histRange = { range };
    bool uniform_hist = true;
    bool accumulate_hist = false;
    
    int WidthBinX = floor( (WidthImg-1)/(2*KBINS) );
    int WidthBinY = floor( (HeightImg-1)/(2*KBINS) );
    
    int WidthSegA = round( WidthImg/2 );
    int HeightSegA = round( HeightImg/2 );
    
    //Mat arrays to hold 3-channels
    Mat dataImg_rgb[3];
    for (int ic = 0; ic < 3; ic++){
        dataImg_rgb[ic].create( dataImg.size().width, dataImg.size().height, CV_8UC1 );
    }
    split( dataImg, dataImg_rgb );//split the image into 3 channels
    
    
    //vector<Mat> dataImgHist;//needs to be an array
    ///change it into an array
    Mat *dataImgHist = new Mat[KBINS * 8 * 3]; //segment and KBINS and channel size
    Mat *hist_temp = new Mat[8];
    for(int i=0; i< KBINS * 8 * 3; i++)
        dataImgHist[i].create( 1, histSize, CV_32FC1 );
    
    
    
    
    
    // Polygon array
    Point triPointsTemp[1][3];
    Point triPoints[8][3];
    triPoints[0][0] = Point( WidthSegA-1+1, HeightSegA-1 );
    triPoints[0][1] = Point( WidthSegA-1+1, 0 );
    triPoints[0][2] = Point( WidthImg-1, 0 );
    
    triPoints[1][0] = Point( WidthSegA-1+1+1, HeightSegA-1 );
    triPoints[1][1] = Point( WidthImg-1, 0+1 );
    triPoints[1][2] = Point( WidthImg-1, HeightSegA-1 );
    
    triPoints[2][0] = Point( WidthSegA-1+1, HeightSegA-1+1 );
    triPoints[2][1] = Point( WidthImg-1, HeightSegA-1+1 );
    triPoints[2][2] = Point( WidthImg-1, HeightImg-1 );
    
    triPoints[3][0] = Point( WidthSegA-1+1, HeightSegA-1+1+1 );
    triPoints[3][1] = Point( WidthImg-1-1, HeightImg-1 );
    triPoints[3][2] = Point( WidthSegA-1+1, HeightImg-1 );
    
    triPoints[4][0] = Point( WidthSegA-1, HeightSegA-1+1 );
    triPoints[4][1] = Point( WidthSegA-1, HeightImg-1 );
    triPoints[4][2] = Point( 0, HeightImg-1 );
    
    triPoints[5][0] = Point( WidthSegA-1-1, HeightSegA-1+1+1 );
    triPoints[5][1] = Point( 0, HeightImg-1-1 );
    triPoints[5][2] = Point( 0, HeightSegA-1+1 );
    
    triPoints[6][0] = Point( WidthSegA-1, HeightSegA-1 );
    triPoints[6][1] = Point( 0, HeightSegA-1 );
    triPoints[6][2] = Point( 0, 0 );
    
    triPoints[7][0] = Point( WidthSegA-1, HeightSegA-1-1 );
    triPoints[7][1] = Point( 0+1, 0 );
    triPoints[7][2] = Point( WidthSegA-1, 0 );
    
    
    
    //patchDis tempDis;
    //vector<patchDis> seedDis;
    
    int xpatnum=0; int ypatnum=0;
    int patchCal=0;
    
    
    int rectALx, rectALy, rectARx, rectARy;
    //  int rectBLx, rectBLy, rectBRx, rectBRy;
    
    
    
    
    int histSum=0;
    int allSum=0;
#pragma omp parallel for num_threads(8) schedule(static) private(rectALx, rectALy, rectARx, rectARy)
    for(int iSeg=0; iSeg<8; iSeg++)
    {
        
        rectALx = iPatX; rectARx = iPatX + WidthImg ;
        rectALy = iPatY; rectARy = iPatY + HeightImg ;
        //they have to be private
        Mat hist_mask_data = dataImg_rgb[0].clone();
        Mat hist_mask_seg = dataImg_rgb[0].clone();
        Mat hist_mask_and = dataImg_rgb[0].clone();
        Mat hist_mask_final = dataImg_rgb[0].clone();
        
        const CvMat cvhist_mask_data = hist_mask_data;
        const CvMat cvhist_mask_seg = hist_mask_seg;
        const CvMat cvhist_mask_and = hist_mask_and;
        CvMat cvhist_mask_final = hist_mask_final;
        
        
        // triPointsTemp
        for(int iP=0; iP<3; iP++){
            triPointsTemp[0][iP]=Point( triPoints[iSeg][iP].x+iPatX, triPoints[iSeg][iP].y+iPatY );
        }
        
        const Point* ppt[1] = {triPointsTemp[0]};
        const int npt=3;
        
        hist_mask_seg( Rect( 0, 0, hist_mask_and.size().width, hist_mask_and.size().height ) ) = Scalar::all(0);
        
        
        
        for( int iKbin =KBINS; iKbin>0; iKbin-- ){
            // Mask ROI
            hist_mask_data( Rect( 0, 0, dataImg.size().width, dataImg.size().height ) )= Scalar::all(0); // clear mask
            hist_mask_data( Rect(rectALx, rectALy, (rectARx-rectALx), (rectARy-rectALy) ) )= Scalar::all(255); // set outside bound
            
            if(iKbin != 1){
                rectALx += WidthBinX; rectALy += WidthBinY;
                rectARx -= WidthBinX; rectARy -= WidthBinY;
                hist_mask_data( Rect(rectALx, rectALy, (rectARx-rectALx), (rectARy-rectALy) )) = Scalar::all(0); // set inside bound
            }
            
            hist_mask_and( Rect( 0, 0, hist_mask_and.size().width, hist_mask_and.size().height ) ) = Scalar::all(255);
            hist_mask_final( Rect( 0, 0, hist_mask_and.size().width, hist_mask_and.size().height ) ) = Scalar::all(0);
            
            for(int iEleY = iPatY; iEleY< iPatY+HeightImg; iEleY++ ){
                for(int iEleX = iPatX; iEleX< iPatX+WidthImg; iEleX++ ){
                    if( iEleY< dataImg.size().height && iEleX< dataImg.size().width ){
                        hist_mask_seg.at<unsigned char>(iEleY, iEleX) = SegHistMask[iSeg].at<unsigned char>(iEleY-iPatY, iEleX-iPatX);
                    }
                }//END_For_iEleX
                
            }//END_For_iEleY
            
            cvAnd( &cvhist_mask_data, &cvhist_mask_seg, &cvhist_mask_final, NULL );
            
            for( int iChn=0; iChn<3; iChn++ ){
                hist_temp[iSeg].create( 1, histSize, CV_32FC1 );
                fea_HAH( dataImg_rgb[iChn], hist_mask_final, hist_temp[iSeg], 256, 256 );
                if(iChn==0)
                    histSum += sum(hist_temp[iSeg])[0];
                hist_temp[iSeg] = hist_temp[iSeg]/(sum(hist_temp[iSeg])[0]);
                
                //dataImgHist.push_back( hist_temp );
                hist_temp[iSeg].copyTo(dataImgHist[iSeg * 3 * KBINS + (iKbin-1)*3 + iChn]);
                hist_temp[iSeg].release();
                
            }
            
        }//END_For_iKbin
        hist_mask_data.release();
        hist_mask_seg.release();
        hist_mask_and.release();
        hist_mask_final.release();
        
        
    }//END_For_iSeg
    
    
    
    float dis = HistDistance_Seg( QueryHist, dataImgHist, KBINS );
    
    for(int i=0; i< KBINS * 8 * 3; i++)
        dataImgHist[i].release();
    
    return dis;
    
}//End_cbir_seg()





int buildFan( Mat* SegHistMask, int WidthImg, int HeightImg ){
    
    for(int i=0; i<8; i++)
    {
        SegHistMask[i].create( HeightImg, WidthImg, CV_8UC1 );
        SegHistMask[i]( Rect(0,0, WidthImg, HeightImg) ) = Scalar::all(0);
    }
    
    int secNum;
    for(int iSeg=0; iSeg<8; iSeg++){
        for(int jj=0; jj<HeightImg; jj++ ){
            for(int ii=0; ii<WidthImg; ii++ ){
                secNum = Seg2_IsSection(ii, jj, WidthImg, HeightImg);
                
                SegHistMask[secNum].at<unsigned char>(jj,ii) = 255;
                
            }//END_For_ii
            
        }//END_For_jj
    }// END_For_iSeg
    
    return 1;
}

