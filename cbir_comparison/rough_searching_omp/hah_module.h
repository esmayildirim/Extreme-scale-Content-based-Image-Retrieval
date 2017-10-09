
// hah_module.h

#include "../cbir_common/fea_HAH.h"
// #include "fea_TEX_COOC.h"
// #include "fea_TEX_CSAC.h"
// #include "fea_TEX_TFCM.h"
// #include "fea_TEX_LBP.h"


/*  hah_module()
 *  This function generate a concatinated feature vector from the image patch around the given point of 
 *    interest (iPatX, iPatY).
 *   The output feature vector is stored in output variable QueryHist.
 *   The input image is passed through input variable QueryImg.
 *   Other input variables are KBINS, scalar_histSize, scalar_histRange, iPatX, iPatY, WidthImg and HeighImg.
 */


void hah_module( Mat& QueryImg, Mat * QueryHist, int KBINS, int scalar_histSize, int scalar_histRange, int iPatX, int iPatY, int WidthImg, int HeightImg, int Index_FEA );
