#ifndef __CV__
#define __CV__
#include <opencv2/opencv.hpp>
#endif

#include "openslide.h"

//#include "../rough_searching_omp/tileCBIR_roughS_omp.h"
#include "../fine_searching_omp/tileCBIR_fineS_omp.h"
//#include "../cbir_common/sort_fun.h"
//#include "../cbir_common/mpi_gather_fs_results.h"

using namespace cv;



double fine_searching_after_clustering(vector<result_distance_t> &clustered_results,
                                     int level_index,
                                     unsigned char * queryBuf_ptr,
                                     int query_imH,
                                     int query_imW,
                                     float OLP,
                                     int KBINS,
                                     int Index_FEA,
                                     vector<result_distance_t>&final_vector,
                                       int64_t ***size_width_height);
