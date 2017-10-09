


#include "fea_HAH.h"

void fea_HAH( Mat& Query_rgb_iChn, Mat hist_mask, Mat& hist_temp, int histSize_scalar, int histRange_scalar ){
    // hist ROI
    float range[]={0, histRange_scalar};
    const float* histRange = { range };
    bool uniform_hist = true; 
    bool accumulate_hist = false;

//    calcHist( &Query_rgb[iChn], 1, 0, hist_mask, hist_temp, 1, &histSize, &histRange, uniform_hist, accumulate_hist );
    calcHist( &Query_rgb_iChn, 1, 0, hist_mask, hist_temp, 1, &histSize_scalar, &histRange, uniform_hist, accumulate_hist );
}

/*
void fea_TEX_COOC( Mat& Query_rgb_iChn, Mat hist_mask, Mat& hist_temp, int histSize_scalar, int histRange_scalar ){

	cout<< "fea_TEX_COOC" << endl;	
}
*/


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void fea_TEX_COOC( Mat& Query_rgb_iChn, Mat hist_mask, Mat& hist_temp, int cooc_D, int cooc_L ){
    cout <<"-- fea_TEX_COOC: cooc_D="<< cooc_D<<",cooc_L="<< cooc_L<<" -- "<< endl;

    // 1. scaled image
    Mat im_SI = Mat( Query_rgb_iChn.size().height, Query_rgb_iChn.size().width, CV_8UC1, Scalar::all(0) );

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
    float im_range = ((float)im_max - (float)im_min)/cooc_L;
cout <<"im_range: "<< im_range << endl;


    // calculate im_SI
    for(int j=0; j<Query_rgb_iChn.size().height; j++){
	for(int i=0; i<Query_rgb_iChn.size().width; i++){
	    im_SI.at<unsigned char>(j,i) = ceil( ( (float)Query_rgb_iChn.at<unsigned char>(j,i) - im_min )/im_range );
	    if( Query_rgb_iChn.at<unsigned char>(j,i) == im_min ){
		im_SI.at<unsigned char>(j,i) = 1;
	    } else if( Query_rgb_iChn.at<unsigned char>(j,i) == im_max ){
		im_SI.at<unsigned char>(j,i) = cooc_L;
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
    Mat im_glcm[4];
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
  

  
    //// 3. glmc_props
    // 3-1
    for(int n=0; n<4; n++){
	for(int i=0; i<cooc_L; i++){
	    im_glcm[n].at<float>(0,i) = 0;
	    im_glcm[n].at<float>(i,0) = 0;
	}
    }

    // 3-2
    Mat im_glcm_f[4];
    for(int n=0; n<4; n++){
	im_glcm_f[n].create( cooc_L, cooc_L, CV_32FC1 );
    }


    int sum = 0;
    for(int n=0; n<4; n++){
	sum = 0;
	for(int j=0; j<cooc_L; j++){
	    for(int i=0; i<cooc_L; i++){
		sum += (int)im_glcm[n].at<float>(j,i);
	    }
	}
cout<<"sum: "<< sum << endl;

	for(int j=0; j<cooc_L; j++){
	    for(int i=0; i<cooc_L; i++){
		im_glcm_f[n].at<float>(j,i) = (int)im_glcm[n].at<float>(j,i)/(float)sum;
	    }
	}

    }


/*
cout <<"im_glcm_f[0]"<< endl;
for(int j=0; j<cooc_L; j++){
    for(int i=0; i<cooc_L; i++){
        printf( "%d, ", im_glcm_f[0].at<float>(j,i) );
    }
    cout << endl;
}
*/

    ////
    float im_contrast[4] = {0,0,0,0}, im_correlation[4]={0,0,0,0}, 
		im_energy[4] = {0,0,0,0}, im_homogeneity[4] = {0,0,0,0};

    float phi_i[4]={0,0,0,0}, phi_j[4]={0,0,0,0}, sigma_i[4]={0,0,0,0}, sigma_j[4]={0,0,0,0};
    for(int n=0; n<4; n++){
	for(int j=0; j<cooc_L; j++){
	    for(int i=0; i<cooc_L; i++){
		phi_i[n] += i*im_glcm_f[n].at<float>(j,i);
		phi_j[n] += j*im_glcm_f[n].at<float>(j,i);
	    }
	}

	for(int j=0; j<cooc_L; j++){
	    for(int i=0; i<cooc_L; i++){
		sigma_i[n] += pow((i-phi_i[n]),2)*im_glcm_f[n].at<float>(j,i);
		sigma_j[n] += pow((j-phi_j[n]),2)*im_glcm_f[n].at<float>(j,i);
	    }
	}
    }



    // 3-1 Contrast
    for(int n=0; n<4; n++){
	for(int j=0; j<cooc_L; j++){
	    for(int i=0; i<cooc_L; i++){
		im_contrast[n] += pow((j-i),2) * im_glcm_f[n].at<float>(j,i);
		im_energy[n] += pow( (im_glcm_f[n].at<float>(j,i)),2 );
		im_homogeneity[n] += im_glcm_f[n].at<float>(j,i)/( 1+abs(i-j) );

		im_correlation[n] += j*i*im_glcm_f[n].at<float>(j,i);	
	    }
	}
	im_correlation[n] = ( im_correlation[n] - phi_i[n]*phi_j[n] )/sqrt( sigma_i[n] )/sqrt( sigma_j[n] );
    }       

cout <<"im_contrast: "<< im_contrast[0]<<","<< im_contrast[1]<<","<< im_contrast[2]<<","<< im_contrast[3]<< endl;
cout <<"im_energy: "<< im_energy[0]<<","<< im_energy[1]<<","<< im_energy[2]<<","<< im_energy[3]<< endl;
cout <<"im_homogeneity: "<< im_homogeneity[0]<<","<< im_homogeneity[1]<<","<< im_homogeneity[2]<<","<< im_homogeneity[3]<< endl;

cout <<"im_corr: "<< im_correlation[0]<<","<< im_correlation[1]<<","<< im_correlation[2]<<","<< im_correlation[3]<< endl;
 

cout<<"fea_TEX_COOC start---" << endl;

    memcpy( hist_temp.data, im_contrast, 4*sizeof(float) );
    memcpy( hist_temp.data+4*sizeof(float), im_correlation, 4*sizeof(float) );
    memcpy( hist_temp.data+8*sizeof(float), im_energy, 4*sizeof(float) );
    memcpy( hist_temp.data+12*sizeof(float), im_homogeneity, 4*sizeof(float) );
/*
    for(int i=0; i<16; i++){
	cout << hist_temp.at<float>(i) << endl;
    }
*/


cout <<"fea_TEX_COOC end"<< endl;

}//END


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void fea_TEX_TFCM( Mat& Query_rgb_iChn, Mat& hist_mask, Mat& hist_temp, int histSize_scalar, int histRange_scalar ){

	cout<< "fea_TEX_TFCM" << endl;	
}



/*

void fea_TEX_TFCM( Mat& Query_rgb_iChn, Mat& hist_mask, Mat& hist_temp, int tfcm_D ){
    cout <<"fea_TEX_CSAC"<< endl;
imwrite("Query_rgb_iChn.png", Query_rgb_iChn);

    float t_aver = 0;
    int t_aver_num = 0;
    for(int j=0; j<hist_mask.size().height; j++){
	for(int i=0; i<hist_mask.size().width; i++){
	    if( hist_mask.at<unsigned char>(j,i)!=0 ){
		t_aver += (int)Query_rgb_iChn.at<unsigned char>(j,i);
		t_aver_num ++;
	    }
	}
    }
    float tolerance = t_aver/t_aver_num/10;
    cout <<"tolerance_" << tolerance << endl;
//    cout <<"tolerance2_" << tolerance2 << endl;


    // 0.
    int SV[4][4] = { 1,2,3,5, 2,7,11,13, 3,11,17,19, 5,13,19,23 };
    for(int j=0; j<4; j++){
	for(int i=0; i<4; i++){
	    cout << SV[j][i] <<" ";
	}
	cout << endl;
    }

    int u[10] = {1,2,3,5,7,11,13,17,19,23};

    // 1.
    int v[10][10] = {0};
    for(int i=0; i<10; i++){
	for(int j=i; j<10; j++){
	    v[i][j] = u[i]*u[j];
	    cout << v[i][j] <<" ";
	}
	cout << endl;
    } 

    vector<int> fv;
    vector<int> FV;
    for(int i=0; i<10; i++){
	for(int j=0; j<10; j++){
	    if( v[j][i] ){
		fv.push_back( 10*(i+1-1)+(j+1));
		FV.push_back( v[j][i] );
	    }
	}
    }
cout <<"fv_SIZE: "<< fv.size() << endl;

    for(int i=0; i<fv.size(); i++){
//	cout<<"fv: "<< fv[i] << endl;
//	cout<<"FV: "<< FV[i] << endl;
    }



    //// 2.
    int row, col;
    row = Query_rgb_iChn.size().height;
    col = Query_rgb_iChn.size().width;

    int PN = row*col;
    int* PixelNumber_ptr = new int[PN];

    int **TFN, **Elfa, **Beda;
    
    TFN = new int*[row];
    Elfa = new int*[row];
    Beda = new int*[row];
    for(int j=0; j<row; j++){
	TFN[j] = new int[col];
	Elfa[j] = new int[col];
	Beda[j] = new int[col];
    }

    for(int i=0; i<row; i++){
	for(int j=0; j<col; j++){
	    TFN[i][j] = 0;
	    Elfa[i][j] = 0;
	    Beda[i][j] = 0;
	}
    }


    int **Line1, **Line2, **Line3, **Line4;
    Line1 = new int*[row-1];
    Line2 = new int*[row-1];
    Line3 = new int*[row-1];
    Line4 = new int*[row-1];

    for(int j=0; j<row-1; j++){
	Line1[j] = new int[col-1];
	Line2[j] = new int[col-1];
	Line3[j] = new int[col-1];
	Line4[j] = new int[col-1];
    }

cout <<"tolerance="<< tolerance << endl;


    // 2.5
cout <<"Line1_start"<< endl;

//    for(int i=1; i<=1; i++){
//	for(int j=1; j<=1; j++){
    for(int i=1; i<row-1; i++){
	for(int j=1; j<col-1; j++){
	    Line1[i][j] = 0;
	    Line2[i][j] = 0;
	    Line3[i][j] = 0;
	    Line4[i][j] = 0;

/*
cout <<"-rroi[i-1][j]="<< (int)Query_rgb_iChn.at<unsigned char>(i-1,j) << endl;
cout <<"-rroi[i][j]="<< (int)Query_rgb_iChn.at<unsigned char>(i,j) << endl;
cout <<"-rroi[i+1][j]="<< (int)Query_rgb_iChn.at<unsigned char>(i+1,j) << endl;

cout <<"unsigned_sub: "<<
uint8_f( (int)Query_rgb_iChn.at<unsigned char>(i-1,j)-(int)Query_rgb_iChn.at<unsigned char>(i,j) )<< endl;


cout <<"abs(rroi[i-1][j]-rroi[i][j])<=tolerance : "<< 
	(abs( uint8_f( (int)Query_rgb_iChn.at<unsigned char>(i-1,j)-(int)Query_rgb_iChn.at<unsigned char>(i,j) ))<=tolerance) << endl;

cout <<"abs(rroi[i][j]-rroi[i+1][j])<=tolerance : "<< 
	( abs( uint8_f( Query_rgb_iChn.at<unsigned char>(i,j) - Query_rgb_iChn.at<unsigned char>(i+1,j) )) <= tolerance ) << endl;
	
cout <<"abs(rroi[i][j]-rroi[i+1][j])>=tolerance : "<< 
	( abs( uint8_f( Query_rgb_iChn.at<unsigned char>(i-1,j) - Query_rgb_iChn.at<unsigned char>(i,j)))>=tolerance ) << endl; 


cout <<"rule_1: "<<
	( (abs( uint8_f(Query_rgb_iChn.at<unsigned char>(i-1,j)- Query_rgb_iChn.at<unsigned char>(i,j)) ) <= tolerance) 
		& ( abs( uint8_f(Query_rgb_iChn.at<unsigned char>(i,j) - Query_rgb_iChn.at<unsigned char>(i+1,j))) <= tolerance ) ) << endl;

cout <<"rule_2: "<< 
	( ( (abs( uint8_f(Query_rgb_iChn.at<unsigned char>(i-1,j)- Query_rgb_iChn.at<unsigned char>(i,j)) ) <= tolerance) 
		    & ( abs( uint8_f(Query_rgb_iChn.at<unsigned char>(i,j) - Query_rgb_iChn.at<unsigned char>(i+1,j)) ) >= tolerance) ) | 
		( abs( uint8_f(Query_rgb_iChn.at<unsigned char>(i-1,j) - Query_rgb_iChn.at<unsigned char>(i,j)) )>=tolerance ) 
		    & ( abs( uint8_f(Query_rgb_iChn.at<unsigned char>(i,j) - Query_rgb_iChn.at<unsigned char>(i+1,j)) )<=tolerance ) ) << endl;

cout <<"rule_3: "<< 
	( ( ((Query_rgb_iChn.at<unsigned char>(i-1,j)- Query_rgb_iChn.at<unsigned char>(i,j) ) > tolerance) 
		    & ( ( Query_rgb_iChn.at<unsigned char>(i,j) - Query_rgb_iChn.at<unsigned char>(i+1,j) ) > tolerance) ) | 
		( ( Query_rgb_iChn.at<unsigned char>(i,j) - Query_rgb_iChn.at<unsigned char>(i-1,j) )>tolerance ) 
		    & ( ( Query_rgb_iChn.at<unsigned char>(i+1,j) - Query_rgb_iChn.at<unsigned char>(i,j) ) > tolerance ) ) << endl;

cout <<"rule_4: "<< 
	( ( (Query_rgb_iChn.at<unsigned char>(i-1,j)-Query_rgb_iChn.at<unsigned char>(i,j)>tolerance) 
		    & ( Query_rgb_iChn.at<unsigned char>(i+1,j)-Query_rgb_iChn.at<unsigned char>(i,j)>tolerance ) )|
		    ( ( Query_rgb_iChn.at<unsigned char>(i,j) - Query_rgb_iChn.at<unsigned char>(i-1,j)>tolerance  ) 
		    & ( Query_rgb_iChn.at<unsigned char>(i,j) - Query_rgb_iChn.at<unsigned char>(i+1,j)>tolerance ) )) << endl;
*/


/*	    //// -Line1
	    if( (abs( uint8_f(Query_rgb_iChn.at<unsigned char>(i-1,j)- Query_rgb_iChn.at<unsigned char>(i,j)) ) <= tolerance) 
		    && ( abs( uint8_f(Query_rgb_iChn.at<unsigned char>(i,j) - Query_rgb_iChn.at<unsigned char>(i+1,j)) ) <= tolerance ) ){
		Line1[i][j] = 1;
	    } 
	    else if( ( (abs( uint8_f(Query_rgb_iChn.at<unsigned char>(i-1,j)- Query_rgb_iChn.at<unsigned char>(i,j)) ) <= tolerance) 
		    && ( abs( uint8_f(Query_rgb_iChn.at<unsigned char>(i,j) - Query_rgb_iChn.at<unsigned char>(i+1,j)) ) >= tolerance) ) ||
		( abs( uint8_f(Query_rgb_iChn.at<unsigned char>(i-1,j) - Query_rgb_iChn.at<unsigned char>(i,j)) )>=tolerance ) 
		    && ( abs( uint8_f(Query_rgb_iChn.at<unsigned char>(i,j) - Query_rgb_iChn.at<unsigned char>(i+1,j)) )<=tolerance ) ){
	        Line1[i][j] = 2;
	    } 
	    else if( ( ( uint8_f(Query_rgb_iChn.at<unsigned char>(i-1,j)- Query_rgb_iChn.at<unsigned char>(i,j) ) > tolerance) 
		    && (  uint8_f( Query_rgb_iChn.at<unsigned char>(i,j) - Query_rgb_iChn.at<unsigned char>(i+1,j) ) > tolerance) ) ||
		( uint8_f( Query_rgb_iChn.at<unsigned char>(i,j) - Query_rgb_iChn.at<unsigned char>(i-1,j) )>tolerance ) 
		    && ( uint8_f( Query_rgb_iChn.at<unsigned char>(i+1,j) - Query_rgb_iChn.at<unsigned char>(i,j) ) > tolerance ) ){
		Line1[i][j] = 3;
	    } 
	    else if( ( uint8_f(Query_rgb_iChn.at<unsigned char>(i-1,j)-Query_rgb_iChn.at<unsigned char>(i,j)>tolerance) 
		    && uint8_f( Query_rgb_iChn.at<unsigned char>(i+1,j)-Query_rgb_iChn.at<unsigned char>(i,j)>tolerance ) )||
		    ( uint8_f( Query_rgb_iChn.at<unsigned char>(i,j) - Query_rgb_iChn.at<unsigned char>(i-1,j)>tolerance ) 
		    && uint8_f( Query_rgb_iChn.at<unsigned char>(i,j) - Query_rgb_iChn.at<unsigned char>(i+1,j)>tolerance ) )){
		Line1[i][j] = 4;	
	    }

	    //// -Line2
	    if( (abs( uint8_f(Query_rgb_iChn.at<unsigned char>(i,j-1)- Query_rgb_iChn.at<unsigned char>(i,j)) ) <= tolerance) 
		    && ( abs( uint8_f(Query_rgb_iChn.at<unsigned char>(i,j) - Query_rgb_iChn.at<unsigned char>(i,j+1)) ) <= tolerance ) ){
		Line2[i][j] = 1;
	    } 
	    else if( ( (abs( uint8_f(Query_rgb_iChn.at<unsigned char>(i,j-1)- Query_rgb_iChn.at<unsigned char>(i,j)) ) <= tolerance) 
		    && ( abs( uint8_f(Query_rgb_iChn.at<unsigned char>(i,j) - Query_rgb_iChn.at<unsigned char>(i,j+1)) ) >= tolerance) ) ||
		( abs( uint8_f(Query_rgb_iChn.at<unsigned char>(i,j-1) - Query_rgb_iChn.at<unsigned char>(i,j)) )>=tolerance ) 
		    && ( abs( uint8_f(Query_rgb_iChn.at<unsigned char>(i,j) - Query_rgb_iChn.at<unsigned char>(i,j+1)) )<=tolerance ) ){
	        Line2[i][j] = 2;
	    } 
	    else if( ( ( uint8_f(Query_rgb_iChn.at<unsigned char>(i,j-1)- Query_rgb_iChn.at<unsigned char>(i,j) ) > tolerance) 
		    && (  uint8_f( Query_rgb_iChn.at<unsigned char>(i,j) - Query_rgb_iChn.at<unsigned char>(i,j+1) ) > tolerance) ) ||
		( uint8_f( Query_rgb_iChn.at<unsigned char>(i,j) - Query_rgb_iChn.at<unsigned char>(i,j-1) )>tolerance ) 
		    && ( uint8_f( Query_rgb_iChn.at<unsigned char>(i,j+1) - Query_rgb_iChn.at<unsigned char>(i,j) ) > tolerance ) ){
		Line2[i][j] = 3;
	    } 
	    else if( ( uint8_f(Query_rgb_iChn.at<unsigned char>(i,j-1)-Query_rgb_iChn.at<unsigned char>(i,j)>tolerance) 
		    && uint8_f( Query_rgb_iChn.at<unsigned char>(i,j+1)-Query_rgb_iChn.at<unsigned char>(i,j)>tolerance ) )||
		    ( uint8_f( Query_rgb_iChn.at<unsigned char>(i,j) - Query_rgb_iChn.at<unsigned char>(i,j-1)>tolerance ) 
		    && uint8_f( Query_rgb_iChn.at<unsigned char>(i,j) - Query_rgb_iChn.at<unsigned char>(i,j+1)>tolerance ) )){
		Line2[i][j] = 4;	
	    }

	    //// -Line3
	    if( (abs( uint8_f(Query_rgb_iChn.at<unsigned char>(i-1,j-1)- Query_rgb_iChn.at<unsigned char>(i,j)) ) <= tolerance) 
		    && ( abs( uint8_f(Query_rgb_iChn.at<unsigned char>(i,j) - Query_rgb_iChn.at<unsigned char>(i+1,j+1)) ) <= tolerance ) ){
		Line3[i][j] = 1;
	    } 
	    else if( ( (abs( uint8_f(Query_rgb_iChn.at<unsigned char>(i-1,j-1)- Query_rgb_iChn.at<unsigned char>(i,j)) ) <= tolerance) 
		    && ( abs( uint8_f(Query_rgb_iChn.at<unsigned char>(i,j) - Query_rgb_iChn.at<unsigned char>(i+1,j+1)) ) >= tolerance) ) ||
		( abs( uint8_f(Query_rgb_iChn.at<unsigned char>(i-1,j-1) - Query_rgb_iChn.at<unsigned char>(i,j)) )>=tolerance ) 
		    && ( abs( uint8_f(Query_rgb_iChn.at<unsigned char>(i,j) - Query_rgb_iChn.at<unsigned char>(i+1,j+1)) )<=tolerance ) ){
	        Line3[i][j] = 2;
	    } 
	    else if( ( ( uint8_f(Query_rgb_iChn.at<unsigned char>(i-1,j-1)- Query_rgb_iChn.at<unsigned char>(i,j) ) > tolerance) 
		    && (  uint8_f( Query_rgb_iChn.at<unsigned char>(i,j) - Query_rgb_iChn.at<unsigned char>(i+1,j+1) ) > tolerance) ) ||
		( uint8_f( Query_rgb_iChn.at<unsigned char>(i,j) - Query_rgb_iChn.at<unsigned char>(i-1,j-1) )>tolerance ) 
		    && ( uint8_f( Query_rgb_iChn.at<unsigned char>(i+1,j+1) - Query_rgb_iChn.at<unsigned char>(i,j) ) > tolerance ) ){
		Line3[i][j] = 3;
	    } 
	    else if( ( uint8_f(Query_rgb_iChn.at<unsigned char>(i-1,j-1)-Query_rgb_iChn.at<unsigned char>(i,j)>tolerance) 
		    && uint8_f( Query_rgb_iChn.at<unsigned char>(i+1,j+1)-Query_rgb_iChn.at<unsigned char>(i,j)>tolerance ) )||
		    ( uint8_f( Query_rgb_iChn.at<unsigned char>(i,j) - Query_rgb_iChn.at<unsigned char>(i-1,j-1)>tolerance ) 
		    && uint8_f( Query_rgb_iChn.at<unsigned char>(i,j) - Query_rgb_iChn.at<unsigned char>(i+1,j+1)>tolerance ) )){
		Line3[i][j] = 4;	
	    }

	    //// -Line4
	    if( (abs( uint8_f(Query_rgb_iChn.at<unsigned char>(i-1,j+1)- Query_rgb_iChn.at<unsigned char>(i,j)) ) <= tolerance) 
		    && ( abs( uint8_f(Query_rgb_iChn.at<unsigned char>(i,j) - Query_rgb_iChn.at<unsigned char>(i+1,j-1)) ) <= tolerance ) ){
		Line4[i][j] = 1;
	    } 
	    else if( ( (abs( uint8_f(Query_rgb_iChn.at<unsigned char>(i-1,j+1)- Query_rgb_iChn.at<unsigned char>(i,j)) ) <= tolerance) 
		    && ( abs( uint8_f(Query_rgb_iChn.at<unsigned char>(i,j) - Query_rgb_iChn.at<unsigned char>(i+1,j-1)) ) >= tolerance) ) ||
		( abs( uint8_f(Query_rgb_iChn.at<unsigned char>(i-1,j+1) - Query_rgb_iChn.at<unsigned char>(i,j)) )>=tolerance ) 
		    && ( abs( uint8_f(Query_rgb_iChn.at<unsigned char>(i,j) - Query_rgb_iChn.at<unsigned char>(i+1,j-1)) )<=tolerance ) ){
	        Line4[i][j] = 2;
	    } 
	    else if( ( ( uint8_f(Query_rgb_iChn.at<unsigned char>(i-1,j+1)- Query_rgb_iChn.at<unsigned char>(i,j) ) > tolerance) 
		    && (  uint8_f( Query_rgb_iChn.at<unsigned char>(i,j) - Query_rgb_iChn.at<unsigned char>(i+1,j-1) ) > tolerance) ) ||
		( uint8_f( Query_rgb_iChn.at<unsigned char>(i,j) - Query_rgb_iChn.at<unsigned char>(i-1,j+1) )>tolerance ) 
		    && ( uint8_f( Query_rgb_iChn.at<unsigned char>(i+1,j-1) - Query_rgb_iChn.at<unsigned char>(i,j) ) > tolerance ) ){
		Line4[i][j] = 3;
	    } 
	    else if( ( uint8_f(Query_rgb_iChn.at<unsigned char>(i-1,j+1)-Query_rgb_iChn.at<unsigned char>(i,j)>tolerance) 
		    && uint8_f( Query_rgb_iChn.at<unsigned char>(i+1,j-1)-Query_rgb_iChn.at<unsigned char>(i,j)>tolerance ) )||
		    ( uint8_f( Query_rgb_iChn.at<unsigned char>(i,j) - Query_rgb_iChn.at<unsigned char>(i-1,j+1)>tolerance ) 
		    && uint8_f( Query_rgb_iChn.at<unsigned char>(i,j) - Query_rgb_iChn.at<unsigned char>(i+1,j-1)>tolerance ) )){
		Line4[i][j] = 4;	
	    }

	    // -2.8
	    Elfa[i][j] = SV[Line1[i][j]-1][Line2[i][j]-1];
	    Beda[i][j] = SV[Line3[i][j]-1][Line4[i][j]-1];
	    TFN[i][j] = Elfa[i][j] * Beda[i][j];

	    for(int p=0; p<fv.size(); p++){
		if(TFN[i][j] == 0)
		    TFN[i][j] = 0;
		else if(TFN[i][j] == FV[p])
		    TFN[i][j] = p+1;
	    }

	}//END_j
    }//END_i


cout <<"save Line1.txt" << endl;
FILE* fp_line1 = fopen("Line1.txt", "w+");
for(int i=0; i<row-1; i++){
    for(int j=0; j<col-1; j++){
	fprintf( fp_line1, "%d ", Line1[i][j] );
    }
    fprintf(fp_line1, "\n");
}
fclose(fp_line1);


cout <<"save Line2.txt" << endl;
FILE* fp_line2 = fopen("Line2.txt", "w+");
for(int i=0; i<row-1; i++){
    for(int j=0; j<col-1; j++){
	fprintf( fp_line2, "%d ", Line2[i][j] );
    }
    fprintf(fp_line2, "\n");
}
fclose(fp_line2);


cout <<"save Line3.txt" << endl;
FILE* fp_line3 = fopen("Line3.txt", "w+");
for(int i=0; i<row-1; i++){
    for(int j=0; j<col-1; j++){
	fprintf( fp_line3, "%d ", Line3[i][j] );
    }
    fprintf(fp_line3, "\n");
}
fclose(fp_line3);


cout <<"save Line4.txt" << endl;
FILE* fp_line4 = fopen("Line4.txt", "w+");
for(int i=0; i<row-1; i++){
    for(int j=0; j<col-1; j++){
	fprintf( fp_line4, "%d ", Line4[i][j] );
    }
    fprintf(fp_line4, "\n");
}
fclose(fp_line4);


cout <<"save TFN.txt" << endl;
FILE* fp_tfn = fopen("TFN.txt", "w+");
for(int i=0; i<row; i++){
    for(int j=0; j<col; j++){
	fprintf( fp_tfn, "%d ", TFN[i][j] );
    }
    fprintf(fp_tfn, "\n");
}
fclose(fp_tfn);


    ////
    int **TFN_d = new int*[row];
    for(int i=0; i<row; i++){
	TFN_d[i] = new int[col];

	memcpy( TFN_d[i], TFN[i], col*sizeof(int));
    }


cout <<"save TFN_d.txt" << endl;
FILE* fp_tfn_d = fopen("TFN_d.txt", "w+");
for(int i=0; i<row; i++){
    for(int j=0; j<col; j++){
	fprintf( fp_tfn_d, "%d ", TFN_d[i][j] );
    }
    fprintf(fp_tfn_d, "\n");
}
fclose(fp_tfn_d);

    int bin_min, bin_max;
    bin_min = 0; bin_max = 55;

    int* data = new int[row];
    float* hist = new float[bin_max-bin_min+1];


    // 
    int **nTFN = new int*[bin_max-bin_min+1];
    for(int i=0; i<bin_max-bin_min+1; i++){
	nTFN[i] = new int[col];
    }

    float *prob = new float[bin_max-bin_min+1];
    for(int i=0; i<bin_max-bin_min+1; i++){
	prob[i] = 0;
    }

    int *sTFN = new int[col];
    int sum;
    for(int j=0; j<col; j++){
	sum = 0;

        for(int i=0; i<row; i++){
	    data[i] = TFN_d[i][j];
        }

        hist_f( hist, data, row, bin_min, bin_max );

        for(int i=0; i<bin_max-bin_min+1; i++){
//	    cout <<"hist_"<< hist[i] << endl;
	
	    nTFN[i][j] = hist[i];
	    sum += hist[i];
        }
	sTFN[j] = sum;
    }

    float sum1=0, sum2=0;
    for(int i=0; i<bin_max-bin_min+1; i++){
	sum1=0; sum2=0;
	for(int j=0; j<col; j++){
	    sum1 += nTFN[i][j] * sTFN[j];
	    sum2 += sTFN[j] * sTFN[j];
	}

	prob[i] = sum1/(sum2+2e-16);
    }

/*
    for(int i=0; i<bin_max-bin_min+1; i++){
	cout <<"prob_"<< prob[i] << endl;
    }
*/ 

    //// 3. ///////////////////////////////////////////////////////////////////
/*    int L = 56;
    int **SI = new int*[row];
    for(int j=0; j<row; j++){
	SI[j] = new int[col];
    } 



    //// 3.2
    Mat TFN_mat = Mat( row, col, CV_8UC1, Scalar::all(0) );
    for(int j=0; j<row; j++){
	for(int i=0; i<col; i++){
	    TFN_mat.at<unsigned char>(j,i) = TFN[j][i];
	}
    }


    Mat im_SI = Mat( row, col, CV_8UC1, Scalar::all(0) );
    Mat im_glcm[4];
    glcm_f( im_SI, im_glcm, TFN_mat, L, tfcm_D );

cout <<"glcm SIZE: "<< im_glcm[0].size() << endl;

    //// 4. glcm_props
    for(int n=0; n<4; n++){
	for(int j=0; j<L; j++){
	    im_glcm[n].at<float>(0,j) = 0;
	    im_glcm[n].at<float>(j,0) = 0;
	}
    }


FILE* fp_glcm1 = fopen("glcm2_0.txt", "w+");
for(int j=0; j<L; j++){
    for(int i=0; i<L; i++){
	fprintf(fp_glcm1, "%d ", (int)im_glcm[0].at<float>(j,i));
    }
    fprintf( fp_glcm1, "\n" );
}
fclose(fp_glcm1);


fp_glcm1 = fopen("glcm2_1.txt", "w+");
for(int j=0; j<L; j++){
    for(int i=0; i<L; i++){
	fprintf(fp_glcm1, "%d ", (int)im_glcm[1].at<float>(j,i));
    }
    fprintf( fp_glcm1, "\n" );
}
fclose(fp_glcm1);



fp_glcm1 = fopen("glcm2_2.txt", "w+");
for(int j=0; j<L; j++){
    for(int i=0; i<L; i++){
	fprintf(fp_glcm1, "%d ", (int)im_glcm[2].at<float>(j,i));
    }
    fprintf( fp_glcm1, "\n" );
}
fclose(fp_glcm1);



fp_glcm1 = fopen("glcm2_3.txt", "w+");
for(int j=0; j<L; j++){
    for(int i=0; i<L; i++){
	fprintf(fp_glcm1, "%d ", (int)im_glcm[3].at<float>(j,i));
    }
    fprintf( fp_glcm1, "\n" );
}
fclose(fp_glcm1);


    //// 4. glcm_props
    int glcmsum[4];
    for(int n=0; n<4; n++){
	glcmsum[n] = 0;

	for(int j=0; j<L; j++){
	    for(int i=0; i<L; i++){
		glcmsum[n] += (int)im_glcm[n].at<float>(j,i);
	    }
	}
	printf("glcmsum_%d: %d\n", n, glcmsum[n] );
    }

    float** prob_glcm0 = new float*[L];
    float** prob_glcm45 = new float*[L];
    float** prob_glcm90 = new float*[L];
    float** prob_glcm135 = new float*[L];
    for(int i=0; i<L; i++){
	prob_glcm0[i] = new float[L];
	prob_glcm45[i] = new float[L];
	prob_glcm90[i] = new float[L];
	prob_glcm135[i] = new float[L];
    }

    for(int j=0; j<L; j++){
	for(int i=0; i<L; i++){
	    prob_glcm0[j][i]=im_glcm[0].at<float>(j,i) / glcmsum[0];
	    prob_glcm45[j][i]=im_glcm[1].at<float>(j,i) / glcmsum[1];
	    prob_glcm90[j][i]=im_glcm[2].at<float>(j,i) / glcmsum[2];
	    prob_glcm135[j][i]=im_glcm[3].at<float>(j,i) / glcmsum[3];
	}
    }


FILE* fp_glcm0 = fopen("glcm0.txt", "w+");
for(int j=0; j<L; j++){
    for(int i=0; i<L; i++){
	fprintf(fp_glcm0, "%f ", prob_glcm0[j][i] );
    }
    fprintf( fp_glcm0, "\n" );
}
fclose(fp_glcm0);


    //// 5. 
    // 5-1
    float Coarseness, Homogeneity;
    Coarseness = prob[56-1];
    Homogeneity = prob[1-1]; 

    // 5-2
    float aver = 0;
    float *mu, *delta;
    mu = new float[col];
    delta = new float[col];

    // mu
    for(int i=0; i<col; i++){
	aver = 0;	
	for(int j=0; j<row; j++){
	    aver += TFN_d[j][i];
	}
	aver = aver/row;
	mu[i] = aver;
    }

for(int i=0; i<col; i++){
//    cout <<"mu_"<< mu[i] << endl;
}

    // delta
    float std;
    for(int i=0; i<col; i++){
	std = 0;
	for(int j=0; j<row; j++){
	    std += pow( (TFN_d[j][i] - mu[i]), 2);
	}
	std = std / row;
	std = sqrt(std);

	delta[i] = std;
    }

for(int i=0; i<col; i++){
//    cout <<"delta_"<< delta[i] << endl;
}


    // 5-3
    float MC;
    float *VAR = new float[col];
    MC = 0;

    sum1 = 0, sum2 = 0;
    for(int i=0; i<col; i++){
	sum1 += mu[i] * delta[i];
	sum2 += delta[i] * delta[i];	
    }
    MC = sum1/sum2;
cout <<"MC="<< MC << endl;

    for(int i=0; i<col; i++){
	VAR[i] = pow(mu[i],2)*prob[0];
//	cout <<"VAR_"<< VAR[i] << endl;
    }

    float VAR_sum = 0;
    for(int i=0; i<col; i++){
	VAR_sum += VAR[i];
    }

    
    // 5-4
    float **CE0, **CE45, **CE90, **CE135;
    CE0 = new float*[L];
    CE45 = new float*[L];
    CE90 = new float*[L];
    CE135 = new float*[L];

    for(int i=0; i<L; i++){
	CE0[i] = new float[L];
	CE45[i] = new float[L];
	CE90[i] = new float[L];
	CE135[i] = new float[L];
    }


    vector<float> newCE0;
    vector<float> newCE45;
    vector<float> newCE90;
    vector<float> newCE135;
    for(int j=0; j<L; j++){
	for(int i=0; i<L; i++){
	    CE0[j][i] = prob_glcm0[j][i] * log(prob_glcm0[j][i]);
	    CE45[j][i] = prob_glcm45[j][i] * log(prob_glcm45[j][i]);
	    CE90[j][i] = prob_glcm90[j][i] * log(prob_glcm90[j][i]);
	    CE135[j][i] = prob_glcm135[j][i] * log(prob_glcm135[j][i]);

	    if( CE0[j][i]<1e8 | CE0[j][i]>1e-8 ){
		newCE0.push_back( CE0[j][i] );
	    }

	    if( CE45[j][i]<1e8 | CE45[j][i]>1e-8 ){
		newCE45.push_back( CE45[j][i] );
	    }

	    if( CE90[j][i]<1e8 | CE90[j][i]>1e-8 ){
		newCE90.push_back( CE90[j][i] );
	    }

	    if( CE135[j][i]<1e8 | CE135[j][i]>1e-8 ){
		newCE135.push_back( CE135[j][i] );
	    }

	}
    }
    cout <<"newCE0_SIZE: "<< newCE0.size() << endl;
    cout <<"newCE45_SIZE: "<< newCE45.size() << endl;
    cout <<"newCE90_SIZE: "<< newCE90.size() << endl;
    cout <<"newCE135_SIZE: "<< newCE135.size() << endl;

    float CE0_num = 0;
    float CE45_num = 0;
    float CE90_num = 0;
    float CE135_num = 0;
    for(int i=0; i<newCE0.size(); i++){
	CE0_num += newCE0[i];
    }

    for(int i=0; i<newCE45.size(); i++){
	CE45_num += newCE45[i];
    }


    for(int i=0; i<newCE90.size(); i++){
	CE90_num += newCE90[i];
    }

    for(int i=0; i<newCE135.size(); i++){
	CE135_num += newCE135[i];
    }

    CE0_num = -1*CE0_num;
    CE45_num = -1*CE45_num;
    CE90_num = -1*CE90_num;
    CE135_num = -1*CE135_num;

cout <<"CE0_num: "<< CE0_num << endl;
cout <<"CE45_num: "<< CE45_num << endl;
cout <<"CE90_num: "<< CE90_num << endl;
cout <<"CE135_num: "<< CE135_num << endl;


    // 5-5
    float CS0, CS45, CS90, CS135;
    CS0 = 0;  CS45 = 0;  CS90 =0; CS135 = 0;
    for(int i=0; i<L; i++){
	CS0 += pow( prob_glcm0[i][i], 2);
	CS45 += pow( prob_glcm45[i][i], 2);
	CS90 += pow( prob_glcm90[i][i], 2);
	CS135 += pow( prob_glcm135[i][i], 2);
    }
cout <<"CS0_"<< CS0 << endl;
cout <<"CS45_"<< CS45 << endl;
cout <<"CS90_"<< CS90 << endl;
cout <<"CS135_"<< CS135 << endl;

    

    //// 6. Store sum
    hist_temp.at<float>(0) = Coarseness;
    hist_temp.at<float>(1) = Homogeneity;

    hist_temp.at<float>(2) = MC;
    hist_temp.at<float>(3) = VAR_sum;

    hist_temp.at<float>(4) = CE0_num;
    hist_temp.at<float>(5) = CE45_num;
    hist_temp.at<float>(6) = CE90_num;
    hist_temp.at<float>(7) = CE135_num;

    hist_temp.at<float>(8) = CS0;
    hist_temp.at<float>(9) = CS45;
    hist_temp.at<float>(10) = CS90;
    hist_temp.at<float>(11) = CS135;

}//END


/*
void ScaledImage2_COOC_f( int** SI, int** TFN, int row, int col, int L ){
    cout <<"ScaledImage2_COOC_f" << endl;
    int TFN_max = 0;
    for(int i=0; i<row; i++){
	for(int j=0; j<col; j++){
	    if(TFN[i][j]>TFN_max){
		TFN_max = TFN[i][j];
	    }
	}
    }
cout<<"TFN_max: "<< TFN_max << endl;

    //
    for(int i=0; i<row; i++){
	for(int j=0; j<col; j++){
	    SI[i][j] = round( (double)TFN[i][j]/TFN_max*L );
	}
    }

}//END_SacledImage2_COOC_f
*/



void hist_f( float* hist, int* data, int data_num, int bin_min, int bin_max ){
//    cout<< "hist_f "<< endl;

    // 
    int bin_num = bin_max-bin_min+1;
    for(int i=0; i<bin_num; i++){
	hist[i] = 0;
    }

    //
    for(int i=0; i<data_num; i++){
	hist[data[i]]++;
    } 

}//END_hist_f()



int uint8_f(int i){
    if(i>=0)
	return i;
    else
	return 0;
}



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void fea_TEX_LBP( Mat& Query_rgb_iChn, Mat& hist_mask, Mat& hist_temp, int histSize_scalar, int histRange_scalar ){

	cout<< "fea_TEX_LBP" << endl;	
}


void fea_TEX_CSAC( Mat& Query_rgb_iChn, Mat& hist_mask, Mat& hist_temp, int histSize_scalar, int histRange_scalar ){

	cout<< "fea_TEX_CSAC" << endl;	
}












/*
int main( int argc, char** argv ){
    // Query Readin
    Mat QueryImg;
    QueryImg = imread( argv[1], 1 );


    if(!QueryImg.data){
	printf("Image read error!");
	return -1;
    }

    namedWindow("QueryImg", CV_WINDOW_AUTOSIZE );
    imshow("QueryImg", QueryImg );
    waitKey(0);
    cvDestroyWindow("QueryImg");

    // Query HAH
    // split into RGB planes
    vector<Mat> Query_rgb;
    split( QueryImg, Query_rgb );

    // cal hist
    Mat r_hist, g_hist, b_hist;
    int histSize = 256;
//    float range[]={0, 256};
//    const float* histRange = { range };
//    bool uniform_hist = true; 
//    bool accumulate_hist = false;
    
    Mat hist_mask = Query_rgb[0].clone();
    int rectLx, rectLy, rectRx, rectRy;

    /// cal QueryHistMatrix
    int WidthImg = QueryImg.size().width;
    int HeightImg = QueryImg.size().height;

    vector<Mat> QueryHist;
    vector<Mat>::iterator QueryHistIter = QueryHist.begin();
    
    int WidthBinX = floor( (WidthImg-1)/(2*KBINS) );
    int WidthBinY = floor( (HeightImg-1)/(2*KBINS) );
cout<<"WidthBin: "<<WidthBinX<<"; "<<WidthBinY<<endl;

    Mat hist_temp;
    rectLx=0; rectLy=0; rectRx=WidthImg; rectRy=HeightImg;
    cout<<"rect size: "<<rectLx <<";"<<rectLy<<"; "<<rectRx<<";"<<rectRy<<endl;   

    Mat hist_temp_kbin;
    hist_temp_kbin.create( 1, 3 * histSize, CV_32FC1 );
  

    float range[]={0, 256};
    const float* histRange = { range };
    bool uniform_hist = true; 
    bool accumulate_hist = false; 

    Mat QueryImgshow = QueryImg.clone();
 
    for( int iKbin=KBINS; iKbin>0; iKbin-- ){
	unsigned char* p_hist_temp_kbin = hist_temp_kbin.data;       
 
	// Mask ROI	
	hist_mask( Rect( 0, 0, WidthImg, HeightImg) )= Scalar::all(0); // clear mask
	hist_mask( Rect(rectLx, rectLy, (rectRx-rectLx), (rectRy-rectLy)) )= Scalar::all(1); // set outside bound


	if( iKbin != 1 ){    
	    rectLx += WidthBinX; rectLy += WidthBinY;
	    rectRx -= WidthBinX; rectRy -= WidthBinY;
	    hist_mask( Rect(rectLx, rectLy, (rectRx-rectLx), (rectRy-rectLy) )) = Scalar::all(0); // set inside bound	    	
	}

	for( int iChn=0; iChn<3; iChn++ ){
    	    hist_temp.create( 1, histSize, CV_32FC1 );

	    // hist ROI
//	    calcHist( &Query_rgb[iChn], 1, 0, hist_mask, hist_temp, 1, &histSize, &histRange, uniform_hist, accumulate_hist );
	    fea_HAH( Query_rgb[iChn], hist_mask, hist_temp, 256, 256 );

cout<< "hist sum: " << sum(hist_temp)[0] <<endl;

	    hist_temp = hist_temp/(sum(hist_temp)[0]); // Hist Nomalize

	    // Concatenate hist from Ch0-Ch2
//	    memcpy( p_hist_temp_kbin, hist_temp_kbin.data, sizeof(float) * histSize );
//	    QueryHistIter = QueryHist.insert( QueryHistIter, hist_temp ); // vector_num = 3 * Kbins
	    
	    QueryHist.push_back( hist_temp );   
//	    p_hist_temp_kbin += sizeof(float) * histSize; 

        }//END_For_iChn	
    }//END_For_iKbin    



    return 0;
}
*/




