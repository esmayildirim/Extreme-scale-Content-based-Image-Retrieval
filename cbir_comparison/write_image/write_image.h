#ifndef __CV__
#define __CV__
#include <opencv2/opencv.hpp>
#endif

#include "openslide.h"
#include "mpi.h"

//#include "../rough_searching_omp/tileCBIR_roughS_omp.h"
#include "../fine_searching_omp/tileCBIR_fineS_omp.h"
//#include "../cbir_common/sort_fun.h"
//#include "../cbir_common/mpi_gather_fs_results.h"

using namespace cv;




void write_image(char * file_name, int query_width, int query_height, int pid, int level_index);
