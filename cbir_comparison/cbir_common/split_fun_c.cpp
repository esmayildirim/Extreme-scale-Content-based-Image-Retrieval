
#include "split_fun_c.h"

void split_fun_c( Mat &QueryImg, Mat* Image_rgb ){

    IplImage Ipl_QueryImg = IplImage(QueryImg);

	IplImage *Ipl_r, *Ipl_g, *Ipl_b;
	Ipl_r = cvCreateImage( cvSize(Ipl_QueryImg.width, Ipl_QueryImg.height), IPL_DEPTH_8U, 1 );
	Ipl_g = cvCreateImage( cvSize(Ipl_QueryImg.width, Ipl_QueryImg.height), IPL_DEPTH_8U, 1 );
	Ipl_b = cvCreateImage( cvSize(Ipl_QueryImg.width, Ipl_QueryImg.height), IPL_DEPTH_8U, 1 );

    
	cvSetImageCOI( &Ipl_QueryImg, 1);
	cvCopy( &Ipl_QueryImg, Ipl_r ); //
	cvSetImageCOI( &Ipl_QueryImg, 2);
	cvCopy( &Ipl_QueryImg, Ipl_g ); //
	cvSetImageCOI( &Ipl_QueryImg, 3); 
	cvCopy( &Ipl_QueryImg, Ipl_b ); //
	
	Mat r = cvarrToMat(Ipl_r);
	Mat g = cvarrToMat(Ipl_g);
	Mat b = cvarrToMat(Ipl_b);

	r.copyTo(Image_rgb[0]);
	g.copyTo(Image_rgb[1]);
	b.copyTo(Image_rgb[2]);

	cvReleaseImage(&Ipl_r);
	cvReleaseImage(&Ipl_g);
	cvReleaseImage(&Ipl_b);
  
}
