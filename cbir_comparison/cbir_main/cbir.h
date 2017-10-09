#ifndef __CV__
#define __CV__
#include <opencv2/opencv.hpp>
#endif


#include "../rough_searching_omp/tileCBIR_roughS_omp.h"
#include "../fine_searching_omp/tileCBIR_fineS_omp.h"
#include "../clustering/tileCBIR_cluster_omp.h"
#include "../final_fine_searching/final_fs.h"
#include "../cbir_common/sort_fun.h"
#include "../cbir_common/mpi_sort_fs_results.h"

#define CLUSTERING_MEMORY_SIZE 104857600

using namespace cv;

int cbir_rs_fs_workflow(vector<result_distance_t>& disPatch_v,
                  unsigned char* tileBuf_ptr,
                  int tile_imH,
                  int tile_imW,
                  unsigned char* queryBuf_ptr,
                  int query_imH,
                  int query_imW,
                  int tile_ID,
                  float OLP,
                  int KBINS,
                  int Index_FEA,
                  char * file_name,
                  long tile_x,
                  long tile_y,
                  int pid,
                  int num_processes);

double cbir_cluster_fs_workflow(long long start_offset,
                             long long end_offset,
                             long long num_tiles,
                             char * file_path,
                             float Beta,
                             int level_index,
                             unsigned char *query_image_buffer,
                             int query_image_height,
                             int query_image_width,
                             float OLP,
                             int KBINS,
                             int FEA_INDEX,
                             char * dest_folder,
                             int pid,
                             int64_t ***size_width_height
                             );

double cbir_fs_after_rs_workflow(long long start_offset,
                              long long end_offset,
                              long long num_tiles,
                              char * file_path,
                              int level_index,
                              Mat QueryImg,
                              int query_image_height,
                              int query_image_width,
                              float OLP,
                              int KBINS,
                              int FEA_INDEX,
                              char * dest_folder,
                              int pid,
                              int64_t *** size_width_height
                              );

