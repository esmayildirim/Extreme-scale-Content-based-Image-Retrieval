// hah_module_tex.cpp 
// 3-31-2016 AE. 

#include "hah_module_tex.h" 



void hah_module_tex( Mat& QueryImg,
                    float* queryHist_arr_1d,
                    int KBINS,
                    int scalar_histSize,
                    int scalar_histRange,
                    int iPatX,
                    int iPatY,
                    int WidthImg,
                    int HeightImg,
                    int Index_FEA,
                    int D,
                    int L )
{
  
	Mat Query_rgb[3];
	for (int ic = 0; ic < 3; ic++){
		Query_rgb[ic].create( QueryImg.size().width, QueryImg.size().height, CV_8UC1 );
	}
	split( QueryImg, Query_rgb );

    int histSize = scalar_histSize;
    float range[]={0, scalar_histRange};

    const float* histRange = { range };
    bool uniform_hist = true; 
    bool accumulate_hist = false;
    
    Mat hist_mask = Query_rgb[0].clone();
    int rectLx, rectLy, rectRx, rectRy;

    int WidthBinX = floor( (WidthImg-1)/(2*KBINS) );
    int WidthBinY = floor( (HeightImg-1)/(2*KBINS) );

    rectLx = iPatX; rectLy = iPatY;
	rectRx = iPatX + WidthImg; rectRy = iPatY + HeightImg;

	char* imfN_str = new char[100]; 
    for( int iKbin=KBINS; iKbin>0; iKbin-- ){

		hist_mask( Rect( 0, 0, QueryImg.size().width, QueryImg.size().height ) )= Scalar::all(0); // clear mask
		hist_mask( Rect(rectLx, rectLy, (rectRx-rectLx), (rectRy-rectLy)) )= Scalar::all(1); // set outside bound

		if( iKbin != 1 ){    
			rectLx += WidthBinX; rectLy += WidthBinY;
			rectRx -= WidthBinX; rectRy -= WidthBinY;
			hist_mask( Rect(rectLx, rectLy, (rectRx-rectLx), (rectRy-rectLy) )) = Scalar::all(0); // set inside bound	    	
		}

		// 
		sprintf(imfN_str, "imK_%d.png", iKbin);
		Mat imK_temp; 
		QueryImg.copyTo( imK_temp, hist_mask ); 
 
		Mat imK_rgb[3]; 
		split( imK_temp, imK_rgb );  
		// 

		for( int iChn=0; iChn<3; iChn++ ){ 
			Mat hist_temp; 
    	    hist_temp.create( 1, histSize, CV_32FC1 );
    	    
    	    switch (Index_FEA){
    	    	case 1:
    	    		fea_TEX_COOC( imK_rgb[iChn], hist_mask, hist_temp, D, L );
    	    		break;
    	    	case 2:
    	    		fea_TEX_CSAC( Query_rgb[iChn], hist_mask, hist_temp, 256, 256 );
    	    		break;
    	    	case 3:
                    fea_TEX_TFCM( imK_rgb[iChn], hist_mask, hist_temp, D ); 
    	    		break;
    	    	case 4:
    	    		fea_TEX_LBP( Query_rgb[iChn], hist_mask, hist_temp, 256, 256 );
    	    		break;
    	    	default:
    	    		fea_HAH( Query_rgb[iChn], hist_mask, hist_temp, 256, 256 );     	    
    	    }//END_SWITCH 



			cout <<" [] sum(hist_temp)[0] = "<< sum(hist_temp)[0] << endl; 
			hist_temp = hist_temp/(sum(hist_temp)[0]); // Hist Nomalize	

			cout<<" --> "; 
			for(int ie=0; ie<16; ie++){
				queryHist_arr_1d[(3*(KBINS-iKbin)+iChn)*L+ie] = hist_temp.at<float>(ie); 
				cout<< hist_temp.at<float>(ie) << ", "; 
			}
			cout << endl; 
        }//END_For_iChn	
    }//END_For_iKbin  

}//END hah_module_tex()



void hah_module_tex_fan( Mat& QueryImg, vector<Mat>& QueryHist, int KBINS, int scalar_histSize, int scalar_histRange, int iPatX, int iPatY, 
						int WidthImg, int HeightImg, int Index_FEA, int D, int L ){
	; 
}//END hah_module_tex_fan() 














