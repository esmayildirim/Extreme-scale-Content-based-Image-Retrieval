
#include "glcm_f.h"

void glcm_f( Mat& im_SI, Mat* im_glcm, Mat& Query_rgb_iChn, int cooc_L, int cooc_D ){

    // 1. scaled image
////    Mat im_SI = Mat( Query_rgb_iChn.size().height, Query_rgb_iChn.size().width, CV_8UC1, Scalar::all(0) );

    int im_min=0, im_max=0;
    for(int j=0; j<Query_rgb_iChn.size().height; j++){
	for(int i=0; i<Query_rgb_iChn.size().width; i++){
	    if( Query_rgb_iChn.at<unsigned char>(j,i) < im_min ){
		im_min = Query_rgb_iChn.at<unsigned char>(j,i);
	    }

	    if( Query_rgb_iChn.at<unsigned char>(j,i) > im_max ){
		im_max = Query_rgb_iChn.at<unsigned char>(j,i);
	    }
	}
    }
cout <<"im_min: "<< im_min<< endl;
cout <<"im_max: "<< im_max<< endl;

    //
    double im_range = ((double)im_max - (double)im_min)/cooc_L;
    cout <<"im_range: "<< im_range << endl;


    // calculate im_SI
    for(int j=0; j<Query_rgb_iChn.size().height; j++){
	for(int i=0; i<Query_rgb_iChn.size().width; i++){
	    im_SI.at<unsigned char>(j,i) = ceil( ( (double)Query_rgb_iChn.at<unsigned char>(j,i) - im_min )/im_range );
	    if( Query_rgb_iChn.at<unsigned char>(j,i) == im_min ){
		im_SI.at<unsigned char>(j,i) = 1;
	    } else if( Query_rgb_iChn.at<unsigned char>(j,i) == im_max ){
		im_SI.at<unsigned char>(j,i) = cooc_L;
	    }

	    if(im_SI.at<unsigned char>(j,i)==28){
		im_SI.at<unsigned char>(j,i) = 29;
	    } 
	}	
    }


/*
FILE *fp_imSI = fopen("imSI.txt", "w+");
for(int j=0; j<im_SI.size().height; j++){
    for(int i=0; i<im_SI.size().width; i++){
	fprintf( fp_imSI, "%d ", im_SI.at<unsigned char>(j,i) );
    }
    fprintf( fp_imSI, "\n" );
}
fclose(fp_imSI);
*/

    // 2. glcm
////    Mat im_glcm[4];
    for(int n=0; n<4; n++){
	im_glcm[n].create( cooc_L, cooc_L, CV_32FC1 );
    }


    for(int n=0; n<4; n++){
        for(int j=0; j<im_glcm[n].size().height; j++){
	    for(int i=0; i<im_glcm[n].size().width; i++){
		im_glcm[n].at<float>(j,i)=0;
	    }
	}
    }

/*
cout << "im_glcm[0].(0,0)_org:" << (int)im_glcm[0].at<unsigned char>(0,0) << endl;
im_glcm[0].at<unsigned char>(0,0) = im_glcm[0].at<unsigned char>(0,0) + 1;
cout << "im_glcm[0].(0,0)_after:" << (int)im_glcm[0].at<unsigned char>(0,0) << endl;
*/


    int I, J;
    // -1 [0,D]
    for(int j=0; j<Query_rgb_iChn.size().height; j++){
	for(int i=0; i<Query_rgb_iChn.size().width - cooc_D; i++){
	    I = (int)im_SI.at<unsigned char>(j,i);
	    J = (int)im_SI.at<unsigned char>(j,i+cooc_D);
//	    cout <<"I/J: "<< I << ", "<< J << endl;

	    im_glcm[0].at<float>(I-1,J-1) = (int)im_glcm[0].at<float>(I-1,J-1) + 1; 
	}
    }

/*
cout <<"im_glcm[0]: "<< endl;
for(int j=0; j<cooc_L; j++){
    for(int i=0; i<cooc_L; i++){
        printf( "%d, ", (int)im_glcm[0].at<float>(j,i) );
    }
    cout << endl;
}
*/


    // -2 [-D,D]
    for(int j=cooc_D; j<Query_rgb_iChn.size().height; j++){
	for(int i=0; i<Query_rgb_iChn.size().width-cooc_D; i++){
	    I = (int)im_SI.at<unsigned char>(j,i);
	    J = (int)im_SI.at<unsigned char>(j-cooc_D, i+cooc_D);

	    im_glcm[1].at<float>(I-1, J-1) = im_glcm[1].at<float>(I-1, J-1) + 1;
	}
    }


/*
cout <<"im_glcm[1]"<< endl;
for(int j=0; j<cooc_L; j++){
    for(int i=0; i<cooc_L; i++){
        printf( "%d, ", im_glcm[1].at<unsigned char>(j,i) );
    }
    cout << endl;
}
*/


    // -3 [-D,0]
    for(int j=cooc_D; j<Query_rgb_iChn.size().height; j++){
	for(int i=0; i<Query_rgb_iChn.size().width; i++){
	    I = (int)im_SI.at<unsigned char>(j,i);
	    J = (int)im_SI.at<unsigned char>(j-cooc_D, i);

	    im_glcm[2].at<float>(I-1, J-1) = im_glcm[2].at<float>(I-1, J-1) + 1;
	}
    }


/*
cout <<"im_glcm[2]"<< endl;
for(int j=0; j<cooc_L; j++){
    for(int i=0; i<cooc_L; i++){
        printf( "%d, ", im_glcm[2].at<unsigned char>(j,i) );
    }
    cout << endl;
}
*/


    // -4 [-D,-D]
    for(int j=cooc_D; j<Query_rgb_iChn.size().height; j++){
	for(int i=cooc_D; i<Query_rgb_iChn.size().width; i++){
	    I = (int)im_SI.at<unsigned char>(j,i);
	        J = (int)im_SI.at<unsigned char>(j-cooc_D, i-cooc_D);

	    im_glcm[3].at<float>(I-1, J-1) = im_glcm[3].at<float>(I-1,J-1) + 1;
	}
    }

/*
cout <<"im_glcm[3]"<< endl;
for(int j=0; j<cooc_L; j++){
    for(int i=0; i<cooc_L; i++){
        printf( "%d, ", im_glcm[3].at<unsigned char>(j,i) );
    }
    cout << endl;
}
*/

}//END



